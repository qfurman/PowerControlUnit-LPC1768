

#define modbufsize 256

extern union{
    char modbuf[modbufsize];

    struct{
        char address;
        char function;
        char exeption;
    }exception;//����������� ��� �������� ��������

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
    }read_req;//������� �����

    struct{
        char address;
        char function;
        char byte_count;
    }read_resp;//������� �������

    struct{
        char address;
        char function;
        unsigned short register_address;
        unsigned short register_value;
    }write_req;//����� ������ ������� �����

    struct{
        char address;
        char function;
        unsigned short register_address;
        unsigned short register_value;
    }write_resp;//����� ������ ������� �������

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
        unsigned char byte_count;
        unsigned short first;
    }write_mult_req;//����� ������ ������� �����

    struct{
        char address;
        char function;
        unsigned short start_address;
        unsigned short quantity;
    }write_mult_resp;//����� ������ ������� �������

}buf;




extern MS ms;//��� ���������� � ������ ������� ����� ��������� �� ���������

extern struct{
    unsigned int task;
    unsigned int req;
    unsigned short err;//������� �� �� ���������� �������� �����
    unsigned short err_cntr;//�������� �������

}aum;

extern struct{
    unsigned int task;
    unsigned int req;
    unsigned short err;//������� �� �� ���������� �������� �����
    unsigned short err_cntr;//�������� �������

}aum2;

extern struct{
    unsigned int task;
    unsigned int req;
    unsigned short err;//������� �� �� ���������� �������� �����
    unsigned short err_cntr;//�������� �������

}aum3;

//�����������
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
