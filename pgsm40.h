#ifndef PGSM40_H
#define PGSM40_H

#include <QObject>
#include <QDebug>

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

//#define COMMAND_CODE_ADRESS_SET  0xA0
//#define COMMAND_CODE_ADRESS_STOP 0xA1
//#define COMMAND_CODE_SPEED       0xA3
//#define COMMAND_CODE_USKORENIE   0xA5
//#define COMMAND_CODE_TORMOGENIE  0xA6
//#define COMMAND_CODE_DIRECTION   0xA7
//#define COMMAND_CODE_STATUS      0x50
//#define COMMAND_CODE_START       0x51
//#define COMMAND_CODE_STOP        0x52

#define GSM40_COMMAND_WRITE         6
#define GSM40_COMMAND_READ          4

#define GSM40_DEFAULT_ADRESS        11

#define GSM40_REGISTR_SPEED         7102    //регистр задания скорости
#define GSM40_REGISTR_USKORENIE     7143
#define GSM40_REGISTR_TORMOGENIE    7144
#define GSM40_REGISTR_RIVOK         7145
#define GSM40_REGISTR_STATUS        7132
#define GSM40_REGISTR_CONTROL       7105

#define GSM40_SPEED_SHAG            0.1

#define GSM40_DEFAULT_USKORENIE     10
#define GSM40_DEFAULT_TORMOGENIE    10

//направление движения:
//вниз - точный актуатор должен выдвигаться, т.е. скорость со знаком +
//вверх - точный актуатор должен задвигаться, т.е. скорость со знаком -
#define GSM40_DIRECTION_DOWN        0
#define GSM40_DIRECTION_UP          1


class PGSM40 : public QObject
{
    Q_OBJECT
public:
    explicit PGSM40(PBMSDPort *port, int adr = 1, QObject *parent = 0);

    PBMSDPort *BMSDport;

    //Функция задает новый адрес для устройства
    void setAdress(int value);
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
    //Команда для устройства
    QString command;
//    //Текстовое описание текущей команды
//    QString currentCommand;
//    //Текстовое описание команды, на которую пришел ответ
//    QString currentResponse;
    //Функция для подсчета контрольной суммы
    int chekSumm(QString command, int size);
signals:
    //сигнал подтверждения приема данных
    void newData(int code);
    //сигнал подтверждения приема данных
//    void newCommand(QString s);

private slots:
    //Функция парсинга ответа
    void parseData(QString s);
};

#endif // PGSM40_H
