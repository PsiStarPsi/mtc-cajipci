#ifndef MONITOR_H
#define MONITOR_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include "console.h"
#include "hw_access.h"
#include "rateplot.h"
#include <QList>
#include <QTimer>
#include <QIcon>
#include <QSettings>

#define MONITOR_UPDATE_INTERVAL 200

namespace Ui {
    class monitor;
}

class monitor : public QMainWindow {
    Q_OBJECT
public:
    monitor(QWidget *parent = 0);
    ~monitor();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *e);
signals:
    void aboutToQuit();

private:
    Ui::monitor *ui;
    console * con;
    altix_wraper * hardware;
    rateplot* sample_rate_plot;
    QTimer * global_timer;
    double old_io;
    QIcon * enabled_icon;
    QIcon * disabled_icon;
    QString storage_file_name;
    void read_settings();
    void save_settings();
    void set_output_file_name(QString name);
private slots:
    void on_vetoctl_box_stateChanged(int );
    void on_outfile_lable_clicked();
    void on_store_meta_stateChanged(int );
    void on_reset_stats_button_clicked();
    void on_flush_to_disk_stateChanged(int );
    void on_rst_one_error_stateChanged(int );
    void on_flowctl_box_stateChanged(int );
    void on_cpci_tree_itemChanged(QTreeWidgetItem* item, int column);
    void on_runButton_toggled(bool checked);
    void on_actionShow_console_triggered();
    void on_cpci_tree_doubleClicked(QModelIndex index);
    void updateGui();
    void set_file_slot();
    void quit_slot();
    void hardware_stoped();
};

#endif // MONITOR_H
