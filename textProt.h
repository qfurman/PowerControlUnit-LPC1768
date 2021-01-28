

/*####################################################################################################*/
const MENU_MAN THYRISTORP[]={
  {SUB_NUM,1,"t prohibit    %03d C","", &N.tyrOFF, 300,900,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"t resume work %03d C","", &N.tyrON,  0,500,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"t heatsink   %+03d \337","",   &Theatsink,0,16,1,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP,DOWN,LEFT,RIGHT},
};
/*####################################################################################################*/
const MENU_MAN COOLERFANP[]={
  {SUB_NUM,1,"t switch ON   %03d C","", &N.cTon, 100,600,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"t switch OFF  %03d C","", &N.cToff,  0,500,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,0,"t heatsink   %+03d \337","",   &Theatsink,0,16,1,0,0,0,preEnterNum,postEnter_aui0_ws_flash,UP,DOWN,LEFT,RIGHT},
};
/*####################################################################################################*/

/*####################################################################################################*/
const MENU_MAN PROTECT_MENU[]={
    {SUB_NUM,0,"21 COOLER FAN       "," ",0,0,0,0,(void*)&COOLERFANP,sizeof(COOLERFANP)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
    {SUB_NUM,0,"22 THYRISTORS PROT. "," ",0,0,0,0,(void*)&THYRISTORP,sizeof(THYRISTORP)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
};
/*####################################################################################################*/


