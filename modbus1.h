
//��� ����������� ���������� --------------------------------






char device;
//���������� ����� � ����������� ��������
/* �� ����� ���������� �� ��� ��� � ������� � �������, ��� AIU ������� ��� ������� ����� -
���������, ����� ����������� ��������� � ����� �������� ��������� � ����� ����������� � �������� ���������
��� AOU - ��������� � ����� ������ ��������� ���������
������ ������ 28 ���
*/
struct{
    unsigned int task;
    unsigned int req;
    unsigned short err;//������� �� �� ���������� �������� �����
    unsigned short err_cntr;//�������� �������

}aum;
/******************************************************************************/
//��� ����������� ���������� ����� --------------------------------
//   �� ������� ��������� �������� RS485/RS422 ��������� 1
/******************************************************************************/

/******************************************************************************/
void modbus_1()
{
    char i;
    int *i_ptr;

    //INTCLEAR_bit.SOFTINTCLR = 1;//����� �����������
    //����������
    if(uart1_sts_bit.end_rx | uart1_sts_bit.timeout | uart1_sts_bit.not_respond){
        if((CRCrx1 == 0)&&(!(buf[1].exception.function & 0x80))){
            switch(device){
              default:
                //AIU
                switch((aum.req >> device*2) & 0x03){
                  case 0:
                    //������ �������� �� �������
                    i = 0;
                    i_ptr = (int*)&aiu[Devices1[device]-1].sts;

                    for(i=0;i<82;i++)
                        *i_ptr++ = (unsigned short)litle_endian(buf[1].read_resp.reg[i]);//����� ���������� �������
                    //cmd
                    if(((aum.task >> device*2) & 0x01)==0){//��� �� ������� ����
                      i=82; 
                      *i_ptr++ = (unsigned short)litle_endian(buf[1].read_resp.reg[i]);//�������
                      i++;
                    }
                    i_ptr = (int*)&aiu[Devices1[device]-1].offset;//��������� ������ �� ��������� ����� ���� �� ��������
                    if(((aum.task >> device*2) & 0x02)==0){
                      //��� �� ������� ����
                        for(i=83;i<104;i++)
                            *i_ptr++ = (unsigned short)litle_endian(buf[1].read_resp.reg[i]);//����� ���������� �������
                    }
                    break;
                  default:
                    // ������ � RAM, FLASH ��� � ������ ���������
                    aum.task ^= aum.req & (0x3 <<device*2);//����� �� ����� ��������
                    aum.req ^= aum.req & (0x3 <<device*2);//�� ������  ��������
                    break;

                }
                break;              
            }
            aum.err &= ~(1<<device);//����� ������ ������� �������
            aum.err_cntr = 0;
            device++;
        }else{
            //������� ������� ��� ������ �������� ��� �������
            if(++aum.err_cntr > 2){
                aum.err_cntr = 0;
                aum.err |= 1<<device;
                aum.task = aum.task & ~(0x3 <<device*2);//����� �� �����
                aum.req = aum.req & ~(0x3 <<device*2);//�� ������ ����� ����� ��
                device++;
            }
        }

        if(device >= sizeof(Devices1)/sizeof(int))device = 0;

        //------------------------ ����������� ������ ------------------------------
        switch(device){
          default:
            //AIU
            switch((aum.task >> device*2) & 0x03){
              case 0:
                //������ ������ �� �������
                buf[1].read_req.address = Devices1[device];
                buf[1].read_req.function = 0x03;//�������
                buf[1].read_req.start_address = big_endian(0x0000);
                buf[1].read_req.quantity = big_endian(104);
                wr_uart1((char*)&buf[1].modbuf[0],6,&buf[1].modbuf[0],256);
                break;

              case 1://�������� ��������� ���������
                aum.req |= aum.task & (3<<device*2);//��������� ������ �������� ������
                buf[1].write_mult_req.address = device+1;
                buf[1].write_mult_req.function = 0x10;//����� �������� �������
                buf[1].write_mult_req.start_address = big_endian(82);
                buf[1].write_mult_req.quantity = big_endian(1);
                buf[1].write_mult_req.byte_count = 1*2;

                buf[1].write_mult_req.reg[0] = big_endian(aiu[device].cmd);
                wr_uart1((char*)&buf[1].modbuf[0],9,&buf[0].modbuf[0],256);
                break;

              case 2://�������� ������ ���������
                aum.req |= aum.task & (3<<device*2);//��������� ������ �������� ������
                buf[1].write_mult_req.address = device+1;
                buf[1].write_mult_req.function = 0x10;//����� �������� �������
                buf[1].write_mult_req.start_address = big_endian(83);
                buf[1].write_mult_req.quantity = big_endian(19);
                buf[1].write_mult_req.byte_count = 19*2;
                
                i_ptr = (int*)&aiu[device].offset;
                for(i=0;i<19;i++){
                    buf[1].write_mult_req.reg[i] = big_endian((unsigned short)(*i_ptr));
                    *i_ptr++;
                }
                wr_uart1((char*)&buf[1].modbuf[0],46,&buf[1].modbuf[0],256);
                break;

              case 3://�������� �� ���������
                aum.req |= aum.task & (3<<device*2);//��������� ������ �������� ������
                buf[1].write_mult_req.address = device+1;
                buf[1].write_mult_req.function = 0x10;//����� �������� �������
                buf[1].write_mult_req.start_address = big_endian(7);
                buf[1].write_mult_req.quantity = big_endian(12);
                buf[1].write_mult_req.byte_count = 12*2;

                i_ptr = (int*)&aiu[device];
                for(i=0;i<4;i++){
                    buf[1].write_mult_req.reg[i] = big_endian(*i_ptr);
                    *i_ptr++;
                }
                for(i=4;i<5;i++){
                    buf[1].write_mult_req.reg[i] = big_endian(*i_ptr);
                    *i_ptr++;
                }
                for(i=5;i<9;i=i+2){
                    buf[1].write_mult_req.reg[i] = big_endian(*i_ptr>>16);
                    buf[1].write_mult_req.reg[i+1] = big_endian((unsigned short)(*i_ptr)>>16);
                    *i_ptr++;
                }
                for(i=9;i<11;i++){
                    buf[1].write_mult_req.reg[i] = big_endian(*i_ptr);
                    *i_ptr++;
                }

                wr_uart1((char*)&buf[1].modbuf[0],29,&buf[1].modbuf[0],256);
                break;
            }
            break;

        }

    }//�����
}

/******************************************************************************/
void modbus_uart1_init(void)
{
    uart1_rd_manag[0].ptr = &buf[1].modbuf[0];     //�� �������� �����
    uart1_rd_manag[0].count = 256;       //1����-�������, 2�����-������
    uart1_rd_manag[0].next = 0;                 //�����������

    uart1_rd_ptr = &uart1_rd_manag[0];          //����������� ������� �������
    CRCtx1_init();
    CRCrx1_init();
    uart1_sts = 0;
    
}
/******************************************************************************/






