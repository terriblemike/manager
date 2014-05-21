/////////////////////////////////////////////////////////////////////////////////
// Основные определения
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// Для работы с сетью
#define DEFAULT_VIDEO_ID        -1
#define DEFAULT_VIDEO_PORT      3535
#define DEFAULT_VIDEO_ADRESS    "192.168.0.3"

#define DEFAULT_ABONENT_ID      2
#define DEFAULT_ABONENT_PORT    50
#define DEFAULT_ABONENT_ADRESS  "192.168.0.19"

#define DEFAULT_CAM1_ADRESS         "192.168.0.11"
#define DEFAULT_CAM2_ADRESS         "192.168.0.12"
#define DEFAULT_CAM3_ADRESS         "192.168.0.13"
#define DEFAULT_CAM4_ADRESS         "192.168.0.14"
#define DEFAULT_VIZIR_ADRESS        "192.168.0.15"
#define DEFAULT_TEPLOVIZOR_ADRESS   "192.168.0.16"



/////////////////////////////////////////////////////////////////////////////////
// Для входных команд НА ПУЛЬТ
// Лампы
#define CP_COMMAND_INPUT_LAMP_ENGAGE			1 // Команда на включение лампы
#define CP_COMMAND_INPUT_LAMP_DISENGAGE			2 // Команда на выключение лампы

// Контроль функционирования
#define CP_COMMAND_INPUT_SELF_CONTROL			3 // Команда на выполнение контроля функционирования

/////////////////////////////////////////////////////////////////////////////////
// Для выходных команд С ПУЛЬТА
// Кнопки
#define CP_COMMAND_OUTPUT_BUTTON_PRESS			4 // Команда, обозначающая нажатие на кнопку
#define CP_COMMAND_OUTPUT_BUTTON_RELEASE		5 // Команда, обозначающая отпускание кнопки

// Джойстик
#define CP_COMMAND_OUTPUT_JOYSTICK_X_AXIS		6 // Команда, обозначающая воздействие на ось Х
#define CP_COMMAND_OUTPUT_JOYSTICK_Y_AXIS		7 // Команда, обозначающая воздействие на ось Y
#define CP_COMMAND_OUTPUT_JOYSTICK_BUTTON		10 // Команда, обозначающая воздействие кнопку джойстика

//Дальномер (дальность передается двумя байтами: старший в аргументе 1, младший - в аргументе 2
#define CP_COMMAND_DALNOMER                     11 // Команда для передачи данных от дальномера

//Энкодеры (градусы передаются в аргументе 1, минуты в аргументе 2)
#define CP_COMMAND_ENCODER_UGOL                 12 // Команда для передачи данных от энкодера угла места
#define CP_COMMAND_ENCODER_AZIM                 13 // Команда для передачи данных от энкодера азимута


//Команда изменения режима наведения (навести, П/А, ручной передается во втором аргументе)
#define CP_COMMAND_NAVED_STATE                  14

//Команда режима автосопровождения
#define CP_COMMAND_AC                           15

//Команда записи логов
#define CP_COMMAND_LOGS                         16

// Контроль функционирования
#define CP_COMMAND_OUTPUT_SELF_CONTROL			8 // Команда-отчет о выполнении контроля функционирования


//Положение курсора по оси X
#define  DR_COMMAND_OUTPUT_CURSOR_POSITION_X    29
//Положение курсора по оси Y
#define DR_COMMAND_OUTPUT_CURSOR_POSITION_Y     30


//Изменение состояния АПУ
#define CP_STATUS_APU                           40
//Изменение состояния ЛВС
#define CP_STATUS_LVS                           41
//Изменение состояния Спойл
#define CP_STATUS_SPOIL                         42
//Изменение состояния ОЕС
#define CP_STATUS_OES                           43
//Изменение состояния получения изображения от видеоустройств
#define CP_STATUS                               44
//Отклонение джойстика по осям
#define DR_COMMAND_OUTPUT_JOYSTICK_X            45
#define DR_COMMAND_OUTPUT_JOYSTICK_Y            46
//Последняя нажатая кнопка на ПУ
#define DR_COMMAND_PULT_BTN                     48

//Константы для режима автосопровождения
#define AC_BREAK    0   //окончание режима
#define AC_BEGIN    1   //начало выделения объекта
#define AC_END      2   //окончание выделения объекта - начало режима



//#define CP_COMMAND_INPUT_LAMPSFROMNOTEBOOK 9 //команда на зажигание с ноутбука

/* Коды ошибок - заполним, когда будет понятен их состав
#define COMMAND_OUTPUT_ERR_1..N
*/

/////////////////////////////////////////////////////////////////////////////////
// Определения для кнопок
/////////////////////////////////////////////////////////////////////////////////
//Номера входов платы дискретных в\в для кнопок пульта управления
//Кнопки без группы
#define PULT_BTN_COMMON_CONTROL 00
#define PULT_BTN_COMMON_WORK    34
#define PULT_BTN_COMMON_BLOCK   100
#define PULT_BTN_COMMON_OBSLUG  35
#define PULT_BTN_COMMON_AC      67
#define PULT_BTN_COMMON_RESERV  52
#define PULT_BTN_COMMON_DALNOST 1
#define PULT_BTN_COMMON_VISOKOE 2

//Экран
#define PULT_BTN_SCREEN_MULTI   23
#define PULT_BTN_SCREEN_OBZOR   26
#define PULT_BTN_SCREEN_NAVED   24
#define PULT_BTN_SCREEN_KAMERA  25

//Неизвестные
#define PULT_BTN_UNKNOWN_HP1    53
#define PULT_BTN_UNKNOWN_HP2    54
#define PULT_BTN_UNKNOWN_HP3    55
#define PULT_BTN_UNKNOWN_HP4    56
#define PULT_BTN_UNKNOWN_HP5    57
#define PULT_BTN_UNKNOWN_HP6    60
#define PULT_BTN_UNKNOWN_HP7    61
#define PULT_BTN_UNKNOWN_HP8    62
#define PULT_BTN_UNKNOWN_HP9    63
#define PULT_BTN_UNKNOWN_IP1    64
#define PULT_BTN_UNKNOWN_IP2    65
#define PULT_BTN_UNKNOWN_IP3    66

//Положение антенны
#define PULT_BTN_POLOG_SLOG     22
#define PULT_BTN_POLOG_RAZV     21

//Фокусировка антенны
#define PULT_BTN_FOCUS_35       30
#define PULT_BTN_FOCUS_50       31
#define PULT_BTN_FOCUS_75       32
#define PULT_BTN_FOCUS_100      33
#define PULT_BTN_FOCUS_AUTO     27

//Наведение
#define PULT_BTN_NAVED_NIGHT     10
#define PULT_BTN_NAVED_NAVESTI  03
#define PULT_BTN_NAVED_PA       07
#define PULT_BTN_NAVED_MANUAL   06
#define PULT_BTN_NAVED_ZOOM_IN  04
#define PULT_BTN_NAVED_ZOOM_OUT 05

//Обзор
#define PULT_BTN_OBZOR_1        11
#define PULT_BTN_OBZOR_2        12
#define PULT_BTN_OBZOR_3        13
#define PULT_BTN_OBZOR_4        14
#define PULT_BTN_OBZOR_A        17
#define PULT_BTN_OBZOR_C        20
#define PULT_BTN_OBZOR_ZOOM_IN  15
#define PULT_BTN_OBZOR_ZOOM_OUT 16
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
// Определения для ламп
/////////////////////////////////////////////////////////////////////////////////
//Номера выходов платы дискретных в\в для индикаторов пульта управления
//Индикаторы без группы
#define PULT_IND_COMMON_CONTROL 4
#define PULT_IND_COMMON_AC      61
#define PULT_IND_COMMON_GK      5
#define PULT_IND_COMMON_VISOKOE 6

//Зумер
#define PULT_IND_COMMON_ZOOMER  33

//Контроль состояния
#define PULT_IND_STATUS_APU     55
#define PULT_IND_STATUS_OES     56
#define PULT_IND_STATUS_LVS     57
#define PULT_IND_STATUS_SPOIL   58

//Экран
#define PULT_IND_SCREEN_MULTI   27
#define PULT_IND_SCREEN_OBZOR   32
#define PULT_IND_SCREEN_NAVED   30
#define PULT_IND_SCREEN_KAMERA  31

//Неизвестные
#define PULT_IND_UNKNOWN_HP1    34
#define PULT_IND_UNKNOWN_HP2    35
#define PULT_IND_UNKNOWN_HP3    36
#define PULT_IND_UNKNOWN_HP4    37
#define PULT_IND_UNKNOWN_HP5    40
#define PULT_IND_UNKNOWN_HP6    41
#define PULT_IND_UNKNOWN_HP7    42
#define PULT_IND_UNKNOWN_HP8    43
#define PULT_IND_UNKNOWN_HP9    44
#define PULT_IND_UNKNOWN_IP1    45
#define PULT_IND_UNKNOWN_IP2    46
#define PULT_IND_UNKNOWN_IP3    47

//Положение антенны
#define PULT_IND_POLOG_POHOD    25
#define PULT_IND_POLOG_WORK     26
//#define PULT_IND_POLOG_SLOG
//#define PULT_IND_POLOG_RAZV

//Фокусировка антенны
#define PULT_IND_FOCUS_35       51
#define PULT_IND_FOCUS_50       52
#define PULT_IND_FOCUS_75       53
#define PULT_IND_FOCUS_100      54
#define PULT_IND_FOCUS_AUTO     50

//Наведение
#define PULT_IND_NAVED_NIGHT     14
#define PULT_IND_NAVED_NAVESTI  07
#define PULT_IND_NAVED_PA       13
#define PULT_IND_NAVED_MANUAL   12
#define PULT_IND_NAVED_ZOOM_IN  10
#define PULT_IND_NAVED_ZOOM_OUT 11

//Обзор
#define PULT_IND_OBZOR_1        15
#define PULT_IND_OBZOR_2        16
#define PULT_IND_OBZOR_3        17
#define PULT_IND_OBZOR_4        20
#define PULT_IND_OBZOR_A        23
#define PULT_IND_OBZOR_C        24
#define PULT_IND_OBZOR_ZOOM_IN  21
#define PULT_IND_OBZOR_ZOOM_OUT 22


/////////////////////////////////////////////////////////////////////////////////
// Определения для концевых выключателей и замков
/////////////////////////////////////////////////////////////////////////////////
//Номера выходов платы дискретных в\в для концевых выключателей и замков
//Концевые выключатели
#define IN_KONC_LEFT_NZ         36
#define IN_KONC_LEFT_NO         37
#define IN_KONC_RIGHT_NZ        40
#define IN_KONC_RIGHT_NO        41
#define IN_KONC_ZTU_NZ          42
#define IN_KONC_ZTU_NO          43
#define IN_KONC_SPOIL_PER_NZ    44
#define IN_KONC_SPOIL_PER_NO    45
#define IN_KONC_SPOIL_ZAD_NZ    46
#define IN_KONC_SPOIL_ZAD_NO    47

#define IN_GERKON_NZ            50
#define IN_GERKON_NO            51

//Замки
#define OUT_ZAMOK_APU1          0
#define OUT_ZAMOK_APU2          1
#define OUT_ZAMOK_SPOIL_PERED   2
#define OUT_ZAMOK_SPOIL_ZAD     3

#define ZAMOK_ZTU_OSLAB         20
#define ZAMOK_ZTU_CLOSE         50


#define ZOOM_MIN    1.0
#define ZOOM_MAX    20.0
#define ZOOM_SHAG   0.1

/*
***************КАК ИСПОЛЬЗУЕТСЯ ФОРМИРОВАНИЕ КОМАНДЫ******************
Информационный пакет состоит из 4-х байт.
Первый байт - код отправителя из файла H_DEVICE_CODES.h (для пульта - #define DEVICE_PANEL     0x04)
Второй байт - код команды. Коды команд представлены в самом начале этого файла.
Третий и четверты байты - если не требуется специальное разделение аргумента (для пульта - не требуется),
то третий - нулевой, а четвертый - аргумент.
*/

// Формирование команды
//void Make_Command(char * ComBuff, int Code, int Argument1 = 0, int Argument2 = 0)
//{
//    *(ComBuff) = 0;
//    *(ComBuff+1) = 0;
//    *(ComBuff+2) = 0;
//    *(ComBuff+3) = 0;

//    // Упаковка команды в байт
//    *(ComBuff) = *(ComBuff) | DEVICE_PANEL;
//    *(ComBuff+1) = *(ComBuff+1) | Code & 0xff;
//    *(ComBuff+2) = *(ComBuff+2) | Argument1 & 0xff;
//    *(ComBuff+3) = *(ComBuff+3) | Argument2 & 0xff;
//}

// Получение команды
//void Get_Command(int * OutputBuffer, char * InputBuffer)
//{
//    // Обнуление буфера
//    *(OutputBuffer) = 0;
//    *(OutputBuffer+1) = 0;
//    *(OutputBuffer+2) = 0;
//    *(OutputBuffer+3) = 0;

//    // Распаковка команды
//    *OutputBuffer = (*(InputBuffer))&255;			// Код отправителя
//    *(OutputBuffer+1) = (*(InputBuffer+1))&255;		// Код команды
//    *(OutputBuffer+2) = (*(InputBuffer+2))&255;		// Первый аргумент
//    *(OutputBuffer+3) = (*(InputBuffer+3))&255;		// Второй аргумент
//}
