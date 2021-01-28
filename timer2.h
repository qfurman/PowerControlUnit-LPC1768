
//================================================================================
__ramfunc void TMR2_IRQHandler( void )
{

    if ( T2IR & T2IR_MR0INT )
    {
      T2IR_bit.MR0INT = 1; /*--- Clear interrupt flag ---*/
    }

    if ( T2IR & T2IR_MR1INT )
    {
      T2IR_bit.MR1INT = 1;
    }

    if ( T2IR & T2IR_MR2INT )
    {
      T2MCR &= ~(1<<6);//зняти MR2I переривання по каналу MR2
      T2IR_bit.MR2INT = 1;
    }


 //T2IR = 0x3f;//зняти 
} /* MA_IntHandler_TIMER2 */

//================================================================================
