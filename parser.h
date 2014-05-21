#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#define CLIENT_SENDER 20
#define SERVER_SENDER 21
#define COMMAND_CLIENT_REQUEST 60
#define COMMAND_SERVER_ANSWER 60
#define COMMAND_CLIENT_HIGHEST_ON_OFF 61
#define COMMAND_CLIENT_SETMODE 62

typedef struct{
    char mode;
    char gk;
    char highest;
} STATE;

typedef struct{
    char sender;
    char command;
    STATE state;
    char CRC;
} DATA;



class Parser : public QObject
{
    Q_OBJECT
public:
    explicit Parser(QObject *parent = 0);
    ~Parser();


    void form_request_to_serv(QByteArray &data_out);
    void parse_message(const QByteArray &data_in, DATA *message);
    void form_command(const DATA *command, QByteArray &data_out, bool tosrv, bool highest_on_off);
    void calculate_crc();


signals:

public slots:

private:
    DATA dat;
    char state_byte;

};

#endif // PARSER_H
