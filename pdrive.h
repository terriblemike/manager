#ifndef PDRIVE_H
#define PDRIVE_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include "ptest.h"

#include <cmath>

#include "pbmsd.h"
#include "pgsm40.h"
#include "pstaticdio.h"
//#include "ppult.h"
#include "CP_Button_Definitions.h"

#define DRIVE_FPS   10
#define MAX_LEFT_DRIVE  -119
#define MAX_RIGHT_DRIVE 119

#define POLOG_FPS   10

//#define POLOG_PAUSE_SLOG_STAGE1  (1 * POLOG_FPS)
//#define POLOG_PAUSE_SLOG_STAGE2  (2 * POLOG_FPS)
//#define POLOG_PAUSE_SLOG_STAGE3  (2 * POLOG_FPS)
//#define POLOG_PAUSE_SLOG_STAGE4  (2 * POLOG_FPS)
//#define POLOG_PAUSE_SLOG_STAGE5  (2 * POLOG_FPS)
//#define POLOG_PAUSE_SLOG_STAGE6  (2 * POLOG_FPS)
//#define POLOG_PAUSE_SLOG_STAGE7  (2 * POLOG_FPS)
//#define POLOG_PAUSE_SLOG_STAGE8  (2 * POLOG_FPS)
//#define POLOG_PAUSE_SLOG_STAGE9  (2 * POLOG_FPS)
//#define POLOG_PAUSE_SLOG_STAGE10 (2 * POLOG_FPS)

//#define POLOG_PAUSE_RAZL_STAGE1  (2 * POLOG_FPS)
//#define POLOG_PAUSE_RAZL_STAGE2  (2 * POLOG_FPS)
//#define POLOG_PAUSE_RAZL_STAGE3  (2 * POLOG_FPS)
//#define POLOG_PAUSE_RAZL_STAGE4  (2 * POLOG_FPS)
//#define POLOG_PAUSE_RAZL_STAGE5  (2 * POLOG_FPS)
//#define POLOG_PAUSE_RAZL_STAGE6  (2 * POLOG_FPS)
//#define POLOG_PAUSE_RAZL_STAGE7  (2 * POLOG_FPS)
//#define POLOG_PAUSE_RAZL_STAGE8  (2 * POLOG_FPS)
//#define POLOG_PAUSE_RAZL_STAGE9  (2 * POLOG_FPS)
//#define POLOG_PAUSE_RAZL_STAGE10 (2 * POLOG_FPS)




#define MAX_TOP_DRIVE       19
#define MAX_BOTTOM_DRIVE    -19

#define NAVED_STATE_NAVESTI 0
#define NAVED_STATE_PA      1
#define NAVED_STATE_MANUAL  2

#define FOCUS_DOPUSK    1
#define FOCUS_MAX_VALUE 50
#define ZTU_DOPUSK      5
#define ZTU_MAX_VALUE   50

#define MAX_SPEED           126
#define DIRECTION_LEFT      0
#define DIRECTION_RIGHT     1
//#define DIRECTION_DOWN      0
//#define DIRECTION_UP     1

#define POLOG_POHOD   false
#define POLOG_WORKING true

#define FOCUS_MAX   50
#define FOCUS_35    (FOCUS_MAX * 0.35)
#define FOCUS_50    (FOCUS_MAX * 0.50)
#define FOCUS_75    (FOCUS_MAX * 0.75)
#define FOCUS_100   (FOCUS_MAX)
#define FOCUS_AUTO  (FOCUS_MAX * 0.52)

#define FOCUS35    35
#define FOCUS50    50
#define FOCUS75    75
#define FOCUS100   100
#define FOCUSAUTO  52

#define TIMESTEP    0.2

//Класс для рассчета положения маркера джойстика и движения платформы
class PDrive : public QObject
{
    Q_OBJECT
public:

    explicit PDrive(QObject *parent = 0);

    //Функция инициализации
    bool Init(PBMSD *value1, PBMSD *value2, PBMSD *value3, PBMSD *value4, PBMSD *value5, PGSM40 *value6, PStaticDIO *value7);
    bool init;

    //признак имитации
    bool imitaciya;

    PBMSD *bmsd1, *bmsd2, *bmsd3, *bmsd4, *bmsd5;
    PGSM40 *gsm40;
    PStaticDIO *staticDIO;

    //положение курсора джойстика по осям в градусах и минутах
    int posXgrad, posXmin;
    int posYgrad, posYmin;

    //отклонение цели в пикселях от центра изображения визирной камеры
    int posAC_X, posAC_Y;

    //Позиция антенны по данным от энкодеров
    int posVizXgrad, posVizXmin;
    int posVizYgrad, posVizYmin;

    //скорость поворота антенны по осям
    int speedVizX, speedVizY;
    //направление движения антенны по осям
    int directionVizX, directionVizY;

    int numCam;
    //таймер рассчета движения
    QTimer *timer;
    //скорость отклонения по осям, заданная с помощью джойстика
    int speedX, speedY;
    //отклонение по осям
    int deltaX, deltaY;

    //предыдущее положение курсора
    int prevPosXgrad, prevPosXmin;
    int prevPosYgrad, prevPosYmin;
    //Фукнция для отправки данных о положении курсора по сети
    void sendPos();

    //состояние режима автоматического переключения камер
    bool stateObzorA;
    bool stateObzorC;

    //состояние режима автосопровождения
    bool stateAC;

    //Состояния режима наведения
    int navedState;
    //состояние кнопки навести
    bool navedNavesti;
    //координаты точки, в которую необходимо навести
    int navedXgrad, navedXmin,
        navedYgrad, navedYmin;

    //Работа с фокусным актуатором и актуатором ЗТУ
    QTimer *timerFocus, *timerZTU;

    int focus, ZTU;
    int newFocus, newZTU;

    //Работа с механизмом изменения положения антенны
    QTimer *timerPolog;
    //признак начала процесса изменения положения
    bool pologInit;
    //Положение антенны
    bool polog;

    //Текущий номер этапа установки положения
    int stageSetPolog;
    //Счетчик нахождения на данном этапе
    int stageCount;

    //Таймер проверки исправности
    QTimer *timerTest;

    QTimer *timerRazlSlog;

    //Количество попыток занять правильное положение
    int numTry;

    //признаки срабатывания оконечных устройств
    bool koncLeftNZ, koncLeftNO,
        koncRightNZ, koncRightNO,
        koncSpoilPerNZ, koncSpoilPerNO,
        koncSpoilZadNZ, koncSpoilZadNO,
        koncZTUNZ, koncZTUNO,
        gerkonNZ, gerkonNO;

    //количество срабатываний геркона
    int countGerkonNO, countGerkonNZ;



    bool zamokAPU1, zamokAPU2,
        zamokSpoilPered, zamokSpoilZad;

    //Признаки нажатия кнопок кратность
    bool navedZoomPlus, navedZoomMinus;
    bool obzorZoomPlus, obzorZoomMinus;

    //значение приближения
    float navedZoom;
    float obzorZoom;

    //функция обновления интерфейса в плане концевых выключателей
    void updateKoncUI();

    //int начальное положение линейного актуатора ЗТУ
    int beginZTU;

    //Признак записи нулевых позиций энкодеров в конф.файл
    bool writeEncNullPos;

signals:
    //сигнал изменения текущей камеры
    void curCam(int num);
    //сигнал для отправки команды по сети
    void sendCommand(int command, int Argument1, int Argument2);
    //скорость движения актуаторов управления положения антенной
//    void newAntennaSpeed(int speedVizX, int directionVizX, int speedVizY, int directionVizY);

    //сигнал о достижении положения ПОХОДНОЕ или РАБОЧЕЕ
    void newPolog(bool state);

    //сигнал об изменения состояния выходов платы дискретных сигналов
    void newStaticDIOOut(int num, bool state);

    //сигнал изменения интерфейса в плане концевого выключателя
    void updateKonc(int num, bool state);


    //Сигналы обновления интерфейса программы в части данных БМСД
    void updateBmsd1UI();
    void updateBmsd2UI();
    void updateBmsd3UI();
    void updateBmsd4UI();
    void updateBmsd5UI();
    void updateGsm40UI();

    //сигналы для изменения углов в интерфейсе
    void newDataUgol(int grad, int min);
    void newDataAzim(int grad, int min);
    //сигнал записи нулевых позиций энкодеров в конф. файл
    void writeEncNPos();

public slots:

    //Слот для обработки новых отклонений по осям джойстика
    void newJoyData(int x, int y);
    //Функция обработки таймера
    void calc();

    void driveFocus();
    void driveZTU();

    void setFocus(int value);
    void setZTU(int value);

    void setNewFocus(int value);
    void setNewZTU(int value);

    void pologTimerSlog();
    void pologTimerRazl();
    //Функция изменения положения антенны (походное, рабочее)
    void setPolog(bool state);

    //положение цели в режиме автосопровождения
    void posTargetAC(int x, int y);

    //функция обработки изменения состояния входов платы дискретных сигналов
    void newStaticDIOState(int num, bool state);


};

#endif // PDRIVE_H
