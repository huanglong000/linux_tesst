#include "canthread.h"


#ifndef PF_CAN
#define PF_CAN 29
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif

CanThread::CanThread()
{
    qRegisterMetaType<canFrame>("canFrame");
    stopped = false;
}

int CanThread::Initial(int canRate)
{
    // Use system command init can0
    QString retStr;
    ExecSysCmd(CMD_CAN_DOWM);
    ExecSysCmd(CMD_SET_BITRATE + QString::number(canRate));

    // 创建一个SocketCan
    canSocket = socket(PF_CAN,SOCK_RAW,CAN_RAW);
    if(canSocket < 0)
    {
        return ERR_CREATE;
    }

    // 设置一些参数，这个是使用的can网络的设备号
    strcpy((char *)(ifr.ifr_name), "can0");
    int ret = ioctl(canSocket, SIOCGIFINDEX, &ifr);
    if(ret < 0)
    {
        qDebug() << "CAN IOCTL SIOCGIFINDEX ERROR!";
        return ret;
    }

    // 设置使用CAN协议
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    ret = bind(canSocket, (struct sockaddr*)&addr, sizeof(addr));
    if(ret < 0)
    {

        qDebug() << "BIND CAN_SOCKET ERROR!";
        return ret;
    }

    return 0;
}

void CanThread::SendMsg(canFrame sndframe)
{
    int nbytes = 0;
    struct can_frame frame;

    // 设置发送到的设备id
    frame.can_id = sndframe.canID;
    frame.can_id &= CAN_EFF_MASK;
    frame.can_id |= CAN_EFF_FLAG;//设置帧格式
    frame.can_dlc = sndframe.dataLen;
    memcpy(frame.data, sndframe.data, frame.can_dlc);
    // 发送数据
    nbytes = sendto(canSocket, &frame, sizeof(struct can_frame), 0, (struct sockaddr*)&addr, sizeof(addr));
    if(nbytes <= 0)
    {
      //  emit UpdateError(true);
    }
}

void CanThread::run()
{
    int nbytes;
    struct can_frame frame;
    QByteArray qData;
    qData.resize(sizeof(canFrame));
    while(!stopped)
    {
       QThread::msleep(5);
       nbytes = read(canSocket, &frame, sizeof(frame));
        if(nbytes > 0)
        {
            recvFrame.canID = frame.can_id;
            recvFrame.dataLen = frame.can_dlc;
            memcpy(recvFrame.data, frame.data, recvFrame.dataLen);
            if(recvFrame.dataLen == 8){

                if(QString::number(recvFrame.canID&0x7fffffff,16) =="18b0ccbb")//待机 两种 4 00 00 a1 ff  ||4 00 00 a2 ff
                {
                    emit UpdateMsg(recvFrame);
                }else if(QString::number(recvFrame.canID&0x7fffffff,16) =="18b2ccbb"){
                    emit UpdateMsg(recvFrame);
                }
                else if(QString::number(recvFrame.canID&0x7fffffff,16) =="18b428f4"){
                        //wendu
                    emit UpdateMsgTandV(recvFrame);
                }
                else if(QString::number(recvFrame.canID&0x7fffffff,16) =="18c828f4"){
                        //dianya 1-4
                    emit UpdateMsgTandV(recvFrame);
                }
                else if(QString::number(recvFrame.canID&0x7fffffff,16) =="18c928f4"){
                    emit UpdateMsgTandV(recvFrame);
                }
                else if(QString::number(recvFrame.canID&0x7fffffff,16) =="18ca28f4"){
                    emit UpdateMsgTandV(recvFrame);
                }
                else if(QString::number(recvFrame.canID&0x7fffffff,16) =="18cb28f4"){
                    emit UpdateMsgTandV(recvFrame);
                }
                else if(QString::number(recvFrame.canID&0x7fffffff,16) =="18cc28f4"){
                    emit UpdateMsgTandV(recvFrame);
                }
                else if(QString::number(recvFrame.canID&0x7fffffff,16) =="18cd28f4"){
                    emit UpdateMsgTandV(recvFrame);
                }
                //jiaozhun
                else if(QString::number(recvFrame.canID&0x7fffffff,16) =="19f1bbcc"){
                      emit UpdateMsgCb(recvFrame);
                  }
                //yunxingcanshu
                  else if(QString::number(recvFrame.canID&0x7fffffff,16) =="19c0ccbb"){
                     qDebug()<<"19c0ccbb";
                      emit UpdateMsgCb(recvFrame);
                  }
                  //qidongtingzhi
                  else if(QString::number(recvFrame.canID&0x7fffffff,16) =="19c1ccbb"){
                      emit UpdateMsgCb(recvFrame);
                  }
            }

        }

    }
}

QString CanThread::ExecSysCmd(QString strCmd)
{
    QProcess p;
    p.start("bash", QStringList() <<"-c" << strCmd);
    p.waitForFinished();
    QString strResult = p.readAllStandardOutput();
    return strResult;
}
void CanThread::stop()
{
    stopped = true;
}
void CanThread::restart()
{
    stopped = false;
}


