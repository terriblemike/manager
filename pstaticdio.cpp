#include "pstaticdio.h"

//Функция преобразования номера входа индикатора в номер бита в слове состояния платы диск.сигналов
int fromIntToDIO(int num)
{
    //из номера выделяем номер группы и номер выхода в этой группе
    int group = num / 10;
    int ind = num % 10;
    //возвращаем номер бита
    return group * 8 + ind;
}

PStaticDIO::PStaticDIO(QObject *parent) :
    QObject(parent)
{
    //Сбрасываем признаки
    status = false;         //статус работы с платой
    settings = false;   //сбрасываем признак задания настроек
    channelIn = 0;      //обнуляем состояние входов
    channelOut = 0;     //и выходов

    //устанавливаем количество опросов в секунду = значение по умолчанию
    QPS = DEFAULT_PERIOD;

    //Подключаем сигнал от таймера к функции посыла запроса устройству
    connect(&timer, SIGNAL(timeout()), this, SLOT(getIn()));

    //подключаем сигнал изменения состояния выхода к функции обработки
    connect(this, SIGNAL(newOut(int, bool)), this, SLOT(setOut(int, bool)));
    //Подключаем сигнал установки состояния всех выходов за раз
//    connect(this, SIGNAL(setInds(UINT64)), this, SLOT(setIndS(UINT64)));
}

PStaticDIO::~PStaticDIO()
{

}

//Функция получения статуса данных
bool PStaticDIO::getStatus()
{
    return status;
}

//Функция инициализации параметров: номера платы в системе
bool PStaticDIO::Init()
{
    DeviceInformation selected(/*description.c_str()*/QString(DFAULT_DEVICE_DESCRIPTION).toStdWString().c_str());
    instantDiCtrl = AdxInstantDiCtrlCreate();
    instantDoCtrl = AdxInstantDoCtrlCreate();

    ErrorCode errorCode = Success;
    errorCode = instantDiCtrl->setSelectedDevice(selected);
    if (errorCode != Success)
    {
        //сбрасываем признак инициализации
        settings = false;
        return settings;
    }

    errorCode = Success;
    errorCode = instantDoCtrl->setSelectedDevice(selected);
    if (errorCode != Success)
    {
        //сбрасываем признак инициализации
        settings = false;
        return settings;
    }

    //выставляем признак инициализации
    settings = true;

    return settings;
}

//Функция установки количества опросов в секунду
void PStaticDIO::setQPS(int value)
{
    QPS = value;
}

//Функция запуска опроса
void PStaticDIO::start()
{
    if (settings)
        if (!timer.isActive())
            timer.start(1000 / QPS);
}

//Функция остановки опроса
void PStaticDIO::stop()
{
    if (timer.isActive())
        timer.stop();
}

//Функция получения состояния входов
uint64 PStaticDIO::getInState()
{
    return channelIn;
}

//Функция получения состояния выходов
uint64 PStaticDIO::getOutState()
{
    return channelOut;
}

//Функция преобразует данные из служебных в 64-х битное значение
void PStaticDIO::translate(UINT8 *in)
{
    //переменная для хранения состояния входов в 64-битном значении
    UINT64 tmp = 0;

    //проходим по всем группам состояний
    for (int i = DEFAULT_NUM_PORTS - 1; i >= 0 ; i--)
    {
        //формируем общее состояние входов
        tmp <<= DEFAULT_NUM_PORTS;
        tmp |= in[i];
    }

    //если были изменения состояний входов
    if (tmp != channelIn)
        //в массиве проходим по всем входам и ищем изменившийся
        for (int i = 0; i < DEFAULT_NUM_PORTS * DEFAULT_NUM_PORTS; i++)
        {
            //побитово сравниваем старое и новое значения
            int newState = (tmp >> i) & 1;
            int oldState = (channelIn >> i) & 1;
            if (newState != oldState)
            {
                //формируем номер входа
                int group = i / 8;
                int formatI = group * 10 + i % 8;

                //высылаем номер изменившегося входа и его состояние
                emit newIn(formatI, (newState > 0) ? true : false);
            }
        }
    //запоминаем полученное состояние входов как текущее
    channelIn = tmp;
}

//Функция запроса состояния входов
void PStaticDIO::getIn()
{
    if (!settings)
        return;

    //читаем состояние входов
    quint8 *portStates = new quint8[DEFAULT_NUM_PORTS];
    ErrorCode errorCode = Success;
    //читаем состояния входов
    errorCode = instantDiCtrl->Read(0, DEFAULT_NUM_PORTS, portStates);
    //если не успешно
    if (errorCode != Success)
        //статус работы с платой сбрасываем
        status = false;
    //если успешно
    else
    {
        //устанавливаем статус работы с платой
        status = true;
        //обрабатываем прочитанные данные
        translate(portStates);
    }
}

//Функция получения состояния одного входа
bool PStaticDIO::getInState(int num)
{
    //если соответствующий бит слова состояния входов установлен,
    //то возвращаем из функции соответствующее состояние
    if ((channelIn >> fromIntToDIO(num)) & 1)
        return true;
    else
        return false;
}

//Функция изменения состояния выхода
void PStaticDIO::setOut(int num, bool state)
{
    ErrorCode errorCode = Success;
    //временная переменная
    UINT64 tmp = 1;
    //смещаем единицу в положение нужного выхода
    tmp <<= fromIntToDIO(num);

    //массив байтов для задания состояние всех восьми групп выходов
    UINT8 portStates[8] = {0,0,0,0,0,0,0,0};

    //если нужно установить выход
    if (state)
    {
        //устанавливаем соответствующий бит
        channelOut |= tmp;
    }
    //если нужно сбросить
    else
    {
        //сбрасываем соответствующий бит
        channelOut &= ~tmp;
    }

    //в цикле проходим по всем портам и формируем их состояние из 64битового значения
    for (int i = 0; i < DEFAULT_NUM_PORTS ; i++)
        portStates[i] |= channelOut >> (DEFAULT_NUM_PORTS * i);

    //записываем состояния групп выходов и получаем результат записи
    errorCode = instantDoCtrl->Write(0, DEFAULT_NUM_PORTS, portStates);

    //если ошибка
    if (errorCode != Success)
        //сбрасываем статус работы с платой
        status = false;
    else
        //устанавливаем статус работы с платой
        status = true;
}

//Функция изменяет состояния всех выходов одновременно
void PStaticDIO::setIndS(UINT64 value)
{
    //код ошибки
    ErrorCode errorCode = Success;

    UINT8 portStates[8] = {0,0,0,0,0,0,0,0};

    //формируем из 64-битного значения состояние групп выходов
    for (int i = 0; i < DEFAULT_NUM_PORTS ; i++)
        portStates[i] |= value >> (DEFAULT_NUM_PORTS * i);

    //записываем состояние выходов и получаем результат
    errorCode = instantDoCtrl->Write(0, DEFAULT_NUM_PORTS, portStates);

    if (errorCode != Success)
        status = false;
    else
        status = true;
}
