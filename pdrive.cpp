#include "pdrive.h"

PDrive::PDrive(QObject *parent) :
    QObject(parent)
{
    init = false;

    imitaciya = true;

    //обнуляем количество срабатываний геркона
    countGerkonNO = 0;
    countGerkonNZ = 0;

    stateAC = false;

    posXgrad = 0;
    posXmin = 0;
    posYgrad = -90;
    posYmin = 0;

    posAC_X = 0;
    posAC_Y = 0;

    speedVizX = 0;
    speedVizY = 0;

    directionVizX = false;
    directionVizY = false;


    posVizXgrad = 0;
    posVizXmin = 0;
    posVizYgrad = 0;
    posVizYmin = 0;

    numCam = 1;
    speedX = 0;
//    prevSpeedX = 1;
    speedY = 0;
//    prevSpeedY = 1;
    deltaX = 0;
    deltaY = 0;
    prevPosXgrad = 0;
    prevPosXmin = 0;

    //Признак записи нулевых позиций энкодеров в конф.файл сбрасываем
    writeEncNullPos = false;

    //сбрасываем состояния оконечных устройств
    koncLeftNZ = true;
    koncLeftNO = false;
    koncRightNZ = true;
    koncRightNO = false;
    koncSpoilPerNZ = true;
    koncSpoilPerNO = false;
    koncSpoilZadNZ = true;
    koncSpoilZadNO = false;
    koncZTUNZ = true;
    koncZTUNO  = false;
    gerkonNZ = true;
    gerkonNO = false;

    //значение приближения
    navedZoom = 1.0;
    obzorZoom = 1.0;

    //сбрасываем состояние зумирования
    navedZoomPlus = false;
    navedZoomMinus = false;
    obzorZoomPlus = false;
    obzorZoomMinus = false;

    //сбрасываем состояние замков
    zamokAPU1 = false;
    zamokAPU2 = false;
    zamokSpoilPered = false;
    zamokSpoilZad = false;

    pologInit = false;
    polog = POLOG_POHOD;

    //устанавливаем режим наведения по умолчанию
    navedState = NAVED_STATE_MANUAL;
    //сбрасываем состояние наведения
    navedNavesti = false;
    //координаты точки наведения сбрасываем в ноль
    navedXgrad = 0;
    navedXmin = 0;
    navedYgrad = 0;
    navedYmin = 0;

    prevPosYgrad = 0;
    prevPosYmin = 0;

    beginZTU = ZTU_MAX_VALUE;


    //создаем таймеры
    //таймер рассчета движения в зависимости от режима наведения
    timer = new QTimer();
    timer->setInterval(1000/DRIVE_FPS);
    connect(timer, SIGNAL(timeout()), this, SLOT(calc()));
    //таймер изменения фокусного расстояния
    timerFocus = new QTimer();
    timerFocus->setInterval(1000/DRIVE_FPS);
    connect(timerFocus, SIGNAL(timeout()), this, SLOT(driveFocus()));
    //таймер актуатора замков ЗТУ
    timerZTU = new QTimer();
    timerZTU->setInterval(1000/DRIVE_FPS);
    connect(timerZTU, SIGNAL(timeout()), this, SLOT(driveZTU()));    
}

//Функция инициализации
bool PDrive::Init(PBMSD *value1, PBMSD *value2, PBMSD *value3, PBMSD *value4, PBMSD *value5, PGSM40 *value6/*, PPult *value7*/, PStaticDIO *value7)
{
    bmsd1 = value1;
    bmsd2 = value2;
    bmsd3 = value3;
    bmsd4 = value4;
    bmsd5 = value5;
    gsm40 = value6;
    staticDIO = value7;

    //обновляем интерфейс в плане концевых выключателей
    updateKoncUI();
    //передаем положение антенны в пульт в интерфейс
    emit newPolog(polog);

    init = true;

    return init;
}

//Фукнция для отправки данных о положении курсора по сети
void PDrive::sendPos()
{
    emit sendCommand(DR_COMMAND_OUTPUT_CURSOR_POSITION_X, posXgrad, posXmin);
    emit sendCommand(DR_COMMAND_OUTPUT_CURSOR_POSITION_Y, posYgrad, posYmin);
}

//Функция таймера разкладывания антенны
void PDrive::pologTimerRazl()
{
    //этап 1
    //разблокируем передний замок спойлера (устанавливаем выход в true)
    //линейный актуатор ЗТУ задвигаем для разблокировки замков
    //этап 2
    //включаем мотор спойлера
    //проверяем количество срабатываний геркона, на первом передний замок спойлера блокируем
    //(уст выход в false), актуатор ЗТУ выдвигаем на в исходное положение, открываем замки АПУ1 и АПУ2,
    //а на втором срабатывании сбрасываем скорость
    //как только сработал концевик спойлера задний, останавливаем мотор

    //этап 3
    //запускаем точный актуатор для подъема (угол измеряем энкодером)
    //как только поднялсся до -60 град, останавливаем.

    //этап 4
    //запускаем грубый актуатор (угол измеряем энкодером)
    //как только поднялся до 30 град (-60 + 90 град), останавливаем

    //этап 5
    //сбрасываем питание с замков АПУ1 и АПУ2
    //запускаем точный актуатор для опускания антенны (угол измеряем энкодером)
    //как только опустился до 0 град, останавливаем.


    //энкодер стоит на подшипнике и измеряет положение антенны независимо от того, каким актуатором двигаем
    //концевик ЗТУ срабатываем, когда антенна ложится.

    switch (stageSetPolog) {
    //этап 1
    //разблокировка переднего замка спойлера и замков ЗТУ
    case 1:
        if (stageCount == 0)
        {
            //задаем состояние замка
            zamokSpoilPered = true;
            //на плате задаем соответствующий выход
            staticDIO->newOut(OUT_ZAMOK_SPOIL_PERED, zamokSpoilPered);
            //обновляем интерфейс
            emit updateKonc(OUT_ZAMOK_SPOIL_PERED, zamokSpoilPered);
            //запоминаем начальное положение актуатора ЗТУ
            beginZTU = ZTU;
            //актуатор ЗТУ задвигаем в нулевое положение
            setNewZTU(0);
            //переходим к следующему подшагу
            stageCount = 1;
        }

        //Если положение актуатора ЗТУ задвинуто
        if (ZTU < 0 + ZTU_DOPUSK)
        {
            //задаем следующий этап
            stageSetPolog++;
            //счетчик подэтапа сбрасываем
            stageCount = 0;
        }
        break;

    //этап 2
    //включаем мотор спойлера
    //проверяем количество срабатываний геркона, на первом передний замок спойлера блокируем
    //(уст выход в false), актуатор ЗТУ выдвигаем на половину, открываем замоки АПУ1 и АПУ2,
    //как только сработал концевик спойлера задний, останавливаем мотор
    case 2:
        //обрабатываем подэтап
        switch (stageCount) {
        //начало движения спойлера - геркон еще не срабатывал
        case 0:
            //медленно запускаем мотор спойлера
            bmsd3->newUskorenie(10);
            bmsd3->newTormogenie(10);
            bmsd3->newDirection(DIRECTION_RIGHT);
            bmsd3->newSpeed(MAX_SPEED);
            //обновляем интерфейс
            emit updateBmsd3UI();
            stageCount = 1;
            break;
        //геркон сработал один раз
        case 1:
            if (countGerkonNO == 1 || countGerkonNZ == 1)
            {
                //сбрасываем состояние замка
                zamokSpoilPered = false;
                //на плате задаем соответствующий выход
                staticDIO->newOut(OUT_ZAMOK_SPOIL_PERED, zamokSpoilPered);
                //обновляем интерфейс
                emit updateKonc(OUT_ZAMOK_SPOIL_PERED, zamokSpoilPered);

                //устанавливаем состояние замка
                zamokAPU1 = true;
                //на плате задаем соответствующий выход
                staticDIO->newOut(OUT_ZAMOK_APU1, zamokAPU1);
                //обновляем интерфейс
                emit updateKonc(OUT_ZAMOK_APU1, zamokAPU1);

                //устанавливаем состояние замка
                zamokAPU2 = true;
                //на плате задаем соответствующий выход
                staticDIO->newOut(OUT_ZAMOK_APU2, zamokAPU2);
                //обновляем интерфейс
                emit updateKonc(OUT_ZAMOK_APU2, zamokAPU2);

                //возвращаем актуатор ЗТУ в исходное положение
                setNewZTU(beginZTU);
                //переходим к следующему подшагу
                stageCount = 2;
            }
            break;
        //геркон сработал два раза - замедляемся для остановки
        case 2:
            if (countGerkonNO == 2 || countGerkonNZ == 2)
            {
                bmsd3->newSpeed(MAX_SPEED / 2);
                emit updateBmsd3UI();
                stageCount = 3;
            }
            break;
        //отслеживаем достижения заднего концевика и останавливаем мотор
        case 3:
            //спойлер достиг концевого выключателя
            if(koncSpoilZadNO || !koncSpoilZadNZ)
            {
                //скорость в ноль
                bmsd3->newSpeed(0);
                //тормозим резко
                bmsd3->newTormogenie(24);
                //меняем направление на обратное
                bmsd3->newDirection(DIRECTION_LEFT);
                //обновляем интерфейс
                emit updateBmsd3UI();
                stageCount = 0;
                //переходим к следующему этапу
                stageSetPolog++;
            }
            break;
        }
        break;
    //этап 3
    //запускаем точный актуатор для подъема (угол измеряем энкодером)
    //как только поднялся до -60 град, останавливаем.
    case 3:
        if (stageCount == 0)
        {
            //запускаем точный актуатор
            gsm40->newUskorenie(GSM40_DEFAULT_USKORENIE);
            gsm40->newTormogenie(GSM40_DEFAULT_TORMOGENIE);
            gsm40->newDirection(GSM40_DIRECTION_UP);
            gsm40->newSpeed(MAX_SPEED);
            //обновляем интерфейс
            emit updateGsm40UI();
        }

        //далее счетчик используется для подсчета секунду
        stageCount++;

        //если имитация включена
        if (imitaciya)
        {
            //каждую секунду увеличиваем градус на 2 и сбрасываем счетчик в 1
            if (stageCount >= 11)
            {
                posYgrad += 2;
                emit newDataUgol(posYgrad, 0);
                stageCount = 1;

            }
        }

        //если подняли более чем на 30 градусов, останавливаем актуатор
        if (posYgrad >= -60)
        {
            gsm40->newSpeed(0);
            gsm40->newDirection(GSM40_DIRECTION_DOWN);
            //обновляем интерфейс
            emit updateGsm40UI();
            //сбрасываем счетчик подэтапов
            stageCount = 0;
            //переходим к следующему этапу
            stageSetPolog++;
        }
        break;
    //этап 4
    //запускаем грубый актуатор (угол измеряем энкодером)
    //как только поднялся до 30 град (-60 + 90), останавливаем
    case 4:
        if (stageCount == 0)
        {
            //запускаем грубый актуатор
            bmsd1->newUskorenie(24);
            bmsd1->newTormogenie(24);
            bmsd1->newDirection(DIRECTION_RIGHT);
            bmsd1->newSpeed(MAX_SPEED);
            //обновляем интерфейс
            emit updateBmsd1UI();
        }

        //далее счетчик используется для подсчета секунду
        stageCount++;

        //если имитация включена
        if (imitaciya)
        {
            //каждую секунду увеличиваем градус на 2 и сбрасываем счетчик в 1
            if (stageCount >= 11)
            {
                posYgrad += 10;
                emit newDataUgol(posYgrad, 0);
                stageCount = 1;
                emit sendCommand(DR_COMMAND_OUTPUT_CURSOR_POSITION_Y, posYgrad, posYmin);
            }
        }

        //если подняли более чем на 90 градусов, останавливаем актуатор
        if (posYgrad >= 30)
        {
            bmsd1->newSpeed(0);
            bmsd1->newDirection(DIRECTION_LEFT);
            //обновляем интерфейс
            emit updateBmsd1UI();
            //сбрасываем счетчик подэтапов
            stageCount = 0;
            //переходим к следующему этапу
            stageSetPolog++;
        }
        break;
    //этап 5
    //сбрасываем замки АПУ1 и АПУ2
    //запускаем точный актуатор для опускания антенны (угол измеряем энкодером)
    //как только опустился до 0 град, останавливаем.
    case 5:
        if (stageCount == 0)
        {
            //сбрасываем состояние замка
            zamokAPU1 = false;
            //на плате задаем соответствующий выход
            staticDIO->newOut(OUT_ZAMOK_APU1, zamokAPU1);
            //обновляем интерфейс
            emit updateKonc(OUT_ZAMOK_APU1, zamokAPU1);

            //сбрасываем состояние замка
            zamokAPU2 = false;
            //на плате задаем соответствующий выход
            staticDIO->newOut(OUT_ZAMOK_APU2, zamokAPU2);
            //обновляем интерфейс
            emit updateKonc(OUT_ZAMOK_APU2, zamokAPU2);

            //запускаем точный актуатор
            gsm40->newUskorenie(GSM40_DEFAULT_USKORENIE);
            gsm40->newTormogenie(GSM40_DEFAULT_TORMOGENIE);
            gsm40->newDirection(GSM40_DIRECTION_DOWN);
            gsm40->newSpeed(MAX_SPEED);
            //обновляем интерфейс
            emit updateGsm40UI();
        }

        //далее счетчик используется для подсчета секунду
        stageCount++;

        //если имитация включена
        if (imitaciya)
        {
            //каждую секунду увеличиваем градус на 2 и сбрасываем счетчик в 1
            if (stageCount >= 11)
            {
                posYgrad -= 2;
                emit newDataUgol(posYgrad, 0);
                stageCount = 1;
                emit sendCommand(DR_COMMAND_OUTPUT_CURSOR_POSITION_Y, posYgrad, posYmin);
            }
        }

        //если опустили до 90 градусов, останавливаем актуатор
        if (posYgrad <= 0)
        {
            gsm40->newSpeed(0);
            //обновляем интерфейс
            emit updateGsm40UI();
            //сбрасываем счетчик подэтапов
            stageCount = 0;
            //переходим к следующему этапу
            stageSetPolog++;
            //признак того, что операция закончена
            pologInit = false;
            //запоминаем положение антенны
            polog = POLOG_WORKING;
            //обновляем положение на пульте в интерфейсе
            emit newPolog(polog);
            //останавливаем таймер
            timerRazlSlog->stop();            

            timer->start();
        }
        break;
    }
}


void PDrive::pologTimerSlog()
{
    //этап 1
    //возвращаем антенну в нормальное положение (0 град по азимуту)

    //этап 2
    //запускаем точный актуатор для поднятия антенны до 30 град.

    //этап 3
    //запускаем грубый актуатор
    //как только опустился до -60 град, останавливаем

    //этап 4
    //разблокируем задний замок спойлера (устанавливаем выход в true)
    //запускаем точный актуатор для опускания до -90 град
    //и до срабатывания концевика ЗТУ

    //этап 5
    //включаем мотор спойлера
    //проверяем количество срабатываний геркона, на первом задний замок спойлера сбрасываем
    //(уст выход в false), а на втором срабатывании сбрасываем скорость.
    //как только сработал концевик спойлера передний, останавливаем мотор

    switch (stageSetPolog) {
    //этап 1
    //возвращаем антенну в нормальное положение (0 град по азимуту)
    case 1:
        //начало движения
        if (posXgrad == 0 && posXmin == 0)
        {
            //останавливаем мотор
            bmsd2->newSpeed(0);
            updateBmsd2UI();
            //задаем следующий этап
            stageSetPolog++;
            //счетчик подэтапа сбрасываем
            stageCount = 0;
            //прерываем
            break;
        }

        if (bmsd2->uskorenie != 24)
            bmsd2->newUskorenie(24);
        if (bmsd2->tormogenie != 24)
            bmsd2->newTormogenie(24);

        //если слева от нуля и направление надо менять
        if (posXgrad <= 0 && posXmin <= 0 && bmsd2->direction != DIRECTION_RIGHT)
            //двигаемся направо
            bmsd2->newDirection(DIRECTION_RIGHT);
        //если справа от нуля
        if (posXgrad >= 0 && posXmin >= 0 && bmsd2->direction != DIRECTION_LEFT)
            //двигаемся влево
            bmsd2->newDirection(DIRECTION_LEFT);

        //задаем скорость
        if (posXgrad != 0)
        {
            if (bmsd2->speed != MAX_SPEED)
                bmsd2->newSpeed(MAX_SPEED);
        }
        else
        {
            if (bmsd2->speed != MAX_SPEED / 4)
                bmsd2->newSpeed(MAX_SPEED / 4);
        }

        updateBmsd2UI();

        //счетчик используется для подсчета секунд
        stageCount++;

        //если имитация включена
        if (imitaciya)
        {
            //каждую секунду уменьшаем градус на 2 и сбрасываем счетчик в 1
            if (stageCount >= 11)
            {
                posXgrad += 10;
                emit newDataAzim(posXgrad, 0);
                stageCount = 1;

            }
        }
        break;
    //этап 2
    //запускаем точный актуатор для поднятия антенны до 30 град.
    case 2:
        if (stageCount == 0)
        {
            //запускаем точный актуатор
            gsm40->newUskorenie(GSM40_DEFAULT_USKORENIE);
            gsm40->newTormogenie(GSM40_DEFAULT_TORMOGENIE);
            gsm40->newDirection(GSM40_DIRECTION_UP);
            gsm40->newSpeed(MAX_SPEED);
            //обновляем интерфейс
            emit updateGsm40UI();
        }

        //далее счетчик используется для подсчета секунд
        stageCount++;

        //если имитация включена
        if (imitaciya)
        {
            //каждую секунду уменьшаем градус на 2 и сбрасываем счетчик в 1
            if (stageCount >= 11)
            {
                posYgrad += 2;
                emit newDataUgol(posYgrad, 0);
                stageCount = 1;

            }
        }

        //если подняли более чем на 30 градусов, останавливаем актуатор
        if (posYgrad >= 30)
        {
            gsm40->newSpeed(0);
            gsm40->newDirection(GSM40_DIRECTION_DOWN);
            //обновляем интерфейс
            emit updateGsm40UI();
            //сбрасываем счетчик подэтапов
            stageCount = 0;
            //переходим к следующему этапу
            stageSetPolog++;
        }
        break;
    //этап 3
    //запускаем грубый актуатор
    //как только опустился до -30 град, останавливаем
    case 3:
        if (stageCount == 0)
        {
            //запускаем грубый актуатор
            bmsd1->newUskorenie(24);
            bmsd1->newTormogenie(24);
            bmsd1->newDirection(DIRECTION_LEFT);
            bmsd1->newSpeed(MAX_SPEED);
            //обновляем интерфейс
            emit updateBmsd1UI();
        }

        //далее счетчик используется для подсчета секунду
        stageCount++;

        //если имитация включена
        if (imitaciya)
        {
            //каждую секунду увеличиваем градус на 2 и сбрасываем счетчик в 1
            if (stageCount >= 11)
            {
                posYgrad -= 10;
                emit newDataUgol(posYgrad, 0);
                stageCount = 1;
                emit sendCommand(DR_COMMAND_OUTPUT_CURSOR_POSITION_Y, posYgrad, posYmin);
            }
        }

        //если опустили более чем на 90 градусов, останавливаем актуатор
        if (posYgrad <= -60)
        {
            bmsd1->newSpeed(0);
            bmsd1->newDirection(DIRECTION_RIGHT);
            //обновляем интерфейс
            emit updateBmsd1UI();

            //сбрасываем счетчик подэтапов
            stageCount = 0;
            //переходим к следующему этапу
            stageSetPolog++;
        }
        break;
    //этап 4
    //разблокируем задний замок спойлера (устанавливаем выход в true)
    //запускаем точный актуатор для опускания до -90 град
    //и до срабатывания концевика ЗТУ
    case 4:
        if (stageCount == 0)
        {
            //разблокируем задний замок спойлера
            zamokSpoilZad = true;
            //на плате задаем соответствующий выход
            staticDIO->newOut(OUT_ZAMOK_SPOIL_ZAD, zamokSpoilZad);
            //обновляем интерфейс
            emit updateKonc(OUT_ZAMOK_SPOIL_ZAD, zamokSpoilZad);

            //запускаем точный актуатор
            gsm40->newUskorenie(GSM40_DEFAULT_USKORENIE);
            gsm40->newTormogenie(GSM40_DEFAULT_TORMOGENIE);
            gsm40->newDirection(GSM40_DIRECTION_DOWN);
            gsm40->newSpeed(MAX_SPEED);
            //обновляем интерфейс
            emit updateGsm40UI();
        }

        //далее счетчик используется для подсчета секунд
        stageCount++;

        //если имитация включена
        if (imitaciya)
        {
            //каждую секунду уменьшаем градус на 2 и сбрасываем счетчик в 1
            if (stageCount >= 11)
            {
                posYgrad -= 2;
                emit newDataUgol(posYgrad, 0);
                stageCount = 1;

            }
        }

        //если опустили антенну или сработал концевик ЗТУ
        if (posYgrad <= -90 || koncZTUNO || !koncZTUNZ)
        {
            gsm40->newSpeed(0);
            gsm40->newDirection(GSM40_DIRECTION_UP);
            //обновляем интерфейс
            emit updateGsm40UI();
            //сбрасываем счетчик подэтапов
            stageCount = 0;
            //переходим к следующему этапу
            stageSetPolog++;
        }
        break;
    //этап 5
    //включаем мотор спойлера
    //проверяем количество срабатываний геркона, на первом задний замок спойлера сбрасываем
    //(уст выход в false), а на втором срабатывании сбрасываем скорость.
    //как только сработал концевик спойлера передний, останавливаем мотор
    case 5:
        //обрабатываем подэтап
        switch (stageCount) {
        //начало движения спойлера - геркон еще не срабатывал
        case 0:
            //медленно запускаем мотор спойлера
            bmsd3->newUskorenie(10);
            bmsd3->newTormogenie(10);
            bmsd3->newDirection(DIRECTION_LEFT);
            bmsd3->newSpeed(MAX_SPEED);
            //обновляем интерфейс
            emit updateBmsd3UI();
            stageCount = 1;
            break;
        //геркон сработал один раз
        case 1:
            if (countGerkonNO == 1 || countGerkonNZ == 1)
            {
                //сбрасываем состояние замка
                zamokSpoilZad = false;
                //на плате задаем соответствующий выход
                staticDIO->newOut(OUT_ZAMOK_SPOIL_ZAD, zamokSpoilZad);
                //обновляем интерфейс
                emit updateKonc(OUT_ZAMOK_SPOIL_ZAD, zamokSpoilZad);
                //переходим к следующему подшагу
                stageCount = 2;
            }
            break;
        //геркон сработал два раза - замедляемся для остановки
        case 2:
            if (countGerkonNO == 2 || countGerkonNZ == 2)
            {
                bmsd3->newSpeed(MAX_SPEED / 2);
                emit updateBmsd3UI();
                stageCount = 3;
            }
            break;
        //отслеживаем достижения переднего концевика и останавливаем мотор
        case 3:
            //спойлер достиг концевого выключателя
            if(koncSpoilPerNO || !koncSpoilPerNZ)
            {
                //скорость в ноль
                bmsd3->newSpeed(0);
                //тормозим резко
                bmsd3->newTormogenie(24);
                //меняем направление на обратное
                bmsd3->newDirection(DIRECTION_RIGHT);
                //обновляем интерфейс
                emit updateBmsd3UI();
                stageCount = 0;
                //признак того, что операция закончена
                pologInit = false;
                //переходим к следующему этапу
                stageSetPolog++;
                //запоминаем положение антенны
                polog = POLOG_POHOD;
                //обновляем положение на пульте в интерфейсе
                emit newPolog(polog);
                //останавливаем таймер
                timerRazlSlog->stop();
            }
            break;
        }
        break;
    }
}

//функция обработки изменения состояния входов/выходов платы дискретных сигналов
void PDrive::newStaticDIOState(int num, bool state)
{
    switch (num) {
    case IN_GERKON_NO:
        gerkonNO = state;
        if (gerkonNO)
        {
            countGerkonNO++;
            if (countGerkonNO > 2)
                countGerkonNO = 1;
        }
        break;
    case IN_GERKON_NZ:
        gerkonNZ = state;
        if (!gerkonNZ)
        {
            countGerkonNZ++;
            if (countGerkonNZ > 2)
                countGerkonNZ = 1;
        }
        break;
    case IN_KONC_LEFT_NO:
        koncLeftNO = state;
        break;
    case IN_KONC_LEFT_NZ:
        koncLeftNZ = state;
        break;
    case IN_KONC_RIGHT_NO:
        koncRightNO = state;
        break;
    case IN_KONC_RIGHT_NZ:
        koncRightNZ = state;
        break;
    case IN_KONC_SPOIL_PER_NO:
        koncSpoilPerNO = state;
        break;
    case IN_KONC_SPOIL_PER_NZ:
        koncSpoilPerNZ = state;
        break;
    case IN_KONC_SPOIL_ZAD_NO:
        koncSpoilZadNO = state;
        break;
    case IN_KONC_SPOIL_ZAD_NZ:
        koncSpoilZadNZ = state;
        break;
    case IN_KONC_ZTU_NO:
        koncZTUNO = state;
        //антенна в сложенном состояниии - записываем начальные значения энкодеров в файл
        //если нулевые позиции энкодеров не были записаны в конф.файл
        //и концевик сработал
        if (!writeEncNullPos && state)
        {
            //устанавливаем признак записи в конф. файл
            writeEncNullPos = true;
            //посылаем сигнал для записи нулевых позиций энкодеров в файл
            emit writeEncNPos();
        }
        break;
    case IN_KONC_ZTU_NZ:
        koncZTUNZ = state;
        //антенна в сложенном состояниии - записываем начальные значения энкодеров в файл
        //если нулевые позиции энкодеров не были записаны в конф.файл
        //и концевик сработал
        if (!writeEncNullPos && !state)
        {
            //устанавливаем признак записи в конф. файл
            writeEncNullPos = true;
            //посылаем сигнал для записи нулевых позиций энкодеров в файл
            emit writeEncNPos();
        }
        break;
    //если в интерфейсе нажали CheckBox для изменения состояния замка
    case OUT_ZAMOK_APU1:
        //если замок уже не был в этом состоянии
        if (zamokAPU1 != state)
        {
            //сохраняем новое состояие
            zamokAPU1 = state;
            //выдаем сигнал на плату для изменения состояния соотв. выхода
            staticDIO->newOut(OUT_ZAMOK_APU1, zamokAPU1);
        }
        break;
    case OUT_ZAMOK_APU2:
        if (zamokAPU2 != state)
        {
            zamokAPU2 = state;
            staticDIO->newOut(OUT_ZAMOK_APU2, zamokAPU2);
        }
        break;
    case OUT_ZAMOK_SPOIL_PERED:
        if (zamokSpoilPered != state)
        {
            zamokSpoilPered = state;
            staticDIO->newOut(OUT_ZAMOK_SPOIL_PERED, zamokSpoilPered);
        }
        break;
    case OUT_ZAMOK_SPOIL_ZAD:
        if (zamokSpoilZad != state)
        {
            zamokSpoilZad = state;
            staticDIO->newOut(OUT_ZAMOK_SPOIL_ZAD, zamokSpoilZad);
        }
        break;
    default:
        break;
    }
}

//Функция изменения положения антенны (походное, рабочее)
void PDrive::setPolog(bool state)
{
    if (pologInit)
        return;

    if (polog == state)
        return;

    pologInit = true;

    timer->stop();

    //Если необходимо перевести в рабочее положение (разложить)
    if (state == POLOG_WORKING)
    {
        //номер этапа первый
        stageSetPolog = 1;
        //номер подэтапа
        stageCount = 0;
        //создаем таймер
        timerRazlSlog = new QTimer();
        //подключаем функцию раскладывания к таймеру
        connect(timerRazlSlog, SIGNAL(timeout()), this, SLOT(pologTimerRazl()));
        //задаем интервал
        timerRazlSlog->setInterval(1000 / POLOG_FPS);
        //запускаем таймер
        timerRazlSlog->start();
    }
    //если необходимо перевести в походное положение (сложить), то поступаем аналогично,
    //но подключаем к таймеру функцию складывания
    else
    {
        stageSetPolog = 1;
        stageCount = 0;
        timerRazlSlog = new QTimer();
        connect(timerRazlSlog, SIGNAL(timeout()), this, SLOT(pologTimerSlog()));
        timerRazlSlog->setInterval(1000 / POLOG_FPS);
        timerRazlSlog->start();
    }
}

//положение цели в режиме автосопровождения
void PDrive::posTargetAC(int x, int y)
{
    posAC_X = x;
    posAC_Y = y;
}

//функция обновления интерфейса в плане концевых выключателей
void PDrive::updateKoncUI()
{
    emit updateKonc(IN_GERKON_NO, gerkonNO);
    emit updateKonc(IN_GERKON_NZ, gerkonNZ);
    emit updateKonc(IN_KONC_LEFT_NO, koncLeftNO);
    emit updateKonc(IN_KONC_LEFT_NZ, koncLeftNZ);
    emit updateKonc(IN_KONC_RIGHT_NO, koncRightNO);
    emit updateKonc(IN_KONC_RIGHT_NZ, koncRightNZ);
    emit updateKonc(IN_KONC_SPOIL_PER_NO, koncSpoilPerNO);
    emit updateKonc(IN_KONC_SPOIL_PER_NZ, koncSpoilPerNZ);
    emit updateKonc(IN_KONC_SPOIL_ZAD_NO, koncSpoilZadNO);
    emit updateKonc(IN_KONC_SPOIL_ZAD_NZ, koncSpoilZadNZ);
    emit updateKonc(IN_KONC_ZTU_NO, koncZTUNO);
    emit updateKonc(IN_KONC_ZTU_NZ, koncZTUNZ);
    emit updateKonc(OUT_ZAMOK_APU1, zamokAPU1);
    emit updateKonc(OUT_ZAMOK_APU2, zamokAPU2);
    emit updateKonc(OUT_ZAMOK_SPOIL_PERED, zamokSpoilPered);
    emit updateKonc(OUT_ZAMOK_SPOIL_ZAD, zamokSpoilZad);
}

//Слот для обработки новых отклонений по осям джойстика
void PDrive::newJoyData(int x, int y)
{
    speedX = x;
    speedY = y;

    deltaX = speedX * TIMESTEP;
    deltaY = speedY * TIMESTEP;
}

//Функция обработки таймера рассчета движения
void PDrive::calc()
{
//    //расчитываем координаты маркера джойстика по оси Х
//    posXgrad += deltaX / 60;
//    posXmin += deltaX % 60;
//    posXgrad += posXmin /60;
//    posXmin = posXmin % 60;

//    //расчитываем координаты маркера джойстика по оси У
//    posYgrad += deltaY / 60;
//    posYmin += deltaY % 60;
//    posYgrad += posYmin /60;
//    posYmin = posYmin % 60;

//    //обрабатываем состояния зумирования и отсылаем соответствующие сигналы в программу видеообработки
//    if (navedZoomPlus)
//        emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_NAVED_ZOOM_IN);
//    if (navedZoomMinus)
//        emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_NAVED_ZOOM_OUT);
//    if (obzorZoomPlus)
//        emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_OBZOR_ZOOM_IN);
//    if (obzorZoomMinus)
//        emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_OBZOR_ZOOM_OUT);

//    //если режим автосопровождения, то от программы видеообработки передаются
//    //значения отклонения от центра экрана в пикселях.
//    //Цель - сделать так, чтобы эти отклонения были нулевыми
//    if (stateAC)
//    {
//        //УПРАВЛЕНИЕ ПО ОСИ Х ---------------------------------------------
//        //если достигли заданного положения по оси Х
//        if (posAC_X == 0)
//        {
//            //останавливаем мотор азимута
//            speedVizX = 0;
//        }
//        else
//        {
//            //если левее центра визирной камеры
//            if (posAC_X < 0)
//                //задаем направление влево мотора азимута
//                directionVizX = DIRECTION_LEFT;

//            //если положение курсора джойстика правее текущего положения антенны
//            if (posXgrad > 0)
//                //задаем направление вправо мотора азимута
//                directionVizX = DIRECTION_RIGHT;

//            //ЗДЕСЬ можно задать более точное управление скоростью
//            speedVizX = int(MAX_SPEED / navedZoom);
//        }

//        //задаем скорость и направление мотора азимута
//        bmsd2->newSpeed(speedVizX);
//        bmsd2->newDirection(directionVizX);

//        //обновляем интерфейс
//        emit updateBmsd2UI();

//        //УПРАВЛЕНИЕ ПО ОСИ Y ---------------------------------------------
//        if (posAC_X < 0)
//        {
//            //останавливаем актуаторы угла/места
//            speedVizY = 0;
//        }
//        else
//        {
//            //если точка наведения ниже текущего положения антенны
//            if (posAC_Y < 0)
//                //задаем мотору азимута скорость и направление вниз
//                directionVizY = GSM40_DIRECTION_DOWN;

//            //если точка наведения выше текущего положения антенны
//            if (posAC_Y > 0)
//                //задаем мотору азимута скорость и направление вверх
//                directionVizY = GSM40_DIRECTION_UP;


//            //ЗДЕСЬ можно задать более точное управление скоростью
//            speedVizY = int(MAX_SPEED / navedZoom);
//        }

//        //задаем скорость и направление актуатора угла места
//        gsm40->speed = speedVizY;
//        gsm40->direction = directionVizY;

//        //обновляем интерфейс
//        emit updateGsm40UI();
//        return;
//    }

//    switch (navedState) {
//    //Если ручное наведение, то управляем моторами в зависимости от отклонения джойстика,
//    //а координаты маркера джойстика совпадают с положением антенны
//    case NAVED_STATE_MANUAL:
//        //координаты маркера джойстика совпадают с координатами антенны
//        posXgrad = posVizXgrad;
//        posXmin = posVizXmin;
//        posYgrad = posVizYgrad;
//        posYmin = posVizYmin;

//        //расчет направления в зависимости от отклонения джойстика
//        directionVizX = speedX > 0 ? DIRECTION_LEFT : DIRECTION_RIGHT;
//        directionVizY = speedY > 0 ? GSM40_DIRECTION_UP : GSM40_DIRECTION_DOWN;

//        //скорость моторов не может быть меньше нуля
//        speedVizX = speedX >= 0 ? speedX : -speedX;
//        speedVizY = speedY >= 0 ? speedY : -speedY;

//        //скорость не может превышать максимально допустимое значение
//        if (speedVizX > MAX_SPEED)
//            speedVizX = MAX_SPEED;

//        if (speedVizY > MAX_SPEED)
//            speedVizY = MAX_SPEED;

//        //задаем скорость и направление мотора азимута
//        bmsd2->newSpeed(speedVizX);
//        bmsd2->newDirection(directionVizX);
//        //обновляем интерфейс
//        emit updateBmsd2UI();

//        //задаем скорость и направление актуатора угла места
//        gsm40->newSpeed(speedVizY);
//        gsm40->newDirection(directionVizY);
//        //обновляем интерфейс
//        emit updateGsm40UI();
//        break;

//    //Если в режиме наведения, то проверяем, нажата ли кнопка навести и
//    //перемещаем антенну в заданное положение
//    case NAVED_STATE_NAVESTI:
//        //если кнопка навести не была нажата или антенна достигла заданного положения
//        if (!navedNavesti)
//            break;

//        //УПРАВЛЕНИЕ ПО ОСИ Х ---------------------------------------------
//        //если достигли заданного положения по оси Х
//        if (navedXgrad == posVizXgrad && navedXmin == posVizXmin)
//        {
//            //останавливаем мотор азимута
//            speedVizX = 0;
//        }
//        else
//        {
//            //если положение курсора джойстика левее текущего положения антенны
//            if (navedXgrad < posVizXgrad || navedXmin < posVizXmin)
//                //задаем направление влево мотора азимута
//                directionVizX = DIRECTION_LEFT;

//            //если положение курсора джойстика правее текущего положения антенны
//            if (navedXgrad > posVizXgrad || navedXmin > posVizXmin)
//                //задаем направление вправо мотора азимута
//                directionVizX = DIRECTION_RIGHT;

//            //здесь можно задать более точное управление скоростью
//            //если расстояние до положения курсора джойстика больше градуса
//            if (abs(navedXgrad - posVizXgrad) > 1)
//                speedVizX = MAX_SPEED; //скорость на максимум
//            else
//                speedVizX = MAX_SPEED / 2; //скорость в половину
//        }

//        //задаем скорость и направление мотора азимута
//        bmsd2->newSpeed(speedVizX);
//        bmsd2->newDirection(directionVizX);

//        //обновляем интерфейс
//        emit updateBmsd2UI();

//        //УПРАВЛЕНИЕ ПО ОСИ Y ---------------------------------------------
//        if (navedYgrad == posVizYgrad && navedYmin == posVizYmin)
//        {
//            //останавливаем актуаторы угла/места
//            speedVizY = 0;
//        }
//        else
//        {
//            //если точка наведения ниже текущего положения антенны
//            if (navedYgrad < posVizXgrad || navedYmin < posVizXmin)
//                //задаем направление вних мотора азимута
//                directionVizY = GSM40_DIRECTION_DOWN;

//            //если точка наведения выше текущего положения антенны
//            if (navedYgrad > posVizXgrad || navedYmin > posVizXmin)
//                //задаем направление вверх мотора азимута
//                directionVizY = GSM40_DIRECTION_UP;

//            //здесь можно задать более точное управление скоростью
//            //если расстояние до положения курсора джойстика больше градуса
//            if (abs(navedYgrad - posVizYgrad) > 1)
//                speedVizY = MAX_SPEED; //скорость на максимум
//            else
//                speedVizY = MAX_SPEED / 2; //скорость в половину
//        }

//        //задаем скорость и направление актуатора угла места
//        gsm40->speed = speedVizY;
//        gsm40->direction = directionVizY;

//        //обновляем интерфейс
//        emit updateGsm40UI();
//        break;

//    //если режим полуавтоматического наведения похож на режим наведения,
//    //но только осуществляется непрерывно и цель совместить положение антенны с положением курсора джойстика
//    case NAVED_STATE_PA:
//        //УПРАВЛЕНИЕ ПО ОСИ Х ---------------------------------------------
//        //если достигли заданного положения по оси Х
//        if (posXgrad == posVizXgrad && posXmin == posVizXmin)
//        {
//            //останавливаем мотор азимута
//            speedVizX = 0;
//        }
//        else
//        {
//            //если положение курсора джойстика левее текущего положения антенны
//            if (posXgrad < posVizXgrad || posXmin < posVizXmin)
//                //задаем скорость и направление влево мотора азимута
//                directionVizX = DIRECTION_LEFT;

//            //если положение курсора джойстика правее текущего положения антенны
//            if (posXgrad > posVizXgrad || posXmin > posVizXmin)
//                //задаем скорость и направление вправо мотора азимута
//                directionVizX = DIRECTION_RIGHT;

//            //здесь можно задать более точное управление скоростью
//            //если расстояние до положения курсора джойстика больше градуса
//            if (abs(posXgrad - posVizXgrad) > 1)
//                speedVizX = MAX_SPEED; //скорость на максимум
//            else
//                speedVizX = MAX_SPEED / 2; //скорость в половину
//        }

//        //задаем скорость и направление мотора азимута
//        bmsd2->newSpeed(speedVizX);
//        bmsd2->newDirection(directionVizX);

//        //обновляем интерфейс
//        emit updateBmsd2UI();

//        //УПРАВЛЕНИЕ ПО ОСИ Y ---------------------------------------------
//        if (posYgrad == posVizYgrad && posYmin == posVizYmin)
//        {
//            //останавливаем актуаторы угла/места
//            speedVizY = 0;
//        }
//        else
//        {
//            //если точка наведения ниже текущего положения антенны
//            if (posYgrad < posVizXgrad || posYmin < posVizXmin)
//                //задаем скорость и направление вних мотора азимута
//                directionVizY = GSM40_DIRECTION_DOWN;

//            //если точка наведения выше текущего положения антенны
//            if (posYgrad > posVizXgrad || posYmin > posVizXmin)
//                //задаем скорость и направление вверх мотора азимута
//                directionVizY = GSM40_DIRECTION_UP;


//            //здесь можно задать более точное управление скоростью
//            //если расстояние до положения курсора джойстика больше градуса
//            if (abs(posYgrad - posVizYgrad) > 1)
//                speedVizY = MAX_SPEED; //скорость на максимум
//            else
//                speedVizY = MAX_SPEED / 2; //скорость в половину
//        }

//        //задаем скорость и направление актуатора угла места
//        gsm40->speed = speedVizY;
//        gsm40->direction = directionVizY;

//        //обновляем интерфейс
//        emit updateGsm40UI();
//        break;
//    }

//    //если координаты курсора поменялись по сравнению с предыдущим положением
//    if (prevPosXgrad != posXgrad || prevPosXmin != posXmin || prevPosYgrad != posYgrad || prevPosYmin != posYmin)
//    {
//        //запоминаем текущие координаты как предыдущие
//        prevPosXgrad = posXgrad;
//        prevPosXmin = posXmin;
//        prevPosYgrad = posYgrad;
//        prevPosYmin = posYmin;
//        //вызываем функцию для передачи новых координат по сети
//        sendPos();
//    }
}

//Функция таймера для достижения заданного положения фокусного актуатора
void PDrive::driveFocus()
{
    //если достигли максимального положения, то изменяем направление на обратное
    if (focus > FOCUS_MAX_VALUE)
        bmsd4->newDirection(DIRECTION_LEFT);
    //если находимся в заданной зоне рядом с требуемым положением
    if (((focus > (newFocus - FOCUS_DOPUSK)) && (focus < (newFocus + FOCUS_DOPUSK))))
    {
        //увеличиваем счетчик пребывания в заданной зоне
        numTry++;
        //при достижении определенного значения счетчика
        //уменьшаем скорость для точного позиционирования
        if (numTry == 5)
        {
            bmsd4->newSpeed(5);
            //обновляем интерфейс
            emit updateBmsd4UI();
        }
        //если пребываем в заданной зоне достаточное время
        if (numTry > 20)
        {
            //останавливаем таймер
            timerFocus->stop();
            //сбрасываем скорость
            bmsd4->newSpeed(0);
            //обновляем интерфейс
            emit updateBmsd4UI();
            //обнуляем счетчик
            numTry = 0;
        }
    }
    //если находимся левее заданной зоны
    if (focus < newFocus - FOCUS_DOPUSK)
    {
        //задаем направление движения вправо
        bmsd4->newDirection(DIRECTION_RIGHT);
        //обновляем интерфейс
        emit updateBmsd4UI();
    }

    //если находимся правее заданной зоны
    if (focus > newFocus + FOCUS_DOPUSK)
    {
        //задаем направление движения влево
        bmsd4->newDirection(DIRECTION_LEFT);
        //обновляем интерфейс
        emit updateBmsd4UI();
    }
}

//Функция таймера для достижения заданного положения актуатора ЗТУ
void PDrive::driveZTU()
{    
    //если достигли максимального положения, то изменяем направление на обратное
    if (ZTU > ZTU_MAX_VALUE)
        bmsd5->newDirection(DIRECTION_LEFT);
    //если находимся в заданной зоне рядом с требуемым положением
    if (((ZTU > (newZTU - ZTU_DOPUSK)) && (ZTU < (newZTU + ZTU_DOPUSK))))
    {
        //увеличиваем счетчик пребывания в заданной зоне
        numTry++;
        //при достижении определенного значения счетчика
        //уменьшаем скорость для точного позиционирования
        if (numTry == 5)
        {
            bmsd5->newSpeed(20);
            //обновляем интерфейс
            emit updateBmsd5UI();
        }
        //если пребываем в заданной зоне достаточное время
        if (numTry > 20)
        {
            //останавливаем таймер
            timerZTU->stop();
            //сбрасываем скорость
            bmsd5->newSpeed(0);
            //обновляем интерфейс
            emit updateBmsd5UI();
            //обнуляем счетчик
            numTry = 0;
        }
    }
    //если находимся левее заданной зоны
    if (ZTU < newZTU - ZTU_DOPUSK)
    {
        //задаем направление движения вправо
        bmsd5->newDirection(DIRECTION_RIGHT);
        //обновляем интерфейс
        emit updateBmsd5UI();
    }

    //если находимся правее заданной зоны
    if (ZTU > newZTU + ZTU_DOPUSK)
    {
        //задаем направление движения влево
        bmsd5->newDirection(DIRECTION_LEFT);
        //обновляем интерфейс
        emit updateBmsd5UI();
    }
}

//Функция получения текущего значения фокусного расстояния
void PDrive::setFocus(int value)
{
    focus = value;
}

//Функция получения текущего значения расстояния ЗТУ
void PDrive::setZTU(int value)
{
    ZTU = value;
}

////Функция задания азимутального положения и
////запуска механизма управления актуатором для его достижения
//void PDrive::setNewAzim(int value)
//{
//    //если таймер активер, то останавливаем его
//    if (timerAzim->isActive())
//        timerAzim->stop();
//    //задаем новое требуемое значение фокусного расстояния
//    newAzim = value;
//    //задаем скорость движения актуатора в максимум
//    bmsd2->newSpeed(MAX_SPEED);
//    //обновляем интерфейс
//    emit updateBmsd2UI();
//    //сбрасываем счетчик нахождения актуатора в заданной зоне
//    numTry = 0;
//    //запускаем таймер
//    timerAzim->start();
//}

//Функция задания нового фокусного положения и
//запуска механизма управления актуатором для его достижения
void PDrive::setNewFocus(int value)
{
    //если таймер активер, то останавливаем его
    if (timerFocus->isActive())
        timerFocus->stop();
    //задаем новое требуемое значение фокусного расстояния
    newFocus = value;
    //задаем скорость движения актуатора в максимум
    bmsd4->newSpeed(MAX_SPEED);
    //обновляем интерфейс
    emit updateBmsd4UI();
    //сбрасываем счетчик нахождения актуатора в заданной зоне
    numTry = 0;
    //запускаем таймер
    timerFocus->start();
}

//Функция задания нового положения ЗТУ и
//запуска механизма управления актуатором для его достижения
void PDrive::setNewZTU(int value)
{
    //если таймер активер, то останавливаем его
    if (timerZTU->isActive())
        timerZTU->stop();
    //задаем новое требуемое значение расстояния
    newZTU = value;
    //задаем скорость движения актуатора в максимум
    bmsd5->newSpeed(MAX_SPEED);
    //обновляем интерфейс
    emit updateBmsd5UI();
    //сбрасываем счетчик нахождения актуатора в заданной зоне
    numTry = 0;
    //запускаем таймер
    timerZTU->start();
}
