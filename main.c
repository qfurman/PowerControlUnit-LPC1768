
#include <intrinsics.h>
#include <stdio.h>
#include <myiolpc1768.h>
#include <assert.h>
#include "board.h"
#include "initTools.h"
#include "lcd.h"
#include "type.h"
#include "keys.h"
#include "menu.h"
#include "ramtron.h"
#include "LPC_Rtc.h"
#include "timers.h"
#include "adc.h"
#include "uart_0.h"
//#include "uart_1.h"
//#include "uart_2.h"
#include "uart_3.h"
#include "modbus.h"
#include "serials.h"
#include "temperatures.h"
#include "smoking.h"
#include "SCRs.h"

/******************************************************************************/
TYRREG TyrReg;//для тиристорного регулятора в програмованій логіці

unsigned int timer_uart[4];
int pwmLim;//границя для генерації шим
int rewrite_esw;//переписати вбудоване програмне забезпечення

PARAMETRS N;
AIU aiu[2]; //плати аналогових входів
RUNSMOKE RSP; //runtime parameters of smoking
int Sts; //current status of system
int resAlrm; //COMMAND reset alarm
int RunPrg; //COMMAND run smoking programm

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/********************** MAIN BEGIN ********************************************/
int main(void)
{

  InitClock(); // Init clock
  SYSTimer_init ();
  Timer0_init ();
  Timer1_init ();
  Timer2_init ();
  Timer3_init ();
  SSP0_SSP1_init ();
  UARTs_init ();
  __enable_interrupt();  
  PCBpins_init ();
  
  AT25_MEMORY_init ();  
  
  kays_init();
  menu_init();
  LCD_init(); 
  
  N.rtc_on = 0;//ознака наявності годинника
  if(N.rtc_on) RTC_Init();
  else RTCCCR_bit.CLKEN = 0;//виключити годинник

  PWMs_init ();
  adc_init();//запустити ацп
  //DACR_bit.VALUE = 0;// DAC value
  UARTs_PostInit ();
 
  EXTERNAL_INT_init ();

  IP0_bit.PRI_3 = 0;//Timer1 найвищий пріоритет
  IP5_bit.PRI_22 = 2<<3;//ADC нижчий пріоритет

  init_recipe (); //rewrite from flash init smoking
  N.PID.ON = 0; //switch OFF the PID regulator after powerON
  
  while(1)
  {
    LNG = N.Lng & 1;//мова 0-rus 1-ua
    ReWR_flash (); //if there is a new parameters through modbus
    
    meen_adc();//знаходження середнього значення для кожного каналу
    Tclamps = ntcMF25_103_3435(aiu[0].ntc0);
    Theatsink = ntcMF25_103_3435(aiu[0].ntc1);
    
    mV0 = Calc_mV( aiu[0].ADC0, aiu[0].Kg0);
    mV1 = Calc_mV( aiu[0].ADC1, aiu[0].Kg1);
    mV2 = Calc_mV( aiu[0].ADC2, aiu[0].Kg2);
    rtdT0 = RTD_Pt100(mV0);//as the current is 1mA hence resistance and Voltage the same
    rtdT1 = RTD_Pt100(mV1);
    TC2 = Termocuple_K (mV2, N.t_comp? Tclamps : 0);
    
    Ht = Humid (rtdT0, rtdT1);//humidity
    PID_regulator (rtdT0);//dry temperature
    putPIDoutput (); // delivery PID result to current regulator
    circulationFAN ();
    exhaustFAN ();
    relHumidRegul ();
    smokingPRC ();
    IspApproach ();//shockless current setpoint changing
    
    pwm_20mA_out (1000);
    lcd_pwm_renew ();
    
    //if(time_lcd <= 1){
      menu();//відображення меню в буфер
      LCD_all();//виведення буфера в паяті в екран
    //}
    if(flag_LCD){
      //далеко не щоразу
        //LCD_init();
        //if(N.lcd.reanim)LCD_reanim();//реанімація
        flag_LCD = 0;
    }

    if (keys_time==0){ keys_time = 2; RdKeyboard ();}
    
    if ( N.tyrOFF > 900) N.tyrOFF = 800;
    if ( N.tyrOFF < 300) N.tyrOFF = 400;
    if ( N.tyrON >= N.tyrOFF) N.tyrON = N.tyrOFF - 200;
    
    if (Theatsink >= N.tyrOFF && !powerONtime) { 
      // here write reaction to hight temperature of heatsink
      N.PID.ON = 0; //switch OFF the PID regulator after powerON
      TyrReg.enable = 1; 
      TyrReg.SCR0 = 0xffff;//switch off the thyristors
    }
    
    SCRsControl ();//power outputs managment
    
    CoolerFan_OnOff (); // fan of cooling of heatsink
  }
}
/********************* MAIN END ***********************************************/
/******************************************************************************/
/*
void TMR0_IRQHandler (void)
{

  T0IR_bit.MR0INT = 1;


  if(++time_time >= 40){
    time_time = 0;
    //ця частина має спрацьовувати раз за 10 мС
    //таймер для дисплея
    if(++time_10mS >=49)time_10mS = 0;
    if(keys_time)keys_time--;//клавіатурний таймер
  }

  NVIC_ClrPend(NVIC_TIMER0);
}
*/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
