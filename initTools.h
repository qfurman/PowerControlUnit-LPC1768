

void NVIC_IntEnable (unsigned int IntNumber);
void vIAP_ReinvokeISP (void);
void InitClock (void);
void SYSTimer_init (void);
void Timer0_init (void);
void Timer1_init (void);
void Timer2_init (void);
void Timer3_init (void);
void SSP0_SSP1_init (void);
void UARTs_init (void);
void PCBpins_init (void);

void PWMs_init (void);
void lcd_pwm_renew (void);
void pwm_20mA_out (int refB);

void UARTs_PostInit (void);
void EXTERNAL_INT_init (void);
void AT25_MEMORY_init (void);
void ReWR_flash (void);