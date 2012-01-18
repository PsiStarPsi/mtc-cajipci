#ifndef CONSOLE_H
#define CONSOLE_H

#include <QWidget>
#include <QString>
namespace Ui {
    class console;
}

class console : public QWidget {
    Q_OBJECT
public:
    console(QWidget *parent = 0);
    ~console();
    void add_log(QString str);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::console *ui;

public slots:
    void add_log_slot(const QString &str);
};

#endif // CONSOLE_H
