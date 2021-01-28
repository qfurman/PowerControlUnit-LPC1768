/*--- Avoid including this file more than once ---*/
#ifndef _IS_INCLUDED_MACROS_H
#define _IS_INCLUDED_MACROS_H

//якщо нема цього рядка то компілятор дуріє
extern unsigned short const CRC_table[];

#define CRCtx0_init() (CRCtx0=0xFFFF)
#define CRC_tx0(B) CRCtx0=(CRC_table[CRCtx0>>8^B]^CRCtx0<<8)

#define CRCrx0_init() (CRCrx0=0xFFFF)
#define CRC_rx0(B) CRCrx0=(CRC_table[CRCrx0>>8^B]^CRCrx0<<8)


#define CRCtx1_init() (CRCtx1=0xFFFF)
#define CRC_tx1(B) CRCtx1=(CRC_table[CRCtx1>>8^B]^CRCtx1<<8)

#define CRCrx1_init() (CRCrx1=0xFFFF)
#define CRC_rx1(B) CRCrx1=(CRC_table[CRCrx1>>8^B]^CRCrx1<<8)


#define CRCtx2_init() (CRCtx2=0xFFFF)
#define CRC_tx2(B) CRCtx2=(CRC_table[CRCtx2>>8^B]^CRCtx2<<8)

#define CRCrx2_init() (CRCrx2=0xFFFF)
#define CRC_rx2(B) CRCrx2=(CRC_table[CRCrx2>>8^B]^CRCrx2<<8)


#define CRCtx3_init() (CRCtx3=0xFFFF)
#define CRC_tx3(B) CRCtx3=(CRC_table[CRCtx3>>8^B]^CRCtx3<<8)

#define CRCrx3_init() (CRCrx3=0xFFFF)
#define CRC_rx3(B) CRCrx3=(CRC_table[CRCrx3>>8^B]^CRCrx3<<8)

#endif

