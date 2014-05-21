#ifndef PBMSD_H
#define PBMSD_H

#include <QObject>
#include <QDebug>
#include <QTimer>

#include "pbmsdport.h"

//#define COMMAND_ADRESS_SET  "Установка адреса"
//#define COMMAND_ADRESS_STOP "Отбой установки адреса"
//#define COMMAND_SPEED       "Скорость"
//#define COMMAND_USKORENIE   "Ускорение"
//#define COMMAND_TORMOGENIE  "Торможение"
//#define COMMAND_DIRECTION   "Направление"
//#define COMMAND_STATUS      "Опрос состояния"
//#define COMMAND_START       "Запуск двигателя"
//#define COMMAND_STOP        "Вход в деж.режим"

#define COMMAND_CODE_ADRESS_SET  0xA0
#define COMMAND_CODE_ADRESS_STOP 0xA1
#define COMMAND_CODE_SPEED       0xA3
#define COMMAND_CODE_USKORENIE   0xA5
#define COMMAND_CODE_TORMOGENIE  0xA6
#define COMMAND_CODE_DIRECTION   0xA7
#define COMMAND_CODE_STATUS      0x50
#define COMMAND_CODE_START       0x51
#define COMMAND_CODE_STOP        0x52

class PBMSD : public QObject
{
    Q_OBJECT
public:
    explicit PBMSD(PBMSDPort *port, int adr = 0xFF, QObject *parent = 0);

    PBMSDPort *BMSDport;

    //Функция задает новый адрес для устройства
    void setAdress(int value);
    //Функция посылает команду для установки нового адреса устройству
    void newAdress(int value);
    //Функция посылает команду для отбоя установки адреса
    void newAbortAdress();
    //Функция посылает команду для установки скорости
    void newSpeed(int value);
    //Функция посылает команду для установки ускорения
    void newUskorenie(int value);
    //Функция посылает команду для установки ускорения
    void newTormogenie(int value);
    //Функция посылает команду для установки направления
    void newDirection(int value);
    //Функция посылает команду для запроса состояния устройства
    void newStatus();
    //Функция получения состояния устройства
    bool getStatus();
    //Функция посылает команду для запуска
    void newStart();
    //Функция посылает команду для установки
    void newStop();

    //Функция возвращает значение скорости
    int getSpeed();
    //Функция возвращает значение ускорения
    int getUskorenie();
    //Функция возвращает значение торможения
    int getTormogenie();
    //Функция возвращает значение направления
    int getDirection();

//    //Функция для получения текстового описания текущей команды
//    QString getCurrentCommand();
//    //Функция для получения текстового описания команды, на которую пришел ответ
//    QString getCurrentResponse();

public:
    //Статус устройства
    bool status;
    //Адрес устройства
    int adress;
    //Скорость
    int speed;
    //Ускорение
    int uskorenie;
    //Торможение
    int tormogenie;
    //Направление
    int direction;
    //Признак запуска
    bool start;
    //Команда для устройства
    QString command;
    //Текстовое описание текущей команды
    QString currentCommand;
    //Текстовое описание команды, на которую пришел ответ
    QString currentResponse;
    //Функция для подсчета контрольной суммы
    uchar chekSumm(uchar inData, uchar seed);

    QTimer *timerStatus;
signals:
    //сигнал подтверждения приема данных
    void newData(int code);
    //сигнал подтверждения приема данных
//    void newCommand(QString s);

private slots:
    //Функция парсинга ответа
    void parseData(QString s);
    //функция установки неисправности устройства
    void setStatusFalse();
};

#endif // PBMSD_H
