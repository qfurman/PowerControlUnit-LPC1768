
/*####################################################################################################*/
const MENU_MAN ACT_RECIPE[]={
  {SUB_BOOL,1,"run programm    %s ","", &RunPrg,0,1,0,0,0,0,preEnterBool, 0, UP_BOOL, UP_BOOL, UP_BOOL, UP_BOOL},
  {SUB_NUM,1,"phase            %02d ","", &RSP.phase, 0,5,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"curr.time  %05d sec","", &RSP.PHtime, 0,65535,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"01 t\337C        %03d \337","", &RSP.Rsp[0].tC, 0,1200,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"01 RH           %02d %%","", &RSP.Rsp[0].rH, 0,100,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"01 time    %05d sec","", &RSP.Rsp[0].time, 1,65535,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"02 t\337C        %03d \337","", &RSP.Rsp[1].tC, 0,1200,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"02 RH           %02d %%","", &RSP.Rsp[1].rH, 0,100,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"02 time    %05d sec","", &RSP.Rsp[1].time, 0,65535,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"03 t\337C        %03d \337","", &RSP.Rsp[2].tC, 0,1200,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"03 RH           %02d %%","", &RSP.Rsp[2].rH, 0,100,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"03 time    %05d sec","", &RSP.Rsp[2].time, 0,65535,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"04 t\337C        %03d \337","", &RSP.Rsp[3].tC, 0,1200,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"04 RH           %02d %%","", &RSP.Rsp[3].rH, 0,100,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"04 time    %05d sec","", &RSP.Rsp[3].time, 0,65535,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"05 t\337C        %03d \337","", &RSP.Rsp[4].tC, 0,1200,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"05 RH           %02d %%","", &RSP.Rsp[4].rH, 0,100,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"05 time    %05d sec","", &RSP.Rsp[4].time, 0,65535,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};
/*####################################################################################################*/

const MENU_MAN DEF_RECIPE[]={
  {SUB_NUM,1,"d1 t\337C        %03d \337","", &N.dsp[0].tC, 0,1200,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"d1 RH           %02d %%","", &N.dsp[0].rH, 0,100,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"d1 time    %05d sec","", &N.dsp[0].time, 1,65535,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"d2 t\337C        %03d \337","", &N.dsp[1].tC, 0,1200,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"d2 RH           %02d %%","", &N.dsp[1].rH, 0,100,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"d2 time    %05d sec","", &N.dsp[1].time, 0,65535,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"d3 t\337C        %03d \337","", &N.dsp[2].tC, 0,1200,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"d3 RH           %02d %%","", &N.dsp[2].rH, 0,100,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"d3 time    %05d sec","", &N.dsp[2].time, 0,65535,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"d4 t\337C        %03d \337","", &N.dsp[3].tC, 0,1200,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"d4 RH           %02d %%","", &N.dsp[3].rH, 0,100,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"d4 time    %05d sec","", &N.dsp[3].time, 0,65535,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"d5 t\337C        %03d \337","", &N.dsp[4].tC, 0,1200,1,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"d5 RH           %02d %%","", &N.dsp[4].rH, 0,100,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
  {SUB_NUM,1,"d5 time    %05d sec","", &N.dsp[4].time, 0,65535,0,0,0,0,preEnterNum,0,UP,DOWN,LEFT,RIGHT},
};
/*####################################################################################################*/

/*####################################################################################################*/
const MENU_MAN RECIPE[]={
    {SUB_NUM,0,"A1 ACTUAL RECIPE    "," ",0,0,0,0,(void*)&ACT_RECIPE,sizeof(ACT_RECIPE)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
    {SUB_NUM,0,"A2 DEFAULT RECIPE   "," ",0,0,0,0,(void*)&DEF_RECIPE,sizeof(DEF_RECIPE)/sizeof(MENU_MAN)-1,0,0,0,0,0,0,0},
};
/*####################################################################################################*/
