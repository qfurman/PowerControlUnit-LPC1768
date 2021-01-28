#include    <myiolpc1768.h>

#include "type.h"
#include "lcd.h"
#include "keys.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "LPC_Rtc.h"
//#include "defines.h"
//#include "uart_0.h"
//#include "uart_1.h"

//#include "spi.h"
#include "ramtron.h"

#include "timers.h"

#include "main.h"
#include "adc.h"
//#include "pack.h"
#include "modbus.h"
#include "temperatures.h"
#include "SCRs.h"

#define COLUMN 20
#define LINE 4

#define SUB_NUM 0
#define SUB_HEX 1
#define SUB_TIME 2
#define SUB_DATE 3
#define SUB_DAY 4
#define SUB_BOOL 5
#define SUB_PARITY 6
#define SUB_CR_LF 7
#define SUB_123 8
#define SUB_TC 9
#define SUB_comp 10
#define SUB_sts 11
#define SUB_Baud 12
#define SUB_SIGP 13

#define menu_key    5
#define up          1
#define left        3
#define right       4
#define down        2
#define enter       6
#define enc_ent     17
//#define inc         7
//#define dec         8

LPC_Rtc_Time_t Time;

const MENU_MAN *manage;//на структуру управління меню
int LNG;//мова 0-rus 1-ua

RTF general_purpose_flags;
int Ti_flag;//ознака зміни тривалості імпульсу
int wd_count;
int menu_ident;//намер підменю, що викликав натисканням старт

//парність
const char *parPt[4][2] ={
  {"0 ODD ","0НeПар"},
  {"1 EVEN","1Парн."},
  {"2 SET1","2Уст.1"},
  {"3 SET0","3Уст.0"}};

//виведення в рядок
void print(char* dest)
{
    int count;
    char ch;
    bool flag=false;
    char *ptr;
    LPC_Rtc_Date_t pdate;

    switch(manage->type){
      case SUB_HEX:
      case SUB_NUM:
        count = sprintf(dest,manage->format[LNG],*manage->number);
        if(manage->fract){              //якщо є знаки після коми
            dest += count;              //поінтемо на кінекь тобто на нуль
            do{
                ch = *dest;
                *(dest+1) = ch;         //перестановка символа ліворуч
                dest--;                 //на одмн символ ліворуч
                if(isdigit(ch)&&!flag){ //якщо цифра
                    flag = true;        //знайдено останню цифру з кінця
                    count = manage->fract;//кількість яку треба переставити
                }
            }while(--count);            //пошук першої цифри з кінця
            *(dest+1) = '.';            //ставимо крапку
            if(*dest == ' ')*dest = '0';//для чисел із незаповненням 0 лівих позицій поставити нуль замість пробіла
        }
        break;

      case SUB_BOOL://BOOL
        sprintf(dest,manage->format[LNG],*manage->number? " ON":"OFF");
        break;

      case SUB_PARITY:
        sprintf(dest,manage->format[LNG],parPt[*manage->number][LNG]);
        break;

      case SUB_TIME:
        //LPC_Rtc_Time_t Time;
        //if(RTC_GetTime(&Time))
        sprintf(dest,manage->format[LNG],*((int*)0x40024028),*((int*)0x40024024),*((int*)0x40024020));//h:m:s
        break;

      case SUB_DATE:
        //sprintf(dest,manage->format[LNG],*((int*)0xE0024028),*((int*)0xE0024024),*((int*)0xE0024020));//h:m:s
        RTC_GetDate(&pdate);
        sprintf(dest,manage->format[LNG],pdate.day, pdate.month, pdate.year);
        break;

      case SUB_DAY:
        switch(*((int*)0x40024030)){//dow
          case 0: ptr = "SUNDAY              ";break;
          case 1: ptr = "MONDAY              ";break;
          case 2: ptr = "TUESDAY             ";break;
          case 3: ptr = "WEDNESDAY           ";break;
          case 4: ptr = "THURSDAY            ";break;
          case 5: ptr = "FIDAY               ";break;
          case 6: ptr = "SATURNDAY           ";break;
          default: ptr = "                    ";break;
        }
        sprintf(dest,manage->format[LNG],ptr);
        return;

      case SUB_CR_LF:
        switch(*manage->number){
          default:
          case 0: ptr = "\2\377\377  \377\377\3";break;
          case 1: ptr = "\2CR  \377\377\3";break;
          case 2: ptr = "\2CR  LF\3";break;
        }
        sprintf(dest,manage->format[LNG],ptr);
        break;

      case SUB_TC:
        switch(*manage->number){
          default:
          case 0: ptr = "0 - Температура в зоні горіння";break;
          case 1: ptr = "1 - Температура в зоні фідера";break;
        }
        sprintf(dest,manage->format[LNG],ptr);
        break;

      case SUB_123:
        switch(*manage->number){
          default:
          case 0: ptr = "0 - mA";break;
          case 1: ptr = "1 - ТХА(K)";break;
          case 2: ptr = "2 - ТПР(B)";break;
          case 3: ptr = "3 - ТСМ100";break;
          case 4: ptr = "4 - ТСП100";break;
        }
        sprintf(dest,manage->format[LNG],ptr);
        break;

      case SUB_comp:
        switch(*manage->number){
          default:
          case 0: ptr = "0 - Термокомпенсації Немає";break;
          case 1: ptr = "1 - Термокомпенсація Є";break;
        }
        sprintf(dest,manage->format[LNG],ptr);
        break;

      case SUB_sts:
        switch(*manage->number){
          default:
          case 0: ptr = "Состояние датчика:        НОРМА";break;
          case 1: ptr = "Состояние датчика:        ОБРЫВ";break;
        }
        sprintf(dest,manage->format[LNG],ptr);
        break;

      case SUB_Baud:
        switch(*manage->number){
          default:
          case 0: ptr = "1:  2400";break;
          case 1: ptr = "2:  4800";break;
          case 2: ptr = "3:  9600";break;
          case 3: ptr = "4: 19200";break;
          case 4: ptr = "5: 38400";break;
          case 5: ptr = "6: 57600";break;
          case 6: ptr = "7:115200";break;
        }
        sprintf(dest,manage->format[LNG],ptr);
        break;

      case SUB_SIGP:
        switch(*manage->number){
        default:
        case 0: ptr = "0:   12V UI";break;
        case 1: ptr = "1:12V+HV UI";break;
        case 2: ptr = "2: PIEZO UI";break;
        }
        sprintf(dest,manage->format[LNG],ptr);
        break;
    }
}// END print


//з рядка в число
void scan(char* src)
{
    int temp;
    char* ptr;
    char sign=0;
    char hour_,min,sec;
    char month_,date_;
    LPC_Rtc_Date_t pdate;

    switch(manage->type){
      case SUB_NUM:
        ptr = strchr(manage->format[LNG],'%');//пошук позиції розміщення числа в рядку-форматі
        if(ptr){
            ptr = (char*)(ptr - manage->format[LNG] + src);//перенесення позиціїї в рядок з самим числом
            temp = atoi(ptr);//претворення з рядка в число "цілої" частини
            if(*ptr == '-')sign = 1;
            if(manage->fract){
                ptr = strrchr(ptr,'.')+1;//на дробову частину
                char ii = manage->fract;
                do temp *= 10; while(--ii);//зсув на кількість дробових знаків
                temp += sign? -atoi(ptr) : atoi(ptr);//додаємо "дробову" частину
            }
            temp = (temp<manage->min)? manage->min:((temp>manage->max)? manage->max:temp);
            *manage->number =  temp;
        }
        break;

      case SUB_BOOL://BOOL
        ptr = strrchr(manage->format[LNG],'%');//пошук позиції розміщення числа в рядку-форматі
        if(ptr){
            ptr = (char*)(ptr - manage->format[LNG] + src);//перенесення позиціїї в рядок з самим числом
            if(*ptr == 'O')*manage->number = 0;//тобто OFF
            else *manage->number = 1;//тобто ON
        }
        break;

      case SUB_PARITY:
        switch(*(strrchr(manage->format[LNG],'%')- manage->format[LNG] + src)){
          default:
          case '0': *manage->number = 0;break;
          case '1': *manage->number = 1;break;
          case '2': *manage->number = 2;break;
          case '3': *manage->number = 3;break;
        }
        break;

      case SUB_TIME:

        ptr = strchr(src,':');
        hour_ = atoi(ptr-2);
        if(hour_ <= 23)*((int*)0x40024028) = hour_;

        min = atoi(ptr+1);
        if(min <= 59)*((int*)0x40024024) = min;

        ptr = strchr(ptr+1,':');
        sec = atoi(ptr+1);
        if(sec <= 59)*((int*)0x40024020) = sec;
        //arch_time = new_time(N.snap_time);//наступний час запису в архів
        return;

      case SUB_DATE:

        //копіювання прочитаної дати в записувану
        RTC_GetDate(&pdate);

        ptr = strchr(src,'.');
        date_ = atoi(ptr-2);
        if((date_ <= 31)||(date_ > 0))pdate.day = date_;

        month_ = atoi(ptr+1);
        if((month_ <= 12)||(month_ > 0))pdate.month = month_;

        ptr = strchr(ptr+1,'.');
        pdate.year = atoi(ptr+1);

        RTC_SetDate(&pdate);
        //arch_time = new_time(N.snap_time);//наступний час запису в архів
        return;

      case SUB_CR_LF:
        ptr = strrchr(manage->format[LNG],'%')- manage->format[LNG] + src+1;
        switch(*ptr){
          default:
          case '\377': *manage->number = 0;break;
          case 'C':  if(*(ptr+4)==0xff)*manage->number = 1; else *manage->number = 2;break;
        }
        break;

      case SUB_HEX:
        ptr = strchr(manage->format[LNG],'%');//пошук позиції розміщення числа в рядку-форматі
        if(ptr){
            ptr = (char*)(ptr - manage->format[LNG] + src);//перенесення позиціїї в рядок з самим числом
            do{ptr++;}while(*ptr >='0');//пошук кінця числа
            ptr--; // назад в число
            char hex_iter,ch;
            hex_iter = 0;
            //претворення з hex в число
            temp = 0;
            do{
                ch = *ptr;
                temp |= ((ch>0x39)?(ch&0x0f)+9:ch&0x0f)<<hex_iter*4;
                hex_iter++;
                ptr--;
            }while(*ptr>='0'&&*ptr<='F');

            temp = (temp<manage->min)? manage->min:((temp>manage->max)? manage->max:temp);
            *manage->number =  temp;
        }
        break;

      case SUB_TC:
        switch(*(strrchr(manage->format[LNG],'%')- manage->format[LNG] + src)){
          default:
          case '0': *manage->number = 0;break;
          case '1': *manage->number = 1;break;
        }
        break;

      case SUB_123:
        switch(*(strrchr(manage->format[LNG],'%')- manage->format[LNG] + src)){
          default:
          case '0': *manage->number = 0;break;
          case '1': if(*(strrchr(manage->format[LNG],'%')- manage->format[LNG] + src+1)==' ')*manage->number = 1;
          else *manage->number = 10;
          break;
          case '2': *manage->number = 2;break;
          case '3': *manage->number = 3;break;
          case '4': *manage->number = 4;break;
          case '5': *manage->number = 5;break;
          case '6': *manage->number = 6;break;
          case '7': *manage->number = 7;break;
          case '8': *manage->number = 8;break;
          case '9': *manage->number = 9;break;
        }
        break;

      case SUB_comp:
        switch(*(strrchr(manage->format[LNG],'%')- manage->format[LNG] + src)){
          default:
          case '0': *manage->number = 0;break;
          case '1': *manage->number = 1;break;
        }
        break;
      case SUB_Baud:
        *manage->number = *(strrchr(manage->format[LNG],'%')- manage->format[LNG] + src)-0x31;
        break;
    case SUB_SIGP:
        switch(*(strrchr(manage->format[LNG],'%')- manage->format[LNG] + src)){
          default:
          case '0': *manage->number = 0;break;
          case '1': *manage->number = 1;break;
          case '2': *manage->number = 2;break;
        }
        break;
    }

    //якщо параметр знаходиться позамежами структури параметрів то не здійснювати запис у рамтрон
    if((int)manage->number<(int)&N || (int)manage->number>(int)&N+sizeof(N))return;

    spi1_init();
    while(spi1_sts_bit.end_rx == 0);

    int addr =(int)manage->number - (int)&N;
    wr_rmtrn(addr, manage->number, sizeof(int));
    while(spi1_sts_bit.end_rx == 0);

    do{
        spi1_rd_status_flash(0);
        while(spi1_sts_bit.end_rx == 0);
    }while(spi1_status_rg&0x01);//while write in progress(WIP)


    rd_rmtrn(addr, manage->number, sizeof(int));
    while(spi1_sts_bit.end_rx == 0);
}// end scan



//################################################################################# menu.c

int password_test;//для введеного пароля
/*************************************************************************************************\
* організація стека для повернення назад по меню
\*************************************************************************************************/
struct cell{
    char sublevel;      //верхній рядок на екрані
    char lcd_flash_row; //рядок, що моргає
    const MENU_MAN *menu_ptr;//адреса
    char menu_size;     //розмір
};
struct cell map;//параметри поточного меню

#define menu_stack_size 16
struct{
    struct cell one_cell[menu_stack_size];
    char index;
}menu_stack;

struct {//маю намір вписувати сюди поточний стан меню
    char input_row;    //рядок де відбувається модифікація
    char fetch; //0 - звичайний режим 1- необхідно захопити рядок 2- відбулося захоплення рядка
    char flash;//кількість позицій що моргають
    bool menu;//якшо 1 то навігація по меню, якщо 0 то головний екран
    void (*alt)();//якщо  не дорівнює нулю то дисплей використовується альткрнативною функцією ареса її сюди і записана
    const MENU_MAN *object;//обєкт що підлягає модифікації
}menu_status;

void cpy_xx(char *xptr_dest, char *xptr_src, unsigned short count)//копіює
{
    do{*xptr_dest++ = *xptr_src++;}
    while(--count);
}

void clr_xx(char *xptr_dest, unsigned short count)//обнуляє память
{
    do{*xptr_dest++ = 0;}
    while(--count);
}

unsigned char menu_push()//повертає одиницю при успішному закиданні в стек і 0 якщо більше нікуди
{
    if(menu_stack.index == menu_stack_size)return 0;
    cpy_xx((char*)&menu_stack.one_cell[menu_stack.index++], (char*)&map, sizeof(map));

    return 1;
}

unsigned char menu_pop()//повертає 1 при успішному вийманні і 0 при порожньому стеку
{
    if(menu_stack.index == 0)return 0;
    cpy_xx((char*)&map, (char*)&menu_stack.one_cell[--menu_stack.index],sizeof(map));
    clr_xx((char*)&menu_stack.one_cell[menu_stack.index],sizeof(map));//очищення памяті
    return 1;
}


typedef struct{
char no_refresh :1;
char input      :1;//модифікація
char enter_bit  :1;
char request_for:1;
char was_make   :1;
}INPUT_STATUS;

union{
    INPUT_STATUS i_sts_bit;
    char input_status;
};

/******************************************************************************/
/******************************************************************************/
char LCD_init_crsr_position(void)
{
    /*
    LCD_crsr_position = LCD_input_str+19;
    do{
        if(isdigit(*LCD_crsr_position))return 1;
    }while(LCD_crsr_position--);
    return 0;
    */
    char *ptr;
    ptr = (char*)strchr(map.menu_ptr[menu_status.input_row].format[LNG],'%');
    if(ptr){
        LCD_crsr_position = (ptr - map.menu_ptr[menu_status.input_row].format[LNG] + &LCD_input_str[0]);
        return 1;
    }
    else{
        LCD_crsr_position = 0;
        menu_status.fetch = 0;
        return 0;
    }

}
/******************************************************************************/
/******************************************************************************/
char LCD_init_crsr_position_left(void)
{
    LCD_crsr_position = LCD_input_str+19;
    do{
        if(isdigit(*LCD_crsr_position))return 1;
    }while(LCD_crsr_position--);
    return 0;

}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


/*-----------------------------------------------------------------------------------------------*/
void enter_key(void)
{
    unsigned char i;

    if(menu_status.fetch == 0){//якщо меню в режимі навігації
        //тобто номер піпункту меню 0..map.menu_size-1
        if((i = map.sublevel+map.lcd_flash_row) > map.menu_size)i -= map.menu_size+1;

        if(map.menu_ptr[i].next && map.menu_ptr[i].change !=128){//якщо є наступна вітка дерева меню і якщо це не пароль то рухатись по ній
            if(map.menu_ptr[i].pre_enter)map.menu_ptr[i].pre_enter(0);//виконати фунцію першого натискання кн. ентер

            menu_push();//закинути в стек поточну позицію
            map.menu_size = map.menu_ptr[i].size;
            map.menu_ptr = map.menu_ptr[i].next;
            map.lcd_flash_row = 0;
            map.sublevel = 0;
            return;
        }
        else{
            /*
            if(map.menu_ptr[menu_status.input_row].pre_enter)
                map.menu_ptr[menu_status.input_row].pre_enter();//виконати фунцію першого натискання кн. ентер
            */
            if(map.menu_ptr[i].change){//параметр можна змінювати
                menu_status.input_row = i;//рядок де буде відбуватися зміна числа
                menu_status.fetch = 1;//необхідно захопити рядок для подальшої модифікаціі

                menu_status.object = map.menu_ptr + menu_status.input_row;//ОБ'ЄКТ ЩО МОДИФІКУЄТЬСЯ

                void(*ptr)(char) = (void(*)(char)) map.menu_ptr[menu_status.input_row].pre_enter;
                if(ptr)ptr(menu_status.input_row);//виконати фунцію першого натискання кн. ентер
            }
        }
    }
    else{//якщо меню в режимі модифікації
        if((map.menu_ptr[menu_status.input_row].post_enter == 0) || (map.menu_ptr[menu_status.input_row].post_enter()==0)){
            manage = &map.menu_ptr[menu_status.input_row];
            scan(LCD_input_str);
        }
        menu_status.fetch = 0;//повернення в режим навігації
    }
}

/*-----------------------------------------------------------------------------------------------*/
void right_key(void)
{
    if(i_sts_bit.was_make)input_status = 0;
    if(menu_status.fetch == 2){
       if(map.menu_ptr[menu_status.input_row].right_funct)
        map.menu_ptr[menu_status.input_row].right_funct();
    }//RIGHT();
}


/*-----------------------------------------------------------------------------------------------*/
void up_key(void)
{
    if(i_sts_bit.was_make)input_status = 0;
    if(menu_status.fetch == 2){
        if(map.menu_ptr[menu_status.input_row].up_funct)
            map.menu_ptr[menu_status.input_row].up_funct();
    }//UP();
    else{
        //для поморгування рядком
        if(map.lcd_flash_row > 0)map.lcd_flash_row--;
        else{
            //якщо в меню рядків меньше ніж чотири
            if(map.menu_size <= LINE-1)map.lcd_flash_row = map.menu_size;
            else{
                if(map.sublevel != 0) map.sublevel--;
                else map.sublevel = map.menu_size;
            }
        }
    }
}

/*-----------------------------------------------------------------------------------------------*/
void down_key(void)
{
    if(i_sts_bit.was_make)input_status = 0;
    if(menu_status.fetch == 2){
        if(map.menu_ptr[menu_status.input_row].down_funct)
        map.menu_ptr[menu_status.input_row].down_funct();
    }//DOWN();
    else{
        //для поморгування рядком
        if(map.lcd_flash_row < LINE-1){
            if(map.lcd_flash_row < map.menu_size)map.lcd_flash_row++;
            else map.lcd_flash_row = 0;
        }
        else{
            //якщо в меню рядків меньше ніж еа екрані
            if(map.menu_size <= LINE-1)map.lcd_flash_row = 0;
            else{
                if(map.sublevel < map.menu_size) map.sublevel++;
                else map.sublevel = 0;
            }
        }
    }
}

/*-----------------------------------------------------------------------------------------------*/
void left_key(void)
{
    if(i_sts_bit.was_make)input_status = 0;
    if(menu_status.fetch == 2){
        if(map.menu_ptr[menu_status.input_row].left_funct)
        map.menu_ptr[menu_status.input_row].left_funct();
    }//LEFT();
}

/*----------------------------------------------------------------------------*/
void menu_KEY(void)
{
    if(menu_status.fetch == 0){
      if(menu_pop()==0){
        menu_status.menu = false;//перейти з меню на голоний екран
        map.lcd_flash_row = 0;
        map.sublevel = 0;
      }
    }
    else menu_status.fetch = 0;//прейти в режим навігації
}
/*----------------------------------------------------------------------------*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int view;//для режиму відображення
int Vbip;// для переключення bip струм або
int I1I2;//переключення змінювання тривалості імпульсів 1 чи 2
int angDl;//кут си затримка
int incr_;//для форсажу енкодера
int PH_pl;//для переключення каналу регулювання тиску
const char bfly[]={'-','+','*','#'};
/*===============================================================================================*/
/*===============================================================================================*/
char bip1str[24];//для біп сигналу два рядки
char bip2str[24];
/*===============================================================================================*/
/*===============================================================================================*/

//============================================================================//
// ВСЕ ТЕ САМЕ ТІЛЬКИ ДЛЯ НАСОСФОРСУНОК
//============================================================================//
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

const char *ver[2]={"  FIRMARE VERSION   ","  FIRMARE VERSION   "};
const char *vern[2]={"ver.  ","ver.  "};
const char *vert[2]={"time ","time "};

void version_functP()
{
    LCD_put_str(0,0,ver[LNG]);
    sprintf(LCD_str,"%s        %s ",vern[LNG], build_num);
    LCD_put_str(1,0,LCD_str);
    sprintf(LCD_str,"date    %s ", build_date);
    LCD_put_str(2,0,LCD_str);
    sprintf(LCD_str,"%s      %s ",vert[LNG], build_time);
    LCD_put_str(3,0,LCD_str);


    //якщо натиснути меню то вийти з процесу пошуку
    switch(kays_pop()){
      case menu_key:
        menu_status.alt = 0;
        menu_status.fetch = 0;
        return;

    }

}
/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
void version_fun(char i)
{
  menu_status.alt = version_functP;

}
/*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
//для організації запуску функцій кнопкою 0 start
void zero_key(void)
{
    unsigned char i;

    if(menu_status.fetch == 0){//якщо меню в режимі навігації
        //тобто номер піпункту меню 0..map.menu_size-1
        if((i = map.sublevel+map.lcd_flash_row) > map.menu_size)i -= map.menu_size+1;

        if(map.menu_ptr[i].next && map.menu_ptr[i].change !=128){//якщо є наступна вітка дерева меню і якщо це не пароль то рухатись по ній
            menu_ident = i;//намер підменю, що викликав натисканням старт
            if(map.menu_ptr[i].post_enter)map.menu_ptr[i].post_enter();//виконати фунцію першого натискання кн. ентер
            return;
        }
    }
}
/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/*-----------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*/
const char menu_key_arr[]={'0','1','2','3','3','4','5','6','6','7','8','9','9','9','0'};
void default_key(char dkey)
{
    if(i_sts_bit.was_make)input_status = 0;
    if(menu_status.fetch == 2){
        if(map.menu_ptr[menu_status.input_row].type < SUB_DAY){
            if(isdigit(*LCD_crsr_position)){
                if(dkey < 15)*LCD_crsr_position = menu_key_arr[dkey];
            }
            else{
                if(*LCD_crsr_position == '+')*LCD_crsr_position = '-';
                else
                    if(*LCD_crsr_position == '-')*LCD_crsr_position = '+';
            }

            if(map.menu_ptr[menu_status.input_row].right_funct)
            map.menu_ptr[menu_status.input_row].right_funct();
        }

    }
    if(menu_status.fetch == 0 && dkey == 0xe) zero_key();
}

/*-----------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*/
void encoder_key(void)
{
    unsigned char i;

    if(menu_status.fetch == 0){//якщо меню в режимі навігації
        //тобто номер піпункту меню 0..map.menu_size-1
        if((i = map.sublevel+map.lcd_flash_row) > map.menu_size)i -= map.menu_size+1;
      //частина що стосується кнопки на енкодері
        if(map.menu_ptr[i].next && map.menu_ptr[i].change !=128){//якщо є наступна вітка дерева меню і якщо це не пароль то рухатись по ній
            menu_ident = i;//намер підменю, що викликав натисканням старт
            if(map.menu_ptr[i].post_enter)map.menu_ptr[i].post_enter();//виконати фунцію першого натискання кн. ентер
            else{
              if(map.menu_ptr[i].pre_enter)map.menu_ptr[i].pre_enter(0);//виконати фунцію першого натискання кн. ентер

              menu_push();//закинути в стек поточну позицію
              map.menu_size = map.menu_ptr[i].size;
              map.menu_ptr = map.menu_ptr[i].next;
              map.lcd_flash_row = 0;
              map.sublevel = 0;
              return;
            }
        }
        else{
            if(map.menu_ptr[i].change){//параметр можна змінювати
                menu_status.input_row = i;//рядок де буде відбуватися зміна числа
                menu_status.fetch = 1;//необхідно захопити рядок для подальшої модифікаціі

                menu_status.object = map.menu_ptr + menu_status.input_row;//ОБ'ЄКТ ЩО МОДИФІКУЄТЬСЯ

                void(*ptr)(char) = (void(*)(char)) map.menu_ptr[menu_status.input_row].pre_enter;
                if(ptr)ptr(menu_status.input_row);//виконати фунцію першого натискання кн. ентер
            }
        }
    }
    else{//якщо меню в режимі модифікації
        if((map.menu_ptr[menu_status.input_row].post_enter == 0) || (map.menu_ptr[menu_status.input_row].post_enter()==0)){
            manage = &map.menu_ptr[menu_status.input_row];
            scan(LCD_input_str);
        }
        menu_status.fetch = 0;//повернення в режим навігації
    }

}

/*-----------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
void UP()
{
    if(*LCD_crsr_position == '9')*LCD_crsr_position = '0';
    else
        if(*LCD_crsr_position == '+')*LCD_crsr_position = '-';
        else
            if(*LCD_crsr_position == '-')*LCD_crsr_position = '+';
            else *LCD_crsr_position = *LCD_crsr_position + 1;
}

/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
void DOWN()
{
    if(*LCD_crsr_position == '0')*LCD_crsr_position = '9';
    else
        if(*LCD_crsr_position == '+')*LCD_crsr_position = '-';
        else
            if(*LCD_crsr_position == '-')*LCD_crsr_position = '+';
            else *LCD_crsr_position = *LCD_crsr_position - 1;
}
/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
void RIGHT()
{
    LCD_crsr_position++;
    if((LCD_crsr_position > &LCD_input_str[0]+COLUMN-1) || (*LCD_crsr_position <= ' ')){
        char *ptr;
        ptr = (char*)strchr(map.menu_ptr[menu_status.input_row].format[LNG],'%');
        if(ptr)LCD_crsr_position = (ptr - map.menu_ptr[menu_status.input_row].format[LNG] + &LCD_input_str[0]);
        else{
            LCD_crsr_position = 0;
            menu_status.fetch = 0;
            return;
        }
    }
    if(*LCD_crsr_position =='.' || *LCD_crsr_position == ':'||*LCD_crsr_position == 'e')LCD_crsr_position++;
}

/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
void LEFT()
{
    if((LCD_crsr_position != &LCD_input_str[0])&&(*(LCD_crsr_position-1) != ' '))LCD_crsr_position--;
    else
    {
        if(LCD_init_crsr_position_left() == 0){
            LCD_crsr_position = 0;
            menu_status.fetch = 0;
            return;
        }
    }
    if(*LCD_crsr_position == '.'||*LCD_crsr_position == ':'||*LCD_crsr_position == 'e')LCD_crsr_position--;
}
/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
void UP_hex()
{
    if(*LCD_crsr_position == 'F')*LCD_crsr_position = '0';
    else
        if(*LCD_crsr_position == '9')*LCD_crsr_position = 'A';
        else *LCD_crsr_position = *LCD_crsr_position + 1;
}
/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
void DOWN_hex()
{
    if(*LCD_crsr_position == '0')*LCD_crsr_position = 'F';
    else
        if(*LCD_crsr_position == 'A')*LCD_crsr_position = '9';
        else *LCD_crsr_position = *LCD_crsr_position - 1;
}

/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
void preEnterNum(char offset)//для звичайних чисел
{
    manage = &map.menu_ptr[offset];
    print(LCD_str);//виведення в рядок модифікації
    LCD_str[COLUMN] = 0;//про всяк випадок захист від переповнення

    //очистити рядок для модифікацфї
    int i;
    i = COLUMN;
    do{LCD_input_str[i]=0;}
    while(--i);

    strcpy(LCD_input_str,LCD_str);//копія в рядок модифікації
    if(LCD_init_crsr_position()){
        menu_status.fetch = 2;
        menu_status.flash = 1;//позицій моргатимуть
    }
    else menu_status.fetch = 0;//невдача
}

/*************************************************************************************************\
\\  ОБРОБКА BOOL - ТИПУ
\*************************************************************************************************/
/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
void UP_BOOL(){
    if(*LCD_crsr_position == 'O')memcpy(LCD_crsr_position," ON",3);
    else memcpy(LCD_crsr_position,"OFF",3);
}
/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/

void preEnterBool(char offset)//для BOOL
{
    char *ptr;
    manage = &map.menu_ptr[offset];
    print(LCD_str);//виведення в рядок модифікації
    LCD_str[31] = 0;//про всяк випадок захист від переповнення

    //очистити рядок для модифікацфї
    int i;
    i = COLUMN;
    do{LCD_input_str[i]=0;}
    while(--i);

    strcpy(LCD_input_str,LCD_str);//копія в рядок модифікації

    ptr = strrchr(map.menu_ptr[offset].format[LNG],'%');//пошук позиції розміщення напису в рядку-форматі
    if(ptr){
        LCD_crsr_position = (char*)(ptr - map.menu_ptr[offset].format[LNG] + LCD_input_str);//ОБЧИСЛЕННЯ позиції
        menu_status.flash = 3;//позицій моргатимуть
        menu_status.fetch = 2;
    }
    else menu_status.fetch = 0;//невдача
}
/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/

void preEnterParity(char offset)//для parity
{
    char *ptr;
    manage = &map.menu_ptr[offset];
    print(LCD_str);//виведення в рядок модифікації
    LCD_str[21] = 0;//про всяк випадок захист від переповнення

    //очистити рядок для модифікацфї
    int i;
    i = COLUMN;
    do{LCD_input_str[i]=0;}
    while(--i);

    strcpy(LCD_input_str,LCD_str);//копія в рядок модифікації

    ptr = strrchr(map.menu_ptr[offset].format[LNG],'%');//пошук позиції розміщення напису в рядку-форматі
    if(ptr){
        LCD_crsr_position = (char*)(ptr - map.menu_ptr[offset].format[LNG] + LCD_input_str);//ОБЧИСЛЕННЯ позиції
        menu_status.flash = 6;//позицій моргатимуть
        menu_status.fetch = 2;
    }
    else menu_status.fetch = 0;//невдача
}
/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
void UP_PARITY(){
    char tmp;
    const char *ptr;

    switch(*(LCD_crsr_position)){
      default:
      case '0': tmp = 0;break;
      case '1': tmp = 1;break;
      case '2': tmp = 2;break;
      case '3': tmp = 3;break;
    }
    if(--tmp > 3)tmp = 3;
    ptr = parPt[tmp][LNG];
    memcpy(LCD_crsr_position,ptr,6);
}
/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
void DOWN_PARITY(){
    char tmp;
    const char *ptr;

    switch(*(LCD_crsr_position)){
      default:
      case '0': tmp = 0;break;
      case '1': tmp = 1;break;
      case '2': tmp = 2;break;
      case '3': tmp = 3;break;
    }
    if(++tmp > 3)tmp = 0;
    ptr = parPt[tmp][LNG];
    memcpy(LCD_crsr_position,ptr,6);
}
/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/

char postEnterBitrate0()//Для швидкості уарт0
{
    manage = &map.menu_ptr[menu_status.input_row];
    scan(LCD_input_str);
    //uart0_bitrate(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart0_change = 1;
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
char postEnter_word_length0()
{
    manage = &map.menu_ptr[menu_status.input_row];
    scan(LCD_input_str);
    //uart0_bitrate(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart0_change = 1;
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
char postEnter_stop_bit0()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    //uart0_bitrate(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart0_change = 1;
    return 1;
}

/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
char postEnter_parity_enable0()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    //uart0_parity_enable(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart0_change = 1;
    return 1;
}

/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
char postEnter_parity_select0()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    //uart0_parity_select(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart0_change = 1;
    return 1;
}



/*------------------------------------------------------------------------------------------------/
/   функція на підміну     UART1
/------------------------------------------------------------------------------------------------*/

char postEnterBitrate1()//Для швидкості уарт1
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    //uart1_bitrate(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart1_change = 1;
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
char postEnter_word_length1()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    //uart1_bitrate(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart1_change = 1;
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
char postEnter_stop_bit1()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    //uart1_bitrate(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart1_change = 1;
    return 1;
}

/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
char postEnter_parity_enable1()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    //uart1_parity_enable(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart1_change = 1;
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
char postEnter_parity_select1()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    //uart1_parity_select(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart1_change = 1;
    return 1;
}

/*-----------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------/
/   функція на підміну     UART1
/------------------------------------------------------------------------------------------------*/
/******************************************************************************/

void preEnterBaud(char offset)//для BOOL
{
    char *ptr;
    manage = &map.menu_ptr[offset];
    print(LCD_str);//виведення в рядок модифікації
    LCD_str[31] = 0;//про всяк випадок захист від переповнення

    //очистити рядок для модифікацфї
    int i;
    i = COLUMN;
    do{LCD_input_str[i]=0;}
    while(--i);

    strcpy(LCD_input_str,LCD_str);//копія в рядок модифікації

    ptr = strrchr(map.menu_ptr[offset].format[LNG],'%');//пошук позиції розміщення напису в рядку-форматі
    if(ptr){
        LCD_crsr_position = (char*)(ptr - map.menu_ptr[offset].format[LNG] + LCD_input_str);//ОБЧИСЛЕННЯ позиції
        menu_status.flash = 8;//позицій моргатимуть
        menu_status.fetch = 2;
    }
    else menu_status.fetch = 0;//невдача
}
/******************************************************************************/
void UP_Baud(){
    char tmp;
    char *ptr;

    switch(*(LCD_crsr_position)){
      default:
      case '1': tmp = 0;break;
      case '2': tmp = 1;break;
      case '3': tmp = 2;break;
      case '4': tmp = 3;break;
      case '5': tmp = 4;break;
      case '6': tmp = 5;break;
      case '7': tmp = 6;break;
    }
    if(--tmp > 6)tmp = 6;
    switch(tmp){
      default:
      case 0: ptr = "1:  2400";break;
      case 1: ptr = "2:  4800";break;
      case 2: ptr = "3:  9600";break;
      case 3: ptr = "4: 19200";break;
      case 4: ptr = "5: 38400";break;
      case 5: ptr = "6: 57600";break;
      case 6: ptr = "7:115200";break;
    }
    memcpy(LCD_crsr_position,ptr,8);
}
/******************************************************************************/
void DOWN_Baud(){
    char tmp;
    char *ptr;

    switch(*(LCD_crsr_position)){
      default:
      case '1': tmp = 0;break;
      case '2': tmp = 1;break;
      case '3': tmp = 2;break;
      case '4': tmp = 3;break;
      case '5': tmp = 4;break;
      case '6': tmp = 5;break;
      case '7': tmp = 6;break;
    }
    if(++tmp > 6)tmp = 0;
    switch(tmp){
      default:
      case 0: ptr = "1:  2400";break;
      case 1: ptr = "2:  4800";break;
      case 2: ptr = "3:  9600";break;
      case 3: ptr = "4: 19200";break;
      case 4: ptr = "5: 38400";break;
      case 5: ptr = "6: 57600";break;
      case 6: ptr = "7:115200";break;
    }
    memcpy(LCD_crsr_position,ptr,8);
}
/******************************************************************************/
void preEnterSIGP(char offset)//для BOOL
{
    char *ptr;
    manage = &map.menu_ptr[offset];
    print(LCD_str);//виведення в рядок модифікації
    LCD_str[31] = 0;//про всяк випадок захист від переповнення

    //очистити рядок для модифікацфї
    int i;
    i = COLUMN;
    do{LCD_input_str[i]=0;}
    while(--i);

    strcpy(LCD_input_str,LCD_str);//копія в рядок модифікації

    ptr = strrchr(map.menu_ptr[offset].format[LNG],'%');//пошук позиції розміщення напису в рядку-форматі
    if(ptr){
        LCD_crsr_position = (char*)(ptr - map.menu_ptr[offset].format[LNG] + LCD_input_str);//ОБЧИСЛЕННЯ позиції
        menu_status.flash = 11;//позицій моргатимуть
        menu_status.fetch = 2;
    }
    else menu_status.fetch = 0;//невдача
}
/******************************************************************************/
void UP_SIGP(){
    char tmp;
    char *ptr;

    switch(*(LCD_crsr_position)){
      default:
      case '0': tmp = 0;break;
      case '1': tmp = 1;break;
      case '2': tmp = 2;break;
    }
    if(--tmp > 2)tmp = 2;
    switch(tmp){
    default:
    case 0: ptr = "0:   12V UI";break;
    case 1: ptr = "1:12V+HV UI";break;
    case 2: ptr = "2: PIEZO UI";break;
    }
    memcpy(LCD_crsr_position,ptr,11);
}
/******************************************************************************/
void DOWN_SIGP(){
    char tmp;
    char *ptr;

    switch(*(LCD_crsr_position)){
      default:
      case '0': tmp = 0;break;
      case '1': tmp = 1;break;
      case '2': tmp = 2;break;
    }
    if(++tmp > 2)tmp = 0;
    switch(tmp){
    default:
    case 0: ptr = "0:   12V UI";break;
    case 1: ptr = "1:12V+HV UI";break;
    case 2: ptr = "2: PIEZO UI";break;
    }
    memcpy(LCD_crsr_position,ptr,11);
}
/******************************************************************************/




















/*------------------------------------------------------------------------------------------------/
/   функція на підміну
/------------------------------------------------------------------------------------------------*/
char postEnter_new_snap_time()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    //arch_time = new_time(N.snap_time);
    return 1;
}

/*------------------------------------------------------------------------------------------------/
/   функція на підміну  ПАРОЛЬ  заміна
/------------------------------------------------------------------------------------------------*/
char postEnter_pass_replace()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    if(password_test == 120978 ||password_test == N.password){
        //menu_push();//закинути в стек поточну позицію
        map.menu_size = map.menu_ptr[menu_status.input_row].size;
        map.menu_ptr = (MENU_MAN*)((int)map.menu_ptr[menu_status.input_row].next);//відсікти знаковий біт
        map.lcd_flash_row = 0;
        map.sublevel = 0;

    }
    password_test = 0;

    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   функція на підміну  ПАРОЛЬ
/------------------------------------------------------------------------------------------------*/
char postEnter_pass_dialog()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    if(password_test == N.password){
        //menu_push();//закинути в стек поточну позицію
        map.menu_size = map.menu_ptr[menu_status.input_row].size;
        map.menu_ptr = (MENU_MAN*)((int)map.menu_ptr[menu_status.input_row].next);
        map.lcd_flash_row = 0;
        map.sublevel = 0;

    }
    password_test = 0;
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/
/------------------------------------------------------------------------------------------------*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/

// тут починаються функції відображення архіву на екрані
/*===============================================================================================*/

/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/*####################################################################################################*/
/*####################################################################################################*/
#include "text_menu.h"
/*####################################################################################################*/
/*####################################################################################################*/
/*-----------------------------------------------------------------------------------------------*/
// розкрутка меню на дисплей
/*-----------------------------------------------------------------------------------------------*/
char last_i;//щоб упевнитись що натиснутий тойже рядок
char press_cntr;//Лічильник натиснутої кнопки
char lcdF;//ознака для поморгування

void menu(void)
{
    unsigned char i,offset;//,key_1;
    
    if(menu_status.menu == true){//якщо режим навігації меню

        if(menu_status.alt){
            menu_status.alt();
            //key_1 = kays_pop();
            return;
        }

        if(time_10mS >= N.lcd.ON[0]) lcdF = 1;
        else lcdF = 0;

        for(i=0;i<LINE;i++){

            if(map.sublevel+i > map.menu_size)offset = map.sublevel+i - map.menu_size-1;
            else offset = map.sublevel+i;
            //якщо в меню меньше ніж рядків на екрані
            if(i > map.menu_size){
                LCD_put_str(i,0,"                    ");
                continue;
            }

            if(map.menu_ptr[offset].pre_funct == 0 || map.menu_ptr[offset].pre_funct()==0){
                manage = &map.menu_ptr[offset];
                print(LCD_str);
            }
            else map.menu_ptr[offset].pre_funct();

            if((menu_status.fetch == 2) && (offset == menu_status.input_row)){//якщо йде процеc модифікації
                //очистити рядок
                char width;
                char *ptr;
                width = COLUMN;
                ptr = LCD_input_str_;
                do{
                    *ptr = ' ';
                    ptr++;
                }while(--width);
                ptr = '\0';

                //поморгування
                strcpy(LCD_input_str_,LCD_input_str);//доправити захоплений рядок на підміну
                if(lcdF){
                    char *ptr_flash = LCD_crsr_position - &LCD_input_str[0] + &LCD_input_str_[0];//початкова позиція поморгування
                    char pos_flash = menu_status.flash;//кількість позицій що моргають
                    do *ptr_flash++ = ' ';
                    while(--pos_flash);
                    //LCD_highlight(i, 0, LCD_input_str_);
                    //LCD_highlight_crsr(i,0,LCD_input_str_);
                }
                LCD_put_str(i,0,LCD_input_str_);
            }
            else{
                //виведення одного рядка на дисплей
                if(i == map.lcd_flash_row){
                    if(lcdF)LCD_put_str(i,0,"                    ");
                    else LCD_put_str(i,0,LCD_str);
                }else LCD_put_str(i,0,LCD_str);
            }
         }


        //  обробка клавіш
        i = kays_pop();
        switch(i){
            case menu_key:  menu_KEY(); break;
          /*
            case inc:        break;
            case dec:        break;
            */
            case enter:     enter_key(); break;
            case enc_ent:   encoder_key(); break;
            case right:     right_key(); break;
            case up:        up_key(); break;
            case down:      down_key(); break;
            case left:      left_key(); break;
          default: if(i)default_key(i);break;
        }

    }
    else{//головний екран
      i = kays_pop();
        if( i == menu_key || i == enter || i == enc_ent)menu_status.menu = true;//прейти в режим навігації меню
        else{

          int fract1, fract2;
          
          fract1 = Tclamps % 10; if (fract1 < 0) fract1 = -fract1;
          fract2 = Theatsink % 10; if (fract2 < 0) fract2 = -fract2;
          
          if(N.rtc_on){
            RTC_GetTime(&Time);
            //sprintf(LCD_str," %03X %04d   %02d:%02d:%02d",adc_code,rpm,Time.hour,Time.minute,Time.second);//h:m:s
            sprintf(LCD_str,"            %02d:%02d:%02d", Time.hour,Time.minute,Time.second);//h:m:s
          }else  sprintf(LCD_str,"tyr:%3d.%01d\337cld:%3d.%01d\337", Theatsink/10, fract2, Tclamps/10, fract1);//
          
          LCD_put_str(0,0,LCD_str);
          
          fract1 = rtdT0 % 10; if (fract1 < 0) fract1 = -fract1;
          fract2 = rtdT1 % 10; if (fract2 < 0) fract2 = -fract2;
          
          sprintf(LCD_str,"dry:%3d.%01d\337wet:%3d.%01d\337", rtdT0/10, fract1, rtdT1/10, fract2);
          LCD_put_str(1,0,LCD_str);
          
          sprintf(LCD_str,"             RH:%3d%%", Ht );
          LCD_put_str(2,0,LCD_str);
          
          sprintf(LCD_str,"in:%3d.%01dV out:%3d.%01dV", RSCR.A/10, RSCR.A%10, R380.A/10, R380.A%10);
          LCD_put_str(3,0,LCD_str);
           


/*
          for(i=0;i<7;i++){
            cap.fr_td[i] = cap.fr_d[i] * 100/589824;//в 0.1 мС
          }


          sprintf(LCD_str,"%4d %4d %4d %4d ", cap.fr_td[0], cap.fr_td[1], cap.fr_td[2], cap.fr_td[3]);
          LCD_put_str(2,0,LCD_str);
          sprintf(LCD_str,"%4d %4d %4d %4d ", cap.fr_td[4], cap.fr_td[5], cap.fr_td[6], cap.T * 100/589824);
          LCD_put_str(3,0,LCD_str);

          //if(cap.arr[1] > cap.arr[0]) cap.tm[0] = (cap.arr[1] - cap.arr[0])* 10000/589824;//в 0.1 мС
          //else cap.tm[0] = (cap.arr[0] - cap.arr[1])* 10000/589824;//в 0.1 мС
          cap.tm[0] = (long long)cap.ti[0] * 10000/589824;//в 0.1 мС
          //cap.tm[1] = (long long)cap.ti[1] * 10000/589824;//в 0.1 мС
          //if(cap.tm[0]>1000)cap.tm[1] = cap.tm[0];
          //unsigned int rpm;
          //rpm = (long long)cap.tm[0] * cap.L/cap.h;//час одного оберту
          //rpm = 60000000 / rpm;

          sprintf(LCD_str,"%4d.%03d  %4d %4d ", cap.tm[0]/1000,cap.tm[0]%1000, (int)((long long)cap.Th[0] * 100/589824), (int)((long long)cap.Th[1] * 100/589824));
          LCD_put_str(0,0,LCD_str);

          sprintf(LCD_str,"%4d      %4d %4d ", cap.rpm, (int)((long long)cap.Th[2] * 100/589824), (int)((long long)cap.Th[3] * 100/589824) );
          LCD_put_str(1,0,LCD_str);
*/
            /*
            for(i=0;i<4;i++){
              sprintf(LCD_str+i*4," %03X ",adc_a.adc[i]);
            }
            LCD_put_str(3,0,LCD_str);
            */

        }
    }

}

/*####################################################################################################*/
/*####################################################################################################*/
/*####################################################################################################*/

void menu_init()
{
    char *ptr;
    char i;

    i = sizeof(menu_stack);
    ptr = (char*)&menu_stack;
    do{*ptr = 0;ptr++;}while(--i);

    menu_status.input_row = 0;
    menu_status.fetch = 0;
    menu_status.flash = 0;
    menu_status.menu = 0;

    map.sublevel = 0;
    map.lcd_flash_row = 0;
    map.menu_ptr = &menu0[0];
    map.menu_size = sizeof(menu0)/sizeof(MENU_MAN)-1;
}
/*####################################################################################################*/
/*####################################################################################################*/
/*####################################################################################################*/



