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
unsigned short CRCtx2,CRCrx2;

LIST *uart2_wr_ptr; //global pointer for outgoing thread
LIST *uart2_rd_ptr;  //global pointer for ingoing thread

union{
    char uart2_sts;
    RXTX uart2_sts_bit;
};// @ 0x10003428;

/*
**===========================================================================
**  4.      GLOBAL FUNCTIONS (declared as 'extern' in some header file)
**===========================================================================
*/

/******************************************************************************/
void start_tx2()
{
    switch(uart2_sts & 0x1f){
      case 0x01:
        uart2_sts_bit.start_tx = 1; //������ ������� ��������
        FIO2CLR_bit.P2_13 = 1;          //����� � ��������
        timer_uart[2] = 1;// 0.25mS
        break;
      case 0x03://�������� ���������� � �������� ��������� - ������ ��������
        //��������
        U2FCR_bit.TFR = 1;//TX FIFO RESET
        U2FCR_bit.RFR = 1;//RX FIFO RESET
        U2IER_bit.THREIE = 1;   //transmite enable

        if(uart2_wr_ptr->count){                       //�������� ������ ���� �� ��������
            uart2_wr_ptr->count--;
            char byte = *uart2_wr_ptr->ptr;
            CRC_tx2(byte);                                //���������� CRC
            U2THR = byte;
            if(uart2_wr_ptr->ptr)*uart2_wr_ptr->ptr++;   //� ���� � ��� �� �������� �� ��������� �������
        }
        break;

      case 0x07://������������� � ������
        FIO2SET_bit.P2_13 = 1;                      //����� � ������
        uart2_sts_bit.start_rx = 1;             //������ ������� �������
        timer_uart[2] = N.UART2.wait_respond * 4;
        break;
      case 0x0F:
        if(U2LSR_bit.DR){    //���� � ����� ������� � ��� �� ������� ��
            modbus_2();
            break;
        }
        //��� ������� ��� ������� ���𳿿 ��������� ������
        uart2_sts_bit.not_respond = 1;
        modbus_2();
        break;
      default:
        uart2_sts = 0;
        uart2_sts_bit.timeout = 1;
        modbus_2();
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

LIST uart2_wr_manag[3];
LIST uart2_rd_manag[2];

char crc_buf2[2];
LIST uart2_wr_crc;  //��� �������� ���������� ����
/******************************************************************************/
void wr_uart2(char *src, unsigned int src_count, char *dest, unsigned int dest_count)
{

    uart2_wr_crc.ptr = &crc_buf2[0];             //�� ������� ��� ����������� ���������� ����
    uart2_wr_crc.count = 2;                     //�������������  � ���������� ���� ���������� ���
    uart2_wr_crc.next = 0;

    uart2_wr_manag[0].ptr = src;                //�������
    uart2_wr_manag[0].count = src_count;        //������� �����
    uart2_wr_manag[0].next = 0;                 //����� ����

    uart2_rd_manag[0].ptr = dest;               //�� �������� �����
    uart2_rd_manag[0].count = dest_count;       //1����-�������, 2�����-������
    uart2_rd_manag[0].next = 0;                 //�����������

    uart2_wr_ptr = &uart2_wr_manag[0];          //����������� ������� ������
    uart2_rd_ptr = &uart2_rd_manag[0];          //����������� ������� �������

    CRCtx2_init();
    CRCrx2_init();
    //N.UART1.simplex
    if(uart2_sts_bit.not_respond){  //���� ������ ������ �� ���� ����� � ����������� �������
        uart2_sts = 0x00;        //���� ������� ������� � �������� start_tx = 1 start_rx = 1;
        uart2_sts_bit.pre_time = 1; //������ ������� ����� ������ ���������
        uart2_sts_bit.start_tx = 1; //������ ������� ��������
        FIO2CLR_bit.P2_13 = 1;       //����� � ��������
        timer_uart[2] = 2;  // 2/4 ��
    }
    else{
        uart2_sts = 0x00;        //���� ������� ������� � �������� start_tx = 1 start_rx = 1;
        uart2_sts_bit.pre_time = 1; //������ ������� ����� ������ ���������
        timer_uart[2] = N.UART2.timeout*4;//����� �����
    }
}
/******************************************************************************/
//void MA_IntHandler_UART2( void )
void UART2_IRQHandler(void)
{
    volatile char dummy;
    char un;

    //if(U0IIR_bit.IP)return;//if no pending interrupts

    switch(U2IIR_bit.IID){//interrupt identification
      case RLS_: //recive line status
            dummy = U2LSR;
        break;

      case CTI_: //caracter timeout indicator
            uart2_sts_bit.timeout = 1;  //������ ������� �������� �������
            //INTSET_bit.SOFTINTSET = 1; //����������� ������
            //modbus_2();
      case RDA_: //recive data available
        if(uart2_sts_bit.timeout)un = 16;//������� ���
        else un = 13;//�������� ����������� ���� ��� ��������� �������� ��������
        do{
            if(U2LSR_bit.DR){                      //���� � ����� ������� � ��� �� ������� ��
                if(uart2_rd_ptr->count){                 //���� � ���� �������
                    if(uart2_rd_ptr->ptr){               //���� ����� �������� � �� ������ ����������
                        char byte = U2RBR;
                        *uart2_rd_ptr->ptr++ = byte;    //���� ������ �� �������� �� ����������� ��� ��������� ����
                        CRC_rx2(byte);
                    }
                    else dummy = U2RBR;                 //������� ���������� ����

                    if(--uart2_rd_ptr->count == 0){      //��������� ������� ���� ��������� �� ��� �������� ���� ������� 0 �� ���������� ������� ��� �� �������
                        if(uart2_rd_ptr->next) uart2_rd_ptr = uart2_rd_ptr->next;//�������� �� � �������� ���� ��� ������� � ����� ���������� �������� ������ � ������� ������� ��������
                        else uart2_sts_bit.end_rx = 1;    //������ ���� �������
                    }
                }
                else dummy = U2RBR;//U2FCR_bit.RFR = 1;//RECIEV FIFO RESET//dummy = U2RBR; //������� ���������� ���� ��� ����������� ����
                //T0MCR_bit.MR0INT = 0;//��� �� �������� �� ����� ���� ������� ��� ����� ������� ����
            }
            else{
                dummy = U2RBR;
                break;
            }
        }while(--un);
        if(uart2_sts_bit.timeout)modbus_2();
        break;

      case THRE_:
        do{
            //if(uart2_sts_bit.end_tx)break;
            //��������
            if(uart2_wr_ptr->count){                             //���� �������� �� �� �� ��� �� ��� ���������
                if(U2LSR_bit.THRE){                              //���� FIFO �� �����������
                    uart2_wr_ptr->count--;                       //��������� ������� ���� �� ��������
                    char byte = *uart2_wr_ptr->ptr;
                        CRC_tx2(byte);
                        U2THR = byte;
                    if(uart2_wr_ptr->ptr)*uart2_wr_ptr->ptr++;   //� ���� � ��� �� �������� �� ��������� �������
                }else break; //��� ������������� ���� ��������� ������ ��� ���������� ����������
            }
            else{
                if(uart2_wr_ptr->next)                   //�������� �� � ��������� ���� ����� �� ��������
                    uart2_wr_ptr = uart2_wr_ptr->next;   //����� ���������� �������� ������ � ������� ������� ��������
                else{
                    if(!uart2_sts_bit.cs){
                        uart2_sts_bit.cs = 1;//CRC ���� ����������
                        uart2_wr_ptr = &uart2_wr_crc;
                        crc_buf2[0] = CRCtx2>>8;
                        crc_buf2[1] = CRCtx2;
                        //U2THR = CRCtx>>8;
                        //U2THR = CRCtx;
                        //return;
                    }else{
                        U2IER_bit.THREIE = 0; //������� ��������
                        uart2_sts_bit.end_tx = 1;//������ ���� ��������
                        timer_uart[2] = T_switchToRX[N.UART2.bitrate];//��� �� �������� ����� + 2/4 ��
                        break;
                    }
                }
            }
        }while(1);
        break;
    }

} /* MA_IntHandler_uart2 */
/******************************************************************************/
/******************************************************************************/
void uart2_setup(int bitrate,char word_length,char stop_bit,char parity_enable, char parity_select)
{
    U2IER = 0; //disable all interrupt from that uart

    bitrate = B_DLL[bitrate];

    U2LCR_bit.DLAB = 1;
    U2DLL = bitrate & 0xff;//U0DLL = 108;
    U2DLM = bitrate >> 8 & 0xff;//U0DLM = 0;
    
    U2FDR_bit.DIVADDVAL = 1;//��� 100��� ��� ��� ���������� ��������
    U2FDR_bit.MULVAL = 2;//��� 100��� ��� ��� ���������� ��������
    U2LCR_bit.DLAB = 0;
    
    //�������� 5,6,7,8
    word_length = word_length - 5;
    U2LCR_bit.WLS = word_length;//������� �����

    //�������� 1,2 ����� 0,1
    stop_bit -= 1;// 1 or 2(1.5for 5bit word length) stop bits
    U2LCR_bit.SBS = stop_bit;

    //0 - disable 1 - enable
    U2LCR_bit.PE = parity_enable;

    // �������� odd, even, 1, 0  = 0 1 2 3
    U2LCR_bit.PS = parity_select;

    U2FCR_bit.RFR = 1;//reset rx fifo
    U2FCR_bit.TFR = 1;//reset tx fifo
    U2FCR_bit.FCRFE = 1;//fifo enable
    U2FCR_bit.RTLS = 3;//trigger level 14 bytes

    U2IER_bit.RDAIE = 1;//enable RDA interrupt
    U2IER_bit.THREIE = 1;//enable THRE interrupt
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

