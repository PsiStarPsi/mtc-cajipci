#include "console.h"
#include "ui_console.h"
#include <QDebug>
#include <QDateTime>
console::console(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::console)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/img/icon.png"));
    this->setWindowTitle("Readout Console");
    this->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint| Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
}

console::~console()
{
    delete ui;
}

void console::add_log(QString str)
{
    QDateTime dateTime = QDateTime::currentDateTime();
    ui->con_text->append("[" + dateTime.toString("dd.MM.yyyy hh:mm:ss") +"] " + str + "\n");
}

void console::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void console::add_log_slot(const QString &str)
{
    QDateTime dateTime = QDateTime::currentDateTime();
    ui->con_text->append("[" + dateTime.toString("dd.MM.yyyy hh:mm:ss") +"] " + str + "\n");
}
