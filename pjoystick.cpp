#include "pjoystick.h"

PJoystick::PJoystick(QObject *parent) :
    QObject(parent)
{
    //Начальная инициализация переменных
    status = false;
    init = false;
    joyX = 0;
    joyY = 0;
    joyNumAxes = 0;
    joyNumButtons = 0;
    button = false;

    //Инициализируем SDL (без видео джойстик не работает)
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
}

//Освобождаем ресурсы
PJoystick::~PJoystick()
{
    //Получаем количество джойстиков в системе
    joyNum = availableJoysticks();

    //Если есть джойстик
    if(joyNum != 0)
    {
        //очищаем список осей
        axis.clear();
        //очищаем список кнопок
        buttons.clear();
        //закрываем открытый джойстик
        if (m_joystick)
            SDL_JoystickClose(m_joystick);
    }
    //Очищаем специальные подсистемы
    SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    this->deleteLater();
}

//Функция инициализации джойстика
bool PJoystick::Init(int js, int nstate = JOYSTICK_NULLSTATE)
{
    //выставляем статус неисправности
    status = false;

    //Получаем количество доступных джойстиков
    joyNum = availableJoysticks();

    //Задаем погрешность начального положения джойстика
    if (nstate >= 0)
        nullstate = nstate;
    else
        nullstate = JOYSTICK_NULLSTATE;

    init = true;

    //Если в системе есть джойстик и выбран правильный номер джойстика
    if (joyNum > 0)
    {
        //Открываем устройство
        m_joystick = SDL_JoystickOpen(js);
        //Если успешно, то возвращаем положительный результат
        if (m_joystick != NULL)
        {
            status = true;
            //Подключаем сигнал от таймера к функции получения данных от устройства
            connect(&timer, SIGNAL(timeout()), this, SLOT(updateData()));
        }
    }

    return init;
}

//Функция одиночного опроса состояния джойстика
void PJoystick::once()
{
    //если устройство инициализировано
    if (init)
        //обновляем данные от устройства
        updateData();
}

//Функция запуска опроса состояния джойстика
void PJoystick::start()
{
    //если устройство инициализировано
    if (init)
        //если таймер активен
        if(!timer.isActive())
            //Запускаем таймер
            timer.start(1000 / QPS_JOYSTICK);
}

//Фукнция остановки опроса
void PJoystick::stop()
{
    if (timer.isActive())
        //останавливаем таймер
        timer.stop();
}

//Функция возвращает смещение джойстика по оси Х
int PJoystick::getPosX()
{
    return joyX;
}

//Функция возвращает смещение джойстика по оси Y
int PJoystick::getPosY()
{
    return joyY;
}

//Функция возвращает состояние кнопки
bool PJoystick::getButton()
{
    return button;
}

//Функция получения статуса устройства
bool PJoystick::getStatus()
{
    return status;
}

//Функция возвращает количество доступных джойстиков
int PJoystick::availableJoysticks()
{
     return SDL_NumJoysticks();
}

//Функция возвращает количество осей
int PJoystick::joystickNumAxes(int js)
{
    Q_ASSERT(js < availableJoysticks());
    Q_ASSERT(js >= 0);
    return (SDL_JoystickNumAxes(m_joystick));
}

//Функция возвращает количество кнопок
int PJoystick::joystickNumButtons(int js)
{
    Q_ASSERT(js < availableJoysticks());
    Q_ASSERT(js >= 0);
    return (SDL_JoystickNumButtons(m_joystick));
}

//Функция получения данных от джойстика
void PJoystick::getdata()
{
    //Очищаем состояние осей
    axis.clear();
    //Очищаем состояние кнопок
    buttons.clear();

    //Переменная для обработки событий
    SDL_Event event;
    try {
        if (SDL_PollEvent(&event) == -1)
            status = false;
        else
            status = true;
    }
    catch (...)
    {
        status = false;
    }

    //Получаем состояние осей
    for(int i = 0; i < SDL_JoystickNumAxes(m_joystick); i++)
    {
        axis.append(SDL_JoystickGetAxis(m_joystick, i) / 256);
    }

    //Получаем состояние кнопок
    for(int i = 0; i < SDL_JoystickNumButtons(m_joystick); i++)
    {
        buttons.append(SDL_JoystickGetButton(m_joystick, i));
    }

}

//Функция обновления данных от устройства
void PJoystick::updateData()
{
    //получаем данные от джойстика
    getdata();

    //Если состояние кнопки изменилось, то выдаем сигнал об этом
    if(buttons[0] != button)
    {
        button = buttons[0];
        emit btnPress(button);
    }

    //Если положение изменилось
    if ((joyX != axis[1]) || (joyY != axis[0]))
    {
        //запоминаем новое положение
        joyX = axis[1];
        joyY = axis[0];
        //Исключаем значения входящие в диапазон начального положения (исключаем дребезг)
        if ((joyX < nullstate) && (joyX > -nullstate) && (joyY < nullstate) && (joyY > -nullstate))
            //выдаем сигнал о новом положении
            emit newData(0, 0);
        else
            emit newData(joyX, joyY);
    }
}


