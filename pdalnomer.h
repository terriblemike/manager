#ifndef PDALNOMER_H
#define PDALNOMER_H

#include <QObject>
#include <QTimer>

#include "pserialport.h"

#define DEFAULT_QPS      10  //количество опросов в секунду
#define DALNOST_QUERY       "$DL5M,ONE*18\r\n"
#define STATUS_QUERY        "$DL5M,ST*5B\r\n"

class PDalnomer : public QObject
{
    Q_OBJECT
public:
    explicit PDalnomer(QObject *parent = 0);
    //Деструктор класса
    ~PDalnomer();
    //Функция получения статуса дальномера
    bool getStatus();
    //Функция для задания настроек последовательного порта
    bool setPortSettings(QString &portName, int baudRate, int dataBits, int parity, int stopBits, int flowControl);
    //Функция установки количества опросов дальномера в секунду
    void setQPS(int value);
    //Функция одиночного запроса обновления данных
    void once();
    //Фуункции получения дальности
    int getDalnost();

private:
    bool status;
    //Объект для работы с компортом
    PSerialPort port;
    //Признак установки параметров порта
    bool portSettings;

    //Количество опроса дальномера в секунду
    int QPS;
    //Значение дальности
    int dalnost;

    //Таймер для опроса дальномера
    QTimer timer;

    //Строка запроса дальности
    QString dalnostQuery;
    //Строка запроса состояния
    QString statusQuery;

    //Функция парсинга ответа от дальномера
    bool parseData(QString s);
signals:
    //сигнал подтверждения приема данных
    void newData();

public slots:
    //Функция запуска опроса дальномера
    void start();
    //Функция остановки опроса дальномера
    void stop();

private slots:
    //Функция запроса данных от дальномера
    void requestData();
    //Функция получения данных от дальномера
    void responseData(QString response);

};

#endif // PDALNOMER_H
