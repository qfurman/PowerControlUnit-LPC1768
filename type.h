
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


//клавіатура
#define kays_buf_size 16
struct STACK{
    unsigned char buf[kays_buf_size];//БУФЕР ДЛЯ НАТИСНУТИХ КЛАВІШ
    unsigned char index;
};
typedef struct STACK KAY_STACK;

struct REPEAT{
    unsigned short first;       //час до першого повторення
    unsigned short all_next;    //час до повторення кожного наступного
    unsigned short all_next_next;    //час до повторення кожного наступного
};
typedef struct REPEAT KAY_REPEAT;
//кінець клавіатури

//For management
struct list{
    char *ptr;              //Operands pointer
    unsigned int count;     //count of bytes
    struct list *next;      //pointer for next element of list
};
typedef struct list LIST;

struct rxtx{
    char pre_time   : 1;//таймаут між передачами використовується якщо відповідь була
    char start_tx   : 1;//початок передачі
    char end_tx     : 1;//кінець передачі
    char start_rx   : 1;//початок прийому
    char end_rx     : 1;//кінець прийому
    char timeout    : 1;//таймаут при прийомі
    char cs         : 1;//crc упакована
    char not_respond: 1;//час очікування відповіді вийшов
};
typedef struct rxtx RXTX;

//управління меню
struct menu_man{
    int type;
    int change; //0-тільки відображується; 1-модифікується; 2-виконати пост-функцію по натисканню кнопки ввод; 128-парольдіалог
    char *format[2];//текстовий рядок з форматом
    int *number;//на число-параметр
    int min;    //мінімальньне значення
    int max;    //максимальне значення
    int fract;//клькість знаків після коми
    void *next;//наступний претворит в MENU_MAN*
    int size;//розмір наступного підменю

    char(*pre_funct)();
    void(*pre_enter)(char);
    char(*post_enter)();
    void(*up_funct)();
    void(*down_funct)();
    void(*left_funct)();
    void(*right_funct)();
};
typedef struct menu_man MENU_MAN;
//біти ознак
struct RUN_TIME_FLAG{
    unsigned int uart0_change    :1; //хоч один параметр було змінено
    unsigned int uart1_change    :1; //хоч один параметр було змінено
    unsigned int                 :30;
};
typedef struct RUN_TIME_FLAG RTF;
//параметри UARTs
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

//ДЛЯ ПЛАТИ АНАЛОГОВОГО ВХОДУ
typedef struct{
         int sts;//статус 0 
         int adc;//швидкий код ацп  1-2
         int ADC0;//повільний код  3-4
         int ADC1;//повільний код  5-6
         int ADC2;//повільний код  7-8
         int ntc0;//9
         int ntc1;//10
         int cmd;//11
         int dacs;//values for DAC0 in low byte and DAC1 in hight byte -12
         int offset0;//зміщення 13-14
         int gain0;//підсилення 15-16
         int Kg0;//коеф.підсилення 17
         int K0;//кількість вибірок за якими робиться усереднення  18       
         int offset1;//зміщення 19-20
         int gain1;//підсилення 21-22
         int Kg1;//коеф.підсилення 23
         int K1;//кількість вибірок за якими робиться усереднення 24
         int offset2;//зміщення 25-26
         int gain2;//підсилення 27-28
         int Kg2;//коеф.підсилення 29
         int K2;//кількість вибірок за якими робиться усереднення  30 або в даному випадку пропустити канал якщо 1
         int ntcOffset0;//adc offset for two ntc temperature channels 31
         int ntcGain0;//adc gain for two ntc temperature channels 32
         int ntcOffset1;//adc offset for two ntc temperature channels 33
         int ntcGain1;//adc gain for two ntc temperature channels 34
         int address;//35
         int baud; //36
}AIU;

//для читання
typedef struct{
    int status;//статус,
    int comm;//команда
    int code;//код АЦП поточний (2 байти, молодший вперед)
}FROM;

typedef struct{
    unsigned int comm;//команда
    unsigned int task;
    unsigned int req;
    unsigned char err;//девайси що не відповідають належним чином
    unsigned char err_cntr;//лічильник помилок
    char chill;//ознака заспокоєння
    unsigned char p;//вказівник на місце запису
    unsigned short buf[32];//буфер для кодів ацп
}ADC;


typedef struct{
  int x;//напруга
  int y;//тиск
}NORM;

//поморнування курсорами--------------------------------------------------------
typedef struct{
  int T[2];//період
  int ON[2];//час включення
  int contrast;//процент контрасності
  int bright;//процент яскравості
  int reanim;//наявність реанімації
}LFLASH;

//PID regulator-----------------------------------------------------------------
typedef struct{
  int ON;// on/off regulator
  int spT;//setpoint
  int speed;//dT/sec.
  int max;//upper lim
  int min;//lower lim 
  int K;//коеф пропорційності
  int Tq;//період квантування
  int Ki;//коеф інтегрування
  int Ti;//час інтегрування
  int Kd;//коеф диференціювання
  int Td;//час дифиренціювання
} PID_REG;

//one smoking stage-------------------------------------------------------------
typedef struct{
  int tC;// temperature of current smoking stage
  int rH;// relative humidity of current smoking stage
  int time;// time in seconds of current smoking stage
} SMOKING;

/*============================================================================*/
//всі параметри меню знаходяться тут--------------------------------------------
struct PARAMETRS{
    UART UART0;//8 parameters
    UART UART1;
    UART UART2;
    UART UART3;
    int snap_time;//часовий інтревал запису у архів
    int archive_addr;//адреса для перегляду архіву
    int password;//пароль

    int ADC_COEF[8];//коефіцієнти ацп
    int ADC_OFF[8];//зміщення для ацп
    int rtc_on;//для відключення годинника

    LFLASH lcd;//для поморгуванням
    int Lng;//мова 0- рус 1-укр
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

//---ВІРТУАЛЬНІ ТАЙМЕРИ
struct VIRTUAL_TIMER
{
    unsigned int  tick;//відліки до нуля
    unsigned char event;
};
typedef struct VIRTUAL_TIMER VIRTUAL_TIMER;
//---ВІРТУАЛЬНІ ТАЙМЕРИ КІНЕЦЬ

struct FLASH_ADDR{
    unsigned int addr    : 20;//20 bit max address - fffff
    unsigned int         : 13;
};
typedef struct FLASH_ADDR FLASH_ADDR;

//65535 РР.ММ.ДД.ГГ:ХХ:СС 00.0 00.0 00.0 00.0 00.0 00.0 00.0 00.0 LF CR
#pragma pack(1)
typedef struct {
    unsigned int time;//упакований час
    unsigned char T[8];//темература 1-біт 0.5С
}REC;
#pragma pack()

//УПРАВЛІННЯ ПРОЦЕСОМ ФОРМУВАННЯ АРХІВУ
struct A_MNG{
    unsigned int flash_addr;//адреса у флеш куди треба записувати наступний запис
    unsigned int rec_num;//номер підзапису в буфері зберігання
    unsigned int global;//глобальний номер запису

    union{
        REC rec[21];//самі записи 12*21=252 ,байти
        char rec_buf[256];
        int rec_ibuf[64];// в останній записувати глобальний номер запису
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
    unsigned int flash_addr;//адреса у флеш звыдки треба читати запис
    unsigned char humid;//показувати вологість
    unsigned char flag1;//ознаки
    unsigned char flag2;
    unsigned char r;//номер запису для перегляду 0-20
    int year;
    int month;
    int day;
    int day_end;//день кінцевий діапазону
    union{
        REC rec[21];//самі записи 12*21=252 ,байти
        char rec_buf[256];
        int rec_ibuf[64];// в останній записувати глобальний номер запису
    };
};
typedef struct ARCH_VIEW ARCH_VIEW;

//адреса розміщення у рамтроні структури a_mng
#define a_mng_rmtrn_addr 0x400

typedef struct{
    int point;//1- записати 0-й масив, 2- записати 1-й і скинути в 0 після запису
    //LPC_Rtc_DateTime_t DateTime;
    char name[16];//для імені файла попереднього запису
    char fname[16];//імя для запису на флешку
    unsigned short p;//вказівник
    unsigned short cntr;//лічильник який ні до чого не зобовязує
    union{
      char rec[512];//
      int reci[128];//
    };
}REC_ARR;

// для організації обміну з компютером
typedef struct{
    int *addr;//початкова адреса параметрів
    char count;//кількість підряд параметрів
}MS;//mobus slave

/* діапазон значень, якщо обидва нульові то записзаборонено і адреса параметра*/
typedef struct{
    unsigned short min;   //min. значення
    unsigned short max;   //max. значення
    int *adr;// адреса параметра
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
    int flag;//ознака того, що запис в буфер був
    int lastDate;// адреса параметра
}FOR_SD_WR;

//для тиристорного регулятора в програмованій логіці
typedef struct{
  int enable;//наявність запису
  int read;//значення для читання
  int SCR0;//значення для запису
  int SCR1;//значення для запису
  int SCR2;//значення для запису
//  int POUT;   // percent of heating power 100.00
  int circ;   // percent of circulation out 100.00
  int exhaust;// percent of exhauster out 100.00
}TYRREG;

//
typedef struct{
  int code;//adc code
  int temp;//temperature
}CvsT;

//оперативні параметри PID регулятора-------------------------------------------
typedef struct{
  int just_on;//ознака першого включення регулятора
  int change;//відбулися обчислення
  int rSp;//runtime setpoint for shockless change
  int Tq;//час - зворотній відлік пропорційна частина
  int Td;//час - зворотній відлік диференційна частина
  int Ti;//час - зворотній відлік інтегральна частина
  int out;//розрахунковий вихід  
  int last_t;//попереднє значення температури для обчислення диф. складової
  int ie;//інтегрування помилки
  int P;//пропорційна складова 
  int I;//інтегральна складова
  int D;//диференційна складова 
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
