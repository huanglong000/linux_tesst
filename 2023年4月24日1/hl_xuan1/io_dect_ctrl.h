#ifndef IO_DECT_CTRL_H
#define IO_DECT_CTRL_H
#include <QThread>

class io_dect_ctrl : public QThread
{
    Q_OBJECT
public:
    io_dect_ctrl(QWidget * parent = nullptr)
    {
        Q_UNUSED(parent);
    }
    void run() override;
    void io_set(char flag);
    QByteArray io_read();
    FILE *fp1;

signals:
    void resultReady(QString string);
    void result1(char flag);
};

#endif // IO_DECT_CTRL_H
