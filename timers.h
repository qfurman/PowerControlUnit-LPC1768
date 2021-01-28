

#ifndef _IS_INCLUDED_TIMERS_H
#define _IS_INCLUDED_TIMERS_H


void start_tx1();
void start_tx0();


void timer0_isr(void);
void timer1_isr(void);

void hvpot_funct(void);

extern unsigned int time_10mS;
extern unsigned int keys_time;//����������� ������

extern int PID_flag;
extern int RH_flag;

extern int powerONtime;//for power on delay
extern int time_lcd;//��� ��� ���������� ��������� LCD
extern char flag_LCD;//������ ��� ���������� ����������� � ��������� LCD

extern int buzz_flag; //��� ��� �����
extern int scr1_flag; //circulation Fan flag
extern int scr2_flag; //exhaust Fan flag
extern int iSp_flag;  //current setpoint flag
#endif

