

#define modbufsize 256

extern union{
    char modbuf[modbufsize];

    struct{
        char address;
        char function;
        char exeption;
    }exception;//повідомлення про виключну ситуацію

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
    }read_req;//читання Запит

    struct{
        char address;
        char function;
        char byte_count;
    }read_resp;//читання відповідь

    struct{
        char address;
        char function;
        unsigned short register_address;
        unsigned short register_value;
    }write_req;//запис одного регістра Запит

    struct{
        char address;
        char function;
        unsigned short register_address;
        unsigned short register_value;
    }write_resp;//запис одного регістра відповідь

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
        unsigned char byte_count;
        unsigned short first;
    }write_mult_req;//запис одного регістра Запит

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
    }write_mult_resp;//запис одного регістра відповідь

}buf;




extern MS ms;//для збереження у фоновій програмі даних отриманих по інтерфейсі

extern struct{
    unsigned int task;
    unsigned int req;
    unsigned short err;//девайси що не відповідають належним чином
    unsigned short err_cntr;//лічильник помилок

}aum;

extern struct{
    unsigned int task;
    unsigned int req;
    unsigned short err;//девайси що не відповідають належним чином
    unsigned short err_cntr;//лічильник помилок

}aum2;

extern struct{
    unsigned int task;
    unsigned int req;
    unsigned short err;//девайси що не відповідають належним чином
    unsigned short err_cntr;//лічильник помилок

}aum3;

//температури
extern union{
  short tcelsius[2][2];
  short TCelsius[2*2];
};


void modbus_uart0_init(void);
void modbus_uart1_init(void);
void modbus_uart2_init(void);
void modbus_uart3_init(void);

//void slave();
void modbus_receive_init();

void transmit_arch(char *src, int count);

void modbus_0();
void modbus_1();
void modbus_2();
void modbus_3();

extern int mregs[4];
