#include "ptest.h"

PTest::PTest(QObject *parent) :
    QObject(parent)
{
    APU = false;
    LVS = false;
    Spoil = false;
    OES = false;

    statusCam1 = false;
    statusCam2 = false;
    statusCam3 = false;
    statusCam4 = false;
    statusVizir = false;
    statusTeplovizor = false;

}

bool PTest::Init(PEncoder *encUgol, PEncoder *encAzim, PDalnomer *dalnomer, PBMSD *bmsd1, PBMSD *bmsd2,
                 PBMSD *bmsd3, PBMSD *bmsd4, PBMSD *bmsd5, PGSM40 *gsm40, PPotenciometr *potenciometr1,
                 PPotenciometr *potenciometr2, PStaticDIO *staticDIO, PJoystick *joystick,
                 QString adressCam1, QString adressCam2, QString adressCam3, QString adressCam4, QString adressVizir,
                 QString adressTeplovizor, QString adressPCVideo)
{
    this->encUgol = encUgol;
    this->encAzim = encAzim;
    this->dalnomer = dalnomer;
    this->bmsd1 = bmsd1;
    this->bmsd2 = bmsd2;
    this->bmsd3 = bmsd3;
    this->bmsd4 = bmsd4;
    this->bmsd5 = bmsd5;
    this->staticDIO = staticDIO;
    this->gsm40 = gsm40;
    this->potenciometr1 = potenciometr1;
    this->potenciometr2 = potenciometr2;
    this->joystick = joystick;
    this->adressCam1 = adressCam1;
    this->adressCam2 = adressCam2;
    this->adressCam3 = adressCam3;
    this->adressCam4 = adressCam4;
    this->adressVizir = adressVizir;
    this->adressTeplovizor = adressTeplovizor;
    this->adressPCVideo = adressPCVideo;

    return true;
}

//Функции проверки исправности АПУ
bool PTest::testAPU()
{
    //обнуляем слово состояния исправности группы
    statusAPU.value = 0;
    //тестирование Двигателя азимута
    if (bmsd2->getStatus())
        statusAPU.bits.b00 = 1;
    else
        statusAPU.bits.b00 = 0;

    //тестирование Актуатора GSM40
    if (gsm40->getStatus())
        statusAPU.bits.b01 = 1;
    else
        statusAPU.bits.b01 = 0;

    //тестирование Актуатора ALI4-F
    if (bmsd1->getStatus())
        statusAPU.bits.b02 = 1;
    else
        statusAPU.bits.b02 = 0;

    //тестирование Актуатора фокуса и его потенциометра
    if (bmsd4->getStatus() && potenciometr1->getStatus())
        statusAPU.bits.b03 = 1;
    else
        statusAPU.bits.b03 = 0;

    //тестирование Актуатора замков и его потенциометра
    if (bmsd5->getStatus() && potenciometr2->getStatus())
        statusAPU.bits.b04 = 1;
    else
        statusAPU.bits.b04 = 0;

    //тестирование энкодера угла места
    if (encUgol->getStatus())
        statusAPU.bits.b05 = 1;
    else
        statusAPU.bits.b05 = 0;

    //тестирование энкодера азимута
    if (encAzim->getStatus())
        statusAPU.bits.b06 = 1;
    else
        statusAPU.bits.b06 = 0;

    //тестирование Левого конц. выключателя
    //проверяем состояния: если НЗ не равен НО, то исправен
    if (staticDIO->getInState(IN_KONC_LEFT_NZ) != staticDIO->getInState(IN_KONC_LEFT_NO))
        statusAPU.bits.b07 = 1;
    else
        statusAPU.bits.b07 = 0;

    //тестирование Правого конц. выключателя
    //проверяем состояния: если НЗ не равен НО, то исправен
    if (staticDIO->getInState(IN_KONC_LEFT_NZ) != staticDIO->getInState(IN_KONC_LEFT_NO))
        statusAPU.bits.b08 = 1;
    else
        statusAPU.bits.b08 = 0;

    //тестирование Концевика выкл. ЗТУ
    //проверяем состояния: если НЗ не равен НО, то исправен
    if (staticDIO->getInState(IN_KONC_ZTU_NZ) != staticDIO->getInState(IN_KONC_ZTU_NO))
        statusAPU.bits.b09 = 1;
    else
        statusAPU.bits.b09 = 0;

    //если выставлены все биты исправности, то группа исправна, в противном случае не исправна
    if (statusAPU.value == 0x3FF)
        APU = true;
    else
        APU = false;

    //возращаем состояние исправности группы
    return APU;
}

//Функции проверки исправности ЛВС
bool PTest::testLVS()
{
    //обнуляем слово состояния исправности группы
    statusLVS.value = 0;
    //тестирование линий связи
    //  с камерой 1
    if (ping(adressCam1))
        statusLVS.bits.b00 = 1;
    else
        statusLVS.bits.b00 = 0;
    //  с камерой 2
    if (ping(adressCam2))
        statusLVS.bits.b01 = 1;
    else
        statusLVS.bits.b01 = 0;
    //  с камерой 3
    if (ping(adressCam3))
        statusLVS.bits.b02 = 1;
    else
        statusLVS.bits.b02 = 0;
    //  с камерой 4
    if (ping(adressCam4))
        statusLVS.bits.b03 = 1;
    else
        statusLVS.bits.b03 = 0;
    //  с визирной камерой
    if (ping(adressVizir))
        statusLVS.bits.b04 = 1;
    else
        statusLVS.bits.b04 = 0;
    //  с тепловизором
    if (ping(adressTeplovizor))
        statusLVS.bits.b05 = 1;
    else
        statusLVS.bits.b05 = 0;
    //  с компьютером видеообработки
    if (ping(adressPCVideo))
        statusLVS.bits.b06 = 1;
    else
        statusLVS.bits.b06 = 0;

    //если выставлены все биты исправности, то группа исправна, в противном случае не исправна
    if (statusLVS.value == 0x7F)
        LVS = true;
    else
        LVS = false;

    //возращаем состояние исправности группы
    return LVS;
}

//Функции проверки исправности спойлера
bool PTest::testSpoil()
{
    //обнуляем слово состояния исправности группы
    statusSpoil.value = 0;
    //тестирование Двигателя спойлера
    if (bmsd3->getStatus())
        statusSpoil.bits.b00 = 1;
    else
        statusSpoil.bits.b00 = 0;

    //тестирование Передний конц. выкл.
    if (staticDIO->getInState(IN_KONC_SPOIL_PER_NZ) != staticDIO->getInState(IN_KONC_SPOIL_PER_NO))
        statusSpoil.bits.b01 = 1;
    else
        statusSpoil.bits.b01 = 0;

    //тестирование Задний конц. выкл.
    if (staticDIO->getInState(IN_KONC_SPOIL_ZAD_NZ) != staticDIO->getInState(IN_KONC_SPOIL_ZAD_NO))
        statusSpoil.bits.b02 = 1;
    else
        statusSpoil.bits.b02 = 0;

    //тестирование геркона
    if (staticDIO->getInState(IN_GERKON_NZ) != staticDIO->getInState(IN_GERKON_NO))
        statusSpoil.bits.b03 = 1;
    else
        statusSpoil.bits.b03 = 0;

    //если выставлены все биты исправности, то группа исправна, в противном случае не исправна
    if (statusSpoil.value == 0xF)
        Spoil = true;
    else
        Spoil = false;

    //возращаем состояние исправности группы
    return Spoil;
}

//Функции проверки исправности
bool PTest::testOES()
{
    //обнуляем слово состояния исправности группы
    statusOES.value = 0;
    //тестирование камер:
    //  камера 1
    if (statusCam1)
        statusOES.bits.b00 = 1;
    else
        statusOES.bits.b00 = 0;
    //  камера 2
    if (statusCam2)
        statusOES.bits.b01 = 1;
    else
        statusOES.bits.b01 = 0;
    //  камера 3
    if (statusCam3)
        statusOES.bits.b02 = 1;
    else
        statusOES.bits.b02 = 0;
    //  камера 4
    if (statusCam4)
        statusOES.bits.b03 = 1;
    else
        statusOES.bits.b03 = 0;
    //  визирной камеры
    if (statusVizir)
        statusOES.bits.b04 = 1;
    else
        statusOES.bits.b04 = 0;
    //  тепловизора
    if (statusTeplovizor)
        statusOES.bits.b05 = 1;
    else
        statusOES.bits.b05 = 0;
    //тестирование дальномера
    if (dalnomer->getStatus())
        statusOES.bits.b06 = 1;
    else
        statusOES.bits.b06 = 0;
    //тестирование пульта (точнее платы дискретных сигналов)
    if (staticDIO->getStatus())
        statusOES.bits.b07 = 1;
    else
        statusOES.bits.b07 = 0;
    //тестирование джойстика
    if (joystick->getStatus())
        statusOES.bits.b08 = 1;
    else
        statusOES.bits.b08 = 0;


    //если выставлены все биты исправности, то группа исправна, в противном случае не исправна
    if (statusOES.value == 0x1FF)
        OES = true;
    else
        OES = false;

    //возращаем состояние исправности группы
    return OES;
}

//функция проверки доступности узла ЛВС
bool PTest::ping(QString adress)
{
    //если адрес пуст
    if(adress.isEmpty())
        //выходим из функции
        return false;

    //результат проверки по умолчанию отрицательный
    bool boolRESULT=false;

    //время ожидания
    int ping_timeout=500;
    //путь к утилите ping
    QString exe_path = "C:\\Windows\\System32\\ping.exe";

    //если утилита недоступна
    if(!QFile::exists(exe_path))
        return false;

    //аргументы для передачи в команду ping
    QStringList arguments;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("cp-866"));
    //в список аргументов заносим адреса узла для проверки
    arguments << adress;

    //запускаем процесс
    QProcess myProcess;
    //запускаем процесс ping
    myProcess.start(exe_path, arguments);
    //ожидаем окончания выполнения
    myProcess.waitForFinished(ping_timeout);

    //переменная для сохранения вывода утилиты ping
    QStringList strlstPingResults;
    //получаем вывод процесса
    strlstPingResults << myProcess.readAll();
    //удаляем процесс
    myProcess.deleteLater();

    //анализируем вывод утилиты ping
    //проходим по всем строкам вывода
    for (int i=0; i <= strlstPingResults.count() - 1; i++)
    {
        //анализируем строку и запоминаем результат
        boolRESULT = strlstPingResults[i].contains("ttl", Qt::CaseInsensitive);
        //если достучались до узла
        if (boolRESULT)
            //завершаем процесс
            break;
    }

    //возвращаем результат
    return boolRESULT;
}

//Слот проверки исправности работы системы
void PTest::testSystem()
{
    testAPU();
    testLVS();
    testOES();
    testSpoil();

    if (APU || LVS || OES || Spoil)
        emit updateStatus(APU, LVS, Spoil, OES);

    if (!APU)
        emit sendCommand(CP_STATUS_APU, statusAPU.bytes.byte1, statusAPU.bytes.byte2);

    if (!LVS)
        emit sendCommand(CP_STATUS_LVS, statusLVS.bytes.byte1, statusLVS.bytes.byte2);

    if (!Spoil)
        emit sendCommand(CP_STATUS_SPOIL, statusSpoil.bytes.byte1, statusSpoil.bytes.byte2);

    if (!OES)
        emit sendCommand(CP_STATUS_OES, statusOES.bytes.byte1, statusOES.bytes.byte2);
}

//Функция получения информации об исправности видеоустройств
void PTest::statusVideo(bool cam1, bool cam2, bool cam3, bool cam4, bool vizir, bool teplovizor)
{
    statusCam1 = cam1;
    statusCam2 = cam2;
    statusCam3 = cam3;
    statusCam4 = cam4;
    statusVizir = vizir;
    statusTeplovizor = teplovizor;
}


