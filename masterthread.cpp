/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "masterthread.h"

#include <QtSerialPort/QSerialPort>

#include <QTime>

#include <QDialog>

QT_USE_NAMESPACE

MasterThread::MasterThread(QObject *parent)
    : QThread(parent), quit(false)
{
}

//! [0]
MasterThread::~MasterThread()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}
//! [0]

//! [1] //! [2]
void MasterThread::transaction(const QString &request)
{
    //! [1]
    QMutexLocker locker(&mutex);
//    this->portName = portName;
//    this->waitTimeout = waitTimeout;
    this->request = request;
    //! [3]
    if (!isRunning())
        start();
    else
        cond.wakeOne();
}
//! [2] //! [3]

//! [4]
void MasterThread::run()
{
    bool currentPortNameChanged = false;
    mutex.lock();
    //! [4] //! [5]
    QString currentPortName;
    quint32 currentBaudRate;
    QSerialPort::DataBits currentDataBits;
    QSerialPort::Parity currentParity;
    QSerialPort::StopBits currentStopBits;
    QSerialPort::FlowControl currentFlowControl;
    if ((currentPortName != portName) ||
        (currentBaudRate != baudRate) ||
        (currentDataBits != dataBits) ||
        (currentParity != parity) ||
        (currentStopBits != stopBits) ||
        (currentFlowControl != flowControl))
    {
        currentPortName = portName;
        currentBaudRate = baudRate;
        currentDataBits = dataBits;
        currentParity = parity;
        currentStopBits = stopBits;
        currentFlowControl = flowControl;
        currentPortNameChanged = true;
    }

    QString currentRequest = request;
    mutex.unlock();
    //! [5] //! [6]
    QSerialPort serial;



    while (!quit) {
        //![6] //! [7]
        if (currentPortNameChanged) {
            serial.close();
            serial.setPortName(currentPortName);

            if (!serial.open(QIODevice::ReadWrite)) {
                emit error(tr("Не удалось подключиться к порту %1, код ошибки %2")
                           .arg(portName).arg(serial.error()));
                return;
            }

//            if (!serial.setBaudRate(QSerialPort::Baud9600)) {
            if (!serial.setBaudRate(currentBaudRate)) {
                emit error(tr("Не удалось установить скорость для порта %1, код ошибки %2")
                           .arg(portName).arg(serial.error()));
                return;
            }

//            if (!serial.setDataBits(QSerialPort::Data8)) {
            if (!serial.setDataBits(currentDataBits)) {
                emit error(tr("Не удалось установить количество бит для порта %1, код ошибки %2")
                           .arg(portName).arg(serial.error()));
                return;
            }

//            if (!serial.setParity(QSerialPort::NoParity)) {
            if (!serial.setParity(currentParity)) {
                emit error(tr("Не удалось установить четность для порта %1, код ошибки %2")
                           .arg(portName).arg(serial.error()));
                return;
            }

//            if (!serial.setStopBits(QSerialPort::OneStop)) {
            if (!serial.setStopBits(currentStopBits)) {
                emit error(tr("Не удалось установить стоповый бит для порта %1, код ошибки %2")
                           .arg(portName).arg(serial.error()));
                return;
            }

//            if (!serial.setFlowControl(QSerialPort::NoFlowControl)) {
            if (!serial.setFlowControl(currentFlowControl)) {
                emit error(tr("Не удалось установить контроль четности для порта %1, код ошибки %2")
                           .arg(portName).arg(serial.error()));
                return;
            }
        }
        QByteArray requestData = 0;

        StringToByteArray(currentRequest, requestData);
        serial.write(requestData);
        if (serial.waitForBytesWritten(WAIT_TIMEOUT)) {
//             read response
            if (serial.waitForReadyRead(WAIT_TIMEOUT)) {
                QByteArray responseData = serial.readAll();
                while (serial.waitForReadyRead(10))
                    responseData += serial.readAll();

                QString response(responseData);
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
        if ((currentPortName != portName) ||
            (currentBaudRate != baudRate) ||
            (currentDataBits != dataBits) ||
            (currentParity != parity) ||
            (currentStopBits != stopBits) ||
            (currentFlowControl != flowControl))
        {
            currentPortName = portName;
            currentBaudRate = baudRate;
            currentDataBits = dataBits;
            currentParity = parity;
            currentStopBits = stopBits;
            currentFlowControl = flowControl;
            currentPortNameChanged = true;
        }
        else {
            currentPortNameChanged = false;
        }
        currentRequest = request;
        mutex.unlock();
    }
    //! [13]
}

void MasterThread::SetComPort(const QString &value)
{
    portName = value;
}

void MasterThread::SetBaudRate(const QString &value)
{
    if (value.compare("1200") == 0)
        baudRate = QSerialPort::Baud1200;
    else if (value.compare("2400")  == 0)
        baudRate = QSerialPort::Baud2400;
    else if (value.compare("4800") == 0)
        baudRate = QSerialPort::Baud4800;
    else if (value.compare("9600") == 0)
        baudRate = QSerialPort::Baud9600;
    else if (value.compare("19200") == 0)
        baudRate = QSerialPort::Baud19200;
    else if (value.compare("38400") == 0)
        baudRate = QSerialPort::Baud38400;
    else if (value.compare("57600") == 0)
        baudRate = QSerialPort::Baud57600;
    else if (value.compare("115200") == 0)
        baudRate = QSerialPort::Baud115200;
    else
        baudRate = QSerialPort::Baud9600;
}

void MasterThread::SetDataBits(const QString &value)
{
    if (value.compare("5") == 0)
        dataBits = QSerialPort::Data5;
    else if (value.compare("6") == 0)
        dataBits = QSerialPort::Data6;
    else if (value.compare("7") == 0)
        dataBits = QSerialPort::Data7;
    else if (value.compare("8") == 0)
        dataBits = QSerialPort::Data8;
    else
        dataBits = QSerialPort::Data8;
}

void MasterThread::SetParity(const QString &value)
{
    if (value.compare("None") == 0)
        parity = QSerialPort::NoParity;
    else if (value.compare("Even") == 0)
        parity = QSerialPort::EvenParity;
    else if (value.compare("Odd") == 0)
        parity = QSerialPort::OddParity;
    else if (value.compare("Space") == 0)
        parity = QSerialPort::SpaceParity;
    else if (value.compare("Mark") == 0)
        parity = QSerialPort::MarkParity;
    else
        parity = QSerialPort::NoParity;
}

void MasterThread::SetStopBits(const QString &value)
{
    if (value.compare("1") == 0)
        stopBits = QSerialPort::OneStop;
    else if (value.compare("1.5") == 0)
        stopBits = QSerialPort::OneAndHalfStop;
    else if (value.compare("2") == 0)
        stopBits = QSerialPort::TwoStop;
    else
        stopBits = QSerialPort::OneStop;
}

void MasterThread::SetFlowControl(const QString &value)
{
    if (value.compare("None") == 0)
        flowControl = QSerialPort::NoFlowControl;
    else if (value.compare("Hardware") == 0)
        flowControl = QSerialPort::HardwareControl;
    else if (value.compare("Xon/Xoff") == 0)
        flowControl = QSerialPort::SoftwareControl;
    else
        flowControl = QSerialPort::NoFlowControl;
}

void MasterThread::StringToByteArray(QString &src, QByteArray &dst)
{
    for (int i = 0; i < src.size(); i++)
        dst[i] = src[i].cell();
}

void MasterThread::ByteArrayToString(QByteArray &src, QString &dst)
{
    for (int i = 0; i < src.size(); i++)
        dst[i] = QChar(src[i] & 0x7F);
}

