#ifndef BLUE_THREAD_H
#define BLUE_THREAD_H
#include <QThread>

class blue_thread : public QThread
{
    Q_OBJECT
public:
    blue_thread(QWidget * parent = nullptr)
    {
        Q_UNUSED(parent);
    }
    void run() override;
    FILE *fp1;

signals:
    void resultReady(QString string);
};

#endif // BLUE_THREAD_H
