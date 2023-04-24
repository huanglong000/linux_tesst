#ifndef CANTHREAD_H
#define CANTHREAD_H

#include <QThread>
#include <QDebug>
#include <QProcess>
#include <QTimer>
#include <QString>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <QDebug>
#include <QMetaType>


#define ERR_CREATE -10

#define CMD_CAN_DOWM    "ip link set can0 down"
#define CMD_CAN_UP      "ip link set can0 up"
#define CMD_SET_BITRATE "ip link set can0 up type can bitrate "

struct canFrame{
    long canID;
    int dataLen;
    uint8_t data[8];
};

class CanThread : public QThread
{
    Q_OBJECT
public:
    CanThread();
    int Initial(int canRate);
    void stop();
    void restart();
protected:
    void run();
private:
    canFrame recvFrame;
    volatile bool stopped;
    int canSocket;
    struct sockaddr_can addr;
    struct ifreq ifr;
public:
    QString ExecSysCmd(QString cmd);

public slots:
    void SendMsg(canFrame sendFrame);
signals:
    void UpdateMsg(canFrame recvFrame);
    void UpdateError(bool flag);
    void UpdateMsgTandV(canFrame recvFrame);
    void UpdateMsgCb(canFrame recvFrame);
};

#endif // CANTHREAD_H
