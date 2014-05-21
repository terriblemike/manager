#include "pbmsdport.h"

PBMSDPort::PBMSDPort(QObject *parent) :
    QObject(parent)
{
    //Сбрасываем признак
    portSettings = false;   //установки параметров порта

    //подключаем сигнал от порта о получении ответа к функции обработки
    connect(&port, SIGNAL(response(QString)), this, SIGNAL(response(QString)));
}

PBMSDPort::~PBMSDPort()
{
    //останавливаем работу порта
    port.stop();
}

//Функция для задания настроек последовательного порта
bool PBMSDPort::setPortSettings(QString &portName, int baudRate, int dataBits, int parity, int stopBits, int flowControl)
{
    portSettings = true;
    port.SetComPort(portName);
    if (!port.SetBaudRate(baudRate))
        portSettings = false;
    if (!port.SetDataBits(dataBits))
        portSettings = false;
    if (!port.SetParity(parity))
        portSettings = false;
    if (!port.SetStopBits(stopBits))
        portSettings = false;
    if (!port.SetFlowControl(flowControl))
        portSettings = false;
    return portSettings;
}

//Функция отправки команды
void PBMSDPort::sendCommand(QString command)
{
    if (portSettings)
        port.transaction(command);
}
