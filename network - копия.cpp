#include "network.h"

Network::Network(QObject *parent) :
    QObject(parent)
{
//    log.add(LOG_INIT_NETWORK_BEGIN);

    Sender = 0;
    Command = 0;
    Argument1 = 0;
    Argument2 = 0;

    ID = DEFAULT_ID;

    //чтение файла настроек сети
//    readNetSettings();

//    log.add(LOG_NET_VIDEO_ADRESS + adress);

    tcpServer = new QTcpServer(this);

//    qDebug() << "Connect success " << tcpServer->serverPort();
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    clientSocket = new QTcpSocket(this);

    if(!tcpServer->listen(QHostAddress("127.0.0.1"), 26));
    {
//        qDebug() << ERROR_LISTEN_NET_PORT << port;
//        log.add(ERROR_LISTEN_NET_PORT ,port);
    }
//    log.add(LOG_INIT_NETWORK_END);
}

Network::~Network()
{
//    log.add(LOG_CLOSE_SERVERS);
    tcpServer->close();
    clientSocket->close();
}

void Network::newConnection()
{
//    log.add(LOG_NEW_CONNECT);
    clientSocket = tcpServer->nextPendingConnection();
    qDebug() << "New connection ";
    connect(clientSocket,SIGNAL(readyRead()),this, SLOT(slotRead()));
    connect(this,SIGNAL(send(int,int,int)),this, SLOT(sendCommand(int,int,int)));
//    tcpServer.close();
}

void Network::parseInputData(QByteArray *inputData)
{
    if (inputData->size() < 4)
    {
//        log.add(LOG_LESS_4_BYTES);
        return;
    }

    parseCommand(inputData);

//    qDebug()<< "INPUTDATA:" << Sender << Command << Argument1 << Argument2;

    // Команды от приборной панели
    if(Sender == idPanel)
    {
//        // Нажатия кнопок
//        if(Command == VP_COMMAND_INPUT_CONTROL_PANEL_BUTTON_PRESS)
//        {
//            switch(Argument2)
//            {
//            case VP_SWITCH_MODE_MULTI:  // Переключиться в режим "Мультиэкран"
//                video->State = MULTI;
//                log.add(LOG_REGIM_MULTI);
//                video->camState = CAM_STATE_MANUAL;
//                break;
//            case VP_SWITCH_MODE_VIZIR:  // Переключиться в режим "Визир"
//                video->State = VIZIR;
//                log.add(LOG_REGIM_VIZIR);
//                video->camState = CAM_STATE_MANUAL;
//                break;
//            case VP_SWITCH_MODE_CAMERA: // Переключиться в режим "Камера"
//                video->State = CAMERA;
//                log.add(LOG_REGIM_CAMERA);
//                video->camState = CAM_STATE_MANUAL;
//                break;
//            case VP_SWITCH_MODE_PAN:    // Переключиться в режим "Обзор"
//                video->State = OBZOR;
//                log.add(LOG_REGIM_OBZOR);
//                video->camState = CAM_STATE_MANUAL;
//                break;
//            ///////////////////////////////////////////////////////////////////////
//            case VP_SELECT_CAMERA_1:    // Выбор камеры №1
//                video->numCam = 1;
//                log.add(LOG_SWITCH_CAM1);
//                video->camState = CAM_STATE_MANUAL;
//                break;
//            case VP_SELECT_CAMERA_2:    // Выбор камеры №2
//                video->numCam = 2;
//                log.add(LOG_SWITCH_CAM2);
//                video->camState = CAM_STATE_MANUAL;
//                break;
//            case VP_SELECT_CAMERA_3:    // Выбор камеры №3
//                video->numCam = 3;
//                log.add(LOG_SWITCH_CAM3);
//                video->camState = CAM_STATE_MANUAL;
//                break;
//            case VP_SELECT_CAMERA_4:    // Выбор камеры №4
//                video->numCam = 4;
//                log.add(LOG_SWITCH_CAM4);
//                video->camState = CAM_STATE_MANUAL;
//                break;
//            ///////////////////////////////////////////////////////////////////////
//            case VP_SELECT_CAMERA_AUTO:		// Выбор камеры в автоматическом режиме
//                video->numCam = 1;
//                log.add(LOG_REGIM_CAMERA_AUTO);
//                video->camState = CAM_STATE_AUTO;
//                break;
//            case VP_SELECT_CAMERA_SEQ:		// Выбор камеры в секвентальном режиме
//                video->timerCamSeq->start();
//                log.add(LOG_REGIM_CAMERA_SEKV);
//                video->camState = CAM_STATE_SEQ;
//                break;
//            case VP_CAMERA_ZOOM_IN:			// Увеличить зум камеры
//                log.add(LOG_ZOOM_PLUS);
//                if (video->Zoom < ZOOM_MAX)
//                    video->Zoom +=ZOOM_SHAG;
//                else
//                    video->Zoom = ZOOM_MAX;
//                video->camState = CAM_STATE_MANUAL;
//                break;
//            case VP_CAMERA_ZOOM_OUT:		// Уменьшить зум камеры
//                log.add(LOG_ZOOM_MINUS);
//                if (video->Zoom > ZOOM_MIN)
//                    video->Zoom -=ZOOM_SHAG;
//                else
//                    video->Zoom = ZOOM_MIN;
//                video->camState = CAM_STATE_MANUAL;
//                break;
//            };
//        }
    }

    // Команды от программы управления приводами
    if(Sender == idDrive)
    {
//        // Курсор - ось Y
//        if(Command == VP_COMMAND_INPUT_CURSOR_Y)
//        {
//            log.add(LOG_DRIVE_CURSOR_Y);
//            video->joyMarkerY = Argument2;
//            video->joyMarkerY = video->joyMarkerY/60;
//            video->joyMarkerY += Argument1;
//        }
//        // Курсор - ось X
//        if(Command == VP_COMMAND_INPUT_CURSOR_X)
//        {
//            log.add(LOG_DRIVE_CURSOR_X);
//            video->joyMarkerX = Argument2;
//            video->joyMarkerX = video->joyMarkerX/60;
//            video->joyMarkerX += Argument1;
//        }
//        // Визир - ось Y
//        if(Command == VP_COMMAND_INPUT_VIZIR_Y)
//        {
//            log.add(LOG_DRIVE_VIZIR_Y);
//            video->antennaMarkerY = Argument2;
//            video->antennaMarkerY = video->antennaMarkerY/60;
//            video->antennaMarkerY += Argument1;
//        }
//        // Визир - ось X
//        if(Command == VP_COMMAND_INPUT_VIZIR_X)
//        {
//            log.add(LOG_DRIVE_VIZIR_X);
//            video->antennaMarkerX = Argument2;
//            video->antennaMarkerX = video->antennaMarkerX/60;
//            video->antennaMarkerX += Argument1;
//        }
    }

    //если по сети пришло более одной команды, то обрабатываем остальные команды
    if (inputData->size() > 4)
    {
        //выводит отладочное сообщение
//        qDebug() << "PARSE: more 4 bytes" ;

        //QByteArray *inputDataOstatok = (inputData + 4);
        //вызываем функцию обработки данных, указывая на следующий пакет данных
        parseInputData(inputData + 4);
    }
}

//Распаковка пакета данных на составляющие
void Network::parseCommand(QByteArray *InputBuffer)
{
    // Обнуление служебных переменных
    Sender      = 0;
    Command     = 0;
    Argument1   = 0;
    Argument2   = 0;

    char *temp = InputBuffer->data();

    // Распаковка пакета данных
    Sender      = (*(temp)) & 255;//InputBuffer[0].toInt();;//(*(InputBuffer))&255;		// Код отправителя
    Command     = (*(temp + 1)) & 255;//InputBuffer[1].toInt();//(*(InputBuffer+1))&255;		// Код команды
    Argument1   = (*(temp + 2)) & 255;//InputBuffer[2].toInt();//(*(InputBuffer+2))&255;		// Первый аргумент
    Argument2   = (*(temp + 3)) & 255;//InputBuffer[3].toInt();//(*(InputBuffer+3))&255;		// Второй аргумент
}

void Network::readNetSettings()
{
//    bool allOk = true;
//    QSettings *netSettings = new QSettings(fileNetSettings,QSettings::IniFormat);

//    //    netSettings->beginGroup(NET_SETTINGS_DEVICES);
//    idPanel = netSettings->value(NET_SETTINGS_PANEL_ID, DEFAULT_ID).toInt();
//    if ((idPanel < 0) || (idPanel > MAX_ID))
//        allOk = false;
//    idDrive = netSettings->value(NET_SETTINGS_DRIVE_ID, DEFAULT_ID).toInt();
//    if ((idDrive < 0) || (idDrive > MAX_ID))
//        allOk = false;

//    idVideo = netSettings->value(NET_SETTINGS_VIDEO_ID, DEFAULT_ID).toInt();
//    if ((idVideo < 0) || (idVideo > MAX_ID))
//        allOk = false;

//    adress  = netSettings->value(NET_SETTINGS_VIDEO_ADRESS, DEFAULT_ADRESS).toString();
//    qDebug() << "Network: adress=" << adress;
//    port    = netSettings->value(NET_SETTINGS_VIDEO_PORT, DEFAULT_PORT).toInt();
//    qDebug() << "Network: port=" << port;
//    if ((port < 0) || (port > MAX_PORT))
//        allOk = false;
////    netSettings->endGroup();

//    if (!allOk)
//        log.add(ERROR_NET_FILE_DATA);

//    if (netSettings->status() != QSettings::NoError)
//    {
//        log.add(ERROR_NET_FILE_OPEN);
//        qDebug() << ERROR_NET_FILE_OPEN;
//    }
//    else
//    {
//        log.add(SUCCESS_NET_FILE_OPEN);
//        qDebug() << SUCCESS_NET_FILE_OPEN;
    //    }
}

void Network::prepareCommand(char *buff, int command, int arg1, int arg2)
{
    *(buff) = 0;
    *(buff  +1) = 0;
    *(buff + 2) = 0;
    *(buff + 3) = 0;

    // Упаковка команды в байт
    *(buff) = *(buff) | ID;
    *(buff + 1) = (*(buff+1) | command) & 0xff;
    *(buff + 2) = (*(buff+2) | arg1) & 0xff;
    *(buff + 3) = (*(buff+3) | arg2) & 0xff;
}

void Network::slotRead()
{
    QByteArray *dataInput = new QByteArray;
    *dataInput = clientSocket->readAll();

    int idSocket=clientSocket->socketDescriptor();
    //преобразование QByteArray в чар
//    qDebug()<<"From"<<idSocket<<":  ";
//    char *inputData = dataInput.data();
    parseInputData(dataInput);
    clientSocket->close();
}

//Функция отправки команды
void Network::sendCommand(int command, int arg1, int arg2)
{
    char temp[4];
    prepareCommand(temp, command, arg1, arg2);

//    qDebug() << "SEND: " << temp[0];

    clientSocket->write(temp, 4);
}
