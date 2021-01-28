


extern PARAMETRS N;
extern AIU aiu[2]; //плати аналогових входів
extern RUNSMOKE RSP; //runtime parameters of smoking
extern int Sts; //current status of system
extern int resAlrm; // reset alarm COMMAND 1-reset 2-soundOFF
extern int RunPrg; // run smoking programm COMMAND


extern TYRREG TyrReg;//для тиристорного регулятора в програмованій логіці

extern int pwmLim;//границя для генерації шим

extern int rewrite_esw;//переписати вбудоване програмне забезпечення

extern const char *build_num;
extern const char build_date[];
extern const char build_time[];

extern unsigned int timer_uart[4];

extern int adc_code;
void read_adc0();
void read_adc1();
