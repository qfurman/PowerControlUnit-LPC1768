#include <stdbool.h>
#include <myiolpc1768.h>

#include "type.h"
#include "main.h"
#include "timers.h"

#include "adc.h"
#include "ntc25_103_3435.h"
#include "Pt100.h"
#include "TCK.h"
#include "sin15b.h"
#include "SCRs.h"


int Tclamps, Theatsink;
int mV0, mV1, mV2, rtdT0, rtdT1, TC2;
int Ht;//humidity;
/*####################################################################################################*/
int ntcMF25_103_3435(int adcCode)
{
  int i;
  int temperature;//one fractional sigh
  
  for (i=1; i < sizeof(ntc25_103_3435)/sizeof(CvsT); i++ ){
    if ( adcCode >= ntc25_103_3435[i].code){
       temperature = (ntc25_103_3435[i].temp - ntc25_103_3435[i-1].temp)*1000 * ( adcCode - ntc25_103_3435[i].code ) / ( ntc25_103_3435[i-1].code - ntc25_103_3435[i].code );
       temperature =   ntc25_103_3435[i].temp*10 - temperature/100;
       return (temperature);
    }
  }
  return (0);
}
/*####################################################################################################*/
//для обчислення код ацп береться з буфера                                                         
int Calc_mV( int ADC, char Kg)                                                                     
{                                                                                                  
    //мВ - 3 знаки після коми                                                                      
    int mV = ((((long long)ADC * 2450000) >> Kg) >> 22);//сунемо два меньше щоб округлити          
    if(mV & 0x3 >= 2) mV = mV + 4;                                                                 
    mV = mV>>2;                                                                                    
    return mV;                                                                                     
}                                                                                                  
/*####################################################################################################*/
//для обчислення код ацп береться з буфера
int Calc_Ohm( int mV)
{
    int Ohm;

    Ohm = mV;// + ((mV%1 >= 1)? 1 : 0);   //ділимо напругу на 1 мА

    return Ohm;
}
/*####################################################################################################*/
int RTD_Pt100(int Ohm)
{
    int T0,T;
    int index;

    index = Ohm/3000;// /3 БО ТАБЛИЦЯ ЧЕРЕЗ 3 Ohm
    if(index > sizeof(Pt100)/sizeof(int)-2) index = sizeof(Pt100)/sizeof(int)-2; //має вказувати на на передостанню точку максимум
    T0 = Pt100[index];
    T = (Ohm - index*3000)*(Pt100[index+1] - T0)/3000 + T0;
    T = T/10 + ((T%10>=5)? 1 : 0);
    
    if (T < -999) T = -999;
    return T;
}
/*####################################################################################################*/
int Termocuple_K (int mV, int Tcool)
{
    int T0,T,uV;
    char index;

    //обмеження термокомпенсації
    if(Tcool > 1000) Tcool = 1000;
    if(Tcool < -500) Tcool = -500;

    if(Tcool >= 0){
        //при додатніх температурах
        index = Tcool/100;
        uV = (Tcool - index*100)*(uV_K[index+1] - uV_K[index])/100 + uV_K[index];
    }else{
        index = Tcool/-50;
        uV = (Tcool + index*50)*(uV_Kn[index+1] + uV_Kn[index])/(-50) + uV_Kn[index];
    }

    mV = mV + uV; // термокомпенсація  зврахуванням того що uV в мкВ а mV в тисячних мВ

    if(mV < 0) return 0;
    //if(mV > 54000) return 99999;//зашкал


    index = mV>>9;// /512 БО ТАБЛИЦЯ ЧЕРЕЗ 0.512 мВ
    if(index > sizeof(TCK)/sizeof(int)-2) index = sizeof(TCK)/sizeof(int)-2; //має вказувати на на передостанню точку максимум
    T0 = TCK[index];
    T = (mV - index*512)*(TCK[index+1] - T0)/512 + T0;
    T = T/10 + ((T%10>=5)? 1 : 0);
    return T;
}
/*####################################################################################################*/

/******************************************************************************/
//стовбчики - сухий термометр
const char Hc[]={10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50};
//рядки - різниця між сухим і мокрим термометрами
const short Hr[]={0,5,10,15,20,25,30,35,40,50,60,70,80,90,100,110,120,130,140,150,160,170,180,190,200,210,220,230,240,250,260};
//відносна вологість
const char humidity[31][21]={
/*
 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50*/  
 {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100},
  {94, 95, 95, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96},
  {88, 89, 90, 91, 91, 91, 91, 92, 92, 93, 93, 93, 93, 93, 94, 94, 94, 95, 95, 95, 95},
  {82, 84, 85, 86, 86, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87},
  {76, 78, 80, 81, 81, 82, 83, 84, 84, 85, 86, 86, 87, 87, 88, 89, 89, 90, 90, 90, 90},
  {71, 73, 75, 77, 78, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79},
  {65, 68, 70, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 81, 82, 82, 83, 84, 84, 84, 84},
  {60, 63, 65, 67, 69, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70},
  {54, 57, 60, 62, 64, 66, 68, 69, 70, 71, 72, 74, 75, 75, 76, 77, 78, 79, 79, 79, 79},
  {44, 48, 51, 54, 56, 68, 60, 62, 64, 65, 66, 68, 69, 69, 70, 71, 72, 73, 74, 74, 74},
  {34, 38, 42, 46, 48, 51, 54, 56, 58, 59, 61, 62, 63, 64, 65, 66, 68, 68, 70, 70, 70},
  {24, 29, 34, 38, 41, 44, 45, 49, 51, 53, 55, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66},
  {15, 20, 25, 30, 34, 36, 40, 43, 45, 47, 50, 52, 53, 54, 55, 56, 58, 59, 60, 60, 60},
  { 0, 11, 18, 23, 27, 30, 34, 37, 40, 42, 44, 46, 48, 49, 50, 51, 53, 54, 55, 56, 58},
  { 0,  0, 10, 16, 20, 24, 28, 31, 34, 37, 40, 42, 44, 45, 46, 47, 49, 50, 51, 52, 54},
  { 0,  0,  0,  8, 13, 17, 22, 26, 29, 32, 35, 37, 39, 40, 42, 43, 45, 46, 47, 48, 50},
  { 0,  0,  0,  0,  6, 11, 16, 20, 24, 27, 30, 32, 34, 36, 38, 40, 42, 43, 45, 46, 47},
  { 0,  0,  0,  0,  0,  0,  9, 15, 19, 22, 25, 27, 30, 32, 34, 36, 38, 39, 41, 42, 44},
  { 0,  0,  0,  0,  0,  0,  0,  9, 14, 17, 20, 24, 26, 29, 31, 32, 35, 36, 38, 39, 41},
  { 0,  0,  0,  0,  0,  0,  0,  0,  9, 14, 17, 20, 22, 25, 28, 29, 31, 33, 34, 36, 37},
  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  9, 13, 16, 19, 21, 24, 25, 28, 30, 31, 33, 34},
  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8, 12, 15, 18, 20, 23, 25, 27, 28, 30, 31},
  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8, 12, 15, 17, 19, 22, 24, 25, 27, 29},
  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  9, 12, 14, 17, 19, 21, 22, 24, 26},
  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8, 11, 14, 16, 18, 20, 22, 24},
  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8, 11, 13, 15, 17, 19, 21},
  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8, 11, 13, 15, 17, 19},
  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10, 10, 12, 14, 16},
  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8, 10, 12, 14},
  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10, 12},
  { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8, 10},
};
/******************************************************************************/
unsigned short Humid(unsigned short Tdry, unsigned short Twet)
{
  char c,r;
  int delta, hor1, hor2;
  int result;
  
  //шукаємо стовбчик
  if(Tdry >= 100 && Tdry <=500){
    for(c=0;c<sizeof(Hc);c++){
      if(Hc[c]*10 >= Tdry)break;
    }
  }else return 0;//неможливо визначити вологість
  
  if(c == sizeof(Hc))return 0;//неможливо визначити вологість
  
  delta = Tdry - Twet;
  if(delta > 260)return 0;//неможливо визначити вологість
  if(delta < 0)return 100;//неможливо визначити вологість
  
  //шукаємо рядок
  for(r=0;r<sizeof(Hr)/sizeof(short);r++){
      if(Hr[r] >= delta)break;// знайшовся рядок
  }
  
  //уточняємо по горизонталі
  if(c != 0 && r != 0){
    //якщо не перший стовбчик то можна віднімати
    //беремо різницю вологостей множимо на різницю температур поділену на крок температур в таблиці
    hor2 = (humidity[r][c] - humidity[r][c-1]) * ( (Tdry-Hc[c-1]*10) /2);
    hor2 = hor2 + humidity[r][c-1] *10;

    hor1 = (humidity[r-1][c] - humidity[r-1][c-1]) * ( (Tdry-Hc[c-1]*10) /2);
    hor1 = hor1 + humidity[r-1][c-1] *10;

    result = hor1 - ( (hor1 - hor2) * ( ((Tdry - Twet) - Hr[r-1])*10/(Hr[r] - Hr[r-1]) ) ) / 10;
  }else result = humidity[r][c]*10;//бо таблиця без десятих
  //округлити і відсікти "дробову" частину
  if(result % 10 >= 5)result = result/10+1;
  else result = result/10;

  return result;
}
/******************************************************************************/
/*===============================================================================================*/
RUNPID runPid;
/*===============================================================================================*/
void PID_regulator (int tC)
{
  if(N.PID.ON){
    
    if (runPid.just_on){
      runPid.last_t = tC;       //попереднє значення температури для обчислення диф. складової
      runPid.rSp = N.PID.spT;   //rewite setpoint without delay
      //runPid.out = TyrReg.SCR0; //write current output
      TyrReg.circ = 10000; //start circulation fan
      runPid.just_on = 0;
    }
    
    if (PID_flag){
      PID_flag = 0;
      if ( runPid.rSp < N.PID.spT) {//shockless set point changing
        runPid.rSp += N.PID.speed;
        if (runPid.rSp > N.PID.spT)runPid.rSp = N.PID.spT;
      }
      if ( runPid.rSp > N.PID.spT) {//shockless set point changing
        runPid.rSp -= N.PID.speed;
        if (runPid.rSp < N.PID.spT)runPid.rSp = N.PID.spT;
      }
      
      runPid.ie += runPid.rSp - tC;//помилка регулювання інтегрування
      
      //пропорційна частина+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      if(--runPid.Tq <= 0){      
        runPid.Tq = N.PID.Tq;//перезапустити таймер
        if(runPid.Tq < 1) runPid.Tq = 1;
        runPid.P = (runPid.rSp - tC) * N.PID.K / 1000;        
        if(runPid.P)runPid.change = 1;
        runPid.lastP = runPid.P;
      }
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      
      //інтегральна частина~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      if(--runPid.Ti <= 0){      
        runPid.Ti = N.PID.Ti;//перезапустити таймер
        if(runPid.Ti < 1) runPid.Ti = 1;        
        runPid.I =  runPid.ie * N.PID.Ki/N.PID.Ti / 1000; 
        runPid.ie = 0;//обнулити накопичену помилку
        if(runPid.I)runPid.change = 1;
        runPid.lastI = runPid.I;
      }
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      
      //диф. частина----------------------------------------------------------------------
      if(--runPid.Td <= 0){      
        runPid.Td = N.PID.Td;//перезапустити таймер
        if(runPid.Td < 1) runPid.Td = 1;        
        runPid.D = (runPid.last_t - tC)* N.PID.Kd/N.PID.Td / 10;        
        if(runPid.D)runPid.change = 1;
        runPid.last_t = tC;//попереднє значення температури для обчислення диф. складової
        runPid.lastD = runPid.D;      
      }        
      //----------------------------------------------------------------------------------
    }
  } else{ runPid.just_on = 1; }

  if(runPid.change){
    //сумування всіх результатів 
    runPid.out = runPid.out + runPid.P + runPid.I + runPid.D;
    if(runPid.out > N.PID.max) runPid.out = N.PID.max;
    if(runPid.out < N.PID.min) runPid.out = N.PID.min;
    runPid.change = 0;
    runPid.P = 0;
    runPid.I = 0; 
    runPid.D = 0;       
  }
}
/*============================================================================*/
void putPIDoutput (void)
{
  if ( N.PID.ON) {
    N.regCon = 1; // if PID regulator is on current regulator switch on
    //limiting and ranging
    if ( N.minI < 0) N.minI = 0;
    if ( N.maxI < N.minI) N.maxI = 3500;
    rPwr.Csp = runPid.out * (N.maxI - N.minI) / 10000 + N.minI; //write result as setpoint for current regulator
  }
}
/*============================================================================*/
int DLSCR1;//delayed SCR1 setpoint
/*============================================================================*/
void circulationFAN (void)
{
  int scr;
  
  if ( scr1_flag){ 
    scr1_flag = 0;//one second flag for scr1 output
    
    if (TyrReg.circ == 0) {// if circulation OFF
      TyrReg.SCR1 = 0; 
      DLSCR1 = 0; 
      return;
    }
    //slowly approach
    if ( DLSCR1 < TyrReg.circ) {
      DLSCR1 += N.circSpd;
      if (DLSCR1 > TyrReg.circ) DLSCR1 = TyrReg.circ;
    }
    if ( DLSCR1 > TyrReg.circ) {
      DLSCR1 -= N.circSpd;
      if (DLSCR1 < TyrReg.circ) DLSCR1 = TyrReg.circ;
    }
    
    //limiting and ranging
    if ( N.minSCR1 < 0) N.minSCR1 = 0;
    if ( N.maxSCR1 < N.minSCR1) N.maxSCR1 = 10000;
    scr = DLSCR1 * (N.maxSCR1 - N.minSCR1) / 10000 + N.minSCR1;
    TyrReg.SCR1 = scr;
  }
}
/*============================================================================*/
/*============================================================================*/
int DLSCR2;//delayed SCR2 setpoint
/*============================================================================*/
void exhaustFAN (void)
{
  int scr;
  
  if ( scr2_flag){ 
    scr2_flag = 0;//one second flag for scr2 output
    
    if (TyrReg.exhaust == 0) {// if exhaust OFF
      TyrReg.SCR2 = 0; 
      DLSCR2 = 0; 
      return;
    }
    //slowly approach
    if ( DLSCR2 < TyrReg.exhaust) {
      DLSCR2 += N.exhSpd;
      if (DLSCR2 > TyrReg.exhaust) DLSCR2 = TyrReg.exhaust;
    }
    if ( DLSCR2 > TyrReg.exhaust) {
      DLSCR2 -= N.exhSpd;
      if (DLSCR2 < TyrReg.exhaust) DLSCR2 = TyrReg.exhaust;
    }
    
    //limiting and ranging
    if ( N.minSCR2 < 0) N.minSCR2 = 0;
    if ( N.maxSCR2 < N.minSCR2) N.maxSCR2 = 10000;
    scr = DLSCR2 * (N.maxSCR2 - N.minSCR2) / 10000 + N.minSCR2;
    TyrReg.SCR2 = scr;
  }
}
/*============================================================================*/
/*============================================================================*/
int rhTime; // timer for next works of regulator
int rhSp;// runtime setpoint for humidity regulator
/*============================================================================*/
void relHumidRegul (void)
{
  int h;
  
  if (N.TqRH < 1) N.TqRH = 1;       // limiting the quantization time
  if (N.TqRH > 300) N.TqRH = 300;
  
  if (N.regRHon == 0) {RH_flag = 0; return;} // if not ON do not works
  if (RH_flag){
    RH_flag = 0;
    if (rhTime < N.TqRH) rhTime++;
    else{
      rhTime = 0;//reset time to next work
      h = (Ht - rhSp) * N.Krh; 
      h = h / 100;
      if (h % 10 >= 5) h = h/10 + 1 ;
      else h /= 10;
      
      h = h + TyrReg.exhaust;
      if (h < 0) h = 0;
      if (h > 10000) h = 10000;
      TyrReg.exhaust = h; // new setup speed of exhauster fan
    }
  }
}
/*============================================================================*/
/*============================================================================*/
#define SCRMAX 0xE800
// function for normalisation of sinus 
// input is 0.00-100.00% and range for write to power regisrer
int normSin (int percent, int range)
{
  unsigned int i, k, m;
  
  if (percent == 0) return 0xffff;
  if (percent == 10000) return 3;
  
  if (percent > 5000) i = 10000 - percent;
  else i = percent;
  
  if (i < 5000){
    i = i*2;// for 0-10000 
    k = i / 100;
    m = sin15b[k+1] - sin15b[k];
    m = m * (i%100);
    m = m / 100 +((m%100 >= 50)? 1 : 0 );
    m += sin15b[k];
  } else m = sin15b[100];
 
  if (percent > 5000) k = (32768 - m) + 32767;
  else k = m;
  
  m = k * range;
  k = (m >> 16) + ((m>>15)? 1 : 0 );
  k = range - k;//change the direction
  return k;
}

/*============================================================================*/

