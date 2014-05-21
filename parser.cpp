#include "parser.h"

Parser::Parser(QObject *parent) :
    QObject(parent)
{
}
Parser::~Parser()
{

}

void Parser::form_request_to_serv(QByteArray &data_out)
{
    memset(&dat,0,sizeof(DATA));
    state_byte=0x00;
    dat.sender=CLIENT_SENDER;
    dat.command=COMMAND_CLIENT_REQUEST;
    dat.state.mode=0;
    dat.state.gk=0;
    dat.state.highest=0;
    calculate_crc();
    state_byte=(dat.state.mode<<2)|(dat.state.highest<<1)|dat.state.gk;
    data_out[0]=dat.sender;
    data_out[1]=dat.command;
    data_out[2]=state_byte;
    data_out[3]=dat.CRC;
    qWarning("  ");
}
void Parser::parse_message(const QByteArray &data_in, DATA *message)
{
    memset(&dat,0,sizeof(DATA));
    dat.sender=data_in[0];
    dat.command=data_in[1];
    dat.state.mode=data_in[2]>>2;
    dat.state.gk=data_in[2]&0x01;
    dat.state.highest=(data_in[2]&0x03)>>1;
    dat.CRC=data_in[3];
    memcpy(message,&dat,sizeof(DATA));
}
void Parser::form_command(const DATA *command, QByteArray &data_out, bool tosrv, bool highest_on_off)
{
     memset(&dat,0,sizeof(DATA));
     state_byte=0x00;
     memcpy(&dat,command,sizeof(DATA));
     if (tosrv){
        dat.sender=CLIENT_SENDER;
        if ( highest_on_off ) dat.command=COMMAND_CLIENT_HIGHEST_ON_OFF;
            else dat.command=COMMAND_CLIENT_SETMODE;
     }
        else{
         dat.sender=SERVER_SENDER;
         dat.command=COMMAND_SERVER_ANSWER;
     }
     calculate_crc();
     state_byte=(dat.state.mode<<2)|(dat.state.highest<<1)|dat.state.gk;
     data_out[0]=dat.sender;
     data_out[1]=dat.command;
     data_out[2]=state_byte;
     data_out[3]=dat.CRC;
     qWarning("  ");

}
void Parser::calculate_crc()
{
    dat.CRC=dat.sender^dat.command^state_byte;
}
