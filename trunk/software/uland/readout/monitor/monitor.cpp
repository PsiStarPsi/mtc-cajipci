#include "monitor.h"
#include "ui_monitor.h"
#include <QDebug>
#include <QFileDialog>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <QTreeWidgetItemIterator>
#include <QCloseEvent>

static inline int tree_get_id(int type)
{
    return (type) >> 4;
}

static inline int tree_get_channel(int type)
{
    return ((type) & 0x7) - 1;
}

static inline int tree_make_type(int card, int channel)
{
    return (card << 4) + channel +1;
}

monitor::monitor(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::monitor)
{
    this->setWindowIcon(QIcon(":/img/icon.png"));
    QTreeWidgetItem * item_l1;
    QTreeWidgetItem * item_l2;
    ui->setupUi(this);
    con = new console(parent);
    global_timer = new QTimer(this);
    hardware = new altix_wraper();
    con->add_log("Starting up");
    storage_file_name = "";
    //Hardware
    hardware->init(con);
    QObject::connect(hardware, SIGNAL(send_log(QString)), con, SLOT(add_log_slot(QString)));
    ui->driver_version_lable->setText("1." + QString::number(hardware->driver_version()));
    sample_rate_plot = new rateplot(ui->capture_tab);
    sample_rate_plot->setObjectName(QString::fromUtf8("capture_rate_plot"));
    sample_rate_plot->setGeometry(QRect(0, 40, 361, 131));
    sample_rate_plot->PlotSetup(256);
    sample_rate_plot->setCustomTitle("Transfer Rate (MB/s)");
    const altix_pci_card_stat * stats = hardware->get_stats();
    old_io = 0;
    //Icons
    enabled_icon = new QIcon(":/img/green-circle.png");
    disabled_icon = new QIcon(":/img/red-circle.png");
    for(int i = 0; i< hardware->get_num_cards(); i++)
    {
        item_l1 = new QTreeWidgetItem(tree_make_type(stats[i].id, -1));
        item_l1->setText(0, QString::number(stats[i].id));
        item_l1->setText(1, QString::number(stats[i].bytes_read/1024));
        item_l1->setText(2,"0");
        old_io += (1.0*((stats[i].bytes_read))/1024/1024.0);
        for(int j = 0; j < 4; j++)
        {
            int itemid = tree_make_type(stats[i].id, j);
            item_l2 = new QTreeWidgetItem(itemid);
            item_l2->setText(0, "Chan "+ QString::number(j));
            item_l2->setCheckState(0,Qt::Unchecked);
            item_l2->setIcon(1, *disabled_icon);
            item_l2->setText(2,"0");
            item_l1->addChild(item_l2);
        }
        ui->cpci_tree->addTopLevelItem(item_l1);
        item_l1->setExpanded(true);
    }
    ui->cpci_tree->setColumnWidth(0, 120);
    //Timer
    connect(global_timer, SIGNAL(timeout()), this, SLOT(updateGui()));
    global_timer->start(MONITOR_UPDATE_INTERVAL);
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(quit_slot()));
    connect(this, SIGNAL(aboutToQuit()), this, SLOT(quit_slot()));
    connect(ui->actionSet_Output, SIGNAL(triggered()), this, SLOT(set_file_slot()));
    connect(hardware, SIGNAL(finished()), this, SLOT(hardware_stoped()));
    //Settings
    this->read_settings();
}

monitor::~monitor()
{
    hardware->deinit();
    delete ui;
}

void monitor::updateGui()
{
    double total_moved = 0;
    QTreeWidgetItem * top_level;
    //Update IO stats
    const altix_pci_card_stat * stats = hardware->get_stats();
    if(stats != NULL)
    {
        for(int i = 0; i< hardware->get_num_cards(); i++)
        {
            top_level = ui->cpci_tree->topLevelItem(i);
            top_level->setText(1, QString::number(stats[i].bytes_read/1024));
            total_moved +=(stats[i].bytes_read); //MB
        }
        total_moved /=1048576;
        if(hardware->isRunning())
            sample_rate_plot->add2plot((total_moved - old_io)*(1000.0/MONITOR_UPDATE_INTERVAL));
        old_io = total_moved;
    }
    else
    {
        if(hardware->isRunning())
        {
            sample_rate_plot->add2plot(0.0);
        }
    }
    ui->aurora_errors_lable->setText(QString::number(hardware->get_aurora_errors()));
    ui->bad_events_lable->setText(QString::number(hardware->get_bad_events()));
    if(hardware->get_total_events() != 0)
        ui->bad_events_ratio_lable->setText(QString::number(hardware->get_bad_events()*100.0/hardware->get_total_events()) + "%");
    else
        ui->bad_events_ratio_lable->setText("0%");
    ui->bus_errors_lable->setText(QString::number(hardware->get_bad_pci()));
    ui->dma_errors_lable->setText(QString::number(hardware->get_bad_dma()));
    ui->watchdog_errors_lable->setText(QString::number(hardware->get_watchdog_overflow()));

    int card_errors;
    int channel_errors;
    for(int i = 0; i< hardware->get_num_cards(); i++)
    {
        card_errors = 0;
        top_level = ui->cpci_tree->topLevelItem(i);
        QTreeWidgetItemIterator it(top_level);
        it++;
        for(int j = 0; j< 4; j++)
        {
            int channel = tree_get_channel((*it)->type());
            int id = tree_get_id((*it)->type());
            if(hardware->is_channel_enabled(id, channel) & ((*it)->checkState(0) == Qt::Checked))
            {
                (*it)->setIcon(1, *enabled_icon);
            }
            else
            {
                (*it)->setIcon(1, *disabled_icon);
                (*it)->setCheckState(0,Qt::Unchecked);
                if(!hardware->isRunning())
                {
                    hardware->set_channel_status(id, channel, false);
                }
            }
            channel_errors = hardware->get_checksum_errors(id,channel);
            (*it)->setText(2, QString::number(channel_errors));
            card_errors += channel_errors;
            it++;
        }
        top_level->setText(2, QString::number(card_errors));
    }
}

void monitor::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void monitor::on_cpci_tree_doubleClicked(QModelIndex index)
{
    QTreeWidgetItem *item = ui->cpci_tree->currentItem();
    int chan = tree_get_channel(item->type());
    int card = tree_get_id(item->type());
}

void monitor::closeEvent(QCloseEvent *event)
{
    this->quit_slot();
    exit(0);
}

void monitor::set_file_slot()
{
    QStringList fileNames;
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setOption(QFileDialog::DontConfirmOverwrite, true);
    dialog.exec();
    fileNames = dialog.selectedFiles();
    if(fileNames.size() >0)
    {
        set_output_file_name(fileNames[0]);
    }
    else
    {
        set_output_file_name(QString(""));
    }
}

void monitor::set_output_file_name(QString name)
{
    if(hardware->set_name(name))
    {
        QFileInfo pathInfo(name);
        ui->outfile_lable->setText(pathInfo.fileName());
        this->storage_file_name = name;
    }
    else
    {
        ui->outfile_lable->setText("None");
        this->storage_file_name = "";
    }
}

void monitor::quit_slot()
{
    if(hardware->isRunning())
        hardware->stop_hardware_loop();
    save_settings();
    hardware->deinit();
    hardware->wait();
    exit(0);
}



void monitor::on_actionShow_console_triggered()
{
    if(!con->isVisible())
        con->show();
    else
        con->hide();
}

void monitor::on_runButton_toggled(bool checked)
{
    if(checked)
    {
        hardware->start_hardware_loop();
        ui->runButton->setText("Stop");
        ui->cpci_tree->setEnabled(false);
        ui->option_toolbox->setEnabled(false);
        ui->reset_stats_button->setEnabled(false);
        ui->actionSet_Output->setEnabled(false);
        ui->outfile_lable->setEnabled(false);
    }
    else
    {
        ui->runButton->setText("Stopping...");
        ui->runButton->setEnabled(false);
        hardware->stop_hardware_loop();
    }
}

void monitor::hardware_stoped()
{
    ui->cpci_tree->setEnabled(true);
    ui->option_toolbox->setEnabled(true);
    ui->reset_stats_button->setEnabled(true);
    ui->outfile_lable->setEnabled(true);
    ui->actionSet_Output->setEnabled(true);
    ui->runButton->setText("Run");
    ui->runButton->setEnabled(true);
}

void monitor::on_cpci_tree_itemChanged(QTreeWidgetItem* item, int column)
{
    if(column != 0) return;
    int card = tree_get_id(item->type());
    int channel = tree_get_channel(item->type());
    if(channel < 0 ) return;
    if(item->checkState(0) == Qt::Checked)
    {
        item->setIcon(1, *enabled_icon);
    }
    else
    {
        item->setIcon(1, *disabled_icon);
    }
    hardware->set_channel_status(card, channel, item->checkState(0) == Qt::Checked);
}

void monitor::on_flowctl_box_stateChanged(int state)
{
    hardware->set_flow_ctl(state);
}

void monitor::on_rst_one_error_stateChanged(int state)
{
    hardware->set_rst_ctl(state);
}

void monitor::on_flush_to_disk_stateChanged(int state)
{
    hardware->set_flush_ctl(state);
}


void monitor::on_store_meta_stateChanged(int state)
{
    hardware->set_store_meta(state);
}

void monitor::on_vetoctl_box_stateChanged(int state)
{
    hardware->set_veto(state);
}

void monitor::on_reset_stats_button_clicked()
{
    hardware->reset_stats();
    this->updateGui();
}

void monitor::read_settings()
{
    QSettings settings;
    bool state;
    settings.beginGroup("console");
    {
        state = settings.value("show", false).toBool();
        con->setVisible(state);
    }
    settings.endGroup();
    settings.beginGroup("storage");
    {
        state = settings.value("flush_disk", true).toBool();
        ui->flush_to_disk->setCheckState(state ? Qt::Checked : Qt::Unchecked);
        hardware->set_flush_ctl(state);

        state = settings.value("store_meta", true).toBool();
        ui->store_meta->setCheckState(state ? Qt::Checked : Qt::Unchecked);
        hardware->set_store_meta(state);

        this->storage_file_name = settings.value("output_file", "").toString();
        this->set_output_file_name(storage_file_name);
    }
    settings.endGroup();
    settings.beginGroup("link");
    {
        state = settings.value("reset_on_error", true).toBool();
        ui->rst_one_error->setCheckState(state ? Qt::Checked : Qt::Unchecked);
        hardware->set_rst_ctl(state);
        
        state = settings.value("flow_ctl", true).toBool();
        ui->flowctl_box->setCheckState(state ? Qt::Checked : Qt::Unchecked);
        hardware->set_flow_ctl(state);

        state = settings.value("veto_clear", true).toBool();
        ui->vetoctl_box->setCheckState(state ? Qt::Checked : Qt::Unchecked);
        hardware->set_veto(state);
    }
    settings.endGroup();
    settings.beginGroup("gui");
    {
        ui->control_tabs->setCurrentIndex(settings.value("selected_tab", 0).toInt());
    }
    settings.endGroup();
}

void monitor::save_settings()
{
    QSettings settings;
    settings.beginGroup("console");
    settings.setValue("show", con->isVisible());
    settings.endGroup();
    settings.beginGroup("storage");
    settings.setValue("flush_disk", ui->flush_to_disk->isChecked());
    settings.setValue("store_meta", ui->store_meta->isChecked());
    settings.setValue("output_file", this->storage_file_name);
    settings.endGroup();
    settings.beginGroup("link");
    settings.setValue("reset_on_error", ui->rst_one_error->isChecked());
    settings.setValue("flow_ctl", ui->flowctl_box->isChecked());
    settings.setValue("veto_clear", ui->vetoctl_box->isChecked());
    settings.endGroup();
    settings.beginGroup("gui");
    settings.setValue("selected_tab", ui->control_tabs->currentIndex());
    settings.endGroup();

}

void monitor::on_outfile_lable_clicked()
{
    set_output_file_name("");
    ui->runButton->setFocus();
}

