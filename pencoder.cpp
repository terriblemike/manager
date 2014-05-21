#include "pencoder.h"

//Калибровочный файла энкодеров
const char* ENC_CALIB_FILE = "./enccalib.ini";

PEncoder::PEncoder(QObject *parent) :
    QObject(parent)
{
    //Сбрасываем признаки
    status = false;         //исправности энкодера
    portSettings = false;   //установки параметров порта

    //нулевые значения энкодера в походном положении обнуляем
    beginNumSteps256 = 0;
    beginNumSteps = 0;
    beginValStep = 0;
    beginValSubStep = 0;

    //вычисляем число для преобразования значений поворота энкодера азимута и угла/места в минуты
    //для азимута - полный поворот платформы (360град) - это 11 полных оборота энкодера
    azimPeredChislo = (32 * 256) / (360 / 11) / 60;
    //для ула/места - полный оборот энкодера - это 360 град
    ugolPeredChislo = (32 * 256) / 360 / 60;

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
bool PEncoder::setBegin(int step256, int step, int valstep, int valsubstep)
{
    beginNumSteps256 = step256;
    beginNumSteps = step;
    beginValStep = valstep;
    beginValSubStep = valsubstep;

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

//Функция получения количества неполного оборота
int PEncoder::getValStep()
{
    return valStep;
}

//Функция получения количества неполного оборота
int PEncoder::getValSubStep()
{
    return valSubStep;
}

//Функция получения градусов азимута
int PEncoder::getGradAzim()
{
    return gradAzim;
}

//Функция получения минут азимута
int PEncoder::getMinAzim()
{
    return minAzim;
}

//Функция получения градусов угла/места
int PEncoder::getGradUgol()
{
    return gradUgol;
}

//Функция получения минут угла/места
int PEncoder::getMinUgol()
{
    return minUgol;
}

//Функция поправки полученных значений на начальное положение энкодера
void PEncoder::prepareValue()
{
    //поправка значения циклов по 256 оборотов
    //если значение лежит справа от начального положения
    if (numSteps256 > beginNumSteps256 && numSteps256 <= MAX_STEPS256)
        numSteps256 -= beginNumSteps256;
    else
        numSteps256 = MAX_STEPS256 - (beginNumSteps256 - numSteps256);

    //тоже самое но только для полных оборотов
    if (numSteps > beginNumSteps && numSteps <= MAX_STEPS)
        numSteps -= beginNumSteps;
    else
        numSteps = MAX_STEPS - (beginNumSteps - numSteps);

    //тоже самое но только для значения неполных оборотов
    if (valStep > beginValStep && valStep <= MAX_VALSTEP)
        valStep -= beginValStep;
    else
        valStep = MAX_VALSTEP - (beginValStep - valStep);

    //тоже самое но только для значения неполного неполного оборота
    if (valSubStep > beginValSubStep && valSubStep <= MAX_VALSUBSTEP)
        valSubStep -= beginValSubStep;
    else
        valSubStep = MAX_VALSUBSTEP - (beginValSubStep - valSubStep);
}

//Функция преобразования количества оборотов в градусы и минуты
void PEncoder::calc()
{
    //Вычисляем азимут
    //максимум возможно 11 полных оборотов, поэтому 5 слева и 5 права от нуля
    //(уже с поправкой на нулевое положение)
    //если в правой части круга
    if (numSteps >= 0 && numSteps < (MAX_STEPS / 2))
    {
        //вычисляем количество минут
        minAzim = (numSteps * MAX_VALSTEP * MAX_VALSUBSTEP + valStep *MAX_VALSUBSTEP + valSubStep) / azimPeredChislo;
        //выделяем количество градусов
        gradAzim = minAzim / 60;
        //остаток минут
        minAzim %= 60;
    }
    //если в левой части круга
    if (numSteps >= (MAX_STEPS / 2) && numSteps < MAX_STEPS)
    {
        //вычисляем количество минут (-1 это поправка, т.к. максимальное значение недостигается)
        minAzim = ((MAX_STEPS - 1 - numSteps) * MAX_VALSTEP * MAX_VALSUBSTEP +
                   (MAX_VALSTEP - 1 - valStep) *MAX_VALSUBSTEP +
                   (MAX_VALSUBSTEP - 1 - valSubStep)) / azimPeredChislo;

//        minAzim = ((15 - numSteps256) * 256 + (255 - numSteps)) /* / PEREDATOCHNOE_CHISLO*/;
        //выделяем количество градусов
        gradAzim = minAzim / 60;
        //остаток минут
        minAzim %= 60;
        //меняем знак
        gradAzim *= -1;
        minAzim *= -1;
    }

    //Вычисляем угол/места аналогично
    if (valStep >= 0 && valStep < (MAX_VALSTEP / 2))
    {
        minUgol = (valStep * MAX_VALSUBSTEP + valSubStep) / ugolPeredChislo;
        gradUgol = minUgol / 60;
        minUgol %= 60;
    }
    if (valStep >= (MAX_VALSTEP / 2) && valStep < MAX_VALSTEP)
    {
        minUgol = ((MAX_VALSTEP - 1 - valStep) * MAX_VALSTEP + (MAX_VALSUBSTEP - 1 - valSubStep)) / ugolPeredChislo;
        gradUgol = minUgol / 60;
        minUgol %= 60;
        gradUgol *= -1;
        minUgol *= -1;
    }
}

//Функция парсинга ответа от энкодера
bool PEncoder::parseEncoderData(QString s)
{
    QString buf = QString(s);
    //разбираем ответную посылку от энкодера на составляющие и выделяем данные
    uint soh = buf[0].cell();         //начало пакета
    uint adr = buf[1].cell() & 0xF;   //адрес энкодера в правильном формате
    uint ead = buf[1].cell();         //адрес энкодера в переданном формате
    uint hbr = buf[2].cell();         //количество циклов по 256 оборотов
    uint lbr = buf[3].cell();         //количество полных оборотов
    uint hbs = buf[4].cell();         //значение неполного оборота (подоборота)
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
        numSteps256 = hbr;      //количество циклов по 256 оборотов
        numSteps = lbr;         //количество полных оборотов
        valStep = hbs;          //значение неполного оборота (подоборота)
        valSubStep = lbs;       //значение неполного подоборота
        return true;
    }
    else
        return false;
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
    QString tmp = response;
    //обрабатываем полученные от энкодера данные
    status = parseEncoderData(tmp);

    //переводим значения с поправкой на нулевое положение энкодера
    prepareValue();

    //Функция перерасчета из количества оборотов в градусы и минуты
    calc();

    //выдаем сигнал новых данных
    emit newData();
}

//Функция записи нулевых позиций энкодеров в конф.файл
bool PEncoder::writeEncNullPos(QString name)
{

    //Открываем калибровочный файл энкодера угла
    QSettings *calib = new QSettings(ENC_CALIB_FILE,QSettings::IniFormat);

    //Записываем данные энкодера угла/места в файл
    calib->beginWriteArray(name);
    calib->setValue("STEP256", numSteps256);
    calib->setValue("STEP", numSteps);
    calib->setValue("VALSTEP", valStep);
    calib->setValue("VALSUBSTEP", valSubStep);
    calib->endArray();

    if (calib->status() == QSettings::NoError)
        return true;
    else
         return false;
}

//Функция чтения нулевых позиций энкодеров в конф.файл
bool PEncoder::readEncNullPos(QString name)
{
    //Открываем калибровочный файл энкодера угла
    QSettings *calib = new QSettings(ENC_CALIB_FILE,QSettings::IniFormat);
    calib->beginWriteArray(name);
    beginNumSteps256 = calib->value("STEP256", 0).toInt();
    beginNumSteps = calib->value("STEP", 0).toInt();
    beginValStep = calib->value("VALSTEP", 0).toInt();
    beginValSubStep = calib->value("VALSUBSTEP", 0).toInt();
    calib->endArray();

    if (calib->status() == QSettings::NoError)
        return true;
    else
         return false;
}



