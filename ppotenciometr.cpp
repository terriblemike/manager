#include "ppotenciometr.h"

PPotenciometr::PPotenciometr(QObject *parent) :
    QObject(parent)
{
    //Сбрасываем признаки
    status = false;         //исправности энкодера
    settings = false;   //установки параметров порта
    target = false;         //установки сигнального положения
    alrLeft = false;        //установки крайнелевого сигнального положения
    alrRight = false;       //установки крайнеправого сигнального положения
    targetSignal = false;
    alrLeftSignal = false;
    alrRightSignal = false;

    //устанавливаем количество опросов в секунду = значение по умолчанию
    QPS = DEFAULT_PERIOD;

    //Устанавливаем значения по умолчанию либо в ноль
    indDevice = DEFAULT_IND_DEVICE;
    channel = 0;
    volume = 0;
    pos = 0;
    prevPos = pos;



    //Подключаем сигнал от таймера к функции посыла запроса устройству
    connect(&timer, SIGNAL(timeout()), this, SLOT(requestData()));
}

PPotenciometr::~PPotenciometr()
{
    if (timer.isActive())
        timer.stop();
    if (settings)
    {
        //Закрываем устройство и освобождаем ресурсы
        ctrl->Cleanup();
        //Уничтожаем объект
        ctrl->Dispose();
    }
}

//Функция получения статуса
bool PPotenciometr::getStatus()
{
    return status;
}

//Функция инициализации параметров потенциометра: номера платы в системе, к которой подключен, и номер канала
bool PPotenciometr::Init(int ind, int chan)
{
    //задаем номер платы и номер канала
    indDevice = ind;
    channel = chan;

    //Создаем объект для работы с платами
    ctrl = AdxInstantAiCtrlCreate();

    //Создаем структуру для хранения информации о плате с заданным номером
    DeviceInformation devInfo(indDevice);

    //Выбираем плату с заданным номером
    ret = ctrl->setSelectedDevice(devInfo);
    //если не удалось подключиться к плате
    if (ret != Success)
        //сбрасываем признак инициализации
        settings = false;
    else
        //выставляем признак инициализации
        settings = true;

    return settings;
}

//Функция установки количества опросов в секунду
void PPotenciometr::setQPS(int value)
{
    QPS = value;
}

//Функция запуска опроса
void PPotenciometr::start()
{
    if (settings)
        if (!timer.isActive())
            timer.start(1000 / QPS);
}

//Функция остановки опроса
void PPotenciometr::stop()
{
    if (timer.isActive())
        timer.stop();
}

//Фуункции получения позиции выдвинутой части
int PPotenciometr::getPos()
{
    //если потенциометр не инициализирован
    if (!settings)
        return -1;

    //запрашиваем данные
    requestData();
    //если статус данных верен
    if (status)
        //возвращаем положение
        return pos;
    else
        //возвращаем ошибку
        return -1;
}

//Функция задания сигнального положения
void PPotenciometr::setTargetPos(int value)
{
    targetP = value;
    target = true;
}

//Функция задания крайнелевого сигнального положения
void PPotenciometr::setAlrLeftPos(int value)
{
    alrLeftP = value;
    alrLeft = true;
}

//Функция задания крайнеправого сигнального положения
void PPotenciometr::setAlrRightPos(int value)
{
    alrRightP = value;
    alrRight = true;
}

//Функция сброса сигнального положения
void PPotenciometr::resetTargetPos()
{
    target = false;
}

//Функция сброса крайнелевого сигнального положения
void PPotenciometr::resetAlrLeftPos()
{
    alrLeft = false;
}

//Функция сброса крайнеправого сигнального положения
void PPotenciometr::resetAlrRightPos()
{
    alrRight = false;
}

//Функция преобразования напряжения в расстояние
bool PPotenciometr::volumeToDistance()
{
    //при подключенном потенциометре напряжение не может быть нулевым
    if (volume == 0)
        status = false;

    if (volume > DEFAULT_VOL_END || volume < DEFAULT_VOL_BEGIN)
        return false;

    volume -= DEFAULT_VOL_BEGIN;

    //Преобразуем напряжение в положение
    pos = int(volume / DEFAULT_VOL_ON_MM);
    if (abs(prevPos - pos ) > 10)
        return true;

    //Предусматриваем небольшой допустимый зазор по положению, т.к. вольтаж может скакать
    //Положение не может быть меньше нуля
    if (pos < 0)
        pos = 0;

    //Положение не может быть больше максимального значения длины
    if (pos > DEFAULT_DLINA)
        pos = DEFAULT_DLINA;

    //сохраняем положение как предыдущее
    prevPos = pos;

    return true;
}

//Функция запроса данных
void PPotenciometr::requestData()
{
    //Читаем напряжение на канале
    ret = ctrl->Read(channel, 1, (int16*)0, &volume);
    if (ret == Success)
    {
        status = true;
        if (volumeToDistance())
            emit newData(pos);
    }
    else
    {
        status = false;
        emit newData(-1);
    }
}














