

#ifndef _IS_INCLUDED_TIMERS_H
#define _IS_INCLUDED_TIMERS_H


void start_tx1();
void start_tx0();


void timer0_isr(void);
void timer1_isr(void);

void hvpot_funct(void);

extern unsigned int time_10mS;
extern unsigned int keys_time;//клавіатурний таймер

extern int PID_flag;
extern int RH_flag;

extern int powerONtime;//for power on delay
extern int time_lcd;//час для орнанізації оновлення LCD
extern char flag_LCD;//однака для організації відображення і реанімації LCD

extern int buzz_flag; //час для звуку
extern int scr1_flag; //circulation Fan flag
extern int scr2_flag; //exhaust Fan flag
extern int iSp_flag;  //current setpoint flag
#endif

