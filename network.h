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

#define NOT_CONNECTED   false
#define CONNECTED       true


#include <QObject>

//#include <QtNetwork/
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QTimer>
#include <QString>
#include "CP_Button_Definitions.h"

#ifdef DEBUG
#include <QDebug>
#endif

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);
    ~Network();

    //Сокет сервера
    QTcpSocket clientSocket;
    //Функция анализа данных, полученных по сети
    void parseInputData(QByteArray inputData);

    //информационные составляющие пакета данных
    int Sender, Command, Argument1, Argument2;

    int idVideo, idAbonent, id;
    QString adress;
    int port;

    void Init(QString adress, int port, int abonentID, int videoID);

private:
    //Функция формирует из данных комманду для отправки
    void prepareCommand(char *buff, int command, int arg1, int arg2, int num);

    //Распаковка команды на состявляющие
    void parseCommand(QByteArray InputBuffer);

    //таймер для подключения
    QTimer *timer;

    //признак инициализации соединения
    bool init;

    //признак инициализации соединения
    bool connected;

    //Функция подключения
    void connectSock();

private slots:
    void slotRead();
    //Функция отправки команды
    void sendCommand(int command, int arg1, int arg2);
    //Функция отправки байтов
    void sendData(int byte0, int byte1, int byte2, int byte3);

    //функция переподключения
    void reconnect();

signals:
    void send(int command, int arg1, int arg2);
    void statusVideo(bool cam1, bool cam2, bool cam3, bool cam4, bool vizir, bool teplovizor);
    void posTargetAC(int x, int y);

public slots:

};

#endif // NETWORK_H
