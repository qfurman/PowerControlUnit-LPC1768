/*
**===========================================================================
**  1       GENERAL
**  1.1     Revisions
**
**  Please read the IAR MakeApp for Philips LPC2138-48 readme file
**
**
**===========================================================================
*/

/*
**===========================================================================
**  1.2     References
**
**  No   Identification          Name or Description
**  ==   ===================     ================================
**  1    November 22, 2004       Philips LPC2131/2132/2138
**                               User Manual, Preliminary Release
**
**===========================================================================
*/

/*
**===========================================================================
**  1.3     Re-definition guard
**===========================================================================
*/

/*--- Avoid including this file more than once ---*/
#ifndef _IS_INCLUDED_UART_3_H
#define _IS_INCLUDED_UART_3_H

/* usercode/template redefinition guard */
/*
**===========================================================================
**  2.      INCLUDE FILES
**  2.1     Standard include files
**===========================================================================
*/

/*
**===========================================================================
**  2.2     Application include files
**===========================================================================
*/

/*
**===========================================================================
**  3.      DECLARATIONS
**  3.1     Global constants
**===========================================================================
*/

/*
**===========================================================================
**  3.2     Global macros
**===========================================================================
*/






/*
**===========================================================================
**  3.3     Global type definitions
**===========================================================================
*/

/*
**===========================================================================
**  3.4     Global variables (defined in some implementation file)
**===========================================================================
*/
void start_tx3();
extern unsigned short CRCtx3,CRCrx3;

extern LIST *uart3_wr_ptr; //global pointer for outgoing thread
extern LIST *uart3_rd_ptr;  //global pointer for ingoing thread

extern union{
    unsigned int uart3_sts;
    RXTX uart3_sts_bit;
};

extern LIST uart3_wr_manag[3];
extern LIST uart3_rd_manag[2];

extern char crc_buf3[2];
extern LIST uart3_wr_crc;  //для відправки контрольної суми
/*
**===========================================================================
**  4.      GLOBAL FUNCTIONS (defined in some implementation file)
**===========================================================================
*/



extern void uart3_setup(int bitrate,char word_length,char stop_bit,char parity_enable, char parity_select);

extern void uart3_bitrate(int bitrate);
extern void uart3_word_length(char temp);
extern void uart3_stop_bit(char temp);
extern void uart3_parity_enable(char temp);
extern void uart3_parity_select(char temp);

void wr_uart3(char *src, unsigned int src_count, char *dest, unsigned int dest_count);
#endif /* _IS_INCLUDED_UART_3_H Match the re-definition guard */

/*
**===========================================================================
** END OF FILE
**===========================================================================
*/


