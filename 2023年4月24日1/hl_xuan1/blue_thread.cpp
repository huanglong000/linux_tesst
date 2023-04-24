#include "blue_thread.h"
void blue_thread::run()
{
    int i = 0;
    char output[50];
    QString string;
    sleep(3);

    QString cmd1 = tr("echo 1 > /sys/class/rfkill/rfkill0/state");
    system(cmd1.toStdString().c_str());
    sleep(3);
    cmd1 = tr("brcm_patchram_plus1  --enable_hci --no2bytes --"
              "use_baudrate_for_download --tosleep 200000 --"
              "baudrate 1500000 --patchram /system/etc/firmware"
              "/bcm43438a0.hcd /dev/ttyS0 &");
    system(cmd1.toStdString().c_str());
    sleep(1);
    cmd1 = tr("hciconfig hci0 up");
    system(cmd1.toStdString().c_str());
    cmd1 = tr("hciconfig -a");
    system(cmd1.toStdString().c_str());
    sleep(1);
    cmd1 = tr("/usr/libexec/bluetooth/bluetoothd -C -n &");
    system(cmd1.toStdString().c_str());
    sleep(1);
    emit resultReady(string);
    #if 0
    cmd1 = tr("hciconfig hci0 iscan");
    system(cmd1.toStdString().c_str());
    fp1 = popen("hcitool scan","r");
    if(fp1)
    {
        string.clear();
        if(fgets(output,50,fp1))
        {
            string = output;
            emit resultReady(string);
        }
    }
    else
    {
        emit resultReady("file not exist");
    }


    cmd1 = tr("bluetoothctl");
    system(cmd1.toStdString().c_str());




    sleep(1);
    cmd1 = tr("power on");
    system(cmd1.toStdString().c_str());
    sleep(1);
    cmd1 = tr("agent on");
    system(cmd1.toStdString().c_str());
    sleep(1);
    cmd1 = tr("pairable on");
    system(cmd1.toStdString().c_str());
    sleep(1);
    cmd1 = tr("discoverable on");
    system(cmd1.toStdString().c_str());
    sleep(1);




    cmd1 = tr("scan on > /usr/blue.txt");
    system(cmd1.toStdString().c_str());
    sleep(3);
    cmd1 = tr("scan off");
    system(cmd1.toStdString().c_str());
    #endif

#if 0
    fp1 = popen("scan on","r");
    if(fp1)
    {
        string.clear();
        for(i = 0;i < 3;i++)
        {
            if(fgets(output,50,fp1))
            {
                //ok ++;
                string = output;
                emit resultReady(string);


            }
            else
            {
                string ="失败";
                emit resultReady(string);
            }


        }
        //if(ok  < 2)
        {
            //ui->listWidget->insertItem(i,"wifi失败");
        }


    }
#endif



}
