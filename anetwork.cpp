#include "anetwork.h"

ANetwork::ANetwork(QObject *parent) :
    QObject(parent)
{    
    //Сокет для приема и отправки данных
    tcpServerSocket = new QTcpSocket();
    //Подключаем сигнал о новом подключении к функции его обработки
    connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));

    //инициализируем сервер на прослушивание определенного порта с любого адреса
    tcpServer.listen(QHostAddress::Any, DEFAULT_PORT);
}

ANetwork::~ANetwork()
{
    //закрываем соедиения
    tcpServerSocket->close();
    tcpServer.close();
}

void ANetwork::newConnection()
{
    //инициализируем сокет новым подключением
    tcpServerSocket = tcpServer.nextPendingConnection();
    //подключаем сигнал о новых данных к функции их чтения и обработки
    connect(tcpServerSocket,SIGNAL(readyRead()),this, SLOT(slotRead()));
}

//Функция чтения и обработки новых поступивших данных
void ANetwork::slotRead()
{
    DATA command;
    memset(&command, 0, sizeof(DATA));
//    //создаем новый сокет
//    QTcpSocket* socket = (QTcpSocket*)sender();

    //читаем все поступившие данные
    QByteArray dataInput = tcpServerSocket->readAll();
    //парсим данные
    parser.parse_message(dataInput, &command);

    //выдаем новые данные в основное окно для их отображения
    emit newInputData(&command);
}

//Функция отправки команды
void ANetwork::sendAnswer(const DATA *answer)
{
    //временный байтовый буфер
    QByteArray temp;
    temp.resize(4);
    parser.form_command(answer,temp,false,false);

    //отправляем данные по сети
    tcpServerSocket->write(temp);
}
