#ifndef PENCODER_H
#define PENCODER_H

#include <QObject>
#include <QTimer>
#include <QSettings>

#include "pserialport.h"


#define DEFAULT_PERIOD              10  //количество опросов в секунду
#define PEREDATOCHNOE_CHISLO        10  //передаточное число для преобразования количества оборотов в градусы и минуты

//Максимальное значение по каждому параметру оборотов
//например 16 - это значения 0-15, т.е. значение 16 никогда не получим, т.к. отсчет от нуля
#define MAX_STEPS256    16
#define MAX_STEPS       256
#define MAX_VALSTEP     32
#define MAX_VALSUBSTEP  256

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
    //Функция получения количества неполного оборота
    int getValStep();
    //Функция получения количества неполного оборота
    int getValSubStep();
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
    bool setBegin(int step256, int step, int valstep, int valsubstep);
    //Функция получения градусов азимута
    int getGradAzim();
    //Функция получения минут азимута
    int getMinAzim();
    //Функция получения градусов угла/места
    int getGradUgol();
    //Функция получения минут угла/места
    int getMinUgol();
    //Функция записи нулевых позиций энкодеров в конф.файл
    bool writeEncNullPos(QString name);
    //Функция чтения нулевых позиций энкодеров в конф.файл
    bool readEncNullPos(QString name);
private:
    bool status;
    //Объект для работы с компортом
    PSerialPort port;
    //Признак установки параметров порта
    bool portSettings;

    //Количество опроса энкодера в секунду
    int QPS;

    //Количество циклов по 256 оборотов (значения от 0 - 15)
    int numSteps256;
    //Количество полных оборотов (значения от 0 - 255)
    int numSteps;
    //Значение неполного оборота (значение от 0 - 31)
    int valStep;
    //Значение неполного неполного борота (значение от 0 - 255)
    int valSubStep;

    //нулевые значения энкодера в походном положении
    int beginNumSteps256;
    int beginNumSteps;
    int beginValStep;
    int beginValSubStep;

    //число для преобразования значений поворота энкодеров азимута и угла/места в минуты
    double azimPeredChislo;
    double ugolPeredChislo;

    //Количество градусов
    int gradAzim;
    //Количество минут
    int minAzim;
    //Количество градусов
    int gradUgol;
    //Количество минут
    int minUgol;

    //Таймер для опроса энкодера
    QTimer timer;

    //Строка запроса данных от энкодера
    QString encoderQuery;
    //Функция парсинга ответа от энкодера
    bool parseEncoderData(QString s);
    //Функция поправки полученных значений на начальное положение энкодера
    void prepareValue();
signals:
    //сигнал обновления данных
    void newData();

private slots:
    //Функция запроса данных от энкодера
    void requestData();
    //Функция получения данных от энкодера
    void responseData(QString response);
};

#endif // PENCODER_H
