#include "io_dect_ctrl.h"
#include <QFile>

void io_dect_ctrl::run()
{
    QString string;
    QByteArray tmp;
    int i;
    QByteArray result[4] =
    {
    #if 0
        {"1000"},
        {"0100"},
        {"0010"},
        {"0001"},
    #endif
        {"0111"},
        {"1011"},
        {"1101"},
        {"1110"},
    };
    for( i= 0;i < 4;i++)
    {
        io_set(1<<i);
        msleep(1);
        io_read();
        tmp = io_read();
        if(tmp != result[i])
        {
            break;
        }

    }
    if(i == 4)
    {
        string = "IO正常";
        emit result1(1);

    }
    else
    {

        string= "理论值："+result[i]+"实际值："+tmp;
        emit result1(0);
    }
    //emit resultReady("1");
    emit resultReady(string);
}
void io_dect_ctrl::io_set(char flag)
{
    QFile file1,file2;
    QString string[4] =
    {
        {"/sys/class/gpio/gpio40/value"},
        {"/sys/class/gpio/gpio41/value"},
        {"/sys/class/gpio/gpio36/value"},
        {"/sys/class/gpio/gpio42/value"},
    };
    QByteArray tmp[4];
    tmp[0].clear();
    tmp[1].clear();
    tmp[1].clear();
    tmp[3].clear();

    if(flag &(1<<0))
    {
        tmp[0] += '1';
    }
    else
    {
        tmp[0] += '0';
    }
    if(flag &(1<<1))
    {
        tmp[1] += '1';
    }
    else
    {
        tmp[1] += '0';
    }
    if(flag &(1<<2))
    {
        tmp[2] += '1';
    }
    else
    {
        tmp[2] += '0';
    }
    if(flag &(1<<3))
    {
        tmp[3] += '1';
    }
    else
    {
        tmp[3] += '0';
    }

    for(int i = 0;i < 4;i++)
    {
        file2.setFileName(string[i]);//chu
        if(file2.exists())
        {
            if(file2.open(QIODevice::WriteOnly))
            {

                file2.write(tmp[i]);

            }
        }
        file2.close();
    }


}
QByteArray io_dect_ctrl::io_read()
{
    QByteArray tmp;
    QString buf[4] =
    {
        {"/sys/class/gpio/gpio132/value"},
        {"/sys/class/gpio/gpio133/value"},
        {"/sys/class/gpio/gpio126/value"},
        {"/sys/class/gpio/gpio127/value"},
    };
    QFile file1;
    for(int i = 0;i < 4;i++)
    {
        file1.setFileName(buf[i]);
        if(file1.exists())
        {
            if(file1.open(QIODevice::ReadOnly))
            {

                tmp += file1.read(1);

            }
        }
        file1.close();
    }
    return tmp;
}
