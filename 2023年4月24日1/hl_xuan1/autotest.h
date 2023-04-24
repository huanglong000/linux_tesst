#ifndef AUTOTEST_H
#define AUTOTEST_H
#include <QThread>

class AutoTest : public QThread
{
    Q_OBJECT
public:
    AutoTest(QWidget * parent = nullptr)
    {
        Q_UNUSED(parent);
    }
    void run() override;
    void io_set(char flag);
    QByteArray io_read();
    FILE *fp1;

signals:
    void resultReady(QString string);
};

#endif // AUTOTEST_H
