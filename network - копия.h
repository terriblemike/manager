/*
***************КАК ИСПОЛЬЗУЕТСЯ ФОРМИРОВАНИЕ КОМАНДЫ******************
Информационный пакет состоит из 4-х байт.
Первый байт - код отправителя из файла H_DEVICE_CODES.h (для пульта - #define DEVICE_PANEL     0x04)
Второй байт - код команды.
Третий и четверты байты - если не требуется специальное разделение аргумента (для пульта - не требуется),
то третий - нулевой, а четвертый - аргумент.
*/

#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>

//#include <QtNetwork/
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#ifdef DEBUG
#include <QDebug>
#endif

//#include "logs.h"
#include "netDefine.h"

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);
    ~Network();

//    Logs log;

    //Сервер
    QTcpServer *tcpServer;
    //Сокет сервера
    QTcpSocket *clientSocket;
    //Функция анализа данных, полученных по сети
    void parseInputData(QByteArray *inputData);

    //информационные составляющие пакета данных
    int Sender, Command, Argument1, Argument2;

    int idPanel;
    int idDrive;
    int idVideo;

    int ID;


    QString adress;
    int port;

    void readNetSettings();

private:
    //Функция формирует из данных комманду для отправки
    void prepareCommand(char *buff, int command, int arg1 = 0, int arg2 = 0);

    //Распаковка команды на состявляющие
    void parseCommand(QByteArray *InputBuffer);

private slots:
    void newConnection();
    void slotRead();
    //Функция отправки команды
    void sendCommand(int command, int arg1, int arg2);

signals:
    void send(int command, int arg1, int arg2);

public slots:

};

#endif // NETWORK_H
