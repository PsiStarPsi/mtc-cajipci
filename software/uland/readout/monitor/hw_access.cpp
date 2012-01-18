#include "hw_access.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <poll.h>


void altix_wraper::init(console *a)
{
    int num_cards;
    int error;
    card_node * node;
    con = a;
    flow_ctl = true;
    rst_ctl = true;
    flush_ctl = true;
    flowpacket = (char*)malloc(altix_packet_size());
    QFile packet(":/packets/command-packet-for-flow-control.20110718.packet");
    packet.open(QIODevice::ReadOnly);
    packet.read(flowpacket, altix_packet_size());
    packet.close();
    this->io_lock = new QReadWriteLock();
    num_cards = altix_initialize((char*)DEVICE_NAME);
    if(num_cards < 0)
    {
        con->add_log("Initialization failed! altix_initialize returned " +QString::number(num_cards));
    }
    fd = ::open(DEVICE_NAME, O_RDWR);
    card_infos = altix_get_cards(fd, &error);
    con->add_log("Found " + QString::number(num_cards) + " PCI cards");
    for(int i = 0; i< num_cards; i++)
    {
        node = (card_node*)malloc(sizeof(card_node));
        node->id = card_infos[i].id;
        node->channels = (channel_node*) malloc(sizeof(channel_node)*4);
        for(int j =0; j< 4; j++)
        {
            node->channels[j].data = (int*)malloc(sizeof(char)*altix_event_size());
            node->channels[j].checksum_errors = 0;
            node->channels[j].enabled = false;
            node->channels[j].lock = new QReadWriteLock();
            node->channels[j].pollok = false;
            node->channels[j].meta = new QList <int>();
        }
        cards.insert(node->id, node);
    }
    foreach(card_node* node, this->cards.values())
    {
        altix_pci_card_info * card_info_ptr = card_infos;
        for(int j = 0; j< cards.size(); j++)
        {
            if(card_info_ptr->id == node->id)
                break;
            else
                card_info_ptr++;
        }
        node->card = new altix_card();
        QObject::connect(node->card, SIGNAL(send_log(QString)), this, SIGNAL(send_log(QString)), Qt::QueuedConnection);
        node->card->init(card_info_ptr);
        node->card->lock_card();
        if(node->card->islocked())
        {
            for(int chan = 0; chan < 4; chan++)
            {
                node->card->enable_channel(chan, false);
            }
        }
    }
    running = false;
    outf = NULL;
    inited = true;
    bad_pci = 0;
    bad_dma = 0;
    overflow_dog = 0;
    aurora_errors = 0;
    bad_events = 0;
    total_events = 0;
}

void altix_wraper::deinit()
{
    if(this->isRunning())
        this->stop_hardware_loop();
    this->io_lock->lockForWrite();
    foreach(card_node* node, this->cards.values())
    {
        for(int chan = 0; chan < 4; chan++)
        {
            delete node->channels[chan].lock;
            delete node->channels[chan].meta;
            free(node->channels[chan].data);
        }
        node->card->unlock_card();
        delete node->card;
        free(node->channels);
        free(node);
    }
    ::close(fd);
    if(outf !=NULL)
    {
        outf->close();
        delete outf;
        outf = NULL;
    }
    inited = false;
    bad_pci = 0;
    bad_dma = 0;
    overflow_dog = 0;
    aurora_errors = 0;
    bad_events = 0;
    total_events = 0;
    this->io_lock->unlock();
}

int altix_wraper::driver_version()
{
    return altix_get_kernel_version(fd);
}

int altix_wraper::library_version()
{
    return ALTIX_DRIVER_VERSION;
}

int altix_wraper::event_size()
{
    return altix_event_size();
}

int altix_wraper::packet_size()
{
    return altix_packet_size();
}

void altix_wraper::set_flow_ctl(bool state)
{
    this->flow_ctl = state;
}
void altix_wraper::set_rst_ctl(bool state)
{
    this->rst_ctl = state;
}

void altix_wraper::set_flush_ctl(bool state)
{
    this->flush_ctl = state;
}

void altix_wraper::set_store_meta(bool state)
{
    this->store_meta = state;
}

void altix_wraper::set_veto(bool state)
{
    this->veto_ctl = state;
}

const altix_pci_card_stat * altix_wraper::get_stats()
{
    int result;
    if(cards.size() > 0)
        return altix_get_stats(fd, &result);
    else
        return NULL;
}

int altix_wraper::get_num_cards()
{
    return cards.size();
}

int altix_wraper::read_packet(void *buff, int card_id, int chan)
{
    if(!cards.contains(card_id))
    {
        return -1;
    }
    memcpy(buff, cards[card_id]->channels[chan].data, altix_event_size());
    return altix_event_size();
}

bool altix_wraper::is_channel_enabled(int card_id, int chan)
{
    if(!cards.contains(card_id))
    {
        return false;
    }
    if(this->isRunning())
    {
        if(cards[card_id]->card->islocked())
        {
            return (cards[card_id]->transfer_status) & (1 << (chan + 8));
        }
        else
            return false;
    }
    else
    {
        if(cards[card_id]->card->islocked())
        {
            if(this->io_lock->tryLockForWrite())
            {
                cards[card_id]->transfer_status = cards[card_id]->card->last_io_status();
                this->io_lock->unlock();
            }
            return (cards[card_id]->transfer_status) & (1 << (chan + 8));
        }
        else
            return false;
    }
    return false;
}

bool altix_wraper::set_name(QString fname)
{
    if(outf == NULL)
    {
        outf = new QFile(fname);
    }
    else
    {
        if(outf->isOpen())
        {
            outf->close();
        }
    }
    if(fname == "")
    {
        emit send_log("No output File");
        return false;
    }
    if(outf->exists())
    {
        QDateTime time = QDateTime::currentDateTime();
        QString newname = fname + time.toString("_hh:mm:ss.dd.MM.yyyy");
        outf->rename(newname);
        emit send_log("Renaming old data file to: " + newname);
        outf->setFileName(fname);
    }
    outf->setFileName(fname);
    if(!outf->open(QIODevice::WriteOnly))
    {
        emit send_log("Could not open the output file  " + fname);
        return false;
    }
    emit send_log("Selected output file " + outf->fileName() + " for raw data blocks");
    return true;
}

void altix_wraper::set_channel_status(int card_id, int chan, bool on)
{
    if(!cards.contains(card_id))
    {
        return;
    }
    if((chan > 3) | (chan <0))
    {
        return;
    }
    cards[card_id]->channels[chan].enabled = on;
    this->io_lock->lockForWrite();
    cards[card_id]->card->enable_channel(chan, on);
    this->io_lock->unlock();
}

int altix_wraper::get_checksum_errors(int card, int chan)
{
    card_node* node;
    if(!cards.contains(card))
    {
        return -1;
    }
    if((chan > 3) | (chan <0))
    {
        return -2;
    }
    node = cards[card];
    return node->channels[chan].checksum_errors;
}

void altix_wraper::start_hardware_loop()
{
    this->io_lock->lockForWrite();
    if(this->isRunning())
        return;
    foreach(card_node* node, this->cards.values())
    {
        node->card->unlock_card();
    }
    emit send_log("Starting Readout loop");
    this->start();
}

void altix_wraper::stop_hardware_loop()
{
    this->io_lock->lockForWrite();
    running = false;
    this->wait(1000);
    if(this->isRunning())
        this->terminate_flag = true;
    this->wait();
    foreach(card_node* node, this->cards.values())
    {
        node->card->lock_card();
    }
    this->io_lock->unlock();
}

void altix_wraper::init_tread()
{
    this->terminate_flag = false;
    this->setTerminationEnabled(false);
    foreach(card_node* node, this->cards.values())
    {
        node->card->lock_card();
        for(int chan = 0; chan<4; chan++)
        {
            node->channels[chan].bytes_left = altix_event_size();
            node->channels[chan].pollok = false;
            node->card->enable_channel(chan, 0);
            node->card->enable_channel(chan, 1);
            node->card->enable_channel(chan ,node->channels[chan].enabled);
            node->channels[chan].meta->clear();
        }
    }
    this->io_lock->unlock();
}

void altix_wraper::deinit_thread()
{
    foreach(const card_node* node, this->cards.values())
    {
        if(node->card->islocked())
        {
            node->card->unlock_card();
        }
    }
    emit send_log("Readout loop done");
}

bool altix_wraper::read_out_loop_done()
{
    if(this->terminate_flag)
    {
        emit send_log("Readout loop did not finish, the last event not fully recovered");
        return true;
    }
    foreach(const card_node* p, this->cards.values())
    {
        if(p->card->islocked())
        {
            for(int chan = 0; chan < 4; chan++)
            {
                if(p->channels[chan].enabled)
                {
                    if(p->channels[chan].bytes_left > 0)
                    {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

void altix_wraper::update_poll()
{
    foreach(const card_node* node, this->cards.values())
    {
        if(node->card->islocked())
        {
            for(int chan = 0; chan< 4; chan++)
            {
                if(node->channels[chan].enabled && (node->channels[chan].bytes_left != 0))
                {
                    node->channels[chan].pollok = node->card->poll_device(chan);
                }
                else
                {
                    node->channels[chan].pollok = false;
                }
            }
        }
    }
}

void altix_wraper::single_readout()
{
    foreach(card_node* node, this->cards.values())
    {
        for(int chan = 0; chan< 4; chan++)
        {
            if(node->channels[chan].pollok)
            {
                node->channels[chan].pollok = false;
                node->channels[chan].lock->lockForWrite();
                int error = node->card->read_chunk(((char*)node->channels[chan].data) + altix_event_size() - node->channels[chan].bytes_left, chan , node->channels[chan].bytes_left);
                node->transfer_status = node->card->last_io_status();
                node->channels[chan].lock->unlock();
                node->channels[chan].meta->push_back(node->transfer_status);
                this->process_meta(node->transfer_status,chan);
                if(error < 0)
                {
                    node->channels[chan].bytes_left = 0;
                    emit send_log("Error on reading card" + QString::number(node->id) + " Channel " + QString::number(chan) + ". System call failed");
                    node->card->enable_channel(chan, 0);
                    node->card->enable_channel(chan, 1);
                }
                else
                {
                    node->channels[chan].bytes_left -= error;
                }
            }
        }

    }
}

void altix_wraper::save_and_process()
{
    int error;
    foreach(const card_node* node, this->cards.values())
    {
        if(node->card->islocked())
        {
            for(int chan = 0; chan < 4; chan++)
            {
                if(node->channels[chan].enabled)
                {
                    this->total_events++;
                    error = altix_card::checksum_packet(node->channels[chan].data);
                    node->channels[chan].checksum_errors += error;
                    if(error)
                    {
                        this->bad_events++;
                        if(this->rst_ctl)
                        {
                            node->card->enable_channel(chan, 0);
                            node->card->enable_channel(chan, 1);
                        }
                    }
                    if(outf != NULL && outf->isOpen())
                    {
                        uint meta_header = (node->id << 16) | (chan);
                        outf->write((char*)&meta_header, sizeof(int));
                        if(this->store_meta)
                        {
                            int meta_size = node->channels[chan].meta->size();
                            outf->write((char*)&meta_size, sizeof(int));
                            for(int m = 0; m < meta_size; m++)
                            {
                                int metadata = node->channels[chan].meta->at(m);
                                outf->write((char*)&metadata, sizeof(int));
                            }
                        }
                        else
                        {
                            int zero = 0;
                            outf->write((char*)&zero, sizeof(int));
                        }
                        outf->write((char*)node->channels[chan].data, altix_event_size());
                    }
                    node->channels[chan].bytes_left = altix_event_size();
                    if(this->flow_ctl)
                    {
                        node->card->send_command(this->flowpacket, chan);
                    }
                    node->channels[chan].meta->clear();
                }
            }
            if(this->veto_ctl)
            {
                node->card->clear_veto();
            }
        }
    }
    if(outf != NULL && outf->isOpen() && flush_ctl)
        outf->flush();
}

void altix_wraper::run()
{
    this->init_tread();
    if(!running && inited)
    {
        running = true;
        while(running)
        {
            while(!read_out_loop_done())
            {
                this->update_poll();
                this->single_readout();
            }
            this->save_and_process();
        }
        this->deinit_thread();
        return;
    }
    else return;
}

void altix_wraper::process_meta(int meta, int chan)
{
    if(meta & (1<< chan))
    {
        this->bad_pci++;
    }
    if(meta & ((1 << chan) << 16))
    {
        this->aurora_errors++;
    }
    if(meta & (1 << 7))
    {
        this->overflow_dog++;
    }
    if(meta & (1 << 4))
    {
        this->bad_dma++;
    }
}

void altix_wraper::reset_stats()
{
    bad_pci = 0;
    bad_dma = 0;
    overflow_dog = 0;
    aurora_errors = 0;
    bad_events = 0;
    total_events = 0;
    foreach(const card_node* node, this->cards.values())
    {
        for(int chan = 0; chan < 4; chan++)
        {
            node->channels[chan].checksum_errors = 0;
        }
    }
    return;
}

///////////////////////////////////////////
altix_card::altix_card()
{
}

int altix_card::init(altix_pci_card_info *card_info)
{
    info = card_info;
    fd = ::open(DEVICE_NAME, O_RDWR);
    locked  = false;
    dma_state = false;
    if(fd < 0 )
        return -1;
    return 0;
}


altix_card::~altix_card()
{
    if(this->islocked())
        altix_release_card(fd, info->id);
    ::close(fd);
}

int altix_card::unlock_card()
{
    int error;
    error = altix_release_card(fd, info->id);
    switch(error)
    {
    case ALTIX_NOT_INITIALIZED:
        emit send_log("Device library not initialized");
        break;
    case ALTIX_CARD_BUSY:
        emit send_log("Device already in use by PID " + QString::number(info->pid));
        break;
    case ALTIX_CARD_NOT_VALID:
        emit send_log("Card with ID " + QString::number(info->id) + " no longer exists");
        break;
    case ALTIX_OK:
        emit send_log ("Card with ID " + QString::number(info->id) + " unlocked");
        locked = false;
        break;
    }
    return error;
}

int altix_card::lock_card()
{
    int error;
    error = altix_lock_card(fd, info->id);
    switch(error)
    {
    case ALTIX_NOT_INITIALIZED:
        emit send_log("Device library not initialized");
        break;
    case ALTIX_CARD_BUSY:
        emit send_log("Device already in use by PID " + QString::number(info->pid));
        break;
    case ALTIX_CARD_NOT_VALID:
        emit send_log("Card with ID " + QString::number(info->id) + " no longer exists");
        break;
    case ALTIX_OK:
        emit send_log ("Card with ID " + QString::number(info->id) + " Found and locked");
        locked = true;
        break;
    }
    return locked;
}

void altix_card::enable_channel(int chan, bool on)
{
    altix_switch_channel(fd, chan);
    altix_enable_chan(fd, on);
}

int altix_card::read_packet(void * buff, int chan)
{
    if(!dma_state)
    {
        altix_toggle_dma(fd);
        dma_state = true;
    }
    altix_switch_channel(fd, chan);
    if(altix_read_event(fd, buff) == NULL) return -1;
    return altix_card::checksum_packet(buff);
}
int altix_card::read_chunk(void* buff, int chan, int chunk_size)
{
    if(!dma_state)
    {
        altix_toggle_dma(fd);
        dma_state = true;
    }
    altix_switch_channel(fd, chan);
    return ::read(fd, buff, chunk_size);
}

int altix_card::send_command(command_type cmd, int chan)
{
    if(!dma_state)
    {
        altix_toggle_dma(fd);
        dma_state = true;
    }
    altix_switch_channel(fd, chan);
    return altix_send_command(fd, cmd);
}

bool altix_card::poll_device(int chan)
{
    int ret;
    altix_switch_channel(fd, chan);
    ret = altix_poll_read(fd);
    return ret;
}

int altix_card::checksum_packet(void* buff)
{
    int* temp = (int*)buff;
    int* place;
    uint checksum;
    int errors = 0;
    int offset = 0;
    int packets_processed = 0;
    int words_per_packet = altix_packet_size()/4;
    int total_words = altix_event_size()/4;
    int segments_per_event = altix_event_size()/altix_packet_size();
    while(offset < total_words)
    {
        while(((temp[offset]) != PACKET_HEADER) && (offset < total_words))
        {
            offset++;
        }
        if((offset + words_per_packet) > total_words)
        {
            return errors + segments_per_event - packets_processed;
        }
        place = &(temp[offset]);
        checksum = 0;
        for(int j = 0; j < words_per_packet; j++)
        {
            if(j == words_per_packet - 2) continue;
            checksum += (uint)place[j];
        }
        if(checksum != (uint)place[words_per_packet - 2])
            errors++;
        packets_processed++;
        offset++;
    }
    return errors + segments_per_event - packets_processed;;
}

int altix_card::last_io_status()
{
    int status;
    int error;
    if(this->islocked())
    {
        error = altix_last_transfer_status(fd, &status);
        if(error == ALTIX_OK)
            return status;
        else
            return 0;
    }
    return 0;
}

void altix_card::clear_veto()
{
    if(this->islocked())
    {
        altix_send_pulse(this->fd, 0);
    }
}
