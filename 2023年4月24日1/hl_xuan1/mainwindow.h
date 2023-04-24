#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QCanBusDevice>
#include <QCanBus>
#include <QTimer>
#include <QThread>
#include "blue_thread.h"
#include "io_dect_ctrl.h"
#include <QCheckBox>
//----bluetooth----------------------
#include <qbluetoothserviceinfo.h>
#include <qbluetoothsocket.h>
#include <qbluetoothhostinfo.h>
#include <remoteselector.h>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void combutton(char flag ,QCheckBox *checkBox);
    void io_rsult(char flag);
    void localAdapterInit();
    void searchForDevices();
    //---bluetooth ver-------------


    /* 多个客户端 */


    /* 远程选择器，使用本地蓝牙去搜索蓝牙，可过滤蓝牙等 */
    RemoteSelector *remoteSelector;

    /* 本地蓝牙 */
    QList<QBluetoothHostInfo> localAdapters;

    /* 本地蓝牙名称 */
    QString localName;


private slots:
    void on_pushButton_clicked();
    void serialPortReadyRead();
    void serialPortReadyRead_485();
    void receivedFrames();
    void receivedFrames1();
    void receivedFrames2();
    void time_over();
    void time_wifi_over();
    void time_eth0_over();
    void time232over();
    void timebtover();
    void time_485_over();
    void newServiceFound(QListWidgetItem*);
    void blue_string(QString string);
    void bt_result();
    void listView(QString string);
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();


    

    
    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_10_clicked();
    void respon();
    void autofail();

    void on_pushButton_15_clicked();

    void on_pushButton_16_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serialPort;//232
    QSerialPort *serialPort_485;//485
    QCanBusDevice *canDevice;//can0
    QCanBusDevice *canDevice1;//can1
    QCanBusDevice *canDevice2;//can2
    QTimer *timer;
    QTimer *timer_232;
    QTimer *timer_485;
    QTimer *timer_wifi;
    QTimer *timer_eth0;
    QTimer *timer_bt;
    blue_thread *workThread;
    io_dect_ctrl *ioDect;
    char bt_flag = 0;
    char auto_mode = 0;
    char auto_switch = 0;

};
#endif // MAINWINDOW_H


