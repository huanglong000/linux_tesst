#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTime>
#include <QTimer>
#include "blue_thread.h"
#include <QFile>
#include <qbluetoothuuid.h>
#include <qbluetoothserver.h>
#include <qbluetoothservicediscoveryagent.h>
#include <qbluetoothdeviceinfo.h>
#include <qbluetoothlocaldevice.h>

static const QLatin1String
serviceUuid("e8e10f95-1a70-4b27-9ccf-02010264e9c8");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serialPort = new QSerialPort(this);
    serialPort_485 = new QSerialPort(this);
    timer = new QTimer(this);
    timer_wifi = new QTimer(this);
    timer_eth0 = new QTimer(this);
    workThread = new blue_thread(this);
    timer_232 = new QTimer(this);
    timer_485 = new QTimer(this);
    timer_bt = new QTimer(this);
    ioDect = new io_dect_ctrl(this);
    //localAdapterInit();
}
#if 1
void MainWindow::localAdapterInit()
{
    /* 查找本地蓝牙的个数 */
    localAdapters = QBluetoothLocalDevice::allDevices();
    qDebug() << "localAdapter: " << localAdapters.count();

    QBluetoothLocalDevice localDevice;
    localDevice.setHostMode(QBluetoothLocalDevice::HostDiscoverable);

    QBluetoothAddress adapter = QBluetoothAddress();
    remoteSelector = new RemoteSelector(adapter, this);

    connect(remoteSelector,
            SIGNAL(newServiceFound(QListWidgetItem*)),
            this, SLOT(newServiceFound(QListWidgetItem*)));
    #if 0
    /* 初始化服务端 */
    server = new ChatServer(this);

    connect(server, SIGNAL(clientConnected(QString)),
            this, SLOT(connected(QString)));

    connect(server, SIGNAL(clientDisconnected(QString)),
            this, SLOT(disconnected(QString)));

    connect(server, SIGNAL(messageReceived(QString, QString)),
            this, SLOT(showMessage(QString, QString)));

    connect(this, SIGNAL(sendMessage(QString)),
            server, SLOT(sendMessage(QString)));

    connect(this, SIGNAL(disconnect()),
            server, SLOT(disconnect()));

    server->startServer();
#endif
    /* 获取本地蓝牙的名称 */
    localName = QBluetoothLocalDevice().name();
}
#endif

MainWindow::~MainWindow()
{
    serialPort->close();
    delete ui;
}


void MainWindow::on_pushButton_clicked()//232
{

    QByteArray date;
    date.append('x');
    date.append('p');
    serialPort->setPortName("ttyS4");
    serialPort->setBaudRate(115200,QSerialPort::Direction::AllDirections);
    disconnect(serialPort, SIGNAL(readyRead()),this, SLOT(serialPortReadyRead()));
    connect(serialPort, SIGNAL(readyRead()),this, SLOT(serialPortReadyRead()));
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    serialPort->close();

    serialPort->open(QIODevice::ReadWrite);
    serialPort->write(date);
    if(!timer_232->isActive())
    {
        timer_232->start(500);
    }
    disconnect(timer_232,&QTimer::timeout,this,&MainWindow::time232over);
    connect(timer_232,&QTimer::timeout,this,&MainWindow::time232over);




}
void MainWindow::time232over()
{
    timer_232->stop();
    listView("232通讯失败");
    autofail();

}
void MainWindow::serialPortReadyRead()//232
{
    /* 接收缓冲区中读取数据 */
    QByteArray rr;
    rr.append('x');
    rr.append('p');

    QByteArray buf = serialPort->readAll();
    timer_232->stop();
    ui->listWidget->addItem("232 recive:");
    ui->listWidget->addItem(buf);
    if(buf == rr)
    {

        listView("232测试正常");
        combutton(1,ui->checkBox_232);
        respon();


    }
    else
    {
        listView("232数据校验失败");
        autofail();
    }

}

void MainWindow::serialPortReadyRead_485()
{
    /* 接收缓冲区中读取数据 */
    QByteArray rr;
    rr.append('x');
    rr.append('p');

    QByteArray buf = serialPort_485->readAll();
    timer_485->stop();
    //ui->listWidget->insertItem(0,buf);
    listView(buf);
    if(buf == rr)
    {
        listView("485测试正常");
        combutton(1,ui->checkBox_485);
        respon();
    }
    else
    {
        listView("数据校验失败");
        autofail();
    }

}

void MainWindow::on_pushButton_2_clicked()//485发送
{
    QByteArray date;
    date.append(0x5a);
    date.append('x');
    date.append('p');
    serialPort_485->setPortName("ttyS7");
    serialPort_485->setBaudRate(115200,QSerialPort::Direction::AllDirections);
    disconnect(serialPort_485, SIGNAL(readyRead()),this, SLOT(serialPortReadyRead_485()));
    connect(serialPort_485, SIGNAL(readyRead()),this, SLOT(serialPortReadyRead_485()));
    serialPort_485->setDataBits(QSerialPort::Data8);
    serialPort_485->setParity(QSerialPort::NoParity);
    serialPort_485->setStopBits(QSerialPort::OneStop);
    serialPort_485->setFlowControl(QSerialPort::NoFlowControl);
    serialPort_485->close();

    serialPort_485->open(QIODevice::ReadWrite);
    serialPort_485->write(date);
    if(!timer_485->isActive())
    {
        timer_485->start(500);
    }
    disconnect(timer_485,&QTimer::timeout,this,&MainWindow::time_485_over);
    connect(timer_485,&QTimer::timeout,this,&MainWindow::time_485_over);

}
void MainWindow::time_485_over()
{
    timer_485->stop();
    listView("485通讯异常");
    autofail();
}
void MainWindow::combutton(char flag ,QCheckBox *checkBox)//0,不选中，1选中
{
    if(flag)
        checkBox->setCheckState(Qt::CheckState::Checked);
    else
        checkBox->setCheckState(Qt::CheckState::Unchecked);
}
void MainWindow::on_pushButton_3_clicked()//can1发送
{
    QString cmd1 =tr("ifconfig can0 down");
    QString cmd3 =tr("ifconfig can1 down");
    QString cmd4 =tr("ifconfig can2 down");
    QString cmd2 =tr("ip link set up can0 type can bitrate 250000");
    QString errorString;
    QString stringC = "18ffff00";
    QByteArray date = "12345678";
    bool ok;
    int framId = stringC.toUInt(&ok,16);
    QCanBusFrame frame = QCanBusFrame(framId,date);
    system(cmd1.toStdString().c_str());
    //system(cmd3.toStdString().c_str());
    //system(cmd4.toStdString().c_str());
    system(cmd2.toStdString().c_str());
    canDevice = QCanBus::instance()->createDevice("socketcan","can0",&errorString);
    if(!canDevice->connectDevice())
    {
        delete canDevice;
        canDevice = nullptr;
    }
    disconnect(canDevice,SIGNAL(framesReceived()),this,SLOT(receivedFrames()));
    connect(canDevice,SIGNAL(framesReceived()),this,SLOT(receivedFrames()));
    canDevice->writeFrame(frame);
}
void MainWindow::receivedFrames()
{
    QString view;
    QString cmd1 =tr("ifconfig can0 down");
    if(!canDevice)
    {
        return;
    }
    if(canDevice->framesAvailable())
    {
        const QCanBusFrame frame = canDevice->readFrame();
        if(frame.frameType() == QCanBusFrame::ErrorFrame)
        {
            view = canDevice->interpretErrorFrame(frame);
            listView(view);
            listView("can1错误");

            system(cmd1.toStdString().c_str());
            disconnect(canDevice,SIGNAL(framesReceived()),this,SLOT(receivedFrames()));
            autofail();

        }
        else
        {
            view = frame.toString();
            listView("can1消息");

            combutton(1,ui->checkBox_can1);
            listView(view);
            respon();
        }
    }
}
void MainWindow::receivedFrames1()
{
    QString view;
    QString cmd1 =tr("ifconfig can1 down");

    if(!canDevice1)
    {
        return;
    }
    if(canDevice1->framesAvailable())
    {
        const QCanBusFrame frame = canDevice1->readFrame();
        if(frame.frameType() == QCanBusFrame::ErrorFrame)
        {
            view = canDevice1->interpretErrorFrame(frame);
            listView(view);
            listView("can2错误");

            system(cmd1.toStdString().c_str());
            disconnect(canDevice1,SIGNAL(framesReceived()),this,SLOT(receivedFrames1()));
            autofail();
        }
        else
        {
            view = frame.toString();
            listView("can2消息");

            combutton(1,ui->checkBox_can2);
            listView(view);
            respon();
        }
    }
}
void MainWindow::receivedFrames2()
{
    QString view;
    QString cmd1 =tr("ifconfig can2 down");
    if(!canDevice2)
    {
        return;
    }
    if(canDevice2->framesAvailable())
    {
        const QCanBusFrame frame = canDevice2->readFrame();
        if(frame.frameType() == QCanBusFrame::ErrorFrame)
        {
            view = canDevice2->interpretErrorFrame(frame);
            listView(view);
            listView("can3错误");

            system(cmd1.toStdString().c_str());
            disconnect(canDevice2,SIGNAL(framesReceived()),this,SLOT(receivedFrames2()));
            autofail();
        }
        else
        {
            view = frame.toString();
            listView("can3消息");
            combutton(1,ui->checkBox_can3);

            listView(view);
            respon();
        }
    }
}

void MainWindow::on_pushButton_8_clicked()//wifi检测
{
    QTime t;

    QString string;
    char output[64];
    listView("test inet,plase wait...");
    QString cmd1 =tr("wpa_passphrase XPtech liliaozheng1301 >> /etc/wpa_supplicant.conf");
    system(cmd1.toStdString().c_str());
    cmd1 = tr("sync");
    system(cmd1.toStdString().c_str());
    cmd1 = tr("wpa_supplicant -Dnl80211 -i wlan0 -c /etc/wpa_supplicant.conf -B");
    system(cmd1.toStdString().c_str());




    cmd1 = tr("udhcpc -b -i wlan0");
    system(cmd1.toStdString().c_str());





    timer->start(1000);
    //timer_wifi->start(5000);
    //disconnect(timer,SIGNAL(QTimer::timeout()),this,SLOT(time_over()));
    //connect(timer,SIGNAL(QTimer::timeout()),this,SLOT(time_over()));
    disconnect(timer,&QTimer::timeout,this,&MainWindow::time_over);
    connect(timer,&QTimer::timeout,this,&MainWindow::time_over);

    //disconnect(timer_wifi,&QTimer::timeout,this,&MainWindow::time_wifi_over);
    //connect(timer_wifi,&QTimer::timeout,this,&MainWindow::time_wifi_over);




}
void MainWindow::time_wifi_over()
{
    timer_wifi->stop();
    listView("wifi失败，插上天线试试");
    autofail();
}
void MainWindow::time_over()
{
    FILE *fp;
    QString string;
    int size;
    int i = 0;
    int ok = 0;
    char output[64];
    fp = popen("ping -I wlan0 www.163.com -w 5","r");
    //ui->listWidget->insertItem(0,"kaishiping");
    timer->stop();
    //timer_wifi->stop();
    if(fp)
    {
        string.clear();
        ok = 0;
        for(i = 0;i < 1000;i++)
        {
            fgets(output,50,fp);
            string = output;
            ok += string.size();
            listView(string);
            if(feof(fp))
            {
                break;
            }
        }
        string.sprintf("%d",ok);
        listView(string);

#if 1
        if(ok  < 300)
        {

            //ui->listWidget->insertItem(i,"wifi失败");
            listView("wifi失败");
            autofail();
        }
        else
        {
            listView("wifi正常");
            combutton(1,ui->checkBox_wifi);
            respon();
        }
#endif


    }

}
void MainWindow::blue_string(QString string)//gai cheng
{
    ui->pushButton_11->setEnabled(false);
    listView("drive ok");
}
void MainWindow::on_pushButton_11_clicked()//蓝牙检测
{
    //QString cmd1 =tr("kill $(pidof brcm_patchram_plus1)");
    listView("wait.......");
    QString cmd1 =tr("kill brcm_patchram_plus1");
    system(cmd1.toStdString().c_str());
    cmd1 = tr("echo 0 > /sys/class/rfkill/rfkill0/state");
    system(cmd1.toStdString().c_str());
    disconnect(workThread,&blue_thread::resultReady,this,&MainWindow::blue_string);
    connect(workThread,&blue_thread::resultReady,this,&MainWindow::blue_string);
    if(!workThread->isRunning())
        workThread->start();

}

void MainWindow::on_pushButton_12_clicked()//输入检测
{

    char buf[3];
    QString string;
    QFile file;
    QByteArray temp;
    temp.clear();
    listView("电平检查结果");
    //-----------------------------------
    file.setFileName("/sys/class/gpio/gpio132/value");
    if(file.exists())
    {
        if(file.open(QIODevice::ReadOnly))
        {
            temp = file.read(1);

        }
    }
    file.close();
    //-----------------------------------
    file.setFileName("/sys/class/gpio/gpio133/value");
    if(file.exists())
    {
        if(file.open(QIODevice::ReadOnly))
        {
            temp += file.read(1);

        }
    }
    file.close();
    //-----------------------------------
    file.setFileName("/sys/class/gpio/gpio126/value");
    if(file.exists())
    {
        if(file.open(QIODevice::ReadOnly))
        {
            temp += file.read(1);

        }
    }
    file.close();
    //-----------------------------------
    file.setFileName("/sys/class/gpio/gpio127/value");
    if(file.exists())
    {
        if(file.open(QIODevice::ReadOnly))
        {
            temp += file.read(1);

        }
    }
    //ui->listWidget->insertItem(0,temp);
    listView(temp);
    file.close();
    //-----------------------------------


}






void MainWindow::on_pushButton_13_clicked()//重新检测
{
    ui->listWidget->clear();
    combutton(0,ui->checkBox_232);
    combutton(0,ui->checkBox_485);
    combutton(0,ui->checkBox_can1);
    combutton(0,ui->checkBox_can2);
    combutton(0,ui->checkBox_can3);
    combutton(0,ui->checkBox_eth);
    combutton(0,ui->checkBox_in);
    combutton(0,ui->checkBox_out);
    combutton(0,ui->checkBox_wifi);
    combutton(0,ui->checkBox_sd);
    combutton(0,ui->checkBox_bt);

    auto_switch = 0;
}
void MainWindow::listView(QString string)
{
    int cnt = ui->listWidget->count()+1;
    ui->listWidget->insertItem(cnt,string);
    ui->listWidget->scrollToBottom();
}

void MainWindow::on_pushButton_4_clicked()//can2
{
    QString cmd1 =tr("ifconfig can1 down");
    QString cmd3 =tr("ifconfig can0 down");
    QString cmd4 =tr("ifconfig can2 down");
    QString cmd2 =tr("ip link set up can1 type can bitrate 250000");
    QString errorString;
    QString stringC = "18ffff00";
    QByteArray date = "12345678";
    bool ok;
    int framId = stringC.toUInt(&ok,16);
    QCanBusFrame frame = QCanBusFrame(framId,date);
    system(cmd1.toStdString().c_str());
    //system(cmd3.toStdString().c_str());
    //system(cmd4.toStdString().c_str());
    system(cmd2.toStdString().c_str());
    canDevice1 = QCanBus::instance()->createDevice("socketcan","can1",&errorString);
    if(!canDevice1->connectDevice())
    {
        delete canDevice1;
        canDevice1 = nullptr;
    }
    disconnect(canDevice1,SIGNAL(framesReceived()),this,SLOT(receivedFrames1()));
    connect(canDevice1,SIGNAL(framesReceived()),this,SLOT(receivedFrames1()));
    canDevice1->writeFrame(frame);
}

void MainWindow::on_pushButton_5_clicked()//can3
{
    QString cmd1 =tr("ifconfig can2 down");
    QString cmd3 =tr("ifconfig can0 down");
    QString cmd4 =tr("ifconfig can1 down");
    QString cmd2 =tr("ip link set up can2 type can bitrate 250000");
    QString errorString;
    QString stringC = "18ffff00";
    QByteArray date = "12345678";
    bool ok;
    int framId = stringC.toUInt(&ok,16);
    QCanBusFrame frame = QCanBusFrame(framId,date);
    system(cmd1.toStdString().c_str());
    //system(cmd3.toStdString().c_str());
    //system(cmd4.toStdString().c_str());
    system(cmd2.toStdString().c_str());
    canDevice2 = QCanBus::instance()->createDevice("socketcan","can2",&errorString);
    if(!canDevice2->connectDevice())
    {
        delete canDevice2;
        canDevice2 = nullptr;
    }
    disconnect(canDevice2,SIGNAL(framesReceived()),this,SLOT(receivedFrames2()));
    connect(canDevice2,SIGNAL(framesReceived()),this,SLOT(receivedFrames2()));
    canDevice2->writeFrame(frame);
}

void MainWindow::on_pushButton_7_clicked()//输出检查
{

    char buf[3];
    int row;
    QString string;
    QFile file;
    QByteArray temp;
    static char flash = 0;
    row = ui->listWidget->count()+1;
    //ui->listWidget->insertItem(row,"输出检查");
    listView("输出检查");
    flash ^= 1;
    if(flash)
    {
        temp = "1";
    }
    else
    {
        temp = "0";
    }
    //temp.clear();
    //-----------------------------------
    file.setFileName("/sys/class/gpio/gpio40/value");
    if(file.exists())
    {
        if(file.open(QIODevice::WriteOnly))
        {
            file.write(temp);

        }
    }
    file.close();
    //-----------------------------------
    file.setFileName("/sys/class/gpio/gpio41/value");
    if(file.exists())
    {
        if(file.open(QIODevice::WriteOnly))
        {
            file.write(temp);

        }
    }
    file.close();
    //-----------------------------------
    file.setFileName("/sys/class/gpio/gpio36/value");
    if(file.exists())
    {
        if(file.open(QIODevice::WriteOnly))
        {
            file.write(temp);

        }
    }
    file.close();
    //-----------------------------------
    file.setFileName("/sys/class/gpio/gpio42/value");
    if(file.exists())
    {
        if(file.open(QIODevice::WriteOnly))
        {
            file.write(temp);

        }
    }
    if(flash)
        //ui->listWidget->insertItem(0,"output high");
        listView("输出高");
    else
        //ui->listWidget->insertItem(0,"output low");
        listView("输出低");
    file.close();
    //-----------------------------------


}

void MainWindow::on_pushButton_6_clicked()//以太网检测
{
    QString cmd1 = tr("udhcpc -b -i eth0");
    listView("test wifi,plase wait...");
    system(cmd1.toStdString().c_str());
    timer_eth0 ->start(1000);
    disconnect(timer_eth0,&QTimer::timeout,this,&MainWindow::time_eth0_over);
    connect(timer_eth0,&QTimer::timeout,this,&MainWindow::time_eth0_over);
}
void MainWindow::time_eth0_over()
{

    FILE *fp;
    QString string;
    int size;
    int i = 0;
    int ok = 0;
    char output[64];
    fp = popen("ping -I eth0 www.163.com -w 2","r");

    timer_eth0->stop();
    if(fp)
    {
        string.clear();
        ok = 0;
        for(i = 0;i < 1000;i++)
        {
            fgets(output,50,fp);
            string = output;
            ok += string.size();
            listView(string);
            if(feof(fp))
            {
                break;
            }
        }
        string.sprintf("%d",ok);
        listView(string);
#if 1
        if(ok  < 300)
        {
            //ui->listWidget->insertItem(i,"wifi失败");
            listView("以太网失败");
            autofail();
        }
        else
        {
            listView("以太网成功");
            combutton(1,ui->checkBox_eth);
            respon();
        }
#endif


    }

}


void MainWindow::on_pushButton_14_clicked()//io输出输入检测
{
    disconnect(ioDect,&io_dect_ctrl::resultReady,this,&MainWindow::listView);
    connect(ioDect,&io_dect_ctrl::resultReady,this,&MainWindow::listView);


    disconnect(ioDect,&io_dect_ctrl::result1,this,&MainWindow::io_rsult);
    connect(ioDect,&io_dect_ctrl::result1,this,&MainWindow::io_rsult);


    if(!ioDect->isRunning())
        ioDect->start();
}
void MainWindow::respon()
{
    if(auto_switch)
    {
        auto_mode++;
        emit ui->pushButton_10->click();
    }

}
void MainWindow::autofail()
{
    if(auto_switch)
    {
        //auto_mode = 0xff;
        auto_mode ++;
        emit ui->pushButton_10->click();
    }

}
void MainWindow::io_rsult(char flag)
{
    if(flag)
    {

       combutton(1,ui->checkBox_in);
       combutton(1,ui->checkBox_out);
       respon();
    }
    else
    {
        autofail();
    }

}

void MainWindow::on_pushButton_10_clicked()//zidonghua
{
    if(auto_switch)
    {
        if(0xff == auto_mode)
        {
            auto_mode = 0;
            listView("auto fail");
        }
        else if(2 == auto_mode)
        {
            emit ui->pushButton_2->click();//485
        }
        else if(3 == auto_mode)
        {
            emit ui->pushButton_3->click();//can1
        }
        else if(4 == auto_mode)
        {
            emit ui->pushButton_4->click();//can2
        }
        else if(5 == auto_mode)
        {
            emit ui->pushButton_5->click();//can3
        }
        else if(6 == auto_mode)
        {
            emit ui->pushButton_6->click();//yitaiwang
        }
        else if(7 == auto_mode)
        {
            emit ui->pushButton_8->click();//wifi
        }
        else if(8 == auto_mode)
        {
            emit ui->pushButton_14->click();//churu
        }
        else if(9 == auto_mode)
        {
            emit ui->pushButton_15->click();//sd
        }
        else if(10 == auto_mode)
        {
            emit ui->pushButton_16->click();//bt
        }
        else if(11 == auto_mode)
        {
            listView("ceshi_end......");
        }

    }
    else
    {
        auto_switch = 1;
        listView("auto start");
        auto_mode = 1;
        emit ui->pushButton->click();//232
    }



}

void MainWindow::on_pushButton_15_clicked()//sd
{
        FILE *fp;
        char buf[50] = {0};
        unsigned flag = 0;
        QString string;
        fp = popen("ls /media/sdcard0","r");
        if(fp)
        {
            if(fgets(buf,50,fp))
            {
                string = buf;
            }
        }
        pclose(fp);
        //-----------------
        if(0 != string.size())
        {
            listView(string);
            listView("0");
            flag++;


        }
        //-----------------
        string.clear();
        fp = popen("ls /media/sdcard1","r");
        if(fp)
        {
            if(fgets(buf,50,fp))
            {
                string = buf;

            }
        }
        pclose(fp);
        //-------------------
        if(0 != string.size())
        {
            listView(string);
            listView("1");
            flag++;
        }
        string.clear();
        string.clear();
        fp = popen("ls /media/sdcard2","r");
        if(fp)
        {
            if(fgets(buf,50,fp))
            {
                string = buf;

            }
        }
        pclose(fp);
        //------------------------
        if(0 != string.size())
        {
            listView(string);
            listView("2");
            flag++;
        }
        if(flag)
        {
            listView("sd success");
            combutton(1,ui->checkBox_sd);
            respon();
        }
        else
        {
            listView("sd fail");
            autofail();
        }
        string.clear();
}
void MainWindow::bt_result()
{

    if(!timer_bt->isActive())
    {
        timer_bt->start(50);
    }
    disconnect(timer_bt,&QTimer::timeout,this,&MainWindow::timebtover);
    connect(timer_bt,&QTimer::timeout,this,&MainWindow::timebtover);
}
void MainWindow::timebtover()
{
    timer_bt->stop();

    if(!bt_flag)
    {
        listView("bt fail");
        autofail();
    }
}

void MainWindow::searchForDevices()
{
    /* 先清空 */
    //listWidget->clear();
    qDebug() << "search for devices!";
    if (remoteSelector) {
        delete remoteSelector;
        remoteSelector = NULL;
    }

    QBluetoothAddress adapter = QBluetoothAddress();
    remoteSelector = new RemoteSelector(adapter, this);

    connect(remoteSelector,
            SIGNAL(newServiceFound(QListWidgetItem*)),
            this, SLOT(newServiceFound(QListWidgetItem*)));

    remoteSelector->m_discoveredServices.clear();
    remoteSelector->startDiscovery(QBluetoothUuid(serviceUuid));
    connect(remoteSelector, SIGNAL(finished()),
            this, SIGNAL(discoveryFinished()));
    disconnect(remoteSelector,&RemoteSelector::finished,this,&MainWindow::bt_result);
    connect(remoteSelector,&RemoteSelector::finished,this,&MainWindow::bt_result);
}
void MainWindow::on_pushButton_16_clicked()//lanya search
{
    static int st = 0;
    bt_flag = 0;
    if(!st)
    {
        st = 1;
        localAdapterInit();
    }


    listView("lanya_name:"+localName);
    searchForDevices();
}
void MainWindow::newServiceFound(QListWidgetItem* item)//lanya device display
{
#if 0
    listWidget->addItem(item);

    /* 设置当前项 */
    listWidget->setCurrentRow(listWidget->count() - 1);
#endif
    //int cnt = ui->listWidget->count()+1;
    bt_flag = 1;
    ui->listWidget->addItem(item);
    ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
    ui->listWidget->scrollToBottom();
    listView("lanya ok");
    combutton(1,ui->checkBox_bt);
    respon();
}

