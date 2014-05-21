#include "pgsm40.h"

// Таблица значений контрольной суммы для старшего байта
unsigned char auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

// Таблица значений контрольной суммы для младшего байта
char auchCRCLo[] = {
    (char)0x00, (char)0xC0, (char)0xC1, (char)0x01, (char)0xC3,
    (char)0x03, (char)0x02, (char)0xC2, (char)0xC6, (char)0x06,
    (char)0x07, (char)0xC7, (char)0x05, (char)0xC5, (char)0xC4,
    (char)0x04, (char)0xCC, (char)0x0C, (char)0x0D, (char)0xCD,
    (char)0x0F, (char)0xCF, (char)0xCE, (char)0x0E, (char)0x0A,
    (char)0xCA, (char)0xCB, (char)0x0B, (char)0xC9, (char)0x09,
    (char)0x08, (char)0xC8, (char)0xD8, (char)0x18, (char)0x19,
    (char)0xD9, (char)0x1B, (char)0xDB, (char)0xDA, (char)0x1A,
    (char)0x1E, (char)0xDE, (char)0xDF, (char)0x1F, (char)0xDD,
    (char)0x1D, (char)0x1C, (char)0xDC, (char)0x14, (char)0xD4,
    (char)0xD5, (char)0x15, (char)0xD7, (char)0x17, (char)0x16,
    (char)0xD6, (char)0xD2, (char)0x12, (char)0x13, (char)0xD3,
    (char)0x11, (char)0xD1, (char)0xD0, (char)0x10, (char)0xF0,
    (char)0x30, (char)0x31, (char)0xF1, (char)0x33, (char)0xF3,
    (char)0xF2, (char)0x32, (char)0x36, (char)0xF6, (char)0xF7,
    (char)0x37, (char)0xF5, (char)0x35, (char)0x34, (char)0xF4,
    (char)0x3C, (char)0xFC, (char)0xFD, (char)0x3D, (char)0xFF,
    (char)0x3F, (char)0x3E, (char)0xFE, (char)0xFA, (char)0x3A,
    (char)0x3B, (char)0xFB, (char)0x39, (char)0xF9, (char)0xF8,
    (char)0x38, (char)0x28, (char)0xE8, (char)0xE9, (char)0x29,
    (char)0xEB, (char)0x2B, (char)0x2A, (char)0xEA, (char)0xEE,
    (char)0x2E, (char)0x2F, (char)0xEF, (char)0x2D, (char)0xED,
    (char)0xEC, (char)0x2C, (char)0xE4, (char)0x24, (char)0x25,
    (char)0xE5, (char)0x27, (char)0xE7, (char)0xE6, (char)0x26,
    (char)0x22, (char)0xE2, (char)0xE3, (char)0x23, (char)0xE1,
    (char)0x21, (char)0x20, (char)0xE0, (char)0xA0, (char)0x60,
    (char)0x61, (char)0xA1, (char)0x63, (char)0xA3, (char)0xA2,
    (char)0x62, (char)0x66, (char)0xA6, (char)0xA7, (char)0x67,
    (char)0xA5, (char)0x65, (char)0x64, (char)0xA4, (char)0x6C,
    (char)0xAC, (char)0xAD, (char)0x6D, (char)0xAF, (char)0x6F,
    (char)0x6E, (char)0xAE, (char)0xAA, (char)0x6A, (char)0x6B,
    (char)0xAB, (char)0x69, (char)0xA9, (char)0xA8, (char)0x68,
    (char)0x78, (char)0xB8, (char)0xB9, (char)0x79, (char)0xBB,
    (char)0x7B, (char)0x7A, (char)0xBA, (char)0xBE, (char)0x7E,
    (char)0x7F, (char)0xBF, (char)0x7D, (char)0xBD, (char)0xBC,
    (char)0x7C, (char)0xB4, (char)0x74, (char)0x75, (char)0xB5,
    (char)0x77, (char)0xB7, (char)0xB6, (char)0x76, (char)0x72,
    (char)0xB2, (char)0xB3, (char)0x73, (char)0xB1, (char)0x71,
    (char)0x70, (char)0xB0, (char)0x50, (char)0x90, (char)0x91,
    (char)0x51, (char)0x93, (char)0x53, (char)0x52, (char)0x92,
    (char)0x96, (char)0x56, (char)0x57, (char)0x97, (char)0x55,
    (char)0x95, (char)0x94, (char)0x54, (char)0x9C, (char)0x5C,
    (char)0x5D, (char)0x9D, (char)0x5F, (char)0x9F, (char)0x9E,
    (char)0x5E, (char)0x5A, (char)0x9A, (char)0x9B, (char)0x5B,
    (char)0x99, (char)0x59, (char)0x58, (char)0x98, (char)0x88,
    (char)0x48, (char)0x49, (char)0x89, (char)0x4B, (char)0x8B,
    (char)0x8A, (char)0x4A, (char)0x4E, (char)0x8E, (char)0x8F,
    (char)0x4F, (char)0x8D, (char)0x4D, (char)0x4C, (char)0x8C,
    (char)0x44, (char)0x84, (char)0x85, (char)0x45, (char)0x87,
    (char)0x47, (char)0x46, (char)0x86, (char)0x82, (char)0x42,
    (char)0x43, (char)0x83, (char)0x41, (char)0x81, (char)0x80, (char)0x40
};

PGSM40::PGSM40(PBMSDPort *port, int adr, QObject *parent) :
    QObject(parent)
{
    BMSDport = port;
    //Устанавливаем значения по умолчанию
    status = false;     //Статус устройства
    adress = adr;      //Адрес устройства
    speed = 0;          //Скорость
    uskorenie = 10;      //Ускорение
    tormogenie = 10;     //Торможение
    direction = 0;      //Направление
//    start = false;      //Признак запуска
}

//Функция задает новый адрес для устройства
void PGSM40::setAdress(int value)
{
    adress = value;
}

//Функция посылает команду для установки скорости
void PGSM40::newSpeed(int value)
{
    //запоминаем значение
    speed = value;
    //формируем строку    
    command[0] = adress;
    command[1] = GSM40_COMMAND_WRITE;
    command[2] = (GSM40_REGISTR_SPEED >> 8) & 0xFF;
    command[3] = GSM40_REGISTR_SPEED & 0xFF;
    if (direction == GSM40_DIRECTION_DOWN)
    {
        command[4] = (value >> 8) & 0xFF;
        command[5] = value & 0xFF;
    }
    else
    {
        //преобразуем значение в дополнительный код
        int newValue = ((~value) | 128) + 1;
        command[4] = (newValue >> 8) & 0xFF;
        command[5] = newValue & 0xFF;
    }
    //подсчет контрольной суммы
    int sseed = chekSumm(command, 6);
    command[6] = (sseed >> 8) & 0xFF;
    command[7] = sseed & 0xFF;

    //выдаем сигнал для посылки новой команды
    BMSDport->sendCommand(command);
    //небольшая пауза (даем время для отсылки команды)
    BMSDport->port.wait(40);
    qDebug() << "SPD " << (int)command[0].cell() << (int)command[1].cell() <<
        (int)command[2].cell() << (int)command[3].cell() <<
        (int)command[4].cell() << (int)command[5].cell() <<
        (int)command[6].cell() << (int)command[7].cell();
}

//Функция посылает команду для установки ускорения
void PGSM40::newUskorenie(int value)
{
    //формируем строку
    command[0] = adress;
    command[1] = GSM40_COMMAND_WRITE;
    command[2] = (GSM40_REGISTR_USKORENIE >> 8) & 0xFF;
    command[3] = GSM40_REGISTR_USKORENIE & 0xFF;
    command[4] = (value >> 8) & 0xFF;
    command[5] = value & 0xFF;

    //подсчет контрольной суммы
    int sseed = chekSumm(command, 6);
    command[6] = (sseed >> 8) & 0xFF;
    command[7] = sseed & 0xFF;
    //Запоминаем текстовую информацию о команде
//    currentCommand = COMMAND_SPEED;
    //запоминаем значение
    uskorenie = value;
    //выдаем сигнал для посылки новой команды
    BMSDport->sendCommand(command);
    //небольшая пауза (даем время для отсылки команды)
    BMSDport->port.wait(40);
    qDebug() << "USK " << (int)command[0].cell() << (int)command[1].cell() <<
        (int)command[2].cell() << (int)command[3].cell() <<
        (int)command[4].cell() << (int)command[5].cell() <<
        (int)command[6].cell() << (int)command[7].cell();
}

//Функция посылает команду для установки ускорения
void PGSM40::newTormogenie(int value)
{
    //формируем строку
    command[0] = adress;
    command[1] = GSM40_COMMAND_WRITE;
    command[2] = (GSM40_REGISTR_TORMOGENIE >> 8) & 0xFF;
    command[3] = GSM40_REGISTR_TORMOGENIE & 0xFF;
    command[4] = (value >> 8) & 0xFF;
    command[5] = value & 0xFF;

    //подсчет контрольной суммы
    int sseed = chekSumm(command, 6);
    command[6] = (sseed >> 8) & 0xFF;
    command[7] = sseed & 0xFF;
    //запоминаем значение
    tormogenie = value;
    //выдаем сигнал для посылки новой команды
    BMSDport->sendCommand(command);
    //небольшая пауза (даем время для отсылки команды)
    BMSDport->port.wait(40);
}

//Функция посылает команду для установки направления
void PGSM40::newDirection(int value)
{
    //запоминаем направление
    direction = value;
    //задаем текущую скорость с поправкой на новое направление
    newSpeed(speed);
    qDebug() << "DIR " << direction;
}

//Функция посылает команду для запроса состояния устройства
void PGSM40::newStatus()
{
    //формируем строку
    command[0] = adress;
    command[1] = GSM40_COMMAND_READ;
    command[2] = (GSM40_REGISTR_STATUS >> 8) & 0xFF;
    command[3] = GSM40_REGISTR_STATUS & 0xFF;
    command[4] = 0;
    command[5] = 1;

    //подсчет контрольной суммы
    int sseed = chekSumm(command, 6);
    command[6] = (sseed >> 8) & 0xFF;
    command[7] = sseed & 0xFF;
    //выдаем сигнал для посылки новой команды
    BMSDport->sendCommand(command);
    //небольшая пауза (даем время для отсылки команды)
    BMSDport->port.wait(40);
}

//Функция получения состояния устройства
bool PGSM40::getStatus()
{
    return status;
}

//Функция возвращает значение скорости
int PGSM40::getSpeed()
{
     return speed;
}

//Функция возвращает значение ускорения
int PGSM40::getUskorenie()
{
    return uskorenie;
}

//Функция возвращает значение торможения
int PGSM40::getTormogenie()
{
    return tormogenie;
}

//Функция возвращает значение направления
int PGSM40::getDirection()
{
    return direction;
}

//Функция для подсчета контрольной суммы
int PGSM40::chekSumm(QString command, int size)
{
    unsigned char uchCRCHi = 0xFF ; //старший байт контрольной суммы
    unsigned char uchCRCLo = 0xFF ; //младший байт контрольной суммы
    unsigned uIndex ; //индекс таблицы контрольных сумм
    //в цикле обработка всех символов посылки
    for (int i = 0; i < size; i++)
    {
        uIndex = uchCRCHi ^ command[i].cell() ; //вычисление индекса таблицы
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
        uchCRCLo = auchCRCLo[uIndex];
    }
    //формируем контрольную сумму и возвращаем из функции
    return (uchCRCHi << 8 | uchCRCLo) ;
}

//Функция парсинга ответа
void PGSM40::parseData(QString s)
{
    //копируем ответ во временную строку
    QString tmp = s;

    //если ответ пришел не от этого устройтсва, то прерываем функцию
    if(tmp[0].cell() != adress)
        return;

    //выставляем признак исправности
    status = true;

    //обрабатываем поступившие данные в зависимости от команды, на которую пришел ответ
    switch (tmp[1].cell()) {
    case GSM40_COMMAND_READ:
        if (tmp[4].cell() & 1)
            status = true;
        else
            status = false;
        break;
    case GSM40_COMMAND_WRITE:
        break;
    }
}




