
/*####################################################################################################*/
const MENU_MAN VOLT_REG[]={
  //{SUB_NUM,1,"outSCR0     %05d %%","",     &SCR0out, 0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"voltageAcc  %03d sec\351","",&N.VoltSpd, 1,999,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"Kv proport.   %04d ","",      &N.Kv,  0,9999,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"tv quant.      %03d s","",    &N.TqV, 1,999,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_BOOL,1,"Volt Regulator  %s ","",     &N.regVon,0,1,0,0,0,0,preEnterBool, 0, UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
  {SUB_NUM,1,"min.outSCR0 %05d %%","",     &N.minSCR0, 0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"max.outSCR0 %05d %%","",     &N.maxSCR0, 0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};
/*####################################################################################################*/

const MENU_MAN CURR_REG[]={
  //{SUB_NUM,1,"outSCR0     %05d %%","",     &SCR0out, 0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"current Sp.I  %04dA","", &rPwr.Csp,0,3000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"delayed Sp.I  %04dA","", &rPwr.iSp,0,3000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"currentAcc  %03d sec\351","",&N.CurrSpd, 1,999,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"Kc proport.  %05d ","",      &N.Kc,  0,99999,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"count tc quant.  %02d ","",    &N.TqC, 1,10,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_BOOL,1,"Curr.Regulator  %s ","",     &N.regCon,0,1,0,0,0,0,preEnterBool, 0, UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
  {SUB_NUM,1,"min. I lim    %04dA","", &N.minI,0,4000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"max. I lim    %04dA","", &N.maxI,0,4000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"min.outSCR0 %05d %%","",     &N.minSCR0, 0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"max.outSCR0 %05d %%","",     &N.maxSCR0, 0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};
/*####################################################################################################*/

/*####################################################################################################*/
const MENU_MAN SCR1[]={
  {SUB_NUM,1,"circul.Acc  %03d sec\351","", &N.circSpd, 1,999,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"Sp.SCR1     %05d %%","", &TyrReg.circ, 0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"TyrReg.SCR1 %05d %%","", &TyrReg.SCR1, 0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"min.outSCR1 %05d %%","", &N.minSCR1, 0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"max.outSCR1 %05d %%","", &N.maxSCR1, 0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};
/*####################################################################################################*/
const MENU_MAN SCR2[]={
  {SUB_NUM,1,"rel.Hum.SP     %03d %%","", &rhSp, 0,100,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"Krh proport.  %04d ","", &N.Krh,  0,9999,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"trh quant.     %03d s","", &N.TqRH, 1,999,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_BOOL,1,"RHregulator     %s ","", &N.regRHon,0,1,0,0,0,0,preEnterBool, 0, UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
  {SUB_NUM,1,"exhaustAcc  %03d sec\351","", &N.exhSpd, 1,999,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"Sp.SCR2     %05d %%","", &TyrReg.exhaust, 0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"TyrReg.SCR2 %05d %%","", &TyrReg.SCR2, 0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"min.outSCR2 %05d %%","", &N.minSCR2, 0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"max.outSCR2 %05d %%","", &N.maxSCR2, 0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};
/*####################################################################################################*/


