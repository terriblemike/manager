#ifndef PBMSDPORT_H
#define PBMSDPORT_H

#include <QObject>

#include "pserialport.h"
//#include "pbmsd.h"

class PBMSDPort : public QObject
{
    Q_OBJECT
public:
    explicit PBMSDPort(QObject *parent = 0);
    ~PBMSDPort();

    //Функция для задания настроек последовательного порта
    bool setPortSettings(QString &portName, int baudRate, int dataBits, int parity, int stopBits, int flowControl);
    //Объект для работы с компортом
    PSerialPort port;

private:
    //Статус порта
    bool status;
    //Признак установки параметров порта
    bool portSettings;

signals:
    //сигнал подтверждения приема данных
    void response(QString response);

public slots:
    //Функция посылки команды
    void sendCommand(QString command);
};

#endif // PBMSDPORT_H
