
#include "type.h"
//#include "timers.h"
#include "intrinsics.h"
#include <myiolpc1768.h>

#include "macros.h"
#include "main.h"
#include "modbus.h"


/*
**===========================================================================
**  3.      DECLARATIONS
**  3.1     Internal constants
**===========================================================================
*/

unsigned short const CRC_table[] = {
0x0000, 0xC1C0, 0x81C1, 0x4001, 0x01C3, 0xC003, 0x8002, 0x41C2, 0x01C6, 0xC006,
0x8007, 0x41C7, 0x0005, 0xC1C5, 0x81C4, 0x4004, 0x01CC, 0xC00C, 0x800D, 0x41CD,
0x000F, 0xC1CF, 0x81CE, 0x400E, 0x000A, 0xC1CA, 0x81CB, 0x400B, 0x01C9, 0xC009,
0x8008, 0x41C8, 0x01D8, 0xC018, 0x8019, 0x41D9, 0x001B, 0xC1DB, 0x81DA, 0x401A,
0x001E, 0xC1DE, 0x81DF, 0x401F, 0x01DD, 0xC01D, 0x801C, 0x41DC, 0x0014, 0xC1D4,
0x81D5, 0x4015, 0x01D7, 0xC017, 0x8016, 0x41D6, 0x01D2, 0xC012, 0x8013, 0x41D3,
0x0011, 0xC1D1, 0x81D0, 0x4010, 0x01F0, 0xC030, 0x8031, 0x41F1, 0x0033, 0xC1F3,
0x81F2, 0x4032, 0x0036, 0xC1F6, 0x81F7, 0x4037, 0x01F5, 0xC035, 0x8034, 0x41F4,
0x003C, 0xC1FC, 0x81FD, 0x403D, 0x01FF, 0xC03F, 0x803E, 0x41FE, 0x01FA, 0xC03A,
0x803B, 0x41FB, 0x0039, 0xC1F9, 0x81F8, 0x4038, 0x0028, 0xC1E8, 0x81E9, 0x4029,
0x01EB, 0xC02B, 0x802A, 0x41EA, 0x01EE, 0xC02E, 0x802F, 0x41EF, 0x002D, 0xC1ED,
0x81EC, 0x402C, 0x01E4, 0xC024, 0x8025, 0x41E5, 0x0027, 0xC1E7, 0x81E6, 0x4026,
0x0022, 0xC1E2, 0x81E3, 0x4023, 0x01E1, 0xC021, 0x8020, 0x41E0, 0x01A0, 0xC060,
0x8061, 0x41A1, 0x0063, 0xC1A3, 0x81A2, 0x4062, 0x0066, 0xC1A6, 0x81A7, 0x4067,
0x01A5, 0xC065, 0x8064, 0x41A4, 0x006C, 0xC1AC, 0x81AD, 0x406D, 0x01AF, 0xC06F,
0x806E, 0x41AE, 0x01AA, 0xC06A, 0x806B, 0x41AB, 0x0069, 0xC1A9, 0x81A8, 0x4068,
0x0078, 0xC1B8, 0x81B9, 0x4079, 0x01BB, 0xC07B, 0x807A, 0x41BA, 0x01BE, 0xC07E,
0x807F, 0x41BF, 0x007D, 0xC1BD, 0x81BC, 0x407C, 0x01B4, 0xC074, 0x8075, 0x41B5,
0x0077, 0xC1B7, 0x81B6, 0x4076, 0x0072, 0xC1B2, 0x81B3, 0x4073, 0x01B1, 0xC071,
0x8070, 0x41B0, 0x0050, 0xC190, 0x8191, 0x4051, 0x0193, 0xC053, 0x8052, 0x4192,
0x0196, 0xC056, 0x8057, 0x4197, 0x0055, 0xC195, 0x8194, 0x4054, 0x019C, 0xC05C,
0x805D, 0x419D, 0x005F, 0xC19F, 0x819E, 0x405E, 0x005A, 0xC19A, 0x819B, 0x405B,
0x0199, 0xC059, 0x8058, 0x4198, 0x0188, 0xC048, 0x8049, 0x4189, 0x004B, 0xC18B,
0x818A, 0x404A, 0x004E, 0xC18E, 0x818F, 0x404F, 0x018D, 0xC04D, 0x804C, 0x418C,
0x0044, 0xC184, 0x8185, 0x4045, 0x0187, 0xC047, 0x8046, 0x4186, 0x0182, 0xC042,
0x8043, 0x4183, 0x0041, 0xC181, 0x8180, 0x4040
};

const int BAUDS[7]={  2400,  4800,  9600, 19200, 38400, 57600,115200};
const char T_switchToRX []={15 + 2, 8 + 2, 4 + 2, 2 + 2, 1 + 2, 1 + 2, 1 + 2};//{17+1, 9+1, 5+1, 3 +1, 2+1, 1+2, 1+2};
const int B_DLL[]={434, 217, 108, 54, 27, 18, 9};//подільник ціла частина, дробова для всіх швидкостей однакова DivAddVal=1 і MulVal=2
const int Devices1[16]={25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40};//адреси приладів на UART1
const int Devices2[14]={13,14,15,16,17,18,19,20,21,22,23,24,42,41};//адреси приладів на UART2
const int Devices3[1]={1};//адреси приладів на UART3


/*
**===========================================================================
**  3.2     Internal macros
**===========================================================================
*/

#define RLS_ 3
#define RDA_ 2
#define CTI_ 6
#define THRE_ 1

/*
**===========================================================================
**  3.3     Internal type definitions
**===========================================================================
*/

/*
**===========================================================================
**  3.4     Global variables (declared as 'extern' in some header file)
**===========================================================================
*/

/*
**===========================================================================
**  3.5     Internal function prototypes (defined in Section 5)
**===========================================================================
*/

/*
**===========================================================================
**  3.6     Internal variables
**===========================================================================
*/
unsigned short CRCtx0,CRCrx0;

LIST *uart0_wr_ptr; //global pointer for outgoing thread
LIST *uart0_rd_ptr;  //global pointer for ingoing thread
/*
#define __IO_REG32_BIT(NAME, ADDRESS, ATTRIBUTE, BIT_STRUCT)\
                        volatile __no_init ATTRIBUTE union \
                         {                                 \
                           unsigned long NAME;             \
                           BIT_STRUCT NAME ## _bit;      \
                         } @ ADDRESS
*/
/*volatile __no_init */union{
    char uart0_sts;
    RXTX uart0_sts_bit;
};// @ 0x10003420;


extern PARAMETRS N;
/*
**===========================================================================
**  4.      GLOBAL FUNCTIONS (declared as 'extern' in some header file)
**===========================================================================
*/

/******************************************************************************/
void start_tx0()
{
    char byte;

    switch(uart0_sts & 0x1f){
      case 0x01:
        uart0_sts_bit.start_tx = 1; //ознака початку передачі
        FIO2CLR_bit.RTS1 = 1;       //стати в передачу
        timer_uart[0] = 1;//0.25mS
        break;
      case 0x03://інтервал постановки в передачу закінчився - почати передачу
        //Передача
        U0FCR_bit.TFR = 1;//TX FIFO RESET
        U0FCR_bit.RFR = 1;//RX FIFO RESET
        U0IER_bit.THREIE = 1;//transmit enable

        if(uart0_wr_ptr->count){                       //закинути перший байт на передачу
            uart0_wr_ptr->count--;
            byte = *uart0_wr_ptr->ptr;
            CRC_tx0(byte);                                //обчислення CRC
            U0THR = byte;
            if(uart0_wr_ptr->ptr)*uart0_wr_ptr->ptr++;   //і якщо є дані на передачу то інкремент поінтера
        }
        break;

      case 0x07://переключитись в прийом
        FIO2SET_bit.RTS1 = 1;                   //стати в прийом
        uart0_sts_bit.start_rx = 1;             //ознака початку прийому
        modbus_receive_init();
        break;

      case 0x0F:
        if(U0LSR_bit.DR){    //якщо у буфері прийому є дані то виймати їх
            break;
        }
        //тут вписати код обробки аваріїї відсутності відповіді
        uart0_sts_bit.not_respond = 1;
        break;
    }
}//end start_tx1
/******************************************************************************/

/*
struct list{
    char *ptr;              //Operands pointer
    unsigned int count;     //count of bytes
    struct list *next;      //pointer for next element of list
};
*/


//RESPONSE_HEAD rx_temp_head;

LIST uart0_wr_manag[3];
LIST uart0_rd_manag[2];

char crc_buf0[2];
LIST uart0_wr_crc;  //для відправки контрольної суми

void wr_uart0(char *src, unsigned int src_count, char *dest, unsigned int dest_count)
{

    //T0MR3 = T0TC + 2500000;//розмір паузи

    uart0_wr_crc.ptr = &crc_buf0[0];             //ця частина для відправлення контрольної суми
    uart0_wr_crc.count = 2;                     //підставляється  в перериванні після досягнення ДНА
    uart0_wr_crc.next = 0;

    uart0_wr_manag[0].ptr = src;                //джерело
    uart0_wr_manag[0].count = src_count;        //кількість байтів
    uart0_wr_manag[0].next = 0;                 //більше нема

    uart0_rd_manag[0].ptr = dest;               //не зберігати байти
    uart0_rd_manag[0].count = dest_count;       //1байт-команда, 2байти-адреса
    uart0_rd_manag[0].next = 0;                 //продовження

    uart0_wr_ptr = &uart0_wr_manag[0];          //ініціалізація поінтера запису
    uart0_rd_ptr = &uart0_rd_manag[0];          //ініціалізація поінтера читання

    CRCtx0_init();
    CRCrx0_init();
    //N.UART1.simplex
    if(uart0_sts_bit.not_respond){  //якщо відповіді небуло то немає сенсу в додатковому таймауті
        uart0_sts = 0x00;        //стан початок прийому і передачі start_tx = 1 start_rx = 1;
        uart0_sts_bit.pre_time = 1; //ознака початку паузи перерд передачою
        uart0_sts_bit.start_tx = 1; //ознака початку передачі
        FIO2CLR_bit.RTS1 = 1;       //стати в передачу
        timer_uart[0] = 2;// 2/4mS
    }
    else{
        uart0_sts = 0x00;        //стан початок прийому і передачі start_tx = 1 start_rx = 1;
        uart0_sts_bit.pre_time = 1; //ознака початку паузи перерд передачою

        timer_uart[0] = N.UART0.timeout*4;//розмір паузи
    }

}
/******************************************************************************/
//void MA_IntHandler_UART0( void )
void UART0_IRQHandler (void)
{
    volatile char dummy;
    char un;

    //U0FCR_bit.DMA = 0;//DMA disable
    
    //if(U0IIR_bit.IP)return;//if no pending interrupts
    
    switch(U0IIR_bit.IID){//interrupt identification
      case RLS_: //recive line status
            dummy = U0LSR;
        break;

      case CTI_: //caracter timeout indicator
            uart0_sts_bit.timeout = 1;  //ознака детекту таймауту прийому
      case RDA_: //recive data available
        if(uart0_sts_bit.timeout)un = 16;//виймати все
        else un = 13;//запобігти спустошенню фіфо для генераціі таймауту передачі
        do{
            if(U0LSR_bit.DR){                      //якщо у буфері прийому є дані то виймати їх
                if(uart0_rd_ptr->count){                 //якщо є куди виймати
                    if(uart0_rd_ptr->ptr){               //якщо треба зберегти а не просто спорожнити
                        char byte = U0RBR;
                        *uart0_rd_ptr->ptr++ = byte;    //якщо поінтер не нульовий то розміститиза ним отриманий байт
                        CRC_rx0(byte);
                    }
                    else dummy = U0RBR;                 //виймати непотрібний байт

                    if(--uart0_rd_ptr->count == 0){      //декремент кількості байт прийнятих за цим поінтером якщо дорівнює 0 то спробувати перейти далі за списком
                        if(uart0_rd_ptr->next) uart0_rd_ptr = uart0_rd_ptr->next;//перевірка чи є наступне місце для прийому і запис наступного елементу списку в активну позицію передачі
                        else uart0_sts_bit.end_rx = 1;    //ознака кінця прийому
                    }
                }
                else dummy = U0RBR;//U1FCR_bit.RFR = 1;//RECIEV FIFO RESET//dummy = U1RBR; //виймати непотрібний байт для спустошення фіфо
            }
            else{
                dummy = U0RBR;//U1FCR_bit.RFR = 1;//RECIEV FIFO RESET//dummy = U1RBR; //виймати непотрібний байт для спустошення фіфо
                break;
            }
        }while(--un);
        if(uart0_sts_bit.timeout == 1) modbus_0();
        break;

      case THRE_:
        do{
            //if(uart1_sts_bit.end_tx)break;
            //Передача
            if(uart0_wr_ptr->count){                             //якщо передано ще не всі дані за цим поінтрером
                if(U0LSR_bit.THRE){                              //якщо FIFO не переповнене
                    uart0_wr_ptr->count--;                       //декремент кількості байт на передачу
                    char byte = *uart0_wr_ptr->ptr;
                        CRC_tx0(byte);
                        U0THR = byte;
                    if(uart0_wr_ptr->ptr)*uart0_wr_ptr->ptr++;   //і якщо є дані на передачу то інкремент поінтера
                }else break; //при переповненому фіфо повторити спробу при наступному перериванні
            }
            else{
                if(uart0_wr_ptr->next)                   //перевірка чи є наступний блок даних на передачу
                    uart0_wr_ptr = uart0_wr_ptr->next;   //запис наступного елементу списку в активну позицію передачі
                else{
                    if(!uart0_sts_bit.cs){
                        uart0_sts_bit.cs = 1;//CRC було відправлено
                        uart0_wr_ptr = &uart0_wr_crc;
                        crc_buf0[0] = CRCtx0>>8;
                        crc_buf0[1] = CRCtx0;
                        //U1THR = CRCtx>>8;
                        //U1THR = CRCtx;
                        //return;
                    }else{
                        U0IER_bit.THREIE = 0; //зупинка передачі
                        uart0_sts_bit.end_tx = 1;             //ознака кінця передачі
                        /*
                        T0MR1 = (20000000/N.UART1.bitrate + 100) + T0TC; //+100uS
                        T0IR_bit.MR1INT = 1;   //int flag reset
                        T0MCR_bit.MR1INT = 1;  //enable interrupt for match channal 1;
                        T0MR3 = T0TC + 300;        //+100uS
                        T0IR_bit.MR3INT = 1;        //int flag reset
                        T0MCR_bit.MR3INT = 1;       //enable interrupt for match channal;
                        */
                        timer_uart[0] = T_switchToRX[N.UART0.bitrate];// +2/4 mS
                        break;
                    }
                }
            }
        }while(1);
        break;
    }

} /* MA_IntHandler_UART0 */

/******************************************************************************/


void uart0_bitrate(int bitrate)
{
    char u0ier_back = U0IER;

    bitrate = 25000000/16/bitrate;

    U0IER = 0; //disable all interrupt from that uart

    U0LCR_bit.DLAB = 1;
    U0DLL = bitrate & 0xff;
    U0DLM = bitrate >> 8 & 0xff;
    U0LCR_bit.DLAB = 0;

    U0IER = u0ier_back;//enable interrupt from that uart
}
/******************************************************************************/
/******************************************************************************/
void uart0_setup(int bitrate,char word_length,char stop_bit,char parity_enable, char parity_select)
{
    U0IER = 0; //disable all interrupt from that uart
    bitrate = B_DLL[bitrate];

    U0LCR_bit.DLAB = 1;
    U0DLL = bitrate & 0xff;//U0DLL = 108;
    U0DLM = bitrate >> 8 & 0xff;//U0DLM = 0;
    
    U0FDR_bit.DIVADDVAL = 1;//для 100МГц для всіх швидкостей однакова
    U0FDR_bit.MULVAL = 2;//для 100МГц для всіх швидкостей однакова
    U0LCR_bit.DLAB = 0;

    //обмежити 5,6,7,8
    word_length = word_length - 5;
    U0LCR_bit.WLS = word_length;//довжина слова

    //обмежити 1,2 тобто 0,1
    stop_bit -= 1;// 1 or 2(1.5for 5bit word length) stop bits
    U0LCR_bit.SBS = stop_bit;

    //0 - disable 1 - enable
    U0LCR_bit.PE = parity_enable;

    // обмежити odd, even, 1, 0  = 0 1 2 3
    U0LCR_bit.PS = parity_select;

    U0FCR_bit.RFR = 1;//reset rx fifo
    U0FCR_bit.TFR = 1;//reset tx fifo
    U0FCR_bit.FCRFE = 0;//fifo enable
    U0FCR_bit.RTLS = 3;//trigger level 14 bytes
    U0FCR_bit.DMA = 0;//DMA disable

    U0IER_bit.RDAIE = 1;//enable RDA interrupt
    U0IER_bit.THREIE = 1;//enable THRE interrupt
}
/*
**===========================================================================
**  5.      INTERNAL FUNCTIONS (declared in Section 3.5)
**===========================================================================
*/


/*
**===========================================================================
** END OF FILE
**===========================================================================
*/


