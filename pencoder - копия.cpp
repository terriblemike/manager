#include "pencoder.h"

PEncoder::PEncoder(QObject *parent) :
    QObject(parent)
{
    //Сбрасываем признаки
    status = false;         //исправности энкодера
    portSettings = false;   //установки параметров порта
    target = false;         //установки сигнального положения
    alrLeft = false;        //установки крайнелевого сигнального положения
    alrRight = false;       //установки крайнеправого сигнального положения
    targetSignal = false;
    alrLeftSignal = false;
    alrRightSignal = false;

    //необходимо доработать в плане чтения этих данных из файла, который формируется один раз при включенни системы при условии что нажат концевик ЗТУ
    beginNumSteps256 = 0;
    beginNumSteps = 0;

    //устанавливаем количество опросов в секунду = значение по умолчанию
    QPS = DEFAULT_PERIOD;

    //формируем команду для запроса данных от энкодера
    encoderQuery[0] = uchar(0x01);
    encoderQuery[1] = uchar(0x80);
    encoderQuery[2] = uchar(0x02);
    encoderQuery[3] = uchar(0x80);
    encoderQuery[4] = uchar(0x04);

    //Подключаем сигнал от таймера к функции посыла запроса устройству
    connect(&timer, SIGNAL(timeout()), this, SLOT(requestData()));
    //подключаем сигнал от порта о получении ответа к функции обработки
    connect(&port, SIGNAL(response(QString)), this, SLOT(responseData(QString)));
}

//Деструктор класса
PEncoder::~PEncoder()
{
    //останавливаем работу порта
    port.stop();
}

//Функция получения статуса данных
bool PEncoder::getStatus()
{
    return status;
}

//Функция задания начального положения энкодера
bool PEncoder::setBegin(int step256, int step)
{
    beginNumSteps256 = step256;
    beginNumSteps = step;

    return true;
}



//Функция для задания настроек последовательного порта
bool PEncoder::setPortSettings(QString &portName, int baudRate, int dataBits, int parity, int stopBits, int flowControl)
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

//Функция установки количества опросов энкодера в секунду
void PEncoder::setQPS(int value)
{
    QPS = value;
}

//Функция запуска опроса энкодера
void PEncoder::start()
{
    //если установлены настройки порта
    if (portSettings)
        if(!timer.isActive())
            //Запускаем таймер
            timer.start(1000 / QPS);
}

//Функция остановки опроса энкодера
void PEncoder::stop()
{
    if (timer.isActive())
        //останавливаем таймер
        timer.stop();
}

//Функция одиночного запроса обновления данных
void PEncoder::once()
{
    if (portSettings)
        port.transaction(encoderQuery);
}

//Фуункции получения количества циклов по 256 оборотов
int PEncoder::getNumSteps256()
{
    return numSteps256;
}

//Функция получения количества полных оборотов
int PEncoder::getNumSteps()
{
    return numSteps;
}

//Функция получения градусов
int PEncoder::getGrad()
{
    return grad;
}

//Функция получения минут
int PEncoder::getMin()
{
    return min;
}

//Функция задания сигнального положения
void PEncoder::setTargetPos(int steps256, int steps)
{
    //устанавливаем сигнальное положение
    targetPosSteps256 = steps256;
    targetPosSteps = steps;
    //устанавливаем признак задания сигнального положения
    target = true;
    //сбрасываем признак того, что сигнал уже был отправлен
    targetSignal = false;
}

//Функция задания крайнелевого сигнального положения
void PEncoder::setAlrLeftPos(int steps256, int steps)
{
    //устанавливаем сигнальное положение
    alrLeftPosSteps256 = steps256;
    alrLeftPosSteps = steps;
    //устанавливаем признак задания сигнального положения
    alrLeft = true;
    //сбрасываем признак того, что сигнал уже был отправлен
    alrLeftSignal = false;
}

//Функция задания крайнеправого сигнального положения
void PEncoder::setAlrRightPos(int steps256, int steps)
{
    //устанавливаем сигнальное положение
    alrRightPosSteps256 = steps256;
    alrRightPosSteps = steps;
    //устанавливаем признак задания сигнального положения
    alrRight = true;
    //сбрасываем признак того, что сигнал уже был отправлен
    alrRightSignal = false;
}

//Функция преобразования количества оборотов в градусы и минуты
void PEncoder::calc()
{
    //ДОДЕЛАТЬ с привязкой к начальному значению
    if (numSteps256 >= 0 && numSteps256 <= 5)
    {
        min = (numSteps256 * 256 + numSteps) /* / PEREDATOCHNOE_CHISLO*/;
        grad = min / 60;
        min %= 60;
    }
    if (numSteps256 >= 6 && numSteps256 <= 15)
    {
        min = ((15 - numSteps256) * 256 + (255 - numSteps)) /* / PEREDATOCHNOE_CHISLO*/;
        grad = min / 60;
        min %= 60;
        grad *= -1;
        min *= -1;
    }
}

//Функция сброса сигнального положения
void PEncoder::resetTargetPos()
{
    target = false;
}

//Функция сброса крайнелевого сигнального положения
void PEncoder::resetAlrLeftPos()
{
    alrLeft = false;
}

//Функция сброса крайнеправого сигнального положения
void PEncoder::resetAlrRightPos()
{
    alrRight = false;
}

//Функция парсинга ответа от энкодера
bool PEncoder::parseEncoderData(QString s)
{
    QString buf = QString(s);
    //разбираем ответную посылку от энкодера на составляющие и выделяем данные
    uint soh = buf[0].cell();         //начало пакета
    uint adr = buf[1].cell() & 0x7;   //адрес энкодера в правильном формате
    uint ead = buf[1].cell();         //адрес энкодера в переданном формате
    uint hbr = buf[2].cell();         //количество циклов по 256 оборотов
    uint lbr = buf[3].cell();         //количество полных оборотов
    uint hbs = buf[4].cell();         //значение неполного оборотоа (подоборота)
    uint lbs = buf[5].cell();         //значение неполного подоборота
    uint lrc = buf[6].cell();         //контрольная сумма
    uint eot = buf[7].cell();         //конец пакета

    //вычисляем контрольную сумму (эталон)
    uint lrc_etalon = ead ^ hbr ^ lbr ^ hbs ^ lbs;

    //проверяем пакет на достоверность путем сравнения с эталонными значениями
    //Т.к. для энкодеров не задаем адрес, то сравниваем его со значением по умолчанию
    //Если посылка правильная, то присваиваем полученные значения полям класса
    if ((soh == 0x01) && (adr == 0x02) && (lrc == lrc_etalon) && (eot == 0x04))
    {
        predNumSteps256 = numSteps256;
        predNumSteps = numSteps;
        numSteps256 = hbr;      //количество циклов по 256 оборотов
        numSteps = lbr;         //количество полных оборотов
        return true;
    }
    //иначе присваиваем полям класса нулевые значения
    else
    {
//        numSteps256 = 0;      //количество циклов по 256 оборотов
//        numSteps = 0;         //количество полных оборотов
        return false;
    }
}

//Функция запроса данных от энкодера
void PEncoder::requestData()
{
    if (portSettings)
        port.transaction(encoderQuery);
}

//Функция получения данных от энкодера
void PEncoder::responseData(QString response)
{
    //обрабатываем полученные от энкодера данные
    status = parseEncoderData(response);

    //Функция перерасчета из количества оборотов в градусы и минуты
    calc();

    //выдаем сигнал новых данных
    emit newData();
    //если данные неверны, то прерываем функцию
    if (!status)
        return;

    //проверяем полученные данные на признак необходимости отправки сигналов
    //о достижении сигнальных состояний

    //если задано сигнальное положение и сигнал еще не был отправлен
    if (target && !targetSignal)
    {
        //если достигнуто заданное положение
        if ((targetPosSteps256 == numSteps256) && (targetPosSteps == numSteps))
        {
            //отсылаем сигнал
            emit targetPos();
            //устанавливаем признак посыла сигнала
            targetSignal = true;
        }
    }

    //если задано крайнелевое сигнальное положение и сигнал еще не был отправлен
    if (alrLeft && !alrLeftSignal)
    {
        //если достигнуто заданное положение
        if ((alrLeftPosSteps256 == numSteps256) && (alrLeftPosSteps == numSteps))
        {
            //отсылаем сигнал
            emit alrLeftPos();
            //устанавливаем признак посыла сигнала
            alrLeftSignal = true;
        }
    }

    //если задано крайнеправое сигнальное положение и сигнал еще не был отправлен
    if (alrRight && !alrRightSignal)
    {
        //если достигнуто заданное положение
        if ((alrRightPosSteps256 == numSteps256) && (alrRightPosSteps == numSteps))
        {
            //отсылаем сигнал
            emit alrRightPos();
            //устанавливаем признак посыла сигнала
            alrRightSignal = true;
        }
    }
}



