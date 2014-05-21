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

#ifndef SLAVETHREAD_H
#define SLAVETHREAD_H

//#define WAIT_TIMEOUT    1000

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QSerialPort>

//! [0]
class SlaveThread : public QThread
{
    Q_OBJECT

public:
    SlaveThread(QObject *parent = 0);
    ~SlaveThread();

    void startSlave();
    void run();

    void SetComPort(const QString &value);
    void SetBaudRate(const QString &value);
    void SetDataBits(const QString &value);
    void SetParity(const QString &value);
    void SetStopBits(const QString &value);
    void SetFlowControl(const QString &value);

signals:
    void request(const QString &s);
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

//    int waitTimeout;
    QMutex mutex;
    bool quit;
};
//! [0]

#endif // SLAVETHREAD_H
