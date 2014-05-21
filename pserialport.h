/****************************************************************************
**Заголовочный файл модуля класса для работы с последовательным портом
****************************************************************************/

#ifndef PSERIALPORT_H
#define PSERIALPORT_H

#define WAIT_WRITE_TIMEOUT  100
#define WAIT_READ_TIMEOUT   1000

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QSerialPort>
#include <QDebug>

//! [0]
class PSerialPort : public QThread
{
    Q_OBJECT

public:
    PSerialPort(QObject *parent = 0);
    ~PSerialPort();

    void transaction(const QString &request);
    void run();
    void stop();
    void SetComPort(const QString &value);
    bool SetBaudRate(int value);
    bool SetDataBits(int value);
    bool SetParity(int value);
    bool SetStopBits(int value);
    bool SetFlowControl(int value);

    void stringToByteArray(QString &src, QByteArray &dst);
    void byteArrayToString(QByteArray &src, QString &dst);
signals:
    void response(const QString &s);
    void error(const QString &s);
    void timeout(const QString &s);

private:
    //парамерты СОМ порта
    QString portName;

    quint32 baudRate;   //скорость
    QSerialPort::DataBits dataBits;   //количество бит
    QSerialPort::Parity parity;     //четность
    QSerialPort::StopBits stopBits;   //стоповые биты
    QSerialPort::FlowControl flowControl;    //контроль четности

    bool reinitPort;
    QString request;
//    int waitTimeout;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;
};
//! [0]

#endif // SerialPort_H
