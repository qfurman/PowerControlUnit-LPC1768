
#include <myiolpc1768.h>

#include "type.h"
#include "timers.h"

/******************************************************************************/
/******************************************************************************/
//���������� �������
/******************************************************************************/
KAY_STACK kays; //����������� ������������ �����

KAY_REPEAT kay_temp={0,0,0};
const KAY_REPEAT kay_set={10,5,1};//������� ��������� ����� *20�� �� �������� *20��
int key;
/******************************************************************************/
/******************************************************************************/
/******************************************************************************\
*   ����� ������ ������ � �����
*   ���� ����� ������������ �� �����
\******************************************************************************/
void kays_push(unsigned char kay_)
{
    if(kays.index >= kays_buf_size)return;
    kays.buf[kays.index++] = kay_;
}

/******************************************************************************\
*   �������� ������ ������ � ������
*   ���� ����� �������� ��������� 0
\******************************************************************************/
char kays_pop(void)
{
    unsigned char kay_;

    if(kays.index == 0)return 0;
    kay_ = kays.buf[--kays.index];
    kays.buf[kays.index] = 0;
    return kay_;
}
/******************************************************************************\
*   ������������ ���������
*   ��������� ���� ������������ �������
\******************************************************************************/
/******************************************************************************/
//P2.7  MENU
//P2.9  ENTER
//P0.19 L0
//P0.20 L1
//P0.21 L2
//P0.22 L3

void kays_init(void)
{
    kays.index = 0;         //init kays stack
}

/******************************************************************************/
/******************************************************************************/
//UP    DOWN   LEFT  RIGHT   MENU  ENTER
const int keys_ref[]={
0x0001,0x0002,0x0004,0x0008,0x0010,0x0020
};
/******************************************************************************/
char last_key;
char key_count,key_count_2;
char new_key;
unsigned int new_keys;

/******************************************************************************/
void plase_kay()
{
    char i;

    for(i=0;i<6;i++){
        if(new_keys == keys_ref[i]){
            if(last_key == i+1){
                if(key_count >= 1){
                    if(new_key != i+1){
                        kays_push(i+1); buzz_flag = 10; FIO3CLR_bit.BUZ = 1;//�������� ����
                    }else{
                            key_count_2++;
                            if(key_count >= 10){
                              key_count = 5;
                              kays_push(i+1); buzz_flag = 10; FIO3CLR_bit.BUZ = 1;//�������� ����
                            }else key_count++;
                    }
                    key = i+1;
                    new_key = key;
                }
                else key_count++;
            }else{
                last_key = i + 1;
                key_count = 0;
                key_count_2 = 0;
            }
            break;
        }
    }

    if(i==6){
        key = 0;
        key_count = 0;
        key_count_2 = 0;
        last_key = key;
        new_key = 0;
    }

}
/******************************************************************************/
/******************************************************************************/
int last_enc;//���������� �������
void RdKeyboard (void)
{
  new_keys = 0;
  
  new_keys = ~FIO0PIN_bit.L & 0xf;
  new_keys |= (~FIO2PIN>>8 & 3)<<4;//Enter Menu
  
  plase_kay();//��������� �� ��������� ������ � ������
  
}
/******************************************************************************/
/******************************************************************************/
void encoder_init( void )
{
  PCONP_bit.PCQEI = 1;//Quadrature Encoder Interface power/clock enable
  PINMODE3_bit.P1_20 = 2;// Pin has neither pull-up nor pull-down resistor enabled
  PINMODE3_bit.P1_23 = 2;// Pin has neither pull-up nor pull-down resistor enabled
  PINSEL3 |= 0x00004100;
  QEIMAXPSOS = 0xffffffff;//QEI Maximum Position
  QEISET_bit.ENCLK_INT = 1;//encoder clock pulse was detected interrupt
  
}
/******************************************************************************/
/******************************************************************************/
void QE_IRQHandler (void)
{
  if(QEIINTSTAT_bit.ENCLK_INT) QEICLR_bit.ENCLK_INT = 0;
  
}
/******************************************************************************/
/******************************************************************************/
