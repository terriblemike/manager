/****************************************************************************
**Файл реализации модуля класса для работы с последовательным портом
****************************************************************************/

#include "pserialport.h"

#include <QtSerialPort/QSerialPort>
#include <QTime>
#include <QDialog>

//QT_USE_NAMESPACE

PSerialPort::PSerialPort(QObject *parent)
    : QThread(parent), quit(false)
{
    reinitPort = true;
}

//! [0]
PSerialPort::~PSerialPort()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
//    wait();
}

void PSerialPort::transaction(const QString &request)
{
    this->request = request;
    if (!isRunning())
        start();
    else
        cond.wakeOne();
}

void PSerialPort::run()
{
    reinitPort = true;
    mutex.lock();
//    cond.wait(&mutex);
    QString currentRequest = request;
    mutex.unlock();

    QSerialPort serial;

    while (!quit) {

        if (reinitPort) {
            serial.close();
            mutex.lock();
//            cond.wait(&mutex);
            serial.setPortName(portName);

            if (!serial.open(QIODevice::ReadWrite)) {
                emit error(tr("Не удалось подключиться к порту %1, код ошибки %2")
                           .arg(portName).arg(serial.error()));
                return;
            }

            if (!serial.setBaudRate(baudRate)) {
                emit error(tr("Не удалось установить скорость для порта %1, код ошибки %2")
                           .arg(portName).arg(serial.error()));
                return;
            }

            if (!serial.setDataBits(dataBits)) {
                emit error(tr("Не удалось установить количество бит для порта %1, код ошибки %2")
                           .arg(portName).arg(serial.error()));
                return;
            }

            if (!serial.setParity(parity)) {
                emit error(tr("Не удалось установить четность для порта %1, код ошибки %2")
                           .arg(portName).arg(serial.error()));
                return;
            }

            if (!serial.setStopBits(stopBits)) {
                emit error(tr("Не удалось установить стоповый бит для порта %1, код ошибки %2")
                           .arg(portName).arg(serial.error()));
                return;
            }

            if (!serial.setFlowControl(flowControl)) {
                emit error(tr("Не удалось установить контроль четности для порта %1, код ошибки %2")
                           .arg(portName).arg(serial.error()));
                return;
            }
            reinitPort = false;
            mutex.unlock();
        }

        QByteArray requestData = 0;

        stringToByteArray(currentRequest, requestData);

        if (serial.write(requestData) == -1)
            reinitPort = true;
//        else
//            qDebug() << "Write" << requestData;
//        serial.write(requestData);
        if (serial.waitForBytesWritten(WAIT_WRITE_TIMEOUT)) {
            if (serial.waitForReadyRead(WAIT_READ_TIMEOUT)) {
                QByteArray responseData = serial.readAll();
                while (serial.waitForReadyRead(10))
                    responseData += serial.readAll();

                 QString response;
                 byteArrayToString(responseData, response);
                emit this->response(response);
            } else {
                emit timeout(tr("Время ожидания ответа истекло: %1")
                             .arg(QTime::currentTime().toString()));
            }
        } else {
            emit timeout(tr("Время ожидания записи истекло: %1")
                         .arg(QTime::currentTime().toString()));
        }
        mutex.lock();
        cond.wait(&mutex);
        currentRequest = request;
        mutex.unlock();
    }
    //! [13]
}

void PSerialPort::stop()
{
    mutex.lock();
//    cond.wait(&mutex);
    quit = true;
    reinitPort = true;
    mutex.unlock();
}

void PSerialPort::SetComPort(const QString &value)
{
    mutex.lock();
//    cond.wait(&mutex);
    portName = value;
    mutex.unlock();
}

bool PSerialPort::SetBaudRate(/*QSerialPort::BaudRate*/int value)
{
    if ((value == QSerialPort::Baud1200) ||
        (value == QSerialPort::Baud2400) ||
        (value == QSerialPort::Baud4800) ||
        (value == QSerialPort::Baud9600) ||
        (value == QSerialPort::Baud19200) ||
        (value == QSerialPort::Baud38400) ||
        (value == QSerialPort::Baud57600) ||
        (value == QSerialPort::Baud115200))
    {
        mutex.lock();
//        cond.wait(&mutex);
        baudRate = value;
        mutex.unlock();
        return true;
    }
    else
    {
        mutex.lock();
//        cond.wait(&mutex);
        baudRate = QSerialPort::Baud9600;
        mutex.unlock();
        return false;
    }
}

bool PSerialPort::SetDataBits(/*QSerialPort::DataBits*/int value)
{
    if ((value == QSerialPort::Data5) ||
        (value == QSerialPort::Data6) ||
        (value == QSerialPort::Data7) ||
        (value == QSerialPort::Data8))
    {
        mutex.lock();
//        cond.wait(&mutex);
        dataBits = QSerialPort::DataBits(value);
        mutex.unlock();
        return true;
    }
    else
    {
        mutex.lock();
//        cond.wait(&mutex);
        dataBits = QSerialPort::Data8;
        mutex.unlock();
        return false;
    }
}

bool PSerialPort::SetParity(/*QSerialPort::Parity*/int value)
{
    if ((value == QSerialPort::NoParity) ||
        (value == QSerialPort::EvenParity) ||
        (value == QSerialPort::OddParity) ||
        (value == QSerialPort::SpaceParity) ||
        (value == QSerialPort::MarkParity))
    {
        mutex.lock();
//        cond.wait(&mutex);
        parity = QSerialPort::Parity(value);
        mutex.unlock();
        return true;
    }
    else
    {
        mutex.lock();
//        cond.wait(&mutex);
        parity = QSerialPort::NoParity;
        mutex.unlock();
        return false;
    }
}

bool PSerialPort::SetStopBits(/*QSerialPort::StopBits*/int value)
{
    if ((value == QSerialPort::OneStop) ||
        (value == QSerialPort::OneAndHalfStop) ||
        (value == QSerialPort::TwoStop))
    {
        mutex.lock();
//        cond.wait(&mutex);
        stopBits = QSerialPort::StopBits(value);
        mutex.unlock();
        return true;
    }
    else
    {
        mutex.lock();
//        cond.wait(&mutex);
        stopBits = QSerialPort::OneStop;
        mutex.unlock();
        return false;
    }
}

bool PSerialPort::SetFlowControl(/*QSerialPort::FlowControl*/int value)
{    
    if ((value == QSerialPort::NoFlowControl) ||
        (value == QSerialPort::HardwareControl) ||
        (value == QSerialPort::SoftwareControl))
    {
        mutex.lock();
//        cond.wait(&mutex);
        flowControl = QSerialPort::FlowControl(value);
        mutex.unlock();
        return true;
    }
    else
    {
        mutex.lock();
//        cond.wait(&mutex);
        flowControl = QSerialPort::NoFlowControl;
        mutex.unlock();
        return false;
    }
}

void PSerialPort::stringToByteArray(QString &src, QByteArray &dst)
{
    for (int i = 0; i < src.size(); i++)
        dst[i] = src[i].cell();
}

void PSerialPort::byteArrayToString(QByteArray &src, QString &dst)
{
    for (int i = 0; i < src.size(); i++)
        dst[i] = uchar(src[i]);
}

