#ifndef PPOTENCIOMETR_H
#define PPOTENCIOMETR_H

#include <QObject>
#include <QTimer>
#include <math.h>

//Подключаем заголовочный файл, поставляемый разработчиком
#include "bdaqctrl.h"

//Подключаем пространство имен, поставляемое разработчиком платы
using namespace Automation::BDaq;

#define DEFAULT_IND_DEVICE  2   //номер платы в системе по умолчанию

#define DEFAULT_PERIOD      10  //количество опросов в секунду
#define DEFAULT_DLINA       100 //ход актуатора по умолчанию 100 мм

#define DEFAULT_VOL_BEGIN   0.500  //начальное значение напряжения на потенциометре в В
#define DEFAULT_VOL_END   3.0  //конечное значение напряжения на потенциометре в В

#define DEFAULT_VOL_ON_MM   ((DEFAULT_VOL_END - DEFAULT_VOL_BEGIN) / DEFAULT_DLINA)  //шаг изменения напряжение на 1 мм

class PPotenciometr : public QObject
{
    Q_OBJECT
public:
    explicit PPotenciometr(QObject *parent = 0);
    ~PPotenciometr();
    //Функция получения статуса данных
    bool getStatus();
    //Функция инициализации параметров потенциометра: номера платы в системе, к которой подключен, и номер канала
    bool Init(int ind, int chan);
    //Функция установки количества опросов в секунду
    void setQPS(int value);
    //Функция запуска опроса
    void start();
    //Функция остановки опроса
    void stop();
    //Фуункции получения позиции выдвинутой части
    int getPos();
    //Функция задания сигнального положения
    void setTargetPos(int value);
    //Функция задания крайнелевого сигнального положения
    void setAlrLeftPos(int value);
    //Функция задания крайнеправого сигнального положения
    void setAlrRightPos(int value);

    //Функция сброса сигнального положения
    void resetTargetPos();
    //Функция сброса крайнелевого сигнального положения
    void resetAlrLeftPos();
    //Функция сброса крайнеправого сигнального положения
    void resetAlrRightPos();

private:
    //Статус данных
    bool status;
    //Признак инициализации параметров потенциометра
    bool settings;
    //Признак установки сигнального положения
    bool target;
    //Признак установки крайнелевого сигнального положения
    bool alrLeft;
    //Признак установки крайнеправого сигнального положения
    bool alrRight;

    //Признак достижения сигнального положения
    bool targetSignal;
    //Признак достижения крайнелевого сигнального положения
    bool alrLeftSignal;
    //Признак достижения крайнеправого сигнального положения
    bool alrRightSignal;

    //Количество опроса  в секунду
    int QPS;
    //Номер устройства в системе
    int indDevice;
    //Номер канала на устройстве
    int channel;
    //Значение напряжения на канале
    double volume;
    //Положение выдвинутой части в мм
    int pos;
    int prevPos;

    //Положение, при котором происходит посылка сигнала
    int targetP;
    //Крайнее левое положение энкодера, при котором происходит обязательная посылка сигнала
    int alrLeftP;
    //Крайнее правое положение энкодера при котором происходит обязательная посылка сигнала
    int alrRightP;

    //Таймер для опроса энкодера
    QTimer timer;

    //Функция преобразования напряжения в расстояние
    bool volumeToDistance();

    //Код ошибки
    ErrorCode ret;
    //Объект для работы с платой
    InstantAiCtrl *ctrl;
signals:
    //Сигнал достижения заданного положения
    void targetPos();
    //Сигнал достижения крайнелевого положения
    void alrLeftPos();
    //сигнал достижения крайнеправого положения
    void alrRightPos();
    //сигнал подтверждения приема данных
    void newData(int value);

private slots:
    //Функция запроса данных
    void requestData();
};

#endif // PPOTENCIOMETR_H
