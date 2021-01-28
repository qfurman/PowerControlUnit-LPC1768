/*####################################################################################################*/
char postEnter_aui0_ws_flash()
{
    manage = &map.menu_ptr[menu_status.input_row];
    scan(LCD_input_str);
    aum3.task = (0x02 << 0*2);//0- for_identification
    return 1;
}
/*####################################################################################################*/
char postEnter_aui0_ws_ram()
{
    manage = &map.menu_ptr[menu_status.input_row];
    scan(LCD_input_str);
    aum3.task = (0x01 << 0*2);//0- for_identification
    return 1;
}
/*####################################################################################################*/

const MENU_MAN AIU_CH0[]={
  /*{SUB_NUM,0,"           %06d mV",  (int*)&aiu_add[n].r.volt,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"            %05d mА", (int*)&aiu_add[n].r.curr,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"            %05d Ohm", (int*)&aiu_add[n].r.Ohm,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"            %05d \260",  (int*)&aiu_add[n].r.t,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_123,1,"%s",  (int*)&N.aiu_r[n].type,0,10,0,0,0,0,preEnter123, postEnter_123, UP_123, DOWN_123,UP_123, DOWN_123},
  {SUB_comp,1,"%s",  (int*)&N.aiu_r[n].comp,0,1, 0,0,0,0,preEnter123, postEnter_123, UP_comp, UP_comp, UP_comp, UP_comp},
  */
  //{SUB_HEX,0,"adc code      %06X","",   (int*)&aiu[0].adc,1,0xffffff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_HEX,1,"command         %04X","", (int*)&aiu[0].cmd,0,0xffffff,0,0,0,0,preEnterNum,postEnter_aui0_ws_ram,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_HEX,1,"dacs            %04X","", (int*)&aiu[0].dacs,0,0xffffff,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_NUM,0,"             %+04d \337","", &rtdT0,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"          %06d mV","",    &mV0,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_HEX,0,"adc0 buffcode %06X","",   (int*)&aiu[0].ADC0,1,0xffffff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_HEX,1,"offset0       %06X","",   (int*)&aiu[0].offset0,0,0xffffff,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_HEX,1,"gain0         %06X","",   (int*)&aiu[0].gain0,0,0xffffff,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_NUM,1,"amplifiring0       %1d","",   (int*)&aiu[0].Kg0,0,0x07,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP,DOWN,LEFT,RIGHT},
  //{SUB_NUM,1,"bufsize0          %02d","",   (int*)&aiu[0].K0,0,16,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"             %+04d \337","", &rtdT1,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"          %06d mV","",    &mV1,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_HEX,0,"adc1 bufcode  %06X","",   (int*)&aiu[0].ADC1,1,0xffffff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_HEX,1,"offset1       %06X","",   (int*)&aiu[0].offset1,0,0xffffff,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_HEX,1,"gain1         %06X","",   (int*)&aiu[0].gain1,0,0xffffff,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_NUM,1,"amplifiring1       %1d","",   (int*)&aiu[0].Kg1,0,0x07,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP,DOWN,LEFT,RIGHT},
  //{SUB_NUM,1,"bufsize1          %02d","",   (int*)&aiu[0].K1,0,16,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"             %+04d \337","", &TC2,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"          %06d mV","",    &mV2,0,0x7fffffff,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_HEX,0,"adc2 buffcode %06X","",   (int*)&aiu[0].ADC2,1,0xffffff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_HEX,1,"offset2       %06X","",   (int*)&aiu[0].offset2,0,0xffffff,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_HEX,1,"gain2         %06X","",   (int*)&aiu[0].gain2,0,0xffffff,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_NUM,1,"amplifiring2       %1d","",   (int*)&aiu[0].Kg2,0,0x07,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP,DOWN,LEFT,RIGHT},
  {SUB_BOOL,1,"thermal compens. %s","",&N.t_comp,0,1,0,0,0,0,preEnterBool, 0, UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
  {SUB_BOOL,1,"absence adc2     %s","",&aiu[0].K2,0,1,0,0,0,0,preEnterBool, postEnter_aui0_ws_flash, UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
  //{SUB_NUM,1,"bufsize2          %02d","",   (int*)&aiu[0].K2,0,16,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"t clamps     %+03d \337","",   &Tclamps,0,16,1,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP,DOWN,LEFT,RIGHT},
  {SUB_HEX,0,"ntc1 buffcode   %04X","",   (int*)&aiu[0].ntc0,1,0xffffff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_HEX,1,"ntc1 offset     %04X","", (int*)&aiu[0].ntcOffset0,0,0xffff,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_HEX,1,"ntc1 gain       %04X","", (int*)&aiu[0].ntcGain0,0,0xffff,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_NUM,0,"t heatsink   %+03d \337","",   &Theatsink,0,16,1,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP,DOWN,LEFT,RIGHT},
  {SUB_HEX,0,"ntc2 buffcode   %04X","",   (int*)&aiu[0].ntc1,1,0xffffff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_HEX,1,"ntc2 offset     %04X","", (int*)&aiu[0].ntcOffset1,0,0xffff,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_HEX,1,"ntc2 gain       %04X","", (int*)&aiu[0].ntcGain1,0,0xffff,0,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP_hex,DOWN_hex,LEFT,RIGHT},
  /*
  {SUB_HEX,1,"GAIN          %06X",   (int*)&aiu[0].gain,0,0xffffff,0,0,0,0,  preEnterNum,postEnter_aui0_ws_flash,UP_hex,DOWN_hex,LEFT,RIGHT},
  //{SUB_NUM,1,"Запуск автокалібрування АЦП  ",   (int*)&aiu[n].sts,0,0xffff,0,0,0,0,(void(*)(char))postEnter_aui0_ws_ram,0,UP,DOWN,LEFT,RIGHT},
  //{SUB_NUM,0,"                              %03d \260",   (int*)&aiu[n].temperature,0,0x07,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  */
};
/*####################################################################################################*/
const MENU_MAN MAINS_ADC[]={
  {SUB_HEX,1,"adc code        %04X","", &RSCR.code,0,0xffffff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_NUM,0,"mains voltage %04dV","", &RSCR.A,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"offset         %+04d","",  &N.ADC_OFF[0],-1000,+1000,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"gain          %04d ","",   &N.ADC_COEF[0],0,9999,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};
/*####################################################################################################*/
const MENU_MAN LOAD_CURR[]={
  {SUB_HEX,1,"adc code        %04X","", &ISCR.code,0,0xffffff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_NUM,0,"load current  %04dA","", &ISCR.A,0,10000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"current offset %+04d","",  &N.ADC_OFF[1],-1000,+1000,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"current gain %05d ","",   &N.ADC_COEF[1],0,99999,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"scr0        %05d %%","",  &TyrReg.SCR0,0,10000,2,0,0,0,preEnterNum, 0, UP, DOWN, LEFT, RIGHT},
};
/*####################################################################################################*/
const MENU_MAN LOAD_VOLTAGE[]={
  {SUB_HEX,1,"adc code        %04X","", &R380.code,0,0xffffff,0,0,0,0,preEnterNum,0,UP_hex,DOWN_hex,LEFT,RIGHT},
  {SUB_NUM,0,"load  voltage %04dV","", &R380.A,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"offset         %+04d","",  &N.ADC_OFF[2],-1000,+1000,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"gain          %04d ","",   &N.ADC_COEF[2],0,9999,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"scr0        %05d %%","",  &TyrReg.SCR0,0,10000,2,0,0,0,preEnterNum, 0, UP, DOWN, LEFT, RIGHT},
};
/*####################################################################################################*/
const MENU_MAN V_I_SUMMARY[]={
  {SUB_NUM,0,"mains voltage %04dV","", &RSCR.A,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"scr0        %05d %%","",  &TyrReg.SCR0,0,10000,2,0,0,0,preEnterNum, 0, UP, DOWN, LEFT, RIGHT},
  {SUB_NUM,0,"load  voltage %04dV","", &R380.A,0,0x7fffffff,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"load current  %04dA","", &ISCR.A,0,0x7fffffff,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"resitence     %04d\364","", &rPwr.R,0,1000,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"Di result    %+05d%%","", &rPwr.lastDi,0,1000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"Du result    %+05d%%","", &rPwr.lastDu,0,1000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"currentAcc  %03d sec\351","",&N.CurrSpd, 1,99,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"current Sp.I  %04dA","", &rPwr.Csp,0,3000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"delayed Sp.I  %04dA","", &rPwr.iSp,0,3000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"min. I lim    %04dA","", &N.minI,0,4000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"max. I lim    %04dA","", &N.maxI,0,4000,2,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"Kc proport.  %05d ","",  &N.Kc,  0,99999,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"count tc quant.  %02d ","",    &N.TqC, 1,10,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_BOOL,1,"Curr.Regulator  %s ","", &N.regCon,0,1,0,0,0,0,preEnterBool, 0, UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
  {SUB_NUM,1,"Kv proport.   %04d ","",      &N.Kv,  0,9999,3,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_BOOL,1,"MainsVoltCorr   %s ","", &N.regVon,0,1,0,0,0,0,preEnterBool, 0, UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
  {SUB_NUM,1,"current gain %05d ","",   &N.ADC_COEF[1],0,99999,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"current offset %+04d","",  &N.ADC_OFF[1],-1000,+1000,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};
/*####################################################################################################*/

/*####################################################################################################*/
const MENU_MAN AIU_SUMM[]={
  {SUB_NUM,0,"11 RTD/TC CHANNELS  "," ",0,0,0,0,(void*)&AIU_CH0,sizeof(AIU_CH0)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
  {SUB_NUM,0,"12 MAINS VOLTAGE    "," ",0,0,0,0,(void*)&MAINS_ADC,sizeof(MAINS_ADC)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
  {SUB_NUM,0,"13 LOAD CURRENT     "," ",0,0,0,0,(void*)&LOAD_CURR,sizeof(LOAD_CURR)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},    
  {SUB_NUM,0,"14 LOAD VOLTAGE     "," ",0,0,0,0,(void*)&LOAD_VOLTAGE,sizeof(LOAD_VOLTAGE)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},    
};
/*####################################################################################################*/



