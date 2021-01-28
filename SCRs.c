
#include <myiolpc1768.h>
#include "type.h"
#include "main.h"
#include "temperatures.h"
#include "adc.h"
#include "timers.h"

/******************************************************************************/
void SCRsControl (void)
{
    int i;
    
    //TyrReg.circ = normSin (TyrReg.exhaust, N.SCRRange);
    //write to SCR0
    FIO2CLR_bit.PWRS = 1;//select PWR
    FIO2CLR_bit.PWRS1 = 1;//select writes to SCR0
    FIO1SET_bit.PWRS2 = 1;//unselect writes to SCR1
    if (TyrReg.enable) FIO2CLR_bit.Rw = 1;//selects to write PWR
    else FIO2SET_bit.Rw = 1;//unselects to write PWR
    SSP0CR0_bit.DSS = 15;//1111 - 16-bit transfer
    while(SSP0SR_bit.BSY); 
    SSP0DR = normSin (TyrReg.SCR0, N.SCRRange);//SSP0DR = TyrReg.SCR0;
    while(SSP0SR_bit.BSY);

    do{
      i = SSP0DR;
    }while(SSP0SR_bit.RNE);
    
    FIO2SET_bit.Rw = 1;//unselects to write PWR
    
    SSP0DR = 0xFFFF;
    while(SSP0SR_bit.BSY);
    TyrReg.read = SSP0DR_bit.DATA;// read back
    if (TyrReg.enable == 0) {TyrReg.SCR0 = 0;/*TyrReg.read*/; TyrReg.enable = 1;}//read after poweron BACK TRANSFORMATION IS NEEDED
    FIO2SET_bit.PWRS = 1;//unselect writes to PWR 
    FIO2SET_bit.PWRS1 = 1;//unselect writes to SCR1    
    FIO1SET_bit.PWRS2 = 1;//unselect writes to SCR2    
    
    //write to SCR1
    if (TyrReg.enable){
      FIO2CLR_bit.PWRS = 1;//select PWR
      FIO2SET_bit.PWRS1 = 1;//select write to SCR1
      FIO1SET_bit.PWRS2 = 1;//unselect writes to SCR2
      if (TyrReg.enable) FIO2CLR_bit.Rw = 1;//selects to write PWR
      else FIO2SET_bit.Rw = 1;//unselects to write PWR
      SSP0CR0_bit.DSS = 15;//1111 - 16-bit transfer
      while(SSP0SR_bit.BSY); 
      SSP0DR = normSin (TyrReg.SCR1, N.SCRRange);
      while(SSP0SR_bit.BSY);
      do {i = SSP0DR;} while(SSP0SR_bit.RNE);
      
      FIO2SET_bit.Rw = 1;//unselects to write PWR
      FIO2SET_bit.PWRS1 = 1;//unselect writes to SCR1    
      FIO1SET_bit.PWRS2 = 1;//unselect writes to SCR2
      FIO2SET_bit.PWRS = 1;//unselect writes to PWR    
    }

    //write to SCR2
    if (TyrReg.enable){
      FIO2CLR_bit.PWRS = 1;//select PWR
      FIO2SET_bit.PWRS1 = 1;//unselect write to SCR1
      FIO1CLR_bit.PWRS2 = 1;//select writes to SCR2
      if (TyrReg.enable) FIO2CLR_bit.Rw = 1;//selects to write PWR
      else FIO2SET_bit.Rw = 1;//unselects to write PWR
      SSP0CR0_bit.DSS = 15;//1111 - 16-bit transfer
      while(SSP0SR_bit.BSY); 
      SSP0DR = normSin (TyrReg.SCR2, N.SCRRange);
      while(SSP0SR_bit.BSY);
      do {i = SSP0DR;} while(SSP0SR_bit.RNE);
      
      FIO2SET_bit.Rw = 1;//unselects to write PWR
      FIO2SET_bit.PWRS1 = 1;//unselect writes to SCR1    
      FIO1SET_bit.PWRS2 = 1;//unselect writes to SCR2
      FIO2SET_bit.PWRS = 1;//unselect writes to PWR    
    }
    if ( i ) i =0;
    SSP0CR0_bit.DSS = 8;//1000 9-bit transfer

}
/******************************************************************************/
void CoolerFan_OnOff (void)
{
    //cooler fan ON/OFF
    if ( N.cTon > 600) N.cTon = 600;
    if ( N.cTon < 100) N.cTon = 100;
    if ( N.cToff < 0)  N.cToff = 0;
    if ( N.cToff >= N.cTon) N.cToff = N.cTon - 50;
    if ( Theatsink >= N.cTon) FIO0CLR_bit.COOL = 1; /*switch ON*/
    else { if ( Theatsink < N.cToff) FIO0SET_bit.COOL = 1; /* swith OFF*/}
}
/******************************************************************************/
RUNPOWER rPwr; //runtime parameters for voltage and current regulation
/********* regulator of current for scr0 channel ******************************/
void CurrentRegul (void)
{
  int dc;
  
  if ( rPwr.Csp > N.maxI ) rPwr.Csp = N.maxI; // limiting I max
      
  if (N.TqC < 1) N.TqC = 1; if (N.TqC > 10) N.TqC = 10;//limiting the number of periods for averaging
  if (rPwr.Iptr > N.TqC-1) rPwr.Iptr = 0;
  rPwr.Ibuff [rPwr.Iptr] = ISCR.A;
  rPwr.Iptr++;
    
  if (rPwr.Iptr == N.TqC){//if number of needed periods reached then doing averaging
    dc = 0;
    for (int i=0; i<rPwr.Iptr; i++){
      dc += rPwr.Ibuff [i];
    }
    dc = dc / rPwr.Iptr;     
    dc = rPwr.iSp - dc;//- Csp ISCR.A;//difference between setup and actual
    dc = dc * N.Kc / 1000;
    rPwr.lastDi = dc; // last delta of regulator of current
    dc = dc + TyrReg.SCR0;
    if (dc < N.minSCR0) dc = N.minSCR0;
    if (dc > N.maxSCR0) dc = N.maxSCR0;
    if (rPwr.Csp == 0) dc = 0;//if setpoint zero write 0 immediately
    TyrReg.SCR0 = dc;    
  }
}
/******************************************************************************/
// mains voltage correction 
/******************************************************************************/
void MainsVoltFollow (void)
{
  int cr;
  
  if (TyrReg.SCR0 == 0) return;
  cr = TyrReg.SCR0 + rPwr.Ucorr; //add mains voltage correction 
  TyrReg.SCR0 = rPwr.Ucorr;
  if (cr < N.minSCR0) cr = N.minSCR0;
  if (cr > N.maxSCR0) cr = N.maxSCR0;
  TyrReg.SCR0 = cr;
}
/******************************************************************************/
//shockless current setpoint changing
/******************************************************************************/
void IspApproach (void)
{
  if (iSp_flag){
    iSp_flag = 0;
    if ( rPwr.Csp == 0) rPwr.iSp =0;//write zero immedeatly
    if ( rPwr.iSp < rPwr.Csp) {//shockless set point changing
      rPwr.iSp += N.CurrSpd;
      if (rPwr.iSp > rPwr.Csp) rPwr.iSp = rPwr.Csp;
    }
    if ( rPwr.iSp > rPwr.Csp) {//shockless set point changing
      rPwr.iSp -= N.CurrSpd;
      if (rPwr.iSp < rPwr.Csp) rPwr.iSp = rPwr.Csp;
    }
  }
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
