#include "network.h"

Network::Network(QObject *parent) :
    QObject(parent)
{
    Sender = 0;
    Command = 0;
    Argument1 = 0;
    Argument2 = 0;

    //признак инициализации соединения
    init = false;

    //признак инициализации соединения
    connected = NOT_CONNECTED;

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(reconnect()));
    timer->setInterval(1000);
}

Network::~Network()
{
    clientSocket.close();
}

void Network::parseInputData(QByteArray inputData)
{
    if (inputData.size() < 4)
    {
        return;
    }

    parseCommand(inputData);

    if (Sender == idVideo)
    {
        if(Command == CP_STATUS)
        {
            emit statusVideo(((Argument2 & 1) ? true : false),
                             (((Argument2 >> 1) & 1) ? true : false),
                             (((Argument2 >> 2) & 1) ? true : false),
                             (((Argument2 >> 3) & 1) ? true : false),
                             (((Argument2 >> 4) & 1) ? true : false),
                             (((Argument2 >> 5) & 1) ? true : false));

        }
        if(Command == CP_COMMAND_AC)
        {
            emit posTargetAC(Argument1, Argument2);
        }
    }
    else if (Sender == idAbonent)
    {

    }

    //если по сети пришло более одной команды, то обрабатываем остальные команды
    if (inputData.size() > 4)
    {
        //вызываем функцию обработки данных, указывая на следующий пакет данных
        parseInputData(*(&inputData + 4));
    }
}

void Network::reconnect()
{
    if (!init)
    {
        if (timer->isActive())
            timer->stop();
        return;
    }

    if(clientSocket.state() == QTcpSocket::UnconnectedState)
    {
        clientSocket.connectToHost(QHostAddress(adress), port);
        if(clientSocket.waitForConnected(100))
        {
            timer->stop();
            connected = CONNECTED;
        }
    }
}

//Функция инициализации сетевого подключения (задается идентификатор, адрес, порт)
void Network::Init(QString adress, int port, int abonentID, int videoID)
{
    idVideo = videoID;
    idAbonent = abonentID;
    if (idVideo != 0)
        id = idVideo;
    if (idAbonent != 0)
        id = idAbonent;

    this->adress = adress;
    this->port = port;

    init = true;

    connectSock();
}

//Распаковка пакета данных на составляющие
void Network::parseCommand(QByteArray InputBuffer)
{
    // Обнуление служебных переменных
    Sender      = 0;
    Command     = 0;
    Argument1   = 0;
    Argument2   = 0;

    char *temp = InputBuffer.data();

    // Распаковка пакета данных
    Sender      = (*(temp)) & 255;//InputBuffer[0].toInt();;//(*(InputBuffer))&255;		// Код отправителя
    Command     = (*(temp + 1)) & 255;//InputBuffer[1].toInt();//(*(InputBuffer+1))&255;		// Код команды
    Argument1   = (*(temp + 2)) & 255;//InputBuffer[2].toInt();//(*(InputBuffer+2))&255;		// Первый аргумент
    Argument2   = (*(temp + 3)) & 255;//InputBuffer[3].toInt();//(*(InputBuffer+3))&255;		// Второй аргумент
}

void Network::connectSock()
{
    if (!init)
    {
        if (timer->isActive())
            timer->stop();
        return;
    }

    connected = NOT_CONNECTED;
    //подключаем сигналы к обработчикам
    connect(this,SIGNAL(send(int,int,int)),this, SLOT(sendCommand(int,int,int)));
    //Подключаемся к серверу. Здесь нужно будет указать адресс сервера на другой машине и порт
    connect(&clientSocket,SIGNAL(readyRead()),this, SLOT(slotRead()));
    clientSocket.connectToHost(QHostAddress(adress), port);

    if(clientSocket.state()== QTcpSocket::UnconnectedState)
    {
        if(clientSocket.waitForConnected(100))
        {
            connected = CONNECTED;
            return;
        }
        if (!timer->isActive())
            timer->start();
    }
}

//void Network::readNetSettings()
//{
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
//        }
//}

void Network::prepareCommand(char *buff, int command, int arg1 = 0, int arg2 = 0, int num = -1)
{
    *(buff) = 0;
    *(buff  +1) = 0;
    *(buff + 2) = 0;
    *(buff + 3) = 0;

    // Упаковка команды в байт
    if (num == -1)
        *(buff) = *(buff) | id;
    else
        *(buff) = *(buff) | num;

    *(buff + 1) = (*(buff+1) | command) & 0xff;
    *(buff + 2) = (*(buff+2) | arg1) & 0xff;
    *(buff + 3) = (*(buff+3) | arg2) & 0xff;
}

void Network::slotRead()
{
    //открываем сокет для приема данных
    QTcpSocket* socket = (QTcpSocket*)sender();
    QByteArray dataInput = socket->readAll();

    parseInputData(dataInput);
}

//Функция отправки команды
void Network::sendCommand(int command, int arg1, int arg2)
{
    if (connected == NOT_CONNECTED)
        return;

    char temp[4];
    prepareCommand(temp, command, arg1, arg2);

    clientSocket.write(temp, 4);
}

//Функция отправки байтов
void Network::sendData(int byte0, int byte1, int byte2, int byte3)
{
    if (connected == NOT_CONNECTED)
        return;

    char temp[4];
    prepareCommand(temp, byte1, byte2, byte3, byte0);

    clientSocket.write(temp, 4);
}
