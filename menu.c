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

const MENU_MAN *manage;//�� ��������� ��������� ����
int LNG;//���� 0-rus 1-ua

RTF general_purpose_flags;
int Ti_flag;//������ ���� ��������� ��������
int wd_count;
int menu_ident;//����� ������, �� �������� ����������� �����

//�������
const char *parPt[4][2] ={
  {"0 ODD ","0�e���"},
  {"1 EVEN","1����."},
  {"2 SET1","2���.1"},
  {"3 SET0","3���.0"}};

//��������� � �����
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
        if(manage->fract){              //���� � ����� ���� ����
            dest += count;              //������� �� ����� ����� �� ����
            do{
                ch = *dest;
                *(dest+1) = ch;         //������������ ������� ������
                dest--;                 //�� ���� ������ ������
                if(isdigit(ch)&&!flag){ //���� �����
                    flag = true;        //�������� ������� ����� � ����
                    count = manage->fract;//������� ��� ����� �����������
                }
            }while(--count);            //����� ����� ����� � ����
            *(dest+1) = '.';            //������� ������
            if(*dest == ' ')*dest = '0';//��� ����� �� ������������� 0 ���� ������� ��������� ���� ������ ������
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
          case 0: ptr = "0 - ����������� � ��� ������";break;
          case 1: ptr = "1 - ����������� � ��� ������";break;
        }
        sprintf(dest,manage->format[LNG],ptr);
        break;

      case SUB_123:
        switch(*manage->number){
          default:
          case 0: ptr = "0 - mA";break;
          case 1: ptr = "1 - ���(K)";break;
          case 2: ptr = "2 - ���(B)";break;
          case 3: ptr = "3 - ���100";break;
          case 4: ptr = "4 - ���100";break;
        }
        sprintf(dest,manage->format[LNG],ptr);
        break;

      case SUB_comp:
        switch(*manage->number){
          default:
          case 0: ptr = "0 - ���������������� ����";break;
          case 1: ptr = "1 - ���������������� �";break;
        }
        sprintf(dest,manage->format[LNG],ptr);
        break;

      case SUB_sts:
        switch(*manage->number){
          default:
          case 0: ptr = "��������� �������:        �����";break;
          case 1: ptr = "��������� �������:        �����";break;
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


//� ����� � �����
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
        ptr = strchr(manage->format[LNG],'%');//����� ������� ��������� ����� � �����-������
        if(ptr){
            ptr = (char*)(ptr - manage->format[LNG] + src);//����������� �������� � ����� � ����� ������
            temp = atoi(ptr);//����������� � ����� � ����� "����" �������
            if(*ptr == '-')sign = 1;
            if(manage->fract){
                ptr = strrchr(ptr,'.')+1;//�� ������� �������
                char ii = manage->fract;
                do temp *= 10; while(--ii);//���� �� ������� �������� �����
                temp += sign? -atoi(ptr) : atoi(ptr);//������ "�������" �������
            }
            temp = (temp<manage->min)? manage->min:((temp>manage->max)? manage->max:temp);
            *manage->number =  temp;
        }
        break;

      case SUB_BOOL://BOOL
        ptr = strrchr(manage->format[LNG],'%');//����� ������� ��������� ����� � �����-������
        if(ptr){
            ptr = (char*)(ptr - manage->format[LNG] + src);//����������� �������� � ����� � ����� ������
            if(*ptr == 'O')*manage->number = 0;//����� OFF
            else *manage->number = 1;//����� ON
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
        //arch_time = new_time(N.snap_time);//��������� ��� ������ � �����
        return;

      case SUB_DATE:

        //��������� ��������� ���� � ����������
        RTC_GetDate(&pdate);

        ptr = strchr(src,'.');
        date_ = atoi(ptr-2);
        if((date_ <= 31)||(date_ > 0))pdate.day = date_;

        month_ = atoi(ptr+1);
        if((month_ <= 12)||(month_ > 0))pdate.month = month_;

        ptr = strchr(ptr+1,'.');
        pdate.year = atoi(ptr+1);

        RTC_SetDate(&pdate);
        //arch_time = new_time(N.snap_time);//��������� ��� ������ � �����
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
        ptr = strchr(manage->format[LNG],'%');//����� ������� ��������� ����� � �����-������
        if(ptr){
            ptr = (char*)(ptr - manage->format[LNG] + src);//����������� �������� � ����� � ����� ������
            do{ptr++;}while(*ptr >='0');//����� ���� �����
            ptr--; // ����� � �����
            char hex_iter,ch;
            hex_iter = 0;
            //����������� � hex � �����
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

    //���� �������� ����������� ���������� ��������� ��������� �� �� ���������� ����� � �������
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

int password_test;//��� ��������� ������
/*************************************************************************************************\
* ���������� ����� ��� ���������� ����� �� ����
\*************************************************************************************************/
struct cell{
    char sublevel;      //������ ����� �� �����
    char lcd_flash_row; //�����, �� �����
    const MENU_MAN *menu_ptr;//������
    char menu_size;     //�����
};
struct cell map;//��������� ��������� ����

#define menu_stack_size 16
struct{
    struct cell one_cell[menu_stack_size];
    char index;
}menu_stack;

struct {//��� ���� ��������� ���� �������� ���� ����
    char input_row;    //����� �� ���������� �����������
    char fetch; //0 - ��������� ����� 1- ��������� �������� ����� 2- �������� ���������� �����
    char flash;//������� ������� �� ��������
    bool menu;//���� 1 �� �������� �� ����, ���� 0 �� �������� �����
    void (*alt)();//����  �� ������� ���� �� ������� ��������������� �������������� �������� ����� �� ���� � ��������
    const MENU_MAN *object;//���� �� ������ �����������
}menu_status;

void cpy_xx(char *xptr_dest, char *xptr_src, unsigned short count)//�����
{
    do{*xptr_dest++ = *xptr_src++;}
    while(--count);
}

void clr_xx(char *xptr_dest, unsigned short count)//������� ������
{
    do{*xptr_dest++ = 0;}
    while(--count);
}

unsigned char menu_push()//������� ������� ��� �������� �������� � ���� � 0 ���� ����� �����
{
    if(menu_stack.index == menu_stack_size)return 0;
    cpy_xx((char*)&menu_stack.one_cell[menu_stack.index++], (char*)&map, sizeof(map));

    return 1;
}

unsigned char menu_pop()//������� 1 ��� �������� ������� � 0 ��� ���������� �����
{
    if(menu_stack.index == 0)return 0;
    cpy_xx((char*)&map, (char*)&menu_stack.one_cell[--menu_stack.index],sizeof(map));
    clr_xx((char*)&menu_stack.one_cell[menu_stack.index],sizeof(map));//�������� �����
    return 1;
}


typedef struct{
char no_refresh :1;
char input      :1;//�����������
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

    if(menu_status.fetch == 0){//���� ���� � ����� ��������
        //����� ����� ������� ���� 0..map.menu_size-1
        if((i = map.sublevel+map.lcd_flash_row) > map.menu_size)i -= map.menu_size+1;

        if(map.menu_ptr[i].next && map.menu_ptr[i].change !=128){//���� � �������� ���� ������ ���� � ���� �� �� ������ �� �������� �� ��
            if(map.menu_ptr[i].pre_enter)map.menu_ptr[i].pre_enter(0);//�������� ������ ������� ���������� ��. �����

            menu_push();//�������� � ���� ������� �������
            map.menu_size = map.menu_ptr[i].size;
            map.menu_ptr = map.menu_ptr[i].next;
            map.lcd_flash_row = 0;
            map.sublevel = 0;
            return;
        }
        else{
            /*
            if(map.menu_ptr[menu_status.input_row].pre_enter)
                map.menu_ptr[menu_status.input_row].pre_enter();//�������� ������ ������� ���������� ��. �����
            */
            if(map.menu_ptr[i].change){//�������� ����� ��������
                menu_status.input_row = i;//����� �� ���� ���������� ���� �����
                menu_status.fetch = 1;//��������� �������� ����� ��� �������� �����������

                menu_status.object = map.menu_ptr + menu_status.input_row;//��'��� �� ����Բ�Ӫ����

                void(*ptr)(char) = (void(*)(char)) map.menu_ptr[menu_status.input_row].pre_enter;
                if(ptr)ptr(menu_status.input_row);//�������� ������ ������� ���������� ��. �����
            }
        }
    }
    else{//���� ���� � ����� �����������
        if((map.menu_ptr[menu_status.input_row].post_enter == 0) || (map.menu_ptr[menu_status.input_row].post_enter()==0)){
            manage = &map.menu_ptr[menu_status.input_row];
            scan(LCD_input_str);
        }
        menu_status.fetch = 0;//���������� � ����� ��������
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
        //��� ������������ ������
        if(map.lcd_flash_row > 0)map.lcd_flash_row--;
        else{
            //���� � ���� ����� ������ �� ������
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
        //��� ������������ ������
        if(map.lcd_flash_row < LINE-1){
            if(map.lcd_flash_row < map.menu_size)map.lcd_flash_row++;
            else map.lcd_flash_row = 0;
        }
        else{
            //���� � ���� ����� ������ �� �� �����
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
        menu_status.menu = false;//������� � ���� �� ������� �����
        map.lcd_flash_row = 0;
        map.sublevel = 0;
      }
    }
    else menu_status.fetch = 0;//������ � ����� ��������
}
/*----------------------------------------------------------------------------*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int view;//��� ������ �����������
int Vbip;// ��� ������������ bip ����� ���
int I1I2;//������������ ��������� ��������� �������� 1 �� 2
int angDl;//��� �� ��������
int incr_;//��� ������� ��������
int PH_pl;//��� ������������ ������ ����������� �����
const char bfly[]={'-','+','*','#'};
/*===============================================================================================*/
/*===============================================================================================*/
char bip1str[24];//��� �� ������� ��� �����
char bip2str[24];
/*===============================================================================================*/
/*===============================================================================================*/

//============================================================================//
// ��� �� ���� Ҳ���� ��� �������������
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


    //���� ��������� ���� �� ����� � ������� ������
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
//��� ���������� ������� ������� ������� 0 start
void zero_key(void)
{
    unsigned char i;

    if(menu_status.fetch == 0){//���� ���� � ����� ��������
        //����� ����� ������� ���� 0..map.menu_size-1
        if((i = map.sublevel+map.lcd_flash_row) > map.menu_size)i -= map.menu_size+1;

        if(map.menu_ptr[i].next && map.menu_ptr[i].change !=128){//���� � �������� ���� ������ ���� � ���� �� �� ������ �� �������� �� ��
            menu_ident = i;//����� ������, �� �������� ����������� �����
            if(map.menu_ptr[i].post_enter)map.menu_ptr[i].post_enter();//�������� ������ ������� ���������� ��. �����
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

    if(menu_status.fetch == 0){//���� ���� � ����� ��������
        //����� ����� ������� ���� 0..map.menu_size-1
        if((i = map.sublevel+map.lcd_flash_row) > map.menu_size)i -= map.menu_size+1;
      //������� �� ��������� ������ �� �������
        if(map.menu_ptr[i].next && map.menu_ptr[i].change !=128){//���� � �������� ���� ������ ���� � ���� �� �� ������ �� �������� �� ��
            menu_ident = i;//����� ������, �� �������� ����������� �����
            if(map.menu_ptr[i].post_enter)map.menu_ptr[i].post_enter();//�������� ������ ������� ���������� ��. �����
            else{
              if(map.menu_ptr[i].pre_enter)map.menu_ptr[i].pre_enter(0);//�������� ������ ������� ���������� ��. �����

              menu_push();//�������� � ���� ������� �������
              map.menu_size = map.menu_ptr[i].size;
              map.menu_ptr = map.menu_ptr[i].next;
              map.lcd_flash_row = 0;
              map.sublevel = 0;
              return;
            }
        }
        else{
            if(map.menu_ptr[i].change){//�������� ����� ��������
                menu_status.input_row = i;//����� �� ���� ���������� ���� �����
                menu_status.fetch = 1;//��������� �������� ����� ��� �������� �����������

                menu_status.object = map.menu_ptr + menu_status.input_row;//��'��� �� ����Բ�Ӫ����

                void(*ptr)(char) = (void(*)(char)) map.menu_ptr[menu_status.input_row].pre_enter;
                if(ptr)ptr(menu_status.input_row);//�������� ������ ������� ���������� ��. �����
            }
        }
    }
    else{//���� ���� � ����� �����������
        if((map.menu_ptr[menu_status.input_row].post_enter == 0) || (map.menu_ptr[menu_status.input_row].post_enter()==0)){
            manage = &map.menu_ptr[menu_status.input_row];
            scan(LCD_input_str);
        }
        menu_status.fetch = 0;//���������� � ����� ��������
    }

}

/*-----------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
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
/   ������� �� �����
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
/   ������� �� �����
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
/   ������� �� �����
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
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void UP_hex()
{
    if(*LCD_crsr_position == 'F')*LCD_crsr_position = '0';
    else
        if(*LCD_crsr_position == '9')*LCD_crsr_position = 'A';
        else *LCD_crsr_position = *LCD_crsr_position + 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void DOWN_hex()
{
    if(*LCD_crsr_position == '0')*LCD_crsr_position = 'F';
    else
        if(*LCD_crsr_position == 'A')*LCD_crsr_position = '9';
        else *LCD_crsr_position = *LCD_crsr_position - 1;
}

/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void preEnterNum(char offset)//��� ��������� �����
{
    manage = &map.menu_ptr[offset];
    print(LCD_str);//��������� � ����� �����������
    LCD_str[COLUMN] = 0;//��� ���� ������� ������ �� ������������

    //�������� ����� ��� �����������
    int i;
    i = COLUMN;
    do{LCD_input_str[i]=0;}
    while(--i);

    strcpy(LCD_input_str,LCD_str);//���� � ����� �����������
    if(LCD_init_crsr_position()){
        menu_status.fetch = 2;
        menu_status.flash = 1;//������� �����������
    }
    else menu_status.fetch = 0;//�������
}

/*************************************************************************************************\
\\  ������� BOOL - ����
\*************************************************************************************************/
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
void UP_BOOL(){
    if(*LCD_crsr_position == 'O')memcpy(LCD_crsr_position," ON",3);
    else memcpy(LCD_crsr_position,"OFF",3);
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/

void preEnterBool(char offset)//��� BOOL
{
    char *ptr;
    manage = &map.menu_ptr[offset];
    print(LCD_str);//��������� � ����� �����������
    LCD_str[31] = 0;//��� ���� ������� ������ �� ������������

    //�������� ����� ��� �����������
    int i;
    i = COLUMN;
    do{LCD_input_str[i]=0;}
    while(--i);

    strcpy(LCD_input_str,LCD_str);//���� � ����� �����������

    ptr = strrchr(map.menu_ptr[offset].format[LNG],'%');//����� ������� ��������� ������ � �����-������
    if(ptr){
        LCD_crsr_position = (char*)(ptr - map.menu_ptr[offset].format[LNG] + LCD_input_str);//���������� �������
        menu_status.flash = 3;//������� �����������
        menu_status.fetch = 2;
    }
    else menu_status.fetch = 0;//�������
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/

void preEnterParity(char offset)//��� parity
{
    char *ptr;
    manage = &map.menu_ptr[offset];
    print(LCD_str);//��������� � ����� �����������
    LCD_str[21] = 0;//��� ���� ������� ������ �� ������������

    //�������� ����� ��� �����������
    int i;
    i = COLUMN;
    do{LCD_input_str[i]=0;}
    while(--i);

    strcpy(LCD_input_str,LCD_str);//���� � ����� �����������

    ptr = strrchr(map.menu_ptr[offset].format[LNG],'%');//����� ������� ��������� ������ � �����-������
    if(ptr){
        LCD_crsr_position = (char*)(ptr - map.menu_ptr[offset].format[LNG] + LCD_input_str);//���������� �������
        menu_status.flash = 6;//������� �����������
        menu_status.fetch = 2;
    }
    else menu_status.fetch = 0;//�������
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
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
/   ������� �� �����
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
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/

char postEnterBitrate0()//��� �������� ����0
{
    manage = &map.menu_ptr[menu_status.input_row];
    scan(LCD_input_str);
    //uart0_bitrate(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart0_change = 1;
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
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
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_stop_bit0()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    //uart0_bitrate(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart0_change = 1;
    return 1;
}

/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_parity_enable0()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    //uart0_parity_enable(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart0_change = 1;
    return 1;
}

/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_parity_select0()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    //uart0_parity_select(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart0_change = 1;
    return 1;
}



/*------------------------------------------------------------------------------------------------/
/   ������� �� �����     UART1
/------------------------------------------------------------------------------------------------*/

char postEnterBitrate1()//��� �������� ����1
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    //uart1_bitrate(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart1_change = 1;
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_word_length1()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    //uart1_bitrate(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart1_change = 1;
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_stop_bit1()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    //uart1_bitrate(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart1_change = 1;
    return 1;
}

/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_parity_enable1()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    //uart1_parity_enable(*map.menu_ptr[menu_status.input_row].number);
    general_purpose_flags.uart1_change = 1;
    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����
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
/   ������� �� �����     UART1
/------------------------------------------------------------------------------------------------*/
/******************************************************************************/

void preEnterBaud(char offset)//��� BOOL
{
    char *ptr;
    manage = &map.menu_ptr[offset];
    print(LCD_str);//��������� � ����� �����������
    LCD_str[31] = 0;//��� ���� ������� ������ �� ������������

    //�������� ����� ��� �����������
    int i;
    i = COLUMN;
    do{LCD_input_str[i]=0;}
    while(--i);

    strcpy(LCD_input_str,LCD_str);//���� � ����� �����������

    ptr = strrchr(map.menu_ptr[offset].format[LNG],'%');//����� ������� ��������� ������ � �����-������
    if(ptr){
        LCD_crsr_position = (char*)(ptr - map.menu_ptr[offset].format[LNG] + LCD_input_str);//���������� �������
        menu_status.flash = 8;//������� �����������
        menu_status.fetch = 2;
    }
    else menu_status.fetch = 0;//�������
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
void preEnterSIGP(char offset)//��� BOOL
{
    char *ptr;
    manage = &map.menu_ptr[offset];
    print(LCD_str);//��������� � ����� �����������
    LCD_str[31] = 0;//��� ���� ������� ������ �� ������������

    //�������� ����� ��� �����������
    int i;
    i = COLUMN;
    do{LCD_input_str[i]=0;}
    while(--i);

    strcpy(LCD_input_str,LCD_str);//���� � ����� �����������

    ptr = strrchr(map.menu_ptr[offset].format[LNG],'%');//����� ������� ��������� ������ � �����-������
    if(ptr){
        LCD_crsr_position = (char*)(ptr - map.menu_ptr[offset].format[LNG] + LCD_input_str);//���������� �������
        menu_status.flash = 11;//������� �����������
        menu_status.fetch = 2;
    }
    else menu_status.fetch = 0;//�������
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
/   ������� �� �����
/------------------------------------------------------------------------------------------------*/
char postEnter_new_snap_time()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    //arch_time = new_time(N.snap_time);
    return 1;
}

/*------------------------------------------------------------------------------------------------/
/   ������� �� �����  ������  �����
/------------------------------------------------------------------------------------------------*/
char postEnter_pass_replace()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    if(password_test == 120978 ||password_test == N.password){
        //menu_push();//�������� � ���� ������� �������
        map.menu_size = map.menu_ptr[menu_status.input_row].size;
        map.menu_ptr = (MENU_MAN*)((int)map.menu_ptr[menu_status.input_row].next);//������ �������� ��
        map.lcd_flash_row = 0;
        map.sublevel = 0;

    }
    password_test = 0;

    return 1;
}
/*------------------------------------------------------------------------------------------------/
/   ������� �� �����  ������
/------------------------------------------------------------------------------------------------*/
char postEnter_pass_dialog()
{
    manage = &map.menu_ptr[menu_status.input_row]; scan(LCD_input_str);
    if(password_test == N.password){
        //menu_push();//�������� � ���� ������� �������
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

// ��� ����������� ������� ����������� ������ �� �����
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
// ��������� ���� �� �������
/*-----------------------------------------------------------------------------------------------*/
char last_i;//��� ���������� �� ���������� ����� �����
char press_cntr;//˳������� ��������� ������
char lcdF;//������ ��� ������������

void menu(void)
{
    unsigned char i,offset;//,key_1;
    
    if(menu_status.menu == true){//���� ����� �������� ����

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
            //���� � ���� ������ �� ����� �� �����
            if(i > map.menu_size){
                LCD_put_str(i,0,"                    ");
                continue;
            }

            if(map.menu_ptr[offset].pre_funct == 0 || map.menu_ptr[offset].pre_funct()==0){
                manage = &map.menu_ptr[offset];
                print(LCD_str);
            }
            else map.menu_ptr[offset].pre_funct();

            if((menu_status.fetch == 2) && (offset == menu_status.input_row)){//���� ��� �����c �����������
                //�������� �����
                char width;
                char *ptr;
                width = COLUMN;
                ptr = LCD_input_str_;
                do{
                    *ptr = ' ';
                    ptr++;
                }while(--width);
                ptr = '\0';

                //������������
                strcpy(LCD_input_str_,LCD_input_str);//��������� ���������� ����� �� �����
                if(lcdF){
                    char *ptr_flash = LCD_crsr_position - &LCD_input_str[0] + &LCD_input_str_[0];//��������� ������� ������������
                    char pos_flash = menu_status.flash;//������� ������� �� ��������
                    do *ptr_flash++ = ' ';
                    while(--pos_flash);
                    //LCD_highlight(i, 0, LCD_input_str_);
                    //LCD_highlight_crsr(i,0,LCD_input_str_);
                }
                LCD_put_str(i,0,LCD_input_str_);
            }
            else{
                //��������� ������ ����� �� �������
                if(i == map.lcd_flash_row){
                    if(lcdF)LCD_put_str(i,0,"                    ");
                    else LCD_put_str(i,0,LCD_str);
                }else LCD_put_str(i,0,LCD_str);
            }
         }


        //  ������� �����
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
    else{//�������� �����
      i = kays_pop();
        if( i == menu_key || i == enter || i == enc_ent)menu_status.menu = true;//������ � ����� �������� ����
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
            cap.fr_td[i] = cap.fr_d[i] * 100/589824;//� 0.1 ��
          }


          sprintf(LCD_str,"%4d %4d %4d %4d ", cap.fr_td[0], cap.fr_td[1], cap.fr_td[2], cap.fr_td[3]);
          LCD_put_str(2,0,LCD_str);
          sprintf(LCD_str,"%4d %4d %4d %4d ", cap.fr_td[4], cap.fr_td[5], cap.fr_td[6], cap.T * 100/589824);
          LCD_put_str(3,0,LCD_str);

          //if(cap.arr[1] > cap.arr[0]) cap.tm[0] = (cap.arr[1] - cap.arr[0])* 10000/589824;//� 0.1 ��
          //else cap.tm[0] = (cap.arr[0] - cap.arr[1])* 10000/589824;//� 0.1 ��
          cap.tm[0] = (long long)cap.ti[0] * 10000/589824;//� 0.1 ��
          //cap.tm[1] = (long long)cap.ti[1] * 10000/589824;//� 0.1 ��
          //if(cap.tm[0]>1000)cap.tm[1] = cap.tm[0];
          //unsigned int rpm;
          //rpm = (long long)cap.tm[0] * cap.L/cap.h;//��� ������ ������
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



