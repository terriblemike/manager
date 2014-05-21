#ifndef PTEST_H
#define PTEST_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QFile>
#include <QStringList>
#include <QTextCodec>
#include <QProcess>

#include "pencoder.h"
#include "pdalnomer.h"
#include "pbmsd.h"
#include "pstaticdio.h"
#include "pjoystick.h"
#include "pgsm40.h"
#include "ppotenciometr.h"
#include "pstatus.h"
#include "CP_Button_Definitions.h"

#define TEST_PERIOD     5000

class PTest : public QObject
{
    Q_OBJECT
public:
    explicit PTest(QObject *parent = 0);

    bool Init(PEncoder *encUgol, PEncoder *encAzim, PDalnomer *dalnomer, PBMSD *bmsd1, PBMSD *bmsd2,
              PBMSD *bmsd3, PBMSD *bmsd4, PBMSD *bmsd5, PGSM40 *gsm40, PPotenciometr *potenciometr1,
              PPotenciometr *potenciometr2, PStaticDIO *staticDIO, PJoystick *joystick,
              QString adressCam1, QString adressCam2, QString adressCam3, QString adressCam4, QString adressVizir,
              QString adressTeplovizor, QString adressPCVideo);

    //Признаки исправности
    bool APU, LVS, Spoil, OES;
    //Слова-состояний исправности по каждой группе
    PStatus statusAPU, statusLVS, statusSpoil, statusOES;

    //Функции проверки исправности
    bool testAPU();
    bool testLVS();
    bool testSpoil();
    bool testOES();

    //Объект для работы с энкодером угла\места
    PEncoder *encUgol;
    //Объект для работы с энкодером азимута
    PEncoder *encAzim;
    //Объект для работы с дальномером
    PDalnomer *dalnomer;
    //Объекты для работы с БМСД
    PBMSD *bmsd1, *bmsd2, *bmsd3, *bmsd4, *bmsd5;
    //Объект для работы с точным актуатором GSM40
    PGSM40 *gsm40;
    //Объект для работы с потенциометрами
    PPotenciometr *potenciometr1, *potenciometr2;
    //Объект для работы с платой дискретных в\в
    PStaticDIO *staticDIO;
    //Объект для работы с джойстиком
    PJoystick *joystick;

    //сетевые адреса до обзорных камер, визира, тепловизора и компьютера видеобработки
    QString adressCam1, adressCam2, adressCam3, adressCam4,
            adressVizir, adressTeplovizor, adressPCVideo;

    //признаки возможности получения изображения с камер - получаем по сети от программы видеобработки
    bool statusCam1, statusCam2, statusCam3, statusCam4, statusVizir, statusTeplovizor;

//    //признак ошибки
//    bool error;

private:
    bool ping(QString adress);

signals:
    //сигнал для отправки команды по сети
    void sendCommand(int command, int Argument1, int Argument2);
    //Сигнал обновления интерфейса программы в части контроля системы
    void updateStatus(bool apu, bool lvs, bool spoil, bool oes);

public slots:
    //Функция проверки исправности работы системы
    void testSystem();
    //Функция получения информации об исправности видеоустройств
    void statusVideo(bool cam1, bool cam2, bool cam3, bool cam4, bool vizir, bool teplovizor);
};

#endif // PTEST_H
