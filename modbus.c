#include <myiolpc1768.h>
#include "type.h"
#include "macros.h"
#include "uart_0.h"
#include "uart_1.h"
#include "uart_2.h"
#include "uart_3.h"
#include "menu.h"
#include "main.h"
#include "timers.h"
#include "main.h"
#include "adc.h"
#include "temperatures.h"
#include "SCRs.h"


#define modbufsize 256

//декларація атрибутів модбасу

#pragma pack(1)
union{
    char modbuf[modbufsize];

    struct{
        char address;
        char function;
        char exeption;
    }exception;//повідомлення про виключну ситуацію

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
    }read_req;//читання Запит

    struct{
        char address;
        char function;
        char byte_count;
        unsigned short reg[125];
    }read_resp;//читання відповідь

    struct{
        char address;
        char function;
        unsigned short register_address;
        unsigned short register_value;
    }write_req;//запис одного регістра Запит

    struct{
        char address;
        char function;
        unsigned short register_address;
        unsigned short register_value;
    }write_resp;//запис одного регістра відповідь

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
        unsigned char byte_count;
        unsigned short reg[123];
    }write_mult_req;//запис регістрів Запит

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
    }write_mult_resp;//запис регістрів відповідь

}buf[4];

//регістри модбаса

/******************************************************************************/
void error();
void start_transmit();
void transmit_prepare(char count);

#define litle_endian(n) ((char)((n)>>8)|(char)(n)<<8)
#define big_endian(n) ((char)((n)>>8)|(char)(n)<<8)

/******************************************************************************/
int reserv0; //void parameter
/*----------------------------------------------------------------------------*/
//адреси параметрів для обміну з компютером
const MMA mma[]={
  0,0,      &Sts,               // current status of system
  0,0,      &rtdT0,             // dry thermometer
  0,0,      &rtdT1,             // wet thermometer
  0,0,      &Ht,                // relative humidity
  0,0,      &RSCR.A,            // mains voltage     000.0            
  0,0,      &ISCR.A,            // heating load current      00.00                
  0,0,      &R380.A,            // heating load  voltage     000.0            
  0, 4000,  &rPwr.Csp,          // current setpoint for regulating load current obtains from runPid.out
  0,10000,  &TyrReg.SCR0,       // heater relative output within limitation between min and max
  0,10000,  &TyrReg.SCR1,       // circulation relative output within limitation between min and max 
  0,10000,  &TyrReg.SCR2,       // exhaust relative output within limitation between min and max
  0,100,    &rhSp,              // runtime setpoint for humidity regulator 
  0,65535,  &reserv0,           // 
  0,65535,  &reserv0,           // 
  0,65535,  &reserv0,           // 
  0,2,      &resAlrm,           // reset alarm COMMAND 1-reset 2-soundOFF 
  0,1,      &RunPrg,            // run smoking programm COMMAND ----------------
  0,10000,  &runPid.out,        // percent of heating output 100.00 without limitation between min and max
  0,10000,  &TyrReg.circ,       // percent of circulation out 100.0 without limitation between min and max
  0,10000,  &TyrReg.exhaust,     // percent of exhaust out    100.0 without limitation between min and max
  0,1200,   &runPid.rSp,        // runtime setpoint of PID regulator delayed for shockless change
  1,5,      &RSP.phase,         // stadium of smoking---------------------------
  1,7200,   &RSP.PHtime,        // time of current phase
  0,1200,   &RSP.Rsp[0].tC,     // temperature of #0 smoking stage
  0,100,    &RSP.Rsp[0].rH,     // relative humidity of #0 smoking stage
  1,65535,  &RSP.Rsp[0].time,   // time in seconds of current smoking stag
  0,1200,   &RSP.Rsp[1].tC,
  0,100,    &RSP.Rsp[1].rH,
  0,65535,  &RSP.Rsp[1].time,
  0,1200,   &RSP.Rsp[2].tC,
  0,100,    &RSP.Rsp[2].rH,
  0,65535,  &RSP.Rsp[2].time,
  0,1200,   &RSP.Rsp[3].tC,
  0,100,    &RSP.Rsp[3].rH,
  0,65535,  &RSP.Rsp[3].time,
  0,1200,   &RSP.Rsp[4].tC,
  0,100,    &RSP.Rsp[4].rH,
  0,65535,  &RSP.Rsp[4].time,
  0,65535,  &reserv0,           // 
  0,65535,  &reserv0,           // 
  0, 4000,  &rPwr.Csp,          // current setpoint for regulating load current obtains from runPid.out
  0,10000,  &TyrReg.SCR0,       // heater relative output within limitation between min and max
  0,10000,  &TyrReg.SCR1,       // circulation relative output within limitation between min and max 
  0,10000,  &TyrReg.SCR2,       // exhaust relative output within limitation between min and max
  0,100,    &rhSp,              // runtime setpoint for humidity regulator 
  1,999,    &N.circSpd,         // circulation acceleration (speed of changing speed of circ)
  1,999,    &N.exhSpd,          // inlet acceleration (speed of changing speed of inlet)
  0,9999,   &N.Krh,             // proportional coeff for regulating of relative humidity
  1,300,    &N.TqRH,            // quantization time for works regulator of relative humidity  
  0,1,      &N.regRHon,         // regulator of relative humidity ON if 1
  0,1,      &N.PID.ON,          // on/off PID heating regulator-----------------
  0,1200,   &N.PID.spT,         // setpoint
  1,99,     &N.PID.speed,       // dT/sec.
  1,10000,  &N.PID.max,         // upper lim
  0,10000,  &N.PID.min,         // lower lim 
  0,9999,   &N.PID.K,           // коеф пропорційності
  1,999,    &N.PID.Tq,          // період квантування
  0,9999,   &N.PID.Ki,          // коеф інтегрування
  1,999,    &N.PID.Ti,          // час інтегрування
  0,65535,  &N.PID.Kd,          // коеф диференціювання
  1,999,    &N.PID.Td,          // час дифиренціювання    
  1,10000,  &N.maxSCR0,         // upper lim heating
  0,10000,  &N.minSCR0,         // lower lim heating
  1,10000,  &N.maxSCR1,         // upper lim circ
  0,10000,  &N.minSCR1,         // lower lim circ
  1,10000,  &N.maxSCR2,         // upper lim exhaust
  0,10000,  &N.minSCR2,         // lower lim exhaust
  0,4000,   &N.minI,            // lower lim current
  0,4000,   &N.maxI             // upper lim current
};  
//для перепису в реальні параметри
/*
int* const madrr[]={
  &Sts,            
  &rtdT0,          
  &rtdT1,          
  &Ht,             
  &RSCR.A,         
  &ISCR.A,         
  &R380.A,         
  &rPwr.Csp,        
  &TyrReg.SCR0,    
  &TyrReg.SCR1,    
  &TyrReg.SCR2,    
  &rhSp,           
  &reserv0,        
  &reserv0,        
  &reserv0,        
  &resAlrm,        
  &RunPrg,         
  &runPid.out,    
  &TyrReg.circ, 
  &TyrReg.exhaust,   
  &runPid.rSp,     
  &RSP.phase,      
  &RSP.PHtime,     
  &RSP.Rsp[0].tC,  
  &RSP.Rsp[0].rH,  
  &RSP.Rsp[0].time,
  &RSP.Rsp[1].tC,  
  &RSP.Rsp[1].rH,  
  &RSP.Rsp[1].time,
  &RSP.Rsp[2].tC,  
  &RSP.Rsp[2].rH,  
  &RSP.Rsp[2].time,
  &RSP.Rsp[3].tC,  
  &RSP.Rsp[3].rH,  
  &RSP.Rsp[3].time,
  &RSP.Rsp[4].tC,  
  &RSP.Rsp[4].rH,  
  &RSP.Rsp[4].time,
  &reserv0,        
  &reserv0,        
  &rPwr.Csp,        
  &TyrReg.SCR0,    
  &TyrReg.SCR1,    
  &TyrReg.SCR2,    
  &rhSp,           
  &N.circSpd,       
  &N.exhSpd,      
  &N.Krh,          
  &N.TqRH,         
  &N.regRHon,      
  &N.PID.ON,       
  &N.PID.spT,      
  &N.PID.speed,    
  &N.PID.max,      
  &N.PID.min,      
  &N.PID.K,        
  &N.PID.Tq,       
  &N.PID.Ki,       
  &N.PID.Ti,       
  &N.PID.Kd,       
  &N.PID.Td,       
  &N.maxSCR0,      
  &N.minSCR0,
  &N.maxSCR1,      
  &N.minSCR1,      
  &N.maxSCR2,      
  &N.minSCR2,
  &N.minI,           
  &N.maxI,  
};  
*/
MS ms;//для збереження у фоновій програмі даних отриманих по інтерфейсі
/*----------------------------------------------------------------------------*/

#include "modbus3.h"
//#include "modbus2.h"
//#include "modbus1.h"
#include "modbus0.h"

/******************************************************************************/



