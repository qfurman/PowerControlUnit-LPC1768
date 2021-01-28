
//��� ����������� ����������  






char device2;
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

}aum2;
/******************************************************************************/
//��� ����������� ���������� ����� 
//   �� ������� ��������� �������� RS485/RS422 ��������� 2
/******************************************************************************/

/******************************************************************************/
void modbus_2()
{
    char i;
    int *i_ptr;


    //����������
    if(uart2_sts_bit.end_rx | uart2_sts_bit.timeout | uart2_sts_bit.not_respond){
        if((CRCrx2 == 0)&&(!(buf[2].exception.function & 0x80))){
            switch(device2){
              default:
                //AIU
                switch((aum2.req >> device2*2) & 0x03){
                  case 0:
                    //������ �������� �� �������
                    i = 0;
                    i_ptr = (int*)&aiu[Devices2[device2]-1].sts;

                    for(i=0;i<82;i++)
                        *i_ptr++ = (unsigned short)litle_endian(buf[2].read_resp.reg[i]);//����� ���������� �������
                    //cmd
                    if(((aum2.task >> device2*2) & 0x01)==0){//��� �� ������� ����
                      i=82; 
                      *i_ptr++ = (unsigned short)litle_endian(buf[2].read_resp.reg[i]);//�������
                      i++;
                    }
                    i_ptr = (int*)&aiu[Devices2[device2]-1].offset;//��������� ������ �� ��������� ����� ���� �� ��������
                    if(((aum2.task >> device2*2) & 0x02)==0){
                      //��� �� ������� ����
                        for(i=83;i<104;i++)
                            *i_ptr++ = (unsigned short)litle_endian(buf[2].read_resp.reg[i]);//����� ���������� �������
                    }
                    break;
                  default:
                    // ������ � RAM, FLASH ��� � ������ ���������
                    aum2.task ^= aum2.req & (0x3 <<device2*2);//����� �� ����� ��������
                    aum2.req ^= aum2.req & (0x3 <<device2*2);//�� ������  ��������
                    break;

                }
                break;              
            }
            aum2.err &= ~(1<<device2);//����� ������ ������� �������
            aum2.err_cntr = 0;
            device2++;
        }else{
            //������� ������� ��� ������ �������� ��� �������
            if(++aum2.err_cntr > 2){
                aum2.err_cntr = 0;
                aum2.err |= 1<<device2;
                aum2.task = aum2.task & ~(0x3 <<device2*2);//����� �� �����
                aum2.req = aum2.req & ~(0x3 <<device2*2);//�� ������ ����� ����� ��
                device2++;
            }
        }

        if(device2 >= sizeof(Devices2)/sizeof(int))device2 = 0;

        //------------------------ ����������� ������ ------------------------------
        switch(device2){
          default:
            //AIU
            switch((aum2.task >> device2*2) & 0x03){
              case 0:
                //������ ������ �� �������
                buf[2].read_req.address = Devices2[device2];
                buf[2].read_req.function = 0x03;//�������
                buf[2].read_req.start_address = big_endian(0x0000);
                buf[2].read_req.quantity = big_endian(104);
                wr_uart2((char*)&buf[2].modbuf[0],6,&buf[2].modbuf[0],256);
                break;

              case 1://�������� ��������� ���������
                aum2.req |= aum2.task & (3<<device2*2);//��������� ������ �������� ������
                buf[2].write_mult_req.address = device2+1;
                buf[2].write_mult_req.function = 0x10;//����� �������� �������
                buf[2].write_mult_req.start_address = big_endian(82);
                buf[2].write_mult_req.quantity = big_endian(1);
                buf[2].write_mult_req.byte_count = 1*2;

                buf[2].write_mult_req.reg[0] = big_endian(aiu[device2].cmd);
                wr_uart2((char*)&buf[2].modbuf[0],9,&buf[0].modbuf[0],256);
                break;

              case 2://�������� ������ ���������
                aum2.req |= aum2.task & (3<<device2*2);//��������� ������ �������� ������
                buf[2].write_mult_req.address = device2+1;
                buf[2].write_mult_req.function = 0x10;//����� �������� �������
                buf[2].write_mult_req.start_address = big_endian(83);
                buf[2].write_mult_req.quantity = big_endian(19);
                buf[2].write_mult_req.byte_count = 19*2;
                
                i_ptr = (int*)&aiu[device2].offset;
                for(i=0;i<19;i++){
                    buf[2].write_mult_req.reg[i] = big_endian((unsigned short)(*i_ptr));
                    *i_ptr++;
                }
                wr_uart2((char*)&buf[2].modbuf[0],46,&buf[2].modbuf[0],256);
                break;

              case 3://�������� �� ���������
                aum2.req |= aum2.task & (3<<device2*2);//��������� ������ �������� ������
                buf[2].write_mult_req.address = device2+1;
                buf[2].write_mult_req.function = 0x10;//����� �������� �������
                buf[2].write_mult_req.start_address = big_endian(7);
                buf[2].write_mult_req.quantity = big_endian(12);
                buf[2].write_mult_req.byte_count = 12*2;

                i_ptr = (int*)&aiu[device2];
                for(i=0;i<4;i++){
                    buf[2].write_mult_req.reg[i] = big_endian(*i_ptr);
                    *i_ptr++;
                }
                for(i=4;i<5;i++){
                    buf[2].write_mult_req.reg[i] = big_endian(*i_ptr);
                    *i_ptr++;
                }
                for(i=5;i<9;i=i+2){
                    buf[2].write_mult_req.reg[i] = big_endian(*i_ptr>>16);
                    buf[2].write_mult_req.reg[i+1] = big_endian((unsigned short)(*i_ptr)>>16);
                    *i_ptr++;
                }
                for(i=9;i<11;i++){
                    buf[2].write_mult_req.reg[i] = big_endian(*i_ptr);
                    *i_ptr++;
                }

                wr_uart2((char*)&buf[2].modbuf[0],29,&buf[2].modbuf[0],256);
                break;
            }
            break;

        }

    }//�����
}

/******************************************************************************/
void modbus_uart2_init(void)
{
    uart2_rd_manag[0].ptr = &buf[2].modbuf[0];     //�� �������� �����
    uart2_rd_manag[0].count = 256;       //1����-�������, 2�����-������
    uart2_rd_manag[0].next = 0;                 //�����������

    uart2_rd_ptr = &uart2_rd_manag[0];          //����������� ������� �������
    CRCtx2_init();
    CRCrx2_init();
    uart2_sts = 0;
    
}
/******************************************************************************/






