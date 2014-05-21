#include "pdalnomer.h"

PDalnomer::PDalnomer(QObject *parent) :
    QObject(parent)
{
    //Сбрасываем признаки
    status = false;         //исправности энкодера
    portSettings = false;   //установки параметров порта

    //устанавливаем количество опросов в секунду = значение по умолчанию
    QPS = DEFAULT_QPS;

    dalnostQuery = QString("%1%2%3%4%5%6%7%8").arg(DALNOST_QUERY).arg(DALNOST_QUERY)
            .arg(DALNOST_QUERY).arg(DALNOST_QUERY).arg(DALNOST_QUERY).arg(DALNOST_QUERY)
            .arg(DALNOST_QUERY).arg(DALNOST_QUERY);

    statusQuery = QString("%1%2").arg(STATUS_QUERY).arg(STATUS_QUERY);

    //Подключаем сигнал от таймера к функции посыла запроса устройству
    connect(&timer, SIGNAL(timeout()), this, SLOT(requestData()));
    //подключаем сигнал от порта о получении ответа к функции обработки
    connect(&port, SIGNAL(response(QString)), this, SLOT(responseData(QString)));
}

PDalnomer::~PDalnomer()
{
    //останавливаем работу порта
    port.stop();
}

////Функция получения статуса данных
//bool PDalnomer::getStatus()
//{
//    return status;
//}

//Функция для задания настроек последовательного порта
bool PDalnomer::setPortSettings(QString &portName, int baudRate, int dataBits, int parity, int stopBits, int flowControl)
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

//Функция установки количества опросов дальномера в секунду
void PDalnomer::setQPS(int value)
{
    QPS = value;
}

//Функция запуска опроса дальномера
void PDalnomer::start()
{
    //если установлены настройки порта
    if (portSettings)
        if(!timer.isActive())
            //Запускаем таймер
            timer.start(1000 / QPS);
}

//Функция остановки опроса дальномера
void PDalnomer::stop()
{
    if (timer.isActive())
        //останавливаем работу порта
        timer.stop();
}

//Функция одиночного запроса обновления данных
void PDalnomer::once()
{
    if (portSettings)
        port.transaction(dalnostQuery);
}

//Функция запроса состояния устройства
bool PDalnomer::getStatus()
{
    if (portSettings)
        port.transaction(statusQuery);
    return status;
}

//Фуункции получения дальности
int PDalnomer::getDalnost()
{
    return dalnost;
}

//Функция парсинга ответа от дальномера
bool PDalnomer::parseData(QString s)
{
    //Переменные для хранения данных для формирования дальности
    int km = -1,
        m100 = -1,
        m10 = -1,
        m = -1;
//        cm10 = -1,
//        cm = -1;

    QString buf = QString(s);
    //разбираем ответную посылку от дальномера, находим и выделяем данные

    //если ответ по длине меньше положенного, то выходим из функции
    if (buf.size() < 12)
        return false;

    //временная переменная для формирования дальности
    int tmpDal = -1;

    //в цикле обрабатываем ответ от дальномера
    for(int i = 0; i < buf.size() - 11 ; i++)
    {
        //если начальные символы ответа верны
        if ((buf[i] == '$') && (buf[i+1] == 'D') && (buf[i+2] == 'L') && (buf[i+3] == '5')
                && (buf[i+4] == 'M') && (buf[i+5] == ','))
        {
            //считываем значения километров, метров и т.п.
            km  = buf[i+6].digitValue();
            m100= buf[i+7].digitValue();
            m10 = buf[i+8].digitValue();
            m   = buf[i+9].digitValue();
//            cm10= buf[i+10].digitValue();
//            cm  = buf[i+11].digitValue();
            //если считанные значения адекватные
            if ((km > -1) && (m100 > -1) && (m10 > -1) &&
                (m > -1)/* && (cm10 > -1) && (cm > -1)*/)
                //формируем значение дальности
                tmpDal = km * 1000. + m100 * 100. + m10 * 10. + m;
            //при запросе статуса устройства в значениях дальности стоят нули,
            //а в байте статуса метка 'G'
            //Если оба признака есть, то это ответ на запрос состояния
            if (tmpDal == 0 && buf[i+13] == 'G')
                //статус устройства готов
                status = true;
            else
                //статус устройства не готов
                status = false;
            //значения километров, метров и т.п. делаем неадекватными
            km = -1,
            m100 = -1,
            m10 = -1,
            m = -1;
//            cm10 = -1,
//            cm = -1;
        }
    }

    //если значение дальности в правильном диапазоне
    if (tmpDal > 0 && tmpDal < 32767)
    {
        //присваиваем значение дальности
        dalnost = tmpDal;
        //выходим из функции со признаком успеха
        return true;
    }
    else
    {
        //обнуляем дальность
        dalnost = 0;
        //выходим из функции с признаком неуспеха
        return false;
    }
}

//Функция запроса данных от дальномера
void PDalnomer::requestData()
{
    if (portSettings)
        port.transaction(dalnostQuery);
}

//Функция получения данных от дальномера
void PDalnomer::responseData(QString response)
{
    //обрабатываем полученные от энкодера данные
    status = parseData(response);

    emit newData();
}

