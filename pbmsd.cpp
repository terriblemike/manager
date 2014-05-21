#include "pbmsd.h"

PBMSD::PBMSD(PBMSDPort *port, int adr, QObject *parent) :
    QObject(parent)
{
    BMSDport = port;
    //Устанавливаем значения по умолчанию
    status = false;     //Статус устройства
    adress = adr;      //Адрес устройства
    speed = 0;          //Скорость
    uskorenie = 24;      //Ускорение
    tormogenie = 24;     //Торможение
    direction = 0;      //Направление
    start = false;      //Признак запуска

    //таймер для установки неисправности, если в течении 2 секунд не пришел ответ от устройства
    timerStatus = new QTimer();
    timerStatus->setInterval(2000);
    connect(timerStatus, SIGNAL(timeout()), this, SLOT(setStatusFalse()));
}

//Функция задает новый адрес для устройства
void PBMSD::setAdress(int value)
{
    adress = value;
}

//Функция посылает команду для установки адреса
void PBMSD::newAdress(int value)
{
    //переменная для подсчета контрольной суммы
    uchar sseed = 0;
    //формируем строку
    command[0] = 0xE6;
    command[1] = 0xFF;
    sseed = chekSumm(command[1].cell(), sseed);
    command[2] = 0xA0;
    sseed = chekSumm(command[2].cell(), sseed);
    command[3] = value;     //задаем новый адрес
    sseed = chekSumm(command[3].cell(), sseed);
    command[4] = sseed;     //подставляем высчитанную контрольную сумму
    //Запоминаем текстовую информацию о команде
//    currentCommand = COMMAND_ADRESS_SET;
    //запоминаем адрес
    adress = value;
    //выдаем сигнал для посылки новой команды
    BMSDport->sendCommand(command);
    //небольшая пауза (даем время для отсылки команды)
    BMSDport->port.wait(40);
}

//Функция посылает команду для отбоя установки адреса
void PBMSD::newAbortAdress()
{
    //переменная для подсчета контрольной суммы
    uchar sseed = 0;
    //формируем строку
    command[0] = 0xE6;
    command[1] = 0xFF;
    sseed = chekSumm(command[1].cell(), sseed);
    command[2] = 0xA1;
    sseed = chekSumm(command[2].cell(), sseed);
    command[3] = 0x1;
    sseed = chekSumm(command[3].cell(), sseed);
    command[4] = sseed;
    //Запоминаем текстовую информацию о команде
//    currentCommand = COMMAND_ADRESS_STOP;
    //выдаем сигнал для посылки новой команды
    BMSDport->sendCommand(command);
    //небольшая пауза (даем время для отсылки команды)
    BMSDport->port.wait(40);
}

//Функция посылает команду для установки скорости
void PBMSD::newSpeed(int value)
{
    //переменная для подсчета контрольной суммы
    uchar sseed = 0;
    //формируем строку
    command[0] = 0xE6;
    command[1] = adress;
    sseed = chekSumm(command[1].cell(), sseed);
    command[2] = 0xA3;
    sseed = chekSumm(command[2].cell(), sseed);
    command[3] = value;
    sseed = chekSumm(command[3].cell(), sseed);
    command[4] = sseed;
    //Запоминаем текстовую информацию о команде
//    currentCommand = COMMAND_SPEED;
    //запоминаем значение
    speed = value;
    //выдаем сигнал для посылки новой команды
    BMSDport->sendCommand(command);
    //небольшая пауза (даем время для отсылки команды)
    BMSDport->port.wait(40);
}

//Функция посылает команду для установки ускорения
void PBMSD::newUskorenie(int value)
{
    //переменная для подсчета контрольной суммы
    uchar sseed = 0;
    //формируем строку
    command[0] = 0xE6;
    command[1] = adress;
    sseed = chekSumm(command[1].cell(), sseed);
    command[2] = 0xA5;
    sseed = chekSumm(command[2].cell(), sseed);
    command[3] = value;
    sseed = chekSumm(command[3].cell(), sseed);
    command[4] = sseed;
    //Запоминаем текстовую информацию о команде
//    currentCommand = COMMAND_USKORENIE;
    //запоминаем значение
    uskorenie = value;
    //выдаем сигнал для посылки новой команды
    BMSDport->sendCommand(command);
    //небольшая пауза (даем время для отсылки команды)
    BMSDport->port.wait(40);
}

//Функция посылает команду для установки ускорения
void PBMSD::newTormogenie(int value)
{
    //переменная для подсчета контрольной суммы
    uchar sseed = 0;
    //формируем строку
    command[0] = 0xE6;
    command[1] = adress;
    sseed = chekSumm(command[1].cell(), sseed);
    command[2] = 0xA6;
    sseed = chekSumm(command[2].cell(), sseed);
    command[3] = value;
    sseed = chekSumm(command[3].cell(), sseed);
    command[4] = sseed;
    //Запоминаем текстовую информацию о команде
//    currentCommand = COMMAND_TORMOGENIE;
    //запоминаем значение
    tormogenie = value;
    //выдаем сигнал для посылки новой команды
    BMSDport->sendCommand(command);
    //небольшая пауза (даем время для отсылки команды)
    BMSDport->port.wait(40);
}

//Функция посылает команду для установки направления
void PBMSD::newDirection(int value)
{
    //переменная для подсчета контрольной суммы
    uchar sseed = 0;
    //формируем строку
    command[0] = 0xE6;
    command[1] = adress;
    sseed = chekSumm(command[1].cell(), sseed);
    command[2] = 0xA7;
    sseed = chekSumm(command[2].cell(), sseed);
    command[3] = value;
    sseed = chekSumm(command[3].cell(), sseed);
    command[4] = sseed;
    //Запоминаем текстовую информацию о команде
//    currentCommand = COMMAND_DIRECTION;
    //запоминаем значение
    direction = value;
    //выдаем сигнал для посылки новой команды
    BMSDport->sendCommand(command);
    //небольшая пауза (даем время для отсылки команды)
    BMSDport->port.wait(40);
}

//Функция посылает команду для запроса состояния устройства
void PBMSD::newStatus()
{
    //переменная для подсчета контрольной суммы
    uchar sseed = 0;
    //формируем строку
    command[0] = 0xE6;
    command[1] = adress;
    sseed = chekSumm(command[1].cell(), sseed);
    command[2] = 0x50;
    sseed = chekSumm(command[2].cell(), sseed);
    command[3] = 0x1;
    sseed = chekSumm(command[3].cell(), sseed);
    command[4] = sseed;
    //Запоминаем текстовую информацию о команде
//    currentCommand = COMMAND_STATUS;
    //выдаем сигнал для посылки новой команды
    BMSDport->sendCommand(command);
    //небольшая пауза (даем время для отсылки команды)
    BMSDport->port.wait(40);
}

//Функция получения состояния устройства
bool PBMSD::getStatus()
{
    //запускаем таймер отсчета времени получения ответа от устройства
    timerStatus->start();
    //отсылаем команду для получения статуса устройства
    newStatus();
    //возвращаем статус устройства (возможно о неисправности будет сообщено только при следующем запроса)
    return status;
}

//Функция посылает команду для запуска
void PBMSD::newStart()
{
    newUskorenie(uskorenie);
    newTormogenie(tormogenie);
    newSpeed(speed);
    newDirection(direction);

    //переменная для подсчета контрольной суммы
    uchar sseed = 0;
    //формируем строку
    command[0] = 0xE6;
    command[1] = adress;
    sseed = chekSumm(command[1].cell(), sseed);
    command[2] = 0x51;
    sseed = chekSumm(command[2].cell(), sseed);
    command[3] = 0x1;
    sseed = chekSumm(command[3].cell(), sseed);
    command[4] = sseed;
    //Запоминаем текстовую информацию о команде
//    currentCommand = COMMAND_START;
    //устанавливаем признак запуска
    start = true;
    //выдаем сигнал для посылки новой команды
    BMSDport->sendCommand(command);
    //небольшая пауза (даем время для отсылки команды)
    BMSDport->port.wait(40);
//    qDebug() << "START BMSD" << adress;
}

//Функция посылает команду для установки
void PBMSD::newStop()
{
    //переменная для подсчета контрольной суммы
    uchar sseed = 0;
    //формируем строку
    command[0] = 0xE6;
    command[1] = adress;
    sseed = chekSumm(command[1].cell(), sseed);
    command[2] = 0x52;
    sseed = chekSumm(command[2].cell(), sseed);
    command[3] = 0x1;
    sseed = chekSumm(command[3].cell(), sseed);
    command[4] = sseed;
    //Запоминаем текстовую информацию о команде
//    currentCommand = COMMAND_STOP;
    //сбрасываем признак запуска
    start = false;
    //выдаем сигнал для посылки новой команды
    BMSDport->sendCommand(command);
    //небольшая пауза (даем время для отсылки команды)
    BMSDport->port.wait(40);
}

//Функция возвращает значение скорости
int PBMSD::getSpeed()
{
     return speed;
}

//Функция возвращает значение ускорения
int PBMSD::getUskorenie()
{
    return uskorenie;
}

//Функция возвращает значение торможения
int PBMSD::getTormogenie()
{
    return tormogenie;
}

//Функция возвращает значение направления
int PBMSD::getDirection()
{
    return direction;
}

////Функция для получения текстового описания текущей команды
//QString PBMSD::getCurrentCommand()
//{
//    return currentCommand;
//}

////Функция для получения текстового описания команды, на которую пришел ответ
//QString PBMSD::getCurrentResponse()
//{
//    return currentResponse;
//}

//Функция для подсчета контрольной суммы
uchar PBMSD::chekSumm(uchar inData, uchar seed)
{
    //служебные переменные
    uchar bitsLeft;
    uchar temp;

    //в цикле проходим по всем битам
    for (bitsLeft = 8; bitsLeft > 0; bitsLeft--)
    {
        //производим побитовые операции согласно описанию на устройство
        temp = ((seed ^ inData) & 0x01);
        if (temp == 0)
        {
            seed >>= 1;
        }
        else
        {
            seed ^= 0x18;
            seed >>= 1;
            seed |= 0x80;
        }
        inData >>= 1;
    }
    //возвращаем полученную контрольную сумму
    return seed;
}

//Функция парсинга ответа
void PBMSD::parseData(QString s)
{
    //копируем ответ во временную строку
    QString tmp = s;

    //если пришел ответ от текущего устройства
    if(tmp[1].cell() == adress)
        //останавливаем таймер для установки неисправности
        timerStatus->stop();



    //обрабатываем поступившие данные в зависимости от команды, на которую пришел ответ
//    switch (tmp[1].cell()) {
//    case COMMAND_CODE_ADRESS_SET:
//        currentResponse = COMMAND_ADRESS_SET;
//        emit newData(COMMAND_CODE_ADRESS_SET);
//        break;
//    case COMMAND_CODE_SPEED:
//        currentResponse = COMMAND_SPEED;
//        emit newData(COMMAND_CODE_SPEED);
//        break;
//    case COMMAND_CODE_USKORENIE:
//        currentResponse = COMMAND_USKORENIE;
//        emit newData(COMMAND_CODE_USKORENIE);
//        break;
//    case COMMAND_CODE_TORMOGENIE:
//        currentResponse = COMMAND_TORMOGENIE;
//        emit newData(COMMAND_CODE_TORMOGENIE);
//        break;
//    case COMMAND_CODE_DIRECTION:
//        currentResponse = COMMAND_DIRECTION;
//        emit newData(COMMAND_CODE_DIRECTION);
//        break;
//    case COMMAND_CODE_START:
//        currentResponse = COMMAND_START;
//        emit newData(COMMAND_CODE_START);
//        break;
//    case COMMAND_CODE_STOP:
//        currentResponse = COMMAND_STOP;
//        emit newData(COMMAND_CODE_STOP);
//        break;
//    default:
//        currentResponse = COMMAND_STATUS;
//        emit newData(COMMAND_CODE_STATUS);
//        break;
    //    }
}

//функция установки неисправности устройства
void PBMSD::setStatusFalse()
{
    //устанавливаем признак неисправности
    status = false;
    //останавливаем таймер отсчета времени ответа
    timerStatus->stop();
}




