
#ifndef _IS_INCLUDED_TYPE_H
#define _IS_INCLUDED_TYPE_H

// RTC#######################################################
typedef struct {
	unsigned short year;  // year value
	unsigned char month;  // month value
	unsigned char day;    // day value

	// the below member is used in get-operation
	unsigned char DOW;    // Day of week
	unsigned short DOY;    // Day of year
} LPC_Rtc_Date_t;

typedef struct {
	unsigned char hour;   // hour value
	unsigned char minute; // minute value
	unsigned char second; // second value
} LPC_Rtc_Time_t;

typedef struct {
	unsigned short year;  // year value
	unsigned char month;  // month value
	unsigned char day;    // day value

	unsigned char hour;   // hour value
	unsigned char minute; // minute value
	unsigned char second; // second value

	// the below member is used in get-operation
	unsigned char DOW;    // Day of week
	unsigned short DOY;    // Day of year
} LPC_Rtc_DateTime_t;
// RTC  END #################################################


//���������
#define kays_buf_size 16
struct STACK{
    unsigned char buf[kays_buf_size];//����� ��� ���������� ���²�
    unsigned char index;
};
typedef struct STACK KAY_STACK;

struct REPEAT{
    unsigned short first;       //��� �� ������� ����������
    unsigned short all_next;    //��� �� ���������� ������� ����������
    unsigned short all_next_next;    //��� �� ���������� ������� ����������
};
typedef struct REPEAT KAY_REPEAT;
//����� ���������

//For management
struct list{
    char *ptr;              //Operands pointer
    unsigned int count;     //count of bytes
    struct list *next;      //pointer for next element of list
};
typedef struct list LIST;

struct rxtx{
    char pre_time   : 1;//������� �� ���������� ��������������� ���� ������� ����
    char start_tx   : 1;//������� ��������
    char end_tx     : 1;//����� ��������
    char start_rx   : 1;//������� �������
    char end_rx     : 1;//����� �������
    char timeout    : 1;//������� ��� ������
    char cs         : 1;//crc ���������
    char not_respond: 1;//��� ���������� ������ ������
};
typedef struct rxtx RXTX;

//��������� ����
struct menu_man{
    int type;
    int change; //0-����� ������������; 1-������������; 2-�������� ����-������� �� ���������� ������ ����; 128-�����������
    char *format[2];//��������� ����� � ��������
    int *number;//�� �����-��������
    int min;    //���������� ��������
    int max;    //����������� ��������
    int fract;//������� ����� ���� ����
    void *next;//��������� ��������� � MENU_MAN*
    int size;//����� ���������� ������

    char(*pre_funct)();
    void(*pre_enter)(char);
    char(*post_enter)();
    void(*up_funct)();
    void(*down_funct)();
    void(*left_funct)();
    void(*right_funct)();
};
typedef struct menu_man MENU_MAN;
//��� �����
struct RUN_TIME_FLAG{
    unsigned int uart0_change    :1; //��� ���� �������� ���� ������
    unsigned int uart1_change    :1; //��� ���� �������� ���� ������
    unsigned int                 :30;
};
typedef struct RUN_TIME_FLAG RTF;
//��������� UARTs
struct UART{
    int bitrate;
    int word_length;
    int stop_bit;
    int parity_enable;
    int parity_select;
    int simplex;
    int wait_respond;
    int timeout;
};
typedef struct UART UART;

//��� ����� ����������� �����
typedef struct{
         int sts;//������ 0 
         int adc;//������� ��� ���  1-2
         int ADC0;//�������� ���  3-4
         int ADC1;//�������� ���  5-6
         int ADC2;//�������� ���  7-8
         int ntc0;//9
         int ntc1;//10
         int cmd;//11
         int dacs;//values for DAC0 in low byte and DAC1 in hight byte -12
         int offset0;//������� 13-14
         int gain0;//��������� 15-16
         int Kg0;//����.��������� 17
         int K0;//������� ������ �� ����� �������� �����������  18       
         int offset1;//������� 19-20
         int gain1;//��������� 21-22
         int Kg1;//����.��������� 23
         int K1;//������� ������ �� ����� �������� ����������� 24
         int offset2;//������� 25-26
         int gain2;//��������� 27-28
         int Kg2;//����.��������� 29
         int K2;//������� ������ �� ����� �������� �����������  30 ��� � ������ ������� ���������� ����� ���� 1
         int ntcOffset0;//adc offset for two ntc temperature channels 31
         int ntcGain0;//adc gain for two ntc temperature channels 32
         int ntcOffset1;//adc offset for two ntc temperature channels 33
         int ntcGain1;//adc gain for two ntc temperature channels 34
         int address;//35
         int baud; //36
}AIU;

//��� �������
typedef struct{
    int status;//������,
    int comm;//�������
    int code;//��� ��� �������� (2 �����, �������� ������)
}FROM;

typedef struct{
    unsigned int comm;//�������
    unsigned int task;
    unsigned int req;
    unsigned char err;//������� �� �� ���������� �������� �����
    unsigned char err_cntr;//�������� �������
    char chill;//������ ����������
    unsigned char p;//�������� �� ���� ������
    unsigned short buf[32];//����� ��� ���� ���
}ADC;


typedef struct{
  int x;//�������
  int y;//����
}NORM;

//������������ ���������--------------------------------------------------------
typedef struct{
  int T[2];//�����
  int ON[2];//��� ���������
  int contrast;//������� �����������
  int bright;//������� ���������
  int reanim;//�������� ���������
}LFLASH;

//PID regulator-----------------------------------------------------------------
typedef struct{
  int ON;// on/off regulator
  int spT;//setpoint
  int speed;//dT/sec.
  int max;//upper lim
  int min;//lower lim 
  int K;//���� �������������
  int Tq;//����� �����������
  int Ki;//���� ������������
  int Ti;//��� ������������
  int Kd;//���� ���������������
  int Td;//��� ���������������
} PID_REG;

//one smoking stage-------------------------------------------------------------
typedef struct{
  int tC;// temperature of current smoking stage
  int rH;// relative humidity of current smoking stage
  int time;// time in seconds of current smoking stage
} SMOKING;

/*============================================================================*/
//�� ��������� ���� ����������� ���--------------------------------------------
struct PARAMETRS{
    UART UART0;//8 parameters
    UART UART1;
    UART UART2;
    UART UART3;
    int snap_time;//������� �������� ������ � �����
    int archive_addr;//������ ��� ��������� ������
    int password;//������

    int ADC_COEF[8];//����������� ���
    int ADC_OFF[8];//������� ��� ���
    int rtc_on;//��� ���������� ���������

    LFLASH lcd;//��� �������������
    int Lng;//���� 0- ��� 1-���
    //--modbus begin--  
    int circSpd;  // circulation fan acceleration
    int exhSpd;   // exhaust acceleration
    int Krh;      // proportional coeff for regulating of relative humidity
    int TqRH;     // quantization time for works regulator of relative humidity
    int regRHon;  // regulator of relative humidity ON if 1
    PID_REG PID;  //regulator
    int minSCR0;  // minimal out for scr0 for mains voltage and current regulators
    int maxSCR0;  // max out for scr0 
    int minSCR1;  // minimal out for circulation fan for start rotating 
    int minSCR2;  // minimal out for exhaust fan for start rotating 
    int maxSCR1;  // max out for circulation fan for start rotating 
    int maxSCR2;  // max out for exhaust fan for start rotating 
    int minI;     // min value of current for main power thyristors output
    int maxI;     // max value of current for main power thyristors output
    //--modbus end-- 
    SMOKING dsp[5];//DEFAULP powerup smoking parametrs
    
    int VoltSpd;  // voltage & current acceleration
    int Kv;       // proportional coeff for regulating of voltage
    int TqV;      // quantization time for works regulator of voltage
    int regVon;   // regulator mains voltage correction ON/OFF ON if 1
    int CurrSpd;  // voltage & current acceleration
    int Kc;       // proportional coeff for regulating of current
    int TqC;      // quantization time for works regulator of current
    int regCon;   // regulator of relative current ON if 1
    int SCRRange; // range of power buffer counter for SCRs outputs  
    int LoadCorOn;// load voltage correction ON/OFF not used

    int Talrm;    // hysteresis of thermal alarm
    int RHalrm;   // hysteresis  of Relative Humidity alarm
    int cToff;  //cooler OFF temperature
    int cTon;   //cooler ON temperature
    int tyrOFF; //thyristors temperature to stop work
    int tyrON;  //thyristors temperature to resume work again  
    int t_comp; //for apply themperature of cold clamps 
};
typedef struct PARAMETRS PARAMETRS;
//------------------------------------------------------------------------------
/*============================================================================*/

//---²������Ͳ �������
struct VIRTUAL_TIMER
{
    unsigned int  tick;//����� �� ����
    unsigned char event;
};
typedef struct VIRTUAL_TIMER VIRTUAL_TIMER;
//---²������Ͳ ������� ʲ����

struct FLASH_ADDR{
    unsigned int addr    : 20;//20 bit max address - fffff
    unsigned int         : 13;
};
typedef struct FLASH_ADDR FLASH_ADDR;

//65535 ��.��.��.��:��:�� 00.0 00.0 00.0 00.0 00.0 00.0 00.0 00.0 LF CR
#pragma pack(1)
typedef struct {
    unsigned int time;//���������� ���
    unsigned char T[8];//���������� 1-�� 0.5�
}REC;
#pragma pack()

//�����˲��� �������� ���������� ��ղ��
struct A_MNG{
    unsigned int flash_addr;//������ � ���� ���� ����� ���������� ��������� �����
    unsigned int rec_num;//����� �������� � ����� ���������
    unsigned int global;//���������� ����� ������

    union{
        REC rec[21];//��� ������ 12*21=252 ,�����
        char rec_buf[256];
        int rec_ibuf[64];// � ������� ���������� ���������� ����� ������
    };

};
typedef struct A_MNG A_MNG;

struct TIME_ARCH{
    unsigned int sec     : 6;
    unsigned int sec_ov  : 2;
    unsigned int min     : 6;
    unsigned int min_ov  : 2;
    unsigned int hour    : 5;
    unsigned int hour_ov : 3;
    unsigned int day     : 3;
    unsigned int day_ov  : 5;
};
typedef struct TIME_ARCH TIME_ARCH;

struct ARCH_VIEW{
    unsigned int flash_addr;//������ � ���� ������ ����� ������ �����
    unsigned char humid;//���������� ��������
    unsigned char flag1;//������
    unsigned char flag2;
    unsigned char r;//����� ������ ��� ��������� 0-20
    int year;
    int month;
    int day;
    int day_end;//���� ������� ��������
    union{
        REC rec[21];//��� ������ 12*21=252 ,�����
        char rec_buf[256];
        int rec_ibuf[64];// � ������� ���������� ���������� ����� ������
    };
};
typedef struct ARCH_VIEW ARCH_VIEW;

//������ ��������� � ������� ��������� a_mng
#define a_mng_rmtrn_addr 0x400

typedef struct{
    int point;//1- �������� 0-� �����, 2- �������� 1-� � ������� � 0 ���� ������
    //LPC_Rtc_DateTime_t DateTime;
    char name[16];//��� ���� ����� ������������ ������
    char fname[16];//��� ��� ������ �� ������
    unsigned short p;//��������
    unsigned short cntr;//�������� ���� � �� ���� �� ��������
    union{
      char rec[512];//
      int reci[128];//
    };
}REC_ARR;

// ��� ���������� ����� � ����������
typedef struct{
    int *addr;//��������� ������ ���������
    char count;//������� ����� ���������
}MS;//mobus slave

/* ������� �������, ���� ������ ������ �� ��������������� � ������ ���������*/
typedef struct{
    unsigned short min;   //min. ��������
    unsigned short max;   //max. ��������
    int *adr;// ������ ���������
}MMA;

typedef struct{
  int acc;//acuumulator
  int i;//iterator
  int b;//wich buffer use at this time
  int F;//flag of full buffer
  int buf[2][64];
  int code;//adc code
  int A;//result
}ADCMNG;

typedef struct{
    int flag;//������ ����, �� ����� � ����� ���
    int lastDate;// ������ ���������
}FOR_SD_WR;

//��� ������������ ���������� � ������������ �����
typedef struct{
  int enable;//�������� ������
  int read;//�������� ��� �������
  int SCR0;//�������� ��� ������
  int SCR1;//�������� ��� ������
  int SCR2;//�������� ��� ������
//  int POUT;   // percent of heating power 100.00
  int circ;   // percent of circulation out 100.00
  int exhaust;// percent of exhauster out 100.00
}TYRREG;

//
typedef struct{
  int code;//adc code
  int temp;//temperature
}CvsT;

//��������� ��������� PID ����������-------------------------------------------
typedef struct{
  int just_on;//������ ������� ��������� ����������
  int change;//�������� ����������
  int rSp;//runtime setpoint for shockless change
  int Tq;//��� - �������� ���� ����������� �������
  int Td;//��� - �������� ���� ������������ �������
  int Ti;//��� - �������� ���� ����������� �������
  int out;//������������� �����  
  int last_t;//�������� �������� ����������� ��� ���������� ���. ��������
  int ie;//������������ �������
  int P;//����������� �������� 
  int I;//����������� ��������
  int D;//������������ �������� 
  int lastP;// last proportional
  int lastI;// last interal
  int lastD;// last differential
} RUNPID;

//runtime parameters of smoking-------------------------------------------------
typedef struct {
  int phase; // stadium of smoking
  int PHtime; // time of current phase
  SMOKING Rsp[5]; // runtime smoking parameters
} RUNSMOKE;

//runtime parameters for voltage and current regulation-------------------------
typedef struct {
  int Vsp;      // voltage setpoint for regulating load voltage
  int Csp;      // current setpoint for regulating load current
  int iSp;      // current setpoint for regulating load current delayed
  int R;        // resistance
  int prUm;     // previous mains voltage
  int Ucorr;    // mains voltage correction 
  int Ibuff[10];// currents buffer
  int Iptr;     // buffer pointer   
  int lastDi;   // last delta of regulator of current
  int lastDu;   // last delta of mains voltage follower
} RUNPOWER;

#endif
