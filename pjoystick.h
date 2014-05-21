#ifndef PJOYSTICK_H
#define PJOYSTICK_H

#include <QObject>
#include <QString>
#include <QList>
#include <QTimer>

#include "SDL/SDL.h"

#define QPS_JOYSTICK    20

#define JOYSTICK_NULLSTATE  2

class PJoystick : public QObject
{
    Q_OBJECT
public:
    explicit PJoystick(QObject *parent = 0);
    ~PJoystick();
    //Функция инициализации джойстика
    bool Init(int js, int nstate);
    //Функция одиночного опроса состояния джойстика
    void once();
    //Функция запуска опроса состояния джойстика
    void start();
    //Фукнция остановки опроса
    void stop();
    //Функция возвращает смещение джойстика по оси Х
    int getPosX();
    //Функция возвращает смещение джойстика по оси У
    int getPosY();
    //Функция возвращает состояние кнопки
    bool getButton();
    //Функция получения статуса устройства
    bool getStatus();


private:
    //Статус устройства
    bool status;
    //Функция возвращает количество доступных джойстиков
    int availableJoysticks();
    //Функция возвращает индекс джойстика
    int currentJoystick();
    //Функция получает имя джойстика по индексу
    QString joystickName(int id);
    //Функция возвращает количество осей
    int joystickNumAxes(int id);
    //Функция возвращает количество кнопок
    int joystickNumButtons(int id);

    //Признак инициализации устройства
    bool init;

    //Погрешность начального состояния
    int nullstate;

    //Состояние осей устройства
    QList<int> axis;
    //Состояние кнопок устройства
    QList<bool> buttons;

    //Функция получения данных от джойстика
    void getdata();

    //Таймер опроса устройства
    QTimer timer;

    //количество джойстиков в системе
    int joyNum;
    //Количество осей
    int joyNumAxes;
    //Количество кнопок
    int joyNumButtons;
    //Положение оси Х
    int joyX;
    //Положение оси У
    int joyY;
    //Состояние нажатия кнопки
    bool button;

    //SDL джойстик
    SDL_Joystick* m_joystick;

signals:
    //Сигнал для уведомления о нажатии кнопки
    void btnPress(bool state);
    //Сигнал для уведомления об обновлении данных по осям
    void newData(int x, int y);

public slots:
    //Функция обновления данных от устройства
    void updateData();

};

#endif // PJOYSTICK_H
