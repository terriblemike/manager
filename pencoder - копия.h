#ifndef PENCODER_H
#define PENCODER_H

#include <QObject>
#include <QTimer>

#include "pserialport.h"


#define DEFAULT_PERIOD              10  //количество опросов в секунду
#define PEREDATOCHNOE_CHISLO        10  //передаточное число для преобразования количества оборотов в градусы и минуты

class PEncoder : public QObject
{
    Q_OBJECT
public:
    explicit PEncoder(QObject *parent = 0);
    ~PEncoder();
    //Функция получения статуса энкодера
    bool getStatus();
    //Функция для задания настроек последовательного порта
    bool setPortSettings(QString &portName, int baudRate, int dataBits, int parity, int stopBits, int flowControl);
    //Функция установки количества опросов энкодера в секунду
    void setQPS(int value);
    //Функция запуска опроса энкодера
    void start();
    //Функция остановки опроса энкодера
    void stop();
    //Функция одиночного запроса обновления данных
    void once();
    //Фуункции получения количества циклов по 256 оборотов
    int getNumSteps256();
    //Функция получения количества полных оборотов
    int getNumSteps();
    //Функция задания сигнального положения
    void setTargetPos(int steps256, int steps);
    //Функция задания крайнелевого сигнального положения
    void setAlrLeftPos(int steps256, int steps);
    //Функция задания крайнеправого сигнального положения
    void setAlrRightPos(int steps256, int steps);

    //Функция преобразования количества оборотов в градусы и минуты
    void calc();

    //Функция сброса сигнального положения
    void resetTargetPos();
    //Функция сброса крайнелевого сигнального положения
    void resetAlrLeftPos();
    //Функция сброса крайнеправого сигнального положения
    void resetAlrRightPos();
    //Функция задания начального положения энкодера
    bool setBegin(int step256, int step);
    //Функция получения градусов
    int getGrad();
    //Функция получения минут
    int getMin();
private:
    bool status;
    //Объект для работы с компортом
    PSerialPort port;
    //Признак установки параметров порта
    bool portSettings;
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

    //Количество опроса энкодера в секунду
    int QPS;

    //Количество циклов по 256 оборотов (значения от 0 - 15)
    int numSteps256;
    //Количество полных оборотов (значения от 0 - 255)
    int numSteps;
    //предыдущее Количество циклов по 256 оборотов (значения от 0 - 15)
    int predNumSteps256;
    //предыдущее Количество полных оборотов (значения от 0 - 255)
    int predNumSteps;

    //Начальное количество циклов по 256 оборотов (значения от 0 - 15)
    int beginNumSteps256;
    //Начальное количество полных оборотов (значения от 0 - 255)
    int beginNumSteps;

    //Количество градусов
    int grad;
    //Количество минут
    int min;




    //Положение энкодера, при котором происходит посылка сигнала
    int targetPosSteps256;
    int targetPosSteps;

    //Крайнее левое положение энкодера, при котором происходит обязательная посылка сигнала
    int alrLeftPosSteps256;
    int alrLeftPosSteps;

    //Крайнее правое положение энкодера при котором происходит обязательная посылка сигнала
    int alrRightPosSteps256;
    int alrRightPosSteps;

    //Таймер для опроса энкодера
    QTimer timer;

    //Строка запроса данных от энкодера
    QString encoderQuery;
    //Функция парсинга ответа от энкодера
    bool parseEncoderData(QString s);
signals:
    //Сигнал достижения заданного положения
    void targetPos();
    //Сигнал достижения крайнелевого положения
    void alrLeftPos();
    //сигнал достижения крайнеправого положения
    void alrRightPos();
    //сигнал подтверждения приема данных
    void newData();

public slots:

private slots:
    //Функция запроса данных от энкодера
    void requestData();
    //Функция получения данных от энкодера
    void responseData(QString response);

};

#endif // PENCODER_H
