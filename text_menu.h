

extern PARAMETRS N;


//---Ĳ���� ������
#define pass(NAME)\
    const MENU_MAN PASS_ ## NAME ={SUB_NUM,1,"������ ?     %06d ",&password_test,(int)& ## NAME ## _PASS_ |0x80000000,0x7fffffff,0,0,sizeof( NAME ## _PASS_)/sizeof(SUB)-1,0,preEnterNum,postEnter_pass_dialog,UP,DOWN,LEFT,RIGHT};\
    SUB PASS_ ## NAME ## _= SUB(&PASS_ ## NAME);
//---ʲ���� Ĳ���� ������



/*--------------------------------------------------------------------------------------------------------------------/
/   ���в�
/--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------/
/   ���o��
/--------------------------------------------------------------------------------------------------------------------*/
/*
const MENU_MAN PASS_G[]={
    {SUB_NUM,1,"�����     %06d",&N.password,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_NUM,1,"WATCHDOG  %06d",(int*)&wd_count,0,0x7fffffff,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT}
};

const MENU_MAN PASS[]={                                                                 //�������� �i� ��� �������� ������ ��������� �����
    {SUB_NUM,128,"������    %06d",&password_test,0,0x7fffffff,0,(void*)&PASS_G,sizeof(PASS_G)/sizeof(MENU_MAN)-1,0,preEnterNum,postEnter_pass_replace,UP,DOWN,LEFT,RIGHT}
};
*/
/*--------------------------------------------------------------------------------------------------------------------/
/   ����� � ����
/--------------------------------------------------------------------------------------------------------------------*/

const MENU_MAN TIME[]={
    //{SUB_NUM,1,"����� %06d      ",&N.snap_time,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_TIME,1,"%#02d:%#02d:%#02d            ","%#02d:%#02d:%#02d            ",0,0,0,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_DATE,1,"%#02d.%#02d.%#04d          ","%#02d.%#02d.%#04d          ",0,0,0,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_DAY,0,"%s","%s",0,0,0,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
    {SUB_BOOL,  1,"Clock enable    %s ","������� �����   %s ",    &N.rtc_on,0,0,      0,0,0,0,preEnterBool, 0, UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
    //{SUB_NUM,128,"������    %06d",&password_test,0,0x7fffffff,0,(void*)&TIME_PASS,sizeof(TIME_PASS)/sizeof(MENU_MAN)-1,0,preEnterNum,postEnter_pass_dialog,UP,DOWN,LEFT,RIGHT},
};

/*--------------------------------------------------------------------------------------------------------------------/
/   UARTs
/--------------------------------------------------------------------------------------------------------------------*/
#define uart_macro(n)\
    {SUB_Baud,  1,"Baud rate   %s",         "�����i���   %s",     &N.UART ## n.bitrate,0,6,     0,0,0,0,preEnterBaud,    0,          UP_Baud, DOWN_Baud, UP_Baud, DOWN_Baud},\
    {SUB_NUM,   1,"Word lenght     %01d   ","�i�� �����      %01d   ", &N.UART ## n.word_length,5,8, 0,0,0,0,preEnterNum,    postEnter_word_length ## n,     UP, DOWN, LEFT, RIGHT},\
    {SUB_NUM,   1,"Stop bits       %01d   ","���� �i��       %01d   ", &N.UART ## n.stop_bit,1,2, 0,0,0,0,preEnterNum,    postEnter_stop_bit ## n,        UP, DOWN, LEFT, RIGHT},\
    {SUB_BOOL,  1,"Parity enable   %s ",    "�i� �������i    %s ",    &N.UART ## n.parity_enable,0,0,      0,0,0,0,preEnterBool,   postEnter_parity_enable ## n,   UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},\
    {SUB_PARITY,1,"Parity select %s",       "��� �������i  %s",      &N.UART ## n.parity_select,0,0,      0,0,0,0,preEnterParity, postEnter_parity_select ## n,   UP_PARITY, DOWN_PARITY, UP_PARITY, DOWN_PARITY},\
    {SUB_NUM,   1,"T response    %03d mS",  "T �i����i�i   %03d ��",  &N.UART ## n.wait_respond,1,999,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},\
    {SUB_NUM,   1,"Pause         %03d mS",  "�����         %03d ��",  &N.UART ## n.timeout,1,999,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},\
    {SUB_HEX,   1,"Address        %02X   ", "������         %02X   ", &N.UART ## n.simplex,1,0xff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},\
    {SUB_BOOL,  1,"WRITE FIRMWARE  %s ",    "����� ����. ��  %s ", &rewrite_esw,0,1,0,0,0,0,preEnterBool,   0,   UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},\
    {SUB_NUM,   1,"FIRMWARE VERSION    ",   "    ����I� �.�.     ", (int*) &build_num,0,6,     0,0,0,0,version_fun,    0,          0, 0, 0, 0},\
    //{SUB_HEX,  0,"������ ��           ", (int*)&build_num,0,1,0,0,0,0,version_fun,0,0,0,0,0},\
    //{SUB_BOOL,  1,"C�������        %s ",    &N.UART ## n.simplex,0,0,      0,0,0,0,preEnterBool,   0,   UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},\

const MENU_MAN UART0[]={uart_macro(0)}; /*const MENU_MAN uart0[]={uart_macroS(0)};*///UART0
//const MENU_MAN UART1[]={uart_macro(1)}; /*const MENU_MAN uart1[]={uart1_macroS(1)};*///UART1

/*
//   ��������� ��� UART 0,1
const MENU_MAN RS485[]={
    {SUB_NUM,1,"RS422 ��������. ",0,0,0,0,(void*)&UART0,sizeof(UART0)/sizeof(MENU_MAN)-2,0,0,0,0,0,0,0},
    {SUB_NUM,1,"RS485 �������   ",0,0,0,0,(void*)&UART1,sizeof(UART1)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0}
};
*/
/*####################################################################################################*/
/*##############################    ���������   ######################################################*/
/*####################################################################################################*/
/*####################################################################################################*/
const MENU_MAN LCD_T[]={
  {SUB_NUM,1,"Fleshing period  %02d ","���i�� ��������. %02d ",&N.lcd.T[0],30,99,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"Time of viewing  %02d ","��� �i���������� %02d ",&N.lcd.ON[0],20,90,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"Contrast        %03d ","�����a��        %03d ",&N.lcd.contrast,0,100,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"Backlight       %03d ","�i���i���       %03d ",&N.lcd.bright,0,100,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_BOOL,  1,"Reinit.         %s ","������.i�i�i��. %s ",&N.lcd.reanim,0,1,      0,0,0,0,preEnterBool, 0, UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
  //{SUB_BOOL,  1,"����. ��������� %s ","�i���.��������� %s ",&N.LCD_OD,0,1,      0,0,0,0,preEnterBool, 0, UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},

//{SUB_NUM,1,"Period 2         %02d ",&N.lcd.T[1],30,90,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  //{SUB_NUM,1,"T ON 2           %02d ",&N.lcd.ON[1],20,90,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  //{SUB_NUM,0,"      ����� <<<     ",0,0,0,0,(void*)1,1,0,0,(char(*)())menu_KEY,0,0,0,0},
};
/*####################################################################################################*/
/*####################################################################################################*/
/*####################################################################################################*/
/*####################################################################################################*/
#include"textAIU.h"
#include"textProt.h"
#include"textSCRs.h"
#include"textPIDReg.h"
#include"textRecipe.h"
/*####################################################################################################*/
/*####################################################################################################*/
const MENU_MAN TYR_F[]={ 
  {SUB_HEX,   1,"scr range      %04X ", "", &N.SCRRange,0x7fff,0xFFFF,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},
  //{SUB_NUM,   1,"INPUT        %05d ","",   &TyrReg.inlet,0,10000,2,0,0,0,preEnterNum, 0, UP, DOWN, LEFT, RIGHT},
  //{SUB_HEX,   1,"OUTPUT HEX     %04X ", "", &TyrReg.exhaust,0,0xFFFF,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},
  //{SUB_NUM,   1,"OUTPUT DEC    %05d ","",   &TyrReg.exhaust,0,0xffff,0,0,0,0,preEnterNum, 0, UP, DOWN, LEFT, RIGHT},
  {SUB_NUM,   1,"SSPO EN        %01d   ","",&TyrReg.enable,0,9,0,0,0,0,preEnterNum, 0, UP, DOWN, LEFT, RIGHT},
  {SUB_HEX,   1,"SSPO RD        %04X ", "", &TyrReg.read,0,0xFFFF,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_NUM,   1,"scr0         %05d ","",   &TyrReg.SCR0,0,10000,2,0,0,0,preEnterNum, 0, UP, DOWN, LEFT, RIGHT},
  {SUB_NUM,   1,"scr1         %05d ","",   &TyrReg.SCR1,0,10000,2,0,0,0,preEnterNum, 0, UP, DOWN, LEFT, RIGHT},
  {SUB_NUM,   1,"scr2         %05d ","",   &TyrReg.SCR2,0,10000,2,0,0,0,preEnterNum, 0, UP, DOWN, LEFT, RIGHT},
};
/*####################################################################################################*/
const MENU_MAN Params[]={
    {SUB_NUM,0,"10 MEASUREMENTS     ","",0,0,0,0,(void*)&AIU_SUMM,sizeof(AIU_SUMM)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
    {SUB_NUM,0,"11 RS-485 MODBUS    ","",0,0,0,0,(void*)&UART0,sizeof(UART0)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
    {SUB_NUM,1,"12 LCD DISPLAY      ","",0,0,0,0,(void*)&LCD_T,sizeof(LCD_T)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
    {SUB_NUM,0,"13 DATE AND TIME    ","",0,0,0,0,(void*)&TIME,sizeof(TIME)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
    {SUB_NUM,0,"14 TYR PARAMETERS   ","",0,0,0,0,(void*)&TYR_F,sizeof(TYR_F)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
    //{SUB_NUM,1,"1  LANGUGE-0       %1d","",&N.Lng,0,1,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};
/*####################################################################################################*/
/*####################################################################################################*/
/*####################################################################################################*/
/*####################################################################################################*/
/*--------------------------------------------------------------------------------------------------------------------/
/   �������� ����
/--------------------------------------------------------------------------------------------------------------------*/

const MENU_MAN menu0[]={
  {SUB_NUM,0,"0A  SMOKING RECIPE  ","",0,0,0,0,(void*)&RECIPE,sizeof(RECIPE)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
  {SUB_NUM,0,"01    PAREMETERS    ","",0,0,0,0,(void*)&Params,sizeof(Params)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
  {SUB_NUM,0,"02    PROTECTING    ","",0,0,0,0,(void*)&PROTECT_MENU,sizeof(PROTECT_MENU)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
  {SUB_NUM,0,"03   PID REGULATOR  ","",0,0,0,0,(void*)&PID_MENU,sizeof(PID_MENU)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
  {SUB_NUM,0,"04 SCR1.CIRCULATION ","",0,0,0,0,(void*)&SCR1,sizeof(SCR1)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
  {SUB_NUM,0,"05 SCR2.REL.HUMIDITY","",0,0,0,0,(void*)&SCR2,sizeof(SCR2)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
};

/*--------------------------------------------------------------------------------------------------------------------/
/   ʲ���� ����
/--------------------------------------------------------------------------------------------------------------------*/


