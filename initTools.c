
#include <myiolpc1768.h>
#include <assert.h>
#include "board.h"
#include "ramtron.h"
#include "main.h"
#include "uart_0.h"
//#include "uart_1.h"
//#include "uart_2.h"
#include "uart_3.h"
#include "modbus.h"

/******************************************************************************/
const char *build_num = {"1.0.0"};//версія програми
const char build_date[] = __DATE__;
const char build_time[] = __TIME__;

/******************************************************************************/
#define	IAP_CMD_REINVOKE_ISP		57
#define IAP_CMD_READ_SERIAL_NUMBER	58

/* IAP boot ROM location and access function */
#define IAP_ROM_LOCATION	0x1FFF1FF1UL
#define IAP_EXECUTE_CMD(a, b)	((void (*)())(IAP_ROM_LOCATION))(a, b)

/*variable for clitical section entry control*/
//Int32U CriticalSecCntr;
/******************************************************************************/
unsigned int serial[5];

void u32IAP_ReadSerialNumber(void)
//unsigned int *pu32byte0, unsigned int *pu32byte1, unsigned int *pu32byte2, unsigned int *pu32byte3
{
	//unsigned int au32Result[5];
	unsigned int au32Command[5];

	au32Command[0] = IAP_CMD_READ_SERIAL_NUMBER;

	IAP_EXECUTE_CMD(au32Command, &serial);

	//*pu32byte0 = au32Result[0];
	//*pu32byte1 = au32Result[1];
	//*pu32byte2 = au32Result[2];
	//*pu32byte3 = au32Result[3];

	return;
}
/******************************************************************************/
void vIAP_ReinvokeISP(void)
{
	unsigned int au32Result[5];
	unsigned int au32Command[5];

	au32Command[0] = IAP_CMD_REINVOKE_ISP;

	IAP_EXECUTE_CMD(au32Command, au32Result);
}
/******************************************************************************/

/*************************************************************************
 * Function Name: NVIC_IntEnable
 * Parameters: IntNumber - Interrup number
 * Return: void
 *
 * Description: Enable interrup at NVIC
 *
 *
 *************************************************************************/
void NVIC_IntEnable(unsigned int IntNumber)
{
volatile unsigned long * pNVIC_SetEn = &SETENA0;

  assert((NVIC_WDT <= IntNumber) && (NVIC_PLL1 >= IntNumber));
  IntNumber -= NVIC_WDT;

  pNVIC_SetEn += IntNumber/32;
  *pNVIC_SetEn |= (1UL<<(IntNumber%32));

}

/*************************************************************************
 * Function Name: NVIC_IntDisable
 * Parameters: IntNumber - Interrup number
 * Return: void
 *
 * Description: Disables interrup at NVIC
 *
 *
 *************************************************************************/
void NVIC_IntDisable(unsigned int IntNumber)
{
volatile unsigned long * pNVIC_ClrEn = &CLRENA0;

  assert((NVIC_WDT <= IntNumber) && (NVIC_PLL1 >= IntNumber));
  IntNumber -= NVIC_WDT;

  pNVIC_ClrEn += IntNumber/32;
  *pNVIC_ClrEn |= (1UL<<(IntNumber%32));

}

/*************************************************************************
 * Function Name: NVIC_ClrPend
 * Parameters: IntNumber - Interrup number
 * Return: void
 *
 * Description:Clear pending interrupt at NVIC
 *
 *
 *************************************************************************/
void NVIC_ClrPend(unsigned int IntNumber)
{
volatile unsigned long * pNVIC_ClrPend = &CLRPEND0;

  assert((NVIC_WDT <= IntNumber) && (NVIC_PLL1 >= IntNumber));
  IntNumber -= NVIC_WDT;

  pNVIC_ClrPend += IntNumber/32;
  *pNVIC_ClrPend |= (1UL<<(IntNumber%32));

}

/*************************************************************************
 * Function Name: NVIC_ClrPend
 * Parameters: IntNumber - Interrup number, Interrupt Priority
 * Return: void
 *
 * Description:Sets Interrupt priority
 *
 *
 *************************************************************************/
void NVIC_IntPri(unsigned int IntNumber, unsigned char Priority)
{
volatile unsigned char * pNVIC_IntPri = (unsigned char *)&IP0;

  assert((NVIC_WDT <= IntNumber) && (NVIC_PLL1 >= IntNumber));
  IntNumber -= NVIC_WDT;
  pNVIC_IntPri += IntNumber;
  *pNVIC_IntPri = Priority;
}

#define FCCLK_FREQ 100000000
/*************************************************************************
 * Function Name: InitClock
 * Parameters: void
 * Return: void
 *
  * Description: Initialize PLL and clocks' dividers. Hclk - 300MHz,
 *              Fcckl = 100MHz
 *
 *************************************************************************/
void InitClock(void)
{
  // Flash accelerator init
  FLASHCFG = (0x5UL<<12) | 0x3AUL;
  
  // 1. Init OSC
  SCS_bit.OSCRANGE = 0;
  SCS_bit.OSCEN = 1;
  // 2.  Wait for OSC ready
  while(!SCS_bit.OSCSTAT);
  // 3. Disconnect PLL
  PLL0CON_bit.PLLC = 0;
  PLL0FEED = 0xAA;
  PLL0FEED = 0x55;
  // 4. Disable PLL
  PLL0CON_bit.PLLE = 0;
  PLL0FEED = 0xAA;
  PLL0FEED = 0x55;
  // 5. Select source clock for PLL
  CLKSRCSEL_bit.CLKSRC = 1; // Selects the main oscillator as a PLL clock source.
  // 6. Set PLL settings 300 MHz   12MHz/6*(75*2)=300MHz
  PLL0CFG_bit.MSEL = 75-1;
  PLL0CFG_bit.NSEL = 6-1;
  PLL0FEED = 0xAA;
  PLL0FEED = 0x55;
  // 7. Enable PLL
  PLL0CON_bit.PLLE = 1;
  PLL0FEED = 0xAA;
  PLL0FEED = 0x55;
  // 8. Wait for the PLL to achieve lock
  while(!PLL0STAT_bit.PLOCK);
  // 9. Set clk divider settings
  CCLKCFG   = 3-1;            // 1/3 Fpll
  PCLKSEL0 = PCLKSEL1 = 0;    // other peripherals 100/4 = 25MHz
  // 10. Connect the PLL
  PLL0CON_bit.PLLC = 1;
  PLL0FEED = 0xAA;
  PLL0FEED = 0x55;
  
  #if FCCLK_FREQ < 20000000
    FLASHCFG = (0x0UL<<12) | 0x3AUL;
  #elif FCCLK_FREQ < 40000000
    FLASHCFG = (0x1UL<<12) | 0x3AUL;
  #elif FCCLK_FREQ < 60000000
    FLASHCFG = (0x2UL<<12) | 0x3AUL;
  #elif FCCLK_FREQ < 80000000
    FLASHCFG = (0x3UL<<12) | 0x3AUL;
  #elif FCCLK_FREQ < 100000000
    FLASHCFG = (0x4UL<<12) | 0x3AUL;
  #endif  
  PCLKSEL1_bit.PCLK_GPIOINT = 1;//PCLK_peripheral = CCLK
  PCLKSEL1_bit.PCLK_PCB = 1;//PCLK_peripheral = CCLK    
}
/******************************************************************************/
void SYSTimer_init (void)
{
  // цей таймер є частиною ядра тому переривання для нього потрібно просто дозволити його регістром
  //STCTRL_bit.ENABLE = 1;// enable System Tick Timer
  //STCTRL_bit.TICKINT = 1;// enable System Tick Timer Int
  STRELOAD = 0xF423F;//відповідає 10мС на 100МГц
  STCTRL = 7;//enables the timer and its interrupt, and selects cclk as the clock source

  PCONP_bit.PCRIT = 1;//Repetitive Interrupt Timer power/clock control bit
  PCLKSEL1_bit.PCLK_RIT = 1;//PCLK_peripheral = CCLK
  //RICOMPVAL = 0xffffffff;//Compare register
  RICTRL_bit.RITEN = 1;//Repetitive Interrupt Timer - enable
}
/******************************************************************************/
#define TIMER0_IN_FREQ        25000000
#define TIMER0_TICK_PER_SEC   4000
/******************************************************************************/
void Timer0_init (void)
{
  // Enable TIM0 clocks
  PCONP_bit.PCTIM0 = 1; // enable clock
  PCLKSEL0_bit.PCLK_TIMER0 = 1;//PCLK_peripheral = CCLK
  // Init Time0
  T0TCR_bit.CE = 0;     // counting  disable
  T0TCR_bit.CR = 1;     // set reset
  T0TCR_bit.CR = 0;     // release reset
  T0CTCR_bit.CTM = 0;   // Timer Mode: every rising PCLK edge
  T0MCR_bit.MR0I = 1;   // Enable Interrupt on MR0
  T0MCR_bit.MR0R = 1;   // Enable reset on MR0
  T0MCR_bit.MR0S = 0;   // Disable stop on MR0
  // set timer 0 period
  //T0PR = 25-1;
  T0MR0 = (TIMER0_IN_FREQ)/((T0PR+1) * TIMER0_TICK_PER_SEC);
  T0PR = 0;
  T0MR0 = 25000;
  // init timer 0 interrupt
  T0IR_bit.MR0INT = 1;  // clear pending interrupt
  T0TCR_bit.CE = 1;     // counting Enable
  
  /*Enable NVIC TMR0 Interrupt*/
  NVIC_IntEnable(NVIC_TIMER0);  
}
/******************************************************************************/
void Timer1_init (void)
{
  // Enable TIM1 clocks
  PCONP_bit.PCTIM1 = 1; // enable clock

  // Init Timer1
  PCLKSEL0_bit.PCLK_TIMER1 = 1;//PCLK_peripheral = CCLK

  T1TCR_bit.CE = 0;     // counting  disable
  T1TCR_bit.CR = 1;     // set reset
  T1TCR_bit.CR = 0;     // release reset
  T1CTCR_bit.CTM = 0;   // Timer Mode: every rising PCLK edge
  T1MCR_bit.MR0I = 1;   // Enable Interrupt on MR0
  //T0MCR_bit.MR0R = 1;   // Enable reset on MR0
  T1MCR_bit.MR0S = 0;   // Disable stop on MR0
  // set timer 1 period
  T1PR = 0;//працює на тій самій частоті 100МГц
  T1MR0 = 0;
  // init timer 1 interrupt
  T1IR_bit.MR0INT = 1;  // clear pending interrupt
  //T1TCR_bit.CE = 1;     // counting Enable
  /*Enable NVIC TMR1 Interrupt*/
  NVIC_IntEnable(NVIC_TIMER1);
}
/******************************************************************************/
void Timer2_init (void)
{
  // Enable TIM2 clocks
  PCONP_bit.PCTIM2 = 1; // enable clock
  // Init Timer2
  PCLKSEL1_bit.PCLK_TIMER2 = 1;//PCLK_peripheral = CCLK

  T2TCR_bit.CE = 0;     // counting  disable
  T2TCR_bit.CR = 1;     // set reset
  T2TCR_bit.CR = 0;     // release reset
  T2CTCR_bit.CTM = 0;   // Timer Mode: every rising PCLK edge
  T2MCR_bit.MR0I = 1;   // Enable Interrupt on MR0
  T2MCR_bit.MR0S = 0;   // Disable stop on MR0
  // set timer 2 period
  T2PR = 0;//працює на тій самій частоті 100МГц
  T2MR0 = 0;
  // init timer 2 interrupt
  T2IR_bit.MR0INT = 1;  // clear pending interrupt
  //T2TCR_bit.CE = 1;     // counting Enable
  /*Enable NVIC TMR2 Interrupt*/
  NVIC_IntEnable(NVIC_TIMER2);
}
/******************************************************************************/
void Timer3_init (void)
{
  // Init Timer3
  PCONP_bit.PCTIM3 = 1; // enable power
  //PINSEL0_bit.P0_10 = 3;//підключити MAT3.0 до вивода 48 (P0.10) 
  PINSEL0_bit.P0_11 = 3;//підключити MAT3.1 до вивода 49 (P0.11) 
  //T3EMR_bit.EMC0 = 3;//Toggle the corresponding External Match bit/output
  T3EMR_bit.EMC1 = 3;//Toggle the corresponding External Match bit/output
  PCLKSEL1_bit.PCLK_TIMER3 = 1;//PCLK_peripheral = CCLK

  T3TCR_bit.CE = 0;     // counting  disable
  T3TCR_bit.CR = 1;     // set reset
  T3TCR_bit.CR = 0;     // release reset
  T3CTCR_bit.CTM = 0;   // Timer Mode: every rising PCLK edge
  //T3MCR_bit.MR0I = 1;   // Enable Interrupt on MR0
  //T3MCR_bit.MR0R = 1;   // Enable reset on MR0
  //T3MCR_bit.MR0S = 0;   // Disable stop on MR0
  //T3MCR_bit.MR0R = 1;   // Enable reset on MR0
  T3MCR_bit.MR1S = 0;   // Disable stop on MR0
  T3MCR_bit.MR1R = 1;   // Enable reset on MR1
  //set timer 3 period
  T3PR = 0;//працює на тій самій частоті 100МГц
  //T3MR0 = 1;//25MHz тільки переключення вибрати один
  T3MR1 = 499;//100kHz переключення і ресет
  // init timer 1 interrupt
  T3IR_bit.MR0INT = 1;  // clear pending interrupt
  T3TCR_bit.CE = 1;     // counting Enable
  /*Enable NVIC TMR3 Interrupt*/
  //NVIC_IntEnable(NVIC_TIMER3);
}
/******************************************************************************/
void SSP0_SSP1_init (void)
{
  /*--- Initialize SSP0 registers ---*/
  //FIO0DIR_bit.SSEL0 = 1;//вихід SSEL0
  PINSEL0_bit.P0_15 = 2;//підключити SCK0 до вивода
  PINSEL1_bit.P0_16 = 2;//підключити SSEL0 до вивода
  PINSEL1_bit.P0_17 = 2;//підключити MISO0 до вивода
  PINSEL1_bit.P0_18 = 2;//підключити MOSI0 до вивода
  SSP0IMSC=0;
  SSP0CPSR_bit.CPSDVSR = 0x5c;//This even value between 2 and 254, by which SSP_PCLK is divided to yield the prescaler output clock. Bit 0 always reads as 0.
  SSP0CR0_bit.DSS = 8;//1000 9-bit transfer
  SSP0CR1_bit.SSE = 1;      /* SSP0 Enable */
  
  /*--- Initialize SSP1 registers ---*/
  FIO0DIR_bit.SSEL1 = 1;//вихід SSEL1
  PINSEL0_bit.P0_7 = 2;//підключити SCK1 до вивода
  PINSEL0_bit.P0_8 = 2;//підключити MISO1 до вивода
  PINSEL0_bit.P0_9 = 2;//підключити MOSI1 до вивода
  SSP1IMSC=0;
  SSP1CPSR  = ( SSP1CPSR & ~0x000000FF ) | 0x00000008;
  SSP1CR0   = ( SSP1CR0 & ~0x0000FFFF ) | 0x00000007;
  SSP1CR1_bit.SSE = 1;      /* SSP Enable */
  /*Enable NVIC SSP1 Interrupt*/
  NVIC_IntEnable(NVIC_SSP1);
}
/******************************************************************************/
void UARTs_init (void)
{

  /*--- Initialize UART0 registers ---*/
  PINSEL0_bit.P0_2 = 1;//підключити TXD0 до вивода
  PINSEL0_bit.P0_3 = 1;//підключити RXD0 до вивода
  FIO2SET_bit.RTS1 = 1;//стати в прийом
  FIO2DIR_bit.RTS1 = 1;//вихід R/T1 switch receive / transmit for uart0
  /*Enable NVIC UART0 Interrupt*/
  NVIC_IntEnable(NVIC_UART0);

//  /*--- Initialize UART1 registers ---*/
//  PINMODE4_bit.P2_2 = 2;//Not pullup and not pulldown
//  PINMODE_OD2_bit.P2_2 = 1;//OpenDrain
//  FIO2SET_bit.P2_2 = 1;//вихід R/T1 в 1
//  FIO2DIR_bit.P2_2 = 1;//вихід R/T1
  PINSEL4_bit.P2_0 = 2;//підключити TXD1 до вивода
  PINSEL4_bit.P2_1 = 2;//підключити RXD1 до вивода
//  /*Enable NVIC UART0 Interrupt*/
//  NVIC_IntEnable(NVIC_UART1);
//
//  /*--- Initialize UART2 registers ---*/
//  PCONP_bit.PCUART2 = 1;//включити живлення
//  PINMODE4_bit.P2_13 = 2;//Not pullup and not pulldown
//  PINMODE_OD2_bit.P2_13 = 1;//OpenDrain
//  FIO2SET_bit.P2_13 = 1;//вихід R/T2 в 1
//  FIO2DIR_bit.P2_13 = 1;//вихід R/T2
//  PINSEL0_bit.P0_10 = 1;//підключити TXD2 до вивода
//  PINSEL0_bit.P0_11 = 1;//підключити RXD2 до вивода
//  /*Enable NVIC UART0 Interrupt*/
//  NVIC_IntEnable(NVIC_UART2);

  /*--- Initialize UART2 registers ---*/
  PCONP_bit.PCUART3 = 1;//включити живлення
  //PINMODE3_bit.P1_29 = 2;//Not pullup and not pulldown
  //PINMODE_OD1_bit.P1_29 = 1;//OpenDrain
  //FIO1SET_bit.P1_29 = 1;//вихід R/T3 в 1
  //FIO1DIR_bit.P1_29 = 1;//вихід R/T3
  PINSEL0_bit.P0_0 = 2;//підключити TXD3 до вивода
  PINSEL0_bit.P0_1 = 2;//підключити RXD3 до вивода
  /*Enable NVIC UART0 Interrupt*/
  NVIC_IntEnable(NVIC_UART3);
}
/******************************************************************************/
void PCBpins_init (void)
{
  FIO0DIR_bit.P0_10 = 1;//COOL coller on/off
  FIO0SET_bit.COOL = 1;
  
  FIO0DIR_bit.P0_30 = 1;//підключити LL3 до вивода не знаю чому але якщо P0.30- не вихід то і P0.29 теж не вихід :)
  FIO0DIR_bit.RELE1 = 1;//вихід rele1 RELE1 on/off P0.29
  FIO0SET_bit.RELE1 = 1;//вихід rele1
  
  FIO3DIR_bit.BUZ = 1;//BUZZER
  FIO3SET_bit.BUZ = 1;//BUZZER
  
  FIO2DIR_bit.P2_3 = 1;//PWRS1 selects power register for SCR1
  FIO2DIR_bit.P2_4 = 1;//PWRS selects power register for SCR0
  FIO2SET_bit.Rw = 1;//unselects to write PWR
  FIO2DIR_bit.P2_5 = 1;//Rw low selects write to power register
  FIO2SET_bit.PWRS1 = 1;//unselect writes to SCR1    
  FIO2SET_bit.PWRS = 1;//unselect writes to SCR0    
  FIO2DIR_bit.P2_6 = 1;//LCDS low lewel select writes to LCD
  FIO1DIR_bit.P1_21 = 1;//PWRS2 selects power register for SCR2
  FIO1SET_bit.PWRS2 = 1;//unselect writes to SCR2     
}
/******************************************************************************/
void PWMs_init (void)
{
  int pwm_Lim;
  
  // Ініціалізація  PWM
  PINSEL4_bit.P2_2 = 1;//PWM1.3
  //PINSEL4_bit.P2_3 = 1;//PWM1.4
  //PINSEL4_bit.P2_4 = 1;//PWM1.5
  //PINSEL4_bit.P2_5 = 1;//PWM1.6
  
  PCLKSEL0_bit.PCLK_PWM1 = 1;//PCLK_peripheral = CCLK
  // запуск ~12 біт шим на частоті 100000000 / (9+1)/10000 = 1000Гц
  PWM1PR    = 9;                    // Load prescaler
  //PWM1PCR = 0x00007e00;                    // PWM channel 1,2,3,4,5,6 output enabled
  PWM1PCR = 0x00001800;                    // PWM channel 3,4 output enabled
  PWM1MCR = 0x00000002;                    // On match with timer reset the counter

  pwm_Lim = 10000000000 / 1000 / (PWM1PR+1);//для тиристорного регулятора встановити жорстко
  if(pwm_Lim % 100 >=50)pwm_Lim += 100;
  pwm_Lim /= 100;

  PWM1MR0 = pwm_Lim;
  PWM1MR1 = pwm_Lim;
  //PWM1MR0 = 10000;                          // максимальне значення
  //PWM1MR1 = 10000;                          // шим 0% інверсія бо поставив транзистор
  PWM1MR3 = 10000;                         // канал 0-20мА 10000 - це максимум щоб при інверсії вийнов 0
  PWM1LER = 0x03;                          // enable shadow latch for match 0 - 1
  PWM1TCR = 0x00000002;                    // Reset counter and prescaler
  PWM1TCR = 0x00000009;                    // enable counter and PWM, release counter from reset
  
  PCONP_bit.PCMC = 1;//Motor Control PWM power/clock control bit
  //PINSEL3_bit.P1_29 = 1;//MCOB2 канал клапана
  PINSEL3_bit.P1_25 = 1;//MCOA1 канал контрасту
  PINSEL3_bit.P1_22 = 1;//MCOB0 канал підсвітки
  //PINMODE3_bit.P1_29 = 2;//Pin has neither pull-up nor pull-down resistor enabled
  //PINMODE_OD1_bit.P1_29 = 1;//Pin is in the open drain mode

  PCLKSEL1_bit.PCLK_MC = 1;//PCLK_peripheral = CCLK

  pwm_Lim = 10000000000 / 1000;//1000Hz
  if(pwm_Lim % 100 >=50)pwm_Lim += 100;
  pwm_Lim /= 100;

  MCLIM2 = pwm_Lim;
  MCMAT2 = pwm_Lim;//початкова глибина 0%
  //MCLIM2 = 100000;//максимальне значення для 1000Гц
  //MCMAT2 = 100000;//початкова глибина 0%
  MCCON_SET_bit.RUN2 = 1;//запустити таймер канал 2

  //контраст і підсвітка індикатора
  pwm_Lim = 10000000000 / 50000000;//PWM 100kHz
  //pwm_Lim = 10000000000 / 100000000;//PWM 200kHz
  MCLIM0 = pwm_Lim;
  MCLIM1 = pwm_Lim;
  MCMAT0 = pwm_Lim *50/100;//канал підсвітки
  MCMAT1 = pwm_Lim *90/100;//канал контрасту початкова глибина % зменшення - зменшує    max 200
  MCCON_SET_bit.RUN0 = 1;//запустити таймер канал підсвітки
  MCCON_SET_bit.RUN1 = 1;//запустити таймер канал контрасту

  PINSEL3_bit.P1_22 = 1;//MCOB0 канал підсвітки
}
/******************************************************************************/
/******************************************************************************/
void lcd_pwm_renew (void)
{
  //lcd обмеження параметрів поморгування
  for (int i=0;i<2;i++){
    if(N.lcd.ON[i] > N.lcd.T[i]-2) N.lcd.ON[i] = N.lcd.T[i]>>1;
  }
  
  if(N.lcd.contrast < 0) N.lcd.contrast = 50;
  if(N.lcd.contrast > 100) N.lcd.contrast = 100;
  if(N.lcd.bright < 0) N.lcd.bright = 50;
  if(N.lcd.bright > 100) N.lcd.bright = 100;
    
  //*** renew the pwm frequency ***/
  pwmLim = 10000000000 / 1000;//1000Hz
  if(pwmLim % 100 >=50)pwmLim += 100;
  pwmLim /= 100;
  if(MCLIM2 != pwmLim){MCLIM2 = pwmLim;}

  MCMAT0 = 200 - N.lcd.bright*2;
  MCMAT1 = 200 - N.lcd.contrast*2;  
}
/******************************************************************************/
/******************************************************************************/
void pwm_20mA_out (int refB)
{
  //*** renew the pwm frequency ***/  
  pwmLim = 10000000000 / 1000 / (PWM1PR+1);//1000Hz
  if(pwmLim % 100 >=50)pwmLim += 100;
  pwmLim /= 100;
  if(PWM1MR0 != pwmLim){PWM1MR0 = pwmLim; PWM1LER |= 0x01;}
  //0-20mA - вихід
  PWM1MR3 = refB * pwmLim/3300;//перерахунок в біти шим
  PWM1LER |= 1<<3;
}    
/******************************************************************************/
/******************************************************************************/
void UARTs_PostInit (void)
{
  //перевірка параметрів внутрішнього інтерфейсу
  
  if(N.UART0.bitrate <= 0)N.UART0.bitrate = 0; if(N.UART0.word_length <= 0)N.UART0.word_length = 8; if(N.UART0.stop_bit <= 0)N.UART0.stop_bit = 1; if(N.UART0.parity_enable < 0)N.UART0.parity_enable = 0; if(N.UART0.parity_select < 0)N.UART0.parity_select = 0; if(N.UART0.wait_respond <= 0)N.UART0.wait_respond = 1; if(N.UART0.timeout <= 0)N.UART0.timeout = 1;
  if(N.UART0.bitrate > 6)N.UART0.bitrate=6; if(N.UART0.word_length > 8)N.UART0.word_length=8; if(N.UART0.stop_bit > 1)N.UART0.stop_bit=1; if(N.UART0.parity_enable > 1)N.UART0.parity_enable=1;if(N.UART0.parity_select > 3)N.UART0.parity_select=3;
  //if(N.UART1.bitrate > 6)N.UART1.bitrate=6; if(N.UART1.word_length > 8)N.UART1.word_length=8; if(N.UART1.stop_bit > 1)N.UART1.stop_bit=1; if(N.UART1.parity_enable > 1)N.UART1.parity_enable=1;if(N.UART1.parity_select > 3)N.UART1.parity_select=3;
  //if(N.UART2.bitrate > 6)N.UART2.bitrate=6; if(N.UART2.word_length > 8)N.UART2.word_length=8; if(N.UART2.stop_bit > 1)N.UART2.stop_bit=1; if(N.UART2.parity_enable > 1)N.UART2.parity_enable=1;if(N.UART2.parity_select > 3)N.UART2.parity_select=3;
  //if(N.UART3.bitrate > 6)N.UART3.bitrate=6; if(N.UART3.word_length > 8)N.UART3.word_length=8; if(N.UART3.stop_bit > 1)N.UART3.stop_bit=1; if(N.UART3.parity_enable > 1)N.UART3.parity_enable=1;if(N.UART3.parity_select > 3)N.UART3.parity_select=3;
  N.UART3.bitrate = 2;/*2-9600*/ N.UART3.word_length = 8; N.UART3.stop_bit=1; N.UART3.parity_enable=0; N.UART3.parity_select=0;

  uart0_setup(N.UART0.bitrate,N.UART0.word_length,N.UART0.stop_bit,N.UART0.parity_enable,N.UART0.parity_select);
  //uart1_setup(N.UART1.bitrate,N.UART1.word_length,N.UART1.stop_bit,N.UART1.parity_enable,N.UART1.parity_select);
  //uart2_setup(N.UART2.bitrate,N.UART2.word_length,N.UART2.stop_bit,N.UART2.parity_enable,N.UART2.parity_select);
  uart3_setup(N.UART3.bitrate,N.UART3.word_length,N.UART3.stop_bit,N.UART3.parity_enable,N.UART3.parity_select);
 
  modbus_uart0_init();
  //modbus_uart1_init();
  //timer_uart[1] = 1;
  //modbus_uart2_init();
  //timer_uart[2] = 100;
  modbus_uart3_init();
  N.UART3.timeout = 333;
  N.UART3.wait_respond = 1000;
  timer_uart[3] = 100;
}
/******************************************************************************/
void EXTERNAL_INT_init (void)
{
  //конфігурація зовн. переривань
  PINMODE4_bit.P2_10 = 2;//eint0 Pin has neither pull-up nor pull-down resistor enabled
  PINMODE4_bit.P2_11 = 0;//eint1 Pin has pull-up resistor enabled
  PINMODE4_bit.P2_12 = 0;//eint1 Pin has pull-up resistor enabled 2;//eint2 Pin has neither pull-up nor pull-down resistor enabled
  PINMODE4_bit.P2_13 = 0;//eint1 Pin has pull-up resistor enabled 2;//eint3 Pin has neither pull-up nor pull-down resistor enabled

  PINSEL4_bit.P2_10 = 1;//підключити EINT0 до вивода
  PINSEL4_bit.P2_11 = 1;//підключити EINT1 до вивода
  PINSEL4_bit.P2_12 = 1;//підключити EINT2 до вивода
  PINSEL4_bit.P2_13 = 1;//підключити EINT3 до вивода
  EXTMODE = 0xf;//eint0 eint1 eint2 eint3 -edge sensetive
  EXTPOLAR_bit.EXTPOLAR0 = 0;//Inj_on falling-edge sensitive

  //полярність датчика обертів
  EXTPOLAR_bit.EXTPOLAR1 = 0;// int1-falling поміняв логіку
 
  //EXTPOLAR_bit.EXTPOLAR2 = 0;// 0-int2-falling, 1-int2-rising
  EXTPOLAR_bit.EXTPOLAR3 = 1;// bip_in -rising

  NVIC_IntEnable(NVIC_EINT0);
  NVIC_IntEnable(NVIC_EINT1);
  NVIC_IntEnable(NVIC_EINT2);
  NVIC_IntEnable(NVIC_EINT3);
}  
/******************************************************************************/
void init_flash();
void AT25_MEMORY_init (void)
{
  //запис і читання
  do{
    spi1_rd_status_flash(0);
    while(spi1_sts_bit.end_rx == 0);
  }while(spi1_status_rg&0x01);//while write in progress(WIP)

  spi1_rd_status();
  while(spi1_sts_bit.end_rx == 0);

  spi1_init();
  while(spi1_sts_bit.end_rx == 0);

  //читаємо з рамтрона
  rd_rmtrn(0,&N,sizeof(N));
  while(spi1_sts_bit.end_rx == 0);

  if(N.password == 0xffffffff)init_flash();
  
  //lcd поморгування
  for (int i=0;i<2;i++){
    if(N.lcd.T[i] > 100) N.lcd.T[i] = 100;
    if(N.lcd.T[i] < 30) N.lcd.T[i] = 30;
    if(N.lcd.ON[i] > 90) N.lcd.ON[i] = 90;
    if(N.lcd.ON[i] < 20) N.lcd.ON[i] = 20;
  }

}
/******************************************************************************/
void init_flash()
{
  N.snap_time = 60;//часовий інтревал запису у архів
  N.archive_addr = 0;//адреса для перегляду архіву
  N.password = 0;

  N.ADC_COEF[0] = 6500;//коефіцієнти ацп
  N.ADC_COEF[1] = 6500;//коефіцієнти ацп
  N.ADC_COEF[2] = 6500;//коефіцієнти ацп
  N.ADC_COEF[3] = 0;//коефіцієнти ацп
  N.ADC_COEF[4] = 0;//коефіцієнти ацп
  N.ADC_COEF[5] = 0;//коефіцієнти ацп
  N.ADC_COEF[6] = 0;//коефіцієнти ацп
  N.ADC_COEF[7] = 0;//коефіцієнти ацп
  N.ADC_OFF[0] = 0;//зміщення для ацп
  N.ADC_OFF[1] = 0;//зміщення для ацп
  N.ADC_OFF[2] = 0;//зміщення для ацп
  N.ADC_OFF[3] = 0;//зміщення для ацп
  N.ADC_OFF[4] = 0;//зміщення для ацп
  N.ADC_OFF[5] = 0;//зміщення для ацп
  N.ADC_OFF[6] = 0;//зміщення для ацп
  N.ADC_OFF[7] = 0;//зміщення для ацп

  N.ADC_COEF[5] = 146;

  N.UART0.bitrate = 2;
  N.UART0.word_length = 8;
  N.UART0.stop_bit = 1;
  N.UART0.parity_enable = 0;
  N.UART0.parity_select = 0;
  N.UART0.wait_respond = 1;
  N.UART0.timeout = 1;
  N.UART0.simplex = 1;

  N.lcd.T[0] = 70;
  N.lcd.ON[0] = 55;
  N.lcd.contrast = 50;
  N.lcd.bright = 50;
  N.lcd.reanim = 1;
  N.Lng = 0;

  N.tyrOFF = 800;
  N.tyrON = 500;
  N.cTon = 300;
  N.cToff = 250;

  N.dsp[0].tC = 250;
  N.dsp[0].rH = 40;
  N.dsp[0].time = 3600;
  N.dsp[1].tC = 0;
  N.dsp[1].rH = 0;
  N.dsp[1].time = 0;
  N.dsp[2].tC = 0;
  N.dsp[2].rH = 0;
  N.dsp[2].time = 0;
  N.dsp[3].tC = 0;
  N.dsp[3].rH = 0;
  N.dsp[3].time = 0;
  N.dsp[4].tC = 0;
  N.dsp[4].rH = 0;
  N.dsp[4].time = 0;
  
  N.SCRRange = 0xE300;
  
  N.minI = 0;
  N.maxI = 3300;
    
  int tim;
  int *ptr;
  ptr = (int*)&N;

  for (int i=0;i < sizeof N; i+=32){
    spi1_init();
    while(spi1_sts_bit.end_rx == 0);

    wr_rmtrn(i, ptr, 32);
    while(spi1_sts_bit.end_rx == 0);

    do{
        spi1_rd_status_flash(0);
        while(spi1_sts_bit.end_rx == 0);
    }while(spi1_status_rg&0x01); //while write in progress(WIP)

    ptr =(int*)( (int)ptr + (int)32);
    tim = 500000;
    while(--tim){};

  }

  spi1_init();
  while(spi1_sts_bit.end_rx == 0);

  rd_rmtrn(0, &N, sizeof (N));
  while(spi1_sts_bit.end_rx == 0);
}
/******************************************************************************/
//запис в рамтрон уставок отриманих через інтерфейс
/******************************************************************************/
void ReWR_flash (void)
{
  int *sptr; // source pointer  
  int addr;  //address of next writting point in external flash
  int wrAmount; //volume in bytes to wite
  int WRcout;//count to next write
  int BusyCnt;
  
  if (ms.count && ms.addr >= &N.circSpd && ms.addr <= &N.maxI){//if modbus address is equal of modbus N.circSpd or bigger
    
    sptr = ms.addr; // source pointer  
    wrAmount = ms.count * sizeof(int);//volume in bytes to wite
    addr = (int)ms.addr - (int)&N;//starting address in external flash
    
    do {
      WRcout = 32 - addr % 32; // rest to end of page, the page size is 32 bytes
      if (WRcout > wrAmount) WRcout = wrAmount; 
      BusyCnt = 0;
      spi1_init(); while(spi1_sts_bit.end_rx == 0);
      wr_rmtrn(addr, sptr, WRcout); while(spi1_sts_bit.end_rx == 0);//start write process
      do { spi1_rd_status_flash(0); while(spi1_sts_bit.end_rx == 0); BusyCnt++;} while(spi1_status_rg&0x01); //while write in progress(WIP) 
      //read back before next write event because next block won't be written I do not know why
      rd_rmtrn(addr, sptr, WRcout);
      while(spi1_sts_bit.end_rx == 0);
    
      addr += WRcout; 
      wrAmount -= WRcout;
      sptr += WRcout / sizeof(int);      
    } while (wrAmount);
 
    ms.count = 0; // reset count to prevent write again
  }
  
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
  