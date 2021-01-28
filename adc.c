#include <myiolpc1768.h>
#include <stdbool.h>
#include "initTools.h"
#include "type.h"
#include "timers.h"
#include "main.h"
#include "SCRs.h"

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
ADCMNG R380, ISCR, RSCR;//rectified load voltage, load current and grid voltage
/******************************************************************************/
int adcI;
int flag_a;
#define BufSz 64
/******************************************************************************/
void ADC_IRQHandler()
{
  int dummy;

  dummy = AD0GDR;
  if(dummy)dummy=0;  

  R380.acc += ADDR0_bit.RESULT;  
  ISCR.acc += ADDR1_bit.RESULT;  
  RSCR.acc += ADDR2_bit.RESULT;  
  //adc_a.t_acc[3] += ADDR3_bit.RESULT;  
  //adc_a.t_acc[4] += ADDR4_bit.RESULT;  
  //adc_a.t_acc[5] += ADDR5_bit.RESULT;
  //adc_a.t_acc[6] += ADDR6_bit.RESULT;  
  //adc_a.t_acc[7] += ADDR7_bit.RESULT;  
  //AD0CR_bit.BURST = 1;//BURST start convertion now

  //якщо відміряли 20mS
  if(++adcI== 200){
    adcI = 0;
    
    R380.buf [R380.b][R380.i] = R380.acc;
    R380.acc = 0;
    R380.i = (R380.i+1);
    if (R380.i >= BufSz) { R380.i = 0; R380.b = !R380.b; R380.F = 1;}
    
    ISCR.buf [ISCR.b][ISCR.i] = ISCR.acc;
    ISCR.acc = 0;
    ISCR.i = (ISCR.i+1);
    if (ISCR.i >= BufSz) { ISCR.i = 0; ISCR.b = !ISCR.b; ISCR.F = 1;}
    
    RSCR.buf [RSCR.b][RSCR.i] = RSCR.acc;//rectified input voltage  from power greed
    RSCR.acc = 0;
    RSCR.i = (RSCR.i+1);
    if (RSCR.i >= BufSz) { RSCR.i = 0; RSCR.b = !RSCR.b; RSCR.F = 1;}
    
  }
  flag_a = !flag_a;

  if(flag_a)FIO1SET_bit.P1_30 = 1;//вихід для превірки часу спрацювання АЦП
  else FIO1CLR_bit.P1_30 = 1;//вихід для превірки часу спрацювання АЦП

}
/******************************************************************************/
void adc_init()
{
  
  PCONP_bit.PCAD = 1;//A/D converter (ADC) power/clock control bit
  NVIC_IntEnable(NVIC_ADC);//Enable NVIC ADC Interrupt
  //підключення входів АЦП
  //PINSEL0_bit.P0_2 = 2;//ch7 TXD0
  //PINSEL0_bit.P0_3 = 2;//ch6 RXD0
  PINMODE1_bit.P0_23 = 2;//pin has neither pull-up nor pull-down
  PINSEL1_bit.P0_23 = 1;//ch0 R380
  PINMODE1_bit.P0_24 = 2;//pin has neither pull-up nor pull-down
  PINSEL1_bit.P0_24 = 1;//ch1 I_SCR
  PINMODE1_bit.P0_25 = 2;//pin has neither pull-up nor pull-down
  PINSEL1_bit.P0_25 = 1;//ch2 R_SCR
  PINMODE1_bit.P0_26 = 2;//pin has neither pull-up nor pull-down
  PINSEL1_bit.P0_26 = 2;//AOUT ch3
  DACR_bit.VALUE = 0x0;//записати в ЦАП, макс. зачення - 0x3ff
  //PINSEL3_bit.P1_30 = 3;//ch4 не використовувати
  //PINMODE3_bit.P1_31 = 2;//pin has neither pull-up nor pull-down
  //PINSEL3_bit.P1_31 = 3;//ch5 не використовувати

  FIO1DIR_bit.P1_30 = 1;//вихід AD0.6 на схемі (хоч реально він AD0.4) для превірки часу спрацювання АЦП
  
  //АЦП 0
  //Initialises the ADC module.
  AD0CR_bit.PDN = 1;//The A/D converter is operational.
  AD0CR_bit.SEL = 0x07;//канал 0,1,2,-,-,-,-,-
  //AD0CR_bit.CLKDIV = 0x18;// f/25 256uS на одне 4-канальне перетворення
  AD0CR_bit.CLKDIV = 0x0c;// 12 - f/13 / 65/3 =100uS на одне 3-канальне перетворення
  ADINTEN_bit.ADGINTEN = 0;//Only the global DONE flag in ADDR is disable to generate an interrupt
  ADINTEN_bit.ADINTEN0 = 1;//Completion of a conversion on ADC channel 2 will generate an interrupt.
  AD0CR_bit.BURST = 1;//BURST start convertion now
  
  R380.i = 0;//buff ptr     
  ISCR.i = 0;//buff ptr
  RSCR.i = 0;//buff ptr
    
}
/******************************************************************************/
//знаходження середнього значення для кожного каналу
//опорна напруга у Вольтах * 1000
#define VREF 3300

void meen_adc()
{
  char k;
  int acc1=0, acc2=0, acc3=0;
  
  //rectified input voltage  from power greed
  if (RSCR.F){
    for (k=0; k<BufSz; k++){
      acc1 += RSCR.buf [!RSCR.b][k];
    }    
    RSCR.F = 0;
    acc1 = acc1 / (BufSz*20);//code adc  
    RSCR.code = acc1 / 10 + ((acc1 % 10 >= 5)? 1 : 0);
    rPwr.prUm = RSCR.A;//previous mains voltage
    if (RSCR.code + N.ADC_OFF[0] > 0) RSCR.A = ((RSCR.code + N.ADC_OFF[0]) *N.ADC_COEF[0])>>12; 
    else RSCR.A = 0;
    int du = (rPwr.prUm - RSCR.A) * 10000; // 100.00%
    du = du * N.Kv / 1000 / (rPwr.prUm + RSCR.A) / 2; // (0...9.999) / mains 
    rPwr.Ucorr = du;// new output correction
    rPwr.lastDu = du; // last delta of mains voltage follower
    if (N.regVon) MainsVoltFollow ();// mains voltage correction 
  }

  if (ISCR.F){
    for (k=0; k<BufSz; k++){
      acc2 += ISCR.buf [!ISCR.b][k];
    }    
    ISCR.F = 0;
    acc2 = acc2 / (BufSz*20);//code adc 
    ISCR.code = acc2 / 10 + ((acc1 % 10 >=5)? 1 : 0);
    if (ISCR.code + N.ADC_OFF[1] > 0) ISCR.A = ((ISCR.code + N.ADC_OFF[1]) *N.ADC_COEF[1])>>12; 
    else ISCR.A = 0;
    if (N.regCon) CurrentRegul ();// cuurent regulator
    //resistance calculation
    if (ISCR.A > 5){
      int dc = R380.A * 1000 / ISCR.A;//one digit after point
      rPwr.R = dc/10 + ((dc % 10)? 1 : 0);
    }else rPwr.R = 9999;// 999.9 Ohm
  }
  
  if (R380.F){
    for (k=0; k<BufSz; k++){
      acc3 += R380.buf [!R380.b][k];
    }    
    R380.F = 0;
    acc3 = acc3 / (BufSz*20);//code adc  
    R380.code = acc3 / 10 + ((acc1 % 10 >=5)? 1 : 0);
    if (R380.code + N.ADC_OFF[2] > 0) R380.A = ((R380.code + N.ADC_OFF[2]) *N.ADC_COEF[2])>>12;
    else R380.A = 0;
  }
  
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

