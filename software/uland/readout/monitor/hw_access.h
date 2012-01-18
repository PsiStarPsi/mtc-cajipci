#ifndef HW_ACCESS_H
#define HW_ACCESS_H
#include <qglobal.h>
extern "C" {
#include "libaltix.h"
}

#include "console.h"
#include <QMap>
#include <QPushButton>
#include <QThread>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QThread>
#include <QFile>
#include <QReadWriteLock>
#include <QList>

typedef void* command_type;
#define DEVICE_NAME "/dev/altixpci0"
#define PACKET_HEADER 0x00be11e2
class altix_card : public QObject
{
    Q_OBJECT
public:
    altix_card();
    ~altix_card();
    int init(altix_pci_card_info *);
    int read_packet(void*, int chan);
    int read_chunk(void*, int chan, int chunk_size);
    bool poll_device(int chan);
    int lock_card();
    int unlock_card();
    int send_command(command_type, int chan);
    bool islocked(){return locked;}
    int getId(){return info->id;}
    static int checksum_packet(void *buff);
    void enable_channel(int chan, bool on);
    int last_io_status();
    void clear_veto();
signals:
    void send_log(QString);
public slots:

private:
    altix_pci_card_info *info;
    bool locked;
    int fd;
    bool dma_state;
};

typedef struct
{
    int* data;
    uint checksum_errors;
    bool enabled;
    uint bytes_left;
    bool pollok;
    QList <int> *meta;
    QReadWriteLock *lock;
} channel_node;

typedef struct
{
    uint id;
    altix_card *card;
    channel_node * channels;
    uint transfer_status;
} card_node;

class altix_wraper : public QThread
{
    Q_OBJECT
public:
    void init(console * a);
    void deinit();
    bool set_name(QString fname);
    int read_packet(void*, int card, int chan);
    int get_num_cards();
    const altix_pci_card_stat * get_stats();
    bool is_channel_enabled(int card, int chan);
    void set_channel_status(int card, int chan, bool on);
    int driver_version();
    int get_checksum_errors(int card, int chan);
    void set_flow_ctl(bool state);
    void set_rst_ctl(bool state);
    void set_flush_ctl(bool state);
    void set_store_meta(bool state);
    void set_veto(bool state);
    static int library_version();
    static int event_size();
    static int packet_size();
    void start_hardware_loop();
    void stop_hardware_loop();
    //Stats:
    void reset_stats();
    uint get_bad_pci(){return this->bad_pci;}
    uint get_bad_dma(){return this->bad_dma;}
    uint get_watchdog_overflow(){return this->overflow_dog;}
    uint get_aurora_errors(){return this->aurora_errors;}
    uint get_bad_events(){return this->bad_events;}
    uint get_total_events(){return this->total_events;}
signals:
    void send_log(const QString &log);
    void enable_channel_signal(int chan, bool on);
protected:
    void run();
private:
    bool read_out_loop_done();
    void init_tread();
    void update_poll();
    void single_readout();
    void save_and_process();
    void deinit_thread();
    void process_meta(int meta, int chan);

    QReadWriteLock * io_lock;
    bool terminate_flag;
    bool flow_ctl;
    bool rst_ctl;
    bool flush_ctl;
    bool veto_ctl;
    bool store_meta;
    QMap <int,card_node*> cards;
    altix_pci_card_info* card_infos;
    char* flowpacket;
    console * con;
    int fd;
    bool running;
    bool inited;
    QFile *outf;

    //statistics
    uint bad_pci;
    uint bad_dma;
    uint overflow_dog;
    uint aurora_errors;
    uint bad_events;
    uint total_events;
};

#endif // HW_ACCESS_H
