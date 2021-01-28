#include "type.h"
//#include "timers.h"
#include "intrinsics.h"

#include <nxp/iolpc1768.h>
#include "macros.h"
#include "uart_0.h"
#include "modbus.h"
#include "main.h"
/*
**===========================================================================
**  2.2     Application include files
**===========================================================================
*/


/*
**---------------------------------------------------
**  IAR MakeApp peripheral modules header files
**  Include the header files used by the application
**---------------------------------------------------
*/


/*
**===========================================================================
**  3.      DECLARATIONS
**  3.1     Internal constants
**===========================================================================
*/




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
unsigned short CRCtx1,CRCrx1;

LIST *uart1_wr_ptr; //global pointer for outgoing thread
LIST *uart1_rd_ptr;  //global pointer for ingoing thread

union{
    char uart1_sts;
    RXTX uart1_sts_bit;
};// @ 0x10003424;;

/*
**===========================================================================
**  4.      GLOBAL FUNCTIONS (declared as 'extern' in some header file)
**===========================================================================
*/

/******************************************************************************/
void start_tx1()
{
    switch(uart1_sts & 0x1f){
      case 0x01:
        uart1_sts_bit.start_tx = 1; //������ ������� ��������
        FIO2CLR_bit.P2_2 = 1;          //����� � ��������
        timer_uart[1] = 1;// 0.25mS
        break;
      case 0x03://�������� ���������� � �������� ��������� - ������ ��������
        //��������
        U1FCR_bit.TFR = 1;//TX FIFO RESET
        U1FCR_bit.RFR = 1;//RX FIFO RESET
        U1IER_bit.THREIE = 1;   //transmite enable

        if(uart1_wr_ptr->count){                       //�������� ������ ���� �� ��������
            uart1_wr_ptr->count--;
            char byte = *uart1_wr_ptr->ptr;
            CRC_tx1(byte);                                //���������� CRC
            U1THR = byte;
            if(uart1_wr_ptr->ptr)*uart1_wr_ptr->ptr++;   //� ���� � ��� �� �������� �� ��������� �������
        }
        break;

      case 0x07://������������� � ������
        FIO2SET_bit.P2_2 = 1;                      //����� � ������
        uart1_sts_bit.start_rx = 1;             //������ ������� �������
        timer_uart[1] = N.UART1.wait_respond * 4;
        break;
      case 0x0F:
        if(U1LSR_bit.DR){    //���� � ����� ������� � ��� �� ������� ��
            modbus_1();
            break;
        }
        //��� ������� ��� ������� ���𳿿 ��������� ������
        uart1_sts_bit.not_respond = 1;
        modbus_1();
        break;
      default:
        uart1_sts = 0;
        uart1_sts_bit.timeout = 1;
        modbus_1();
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

LIST uart1_wr_manag[3];
LIST uart1_rd_manag[2];

char crc_buf1[2];
LIST uart1_wr_crc;  //��� �������� ���������� ����
/******************************************************************************/
void wr_uart1(char *src, unsigned int src_count, char *dest, unsigned int dest_count)
{

    uart1_wr_crc.ptr = &crc_buf1[0];             //�� ������� ��� ����������� ���������� ����
    uart1_wr_crc.count = 2;                     //�������������  � ���������� ���� ���������� ���
    uart1_wr_crc.next = 0;

    uart1_wr_manag[0].ptr = src;                //�������
    uart1_wr_manag[0].count = src_count;        //������� �����
    uart1_wr_manag[0].next = 0;                 //����� ����

    uart1_rd_manag[0].ptr = dest;               //�� �������� �����
    uart1_rd_manag[0].count = dest_count;       //1����-�������, 2�����-������
    uart1_rd_manag[0].next = 0;                 //�����������

    uart1_wr_ptr = &uart1_wr_manag[0];          //����������� ������� ������
    uart1_rd_ptr = &uart1_rd_manag[0];          //����������� ������� �������

    CRCtx1_init();
    CRCrx1_init();
    //N.UART1.simplex
    if(uart1_sts_bit.not_respond){  //���� ������ ������ �� ���� ����� � ����������� �������
        uart1_sts = 0x00;        //���� ������� ������� � �������� start_tx = 1 start_rx = 1;
        uart1_sts_bit.pre_time = 1; //������ ������� ����� ������ ���������
        uart1_sts_bit.start_tx = 1; //������ ������� ��������
        FIO2CLR_bit.P2_2 = 1;       //����� � ��������
        timer_uart[1] = 2;  // 2/4 ��
    }
    else{
        uart1_sts = 0x00;        //���� ������� ������� � �������� start_tx = 1 start_rx = 1;
        uart1_sts_bit.pre_time = 1; //������ ������� ����� ������ ���������
        timer_uart[1] = N.UART1.timeout*4;//����� �����
    }
}
/******************************************************************************/
//void MA_IntHandler_UART0( void )
void UART1_IRQHandler(void)
{
    volatile char dummy;
    char un;

    //if(U0IIR_bit.IP)return;//if no pending interrupts

    switch(U1IIR_bit.IID){//interrupt identification
      case RLS_: //recive line status
            dummy = U1LSR;
        break;

      case CTI_: //caracter timeout indicator
            uart1_sts_bit.timeout = 1;  //������ ������� �������� �������
            //INTSET_bit.SOFTINTSET = 1; //����������� ������
            //modbus_1();
      case RDA_: //recive data available
        if(uart1_sts_bit.timeout)un = 16;//������� ���
        else un = 13;//�������� ����������� ���� ��� ��������� �������� ��������
        do{
            if(U1LSR_bit.DR){                      //���� � ����� ������� � ��� �� ������� ��
                if(uart1_rd_ptr->count){                 //���� � ���� �������
                    if(uart1_rd_ptr->ptr){               //���� ����� �������� � �� ������ ����������
                        char byte = U1RBR;
                        *uart1_rd_ptr->ptr++ = byte;    //���� ������ �� �������� �� ����������� ��� ��������� ����
                        CRC_rx1(byte);
                    }
                    else dummy = U1RBR;                 //������� ���������� ����

                    if(--uart1_rd_ptr->count == 0){      //��������� ������� ���� ��������� �� ��� �������� ���� ������� 0 �� ���������� ������� ��� �� �������
                        if(uart1_rd_ptr->next) uart1_rd_ptr = uart1_rd_ptr->next;//�������� �� � �������� ���� ��� ������� � ����� ���������� �������� ������ � ������� ������� ��������
                        else uart1_sts_bit.end_rx = 1;    //������ ���� �������
                    }
                }
                else dummy = U1RBR;//U1FCR_bit.RFR = 1;//RECIEV FIFO RESET//dummy = U1RBR; //������� ���������� ���� ��� ����������� ����
                //T0MCR_bit.MR0INT = 0;//��� �� �������� �� ����� ���� ������� ��� ����� ������� ����
            }
            else{
                dummy = U1RBR;
                break;
            }
        }while(--un);
        if(uart1_sts_bit.timeout)modbus_1();
        break;

      case THRE_:
        do{
            //if(uart0_sts_bit.end_tx)break;
            //��������
            if(uart1_wr_ptr->count){                             //���� �������� �� �� �� ��� �� ��� ���������
                if(U1LSR_bit.THRE){                              //���� FIFO �� �����������
                    uart1_wr_ptr->count--;                       //��������� ������� ���� �� ��������
                    char byte = *uart1_wr_ptr->ptr;
                        CRC_tx1(byte);
                        U1THR = byte;
                    if(uart1_wr_ptr->ptr)*uart1_wr_ptr->ptr++;   //� ���� � ��� �� �������� �� ��������� �������
                }else break; //��� ������������� ���� ��������� ������ ��� ���������� ����������
            }
            else{
                if(uart1_wr_ptr->next)                   //�������� �� � ��������� ���� ����� �� ��������
                    uart1_wr_ptr = uart1_wr_ptr->next;   //����� ���������� �������� ������ � ������� ������� ��������
                else{
                    if(!uart1_sts_bit.cs){
                        uart1_sts_bit.cs = 1;//CRC ���� ����������
                        uart1_wr_ptr = &uart1_wr_crc;
                        crc_buf1[0] = CRCtx1>>8;
                        crc_buf1[1] = CRCtx1;
                        //U1THR = CRCtx>>8;
                        //U1THR = CRCtx;
                        //return;
                    }else{
                        U1IER_bit.THREIE = 0; //������� ��������
                        uart1_sts_bit.end_tx = 1;//������ ���� ��������
                        timer_uart[1] = T_switchToRX[N.UART1.bitrate];//��� �� �������� ����� + 2/4 ��
                        break;
                    }
                }
            }
        }while(1);
        break;
    }

} /* MA_IntHandler_uart0 */
/******************************************************************************/

void uart1_bitrate(int bitrate)
{
    char u1ier_back = U1IER;

    bitrate = 25000000/16/bitrate;

    U1IER = 0; //disable all interrupt from that uart

    U1LCR_bit.DLAB = 1;
    U1DLL = bitrate & 0xff;
    U1DLM = bitrate >> 8 & 0xff;
    U1LCR_bit.DLAB = 0;

    U1IER = u1ier_back;//enable interrupt from that uart
}
/******************************************************************************/
/******************************************************************************/
void uart1_setup(int bitrate,char word_length,char stop_bit,char parity_enable, char parity_select)
{
    U1IER = 0; //disable all interrupt from that uart

    bitrate = B_DLL[bitrate];

    U1LCR_bit.DLAB = 1;
    U1DLL = bitrate & 0xff;//U0DLL = 108;
    U1DLM = bitrate >> 8 & 0xff;//U0DLM = 0;
    
    U1FDR_bit.DIVADDVAL = 1;//��� 100��� ��� ��� ���������� ��������
    U1FDR_bit.MULVAL = 2;//��� 100��� ��� ��� ���������� ��������
    U1LCR_bit.DLAB = 0;
    
    //�������� 5,6,7,8
    word_length = word_length - 5;
    U1LCR_bit.WLS = word_length;//������� �����

    //�������� 1,2 ����� 0,1
    stop_bit -= 1;// 1 or 2(1.5for 5bit word length) stop bits
    U1LCR_bit.SBS = stop_bit;

    //0 - disable 1 - enable
    U1LCR_bit.PE = parity_enable;

    // �������� odd, even, 1, 0  = 0 1 2 3
    U1LCR_bit.PS = parity_select;

    U1FCR_bit.RFR = 1;//reset rx fifo
    U1FCR_bit.TFR = 1;//reset tx fifo
    U1FCR_bit.FCRFE = 1;//fifo enable
    U1FCR_bit.RTLS = 3;//trigger level 14 bytes

    U1IER_bit.RDAIE = 1;//enable RDA interrupt
    U1IER_bit.THREIE = 1;//enable THRE interrupt
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

