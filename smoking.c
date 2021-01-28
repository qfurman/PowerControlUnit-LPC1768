#include <myiolpc1768.h>

#include "type.h"
#include "main.h"
#include "timers.h"
#include "temperatures.h"
#include "SCRs.h"

#define PRGwork 1
#define PRGend  2
#define REGsStop 3

/*############################################################################*/
int smTfl; // up count timer enable 

/*############################################################################*/
void stopEverthing (void)
{
  N.PID.ON = 0;  // switch OFF heating regulator
  runPid.out = 0; // PID output reset 
  rPwr.Csp = 0; // setpoint of current regelator reset
  N.regRHon = 0; // switch OFF regulator of relative humidity
  TyrReg.circ = 0; //switch OFF circulation FAN
  TyrReg.exhaust = 0; //switch OFF exhaust FAN  
}
/*############################################################################*/
void smokingPRC ()
{
  if (RunPrg == 0){
    RSP.phase = 0; // IF SMOKING PROCESS STOPED RESET THE PHASE
    if (Sts & (1<<PRGwork)) stopEverthing (); // regulators stop
    Sts &=~(0xE); // reset a flags for begin, end work and REGsStop on a smoking program
  } else {
    if ( Sts & (1<<PRGend) ){
      if (!(Sts & (1<<REGsStop))) {
        stopEverthing (); // regulators stop
        Sts |= 1 << REGsStop; // set a flag for work on a smoking program 
      }
    }
  }
  
  switch (RSP.phase){
  case 0:
    if (RunPrg) { // if commnd for start smoking process is set
      RSP.PHtime = 0;
      RSP.phase = 1;
      N.regRHon = 1; // switch ON regulator of relative humidity
      N.PID.ON = 1;  // switch ON heating regulator
      Sts |= 1 << PRGwork; // set a flag for work on a smoking program 
    }
    break;
  case 1:
    if (smTfl){ // if one second flag setted
      smTfl = 0;
      N.PID.spT = RSP.Rsp[0].tC; // temperature setpoint
      rhSp = RSP.Rsp[0].rH;// runtime setpoint for humidity regulator
      if (RSP.PHtime < RSP.Rsp[0].time) RSP.PHtime++; // increment time of current phase
      if (RSP.PHtime >= RSP.Rsp[0].time){ // if time is up
        if (RSP.Rsp[1].tC != 0 && RSP.Rsp[1].rH !=0){ // if next phase is present
          RSP.PHtime = 0; // reset time
          RSP.phase = 2; // next phase  
          Sts &=~(1 << PRGend); // reset a flag for end work on a smoking program 
        }else Sts |= 1 << PRGend; // set a flag for end work on a smoking program 
      }
    }
    break;
  case 2:
    if (smTfl){ // if one second flag setted
      smTfl = 0;
      N.PID.spT = RSP.Rsp[1].tC; // temperature setpoint
      rhSp = RSP.Rsp[1].rH;// runtime setpoint for humidity regulator
      if (RSP.PHtime < RSP.Rsp[1].time) RSP.PHtime++; // increment time of current phase
      if (RSP.PHtime >= RSP.Rsp[1].time){ // if time is up
        if (RSP.Rsp[2].tC != 0 && RSP.Rsp[2].rH !=0){ // if next phase is present
          RSP.PHtime = 0; // reset time
          RSP.phase = 3; // next phase  
          Sts &=~(1 << PRGend); // reset a flag for end work on a smoking program 
        }else Sts |= 1 << PRGend; // set a flag for end work on a smoking program 
      }
    }    
    break;
  case 3:
    if (smTfl){ // if one second flag setted
      smTfl = 0;
      N.PID.spT = RSP.Rsp[2].tC; // temperature setpoint
      rhSp = RSP.Rsp[2].rH;// runtime setpoint for humidity regulator
      if (RSP.PHtime < RSP.Rsp[2].time) RSP.PHtime++; // increment time of current phase
      if (RSP.PHtime >= RSP.Rsp[2].time){ // if time is up
        if (RSP.Rsp[3].tC != 0 && RSP.Rsp[3].rH !=0){ // if next phase is present
          RSP.PHtime = 0; // reset time
          RSP.phase = 4; // next phase  
          Sts &=~(1 << PRGend); // reset a flag for end work on a smoking program 
        }else Sts |= 1 << PRGend; // set a flag for end work on a smoking program 
      }
    }    
    break;
  case 4:
    if (smTfl){ // if one second flag setted
      smTfl = 0;
      N.PID.spT = RSP.Rsp[3].tC; // temperature setpoint
      rhSp = RSP.Rsp[3].rH;// runtime setpoint for humidity regulator
      if (RSP.PHtime < RSP.Rsp[3].time) RSP.PHtime++; // increment time of current phase
      if (RSP.PHtime >= RSP.Rsp[3].time){ // if time is up
        if (RSP.Rsp[4].tC != 0 && RSP.Rsp[4].rH !=0){ // if next phase is present
          RSP.PHtime = 0; // reset time
          RSP.phase = 5; // next phase  
          Sts &=~(1 << PRGend); // reset a flag for end work on a smoking program 
        }else Sts |= 1 << PRGend; // set a flag for end work on a smoking program 
      }
    }    
    break;
  case 5:
    if (smTfl){ // if one second flag setted
      smTfl = 0;
      N.PID.spT = RSP.Rsp[4].tC; // temperature setpoint
      rhSp = RSP.Rsp[4].rH;// runtime setpoint for humidity regulator
      if (RSP.PHtime < RSP.Rsp[4].time) RSP.PHtime++; // increment time of current phase
      if (RSP.PHtime >= RSP.Rsp[4].time){ // if time is up
        Sts |= 1 << PRGend; // set a flag for end work on a smoking program 
      }
    }    
    break;
  }
}
/*############################################################################*/
/*############################################################################*/
void init_recipe (void)
{
  RSP.phase = 0;  // stadium of smoking
  RSP.PHtime = 0; // timer of current phase
  RSP.Rsp[0].tC   = N.dsp[0].tC;  
  RSP.Rsp[0].rH   = N.dsp[0].rH;  
  RSP.Rsp[0].time = N.dsp[0].time;
  RSP.Rsp[1].tC   = N.dsp[1].tC;  
  RSP.Rsp[1].rH   = N.dsp[1].rH;  
  RSP.Rsp[1].time = N.dsp[1].time;
  RSP.Rsp[2].tC   = N.dsp[2].tC;  
  RSP.Rsp[2].rH   = N.dsp[2].rH;  
  RSP.Rsp[2].time = N.dsp[2].time;
  RSP.Rsp[3].tC   = N.dsp[3].tC;  
  RSP.Rsp[3].rH   = N.dsp[3].rH;  
  RSP.Rsp[3].time = N.dsp[3].time;
  RSP.Rsp[4].tC   = N.dsp[4].tC;  
  RSP.Rsp[4].rH   = N.dsp[4].rH;  
  RSP.Rsp[4].time = N.dsp[4].time;
}
/*############################################################################*/
/*############################################################################*/
