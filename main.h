


extern PARAMETRS N;
extern AIU aiu[2]; //����� ���������� �����
extern RUNSMOKE RSP; //runtime parameters of smoking
extern int Sts; //current status of system
extern int resAlrm; // reset alarm COMMAND 1-reset 2-soundOFF
extern int RunPrg; // run smoking programm COMMAND


extern TYRREG TyrReg;//��� ������������ ���������� � ������������ �����

extern int pwmLim;//������� ��� ��������� ���

extern int rewrite_esw;//���������� ��������� ��������� ������������

extern const char *build_num;
extern const char build_date[];
extern const char build_time[];

extern unsigned int timer_uart[4];

extern int adc_code;
void read_adc0();
void read_adc1();
