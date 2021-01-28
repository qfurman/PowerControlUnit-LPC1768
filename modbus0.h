/******************************************************************************/
void modbus_uart0_init(void)
{
    uart0_rd_manag[0].ptr = &buf[0].modbuf[0];  //�� �������� �����
    uart0_rd_manag[0].count = 256;              //1����-�������, 2�����-������
    uart0_rd_manag[0].next = 0;                 //�����������

    uart0_rd_ptr = &uart0_rd_manag[0];          //����������� ������� �������
    CRCtx0_init();
    CRCrx0_init();
    uart0_sts = 0x07;//����� ������
}

/******************************************************************************/
void modbus_receive_init()
{
    uart0_rd_manag[0].ptr = &buf[0].modbuf[0];  //�������� �����
    uart0_rd_manag[0].count = 256;              //�������
    uart0_rd_manag[0].next = 0;                 //����������� ����
    uart0_rd_ptr = &uart0_rd_manag[0];          //����������� ������� �������
    CRCrx0_init();
}
/******************************************************************************/
/******************************************************************************/
/*----------------------------------------------------------------------------*/


/******************************************************************************/
/******************************************************************************/
//Slave
void modbus_0()
{
  unsigned char i;
  int address;
  int quantity;
  int off;
  
  //�������� ���������� ����
  //�������� ������
  if (CRCrx0 == 0 && (buf[0].modbuf[0] == 0 || buf[0].modbuf[0] == N.UART0.simplex) ){ 
    switch (buf[0].modbuf[1]){	//function code
    case 3:                     //������� ������ ��� ����� ������� (�� ������� ������� � ������� RAM)
      if(litle_endian(buf[0].read_req.quantity) > 125){
        buf[0].exception.exeption = 3; //�������� �������
        error();
        break;
      }
      // ���������� ������� 16-����� ������� �������
      quantity = litle_endian(buf[0].read_req.start_address);
      quantity += litle_endian(buf[0].read_req.quantity);
      //if(quantity > sizeof(ALL_T.Ts)/sizeof(short)){
      if(quantity > sizeof(mma)/sizeof(MMA)){
        buf[0].exception.exeption = 2; //������� ������ ��� ��������
        error();
        break;
      }
      
      // ��������� ������
      
      //i_ptr = (int*)m_adr[litle_endian(buf[0].read_req.start_address)];
      //s_ptr = (short*)&buf[0].read_resp.reg[0];
      address = litle_endian(buf[0].read_req.start_address);//������
      quantity = litle_endian(buf[0].read_req.quantity);
      buf[0].read_resp.byte_count = litle_endian(buf[0].read_req.quantity)*2;
      
      //��������� ��� �� ��������
      for(i=0;i<quantity;i++){
        buf[0].read_resp.reg[i] = big_endian(*mma[address].adr);
        address++;
      }
      
      transmit_prepare(3+buf[0].read_resp.byte_count);
      start_transmit();
      break;
      
    case 6:
      // ���������� ������� 16-����� ������� �������
      address = litle_endian(buf[0].write_req.register_address);
      if(address >= sizeof(mma)/sizeof(MMA)){
        buf[0].exception.exeption = 2; //������� ������ ��� ��������
        error();
        break;
      }
      //����²��� �������� �������
      if(mma[address].min ==0 && mma[address].max == 0){
        buf[0].exception.exeption = 2; //������� ������ ��� ��������
        error();
        return;
      }
      if((litle_endian(buf[0].write_req.register_value)) < mma[address].min || (litle_endian(buf[0].write_req.register_value)) > mma[address].max){
        buf[0].exception.exeption = 3; //������� ��������
        error();
        return;
      }
      //���������� ��������� ������ � ������� ��� ���� ��� � ��� ��������
      ms.addr = mma[address].adr;
      ms.count = 1;
      *mma[address].adr = litle_endian(buf[0].write_req.register_value);
      
      
      transmit_prepare(4+2);
      start_transmit();
      break;
      
    case 16:                     //����� ������ ��� ����� �������
      if(litle_endian(buf[0].write_mult_req.quantity) > sizeof(mma)/sizeof(MMA)){
        buf[0].exception.exeption = 3; //�������� �������
        error();
        break;
      }
      // ���������� ������� 16-����� ������� �������
      quantity = litle_endian(buf[0].write_mult_req.start_address);
      quantity += litle_endian(buf[0].write_mult_req.quantity);
      if(quantity > sizeof(mma)/sizeof(MMA)){
        buf[0].exception.exeption = 2; //������� ������ ��� ��������
        error();
        break;
      }
      
      //����²��� �������� �������
      off = 0;//��� ���� �� ������ �������
      address = litle_endian(buf[0].write_mult_req.start_address);//������
      quantity = litle_endian(buf[0].write_mult_req.quantity);
      
      for(i=address;i<address + quantity;i++){
        if(mma[i].min ==0 && mma[i].max == 0){
          buf[0].exception.exeption = 2; //������� ������ ��� ��������
          error();
          return;
        }
        if((litle_endian(buf[0].write_mult_req.reg[off])) < mma[i].min || (litle_endian(buf[0].write_mult_req.reg[off])) > mma[i].max){
          buf[0].exception.exeption = 3; //������� ��������
          error();
          return;
        }
        off++;
      }
      //���������� ��������� ������ � ������� ��� ���� ��� � ��� ��������
      ms.addr = mma[address].adr;
      ms.count = quantity;
      //���������
      off = 0;
      for(i=address;i<address + quantity;i++){
        *mma[i].adr = litle_endian(buf[0].write_mult_req.reg[off]);
        off++;
      }
      
      
      transmit_prepare(4+2);
      start_transmit();
      break;
      
    default:
      /* ������� �� ����������� */
      buf[0].exception.exeption = 1;
      error();
      break;
    }
    //�������� ������ �� 3,5�������
  }
  else{
    //����������� ��������� ������� ���� �������
    U0FCR_bit.TFR = 1;//TX FIFO RESET
    U0FCR_bit.RFR = 1;//RX FIFO RESET
    modbus_uart0_init();
  }
  
}
/******************************************************************************/
void error()
{
  buf[0].exception.function |= 0x80;
  transmit_prepare(3);
  start_transmit();
}
/******************************************************************************/
void transmit_prepare(char count)
{
    uart0_wr_crc.ptr = &crc_buf0[0];            //�� ������� ��� ����������� ���������� ����
    uart0_wr_crc.count = 2;                     //�������������  � ���������� ���� ���������� ���
    uart0_wr_crc.next = 0;

    uart0_wr_manag[0].ptr = &buf[0].modbuf[0];         //�������
    uart0_wr_manag[0].count = count;            //������� �����
    uart0_wr_manag[0].next = 0;                 //����� ����
    uart0_wr_ptr = &uart0_wr_manag[0];          //����������� ������� ������

    uart0_rd_manag[0].ptr = 0;     //�������� �����
    uart0_rd_manag[0].count = 0;              //�������
    uart0_rd_manag[0].next = 0;                 //����������� ����
    uart0_rd_ptr = &uart0_rd_manag[0];          //����������� ������� �������
    CRCtx0_init();
    CRCrx0_init();
}
/******************************************************************************/
void start_transmit()
{
    uart0_sts = 0x00;        //���� ������� ������� � �������� start_tx = 1 start_rx = 1;
    uart0_sts_bit.pre_time = 1; //������ ������� ����� ������ ���������
    timer_uart[0] = N.UART0.timeout*4;//����� �� �������� � ���������

}
/******************************************************************************/

/******************************************************************************/

