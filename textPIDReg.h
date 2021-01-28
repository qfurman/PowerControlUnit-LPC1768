
/*####################################################################################################*/
const MENU_MAN PID_PARAM[]={
  {SUB_NUM,0,"t\337C           %03d \337","", &rtdT0, 0,1200,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"setpoint      %03d \337","", &N.PID.spT, 0,1200,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"change speed %02d \337/s","", &N.PID.speed,  1,99,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"min output  %05d %%","", &N.PID.min,  0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"max output  %05d %%","", &N.PID.max,  10,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"K proport.    %04d ","", &N.PID.K,  0,9999,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"t quant.      %03d s","", &N.PID.Tq, 1,999,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"K integr.     %04d ","", &N.PID.Ki,  0,9999,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"t integr.     %03d s","", &N.PID.Ti, 1,999,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"K deriv.     %05d ","", &N.PID.Kd,  0,65535,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"t deriv.      %03d s","", &N.PID.Td,  1,999,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"pid output  %05d %%","", &runPid.out, 0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"P  result    %+05d%%","", &runPid.lastP,0,1000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"I  result    %+05d%%","", &runPid.lastI,0,1000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"D  result    %+05d%%","", &runPid.lastD,0,1000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},};
/*####################################################################################################*/

/*####################################################################################################*/
const MENU_MAN RUN_pid_PARAM[]={
  {SUB_NUM,0,"t\337C           %03d \337","", &rtdT0, 0,1200,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"run setpoint  %03d \337","", &runPid.rSp, 0,1200,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"setpoint      %03d \337","", &N.PID.spT, 0,1200,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"change speed %02d \337/s","", &N.PID.speed, 1,99,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_BOOL,1,"regulator       %s ","", &N.PID.ON,0,1,0,0,0,0,preEnterBool, 0, UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
  {SUB_NUM,0,"P  result    %+05d%%","", &runPid.lastP,0,1000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"I  result    %+05d%%","", &runPid.lastI,0,1000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"D  result    %+05d%%","", &runPid.lastD,0,1000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"pid output  %05d %%","",     &runPid.out, 0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT}, 
  {SUB_NUM,1,"current Sp.I  %04dA","", &rPwr.Csp,0,3000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"load current  %04dA","", &ISCR.A,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};
/*####################################################################################################*/

/*####################################################################################################*/
const MENU_MAN PID_MENU[]={
    {SUB_NUM,0,"31 RUNTIME    PID   "," ",0,0,0,0,(void*)&RUN_pid_PARAM,sizeof(RUN_pid_PARAM)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
    {SUB_NUM,0,"32 PARAMETERS PID   "," ",0,0,0,0,(void*)&PID_PARAM,sizeof(PID_PARAM)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
  	{SUB_NUM,0,"33 V/I SUMMARY      "," ",0,0,0,0,(void*)&V_I_SUMMARY,sizeof(V_I_SUMMARY)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},    
    {SUB_NUM,0,"34 VOLTAGE REGULATOR"," ",0,0,0,0,(void*)&VOLT_REG,sizeof(VOLT_REG)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
    {SUB_NUM,0,"35 CURRENT REGULATOR"," ",0,0,0,0,(void*)&CURR_REG,sizeof(CURR_REG)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
};
/*####################################################################################################*/
