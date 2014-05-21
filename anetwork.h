/*
***************КАК ИСПОЛЬЗУЕТСЯ ФОРМИРОВАНИЕ КОМАНДЫ******************
Информационный пакет состоит из 4-х байт.
Первый байт - код отправителя из файла H_DEVICE_CODES.h (для пульта - #define DEVICE_PANEL     0x04)
Второй байт - код команды.
Третий и четверты байты - если не требуется специальное разделение аргумента (для пульта - не требуется),
то третий - нулевой, а четвертый - аргумент.
*/

#ifndef ANETWORK_H
#define ANETWORK_H

#include <QObject>

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include "parser.h"

#define DEFAULT_PORT    90

class ANetwork : public QObject
{
    Q_OBJECT
public:
    explicit ANetwork(QObject *parent = 0);
    ~ANetwork();

    QTcpServer tcpServer;
    //Сокет для отправки данных
    QTcpSocket *tcpServerSocket;
    //Функция отправки ответа
    void sendAnswer(const DATA *answer);

public slots:
    //Функция чтения данных поступивших по сети
    void slotRead();
    //Функция вызываемая при поступлении нового подключения
    void newConnection();

signals:
    //Сигнал, оповещающий основную программу, что пришли новые данные
    void newInputData(DATA *data);

public slots:

private:
    Parser parser;

};

#endif // NETWORK_H
