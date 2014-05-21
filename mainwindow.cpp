#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDialog>

//Конфигурационный файл
const char* FILE_SETTINGS = "./settings.ini";

QString ENCODER_PORT_UGOL = "COM6";
QString ENCODER_PORT_AZIM = "COM5";
QString DALNOMER_PORT = "COM4";

QString BMSD3_PORT = "COM9";
QString BMSD2_PORT = "COM10";
QString GSM40_PORT = "COM7";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    systemState = BLOCK;

    //Читаем настройки программы
    readSettings();

    networkVideo.Init(netPCVideoAdress, netPCVideoPort, 0, netPCVideoID);
//    networkAbonent.Init(netPCAbonentAdress, netPCAbonentPort, netPCAbonentID, 0);

    //Признаки обновления информации о статусах устройств
    bEncUgol = false;
    bEncAzim = false;
    bDalnomer = false;
    bBmsd1 = false;
    bBmsd2 = false;
    bBmsd3 = false;
    bBmsd4 = false;
    bBmsd5 = false;
    bPotenciometr1 = false;
    bPotenciometr2 = false;
    bStaticDIO = false;
    bJoystick = false;
    bPult = false;

    // Энкодеры ---------------------------------------------------------------
    //Инициализация энкодеров
    encUgol = new PEncoder();
    encAzim = new PEncoder();

    //Подключение обработчика сигнала от энкодеров о новых данных
    connect(encUgol, SIGNAL(newData()), this, SLOT(newDataFromEncUgol()));
    connect(encAzim, SIGNAL(newData()), this, SLOT(newDataFromEncAzim()));    

    if (initEncUgol())
    {
        bEncUgol = true;
        encUgol->start();
    }

    if (initEncAzim())
    {
        bEncAzim = true;
        encAzim->start();
    }

    // Дальномер --------------------------------------------------------------

    dalnomer = new PDalnomer();

    connect(dalnomer, SIGNAL(newData()), this, SLOT(newDataFromDalnomer()));

    if (initDalnomer())
    {
        bDalnomer = true;        
    }

    // БМСД  --------------------------------------------------------------

    BMSD2 = new PBMSDPort();
    BMSD3 = new PBMSDPort();
    GSM40 = new PBMSDPort();

    bmsd1 = new PBMSD(BMSD3);
    bmsd2 = new PBMSD(BMSD3);
    bmsd3 = new PBMSD(BMSD3);
    bmsd4 = new PBMSD(BMSD2);
    bmsd5 = new PBMSD(BMSD2);
    gsm40 = new PGSM40(GSM40);

    //Подключаем сигнал приема портом ответного сообщения на слот обработки ответа
    connect(BMSD3, SIGNAL(response(QString)), bmsd1, SLOT(parseData(QString)));
    connect(BMSD3, SIGNAL(response(QString)), bmsd2, SLOT(parseData(QString)));
    connect(BMSD3, SIGNAL(response(QString)), bmsd3, SLOT(parseData(QString)));
    connect(BMSD2, SIGNAL(response(QString)), bmsd4, SLOT(parseData(QString)));
    connect(BMSD2, SIGNAL(response(QString)), bmsd5, SLOT(parseData(QString)));
    connect(GSM40, SIGNAL(response(QString)), gsm40, SLOT(parseData(QString)));

    bool bBMSD2 = false, bBMSD3 = false, bGSM40 = false, bBMSD = false;

    //Инициализируем порты БМСД параметрами из конф. файла
    if (initBMSD3())
        bBMSD3 = true;

    if (initBMSD2())
        bBMSD2 = true;

    if (initGSM40())
        bGSM40 = true;

//    initBMSD2();
//    initBMSD3();
//    initBMSD4();

    //Инициализируем устройства БМСД параметрами из конф. файла
    if (initBmsd())
        bBMSD = true;

    if (bBMSD3 && bBMSD)
    {
        bBmsd1 = true;
        bBmsd2 = true;
        bBmsd3 = true;

        bmsd1->newStart();
        bmsd2->newStart();
        bmsd3->newStart();
    }

    //Инициализируем группы параметрами из конф. файла
    if (bBMSD2 && bBMSD)
    {
        bBmsd4 = true;
        bBmsd5 = true;
        bmsd4->newStart();
        bmsd5->newStart();
    }

    //Инициализируем группы параметрами из конф. файла
    if (bGSM40 && bBMSD)
    {
        bGsm40 = true;
    }

    // Потенциометры ----------------------------------------------------------
    potenciometr1 = new PPotenciometr();
    potenciometr2 = new PPotenciometr();

    connect(potenciometr1, SIGNAL(newData(int)), this, SLOT(newDataFromPotenciometr1(int)));
    connect(potenciometr2, SIGNAL(newData(int)), this, SLOT(newDataFromPotenciometr2(int)));

    if (initPotenciometr())
    {
        bPotenciometr1 = false;
        bPotenciometr2 = false;
        potenciometr1->start();
        potenciometr2->start();
    }

    // PCI-1758UDIO ----------------------------------------------------------
    staticDIO = new PStaticDIO();
    connect(staticDIO, SIGNAL(newIn(int,bool)), this, SLOT(updateKonc(int,bool)));

    initStaticDIO();

    // Джойстик  ----------------------------------------------------------
    joystick = new PJoystick();
    connect(joystick, SIGNAL(btnPress(bool)), this, SLOT(joystickPressBtn(bool)));
    connect(joystick, SIGNAL(newData(int,int)), this, SLOT(joystickNewData(int,int)));

    if (initJoystick())
    {
        bJoystick = true;
        joystick->start();
    }

    // Пульт ----------------------------------------------------------
    pult = new PPult();
    //подключаем сигнал установки индикаторов через плату к функции установки их в интерфейсе
    connect(pult, SIGNAL(newOut(int,bool)), this, SLOT(setPultUIInd(int,bool)));
    connect(pult, SIGNAL(newOut(int,bool)), staticDIO, SIGNAL(newOut(int,bool)));
    connect(pult, SIGNAL(setIndS(UINT64)), this, SLOT(setAllBtnState(UINT64)));
    connect(pult, SIGNAL(setIndS(UINT64)), staticDIO, SLOT(setIndS(UINT64)));
    //подключаем сигнал изменения положения джойстика к функции обработки этих данных в пульте    
    connect(pult, SIGNAL(sendCommand(int,int,int)), &networkVideo, SLOT(sendCommand(int,int,int)));
    connect(pult, SIGNAL(dalnomerStart()), dalnomer, SLOT(start()));
    connect(pult, SIGNAL(dalnomerStop()), dalnomer, SLOT(stop()));
    connect(staticDIO, SIGNAL(newIn(int,bool)), pult, SLOT(newState(int,bool)));

    // Расчет движения ----------------------------------------------------------
    drive = new PDrive();
    connect(joystick, SIGNAL(newData(int,int)), drive, SLOT(newJoyData(int,int)));
    connect(drive, SIGNAL(sendCommand(int,int,int)), &networkVideo, SLOT(sendCommand(int,int,int)));
    connect(potenciometr1, SIGNAL(newData(int)), drive, SLOT(setFocus(int)));
    connect(potenciometr2, SIGNAL(newData(int)), drive, SLOT(setZTU(int)));
    connect(drive, SIGNAL(updateBmsd1UI()), this, SLOT(updateBmsd1UI()));
    connect(drive, SIGNAL(updateBmsd2UI()), this, SLOT(updateBmsd2UI()));
    connect(drive, SIGNAL(updateBmsd3UI()), this, SLOT(updateBmsd3UI()));
    connect(drive, SIGNAL(updateBmsd4UI()), this, SLOT(updateBmsd4UI()));
    connect(drive, SIGNAL(updateBmsd5UI()), this, SLOT(updateBmsd5UI()));
    connect(drive, SIGNAL(updateGsm40UI()), this, SLOT(updateGsm40UI()));    
//    connect(drive, SIGNAL(newPolog(bool)), this, SLOT(updatePultPolog(bool)));
    connect(drive, SIGNAL(newPolog(bool)), pult, SLOT(newPolog(bool)));
    connect(drive, SIGNAL(newStaticDIOOut(int,bool)), this, SLOT(updateKonc(int,bool)));
    connect(staticDIO, SIGNAL(newIn(int,bool)), drive, SLOT(newStaticDIOState(int,bool)));
    connect(drive, SIGNAL(updateKonc(int,bool)), this, SLOT(updateKonc(int,bool)));
    connect(drive, SIGNAL(newDataUgol(int,int)), this, SLOT(newDataUgol(int,int)));
    connect(drive, SIGNAL(newDataAzim(int,int)), this, SLOT(newDataAzim(int,int)));
    connect(drive, SIGNAL(writeEncNPos()), this, SLOT(writeEncNullPos()));
    connect(drive, SIGNAL(updateStatusUI(bool,bool,bool,bool)), pult, SLOT(updateStatus(bool,bool,bool,bool)));
    connect(&networkVideo, SIGNAL(posTargetAC(int,int)), drive, SLOT(posTargetAC(int,int)));

    // Тестирование системы ----------------------------------------------------------
    test = new PTest();
    connect(test, SIGNAL(sendCommand(int,int,int)), &networkVideo, SLOT(sendCommand(int,int,int)));
    connect(test, SIGNAL(updateStatus(bool,bool,bool,bool)), pult, SLOT(updateStatus(bool,bool,bool,bool)));
    connect(&networkVideo, SIGNAL(statusVideo(bool,bool,bool,bool,bool,bool)), test, SLOT());

    ui->setupUi(this);

    initPult();
    initDrive();
    initTest();

    getStatus();
    //Подключаем таймер опроса состояний устройств
    connect(&timer, SIGNAL(timeout()), SLOT(getStatus()));
    timer.setInterval(statusQPS);
    //Включаем таймер опроса устройств
    timer.start();

    updateBmsd1UI();
    updateBmsd2UI();
    updateBmsd3UI();
    updateBmsd4UI();
    updateBmsd5UI();
    updateGsm40UI();

    //проверяем состояние флага имитации
    if (ui->cbImitaciya->checkState() == Qt::Checked)
    {
        //устанавливаем признак имитации
        drive->imitaciya = true;
        //обновляем значения в интерфейсе
        emit ui->sbImitUgol->valueChanged(ui->sbImitUgol->value());
        emit ui->sbImitAzim->valueChanged(ui->sbImitAzim->value());
//        drive->posYgrad = ui->sbImitUgol->value();
//        drive->posXgrad = ui->sbImitAzim->value();
    }
    else
        drive->imitaciya = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::readSettings()
{
    //Открываем конфигурационный файл
    QSettings *settings = new QSettings(FILE_SETTINGS,QSettings::IniFormat);

    //Читаем настройки
    statusQPS = settings->value("SETTINGS/STATUSQPS", DEFAULT_STATUSQPS).toInt();

    netPCVideoAdress = settings->value("PC_VIDEO/Adress", DEFAULT_VIDEO_ADRESS).toString();
    netPCAbonentAdress = settings->value("PC_ABONENT/Adress", DEFAULT_ABONENT_ADRESS).toString();

    netPCVideoPort = settings->value("PC_VIDEO/Port", DEFAULT_VIDEO_ADRESS).toInt();
    netPCAbonentPort = settings->value("PC_ABONENT/Port", DEFAULT_ABONENT_ADRESS).toInt();

    netPCVideoID = settings->value("PC_VIDEO/ID", DEFAULT_VIDEO_ADRESS).toInt();
    netPCAbonentID = settings->value("PC_ABONENT/ID", DEFAULT_ABONENT_ADRESS).toInt();
    return true;
}

void MainWindow::StringToByteArray(QString &src, QByteArray &dst)
{
    for (int i = 0; i < src.size(); i++)
        dst[i] = src[i].cell();
}

//Функция запроса состояний устройств по таймеру
void MainWindow::getStatus()
{
    if (bEncUgol)
        sEncUgol = encUgol->getStatus();
    if (bEncAzim)
        sEncAzim = encAzim->getStatus();
    if (bDalnomer)
        sDalnomer = dalnomer->getStatus();
    if (bBmsd1)
    {
        sBmsd1 = bmsd1->getStatus();
    }
    if (bBmsd2)
    {
        sBmsd2 = bmsd2->getStatus();
    }
    if (bBmsd3)
    {
        sBmsd3 = bmsd3->getStatus();
    }
    if (bBmsd4)
    {
        sBmsd4 = bmsd4->getStatus();
    }
    if (bBmsd5)
    {
        sBmsd5 = bmsd5->getStatus();
    }

//    if (bBmsd6)
//    {
//        sBmsd6 = bmsd6->getStatus();
//    }
    if (bPotenciometr1)
        sPotenciometr1 = potenciometr1->getStatus();
    if (bPotenciometr2)
        sPotenciometr2 = potenciometr2->getStatus();
    if (bStaticDIO)
        sStaticDIO = staticDIO->getStatus();
    if (bJoystick)
        sJoystick = joystick->getStatus();
    if (bPult)
        sPult = pult->getStatus();

    ui->lblStatusEncUgol->setText(sEncUgol ? STATUS_SUCCESS : STATUS_ERROR);
    ui->lblStatusEncAzim->setText(sEncAzim ? STATUS_SUCCESS : STATUS_ERROR);
    ui->lblStatusDalnomer->setText(sDalnomer ? STATUS_SUCCESS : STATUS_ERROR);
    ui->lblStatusBmsd1->setText(sBmsd1 ? STATUS_SUCCESS : STATUS_ERROR);
    ui->lblStatusBmsd2->setText(sBmsd2 ? STATUS_SUCCESS : STATUS_ERROR);
    ui->lblStatusBmsd3->setText(sBmsd3 ? STATUS_SUCCESS : STATUS_ERROR);
    ui->lblStatusBmsd4->setText(sBmsd4 ? STATUS_SUCCESS : STATUS_ERROR);
    ui->lblStatusBmsd5->setText(sBmsd5 ? STATUS_SUCCESS : STATUS_ERROR);
    ui->lblStatusBmsd6->setText(sBmsd6 ? STATUS_SUCCESS : STATUS_ERROR);
    ui->lblStatusPotenciometr1->setText(sPotenciometr1 ? STATUS_SUCCESS : STATUS_ERROR);
    ui->lblStatusPotenciometr2->setText(sPotenciometr2 ? STATUS_SUCCESS : STATUS_ERROR);
    ui->lblStatusPult->setText(sPult ? STATUS_SUCCESS : STATUS_ERROR);
    ui->lblStatusJoystick->setText(sJoystick ? STATUS_SUCCESS : STATUS_ERROR);
}

void MainWindow::newDataFromEncUgol()
{
    //если имитация, то не получаем данные от энкодера
    if (ui->cbImitaciya->isChecked())
        return;

    if (encUgol->getStatus())
    {
        int grad = encUgol->getGradUgol();
        int min = encUgol->getMinUgol();
        ui->lblStatusEncUgol->setText(STATUS_SUCCESS);
        ui->lblEncUgolSteps256->setText(QString("%1").arg(grad));
        ui->lblEncUgolSteps->setText(QString("%1").arg(min));
        networkVideo.send(CP_COMMAND_ENCODER_UGOL, grad, min);
        drive->posVizXgrad = grad;
        drive->posVizXmin = min;
    }
    else
    {
        ui->lblStatusEncUgol->setText(STATUS_ERROR);
        ui->lblEncUgolSteps256->setText(STATUS_ERROR);
        ui->lblEncUgolSteps->setText(STATUS_ERROR);
    }
}

void MainWindow::newDataFromEncAzim()
{
    //если имитация, то не получаем данные от энкодера
    if (ui->cbImitaciya->isChecked())
        return;

    if (encAzim->getStatus())
    {
        int grad = encAzim->getGradAzim();
        int min = encAzim->getMinAzim();
        ui->lblStatusEncAzim->setText(STATUS_SUCCESS);
        ui->lblEncAzimSteps256->setText(QString("%1").arg(grad));
        ui->lblEncAzimSteps->setText(QString("%1").arg(min));
        networkVideo.send(CP_COMMAND_ENCODER_AZIM, grad, min);
        drive->posVizYgrad = grad;
        drive->posVizYmin = min;
    }
    else
    {
        ui->lblStatusEncAzim->setText(STATUS_ERROR);
        ui->lblEncAzimSteps256->setText(STATUS_ERROR);
        ui->lblEncAzimSteps->setText(STATUS_ERROR);
    }
}

//Функция обработки сигнала для изменения данных угла\места
void MainWindow::newDataUgol(int grad, int min)
{
    ui->lblEncUgolSteps256->setText(QString("%1").arg(grad));
    ui->lblEncUgolSteps->setText(QString("%1").arg(min));
    networkVideo.send(CP_COMMAND_ENCODER_UGOL, grad, min);
}

//Функция обработки сигнала для изменения данных азимута
void MainWindow::newDataAzim(int grad, int min)
{
    ui->lblEncAzimSteps256->setText(QString("%1").arg(grad));
    ui->lblEncAzimSteps->setText(QString("%1").arg(min));
    networkVideo.send(CP_COMMAND_ENCODER_AZIM, grad, min);
}

//Функция записи нулевых позиций энкодеров в конф.файл
void MainWindow::writeEncNullPos()
{
    encAzim->writeEncNullPos("ENCODER_AZIM");
    encUgol->writeEncNullPos("ENCODER_UGOL");
}

//Функция обработки сигнала от дальномера о поступлении новых данных
void MainWindow::newDataFromDalnomer()
{
    if (dalnomer->getStatus())
    {
        int dal = dalnomer->getDalnost();
        ui->lblStatusDalnomer->setText(STATUS_SUCCESS);
        ui->lblDalnomerDalnost->setText(QString("%1").arg(dal));
        networkVideo.send(CP_COMMAND_DALNOMER, (dal >> 8) & 0xFF, dal & 0xFF);
    }
    else
    {
        ui->lblStatusDalnomer->setText(STATUS_ERROR);
        ui->lblDalnomerDalnost->setText(STATUS_ERROR);
    }
}

//Функция обработки сигнала от потенциометра о поступлении новых данных
void MainWindow::newDataFromPotenciometr1(int arg1)
{
    if (potenciometr1->getStatus())
    {
        ui->lblStatusPotenciometr1->setText(STATUS_SUCCESS);
        ui->lblPosPotenciometr1->setText(QString("%1").arg(arg1));
    }
    else
    {
        ui->lblStatusPotenciometr1->setText(STATUS_ERROR);
    }
}

//Функция обработки сигнала от потенциометра о поступлении новых данных
void MainWindow::newDataFromPotenciometr2(int arg1)
{
    if (potenciometr2->getStatus())
    {
        ui->lblStatusPotenciometr2->setText(STATUS_SUCCESS);
        ui->lblPosPotenciometr2->setText(QString("%1").arg(arg1));
    }
    else
    {
        ui->lblStatusPotenciometr2->setText(STATUS_ERROR);
    }
}

//Функция обработки нажатия кнопки джойстика
void MainWindow::joystickPressBtn(bool state)
{
    if (state)
        ui->lblJoystikBtn->setText("Нажата");
    else
        ui->lblJoystikBtn->setText("Отжата");
}

//Функция обработки изменения положения осей джойстика
void MainWindow::joystickNewData(int x, int y)
{
    ui->lblJoystikX->setText(QString("%1").arg(x));
    ui->lblJoystikY->setText(QString("%1").arg(y));
}

void MainWindow::updateBmsd1UI()
{
    ui->sbTormozBmsd_1->setValue(bmsd1->getTormogenie());
    ui->sbUskBmsd_1->setValue(bmsd1->getUskorenie());
    ui->sbSpeedBmsd_1->setValue(bmsd1->getSpeed());
    ui->lblDirectionBmsd_1->setText(bmsd1->direction == BMSD_DIRECTION_LEFT ? "<--" : "-->");
}

void MainWindow::updateBmsd2UI()
{
    ui->sbTormozBmsd_2->setValue(bmsd2->getTormogenie());
    ui->sbUskBmsd_2->setValue(bmsd2->getUskorenie());
    ui->sbSpeedBmsd_2->setValue(bmsd2->getSpeed());
    ui->lblDirectionBmsd_2->setText(bmsd2->direction == BMSD_DIRECTION_LEFT ? "<--" : "-->");
}

void MainWindow::updateBmsd3UI()
{
    ui->sbTormozBmsd_3->setValue(bmsd3->getTormogenie());
    ui->sbUskBmsd_3->setValue(bmsd3->getUskorenie());
    ui->sbSpeedBmsd_3->setValue(bmsd3->getSpeed());
    ui->lblDirectionBmsd_3->setText(bmsd3->direction == BMSD_DIRECTION_LEFT ? "<--" : "-->");
}

void MainWindow::updateBmsd4UI()
{
    ui->sbTormozBmsd_4->setValue(bmsd4->getTormogenie());
    ui->sbUskBmsd_4->setValue(bmsd4->getUskorenie());
    ui->sbSpeedBmsd_4->setValue(bmsd4->getSpeed());
    bmsd4->newDirection(bmsd4->direction);
    ui->lblDirectionBmsd_4->setText(bmsd4->direction == BMSD_DIRECTION_LEFT ? "<--" : "-->");
}

void MainWindow::updateBmsd5UI()
{
    ui->sbTormozBmsd_5->setValue(bmsd5->getTormogenie());
    ui->sbUskBmsd_5->setValue(bmsd5->getUskorenie());
    ui->sbSpeedBmsd_5->setValue(bmsd5->getSpeed());
    ui->lblDirectionBmsd_5->setText(bmsd5->direction == BMSD_DIRECTION_LEFT ? "<--" : "-->");
}

void MainWindow::updateGsm40UI()
{
    if (!ui->cbControlGsm40->isChecked())
    {
        ui->sbTormozBmsd_6->setValue(gsm40->getTormogenie());
        ui->sbUskBmsd_6->setValue(gsm40->getUskorenie());
        ui->sbSpeedBmsd_6->setValue(gsm40->getSpeed());
        ui->lblDirectionBmsd_6->setText(gsm40->direction == GSM40_DIRECTION_UP ? "<--" : "-->");
    }
}

//функция обновления интерфейса концевых выключателей
void MainWindow::updateKonc(int num, bool state)
{
    switch (num) {
    case IN_GERKON_NO:
        ui->cbGerkonNO->setChecked(state);
        break;
    case IN_GERKON_NZ:
        ui->cbGerkonNZ->setChecked(state);
        break;
    case IN_KONC_LEFT_NO:
        ui->cbKoncLeftNO->setChecked(state);
        break;
    case IN_KONC_LEFT_NZ:
        ui->cbKoncLeftNZ->setChecked(state);
        break;
    case IN_KONC_RIGHT_NO:
        ui->cbKoncRightNO->setChecked(state);
        break;
    case IN_KONC_RIGHT_NZ:
        ui->cbKoncRightNZ->setChecked(state);
        break;
    case IN_KONC_SPOIL_PER_NO:
        ui->cbKoncSpoilPerNO->setChecked(state);
        break;
    case IN_KONC_SPOIL_PER_NZ:
        ui->cbKoncSpoilPerNZ->setChecked(state);
        break;
    case IN_KONC_SPOIL_ZAD_NO:
        ui->cbKoncSpoilZadNO->setChecked(state);
        break;
    case IN_KONC_SPOIL_ZAD_NZ:
        ui->cbKoncSpoilZadNZ->setChecked(state);
        break;
    case IN_KONC_ZTU_NO:
        ui->cbKoncZTUNO->setChecked(state);
        break;
    case IN_KONC_ZTU_NZ:
        ui->cbKoncZTUNZ->setChecked(state);
        break;
    case OUT_ZAMOK_APU1:
        ui->cbZamokAPU1->setChecked(drive->zamokAPU1);
        break;
    case OUT_ZAMOK_APU2:
        ui->cbZamokAPU2->setChecked(drive->zamokAPU2);
        break;
    case OUT_ZAMOK_SPOIL_PERED:
        ui->cbZamokSpoilPered->setChecked(drive->zamokSpoilPered);
        break;
    case OUT_ZAMOK_SPOIL_ZAD:
        ui->cbZamokSpoilZad->setChecked(drive->zamokSpoilZad);
        break;
    default:
        break;
    }
}

//Функция обновления интерфейса
void MainWindow::updateStatusUI(bool apu, bool lvs, bool spoil, bool oes)
{
    setBtnState(ui->btnPultStatusAPU, apu);
    setBtnState(ui->btnPultStatusAPU, lvs);
    setBtnState(ui->btnPultStatusAPU, spoil);
    setBtnState(ui->btnPultStatusAPU, oes);
}

//Фукнция изменения индикаторов положения антенны в интерфейсе
//void MainWindow::updatePultPolog(bool state)
//{
//    if (state == POHOD)
//    {
//        setBtnState(ui->btnPultPologPohod, true);
//        setBtnState(ui->btnPultPologWork, false);
//    }
//    else
//    {
//        setBtnState(ui->btnPultPologPohod, false);
//        setBtnState(ui->btnPultPologWork, true);
//    }
//}

//Функция обработки индикации в интерфейсе
void MainWindow::setPultUIInd(int num, bool state)
{
    switch (num) {
    case PULT_IND_STATUS_APU:
        setBtnState(ui->btnPultStatusAPU, state);
        break;
    case PULT_IND_STATUS_LVS:
        setBtnState(ui->btnPultStatusLVS, state);
        break;
    case PULT_IND_STATUS_OES:
        setBtnState(ui->btnPultStatusOEC, state);
        break;
    case PULT_IND_STATUS_SPOIL:
        setBtnState(ui->btnPultStatusSPOIL, state);
        break;
    case PULT_IND_COMMON_AC:
        setBtnState(ui->btnPultCommonAC, state);
        break;
    case PULT_IND_COMMON_CONTROL:
        setBtnState(ui->btnPultCommonControl, state);
        break;
    case PULT_IND_COMMON_GK:
        setBtnState(ui->btnPultCommonGK, state);
        break;
    case PULT_IND_COMMON_VISOKOE:
        setBtnState(ui->btnPultCommonVisokoe1, state);
        break;
    case PULT_IND_FOCUS_35:
        setBtnState(ui->btnPultFocus35, state);
        break;
    case PULT_IND_FOCUS_50:
        setBtnState(ui->btnPultFocus50, state);
        break;
    case PULT_IND_FOCUS_75:
        setBtnState(ui->btnPultFocus75, state);
        break;
    case PULT_IND_FOCUS_100:
        setBtnState(ui->btnPultFocus100, state);
        break;
    case PULT_IND_FOCUS_AUTO:
        setBtnState(ui->btnPultFocusAuto, state);
        break;
    case PULT_IND_NAVED_MANUAL:
        setBtnState(ui->btnPultNavedManual, state);
        break;
    case PULT_IND_NAVED_NAVESTI:
        setBtnState(ui->btnPultNavedNavesti, state);
        break;
    case PULT_IND_NAVED_NIGHT:
        setBtnState(ui->btnPultNavedNight, state);
        break;
    case PULT_IND_NAVED_PA:
        setBtnState(ui->btnPultNavedPA, state);
        break;
    case PULT_IND_NAVED_ZOOM_IN:
        setBtnState(ui->btnPultNavedZoomIn, state);
        break;
    case PULT_IND_NAVED_ZOOM_OUT:
        setBtnState(ui->btnPultNavedZoomOut, state);
        break;
    case PULT_IND_OBZOR_1:
        setBtnState(ui->btnPultObzor1, state);
        break;
    case PULT_IND_OBZOR_2:
        setBtnState(ui->btnPultObzor2, state);
        break;
    case PULT_IND_OBZOR_3:
        setBtnState(ui->btnPultObzor3, state);
        break;
    case PULT_IND_OBZOR_4:
        setBtnState(ui->btnPultObzor4, state);
        break;
    case PULT_IND_OBZOR_A:
        setBtnState(ui->btnPultObzorA, state);
        break;
    case PULT_IND_OBZOR_C:
        setBtnState(ui->btnPultObzorC, state);
        break;
    case PULT_IND_OBZOR_ZOOM_IN:
        setBtnState(ui->btnPultObzorZoomIn, state);
        break;
    case PULT_IND_OBZOR_ZOOM_OUT:
        setBtnState(ui->btnPultObzorZoomOut, state);
        break;
    case PULT_IND_POLOG_POHOD:
        setBtnState(ui->btnPultPologPohod, state);
        break;
    case PULT_IND_POLOG_WORK:
        setBtnState(ui->btnPultPologWork, state);
        break;
    case PULT_IND_SCREEN_KAMERA:
        setBtnState(ui->btnPultScreenCamera, state);
        break;
    case PULT_IND_SCREEN_MULTI:
        setBtnState(ui->btnPultScreenMulti, state);
        break;
    case PULT_IND_SCREEN_NAVED:
        setBtnState(ui->btnPultScreenNaved, state);
        break;
    case PULT_IND_SCREEN_OBZOR:
        setBtnState(ui->btnPultScreenObzor, state);
        break;
    case PULT_IND_UNKNOWN_HP1:
        setBtnState(ui->btnPultUnknownHP1, state);
        break;
    case PULT_IND_UNKNOWN_HP2:
        setBtnState(ui->btnPultUnknownHP2, state);
        break;
    case PULT_IND_UNKNOWN_HP3:
        setBtnState(ui->btnPultUnknownHP3, state);
        break;
    case PULT_IND_UNKNOWN_HP4:
        setBtnState(ui->btnPultUnknownHP4, state);
        break;
    case PULT_IND_UNKNOWN_HP5:
        setBtnState(ui->btnPultUnknownHP5, state);
        break;
    case PULT_IND_UNKNOWN_HP6:
        setBtnState(ui->btnPultUnknownHP6, state);
        break;
    case PULT_IND_UNKNOWN_HP7:
        setBtnState(ui->btnPultUnknownHP7, state);
        break;
    case PULT_IND_UNKNOWN_HP8:
        setBtnState(ui->btnPultUnknownHP8, state);
        break;
    case PULT_IND_UNKNOWN_HP9:
        setBtnState(ui->btnPultUnknownHP9, state);
        break;
    case PULT_IND_UNKNOWN_IP1:
        setBtnState(ui->btnPultUnknownIP1, state);
        break;
    case PULT_IND_UNKNOWN_IP2:
        setBtnState(ui->btnPultUnknownIP2, state);
        break;
    case PULT_IND_UNKNOWN_IP3:
        setBtnState(ui->btnPultUnknownIP3, state);
        break;
    default:
        break;
    }
}

//Функция зажигания всех индикаторов в интерфейсе
void MainWindow::setAllBtnState(UINT64 state)
{
    UINT64 prevState = state;
    //преобразуем номер входа в номер бита и проверяем его
    if ((prevState >> fromIntToDIO(PULT_IND_COMMON_AC)) & 1)
        setBtnState(ui->btnPultCommonAC, true);
    else
        setBtnState(ui->btnPultCommonAC, false);

    if ((prevState >> fromIntToDIO(PULT_IND_COMMON_CONTROL)) & 1)
        setBtnState(ui->btnPultCommonControl, true);
    else
        setBtnState(ui->btnPultCommonControl, false);

    if ((prevState >> fromIntToDIO(PULT_IND_COMMON_VISOKOE)) & 1)
        setBtnState(ui->btnPultCommonVisokoe1, true);
    else
        setBtnState(ui->btnPultCommonVisokoe1, false);

    if ((prevState >> fromIntToDIO(PULT_IND_COMMON_GK)) & 1)
        setBtnState(ui->btnPultCommonGK, true);
    else
        setBtnState(ui->btnPultCommonGK, false);

    if ((prevState >> fromIntToDIO(PULT_IND_FOCUS_35)) & 1)
        setBtnState(ui->btnPultFocus35, true);
    else
        setBtnState(ui->btnPultFocus35, false);

    if ((prevState >> fromIntToDIO(PULT_IND_FOCUS_50)) & 1)
        setBtnState(ui->btnPultFocus50, true);
    else
        setBtnState(ui->btnPultFocus50, false);

    if ((prevState >> fromIntToDIO(PULT_IND_FOCUS_75)) & 1)
        setBtnState(ui->btnPultFocus75, true);
    else
        setBtnState(ui->btnPultFocus75, false);

    if ((prevState >> fromIntToDIO(PULT_IND_FOCUS_100)) & 1)
        setBtnState(ui->btnPultFocus100, true);
    else
        setBtnState(ui->btnPultFocus100, false);

    if ((prevState >> fromIntToDIO(PULT_IND_FOCUS_AUTO)) & 1)
        setBtnState(ui->btnPultFocusAuto, true);
    else
        setBtnState(ui->btnPultFocusAuto, false);

    if ((prevState >> fromIntToDIO(PULT_IND_NAVED_MANUAL)) & 1)
        setBtnState(ui->btnPultNavedManual, true);
    else
        setBtnState(ui->btnPultNavedManual, false);

    if ((prevState >> fromIntToDIO(PULT_IND_NAVED_NAVESTI)) & 1)
        setBtnState(ui->btnPultNavedNavesti, true);
    else
        setBtnState(ui->btnPultNavedNavesti, false);

    if ((prevState >> fromIntToDIO(PULT_IND_NAVED_NIGHT)) & 1)
        setBtnState(ui->btnPultNavedNight, true);
    else
        setBtnState(ui->btnPultNavedNight, false);

    if ((prevState >> fromIntToDIO(PULT_IND_NAVED_PA)) & 1)
        setBtnState(ui->btnPultNavedPA, true);
    else
        setBtnState(ui->btnPultNavedPA, false);

    if ((prevState >> fromIntToDIO(PULT_IND_NAVED_ZOOM_IN)) & 1)
        setBtnState(ui->btnPultNavedZoomIn, true);
    else
        setBtnState(ui->btnPultNavedZoomIn, false);

    if ((prevState >> fromIntToDIO(PULT_IND_NAVED_ZOOM_OUT)) & 1)
        setBtnState(ui->btnPultNavedZoomOut, true);
    else
        setBtnState(ui->btnPultNavedZoomOut, false);

    if ((prevState >> fromIntToDIO(PULT_IND_OBZOR_1)) & 1)
        setBtnState(ui->btnPultObzor1, true);
    else
        setBtnState(ui->btnPultObzor1, false);

    if ((prevState >> fromIntToDIO(PULT_IND_OBZOR_2)) & 1)
        setBtnState(ui->btnPultObzor2, true);
    else
        setBtnState(ui->btnPultObzor2, false);

    if ((prevState >> fromIntToDIO(PULT_IND_OBZOR_3)) & 1)
        setBtnState(ui->btnPultObzor3, true);
    else
        setBtnState(ui->btnPultObzor3, false);

    if ((prevState >> fromIntToDIO(PULT_IND_OBZOR_4)) & 1)
        setBtnState(ui->btnPultObzor4, true);
    else
        setBtnState(ui->btnPultObzor4, false);

    if ((prevState >> fromIntToDIO(PULT_IND_OBZOR_A)) & 1)
        setBtnState(ui->btnPultObzorA, true);
    else
        setBtnState(ui->btnPultObzorA, false);

    if ((prevState >> fromIntToDIO(PULT_IND_OBZOR_C)) & 1)
        setBtnState(ui->btnPultObzorC, true);
    else
        setBtnState(ui->btnPultObzorC, false);

    if ((prevState >> fromIntToDIO(PULT_IND_OBZOR_ZOOM_IN)) & 1)
        setBtnState(ui->btnPultObzorZoomIn, true);
    else
        setBtnState(ui->btnPultObzorZoomIn, false);

    if ((prevState >> fromIntToDIO(PULT_IND_OBZOR_ZOOM_OUT)) & 1)
        setBtnState(ui->btnPultObzorZoomOut, true);
    else
        setBtnState(ui->btnPultObzorZoomOut, false);

    if ((prevState >> fromIntToDIO(PULT_IND_POLOG_POHOD)) & 1)
        setBtnState(ui->btnPultPologPohod, true);
    else
        setBtnState(ui->btnPultPologPohod, false);

    if ((prevState >> fromIntToDIO(PULT_IND_POLOG_WORK)) & 1)
        setBtnState(ui->btnPultPologWork, true);
    else
        setBtnState(ui->btnPultPologWork, false);

    if ((prevState >> fromIntToDIO(PULT_IND_SCREEN_KAMERA)) & 1)
        setBtnState(ui->btnPultScreenCamera, true);
    else
        setBtnState(ui->btnPultScreenCamera, false);

    if ((prevState >> fromIntToDIO(PULT_IND_SCREEN_MULTI)) & 1)
        setBtnState(ui->btnPultScreenMulti, true);
    else
        setBtnState(ui->btnPultScreenMulti, false);

    if ((prevState >> fromIntToDIO(PULT_IND_SCREEN_NAVED)) & 1)
        setBtnState(ui->btnPultScreenNaved, true);
    else
        setBtnState(ui->btnPultScreenNaved, false);


    if ((prevState >> fromIntToDIO(PULT_IND_SCREEN_OBZOR)) & 1)
        setBtnState(ui->btnPultScreenObzor, true);
    else
        setBtnState(ui->btnPultScreenObzor, false);

    if ((prevState >> fromIntToDIO(PULT_IND_STATUS_APU)) & 1)
        setBtnState(ui->btnPultStatusAPU, true);
    else
        setBtnState(ui->btnPultStatusAPU, false);

    if ((prevState >> fromIntToDIO(PULT_IND_STATUS_LVS)) & 1)
        setBtnState(ui->btnPultStatusLVS, true);
    else
        setBtnState(ui->btnPultStatusLVS, false);

    if ((prevState >> fromIntToDIO(PULT_IND_STATUS_OES)) & 1)
        setBtnState(ui->btnPultStatusOEC, true);
    else
        setBtnState(ui->btnPultStatusOEC, false);

    if ((prevState >> fromIntToDIO(PULT_IND_STATUS_SPOIL)) & 1)
        setBtnState(ui->btnPultStatusSPOIL, true);
    else
        setBtnState(ui->btnPultStatusSPOIL, false);

    if ((prevState >> fromIntToDIO(PULT_IND_UNKNOWN_HP1)) & 1)
        setBtnState(ui->btnPultUnknownHP1, true);
    else
        setBtnState(ui->btnPultUnknownHP1, false);

    if ((prevState >> fromIntToDIO(PULT_IND_UNKNOWN_HP2)) & 1)
        setBtnState(ui->btnPultUnknownHP2, true);
    else
        setBtnState(ui->btnPultUnknownHP2, false);

    if ((prevState >> fromIntToDIO(PULT_IND_UNKNOWN_HP3)) & 1)
        setBtnState(ui->btnPultUnknownHP3, true);
    else
        setBtnState(ui->btnPultUnknownHP3, false);

    if ((prevState >> fromIntToDIO(PULT_IND_UNKNOWN_HP4)) & 1)
        setBtnState(ui->btnPultUnknownHP4, true);
    else
        setBtnState(ui->btnPultUnknownHP4, false);

    if ((prevState >> fromIntToDIO(PULT_IND_UNKNOWN_HP5)) & 1)
        setBtnState(ui->btnPultUnknownHP5, true);
    else
        setBtnState(ui->btnPultUnknownHP5, false);

    if ((prevState >> fromIntToDIO(PULT_IND_UNKNOWN_HP6)) & 1)
        setBtnState(ui->btnPultUnknownHP6, true);
    else
        setBtnState(ui->btnPultUnknownHP6, false);

    if ((prevState >> fromIntToDIO(PULT_IND_UNKNOWN_HP7)) & 1)
        setBtnState(ui->btnPultUnknownHP7, true);
    else
        setBtnState(ui->btnPultUnknownHP7, false);

    if ((prevState >> fromIntToDIO(PULT_IND_UNKNOWN_HP8)) & 1)
        setBtnState(ui->btnPultUnknownHP8, true);
    else
        setBtnState(ui->btnPultUnknownHP8, false);

    if ((prevState >> fromIntToDIO(PULT_IND_UNKNOWN_HP9)) & 1)
        setBtnState(ui->btnPultUnknownHP9, true);
    else
        setBtnState(ui->btnPultUnknownHP9, false);

    if ((prevState >> fromIntToDIO(PULT_IND_UNKNOWN_IP1)) & 1)
        setBtnState(ui->btnPultUnknownIP1, true);
    else
        setBtnState(ui->btnPultUnknownIP1, false);

    if ((prevState >> fromIntToDIO(PULT_IND_UNKNOWN_IP2)) & 1)
        setBtnState(ui->btnPultUnknownIP2, true);
    else
        setBtnState(ui->btnPultUnknownIP2, false);

    if ((prevState >> fromIntToDIO(PULT_IND_UNKNOWN_IP3)) & 1)
        setBtnState(ui->btnPultUnknownIP3, true);
    else
        setBtnState(ui->btnPultUnknownIP3, false);
}

//Функция инициализации параметров энкодера угла\места значениями из конф.файла
bool MainWindow::initEncUgol()
{
    bool result = false;
    //Открываем конфигурационный файл
    QSettings *settings = new QSettings(FILE_SETTINGS,QSettings::IniFormat);

    //Читаем настройки
    QString port = settings->value("ENCODER_UGOL/Port", ENCODER_PORT_UGOL).toString();
    int baudRate = settings->value("ENCODER_UGOL/BaudRate", ENCODER_BAUD_RATE).toInt();
    int dataBits = settings->value("ENCODER_UGOL/DataBits", ENCODER_DATA_BITS).toInt();
    int parity   = settings->value("ENCODER_UGOL/Parity", ENCODER_PARITY).toInt();
    int stopBits = settings->value("ENCODER_UGOL/StopBits", ENCODER_STOP_BITS).toInt();
    int flowControl = settings->value("ENCODER_UGOL/FlowControl", ENCODER_FLOW_CONTROL).toInt();
    int qps = settings->value("ENCODER_UGOL/QPS", ENCODER_QPS).toInt();

    if (settings->status() == QSettings::NoError)
    {
        encUgol->setPortSettings(port, baudRate, dataBits, parity, stopBits, flowControl);
        encUgol->setQPS(qps);
        result = true;
    }
    else
    {
        encUgol->setPortSettings(ENCODER_PORT_UGOL, ENCODER_BAUD_RATE,
                                ENCODER_DATA_BITS, ENCODER_PARITY,
                                ENCODER_STOP_BITS, ENCODER_FLOW_CONTROL);
        encUgol->setQPS(ENCODER_QPS);
        result = false;
    }

    //читаем калибровочные настройки начального положения энкодера
    encUgol->readEncNullPos("ENCODER_UGOL");

    return result;
}

//Функция инициализации параметров энкодера азимута значениями из конф.файла
bool MainWindow::initEncAzim()
{
    bool result = false;

    //Открываем конфигурационный файл
    QSettings *settings = new QSettings(FILE_SETTINGS,QSettings::IniFormat);

    //Читаем настройки
    QString port = settings->value("ENCODER_AZIM/Port", ENCODER_PORT_AZIM).toString();
    int baudRate = settings->value("ENCODER_AZIM/BaudRate", ENCODER_BAUD_RATE).toInt();
    int dataBits = settings->value("ENCODER_AZIM/DataBits", ENCODER_DATA_BITS).toInt();
    int parity   = settings->value("ENCODER_AZIM/Parity", ENCODER_PARITY).toInt();
    int stopBits = settings->value("ENCODER_AZIMUT/StopBits", ENCODER_STOP_BITS).toInt();
    int flowControl = settings->value("ENCODER_AZIMUT/FlowControl", ENCODER_FLOW_CONTROL).toInt();
    int qps = settings->value("ENCODER_AZIM/QPS", ENCODER_QPS).toInt();

    if (settings->status() == QSettings::NoError)
    {
        encAzim->setPortSettings(port, baudRate, dataBits, parity, stopBits, flowControl);
        encAzim->setQPS(qps);
        return true;
    }
    else
    {
        encAzim->setPortSettings(ENCODER_PORT_AZIM, ENCODER_BAUD_RATE,
                                ENCODER_DATA_BITS, ENCODER_PARITY,
                                ENCODER_STOP_BITS, ENCODER_FLOW_CONTROL);
        encAzim->setQPS(ENCODER_QPS);
        return false;
    }

    //читаем калибровочные настройки начального положения энкодера
    encAzim->readEncNullPos("ENCODER_AZIM");

    return result;
}

bool MainWindow::initDalnomer()
{
    //Открываем конфигурационный файл
    QSettings *settings = new QSettings(FILE_SETTINGS,QSettings::IniFormat);

    //Читаем настройки
    QString port = settings->value("DALNOMER/Port", DALNOMER_PORT).toString();
    int baudRate = settings->value("DALNOMER/BaudRate", DALNOMER_BAUD_RATE).toInt();
    int dataBits = settings->value("DALNOMER/DataBits", DALNOMER_DATA_BITS).toInt();
    int parity   = settings->value("DALNOMER/Parity", DALNOMER_PARITY).toInt();
    int stopBits = settings->value("DALNOMER/StopBits", DALNOMER_STOP_BITS).toInt();
    int flowControl = settings->value("DALNOMER/FlowControl", DALNOMER_FLOW_CONTROL).toInt();
    int qps = settings->value("DALNOMER/QPS", DALNOMER_QPS).toInt();

    if (settings->status() == QSettings::NoError)
    {
        dalnomer->setPortSettings(port, baudRate, dataBits, parity, stopBits, flowControl);
        dalnomer->setQPS(qps);
        return true;
    }
    else
    {
        dalnomer->setPortSettings(DALNOMER_PORT, DALNOMER_BAUD_RATE,
                                DALNOMER_DATA_BITS, DALNOMER_PARITY,
                                DALNOMER_STOP_BITS, DALNOMER_FLOW_CONTROL);
        dalnomer->setQPS(DALNOMER_QPS);
        return true;
    }
}

//Функция инициализации параметров порта группы из 3-х БМСД значениями из конф.файла
bool MainWindow::initBMSD3()
{
    //Открываем конфигурационный файл
    QSettings *settings = new QSettings(FILE_SETTINGS,QSettings::IniFormat);

    //Читаем настройки
    QString port = settings->value("BMSD3/Port", BMSD3_PORT).toString();
    int baudRate = settings->value("BMSD3/BaudRate", BMSD_BAUD_RATE).toInt();
    int dataBits = settings->value("BMSD3/DataBits", BMSD_DATA_BITS).toInt();
    int parity   = settings->value("BMSD3/Parity", BMSD_PARITY).toInt();
    int stopBits = settings->value("BMSD3/StopBits", BMSD_STOP_BITS).toInt();
    int flowControl = settings->value("BMSD3/FlowControl", BMSD_FLOW_CONTROL).toInt();

    if (settings->status() == QSettings::NoError)
    {
        BMSD3->setPortSettings(port, baudRate, dataBits, parity, stopBits, flowControl);
        return true;
    }
    else
    {
        BMSD3->setPortSettings(BMSD3_PORT, BMSD_BAUD_RATE,
                                BMSD_DATA_BITS, BMSD_PARITY,
                                BMSD_STOP_BITS, BMSD_FLOW_CONTROL);
        return true;
    }
}

//Функция инициализации параметров порта группы из 2-х БМСД значениями из конф.файла
bool MainWindow::initBMSD2()
{
    //Открываем конфигурационный файл
    QSettings *settings = new QSettings(FILE_SETTINGS,QSettings::IniFormat);

    //Читаем настройки
    QString port = settings->value("BMSD2/Port", BMSD2_PORT).toString();
    int baudRate = settings->value("BMSD2/BaudRate", BMSD_BAUD_RATE).toInt();
    int dataBits = settings->value("BMSD2/DataBits", BMSD_DATA_BITS).toInt();
    int parity   = settings->value("BMSD2/Parity", BMSD_PARITY).toInt();
    int stopBits = settings->value("BMSD2/StopBits", BMSD_STOP_BITS).toInt();
    int flowControl = settings->value("BMSD2/FlowControl", BMSD_FLOW_CONTROL).toInt();

    if (settings->status() == QSettings::NoError)
    {
        BMSD2->setPortSettings(port, baudRate, dataBits, parity, stopBits, flowControl);
        return true;
    }
    else
    {
        BMSD2->setPortSettings(BMSD2_PORT, BMSD_BAUD_RATE,
                                BMSD_DATA_BITS, BMSD_PARITY,
                                BMSD_STOP_BITS, BMSD_FLOW_CONTROL);
        return true;
    }
}

//Функция инициализации параметров порта точного актуатора значениями из конф.файла
bool MainWindow::initGSM40()
{
    //Открываем конфигурационный файл
    QSettings *settings = new QSettings(FILE_SETTINGS,QSettings::IniFormat);

    //Читаем настройки
    QString port = settings->value("GSM40/Port", GSM40_PORT).toString();
    int baudRate = settings->value("GSM40/BaudRate", GSM40_BAUD_RATE).toInt();
    int dataBits = settings->value("GSM40/DataBits", GSM40_DATA_BITS).toInt();
    int parity   = settings->value("GSM40/Parity", GSM40_PARITY).toInt();
    int stopBits = settings->value("GSM40/StopBits", GSM40_STOP_BITS).toInt();
    int flowControl = settings->value("GSM40/FlowControl", GSM40_FLOW_CONTROL).toInt();

    if (settings->status() == QSettings::NoError)
    {
        GSM40->setPortSettings(port, baudRate, dataBits, parity, stopBits, flowControl);
        return true;
    }
    else
    {
        GSM40->setPortSettings(GSM40_PORT, GSM40_BAUD_RATE,
                                GSM40_DATA_BITS, GSM40_PARITY,
                                GSM40_STOP_BITS, GSM40_FLOW_CONTROL);
        return true;
    }
}

//Функция инициализации адресов БМСД значениями из конф.файла
bool MainWindow::initBmsd()
{
    //Открываем конфигурационный файл
    QSettings *settings = new QSettings(FILE_SETTINGS,QSettings::IniFormat);

    //Читаем настройки
    int adressBmsd1 = settings->value("BMSD_1/Adress", BMSD_ADRESS_1).toInt();
    int adressBmsd2 = settings->value("BMSD_2/Adress", BMSD_ADRESS_2).toInt();
    int adressBmsd3 = settings->value("BMSD_3/Adress", BMSD_ADRESS_3).toInt();
    int adressBmsd4 = settings->value("BMSD_4/Adress", BMSD_ADRESS_4).toInt();
    int adressBmsd5 = settings->value("BMSD_5/Adress", BMSD_ADRESS_5).toInt();
    int adressBmsd6 = settings->value("GSM40/Adress",  GSM40_ADRESS).toInt();

    if (settings->status() == QSettings::NoError)
    {
        bmsd1->setAdress(adressBmsd1);
        bmsd2->setAdress(adressBmsd2);
        bmsd3->setAdress(adressBmsd3);
        bmsd4->setAdress(adressBmsd4);
        bmsd5->setAdress(adressBmsd5);
        gsm40->setAdress(adressBmsd6);
        return true;
    }
    else
    {
        bmsd1->setAdress(BMSD_ADRESS_1);
        bmsd2->setAdress(BMSD_ADRESS_2);
        bmsd3->setAdress(BMSD_ADRESS_3);
        bmsd4->setAdress(BMSD_ADRESS_4);
        bmsd5->setAdress(BMSD_ADRESS_5);
        gsm40->setAdress(GSM40_ADRESS);
        return true;
    }
}



//Функция инициализации потенциометров значениями из конф.файла
bool MainWindow::initPotenciometr()
{
    if (potenciometr1->Init(2, 0) && potenciometr2->Init(2, 2))
        return true;
    else
        return false;
}

//Функция инициализации платы дискретных в\в значениями из конф.файла
bool MainWindow::initStaticDIO()
{
    //здесь возможно должно быть инициализация значений из конф. файла

    //инициализируем переменные
    return staticDIO->Init();
}

//Функция инициализации джойстика
bool MainWindow::initJoystick()
{
    //Открываем конфигурационный файл
    QSettings *settings = new QSettings(FILE_SETTINGS,QSettings::IniFormat);

    //Читаем настройки
    //Допуск начального состояния - исключаем дребезг
    int nullstate = settings->value("JOYSTICK/NULLSTATE", JOYSTICK_NULLSTATE).toInt();

    return joystick->Init(0, nullstate);
}

//Функция инициализации пульта
bool MainWindow::initPult()
{
    //Открываем конфигурационный файл
    QSettings *settings = new QSettings(FILE_SETTINGS,QSettings::IniFormat);

    pult->Init(staticDIO, drive, &networkAbonent);

    //Читаем настройки
    settings->beginGroup("PULT");
//    pult->iPULT_BTN_COMMON_CONTROL    = settings->value("PULT_BTN_COMMON_CONTROL", PULT_BTN_COMMON_CONTROL).toInt();
//    pult->iPULT_BTN_COMMON_WORK       = settings->value("PULT_BTN_COMMON_WORK", PULT_BTN_COMMON_WORK).toInt();
//    pult->iPULT_BTN_COMMON_OBSLUG     = settings->value("PULT_BTN_COMMON_OBSLUG", PULT_BTN_COMMON_OBSLUG).toInt();
//    pult->iPULT_BTN_COMMON_RESERV     = settings->value("PULT_BTN_COMMON_RESERV", PULT_BTN_COMMON_RESERV).toInt();
//    pult->iPULT_BTN_COMMON_DALNOST    = settings->value("PULT_BTN_COMMON_DALNOST", PULT_BTN_COMMON_DALNOST).toInt();
//    pult->iPULT_BTN_COMMON_VISOKOE    = settings->value("PULT_BTN_COMMON_VISOKOE", PULT_BTN_COMMON_VISOKOE).toInt();

//    //Экран
//    pult->iPULT_BTN_SCREEN_MULTI      = settings->value("PULT_BTN_SCREEN_MULTI", PULT_BTN_SCREEN_MULTI).toInt();
//    pult->iPULT_BTN_SCREEN_OBZOR      = settings->value("PULT_BTN_SCREEN_OBZOR", PULT_BTN_SCREEN_OBZOR).toInt();
//    pult->iPULT_BTN_SCREEN_NAVED      = settings->value("PULT_BTN_SCREEN_NAVED", PULT_BTN_SCREEN_NAVED).toInt();
//    pult->iPULT_BTN_SCREEN_KAMERA     = settings->value("PULT_BTN_SCREEN_KAMERA", PULT_BTN_SCREEN_KAMERA).toInt();

//    //Неизвестные
//    pult->iPULT_BTN_UNKNOWN_HP1       = settings->value("PULT_BTN_UNKNOWN_HP1", PULT_BTN_UNKNOWN_HP1).toInt();
//    pult->iPULT_BTN_UNKNOWN_HP2       = settings->value("PULT_BTN_UNKNOWN_HP2", PULT_BTN_UNKNOWN_HP2).toInt();
//    pult->iPULT_BTN_UNKNOWN_HP3       = settings->value("PULT_BTN_UNKNOWN_HP3", PULT_BTN_UNKNOWN_HP3).toInt();
//    pult->iPULT_BTN_UNKNOWN_HP4       = settings->value("PULT_BTN_UNKNOWN_HP4", PULT_BTN_UNKNOWN_HP4).toInt();
//    pult->iPULT_BTN_UNKNOWN_HP5       = settings->value("PULT_BTN_UNKNOWN_HP5", PULT_BTN_UNKNOWN_HP5).toInt();
//    pult->iPULT_BTN_UNKNOWN_HP6       = settings->value("PULT_BTN_UNKNOWN_HP6", PULT_BTN_UNKNOWN_HP6).toInt();
//    pult->iPULT_BTN_UNKNOWN_HP7       = settings->value("PULT_BTN_UNKNOWN_HP7", PULT_BTN_UNKNOWN_HP7).toInt();
//    pult->iPULT_BTN_UNKNOWN_HP8       = settings->value("PULT_BTN_UNKNOWN_HP8", PULT_BTN_UNKNOWN_HP8).toInt();
//    pult->iPULT_BTN_UNKNOWN_HP9       = settings->value("PULT_BTN_UNKNOWN_HP9", PULT_BTN_UNKNOWN_HP9).toInt();
//    pult->iPULT_BTN_UNKNOWN_IP1       = settings->value("PULT_BTN_UNKNOWN_IP1", PULT_BTN_UNKNOWN_IP1).toInt();
//    pult->iPULT_BTN_UNKNOWN_IP2       = settings->value("PULT_BTN_UNKNOWN_IP2", PULT_BTN_UNKNOWN_IP2).toInt();
//    pult->iPULT_BTN_UNKNOWN_IP3       = settings->value("PULT_BTN_UNKNOWN_IP3", PULT_BTN_UNKNOWN_IP3).toInt();

//    //Положение антенны
//    pult->iPULT_BTN_POLOG_SLOG        = settings->value("PULT_BTN_POLOG_SLOG", PULT_BTN_POLOG_SLOG).toInt();
//    pult->iPULT_BTN_POLOG_RAZV        = settings->value("PULT_BTN_POLOG_RAZV", PULT_BTN_POLOG_RAZV).toInt();

//    //Фокусировка антенны
//    pult->iPULT_BTN_FOCUS_35          = settings->value("PULT_BTN_FOCUS_35", PULT_BTN_FOCUS_35).toInt();
//    pult->iPULT_BTN_FOCUS_50          = settings->value("PULT_BTN_FOCUS_50", PULT_BTN_FOCUS_50).toInt();
//    pult->iPULT_BTN_FOCUS_75          = settings->value("PULT_BTN_FOCUS_75", PULT_BTN_FOCUS_75).toInt();
//    pult->iPULT_BTN_FOCUS_100         = settings->value("PULT_BTN_FOCUS_100", PULT_BTN_FOCUS_100).toInt();
//    pult->iPULT_BTN_FOCUS_AUTO        = settings->value("PULT_BTN_FOCUS_AUTO", PULT_BTN_FOCUS_AUTO).toInt();

//    //Наведение
//    pult->iPULT_BTN_NAVED_NIGHT        = settings->value("PULT_BTN_NAVED_NIGHT", PULT_BTN_NAVED_NIGHT).toInt();
//    pult->iPULT_BTN_NAVED_NAVESTI     = settings->value("PULT_BTN_NAVED_NAVESTI", PULT_BTN_NAVED_NAVESTI).toInt();
//    pult->iPULT_BTN_NAVED_PA          = settings->value("PULT_BTN_NAVED_PA", PULT_BTN_NAVED_PA).toInt();
//    pult->iPULT_BTN_NAVED_MANUAL      = settings->value("PULT_BTN_NAVED_MANUAL", PULT_BTN_NAVED_MANUAL).toInt();
//    pult->iPULT_BTN_NAVED_ZOOM_IN     = settings->value("PULT_BTN_NAVED_ZOOM_IN", PULT_BTN_NAVED_ZOOM_IN).toInt();
//    pult->iPULT_BTN_NAVED_ZOOM_OUT    = settings->value("PULT_BTN_NAVED_ZOOM_OUT", PULT_BTN_NAVED_ZOOM_OUT).toInt();

//    //Обзор
//    pult->iPULT_BTN_OBZOR_1           = settings->value("PULT_BTN_OBZOR_1", PULT_BTN_OBZOR_1).toInt();
//    pult->iPULT_BTN_OBZOR_2           = settings->value("PULT_BTN_OBZOR_2", PULT_BTN_OBZOR_2).toInt();
//    pult->iPULT_BTN_OBZOR_3           = settings->value("PULT_BTN_OBZOR_3", PULT_BTN_OBZOR_3).toInt();
//    pult->iPULT_BTN_OBZOR_4           = settings->value("PULT_BTN_OBZOR_4", PULT_BTN_OBZOR_4).toInt();
//    pult->iPULT_BTN_OBZOR_A           = settings->value("PULT_BTN_OBZOR_A", PULT_BTN_OBZOR_A).toInt();
//    pult->iPULT_BTN_OBZOR_C           = settings->value("PULT_BTN_OBZOR_C", PULT_BTN_OBZOR_C).toInt();
//    pult->iPULT_BTN_OBZOR_ZOOM_IN     = settings->value("PULT_BTN_OBZOR_ZOOM_IN", PULT_BTN_OBZOR_ZOOM_IN).toInt();
//    pult->iPULT_BTN_OBZOR_ZOOM_OUT    = settings->value("PULT_BTN_OBZOR_ZOOM_OUT", PULT_BTN_OBZOR_ZOOM_OUT).toInt();


//    //Номера выходов платы дискретных в\в для индикаторов пульта управления
//    //Индикаторы без группы
//    pult->iPULT_IND_COMMON_CONTROL    = settings->value("PULT_IND_COMMON_CONTROL", PULT_IND_COMMON_CONTROL).toInt();
//    pult->iPULT_IND_COMMON_AC         = settings->value("PULT_IND_COMMON_AC", PULT_IND_COMMON_AC).toInt();
//    pult->iPULT_IND_COMMON_GK         = settings->value("PULT_IND_COMMON_GK", PULT_IND_COMMON_GK).toInt();
//    pult->iPULT_IND_COMMON_VISOKOE    = settings->value("PULT_IND_COMMON_VISOKOE", PULT_IND_COMMON_VISOKOE).toInt();
//    //Зумер
//    pult->iPULT_IND_COMMON_ZOOMER     = settings->value("PULT_IND_COMMON_ZOOMER", PULT_IND_COMMON_ZOOMER).toInt();

//    //Контроль состояния
//    pult->iPULT_IND_STATUS_APU        = settings->value("PULT_IND_STATUS_APU", PULT_IND_STATUS_APU).toInt();
//    pult->iPULT_IND_STATUS_OEC        = settings->value("PULT_IND_STATUS_OEC", PULT_IND_STATUS_OEC).toInt();
//    pult->iPULT_IND_STATUS_LVS        = settings->value("PULT_IND_STATUS_LVS", PULT_IND_STATUS_LVS).toInt();
//    pult->iPULT_IND_STATUS_SPOIL      = settings->value("PULT_IND_STATUS_SPOIL", PULT_IND_STATUS_SPOIL).toInt();

//    //Экран
//    pult->iPULT_IND_SCREEN_MULTI      = settings->value("PULT_IND_SCREEN_MULTI", PULT_IND_SCREEN_MULTI).toInt();
//    pult->iPULT_IND_SCREEN_OBZOR      = settings->value("PULT_IND_SCREEN_OBZOR", PULT_IND_SCREEN_OBZOR).toInt();
//    pult->iPULT_IND_SCREEN_NAVED      = settings->value("PULT_IND_SCREEN_NAVED", PULT_IND_SCREEN_NAVED).toInt();
//    pult->iPULT_IND_SCREEN_KAMERA     = settings->value("PULT_IND_SCREEN_KAMERA", PULT_IND_SCREEN_KAMERA).toInt();

//    //Неизвестные
//    pult->iPULT_IND_UNKNOWN_HP1       = settings->value("PULT_IND_UNKNOWN_HP1", PULT_IND_UNKNOWN_HP1).toInt();
//    pult->iPULT_IND_UNKNOWN_HP2       = settings->value("PULT_IND_UNKNOWN_HP2", PULT_IND_UNKNOWN_HP2).toInt();
//    pult->iPULT_IND_UNKNOWN_HP3       = settings->value("PULT_IND_UNKNOWN_HP3", PULT_IND_UNKNOWN_HP3).toInt();
//    pult->iPULT_IND_UNKNOWN_HP4       = settings->value("PULT_IND_UNKNOWN_HP4", PULT_IND_UNKNOWN_HP4).toInt();
//    pult->iPULT_IND_UNKNOWN_HP5       = settings->value("PULT_IND_UNKNOWN_HP5", PULT_IND_UNKNOWN_HP5).toInt();
//    pult->iPULT_IND_UNKNOWN_HP6       = settings->value("PULT_IND_UNKNOWN_HP6", PULT_IND_UNKNOWN_HP6).toInt();
//    pult->iPULT_IND_UNKNOWN_HP7       = settings->value("PULT_IND_UNKNOWN_HP7", PULT_IND_UNKNOWN_HP7).toInt();
//    pult->iPULT_IND_UNKNOWN_HP8       = settings->value("PULT_IND_UNKNOWN_HP8", PULT_IND_UNKNOWN_HP8).toInt();
//    pult->iPULT_IND_UNKNOWN_HP9       = settings->value("PULT_IND_UNKNOWN_HP9", PULT_IND_UNKNOWN_HP9).toInt();
//    pult->iPULT_IND_UNKNOWN_IP1       = settings->value("PULT_IND_UNKNOWN_IP1", PULT_IND_UNKNOWN_IP1).toInt();
//    pult->iPULT_IND_UNKNOWN_IP2       = settings->value("PULT_IND_UNKNOWN_IP2", PULT_IND_UNKNOWN_IP2).toInt();
//    pult->iPULT_IND_UNKNOWN_IP3       = settings->value("PULT_IND_UNKNOWN_IP3", PULT_IND_UNKNOWN_IP3).toInt();

//    //Положение антенны
//    pult->iPULT_IND_POLOG_POHOD       = settings->value("PULT_IND_POLOG_POHOD", PULT_IND_POLOG_POHOD).toInt();
//    pult->iPULT_IND_POLOG_WORK        = settings->value("PULT_IND_POLOG_WORK", PULT_IND_POLOG_WORK).toInt();

//    //Фокусировка антенны
//    pult->iPULT_IND_FOCUS_35          = settings->value("PULT_IND_FOCUS_35", PULT_IND_FOCUS_35).toInt();
//    pult->iPULT_IND_FOCUS_50          = settings->value("PULT_IND_FOCUS_50", PULT_IND_FOCUS_50).toInt();
//    pult->iPULT_IND_FOCUS_75          = settings->value("PULT_IND_FOCUS_75", PULT_IND_FOCUS_75).toInt();
//    pult->iPULT_IND_FOCUS_100         = settings->value("PULT_IND_FOCUS_100", PULT_IND_FOCUS_100).toInt();
//    pult->iPULT_IND_FOCUS_AUTO        = settings->value("PULT_IND_FOCUS_AUTO", PULT_IND_FOCUS_AUTO).toInt();

//    //Наведение
//    pult->iPULT_IND_NAVED_NIGHT        = settings->value("PULT_IND_NAVED_NGHT", PULT_IND_NAVED_NGHT).toInt();
//    pult->iPULT_IND_NAVED_NAVESTI     = settings->value("PULT_IND_NAVED_NAVESTI", PULT_IND_NAVED_NAVESTI).toInt();
//    pult->iPULT_IND_NAVED_PA          = settings->value("PULT_IND_NAVED_PA", PULT_IND_NAVED_PA).toInt();
//    pult->iPULT_IND_NAVED_MANUAL      = settings->value("PULT_IND_NAVED_MANUAL", PULT_IND_NAVED_MANUAL).toInt();
//    pult->iPULT_IND_NAVED_ZOOM_IN     = settings->value("PULT_IND_NAVED_ZOOM_IN", PULT_IND_NAVED_ZOOM_IN).toInt();
//    pult->iPULT_IND_NAVED_ZOOM_OUT    = settings->value("PULT_IND_NAVED_ZOOM_OUT", PULT_IND_NAVED_ZOOM_OUT).toInt();

//    //Обзор
//    pult->iPULT_IND_OBZOR_1           = settings->value("PULT_IND_OBZOR_1", PULT_IND_OBZOR_1).toInt();
//    pult->iPULT_IND_OBZOR_2           = settings->value("PULT_IND_OBZOR_2", PULT_IND_OBZOR_2).toInt();
//    pult->iPULT_IND_OBZOR_3           = settings->value("PULT_IND_OBZOR_3", PULT_IND_OBZOR_3).toInt();
//    pult->iPULT_IND_OBZOR_4           = settings->value("PULT_IND_OBZOR_4", PULT_IND_OBZOR_4).toInt();
//    pult->iPULT_IND_OBZOR_A           = settings->value("PULT_IND_OBZOR_A", PULT_IND_OBZOR_A).toInt();
//    pult->iPULT_IND_OBZOR_C           = settings->value("PULT_IND_OBZOR_C", PULT_IND_OBZOR_C).toInt();
//    pult->iPULT_IND_OBZOR_ZOOM_IN     = settings->value("PULT_IND_OBZOR_ZOOM_IN", PULT_IND_OBZOR_ZOOM_IN).toInt();
//    pult->iPULT_IND_OBZOR_ZOOM_OUT    = settings->value("PULT_IND_OBZOR_ZOOM_OUT", PULT_IND_OBZOR_ZOOM_OUT).toInt();
    settings->endGroup();


    return true;
}

//Функция инициализации объекта расчета движения
bool MainWindow::initDrive()
{
    drive->Init(bmsd1, bmsd2, bmsd3, bmsd4, bmsd5, gsm40, staticDIO);
    drive->timer->start();
//    drive->timerTest->start();
    return true;
}

//Функция инициализации объекта проверки системы
bool MainWindow::initTest()
{
    //Открываем конфигурационный файл
    QSettings *settings = new QSettings(FILE_SETTINGS,QSettings::IniFormat);

    //Читаем настройки
    QString adrCam1 = settings->value("TEST/AdressCam1", DEFAULT_CAM1_ADRESS).toString();
    QString adrCam2 = settings->value("TEST/AdressCam2", DEFAULT_CAM2_ADRESS).toString();
    QString adrCam3 = settings->value("TEST/AdressCam3", DEFAULT_CAM3_ADRESS).toString();
    QString adrCam4 = settings->value("TEST/AdressCam4", DEFAULT_CAM4_ADRESS).toString();
    QString adrVizir = settings->value("TEST/AdressVizir", DEFAULT_VIZIR_ADRESS).toString();
    QString adrTeplovizor = settings->value("TEST/AdressTeplovizor", DEFAULT_TEPLOVIZOR_ADRESS).toString();

    test->Init(encUgol,encAzim, dalnomer, bmsd1, bmsd2, bmsd3, bmsd4, bmsd5, gsm40, potenciometr1,
               potenciometr2, staticDIO, joystick,
               adrCam1, adrCam2, adrCam3, adrCam4, adrVizir, adrTeplovizor, netPCVideoAdress);

    timerTest = new QTimer();
    connect(timerTest, SIGNAL(timeout()), this, SLOT(testSystem()));
    timerTest->setInterval(TEST_PERIOD);
    timerTest->start();
    return true;
}

//Фукнция изменения состояния кнопки в интерфейсе
void MainWindow::setBtnState(QPushButton *btn, bool state)
{
    //если нужно установить в состояние установлено
    if (state)
        //если кнопки из группы красных
        if (btn == ui->btnPultStatusAPU ||
                btn == ui->btnPultStatusLVS ||
                btn == ui->btnPultStatusSPOIL ||
                btn == ui->btnPultStatusOEC ||
                btn == ui->btnPultCommonVisokoe1)
            //устанавливаем красную иконку
            btn->setIcon(QIcon("://resourses/redBtnOn.png"));
        else if (btn == ui->btnPultCommonStateSystem)
            //устанавливаем переключатель
            btn->setIcon(QIcon("://resourses/selector_rabota.png"));
        else
            //в противном случае зеленую
            btn->setIcon(QIcon("://resourses/greenBtnOn.png"));
    //если нужно установить в состояние сброшено
    else
        //если кнопки из группы красных
        if (btn == ui->btnPultStatusAPU ||
                btn == ui->btnPultStatusLVS ||
                btn == ui->btnPultStatusSPOIL ||
                btn == ui->btnPultStatusOEC ||
                btn == ui->btnPultCommonVisokoe1)
            //устанавливаем красную иконку
            btn->setIcon(QIcon("://resourses/redBtnOff.png"));
        else if (btn == ui->btnPultCommonStateSystem)
            //устанавливаем переключатель
            btn->setIcon(QIcon("://resourses/selector_block.png"));
        else
            //в противном случае зеленую
            btn->setIcon(QIcon("://resourses/greenBtnOff.png"));
}

void MainWindow::on_sbSetAdressBMSD2_clicked()
{
    PBMSD *tmpBMSD = new PBMSD(BMSD2);
//    connect(tmpBMSD, SIGNAL(newCommand(QString)), BMSD2, SLOT(sendCommand(QString)));
    tmpBMSD->newAdress(ui->sbAdressBMSD2->text().toInt());
//    disconnect(tmpBMSD, SIGNAL(newCommand(QString)), BMSD2, SLOT(sendCommand(QString)));
}

void MainWindow::on_sbAbortAdressBMSD2_clicked()
{
    PBMSD *tmpBmsd = new PBMSD(BMSD2);
//    connect(tmpBmsd, SIGNAL(newCommand(QString)), BMSD2, SLOT(sendCommand(QString)));
    tmpBmsd->newAbortAdress();
//    disconnect(tmpBmsd, SIGNAL(newCommand(QString)), BMSD2, SLOT(sendCommand(QString)));
}

void MainWindow::on_sbSetAdressBMSD3_clicked()
{
    PBMSD *tmpBMSD = new PBMSD(BMSD3);
//    connect(tmpBMSD, SIGNAL(newCommand(QString)), BMSD3, SLOT(sendCommand(QString)));
    tmpBMSD->newAdress(ui->sbAdressBMSD3->text().toInt());
//    disconnect(tmpBMSD, SIGNAL(newCommand(QString)), BMSD3, SLOT(sendCommand(QString)));
}

void MainWindow::on_sbAbortAdressBMSD3_clicked()
{
    PBMSD *tmpBmsd = new PBMSD(BMSD3);
//    connect(tmpBmsd, SIGNAL(newCommand(QString)), BMSD3, SLOT(sendCommand(QString)));
    tmpBmsd->newAbortAdress();
//    disconnect(tmpBmsd, SIGNAL(newCommand(QString)), BMSD3, SLOT(sendCommand(QString)));
}
//--------------------------------------------------------БМСД1
void MainWindow::on_btnDirectLeftBmsd_1_clicked()
{
    bmsd1->newDirection(BMSD_DIRECTION_LEFT);
    updateBmsd1UI();
}

void MainWindow::on_btnDirectRightBmsd_1_clicked()
{
    bmsd1->newDirection(BMSD_DIRECTION_RIGHT);
    updateBmsd1UI();
}

//--------------------------------------------------------БМСД2
void MainWindow::on_btnDirectLeftBmsd_2_clicked()
{
    bmsd2->newDirection(BMSD_DIRECTION_LEFT);
    updateBmsd2UI();
}

void MainWindow::on_btnDirectRightBmsd_2_clicked()
{
    bmsd2->newDirection(BMSD_DIRECTION_RIGHT);
    updateBmsd2UI();
}

//--------------------------------------------------------БМСД3
void MainWindow::on_btnDirectLeftBmsd_3_clicked()
{
    bmsd3->newDirection(BMSD_DIRECTION_LEFT);
    updateBmsd3UI();
}

void MainWindow::on_btnDirectRightBmsd_3_clicked()
{
    bmsd3->newDirection(BMSD_DIRECTION_RIGHT);
    updateBmsd3UI();
}

//--------------------------------------------------------БМСД4
void MainWindow::on_btnDirectLeftBmsd_4_clicked()
{
    bmsd4->newDirection(BMSD_DIRECTION_LEFT);
    updateBmsd4UI();
}

void MainWindow::on_btnDirectRightBmsd_4_clicked()
{
    bmsd4->newDirection(BMSD_DIRECTION_RIGHT);
    updateBmsd4UI();
}

//--------------------------------------------------------БМСД5
void MainWindow::on_btnDirectLeftBmsd_5_clicked()
{
    bmsd5->newDirection(BMSD_DIRECTION_LEFT);
    updateBmsd5UI();
}

void MainWindow::on_btnDirectRightBmsd_5_clicked()
{
    bmsd5->newDirection(BMSD_DIRECTION_RIGHT);
    updateBmsd5UI();
}

//--------------------------------------------------------БМСД6
void MainWindow::on_btnDirectLeftBmsd_6_clicked()
{
    gsm40->newDirection(GSM40_DIRECTION_DOWN);
    updateGsm40UI();
}

void MainWindow::on_btnDirectRightBmsd_6_clicked()
{
    gsm40->newDirection(GSM40_DIRECTION_UP);
    updateGsm40UI();
}

void MainWindow::on_btnPultScreenMulti_pressed()
{
    emit pult->newIn(PULT_BTN_SCREEN_MULTI, true);
    emit networkVideo.send(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_SCREEN_MULTI);
}

void MainWindow::on_btnPultScreenObzor_pressed()
{
    emit pult->newIn(PULT_BTN_SCREEN_OBZOR, true);
    emit networkVideo.send(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_SCREEN_OBZOR);
}

void MainWindow::on_btnPultScreenNaved_pressed()
{
    emit pult->newIn(PULT_BTN_SCREEN_NAVED, true);
    emit networkVideo.send(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_SCREEN_NAVED);
}

void MainWindow::on_btnPultScreenCamera_pressed()
{
    emit pult->newIn(PULT_BTN_SCREEN_KAMERA, true);
    emit networkVideo.send(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_SCREEN_KAMERA);
}

void MainWindow::on_btnPultUnknownHP1_pressed()
{
    emit pult->newIn(PULT_BTN_UNKNOWN_HP1, true);
}

void MainWindow::on_btnPultUnknownHP2_pressed()
{
    emit pult->newIn(PULT_BTN_UNKNOWN_HP2, true);
}

void MainWindow::on_btnPultUnknownHP3_pressed()
{
    emit pult->newIn(PULT_BTN_UNKNOWN_HP3, true);
}

void MainWindow::on_btnPultUnknownHP4_pressed()
{
    emit pult->newIn(PULT_BTN_UNKNOWN_HP4, true);
}

void MainWindow::on_btnPultUnknownHP5_pressed()
{
    emit pult->newIn(PULT_BTN_UNKNOWN_HP5, true);
}

void MainWindow::on_btnPultUnknownHP6_pressed()
{
    emit pult->newIn(PULT_BTN_UNKNOWN_HP6, true);
}

void MainWindow::on_btnPultUnknownHP7_pressed()
{
    emit pult->newIn(PULT_BTN_UNKNOWN_HP7, true);
}

void MainWindow::on_btnPultUnknownHP8_pressed()
{
    emit pult->newIn(PULT_BTN_UNKNOWN_HP8, true);
}

void MainWindow::on_btnPultUnknownHP9_pressed()
{
    emit pult->newIn(PULT_BTN_UNKNOWN_HP9, true);
}

void MainWindow::on_btnPultUnknownIP1_pressed()
{
    emit pult->newIn(PULT_BTN_UNKNOWN_IP1, true);
}

void MainWindow::on_btnPultUnknownIP2_pressed()
{
    emit pult->newIn(PULT_BTN_UNKNOWN_IP2, true);
}

void MainWindow::on_btnPultUnknownIP3_pressed()
{
    emit pult->newIn(PULT_BTN_UNKNOWN_IP3, true);
}

void MainWindow::on_btnPultFocus35_pressed()
{
    emit pult->newIn(PULT_BTN_FOCUS_35, true);
}

void MainWindow::on_btnPultFocus50_pressed()
{
    emit pult->newIn(PULT_BTN_FOCUS_50, true);
}

void MainWindow::on_btnPultFocus75_pressed()
{
    emit pult->newIn(PULT_BTN_FOCUS_75, true);
}

void MainWindow::on_btnPultFocus100_pressed()
{
    emit pult->newIn(PULT_BTN_FOCUS_100, true);
}

void MainWindow::on_btnPultFocusAuto_pressed()
{
    emit pult->newIn(PULT_BTN_FOCUS_AUTO, true);
}

void MainWindow::on_btnPultNavedNight_pressed()
{
    emit pult->newIn(PULT_BTN_NAVED_NIGHT, true);
}

void MainWindow::on_btnPultNavedNavesti_pressed()
{
    emit pult->newIn(PULT_BTN_NAVED_NAVESTI, true);
}

void MainWindow::on_btnPultNavedPA_pressed()
{
    emit pult->newIn(PULT_BTN_NAVED_PA, true);
}

void MainWindow::on_btnPultNavedManual_pressed()
{
    emit pult->newIn(PULT_BTN_NAVED_MANUAL, true);
}

void MainWindow::on_btnPultNavedZoomIn_pressed()
{
    emit pult->newIn(PULT_BTN_NAVED_ZOOM_IN, true);
}
void MainWindow::on_btnPultNavedZoomIn_released()
{
    emit pult->newIn(PULT_BTN_NAVED_ZOOM_IN, false);
}

void MainWindow::on_btnPultNavedZoomOut_pressed()
{
    emit pult->newIn(PULT_BTN_NAVED_ZOOM_OUT, true);
}

void MainWindow::on_btnPultNavedZoomOut_released()
{
    emit pult->newIn(PULT_BTN_NAVED_ZOOM_OUT, false);
}

void MainWindow::on_btnPultObzor1_pressed()
{
    emit pult->newIn(PULT_BTN_OBZOR_1, true);
}

void MainWindow::on_btnPultObzor2_pressed()
{
    emit pult->newIn(PULT_BTN_OBZOR_2, true);
}

void MainWindow::on_btnPultObzor3_pressed()
{
    emit pult->newIn(PULT_BTN_OBZOR_3, true);
}

void MainWindow::on_btnPultObzor4_pressed()
{
    emit pult->newIn(PULT_BTN_OBZOR_4, true);
}

void MainWindow::on_btnPultObzorA_pressed()
{
    emit pult->newIn(PULT_BTN_OBZOR_A, true);
    emit networkVideo.send(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_OBZOR_A);
}

void MainWindow::on_btnPultObzorC_pressed()
{
    emit pult->newIn(PULT_BTN_OBZOR_C, true);
    emit networkVideo.send(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_OBZOR_C);
}

void MainWindow::on_btnPultObzorZoomIn_pressed()
{
    emit pult->newIn(PULT_BTN_OBZOR_ZOOM_IN, true);
    emit networkVideo.send(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_OBZOR_ZOOM_IN);
}
void MainWindow::on_btnPultObzorZoomIn_released()
{
    emit pult->newIn(PULT_BTN_OBZOR_ZOOM_IN, false);
    emit networkVideo.send(CP_COMMAND_OUTPUT_BUTTON_RELEASE, 0, PULT_BTN_OBZOR_ZOOM_IN);
}

void MainWindow::on_btnPultObzorZoomOut_pressed()
{
    emit pult->newIn(PULT_BTN_OBZOR_ZOOM_OUT, true);
    emit networkVideo.send(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_OBZOR_ZOOM_OUT);
}
void MainWindow::on_btnPultObzorZoomOut_released()
{
    emit pult->newIn(PULT_BTN_OBZOR_ZOOM_OUT, false);
    emit networkVideo.send(CP_COMMAND_OUTPUT_BUTTON_RELEASE, 0, PULT_BTN_OBZOR_ZOOM_OUT);
}

void MainWindow::on_btnPultCommonReserv_pressed()
{
    emit pult->newIn(PULT_BTN_COMMON_RESERV, true);
}

void MainWindow::on_btnPultCommonDalnost_pressed()
{
    emit pult->newIn(PULT_BTN_COMMON_DALNOST, true);
    emit networkVideo.send(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_COMMON_DALNOST);
    int dalnost = dalnomer->getDalnost();
    emit networkVideo.send(CP_COMMAND_DALNOMER, dalnost >> 8 , dalnost & 0xFF);
}

void MainWindow::on_btnPultCommonDalnost_released()
{
    emit pult->newIn(PULT_BTN_COMMON_DALNOST, false);
    emit networkVideo.send(CP_COMMAND_OUTPUT_BUTTON_RELEASE, 0, PULT_BTN_COMMON_DALNOST);
}

void MainWindow::on_btnPultCommonVisokoe_pressed()
{
    emit pult->newIn(PULT_BTN_COMMON_VISOKOE, true);
}
void MainWindow::on_btnPultCommonVisokoe_released()
{
    emit pult->newIn(PULT_BTN_COMMON_VISOKOE, false);
}

void MainWindow::on_btnPultCommonControl_pressed()
{
   emit pult->newIn(PULT_BTN_COMMON_CONTROL, true);
}

void MainWindow::on_btnPultCommonControl_released()
{
    emit pult->newIn(PULT_BTN_COMMON_CONTROL, false);
}

void MainWindow::on_btnPultPologSlog_pressed()
{
//    if (drive->pologInit)
//        return;
    emit pult->newIn(PULT_BTN_POLOG_SLOG, true);
    emit networkVideo.send(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_POLOG_SLOG);
//    if (pult->getStateSystem() != BLOCK && pult->getStatePolog() == WORKING)
//        drive->setPolog(POHOD);
}

void MainWindow::on_btnPultPologRazv_pressed()
{
    if (drive->pologInit)
        return;
    emit pult->newIn(PULT_BTN_POLOG_RAZV, true);
    emit networkVideo.send(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_POLOG_RAZV);
//    if (pult->getStateSystem() != BLOCK && pult->getStatePolog() == POHOD)
//        drive->setPolog(WORKING);
    if (pult->getStateSystem() != BLOCK && drive->polog == POHOD)
        drive->setPolog(WORKING);
}


void MainWindow::on_btnSetUskBmsd_1_clicked()
{
    bmsd1->newUskorenie(ui->sbUskBmsd_1->value());
}

void MainWindow::on_btnSetTormozBmsd_1_clicked()
{
    bmsd1->newTormogenie(ui->sbTormozBmsd_1->value());
}

void MainWindow::on_btnSetSpeedBmsd_1_clicked()
{
    bmsd1->newSpeed(ui->sbSpeedBmsd_1->value());
}

void MainWindow::on_btnSetUskBmsd_2_clicked()
{
    bmsd2->newUskorenie(ui->sbUskBmsd_2->value());
}

void MainWindow::on_btnSetTormozBmsd_2_clicked()
{
    bmsd2->newTormogenie(ui->sbTormozBmsd_2->value());
}

void MainWindow::on_btnSetSpeedBmsd_2_clicked()
{
    bmsd2->newSpeed(ui->sbSpeedBmsd_2->value());
}

void MainWindow::on_btnSetUskBmsd_3_clicked()
{
    bmsd3->newUskorenie(ui->sbUskBmsd_3->value());
}

void MainWindow::on_btnSetTormozBmsd_3_clicked()
{
    bmsd3->newTormogenie(ui->sbTormozBmsd_3->value());
}

void MainWindow::on_btnSetSpeedBmsd_3_clicked()
{
    bmsd3->newSpeed(ui->sbSpeedBmsd_3->value());
}

void MainWindow::on_btnSetUskBmsd_4_clicked()
{
    bmsd4->newUskorenie(ui->sbUskBmsd_4->value());
}

void MainWindow::on_btnSetTormozBmsd_4_clicked()
{
    bmsd4->newTormogenie(ui->sbTormozBmsd_4->value());
}

void MainWindow::on_btnSetSpeedBmsd_4_clicked()
{
    bmsd4->newSpeed(ui->sbSpeedBmsd_4->value());
}

void MainWindow::on_btnSetUskBmsd_5_clicked()
{
    bmsd5->newUskorenie(ui->sbUskBmsd_5->value());
}

void MainWindow::on_btnSetTormozBmsd_5_clicked()
{
    bmsd5->newTormogenie(ui->sbTormozBmsd_5->value());
}

void MainWindow::on_btnSetSpeedBmsd_5_clicked()
{
    bmsd5->newSpeed(ui->sbSpeedBmsd_5->value());
}

void MainWindow::on_btnSetUskBmsd_6_clicked()
{
    gsm40->newUskorenie(ui->sbUskBmsd_6->value());
}

void MainWindow::on_btnSetTormozBmsd_6_clicked()
{
    gsm40->newTormogenie(ui->sbTormozBmsd_6->value());
}

void MainWindow::on_btnSetSpeedBmsd_6_clicked()
{
    gsm40->newSpeed(ui->sbSpeedBmsd_6->value());
}

void MainWindow::on_btnPultCommonStateSystem_clicked()
{
    systemState++;

    if (systemState > OBSLUG)
        systemState = BLOCK;

    switch (systemState)
    {
    case WORK:
        emit pult->newIn(PULT_BTN_COMMON_WORK, true);
//        emit networkVideo.send(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_COMMON_WORK);
        setBtnState(ui->btnPultCommonStateSystem, true);
        ui->lblBlock->setFont(QFont("MS Shell Dlg 2", 8));
        ui->lblWork->setFont(QFont("MS Shell Dlg 2", 8, QFont::Bold));
        ui->lblObslug->setFont(QFont("MS Shell Dlg 2", 8));

        break;
    case BLOCK:
        emit pult->newIn(PULT_BTN_COMMON_WORK, false);
        emit pult->newIn(PULT_BTN_COMMON_OBSLUG, false);
//        emit networkVideo.send(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_COMMON_BLOCK);
        setBtnState(ui->btnPultCommonStateSystem, false);
        ui->lblBlock->setFont(QFont("MS Shell Dlg 2", 8, QFont::Bold));
        ui->lblWork->setFont(QFont("MS Shell Dlg 2", 8));
        ui->lblObslug->setFont(QFont("MS Shell Dlg 2", 8));
        break;
    case OBSLUG:
        emit pult->newIn(PULT_BTN_COMMON_OBSLUG, true);
//        emit networkVideo.send(CP_COMMAND_OUTPUT_BUTTON_PRESS, 0, PULT_BTN_COMMON_OBSLUG);
        setBtnState(ui->btnPultCommonStateSystem, false);
        ui->lblBlock->setFont(QFont("MS Shell Dlg 2", 8));
        ui->lblWork->setFont(QFont("MS Shell Dlg 2", 8));
        ui->lblObslug->setFont(QFont("MS Shell Dlg 2", 8, QFont::Bold));
        break;
    }
}


void MainWindow::on_cbKoncLeftNZ_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->cbKoncLeftNO->setCheckState(Qt::Unchecked);
        drive->newStaticDIOState(IN_KONC_LEFT_NZ, true);
    }
    else
    {
        ui->cbKoncLeftNO->setCheckState(Qt::Checked);
        drive->newStaticDIOState(IN_KONC_LEFT_NZ, false);
    }
}

void MainWindow::on_cbKoncLeftNO_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->cbKoncLeftNZ->setCheckState(Qt::Unchecked);
        drive->newStaticDIOState(IN_KONC_LEFT_NO, true);
    }
    else
    {
        ui->cbKoncLeftNZ->setCheckState(Qt::Checked);
        drive->newStaticDIOState(IN_KONC_LEFT_NO, false);
    }
}

void MainWindow::on_cbKoncRightNZ_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->cbKoncRightNO->setCheckState(Qt::Unchecked);
        drive->newStaticDIOState(IN_KONC_RIGHT_NZ, true);
    }
    else
    {
        ui->cbKoncRightNO->setCheckState(Qt::Checked);
        drive->newStaticDIOState(IN_KONC_RIGHT_NZ, false);
    }
}

void MainWindow::on_cbKoncRightNO_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->cbKoncRightNZ->setCheckState(Qt::Unchecked);
        drive->newStaticDIOState(IN_KONC_RIGHT_NO, true);
    }
    else
    {
        ui->cbKoncRightNZ->setCheckState(Qt::Checked);
        drive->newStaticDIOState(IN_KONC_RIGHT_NO, false);
    }
}

void MainWindow::on_cbKoncSpoilPerNZ_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->cbKoncSpoilPerNO->setCheckState(Qt::Unchecked);
        drive->newStaticDIOState(IN_KONC_SPOIL_PER_NZ, true);
    }
    else
    {
        ui->cbKoncSpoilPerNO->setCheckState(Qt::Checked);
        drive->newStaticDIOState(IN_KONC_SPOIL_PER_NZ, false);
    }
}

void MainWindow::on_cbKoncSpoilPerNO_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->cbKoncSpoilPerNZ->setCheckState(Qt::Unchecked);
        drive->newStaticDIOState(IN_KONC_SPOIL_PER_NO, true);
    }
    else
    {
        ui->cbKoncSpoilPerNZ->setCheckState(Qt::Checked);
        drive->newStaticDIOState(IN_KONC_SPOIL_PER_NO, false);
    }
}

void MainWindow::on_cbKoncSpoilZadNZ_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->cbKoncSpoilZadNO->setCheckState(Qt::Unchecked);
        drive->newStaticDIOState(IN_KONC_SPOIL_ZAD_NZ, true);
    }
    else
    {
        ui->cbKoncSpoilZadNO->setCheckState(Qt::Checked);
        drive->newStaticDIOState(IN_KONC_SPOIL_ZAD_NZ, false);
    }
}

void MainWindow::on_cbKoncSpoilZadNO_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->cbKoncSpoilZadNZ->setCheckState(Qt::Unchecked);
        drive->newStaticDIOState(IN_KONC_SPOIL_ZAD_NO, true);
    }
    else
    {
        ui->cbKoncSpoilZadNZ->setCheckState(Qt::Checked);
        drive->newStaticDIOState(IN_KONC_SPOIL_ZAD_NO, false);
    }
}

void MainWindow::on_cbKoncZTUNZ_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->cbKoncZTUNO->setCheckState(Qt::Unchecked);
        drive->newStaticDIOState(IN_KONC_ZTU_NZ, true);
    }
    else
    {
        ui->cbKoncZTUNO->setCheckState(Qt::Checked);
        drive->newStaticDIOState(IN_KONC_ZTU_NZ, false);
    }
}

void MainWindow::on_cbKoncZTUNO_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->cbKoncZTUNZ->setCheckState(Qt::Unchecked);
        drive->newStaticDIOState(IN_KONC_ZTU_NO, true);
    }
    else
    {
        ui->cbKoncZTUNZ->setCheckState(Qt::Checked);
        drive->newStaticDIOState(IN_KONC_ZTU_NO, false);
    }
}

void MainWindow::on_cbGerkonNZ_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->cbGerkonNO->setCheckState(Qt::Unchecked);
        drive->newStaticDIOState(IN_GERKON_NZ, true);
    }
    else
    {
        ui->cbGerkonNO->setCheckState(Qt::Checked);
        drive->newStaticDIOState(IN_GERKON_NZ, false);
    }
}

void MainWindow::on_cbGerkonNO_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->cbGerkonNZ->setCheckState(Qt::Unchecked);
        drive->newStaticDIOState(IN_GERKON_NO, true);
    }
    else
    {
        ui->cbGerkonNZ->setCheckState(Qt::Checked);
        drive->newStaticDIOState(IN_GERKON_NO, false);
    }
}


void MainWindow::on_cbZamokAPU1_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
        drive->newStaticDIOState(OUT_ZAMOK_APU1, true);
    else
        drive->newStaticDIOState(OUT_ZAMOK_APU1, false);
}

void MainWindow::on_cbZamokAPU2_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
        drive->newStaticDIOState(OUT_ZAMOK_APU2, true);
    else
        drive->newStaticDIOState(OUT_ZAMOK_APU2, false);
}

void MainWindow::on_cbZamokSpoilPered_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
        drive->newStaticDIOState(OUT_ZAMOK_SPOIL_PERED, true);
    else
        drive->newStaticDIOState(OUT_ZAMOK_SPOIL_PERED, false);
}

void MainWindow::on_cbZamokSpoilZad_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
        drive->newStaticDIOState(OUT_ZAMOK_SPOIL_ZAD, true);
    else
        drive->newStaticDIOState(OUT_ZAMOK_SPOIL_ZAD, false);
}

void MainWindow::on_cbImitaciya_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        drive->imitaciya = true;
        ui->sbImitUgol->setEnabled(true);
        ui->sbImitAzim->setEnabled(true);
    }
    else
    {
        drive->imitaciya = false;
        ui->sbImitUgol->setEnabled(false);
        ui->sbImitAzim->setEnabled(false);
    }

}

void MainWindow::on_sbImitUgol_valueChanged(int arg1)
{
    if (drive->imitaciya == true)
    {
        drive->posYgrad = arg1;
        ui->lblEncUgolSteps256->setText(QString("%1").arg(drive->posYgrad));
        ui->lblEncUgolSteps->setText(QString("%1").arg(drive->posYmin));
    }
}

void MainWindow::on_sbImitAzim_valueChanged(int arg1)
{
    if (drive->imitaciya == true)
    {
        drive->posXgrad = arg1;
        ui->lblEncAzimSteps256->setText(QString("%1").arg(drive->posXgrad));
        ui->lblEncAzimSteps->setText(QString("%1").arg(drive->posXmin));
    }
}
