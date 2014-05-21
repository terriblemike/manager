#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QScrollBar>
#include <QTimer>
#include <QSettings>
#include <QPushButton>

#include "pserialport.h"
#include "pencoder.h"
#include "pdalnomer.h"
#include "pbmsd.h"
#include "pgsm40.h"
#include "pbmsdport.h"
#include "ppotenciometr.h"
#include "pstaticdio.h"
#include "ppult.h"
#include "pjoystick.h"
#include "pdrive.h"

#include "CP_Button_Definitions.h"

#include "network.h"
#include "anetwork.h"

//-------Определения программы---------------------------------
// Конфигурационный файл программы
extern const char* FILE_SETTINGS;
#define STATUS_SUCCESS  "успешно"
#define STATUS_ERROR    "ошибка"

#define DEFAULT_STATUSQPS   10000

//-------Дальномера---------------------------------
extern QString DALNOMER_PORT;
#define DALNOMER_BAUD_RATE       19200
#define DALNOMER_DATA_BITS       8
#define DALNOMER_PARITY          0
#define DALNOMER_STOP_BITS       1
#define DALNOMER_FLOW_CONTROL    0

#define DALNOMER_QPS             10

//-------Энкодеры---------------------------------------------------------------
extern QString ENCODER_PORT_UGOL;
extern QString ENCODER_PORT_AZIM;
#define ENCODER_BAUD_RATE       38400
#define ENCODER_DATA_BITS       8
#define ENCODER_PARITY          0
#define ENCODER_STOP_BITS       1
#define ENCODER_FLOW_CONTROL    0

#define ENCODER_QPS             50


//-------GSM40------------------------------------------------------------------
extern QString GSM40_PORT;

#define GSM40_ADRESS          1

#define GSM40_BAUD_RATE       19200
#define GSM40_DATA_BITS       8
#define GSM40_PARITY          0
#define GSM40_STOP_BITS       2
#define GSM40_FLOW_CONTROL    0

#define GSM40_DEFAULT_USKORENIE   10
#define GSM40_DEFAULT_TORMOGENIE  10
//-------БМСД------------------------------------------------------------------
extern QString BMSD3_PORT;
extern QString BMSD2_PORT;

#define BMSD_BAUD_RATE       9600
#define BMSD_DATA_BITS       8
#define BMSD_PARITY          0
#define BMSD_STOP_BITS       1
#define BMSD_FLOW_CONTROL    0

#define BMSD_ADRESS_1        0x01
#define BMSD_ADRESS_2        0x02
#define BMSD_ADRESS_3        0x03
#define BMSD_ADRESS_4        0x04
#define BMSD_ADRESS_5        0x05

#define BMSD_DIRECTION_LEFT     0
#define BMSD_DIRECTION_RIGHT    1

#define DEFAULT_USKORENIE   24
#define DEFAULT_TORMOGENIE  24

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool readSettings();

    void StringToByteArray(QString &src, QByteArray &dst);
private slots:
    //Функция запроса состояний устройств по таймеру
    void getStatus();
    //Функция обработки сигнала от энкодера угла\места о поступлении новых данных
    void newDataFromEncUgol();
    //Функция обработки сигнала от энкодера азимута о поступлении новых данных
    void newDataFromEncAzim();

    //Функция обработки сигнала для изменения данных угла\места
    void newDataUgol(int grad, int min);
    //Функция обработки сигнала для изменения данных азимута
    void newDataAzim(int grad, int min);
    //Функция записи нулевых позиций энкодеров в конф.файл
    void writeEncNullPos();

    //Функция обработки сигнала от дальномера о поступлении новых данных
    void newDataFromDalnomer();

    //Функция обработки сигнала от потенциометров о поступлении новых данных
    void newDataFromPotenciometr1(int arg1);
    void newDataFromPotenciometr2(int arg1);

    //Функция обработки сигналов от джойстика
    void joystickPressBtn(bool state);
    void joystickNewData(int x, int y);

    //Функция задания скоростей моторов управления положением антенны
//    void setDriveSpeed(int speedVizX, int directionVizX, int speedVizY, int directionVizY);

    //Функции обновления интерфейса БМСД
    void updateBmsd1UI();
    void updateBmsd2UI();
    void updateBmsd3UI();
    void updateBmsd4UI();
    void updateBmsd5UI();
    void updateGsm40UI();

    //функция обновления интерфейса концевых выключателей
    void updateKonc(int num, bool state);

    //Функция обновления интерфейса
    void updateStatusUI(bool apu, bool lvs, bool spoil, bool oes);

    //Фукнция изменения индикаторов положения антенны в интерфейсе
//    void updatePultPolog(bool state);

    //Функция обработки индикации в интерфейсе
    void setPultUIInd(int num, bool state);

    //Функция зажигания всех индикаторов в интерфейсе
    void setAllBtnState(UINT64 state);

    void on_sbSetAdressBMSD2_clicked();

    void on_sbAbortAdressBMSD2_clicked();

    void on_sbSetAdressBMSD3_clicked();

    void on_sbAbortAdressBMSD3_clicked();

    //-----------------------------------------------БМСД1
//    void on_sbUskBmsd_1_valueChanged(int arg1);

//    void on_sbTormozBmsd_1_valueChanged(int arg1);

//    void on_sbSpeedBmsd_1_valueChanged(int arg1);

    void on_btnDirectLeftBmsd_1_clicked();

    void on_btnDirectRightBmsd_1_clicked();

    //-----------------------------------------------БМСД2
//    void on_sbUskBmsd_2_valueChanged(int arg1);

//    void on_sbTormozBmsd_2_valueChanged(int arg1);

//    void on_sbSpeedBmsd_2_valueChanged(int arg1);

    void on_btnDirectLeftBmsd_2_clicked();

    void on_btnDirectRightBmsd_2_clicked();

    //-----------------------------------------------БМСД3
//    void on_sbUskBmsd_3_valueChanged(int arg1);

//    void on_sbTormozBmsd_3_valueChanged(int arg1);

//    void on_sbSpeedBmsd_3_valueChanged(int arg1);

    void on_btnDirectLeftBmsd_3_clicked();

    void on_btnDirectRightBmsd_3_clicked();

    //-----------------------------------------------БМСД4
//    void on_sbUskBmsd_4_valueChanged(int arg1);

//    void on_sbTormozBmsd_4_valueChanged(int arg1);

//    void on_sbSpeedBmsd_4_valueChanged(int arg1);

    void on_btnDirectLeftBmsd_4_clicked();

    void on_btnDirectRightBmsd_4_clicked();

    //-----------------------------------------------БМСД5
//    void on_sbUskBmsd_5_valueChanged(int arg1);

//    void on_sbTormozBmsd_5_valueChanged(int arg1);

//    void on_sbSpeedBmsd_5_valueChanged(int arg1);

    void on_btnDirectLeftBmsd_5_clicked();

    void on_btnDirectRightBmsd_5_clicked();

    //-----------------------------------------------БМСД6
//    void on_sbUskBmsd_6_valueChanged(int arg1);

//    void on_sbTormozBmsd_6_valueChanged(int arg1);

//    void on_sbSpeedBmsd_6_valueChanged(int arg1);

    void on_btnDirectLeftBmsd_6_clicked();

    void on_btnDirectRightBmsd_6_clicked();

    //----------------------------------------------ПОТЕНЦИОМЕТРЫ



    //------------------------------------------ ПУЛЬТ
    void on_btnPultScreenMulti_pressed();

    void on_btnPultScreenObzor_pressed();

    void on_btnPultScreenNaved_pressed();

    void on_btnPultScreenCamera_pressed();

    void on_btnPultUnknownHP1_pressed();

    void on_btnPultUnknownHP2_pressed();

    void on_btnPultUnknownHP3_pressed();

    void on_btnPultUnknownHP4_pressed();

    void on_btnPultUnknownHP5_pressed();

    void on_btnPultUnknownHP6_pressed();

    void on_btnPultUnknownHP7_pressed();

    void on_btnPultUnknownHP8_pressed();

    void on_btnPultUnknownHP9_pressed();

    void on_btnPultUnknownIP1_pressed();

    void on_btnPultUnknownIP2_pressed();

    void on_btnPultUnknownIP3_pressed();

    void on_btnPultFocus35_pressed();

    void on_btnPultFocus50_pressed();

    void on_btnPultFocus75_pressed();

    void on_btnPultFocus100_pressed();

    void on_btnPultFocusAuto_pressed();

    void on_btnPultNavedNight_pressed();

    void on_btnPultNavedNavesti_pressed();

    void on_btnPultNavedPA_pressed();

    void on_btnPultNavedManual_pressed();

    void on_btnPultNavedZoomIn_pressed();

    void on_btnPultNavedZoomIn_released();

    void on_btnPultNavedZoomOut_pressed();

    void on_btnPultNavedZoomOut_released();

    void on_btnPultObzor1_pressed();

    void on_btnPultObzor2_pressed();

    void on_btnPultObzor3_pressed();

    void on_btnPultObzor4_pressed();

    void on_btnPultObzorA_pressed();

    void on_btnPultObzorC_pressed();

    void on_btnPultObzorZoomIn_pressed();

    void on_btnPultObzorZoomIn_released();

    void on_btnPultObzorZoomOut_pressed();

    void on_btnPultObzorZoomOut_released();

    void on_btnPultCommonReserv_pressed();

    void on_btnPultCommonDalnost_pressed();

    void on_btnPultCommonDalnost_released();

    void on_btnPultCommonVisokoe_pressed();

    void on_btnPultCommonVisokoe_released();

    void on_btnPultCommonControl_pressed();

    void on_btnPultCommonControl_released();

    void on_btnPultPologSlog_pressed();

    void on_btnPultPologRazv_pressed();



    void on_btnSetUskBmsd_1_clicked();

    void on_btnSetTormozBmsd_1_clicked();

    void on_btnSetSpeedBmsd_1_clicked();

    void on_btnSetUskBmsd_2_clicked();

    void on_btnSetTormozBmsd_2_clicked();

    void on_btnSetSpeedBmsd_2_clicked();

    void on_btnSetUskBmsd_3_clicked();

    void on_btnSetTormozBmsd_3_clicked();

    void on_btnSetSpeedBmsd_3_clicked();

    void on_btnSetUskBmsd_4_clicked();

    void on_btnSetTormozBmsd_4_clicked();

    void on_btnSetSpeedBmsd_4_clicked();

    void on_btnSetUskBmsd_5_clicked();

    void on_btnSetTormozBmsd_5_clicked();

    void on_btnSetSpeedBmsd_5_clicked();

    void on_btnSetUskBmsd_6_clicked();

    void on_btnSetTormozBmsd_6_clicked();

    void on_btnSetSpeedBmsd_6_clicked();

    void on_btnPultCommonStateSystem_clicked();

    void on_cbKoncLeftNZ_stateChanged(int arg1);

    void on_cbKoncLeftNO_stateChanged(int arg1);

    void on_cbKoncRightNZ_stateChanged(int arg1);

    void on_cbKoncRightNO_stateChanged(int arg1);

    void on_cbKoncSpoilPerNZ_stateChanged(int arg1);

    void on_cbKoncSpoilPerNO_stateChanged(int arg1);

    void on_cbKoncSpoilZadNZ_stateChanged(int arg1);

    void on_cbKoncSpoilZadNO_stateChanged(int arg1);

    void on_cbKoncZTUNZ_stateChanged(int arg1);

    void on_cbKoncZTUNO_stateChanged(int arg1);

    void on_cbGerkonNZ_stateChanged(int arg1);

    void on_cbGerkonNO_stateChanged(int arg1);


    void on_cbZamokAPU1_stateChanged(int arg1);

    void on_cbZamokAPU2_stateChanged(int arg1);

    void on_cbZamokSpoilPered_stateChanged(int arg1);

    void on_cbZamokSpoilZad_stateChanged(int arg1);

    void on_cbImitaciya_stateChanged(int arg1);

    void on_sbImitUgol_valueChanged(int arg1);

    void on_sbImitAzim_valueChanged(int arg1);

signals:
    //сигнал изменения состояния выхода
    void newStaticOut(int num, bool state);

private:

    int systemState;

    //адрес компьютера видеообработки и компьютера абонента
    QString netPCVideoAdress, netPCAbonentAdress;
    //порт компьютера видеообработки и компьютера абонента
    int netPCVideoPort, netPCAbonentPort;
    //ИД компьютера видеообработки и компьютера абонента
    int netPCVideoID, netPCAbonentID;

    Ui::MainWindow *ui;

    //Объект для работы с сетью c программой видеообработки
    Network networkVideo;
    //Объект для работы с сетью cо сторонним абонентом
    ANetwork networkAbonent;
    //

    //Таймер для опроса состояния устройств
    QTimer timer;
    //Период опроса таймера состояния устройств
    int statusQPS;

    //Статусы устройств
    bool sEncUgol, sEncAzim, sDalnomer, sBmsd1, sBmsd2, sBmsd3, sBmsd4, sBmsd5, sBmsd6,
        sPotenciometr1, sPotenciometr2, sStaticDIO, sJoystick, sPult;
    //Признаки, надо ли обновлять статус определенного устройства
    bool bEncUgol, bEncAzim, bDalnomer, bBmsd1, bBmsd2, bBmsd3, bBmsd4, bBmsd5, bGsm40,
        bPotenciometr1, bPotenciometr2, bStaticDIO, bJoystick, bPult;

    //Объект для работы с энкодером угла\места
    PEncoder *encUgol;
    //Объект для работы с энкодером азимута
    PEncoder *encAzim;

    //Функция инициализации параметров энкодера угла\места значениями из конф.файла
    bool initEncUgol();
    //Функция инициализации параметров энкодера азимута значениями из конф.файла
    bool initEncAzim();

    //Объект для работы с дальномером
    PDalnomer *dalnomer;
    //Функция инициализации параметров дальномера значениями из конф.файла
    bool initDalnomer();

    //Объект для работы с портами, на которых подключены группы из 3 и 2 БМСД
    PBMSDPort *BMSD2;
    PBMSDPort *BMSD3;
    //Объект для работы с портом, на котором подключен точный актуатор
    PBMSDPort *GSM40;
    //Функция инициализации параметров порта группы из 2-х БМСД значениями из конф.файла
    bool initBMSD2();
    //Функция инициализации параметров порта группы из 3-х БМСД значениями из конф.файла
    bool initBMSD3();
    //Функция инициализации параметров порта точного актуатора значениями из конф.файла
    bool initGSM40();

    //Объекты для работы с БМСД
    PBMSD *bmsd1, *bmsd2, *bmsd3, *bmsd4, *bmsd5;
    //Объект для работы с точным актуатором GSM40
    PGSM40 *gsm40;
    //Функция инициализации адресов БМСД значениями из конф.файла
    bool initBmsd();

    //Объекты для работы с потенциометрами у линейных актуаторов
    PPotenciometr *potenciometr1;
    PPotenciometr *potenciometr2;
    //Функция инициализации потенциометров значениями из конф.файла
    bool initPotenciometr();

    //Объект для работы с платой дискретных в\в
    PStaticDIO *staticDIO;
    //Функция инициализации платы дискретных в\в значениями из конф.файла
    bool initStaticDIO();

    //Объект для работы с джойстиком
    PJoystick *joystick;
    //Функция инициализации джойстика
    bool initJoystick();

    //Объект для работы с пультом
    PPult *pult;
    //Функция инициализации пульта
    bool initPult();

    //Объект для расчета маркера джойстика и движения платформы
    PDrive *drive;
    //Функция инициализации объекта расчета движения
    bool initDrive();

    //Объект для проверки системы
    PTest *test;
    //Функция инициализации объекта проверки системы
    bool initTest();
    QTimer *timerTest;

    //Фукнция изменения состояния кнопки в интерфейсе
    void setBtnState(QPushButton *btn, bool state);
};

#endif // MAINWINDOW_H
