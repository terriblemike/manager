#ifndef PSTATICDIO_H
#define PSTATICDIO_H

#include <QObject>
#include <QTimer>

#include <QDebug>

//Подключаем заголовочные файлы, поставляемый разработчиком
#include "bdaqctrl.h"

//Подключаем пространство имен, поставляемое разработчиком платы
using namespace Automation::BDaq;

#define DEFAULT_PERIOD      10  //количество опросов в секунду

#define DEFAULT_NUM_PORTS   8  //количество портов

#define DFAULT_DEVICE_DESCRIPTION   "PCI-1758UDIO,BID#0"

//Функция преобразования номера входа индикатора в номер бита в слове состояния платы диск.сигналов
int fromIntToDIO(int num);

class PStaticDIO : public QObject
{
    Q_OBJECT
public:
    explicit PStaticDIO(QObject *parent = 0);
    ~PStaticDIO();

    //Функция получения статуса данных
    bool getStatus();
    //Функция инициализации параметров: номера платы в системе
    bool Init();
    //Функция установки количества опросов в секунду
    void setQPS(int value);
    //Функция запуска опроса
    void start();
    //Функция остановки опроса
    void stop();
    //Функция получения состояния входов
    uint64 getInState();
    //Функция получения состояния выходов
    uint64 getOutState();
    //Функция получения состояния одного входа
    bool getInState(int num);
private:
    //Статус данных
    bool status;
    //Признак инициализации параметров потенциометра
    bool settings;

    //Количество опроса  в секунду
    int QPS;

    //Состояние выходов
    UINT64 channelOut;

    //Состояние входов
    UINT64 channelIn;

    //Таймер для опроса энкодера
    QTimer timer;

    //Код ошибки
    ErrorCode ret;

    //Объекты для работы с платой
    InstantDiCtrl *instantDiCtrl;
    InstantDoCtrl *instantDoCtrl;

    //Функция преобразует данные из служебных в 64-х битное значение
    void translate(UINT8 *in);
signals:
    //сигнал изменения состояния входа
    void newIn(int num, bool state);
    //сигнал изменения состояния выхода
    void newOut(int num, bool state);
    //сигнал изменения состояния выхо
//    void setInds(UINT64 value);

private slots:
    //Функция запроса состояния входов
    void getIn();
    //Функция изменения состояния выхода
    void setOut(int num, bool state);    
    //Функция изменяет состояния всех выходов одновременно
    void setIndS(UINT64 value);
};

#endif // PSTATICDIO_H
