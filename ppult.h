#ifndef PPULT_H
#define PPULT_H

#include <QObject>
#include "pstaticdio.h"
#include "pdrive.h"
#include "anetwork.h"

#include "CP_Button_Definitions.h"
#include <QTimer>
#include <QDir>

#define MULTI   1
#define KAMERA  2
#define NAVED   3
#define OBZOR   4
#define TEST    5

#define BLOCK   1
#define WORK    2
#define OBSLUG  3

#define CAM1    1
#define CAM2    2
#define CAM3    3
#define CAM4    4
#define VIZ     5

#define POHOD   false
#define WORKING true

#define TIME_SET_ENABLE_DALNOST    2   //время, после которого удержание кнопки дальность включает постоянное измерение дальности


class PPult : public QObject
{
    Q_OBJECT
public:
    explicit PPult(QObject *parent = 0);
    ~PPult();

    //Функция инициализации
    bool Init(PStaticDIO *DIO, PDrive *DRIVE, ANetwork *ANETWORK);

    bool init;

    //Указатель на объект для обмена данными по сети со сторонним абонентом
    ANetwork *anetwork;

    //Структуры данных для обмена данными со сторонним абонентом
    DATA DtReceive, DtSendState;

    //Таймер для отображения процесса записи логов на флешку с помощью индикаторов
    QTimer *flashTimer;
    //индекс текущего индикатора для процесса записи на флешку
    int flashInd;
    //признак окончания копирования на флешку
    bool copyFlash;
    //индекс для отслеживания нажатия последовательности кнопок для записи на флешку
    bool startCopyToFlash;
    //Функция записи логов на флешку
    void copyLogsToFlash();

    //Функции установки контроля состояния
    void setStateStatusAPU(bool state);
    void setStateStatusOEC(bool state);
    void setStateStatusLVS(bool state);
    void setStateStatusSPOIL(bool state);

    //Функция установки положения антенны
    void setStatePolog(bool state);

    //Функция установки признака обработки нажатия высокого
    void setStateVisokoe(bool state);
    //Функция установки признака возможности нажатия высокого
    void setStateGK(bool state);

    //Функция проверки возможности обработки нажатия кнопки в текущем состоянии системы
//    bool btnEnable(int btn);

    //Получение состояния пульта
    bool getStatus();

    //Получение состояния режима работы программы
    int getStateSystem();
    //Получение состояния положения антенны
    bool getStatePolog();

    //Функция управления индикаторами в зависимости от состояния системы
    void analizeInd();
private:
    //таймер для измерения времени нажатия кнопки дальность
    QTimer *timerDalnost;
    //количество секунду с нажатия кнопки дальность
    int numSecDalnost;

    //Указатель на объект для работы с платой
    PStaticDIO *dio;
    //Указатель на объект для работы с классом управления
    PDrive *drive;

    //состояние пульта
    bool status;

    //Состояния системы
    int stateScreen;   //режим экрана
    bool stateWork;
    bool stateObslug;
    int stateSystem;   //режим работы пульта

    //Предыдущее состояния индикаторов
    UINT prevIndState;

    //состояние исправности системы по группам
    bool stateStatusAPU;
    bool stateStatusOES;
    bool stateStatusLVS;
    bool stateStatusSPOIL;

    int stateFocus;

    bool stateNavedNight;

    int stateNaved;

    bool stateNavedZoomIn;
    bool stateNavedZoomOut;

    int stateObzorNumCam;
    bool stateObzorZoomIn;
    bool stateObzorZoomOut;
    bool stateObzorA;
    bool stateObzorC;

    bool stateUnknownHP1;
    bool stateUnknownHP2;
    bool stateUnknownHP3;
    bool stateUnknownHP4;
    bool stateUnknownHP5;
    bool stateUnknownHP6;
    bool stateUnknownHP7;
    bool stateUnknownHP8;
    bool stateUnknownHP9;
    bool stateUnknownIP1;
    bool stateUnknownIP2;
    bool stateUnknownIP3;

    bool statePolog;
    bool statePologSlog;
    bool statePologRazv;

    bool stateCommonControl;
    bool stateCommonAC;
    bool stateCommonGK;
    bool stateCommonReserv;
    bool stateCommonDalnost;
    bool stateCommonVisokoe;

    //состояние работы высокого от абонента
    bool stateVisokoe;
    //состояние ГК от абонента
    bool stateGK;
    //состояние индикаторов группы Unknown
    bool stateHP1;
    bool stateHP2;
    bool stateHP3;
    bool stateHP4;
    bool stateHP5;
    bool stateHP6;
    bool stateHP7;
    bool stateHP8;
    bool stateHP9;
    bool stateIP1;
    bool stateIP2;
    bool stateIP3;

    //признак режима автосопровождение
    bool stateAC;

//    //Смещение по осям джойстика
//    int joyX;
//    int joyY;
//    //Смещение по осям джойстика (предыдущее положение джойстика)
//    int prevJoyX;
//    int prevJoyY;
//    //Состояние нажатия кнопки на джойстике
//    bool joyBtn;

    //Функция формирования 64-битного слова состояния индикаторов
    UINT64 prepareIndsState(UINT64 prevstate, int num, bool state);
signals:
    //сигнал запуска измерения дальномера
    void dalnomerStart();
    //сигнал останова измерения дальномера
    void dalnomerStop();

    //сигнал для изменения состояния индикатора
    void newOut(int num, bool state);
    //сигнал для изменения состояния всех индикаторов одновременно
    void setIndS(UINT64 value);
    //сигнал для отправки команды по сети
    void sendCommand(int command, int Argument1, int Argument2);
    //сигнал изменения состояния кнопки
    void newIn(int num, bool state);
    //сигнал для стороннего абонента об измении состояния их кнопок
    void sendAbonent(int num, bool state);
public slots:
    //Функция таймера копирования на флешку
    void flashTimerFunc();
    //функция обработки изменения состояния кнопки
    void newState(int num, bool state);
    //Функция обновления состояния исправности
    void updateStatus(bool apu, bool lvs, bool spoil, bool oes);
    //функция изменения положения антенны
    void newPolog(bool state);
    //функция обработки таймера измерения времени удержания кнопки дальность
    void timeDal();
    //функция получения данных от стороннего абонента
    void analizeAbonentData(DATA* data);

    //функция обработки нажатия кнопки джойстика
//    void newJoyBtn(bool state);
};

#endif // PPULT_H
