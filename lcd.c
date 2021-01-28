
#include <intrinsics.h>
#include <stdio.h>
#include <myiolpc1768.h>
#include <assert.h>

#include "board.h"
#include "type.h"
#include "main.h"

#define LCD_WRC( COM )  while(SSP0SR_bit.BSY); SSP0DR = COM; 
#define LCD_WRD( COM )  while(SSP0SR_bit.BSY); SSP0DR = 0x100 | COM; 

char LCD_str[32];
char LCD_input_str[32];
char LCD_input_str_[32];
char *LCD_crsr_position;
char LCD_buf[80];

const char LCD_koi[64]=
{0x41,0xA0,0x42,0xA1,0xE0,0x45,0xA3,0xA4,0xA5,0xA6,0x4B,0xA7,0x4D,0x48,0x4F,0xA8,
 0x50,0x43,0x54,0xA9,0xAA,0x58,0xE1,0xAB,0xAC,0xE2,0xAD,0xAE,0x62,0xAF,0xB0,0xB1,
 0x61,0xB2,0xB3,0xB4,0xE3,0x65,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0x6F,0xBE,
 0x70,0x63,0xBF,0x79,0xE4,0x78,0xE5,0xC0,0xC1,0xE6,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7};
/******************************************************************************/
/*
const unsigned char c_1   [8] = {0x00,0x00,0x00,0x01,0x01,0x19,0x03,0x01}; //c-1{0x00,0x0C,0x10,0x10,0x0D,0x01,0x0D,0x01}
const unsigned char cel   [8] = {0x00,0x00,0x00,0x00,0x06,0x09,0x09,0x06}; //°C{0x00,0x03,0x04,0x04,0x03,0x08,0x14,0x08}
const unsigned char yi_l  [8] = {0x00,0x0E,0x04,0x04,0x04,0x0C,0x00,0x0A}; //ї
const unsigned char ye_l  [8] = {0x00,0x0E,0x11,0x1C,0x11,0x0E,0x00,0x00}; //є
const unsigned char ye_U  [8] = {0x00,0x0E,0x11,0x10,0x1C,0x10,0x11,0x0E}; //Є
const unsigned char wav   [8] = {0x00,0x00,0x00,0x16,0x09,0x00,0x00,0x00}; //~
*/

const unsigned char c_1   [] = {
  0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,
  0x00,0x00,0x00,0x01,0x01,0x19,0x03,0x01,
  0x00,0x00,0x00,0x00,0x06,0x09,0x09,0x06,
  0x00,0x0E,0x04,0x04,0x04,0x0C,0x00,0x0A,
  0x00,0x0E,0x11,0x1C,0x11,0x0E,0x00,0x00,
  0x00,0x0E,0x11,0x10,0x1C,0x10,0x11,0x0E,
  0x00,0x00,0x00,0x16,0x09,0x00,0x00,0x00,
};

/******************************************************************************/
/******************************************************************************/
/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
/******************************************************************************/
void LCD_delay (unsigned int arg)
{
    unsigned int arg_del;
    arg_del = arg*2400*25;
    arg_del/= 4;// ~1mS
    while (arg_del) arg_del--; //затримка arg * 1mS
}
/******************************************************************************/
//#define readyDL 5
int readyDL=5;
void LCD_ready(void)
{
    unsigned int arg_del;
    arg_del = readyDL*24*25;
    arg_del/= 4;// ~0.01mS
    while (arg_del) arg_del--; //затримка arg * 0.01mS
}
/******************************************************************************/
/******************************************************************************/
void LCD_cls(void)
{
    LCD_WRC(0x01);
}
/******************************************************************************/
void LCD_init (void)
{

  FIO3DIR_bit.nLCD_OE = 1;//вихід
  FIO3CLR_bit.nLCD_OE = 1;//0  
  
  FIO2CLR_bit.LCDS = 1;//select writes to LCD
  SSP0DR = 0x1ff;//all hight
  while(SSP0SR_bit.BSY);
  
  LCD_delay(30);

  LCD_WRC(0x30);
  LCD_delay(5);
  LCD_WRC(0x30);
  LCD_delay(1);
  LCD_WRC(0x30);
  LCD_delay(5);

  LCD_WRC(0x38);//0 0 1 1 N=1 F=0 * *
  LCD_delay(5);
  LCD_WRC(0x08);
  LCD_delay(5);
  LCD_WRC(0x01);
  LCD_delay(5);
  LCD_WRC(0x06);// I/D=1 S=0
  LCD_delay(5);
  LCD_WRC(0x0C);// 1DCB  
  LCD_delay(5);
    // ukrainian  0x01-~, 0х02-Є, 0х03-є, 0х04-ї, 5- градус, 6 - мінус перша степінь
  LCD_WRC(0x48);
  LCD_delay(1);
  int adress = 56;//кількість
  while (adress)
  {
    LCD_WRD(c_1 [adress-1]);
    LCD_delay(1);
    adress--;
  }

  LCD_WRC(0x01);//cls
  LCD_delay(1);
  
  FIO2SET_bit.LCDS = 1;//unselect writes to LCD
}
/******************************************************************************/
void LCD_reanim (void)
{
}
/******************************************************************************/
/******************************************************************************/
const char r_rp[] = {0,40,20,60};
void LCD_put_str(char row, char column,const char *string)
{
  char i,ch,flag=0;

  i = r_rp[row] + column;
  do{
    ch = *string++;
    if(!ch)break;           //якщо 0 - тобто кінець рядка то вийти
    //switch(ch){
    //case 7:                       //службовий символ 7 - не переодовувати в російську розкладу один сивол
    //case 8: flag = ch;continue;   //8 -не перетворювати в російську розкладку доки не зустріниця службосий символ 3
    //case 9: flag = 0;continue;    //9 - кінець символів котрі нетреба перетворювати в російську розкладку
    //}
    //if(ch>=192 && !flag) ch = LCD_koi[ch-192];
    if(flag == 1)flag = 0;

    LCD_buf[i++] = ch;
  }while(i<80);
}
/******************************************************************************/
void LCD_all()
{
  char i;
  
  FIO2CLR_bit.LCDS = 1;//select writes to LCD
  LCD_WRC(0x80);
  //LCD_ready();

  for(i=0;i<80;i++){
    LCD_WRD(LCD_buf[i]);
    //LCD_ready();
  }  
  while(SSP0SR_bit.BSY);
  FIO2SET_bit.LCDS = 1;//unselect writes to LCD
}
/******************************************************************************/
void LCD_put_str_(char row, char column,const char *string)
{
    char ch,flag=0;

  start:
    switch (row)
    {
         case 0: LCD_WRC(0x80|(0x00+column));LCD_ready();break;
         case 1: LCD_WRC(0x80|(0x40+column));LCD_ready();break;
         case 2: LCD_WRC(0x80|(0x14+column));LCD_ready();break;
         case 3: LCD_WRC(0x80|(0x54+column));LCD_ready();break;
    default: return;
    }

    for(;;){
        ch = *string++;
        if(!ch)break;           //якщо 0 - тобто кінець рядка то вийти
        switch(ch){
          case 7:                       //службовий символ 7 - не переодовувати в російську розкладу один сивол
          case 8: flag = ch;continue;   //8 -не перетворювати в російську розкладку доки не зустріниця службосий символ 3
          case 9: flag = 0;continue;    //9 - кінець символів котрі нетреба перетворювати в російську розкладку
          case 0xa:
          case 0xd: column = 0; row++; goto start;  //переведення рядка
        default: if(column++ == 20)return;//запобігання переповненню рядка
        }
        if(ch>=192 && !flag) ch = LCD_koi[ch-192];
        if(flag == 1)flag = 0;

        LCD_WRD(ch);
        LCD_ready();
    }

}

/******************************************************************************/
