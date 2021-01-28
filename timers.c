

#include <stdbool.h>
#include <myiolpc1768.h>

#include "type.h"

#include "main.h"
#include "menu.h"


#include "adc.h"

#include "modbus.h"
#include "smoking.h"

#include "uart_0.h"
//#include "uart_1.h"
//#include "uart_2.h"
#include "uart_3.h"
/*------------------------------------------------------------------------------------------------/
/  ОРГАНІЗАЦІЯ ІНТЕРВАЛУ ПОСТАНОВКИ В ПЕРЕДАЧУ
/  і інтервалу утримування передачіпісля передачі останнього байту
/------------------------------------------------------------------------------------------------*/

bool block_1s;      //блокування на 1с.
bool block_100ms;   //блокування на 100 мс.
int time_1s;
int time_100ms;
int powerONtime = 8;//8sec
int scr1_flag; //circulation Fan flag
int scr2_flag; //exhaust Fan flag
int iSp_flag;  //current setpoint flag

//==============================================================================
//==============================================================================
//таймери для візуальної частини
//VIRTUAL_TIMER volatile vtimer_arr[10];
void Timer_end_funct(char i);

unsigned int time_1sa;
unsigned int time_10mS;
unsigned int keys_time;//клавіатурний таймер

unsigned int time_time;//затормозити цей таймер в 30 разів бо треба 10 мс а є 1/3мс
unsigned int times_100ms;//100мС таймер для під регуляторів

int PID_flag;
int buzz_flag;//час для звуку
int RH_flag;

int time_lcd;//час для орнанізації оновлення LCD
char flag_LCD;//однака для організації відображення і реанімації LCD

/******************************************************************************/
//спрацьовує кожні 10мС
//void timer0_isr(void)
void SysTick_Handler (void)
{
   
  //ця частина має спрацьовувати раз за 10 мС
  //таймер для дисплея
  if(++time_lcd > 12){
    time_lcd = 0;//час для організації оновлення LCD
    flag_LCD = 1;//реанімації LCD
  }
  if(++time_10mS >= N.lcd.T[0])time_10mS = 0;
  if(keys_time)keys_time--;//клавіатурний таймер
  
  //================================================================================
  //================================================================================
  //секундний таймер
  if(++time_1sa >= 99){
    time_1sa = 0;
    if (powerONtime) powerONtime--; 
    PID_flag = 1; // set flag for pid regulator
    RH_flag = 1; // set flag for realative humidity regulator
    smTfl = 1; // set flag for smoking process
    scr1_flag = 1;//circulation Fan flag
    scr2_flag = 1;//exhaust Fan flag
    iSp_flag = 1;  //current setpoint flag
    
  }
  //================================================================================
  //100mS таймер
  if(++times_100ms >= 10){
    times_100ms = 0;

  }
  //================================================================================
  if (buzz_flag){
    if (--buzz_flag == 0){
      FIO3SET_bit.BUZ = 1;//виключити звук
    }
  }
  //================================================================================
}
///////////////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/******************************************************************************/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//================================================================================
//================================================================================
#define T0IR_MR0INT                0x01   /* Bit 0: MR0INT (Interrupt flag for match channel 0) */
#define T0IR_MR1INT                0x02   /* Bit 1: MR1INT (Interrupt flag for match channel 0) */
#define T0IR_MR2INT                0x04   /* Bit 2: MR2INT (Interrupt flag for match channel 0) */
#define T1IR_CR2INT                0x40   /* Bit 6: CR2INT (Interrupt flag for capture channel 2) */

//емітація колеса із мітками
char points[14] = {3,78,3,6,3,60,3,24,3,69,3,15,3,87};//градусні міри кожного інтервалу всього 360 градусів
char pnt;//вказівник на мітку
int Erpm = 1000;//емітовані оберти за хвилину диска




void TMR0_IRQHandler( void )
{
    //unsigned int IntSrc;

    //IntSrc = T0IR; /* Read register only once */

    if ( T0IR & T0IR_MR0INT )
    {
      if(timer_uart[0] != 0){
        timer_uart[0] -= 1;
        if(timer_uart[0] == 0) start_tx0();
      }
      /*
      if(timer_uart[1] != 0){
        timer_uart[1] -= 1;
        if(timer_uart[1] == 0) start_tx1();
      }
      
      if(timer_uart[2] != 0){
        timer_uart[2] -= 1;
        if(timer_uart[2] == 0) start_tx2();
      }
      */
      if(timer_uart[3] != 0){
        timer_uart[3] -= 1;
        if(timer_uart[3] == 0) start_tx3();
      }
      /*--- Clear interrupt flag ---*/
      T0IR_bit.MR0INT = 1;
    }

    if ( T0IR & T0IR_MR1INT )
    {
      //FIO0CLR = 1<<14;
      /*--- Clear interrupt flag ---*/
      T0MCR &= ~(1<<3);//MR1I переривання по каналу MR1
      T0IR_bit.MR1INT = 1;
    }

    if ( T0IR & T0IR_MR2INT )
    {
      //FIO0SET = 1<<14;
      /*--- Clear interrupt flag ---*/
      T0MCR &= ~(1<<6);//MR2I переривання по каналу MR2
      T0IR_bit.MR2INT = 1;
    }
} /* MA_IntHandler_TIMER0 */
//================================================================================
#define T1IR_MR0INT                0x01   /* Bit 0: MR0INT (Interrupt flag for match channel 0) */
#define T1IR_MR1INT                0x02   /* Bit 1: MR0INT (Interrupt flag for match channel 1) */
#define T1IR_MR2INT                0x04   /* Bit 2: MR0INT (Interrupt flag for match channel 2) */
#define T1IR_MR3INT                0x08   /* Bit 3: MR0INT (Interrupt flag for match channel 3) */

unsigned int T_begin[4]; //час початку імпульсу
unsigned int T_end[2][4]; //час спрацювання струму
int BIP[2][4]; //час спрацювання
int fault_flag;//тимчасова змінна для виловлування не спрацювання вихода

__ramfunc void TMR1_IRQHandler( void )
{


    if ( T1IR & T1IR_MR1INT ){
      T1IR_bit.MR1INT = 1; //--- Clear interrupt flag ---
    }

    if ( T1IR & T1IR_MR2INT ){
      T1IR_bit.MR2INT = 1;//--- Clear interrupt flag ---
    }
    
    //обмеження струму
    if ( T1IR & T1IR_MR3INT ){
        T1IR_bit.MR3INT = 1; //--- Clear interrupt flag ---
    }
    // включення нижнього спільного ключа для заряду бустерного конденсатора
    if ( T1IR & T1IR_MR0INT ){
      T1IR_bit.MR0INT = 1; //--- Clear interrupt flag ---
    }

} /* MA_IntHandler_TIMER1 */

//================================================================================
//================================================================================
//================================================================================
#define T2IR_MR0INT                0x01   /* Bit 0: MR0INT (Interrupt flag for match channel 0) */
#define T2IR_MR1INT                0x02   /* Bit 1: MR0INT (Interrupt flag for match channel 1) */
#define T2IR_MR2INT                0x04   /* Bit 2: MR0INT (Interrupt flag for match channel 2) */
#define T2IR_MR3INT                0x08   /* Bit 3: MR0INT (Interrupt flag for match channel 3) */
#include "timer2.h"
//================================================================================
//================================================================================
//================================================================================
//================================================================================
//зняти напруги
#define Switch_Off FIO0SET_bit.INJ1 = 1; FIO0SET_bit.INJ2 = 1; FIO0SET_bit.INJ3 = 1; FIO0SET_bit.INJ4 = 1; FIO0SET_bit.PZ_OFF = 1;//FIO0SET = 1<<inj1 | 1<<inj2 | 1<<inj3 | 1<<inj4 | 1<<on140; FIO0SET_bit.Uncherge = 1;
//================================================================================

void EINT1_IRQHandler(void)
{
  if(EXTINT_bit.EINT1){
    EXTINT_bit.EINT1 = 1;//зняти переривання 0
  
  }
}
/******************************************************************************/

/******************************************************************************/
void EINT2_IRQHandler(void)
{
  if(EXTINT_bit.EINT2){
    EXTINT_bit.EINT2 = 1;//зняти переривання 0

  }
}
/******************************************************************************/

/******************************************************************************/
__ramfunc void EINT0_IRQHandler(void)
{
  if(EXTINT_bit.EINT0){
    EXTINT_bit.EINT0 = 1;//зняти переривання 0

  }
}
/******************************************************************************/
__ramfunc void EINT3_IRQHandler(void)
{
  if(EXTINT_bit.EINT3){
    EXTINT_bit.EINT3 = 1;//зняти переривання 3
  }
}
/******************************************************************************/
//================================================================================
//================================================================================
//================================================================================
//================================================================================
//================================================================================
