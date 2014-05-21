#include "ppult.h"

PPult::PPult(QObject *parent) :
    QObject(parent)
{
    //Начальная инициализация состояния пульта
    status = false;
    //Признак инициализации
    init = false;

    //Подключаем выходной сигнал для установки индикатора на функцию обработки платы
//    connect(this, SIGNAL(newOut(int,bool)), dio, SLOT(setOut(int,bool)));

    //Подключаем выходной сигнал для установки состояний всех индикаторов на сигнал платы
//    connect(this, SIGNAL(setIndS(UINT64)), dio, SIGNAL(setInds(UINT64)));

    //Подключаем выходной сигнал для установки состояний индикатора на сигнал платы
//    connect(dio, SIGNAL(newIn(int,bool)), this, SIGNAL(newIn(int,bool)));
    //Подключаем сигнал о новом состоянии кнопки к его обработчику
    //Такое подключение через сигнал сделано для того, чтобы с интерфейса можно было
    //посылать сигнал и имитировать нажатие кнопки на пульте
    connect(this, SIGNAL(newIn(int,bool)), this, SLOT(newState(int,bool)));

    //Начальная инициализация состояния системы
    stateScreen = MULTI;

    prevIndState = 0;

    //признаки неисправности системы по группам
    stateStatusAPU = false;
    stateStatusOES = false;
    stateStatusLVS = false;
    stateStatusSPOIL = false;

    stateWork = false;
    stateObslug = false;

    stateSystem = BLOCK;

    stateFocus = FOCUSAUTO;

    stateNavedNight = false;
    stateNaved = NAVED_STATE_NAVESTI;

    stateNavedZoomIn = false;
    stateNavedZoomOut = false;

    stateObzorZoomIn = false;
    stateObzorZoomOut = false;
    stateObzorA = false;
    stateObzorC = false;

    stateUnknownHP1 = false;
    stateUnknownHP2 = false;
    stateUnknownHP3 = false;
    stateUnknownHP4 = false;
    stateUnknownHP5 = false;
    stateUnknownHP6 = false;
    stateUnknownHP7 = false;
    stateUnknownHP8 = false;
    stateUnknownHP9 = false;
    stateUnknownIP1 = false;
    stateUnknownIP2 = false;
    stateUnknownIP3 = false;

    statePolog = POHOD;

    stateVisokoe = false;
    stateGK = false;

    //состояние индикаторов группы Unknown
    stateHP1 = false;
    stateHP2 = false;
    stateHP3 = false;
    stateHP4 = false;
    stateHP5 = false;
    stateHP6 = false;
    stateHP7 = false;
    stateHP8 = false;
    stateHP9 = false;
    stateIP1 = false;
    stateIP2 = false;
    stateIP3 = false;

    statePologSlog = false;
    statePologRazv = false;

    stateCommonControl = false;
    stateCommonAC = false;
    stateCommonGK = false;
    stateCommonReserv = false;
    stateCommonDalnost = false;
    stateCommonVisokoe = false;

    //создаем таймер для кнопки дальность
    timerDalnost = new QTimer();
    timerDalnost->setInterval(1000);
    connect(timerDalnost, SIGNAL(timeout()), SLOT(timeDal()));

    //создаем таймер для копирования логов на флешку
    flashTimer = new QTimer();
    flashTimer->setInterval(2000);
    connect(flashTimer, SIGNAL(timeout()), SLOT(flashTimerFunc()));

    //количество секунд удержавния кнопки дальность обнуляем
    numSecDalnost = 0;

    //признак автосопровождения обнуляем
    stateAC = false;
}

PPult::~PPult()
{

}

//Функция инициализации
bool PPult::Init(PStaticDIO *DIO, PDrive *DRIVE, ANetwork *ANETWORK)
{
    //Указатель на объект для работы с платой
    dio = DIO;
    //Указатель на объект для работы с классом управления
    drive = DRIVE;

    //Указатель на объект для обмена данными по сети со сторонним абонентом
    anetwork = ANETWORK;

    connect(anetwork, SIGNAL(newInputData(DATA*)), this, SLOT()

    init = true;

    return init;
}

//Функция записи логов на флешку
void PPult::copyLogsToFlash()
{
    //осуществляем поиск флешки

    //признак наличия флешки
    bool isFlash = false;

    //литера диска флешки
    QString path;

    DWORD dr = GetLogicalDrives(); // получаем битовое слово подключенных дисков

    //проходим по всем битам слова
    for(int x = 0; x < 26; x++)
    {
        //если диск подключен
        if ((dr>>x) & 1)
        {
            //получаем литеру диска
            path = QString(((char)(65+x)))+QString(":\\");

            //далее узнаём готово ли устройство
            WORD OldErrorMode;
            //убираем показ ошибок
            OldErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
            QDir _dir;
            //пытаемcя открыть корневую директорию
            bool ready = _dir.exists(path);
            //восстанавливаем старый режим показа ошибок
            SetErrorMode(OldErrorMode);

            //если устройство готово
            if (ready)
            {
                //узнаем тип диска
                UINT drive_type = GetDriveType((wchar_t*)QString(path).utf16());
                //если флешка, то устанавливаем признак наличия флешки
                if (drive_type == DRIVE_REMOVABLE)
                    isFlash = true;
            }
        }
    }

    //если флешка найдена
    if (isFlash)
    {
        //обнуляем индекс горящего индикатора
        flashInd = 0;
        //устанавливаем признак начала записи на флешку
        copyFlash = true;
        //запускаем таймер загорания индикаторов
        flashTimer->start();

        //адрес источника папки с логами
        QString sourceFolder = ".\\logs";
        //адрес конечной папки
        QString destFolder = path + "logs";

        QDir sourceDir(sourceFolder);
        //если папка с логами не существует
        if(!sourceDir.exists())
        {
            //устанавливаем признак окончания копирования файлов
            copyFlash = false;
            //выходим из функции
            return;
        }

        QDir destDir(destFolder);
        //если конечная папку не существует
        if(!destDir.exists())
            destDir.mkdir(destFolder);

        //получаем список файлов в папке
        QStringList files = sourceDir.entryList(QDir::Files);

        //в цикле копируем все файлы
        for(int i = 0; i< files.count(); i++)
        {
            QString srcName = sourceFolder + "\\" + files[i];
            QString destName = destFolder + "\\" + files[i];
            QFile::copy(srcName, destName);
        }
        //устанавливаем признак окончания копирования файлов
        copyFlash = false;
    }
}

//Функции установки контроля состояния
void PPult::setStateStatusAPU(bool state)
{
    stateStatusAPU = state;
    emit newOut(PULT_IND_STATUS_APU, state);
}

//Функции установки контроля состояния
void PPult::setStateStatusOEC(bool state)
{
    stateStatusOES = state;
    emit newOut(PULT_IND_STATUS_OES, state);
}

//Функции установки контроля состояния
void PPult::setStateStatusLVS(bool state)
{
    stateStatusLVS = state;
    emit newOut(PULT_IND_STATUS_LVS, state);
}

//Функции установки контроля состояния
void PPult::setStateStatusSPOIL(bool state)
{
    stateStatusSPOIL = state;
    emit newOut(PULT_IND_STATUS_SPOIL, state);
}

//Функция установки признака обработки нажатия высокого
void PPult::setStateVisokoe(bool state)
{
    stateVisokoe = state;
    emit newOut(PULT_IND_COMMON_VISOKOE, state);
}

//Функция установки признака возможности нажатия высокого
void PPult::setStateGK(bool state)
{
    stateGK = state;
    emit newOut(PULT_IND_COMMON_GK, state);
}

//Функция формирования 64-битного слова состояния индикаторов
UINT64 PPult::prepareIndsState(UINT64 prevstate, int num, bool state)
{
    UINT64 newState = prevstate;

    //временная переменная
    UINT64 tmp = 1;
    //устанавливаем единицу в положение нужного выхода
    tmp <<= fromIntToDIO(num);

    //если нужно установить выход
    if (state)
    {
        //устанавливаем соответствующий бит
        newState |= tmp;
    }
    //если нужно сбросить
    else
    {
        //сбрасываем соответствующий бит
        newState &= ~tmp;
    }

    return newState;
}

//Функция таймера копирования на флешку
void PPult::flashTimerFunc()
{
    //запоминаем текущее состояние выходов платы
    prevIndState = dio->getOutState();

    //новое состояние индикаторов
    UINT64 state = prevIndState;

    //если копирование закончено
    if (!copyFlash)
    {
        //гасим индикаторы
        state = prepareIndsState(state,PULT_IND_OBZOR_1, false);
        state = prepareIndsState(state,PULT_IND_OBZOR_2, false);
        state = prepareIndsState(state,PULT_IND_OBZOR_3, false);
        state = prepareIndsState(state,PULT_IND_OBZOR_4, false);

        //отсылаем сигнал на установку нового состояния индикаторов
        emit setIndS(state);
        //останавливаем таймер
        flashTimer->stop();

        return;
    }

    //запоминаем предыдущий индекс горящего индикатора
    int prevFlashInd = flashInd;

    //если следующий индекс выходит за допустимое значение
    if (flashInd++ > 4)
        //присваиваем номер первого индикатора
        flashInd = 1;

    //гасим предыдущий горящий индикатор
    switch (prevFlashInd) {
    case 1:
        state = prepareIndsState(state,PULT_IND_OBZOR_1, false);
        break;
    case 2:
        state = prepareIndsState(state,PULT_IND_OBZOR_2, false);
        break;
    case 3:
        state = prepareIndsState(state,PULT_IND_OBZOR_3, false);
        break;
    case 4:
        state = prepareIndsState(state,PULT_IND_OBZOR_4, false);
        break;
    }

    //зажигаем текущий индикатор
    switch (flashInd) {
    case 1:
        state = prepareIndsState(state,PULT_IND_OBZOR_1, true);
        break;
    case 2:
        state = prepareIndsState(state,PULT_IND_OBZOR_2, true);
        break;
    case 3:
        state = prepareIndsState(state,PULT_IND_OBZOR_3, true);
        break;
    case 4:
        state = prepareIndsState(state,PULT_IND_OBZOR_4, true);
        break;
    }

    //отсылаем сигнал на установку нового состояния индикаторов
    emit setIndS(state);
}

//Функция управления индикаторами в зависимости от состояния системы
void PPult::analizeInd()
{
    //запоминаем текущее состояние выходов платы
    prevIndState = dio->getOutState();

    //новое состояние индикаторов изначально подразумевает, что все индикаторы выключены
    UINT64 state = 0;

    //если контроль кнопок
    if (stateCommonControl)
        //переменная для зажигания всех индикаторов
        state = 0xFFFFFFFFFFFFFFFF;

    //не обновляем состояние выходов, не задействованных для индикаторов
    //биты для замков выставляем в их текущее положение
    state = prepareIndsState(state, OUT_ZAMOK_APU1, drive->zamokAPU1);
    state = prepareIndsState(state, OUT_ZAMOK_APU2, drive->zamokAPU1);
    state = prepareIndsState(state, OUT_ZAMOK_SPOIL_PERED, drive->zamokSpoilPered);
    state = prepareIndsState(state, OUT_ZAMOK_SPOIL_ZAD, drive->zamokSpoilZad);

    //если контроль кнопок
    if (stateCommonControl)
    {
        //если состояние выходов платы надо изменить
        if (state != prevIndState)
            //отсылаем сигнал на установку нового состояния
            emit setIndS(state);
        //выходим из функции
        return;
    }


    //биты для состояния исправности
    state = prepareIndsState(state, PULT_IND_STATUS_APU, stateStatusAPU);
    state = prepareIndsState(state, PULT_IND_STATUS_LVS, stateStatusLVS);
    state = prepareIndsState(state, PULT_IND_STATUS_OES, stateStatusOES);
    state = prepareIndsState(state, PULT_IND_STATUS_SPOIL, stateStatusSPOIL);

    //в любом состоянии отображаем текущее положение антенны
    state = prepareIndsState(state, PULT_IND_POLOG_POHOD, !statePolog);
    state = prepareIndsState(state, PULT_IND_POLOG_WORK, statePolog);


    //В режиме блок никакие индикаторы, кроме уже обработанных, не горят
    if (stateSystem == BLOCK)
    {
        //если состояние выходов платы надо изменить
        if (state != prevIndState)
            //отсылаем сигнал на установку нового состояния
            emit setIndS(state);
        //выходим из функции
        return;
    }

    //далее обрабатываем общие для режимов работа и обслуживание индикаторы
    //Группа COMMON
    state = prepareIndsState(state, PULT_IND_COMMON_CONTROL, stateCommonControl);
    state = prepareIndsState(state, PULT_IND_COMMON_GK, stateGK);
    state = prepareIndsState(state, PULT_IND_COMMON_VISOKOE, stateVisokoe);

    //Если положение походное, то прерываем дальнейшую работу и отсылаем текущее состояние индикаторов
    if (statePolog == POHOD)
    {
        //если состояние выходов платы надо изменить
        if (state != prevIndState)
            //отсылаем сигнал на установку нового состояния
            emit setIndS(state);
        //выходим из функции
        return;
    }
    //если антенна разложена, то обрабатываем состояние фокуса
    else
    {
        //Группа FOCUS
        switch (stateFocus) {
        case FOCUS35:
            state = prepareIndsState(state, PULT_IND_FOCUS_35, true);
            break;
        case FOCUS50:
            state = prepareIndsState(state, PULT_IND_FOCUS_50, true);
            break;
        case FOCUS75:
            state = prepareIndsState(state, PULT_IND_FOCUS_75, true);
            break;
        case FOCUS100:
            state = prepareIndsState(state, PULT_IND_FOCUS_100, true);
            break;
        case FOCUSAUTO:
            state = prepareIndsState(state, PULT_IND_FOCUS_AUTO, true);
            break;
        default:
            break;
        }
    }

    //В режиме обслуживания никакие индикаторы, кроме уже обработанных, не горят
    if (stateSystem == OBSLUG)
    {
        //если состояние выходов платы надо изменить
        if (state != prevIndState)
            //отсылаем сигнал на установку нового состояния
            emit setIndS(state);
        //выходим из функции
        return;
    }

    //обрабатываем состояние индикаторов группы экран
    switch (stateScreen) {
    case MULTI:
        state = prepareIndsState(state, PULT_IND_SCREEN_MULTI, true);
        break;
    case NAVED:
        state = prepareIndsState(state, PULT_IND_SCREEN_NAVED, true);
        break;
    case KAMERA:
        state = prepareIndsState(state, PULT_IND_SCREEN_KAMERA, true);
        break;
    case OBZOR:
        state = prepareIndsState(state, PULT_IND_SCREEN_OBZOR, true);
        break;
    }

    //отображаем состояние индикаторов абонента
    state = prepareIndsState(state, PULT_IND_UNKNOWN_HP1, stateHP1);
    state = prepareIndsState(state, PULT_IND_UNKNOWN_HP2, stateHP2);
    state = prepareIndsState(state, PULT_IND_UNKNOWN_HP3, stateHP3);
    state = prepareIndsState(state, PULT_IND_UNKNOWN_HP4, stateHP4);
    state = prepareIndsState(state, PULT_IND_UNKNOWN_HP5, stateHP5);
    state = prepareIndsState(state, PULT_IND_UNKNOWN_HP6, stateHP6);
    state = prepareIndsState(state, PULT_IND_UNKNOWN_HP7, stateHP7);
    state = prepareIndsState(state, PULT_IND_UNKNOWN_HP8, stateHP8);
    state = prepareIndsState(state, PULT_IND_UNKNOWN_HP9, stateHP9);
    state = prepareIndsState(state, PULT_IND_UNKNOWN_IP1, stateIP1);
    state = prepareIndsState(state, PULT_IND_UNKNOWN_IP2, stateIP2);
    state = prepareIndsState(state, PULT_IND_UNKNOWN_IP3, stateIP3);

    //ЗДЕСЬ необходимо уточнить работу индикаторов в различных режимах

    //если не режим наведения и не обзор
    if (stateScreen != NAVED && stateScreen != OBZOR)
    {
        //обрабатываем индикаторы номера камеры группы обзор
        switch (drive->numCam) {
        case CAM1:
            state = prepareIndsState(state, PULT_IND_OBZOR_1, true);
            break;
        case CAM2:
            state = prepareIndsState(state, PULT_IND_OBZOR_2, true);
            break;
        case CAM3:
            state = prepareIndsState(state, PULT_IND_OBZOR_3, true);
            break;
        case CAM4:
            state = prepareIndsState(state, PULT_IND_OBZOR_4, true);
            break;
        default:
            break;
        }
        //обрабатываем остальные индикаторы группы обзор
        state = prepareIndsState(state, PULT_IND_OBZOR_A, stateObzorA);
        state = prepareIndsState(state, PULT_IND_OBZOR_C, stateObzorC);
        state = prepareIndsState(state, PULT_IND_OBZOR_ZOOM_IN, stateObzorZoomIn);
        state = prepareIndsState(state, PULT_IND_OBZOR_ZOOM_OUT, stateObzorZoomOut);
    }

    //если режим не обзор
    if (stateScreen != OBZOR)
    {
        //индикатор режима автосопровождения
        state = prepareIndsState(state, PULT_IND_COMMON_AC, stateCommonAC);
        //индикаторы режима наведения
        switch (stateNaved) {
        case NAVED_STATE_MANUAL:
            state = prepareIndsState(state, PULT_IND_NAVED_MANUAL, true);
            break;
        case NAVED_STATE_NAVESTI:
            state = prepareIndsState(state, PULT_IND_NAVED_NAVESTI, true);
            break;
        case NAVED_STATE_PA:
            state = prepareIndsState(state, PULT_IND_NAVED_PA, true);
            break;
        }
        //если режим не камера
        if (stateScreen != KAMERA)
        {
            //остальные индикаторы группы наведения
            state = prepareIndsState(state, PULT_IND_NAVED_NIGHT, stateNavedNight);
            state = prepareIndsState(state, PULT_IND_NAVED_ZOOM_IN, stateNavedZoomIn);
            state = prepareIndsState(state, PULT_IND_NAVED_ZOOM_OUT, stateNavedZoomOut);
        }
    }

    //если состояние выходов платы надо изменить
    if (state != prevIndState)
        //отсылаем сигнал на установку нового состояния
        emit setIndS(state);
}


//Получение состояния пульта
bool PPult::getStatus()
{
    return status;
}

//Получение состояния режима работы программы
int PPult::getStateSystem()
{
    return stateSystem;
}

//Получение состояния положения антенны
bool PPult::getStatePolog()
{
    return statePolog;
}


//функция обработки изменения состояния кнопки
//ДОРАБОТАТЬ в плане проверки возможности обработки кнопки в данном режиме
void PPult::newState(int num, bool state)
{
    //Если режим обслуживания, но отправляем команду по сети для отображения в программе видеообработки
    if (stateSystem == OBSLUG && state)
        sendCommand(DR_COMMAND_PULT_BTN, 0, num);

    switch(num)
    {
    //Контроль ламп возможен в любом режиме
    case PULT_BTN_COMMON_CONTROL:
        stateCommonControl = state;
        break;
    //переключатель РАБОТА/БЛОК/ОБСЛУЖИВАНИЕ
    case PULT_BTN_COMMON_WORK:
        stateWork = state;
        //если переключатель установлен в положение РАБОТА
        if (state)
        {
            //выставляем режим работа
            stateSystem = WORK;
            //отправляем команду по сети
            emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_COMMON_WORK);

        }
        //если переключатель РАБОТА сброшен
        else
            //если выбран режим БЛОК
            if (!stateWork && !stateObslug)
            {
                //выставляем режим блок
                stateSystem = BLOCK;
                //отправляем команду по сети
                emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_COMMON_BLOCK);
            }
        break;
    //переключатель РАБОТА/БЛОК/ОБСЛУЖИВАНИЕ (аналогично предыдущему)
    case PULT_BTN_COMMON_OBSLUG:
        stateObslug = state;
        if (state)
        {
            stateSystem = OBSLUG;
            emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_COMMON_OBSLUG);
            //запоминаем состояние наведения
            stateNaved = NAVED_STATE_MANUAL;
            //устанавливаем состояние наведения в объекте управления
            drive->navedState = NAVED_STATE_MANUAL;
            emit sendCommand(CP_COMMAND_NAVED_STATE, 0, PULT_BTN_NAVED_MANUAL);
        }
        else
            if (!stateWork && !stateObslug)
            {
                stateSystem = BLOCK;
                emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_COMMON_BLOCK);
                qDebug() << "Обработка PULT_BTN_COMMON_OBSLUG BLOCK";
            }
        break;
    //кнопка дальность
    case PULT_BTN_COMMON_DALNOST:
        //если система не в режиме блок
        if (stateSystem != BLOCK && statePolog == WORKING)
        {
            //если нажата
            if (state)
            {
                //обнуляем количество секунду удержания кнопки дальность
                numSecDalnost = 0;
                //запускаем таймер измерения времени удержания кнопки
                timerDalnost->start();
                //запускаем дальномер на измерение
                emit dalnomerStart();
                //устанавливаем признак измерения дальности
                stateCommonDalnost = true;
            }
            else
            {
                //останавливаем таймер измерения времени удержания кнопки
                timerDalnost->stop();
                //если удерживали кнопку менее двух секунду
                if (numSecDalnost < TIME_SET_ENABLE_DALNOST)
                {
                    //сбрасываем признак измерения дальности
                    stateCommonDalnost = false;
                    //останавливаем дальномер
                    emit dalnomerStop();
                }
            }
            qDebug() << "Обработка PULT_BTN_COMMON_DALNOST";            
        }
        break;
    //кнопка высокое
    case PULT_BTN_COMMON_VISOKOE:
        //если система в рабочем режиме и Высокое возможно
        if (stateSystem == WORK && stateGK && statePolog == WORKING)
        {
            //запоминаем состояние кнопки
            stateCommonVisokoe = state;
            //отправляем сигнал для отправки команды стороннему абоненту
            emit sendAbonent(PULT_BTN_COMMON_VISOKOE, state);
            qDebug() << "Обработка PULT_BTN_COMMON_VISOKOE";
        }
        break;
    //кнопка резерв
    case PULT_BTN_COMMON_RESERV:
        //запоминаем состояние кнопки
        stateCommonReserv = state;
        break;
    //кнопка АС - автосопровождение
    case PULT_BTN_COMMON_AC:
        //если рабочий режим, режим не обзор и рабочее положение антенны
        if (stateSystem == WORK && stateScreen != OBZOR && statePolog == WORKING)
        {
            //если кнопка нажата
            if (state)
            {
                //если режим не был задан
                if (!stateAC)
                {
                    //задаем режим
                    stateAC = true;
                    //устанавливаем данные режим в объекте управления движением
                    drive->stateAC = true;
                    //выставляем признак автосопровождения
                    stateCommonAC = true;
                    //отправляем команду программе видеобработки для начала выделения объекта
                    sendCommand(CP_COMMAND_AC, 0, AC_BEGIN);
                }
                //если режим был задан
                else
                {
                    //сбрасываем режим
                    stateAC = false;
                    //сбрасываем данные режим в объекте управления движением
                    drive->stateAC = false;
                    //сбрасываем признак автосопровождения
                    stateCommonAC = false;
                    //отправляем команду программе видеобработки для окончания автосопровождения
                    sendCommand(CP_COMMAND_AC, 0, AC_BREAK);
                }
            }
            //если кнопка отжата
            else
            {
                //если задан режим
                if (stateAC)
                {
                    //посылаем команду программе видеообработки
                    sendCommand(CP_COMMAND_AC, 0, AC_END);
                }
            }

        }
        break;
    //кнопка мульти
    case PULT_BTN_SCREEN_MULTI:
        //если кнопка не нажата
        if (!state)
            break;
        //если рабочий режим
        if (stateSystem == WORK && statePolog == WORKING)
        {
            //устанавливаем режим отображения в мультиэкран
            stateScreen = MULTI;
            //отправляем команду по сети
            emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_SCREEN_MULTI);
        }
        break;
    //кнопка обзор
    case PULT_BTN_SCREEN_OBZOR:
        //если кнопка не нажата
        if (!state)
            break;
        //если рабочий режим
        if (stateSystem == WORK && statePolog == WORKING)
        {
            //устанавливаем режим отображения в обзор
            stateScreen = OBZOR;
            //передаем по сети номер текущей камеры
            switch (drive->numCam) {
            case CAM1:
                emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_OBZOR_1);
                break;
            case CAM2:
                emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_OBZOR_2);
                break;
            case CAM3:
                emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_OBZOR_3);
                break;
            case CAM4:
                emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_OBZOR_4);
                break;
            }
            //отправляем команду на изменение режима отображения
            emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_SCREEN_OBZOR);
        }
        break;
    //кнопка наведение
    case PULT_BTN_SCREEN_NAVED:
        //если кнопка не нажата
        if (!state)
            break;
        //если рабочий режим
        if (stateSystem == WORK && statePolog == WORKING)
        {
            //устанавливаем режим отображения в наведение
            stateScreen = NAVED;
            //отправляем команду по сети
            emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_SCREEN_NAVED);
        }
        break;
    //кнопка камера
    case PULT_BTN_SCREEN_KAMERA:
        //если кнопка не нажата
        if (!state)
            break;
        //если рабочий режим
        if (stateSystem == WORK && statePolog == WORKING)
        {
            //устанавливаем режим отображения в наведение
            stateScreen = KAMERA;
            //отправляем команду по сети
            emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_SCREEN_KAMERA);
        }
        break;
    //кнопка НР1  (остальные кнопки аналогично этой)
    case PULT_BTN_UNKNOWN_HP1:
        //если рабочий режим
        if (stateSystem == WORK)
        {
            //запоминаем состояние кнопки
            stateUnknownHP1 = state;
            //отправляем сигнал для отправки команды стороннему абоненту
            emit sendAbonent(PULT_BTN_UNKNOWN_HP1, state);
            qDebug() << "Обработка PULT_BTN_UNKNOWN_HP1";
        }
        break;
    case PULT_BTN_UNKNOWN_HP2:
        if (stateSystem == WORK)
        {
            stateUnknownHP2 = state;
            emit sendAbonent(PULT_BTN_UNKNOWN_HP2, state);
            qDebug() << "Обработка PULT_BTN_UNKNOWN_HP2";
        }
        break;
    case PULT_BTN_UNKNOWN_HP3:
        if (stateSystem == WORK)
        {
            stateUnknownHP3 = state;
            emit sendAbonent(PULT_BTN_UNKNOWN_HP3, state);
            qDebug() << "Обработка PULT_BTN_UNKNOWN_HP3";
        }
        break;
    case PULT_BTN_UNKNOWN_HP4:
        if (stateSystem == WORK)
        {
            stateUnknownHP4 = state;
            emit sendAbonent(PULT_BTN_UNKNOWN_HP4, state);
            qDebug() << "Обработка PULT_BTN_UNKNOWN_HP4";
        }
        break;
    case PULT_BTN_UNKNOWN_HP5:
        if (stateSystem == WORK)
        {
            stateUnknownHP5 = state;
            emit sendAbonent(PULT_BTN_UNKNOWN_HP5, state);
            qDebug() << "Обработка PULT_BTN_UNKNOWN_HP5";
        }
        break;
    case PULT_BTN_UNKNOWN_HP6:
        if (stateSystem == WORK)
        {
            stateUnknownHP6 = state;
            emit sendAbonent(PULT_BTN_UNKNOWN_HP6, state);
            qDebug() << "Обработка PULT_BTN_UNKNOWN_HP6";
        }
        break;
    case PULT_BTN_UNKNOWN_HP7:
        if (stateSystem == WORK)
        {
            stateUnknownHP7 = state;
            emit sendAbonent(PULT_BTN_UNKNOWN_HP7, state);
            qDebug() << "Обработка PULT_BTN_UNKNOWN_HP7";
        }
        break;
    case PULT_BTN_UNKNOWN_HP8:
        if (stateSystem == WORK)
        {
            stateUnknownHP8 = state;
            emit sendAbonent(PULT_BTN_UNKNOWN_HP8, state);
            qDebug() << "Обработка PULT_BTN_UNKNOWN_HP8";
        }
        break;
    case PULT_BTN_UNKNOWN_HP9:
        if (stateSystem == WORK)
        {
            stateUnknownHP9 = state;
            emit sendAbonent(PULT_BTN_UNKNOWN_HP9, state);
            qDebug() << "Обработка PULT_BTN_UNKNOWN_HP9";
        }
        break;
    case PULT_BTN_UNKNOWN_IP1:
        if (stateSystem == WORK)
        {
            stateUnknownIP1 = state;
            emit sendAbonent(PULT_BTN_UNKNOWN_IP1, state);
            qDebug() << "Обработка PULT_BTN_UNKNOWN_IP1";
        }
        break;
    case PULT_BTN_UNKNOWN_IP2:
        if (stateSystem == WORK)
        {
            stateUnknownIP2 = state;
            emit sendAbonent(PULT_BTN_UNKNOWN_IP2, state);
            qDebug() << "Обработка PULT_BTN_UNKNOWN_IP2";
        }
        break;
    case PULT_BTN_UNKNOWN_IP3:
        if (stateSystem == WORK)
        {
            stateUnknownIP3 = state;
            emit sendAbonent(PULT_BTN_UNKNOWN_IP3, state);
            qDebug() << "Обработка PULT_BTN_UNKNOWN_IP3";
        }
        break;
    //кнопка сложить антенну
    case PULT_BTN_POLOG_SLOG:
        //если кнопка не нажата
        if (!state)
            break;
        //если режим не блок и антенна развернута
        if (stateSystem != BLOCK && drive->polog == WORKING)
        {
            //вызываем функцию cкладывания антенны
            drive->setPolog(POHOD);
            //запоминаем состояние кнопки
            statePologSlog = state;
            qDebug() << "Обработка PULT_BTN_POLOG_SLOG";
        }
        break;
        //кнопка разложить антенну
    case PULT_BTN_POLOG_RAZV:
        //если кнопка не нажата
        if (!state)
            break;
        //если режим не блок и антенна сложена
        if (stateSystem != BLOCK && statePolog == POHOD)
        {
            //вызываем функцию разкладывания антенны
            drive->setPolog(WORKING);
            //запоминаем состояние кнопки
            statePologSlog = state;
            qDebug() << "Обработка PULT_BTN_POLOG_RAZV";
        }
        break;

    //кнопка фокус 35
    case PULT_BTN_FOCUS_35:
        //если кнопка не нажата
        if (!state)
            break;
        //если режим не блок, антенна разложена, фокус не в заданном положении
        if (stateSystem != BLOCK && statePolog == WORKING && stateFocus != FOCUS35)
        {
            //вызываем функцию изменения фокусного расстояния
            drive->setNewFocus(FOCUS_35);
            //запоминаем фокусное расстояние
            stateFocus = FOCUS35;
            qDebug() << "Обработка PULT_BTN_FOCUS_35";
        }
        break;
    //кнопка фокус 50
    case PULT_BTN_FOCUS_50:
        //если кнопка не нажата
        if (!state)
            break;
        //если режим не блок, антенна разложена, фокус не в заданном положении
        if (stateSystem != BLOCK && statePolog == WORKING && stateFocus != FOCUS50)
        {
            //вызываем функцию изменения фокусного расстояния
            drive->setNewFocus(FOCUS_50);
            //запоминаем фокусное расстояние
            stateFocus = FOCUS50;
            qDebug() << "Обработка PULT_BTN_FOCUS_50";
        }
        break;
    //кнопка фокус 75
    case PULT_BTN_FOCUS_75:
        //если кнопка не нажата
        if (!state)
            break;
        //если режим не блок, антенна разложена, фокус не в заданном положении
        if (stateSystem != BLOCK && statePolog == WORKING && stateFocus != FOCUS75)
        {
            //вызываем функцию изменения фокусного расстояния
            drive->setNewFocus(FOCUS_75);
            //запоминаем фокусное расстояние
            stateFocus = FOCUS75;
            qDebug() << "Обработка PULT_BTN_FOCUS_75";
        }
        break;
    //кнопка фокус 100
    case PULT_BTN_FOCUS_100:
        //если кнопка не нажата
        if (!state)
            break;
        //если режим не блок, антенна разложена, фокус не в заданном положении
        if (stateSystem != BLOCK && statePolog == WORKING && stateFocus != FOCUS100)
        {
            //вызываем функцию изменения фокусного расстояния
            drive->setNewFocus(FOCUS_100);
            //запоминаем фокусное расстояние
            stateFocus = FOCUS100;
            qDebug() << "Обработка PULT_BTN_FOCUS_100";
        }
        break;
    //кнопка фокус AUTO
    case PULT_BTN_FOCUS_AUTO:
        //если кнопка не нажата
        if (!state)
            break;
        //если режим не блок, антенна разложена, фокус не в заданном положении
        if (stateSystem != BLOCK && statePolog == WORKING && stateFocus != FOCUSAUTO)
        {
            //вызываем функцию изменения фокусного расстояния
            drive->setNewFocus(FOCUS_AUTO);
            //запоминаем фокусное расстояние
            stateFocus = FOCUSAUTO;
            qDebug() << "Обработка PULT_BTN_FOCUS_AUTO";
        }
        break;

    //кнопка ночь
    case PULT_BTN_NAVED_NIGHT:
        //если кнопка не нажата
        if (!state)
            break;
        //если рабочий режим, режим мульти либо наведения
        if (stateSystem == WORK && (stateScreen == MULTI || stateScreen == NAVED) && statePolog == WORKING)
        {
            //меняем отображения визира/тепловизора на обратное
            stateNavedNight = !stateNavedNight;
            //отправляем команду по сети
            emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_NAVED_NIGHT);
            qDebug() << "Обработка PULT_BTN_NAVED_NIGHT";
        }
        break;
    //кнопка навести
    case PULT_BTN_NAVED_NAVESTI:
        //если кнопка не нажата
        if (!state)
            break;
        //если рабочий режим, режим наведения надо менять, режим экрана не обзор, положение антенны рабочее
        if (stateSystem == WORK && stateScreen != OBZOR &&
                stateNaved != NAVED_STATE_NAVESTI && statePolog == WORKING)
        {
            //запоминаем состояние наведения
            stateNaved = NAVED_STATE_NAVESTI;
            //устанавливаем состояние наведения в объекте управления
            drive->navedState = NAVED_STATE_NAVESTI;
            //устанавливаем признак нажатия кнопки
            drive->navedNavesti = true;
            //записываем координаты для наведения
            drive->navedXgrad = drive->posXgrad;
            drive->navedXmin = drive->posXmin;
            drive->navedYgrad = drive->posYgrad;
            drive->navedYmin = drive->posYmin;
            //выдаем команду по сети о нажатии кнопки навести
            emit sendCommand(CP_COMMAND_NAVED_STATE, 0, PULT_BTN_NAVED_NAVESTI);
            qDebug() << "Обработка PULT_BTN_NAVED_NAVESTI";
        }
        break;
    //кнопка П/А
    case PULT_BTN_NAVED_PA:
        //если кнопка не нажата
        if (!state)
            break;
        //если рабочий режим, режим наведения надо менять, режим экрана не обзор, положение антенны рабочее
        if (stateSystem == WORK && stateScreen != OBZOR &&
                stateNaved != NAVED_STATE_PA && statePolog == WORKING)
        {
            //запоминаем состояние наведения
            stateNaved = NAVED_STATE_PA;
            //устанавливаем состояние наведения в объекте управления
            drive->navedState = NAVED_STATE_PA;
            //отправляем команду по сети
            emit sendCommand(CP_COMMAND_NAVED_STATE, 0, PULT_BTN_NAVED_PA);
            qDebug() << "Обработка PULT_BTN_NAVED_PA";
        }
        break;
    //кнопка РУЧНОЕ
    case PULT_BTN_NAVED_MANUAL:
        //если кнопка не нажата
        if (!state)
            break;
        //если рабочий режим, режим наведения надо менять, режим экрана не обзор, положение антенны рабочее
        if (stateSystem == WORK && stateScreen != OBZOR &&
                stateNaved != NAVED_STATE_MANUAL && statePolog == WORKING)
        {
            //запоминаем состояние наведения
            stateNaved = NAVED_STATE_MANUAL;
            //устанавливаем состояние наведения в объекте управления
            drive->navedState = NAVED_STATE_MANUAL;
            emit sendCommand(CP_COMMAND_NAVED_STATE, 0, PULT_BTN_NAVED_MANUAL);
        }
        break;
    //кнопка кратность + группы наведение
    case PULT_BTN_NAVED_ZOOM_IN:
        //если рабочий режим, режим мульти либо наведения
        if (stateSystem == WORK && (stateScreen == MULTI || stateScreen == NAVED) && statePolog == WORKING)
        {
            //запоминаем состояние
            stateNavedZoomIn = state;
            //устанавливаем состояние зумирования в объекте управления
            drive->navedZoomPlus = state;
            //вычисляем значение зума
            if (drive->navedZoom < ZOOM_MAX)
                drive->navedZoom += ZOOM_SHAG;
        }
        break;
    //кнопка кратность - группы наведение
    case PULT_BTN_NAVED_ZOOM_OUT:
        //если рабочий режим, режим мульти либо наведения
        if (stateSystem == WORK && (stateScreen == MULTI || stateScreen == NAVED) && statePolog == WORKING)
        {
            //запоминаем состояние
            stateNavedZoomOut = state;
            //устанавливаем состояние зумирования в объекте управления
            drive->navedZoomMinus= state;
            //вычисляем значение зума
            if (drive->navedZoom > ZOOM_MIN)
                drive->navedZoom -= ZOOM_SHAG;
        }
        break;
    //кнопка камера 1 группы обзор
    case PULT_BTN_OBZOR_1:
        //если кнопка не нажата
        if (!state)
            break;

        //если режим обслуживания, то необходимо обработать последовательность
        //нажатия кнопок 1,2,3,4 в группе обзор для копирования логов на флешку
        if (stateSystem == OBSLUG)
        {
            //отмечаем что последовательность начата
            startCopyToFlash = 1;
        }

        //если рабочий режим, режим не наведение
        if (stateSystem == WORK && (stateScreen != NAVED) && statePolog == WORKING)
        {
            //устанавливаем номер камеры в объекте управления
            drive->numCam = CAM1;
            //если режим обзор, то нажатие кнопки выбора камеры приводит к переключению в режим камера
            //и переключением на текущую выбранной камеры
            if (stateScreen == OBZOR)
                newState(PULT_BTN_SCREEN_KAMERA, true);
            else
                //отправляем номер камеры по сети
                emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_OBZOR_1);
        }
        break;
    //кнопка камера 2 группы обзор
    case PULT_BTN_OBZOR_2:
        //если кнопка не нажата
        if (!state)
            break;

        //если режим обслуживания, то необходимо обработать последовательность
        //нажатия кнопок 1,2,3,4 в группе обзор для копирования логов на флешку
        if (stateSystem == OBSLUG)
        {
            //если последовательность продолжается
            if (startCopyToFlash == 1)
                //отмечаем что последовательность начата
                startCopyToFlash = 2;
            else
                startCopyToFlash = 0;

        }

        //если рабочий режим, режим не наведение
        if (stateSystem == WORK && (stateScreen != NAVED) && statePolog == WORKING)
        {
            //устанавливаем номер камеры в объекте управления
            drive->numCam = CAM2;
            //если режим обзор, то нажатие кнопки выбора камеры приводит к переключению в режим камера
            //и переключением на текущую выбранной камеры
            if (stateScreen == OBZOR)
                newState(PULT_BTN_SCREEN_KAMERA, true);
            else
                //отправляем номер камеры по сети
                emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_OBZOR_2);
            qDebug() << "Обработка PULT_BTN_OBZOR_2";
        }
        break;
    //кнопка камера 3 группы обзор
    case PULT_BTN_OBZOR_3:
        //если кнопка не нажата
        if (!state)
            break;

        //если режим обслуживания, то необходимо обработать последовательность
        //нажатия кнопок 1,2,3,4 в группе обзор для копирования логов на флешку
        if (stateSystem == OBSLUG)
        {
            //если последовательность продолжается
            if (startCopyToFlash == 2)
                //отмечаем что последовательность начата
                startCopyToFlash = 3;
            else
                startCopyToFlash = 0;

        }

        //если рабочий режим, режим не наведение
        if (stateSystem == WORK && (stateScreen != NAVED) && statePolog == WORKING)
        {
            //устанавливаем номер камеры в объекте управления
            drive->numCam = CAM3;
            //если режим обзор, то нажатие кнопки выбора камеры приводит к переключению в режим камера
            //и переключением на текущую выбранной камеры
            if (stateScreen == OBZOR)
                newState(PULT_BTN_SCREEN_KAMERA, true);
            else
                //отправляем номер камеры по сети
                emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_OBZOR_3);
            qDebug() << "Обработка PULT_BTN_OBZOR_3";
        }
        break;
    //кнопка камера 4 группы обзор
    case PULT_BTN_OBZOR_4:
        //если кнопка не нажата
        if (!state)
            break;

        //если режим обслуживания, то необходимо обработать последовательность
        //нажатия кнопок 1,2,3,4 в группе обзор для копирования логов на флешку
        if (stateSystem == OBSLUG)
        {
            //если последовательность закончена
            if (startCopyToFlash == 3)
            {
                startCopyToFlash = 0;
                copyLogsToFlash();
                //отправляем команду для записи логов на устройство видеообработки
                emit sendCommand(CP_COMMAND_LOGS, 0, 0);
            }
            else
                startCopyToFlash = 0;

        }

        //если рабочий режим, режим не наведение
        if (stateSystem == WORK && (stateScreen != NAVED) && statePolog == WORKING)
        {
            //устанавливаем номер камеры в объекте управления
            drive->numCam = CAM4;
            //если режим обзор, то нажатие кнопки выбора камеры приводит к переключению в режим камера
            //и переключением на текущую выбранной камеры
            if (stateScreen == OBZOR)
                newState(PULT_BTN_SCREEN_KAMERA, true);
            else
                //отправляем номер камеры по сети
                emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_OBZOR_4);
            qDebug() << "Обработка PULT_BTN_OBZOR_4";
        }
        break;
    //кнопка А группы обзор: нажатие - включает, а повторное - выключает режим автоматического выбора камеры
    case PULT_BTN_OBZOR_A:
        //если кнопка не нажата
        if (!state)
            break;
        //если рабочий режим, режим мульти либо камера
        if (stateSystem == WORK && (stateScreen == MULTI || stateScreen == KAMERA) && statePolog == WORKING)
        {
            //режим автоматического выбора камеры устанавливаем на противоположные
            stateObzorA = !stateObzorA;
            //устанавливаем данный режим в объекте управления
            drive->stateObzorA = stateObzorA;
            //отправляем режим по сети
            if (stateObzorA)
                emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 1, PULT_BTN_OBZOR_A);
            else
                emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_OBZOR_A);
            qDebug() << "Обработка PULT_BTN_OBZOR_A";
        }
        break;
    //кнопка C группы обзор: нажатие - включает, а повторное - выключает режим последовательного выбора камеры
    case PULT_BTN_OBZOR_C:
        //если кнопка не нажата
        if (!state)
            break;
        //если рабочий режим, режим мульти либо камера
        if (stateSystem == WORK && (stateScreen == MULTI || stateScreen == KAMERA) && statePolog == WORKING)
        {
            //режим автоматического выбора камеры устанавливаем на противоположные
            stateObzorC = !stateObzorC;
            //устанавливаем данный режим в объекте управления
            drive->stateObzorC = stateObzorC;
            //отправляем режим по сети
            if (stateObzorC)
                emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 1, PULT_BTN_OBZOR_C);
            else
                emit sendCommand(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_OBZOR_C);
            qDebug() << "Обработка PULT_BTN_OBZOR_C";
        }
        break;
    //кнопка кратность + группы обзор
    case PULT_BTN_OBZOR_ZOOM_IN:
        //если рабочий режим, режим мульти либо камера
        if (stateSystem == WORK && (stateScreen == MULTI || stateScreen == KAMERA) && statePolog == WORKING)
        {
            //запоминаем состояние
            stateObzorZoomIn = state;
            //устанавливаем состояние зумирования в объекте управления
            drive->obzorZoomPlus = state;
            //вычисляем значение зума
            if (drive->obzorZoom < ZOOM_MAX)
                drive->obzorZoom += ZOOM_SHAG;
        }
        break;
    //кнопка кратность - группы обзор
    case PULT_BTN_OBZOR_ZOOM_OUT:
        //если рабочий режим, режим мульти либо камера
        if (stateSystem == WORK && (stateScreen == MULTI || stateScreen == KAMERA) && statePolog == WORKING)
        {
            //запоминаем состояние
            stateObzorZoomOut = state;
            //устанавливаем состояние зумирования в объекте управления
            drive->obzorZoomMinus = state;
            //вычисляем значение зума
            if (drive->obzorZoom > ZOOM_MIN)
                drive->obzorZoom -= ZOOM_SHAG;
        }
        break;
    }
    //обновляем состояние индикаторов
    analizeInd();
}

//Функция обновления состояния исправности
void PPult::updateStatus(bool apu, bool lvs, bool spoil, bool oes)
{
    //если состояние изменилось
    if (stateStatusAPU != apu)
    {
        //запоминаем новое состояние
        stateStatusLVS = lvs;
        //посылаем новое состояние
        emit newOut(PULT_IND_STATUS_APU, apu);
    }
    if (stateStatusOES != oes)
    {
        stateStatusOES = oes;
        emit newOut(PULT_IND_STATUS_OES, oes);
    }
    if (stateStatusLVS != lvs)
    {
        stateStatusLVS = lvs;
        emit newOut(PULT_IND_STATUS_LVS, lvs);
    }
    if (stateStatusSPOIL != spoil)
    {
        stateStatusSPOIL = spoil;
        emit newOut(PULT_IND_STATUS_SPOIL, spoil);
    }
}

//функция изменения положения антенны
void PPult::newPolog(bool state)
{
    statePolog = state;
    analizeInd();
}

//функция обработки таймера измерения времени удержания кнопки дальность
void PPult::timeDal()
{
    numSecDalnost++;
}

//функция получения данных от стороннего абонента
void PPult::analizeAbonentData(DATA *data)
{
    QByteArray buff;
    buff.resize(4);

    memset(&DtReceive,0,sizeof(DATA));
    memcpy(&DtReceive,data,sizeof(DATA));

    switch (DtReceive.command) {
    //команда установки режима
    case COMMAND_CLIENT_SETMODE:
        if(DtReceive.state.mode > 0)
        {
            //зажигаем соответствующий индикатор
            //rb[DtReceive.state.mode-1]->setChecked(true);
        }
        //если ни один из режимов не включен
        else
        {
            //гасим все индикаторы
        }
        memcpy(&DtSendState, &DtReceive, sizeof(DATA));
        break;
    //команда установки высокого
    case COMMAND_CLIENT_HIGHEST_ON_OFF:
//        if(DtReceive.state.highest > 0)
//            ui->checkBox_2->setChecked(true);
//        else
//            ui->checkBox_2->setChecked(false);
        memcpy(&DtSendState, &DtReceive, sizeof(DATA));
        break;
    //запрос на
    case COMMAND_CLIENT_REQUEST:
        //отправляем ответ
        if(ui->checkBox->isChecked())
            DtSendState.state.gk=1;
        else
            DtSendState.state.gk=0;
        network.sendAnswer(&DtSendState);
        break;
    }
}
