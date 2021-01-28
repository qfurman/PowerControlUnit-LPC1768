
//для внутрішнього інтерфейсу  






char device3;
//організація обміну з аналоговими модулями
/* на кожен виділяється по два біта в задачах і запитах, аля AIU можливо три варіатни обміну -
звичайний, запис оперативних параметрів і запис постійних параметрів і запис оперативних і постійних одночасно
Для AOU - звичайний і запис одного вихідного параметру
всього задіяно 28 бітів
*/
struct{
    unsigned int task;
    unsigned int req;
    unsigned short err;//девайси що не відповідають належним чином
    unsigned short err_cntr;//лічильник помилок

}aum3;
/******************************************************************************/
//для внутрішнього інтерфейсу кінець 
//   ця частина обслуговує внутрішній RS485/RS422 інтерфейс 3
/******************************************************************************/

/******************************************************************************/
void modbus_3()
{
    char i;
    int *i_ptr;

    //опитування
    if(uart3_sts_bit.end_rx | uart3_sts_bit.timeout | uart3_sts_bit.not_respond){
        if((CRCrx3 == 0)&&(!(buf[3].exception.function & 0x80))){
            switch(device3){
              default:
                //AIU
                switch((aum3.req >> device3*2) & 0x03){
                  case 0:
                    //просто копіювати всі регістри
                    aiu[0].sts = (unsigned short)litle_endian(buf[3].read_resp.reg[0]);//aiu[Devices3[device3]-1].sts 
                    aiu[0].adc = ((unsigned short)litle_endian(buf[3].read_resp.reg[1]))<<16 |((unsigned short)litle_endian(buf[3].read_resp.reg[2]));
                    aiu[0].ADC0 = ((unsigned short)litle_endian(buf[3].read_resp.reg[3]))<<16 |((unsigned short)litle_endian(buf[3].read_resp.reg[4]));
                    aiu[0].ADC1 = ((unsigned short)litle_endian(buf[3].read_resp.reg[5]))<<16 |((unsigned short)litle_endian(buf[3].read_resp.reg[6]));
                    aiu[0].ADC2 = ((unsigned short)litle_endian(buf[3].read_resp.reg[7]))<<16 |((unsigned short)litle_endian(buf[3].read_resp.reg[8]));
                    aiu[0].ntc0 = (unsigned short)litle_endian(buf[3].read_resp.reg[9]);
                    aiu[0].ntc1 = (unsigned short)litle_endian(buf[3].read_resp.reg[10]);
                    if(((aum3.task >> device3*2) & 0x01)==0) /*щоб не затерти зміни*/ aiu[0].cmd = (unsigned short)litle_endian(buf[3].read_resp.reg[11]);
                    if(((aum3.task >> device3*2) & 0x02)==0){//щоб не затерти зміни
                      aiu[0].dacs = (unsigned short)litle_endian(buf[3].read_resp.reg[12]);
                      aiu[0].offset0 = ((unsigned short)litle_endian(buf[3].read_resp.reg[13]))<<16 |((unsigned short)litle_endian(buf[3].read_resp.reg[14]));
                      aiu[0].gain0 = ((unsigned short)litle_endian(buf[3].read_resp.reg[15]))<<16 |((unsigned short)litle_endian(buf[3].read_resp.reg[16]));
                      aiu[0].Kg0 = (unsigned short)litle_endian(buf[3].read_resp.reg[17]);
                      aiu[0].K0 = (unsigned short)litle_endian(buf[3].read_resp.reg[18]);
                      aiu[0].offset1 = ((unsigned short)litle_endian(buf[3].read_resp.reg[19]))<<16 |((unsigned short)litle_endian(buf[3].read_resp.reg[20]));
                      aiu[0].gain1 = ((unsigned short)litle_endian(buf[3].read_resp.reg[21]))<<16 |((unsigned short)litle_endian(buf[3].read_resp.reg[22]));
                      aiu[0].Kg1 = (unsigned short)litle_endian(buf[3].read_resp.reg[23]);
                      aiu[0].K1 = (unsigned short)litle_endian(buf[3].read_resp.reg[24]);
                      aiu[0].offset2 = ((unsigned short)litle_endian(buf[3].read_resp.reg[25]))<<16 |((unsigned short)litle_endian(buf[3].read_resp.reg[26]));
                      aiu[0].gain2 = ((unsigned short)litle_endian(buf[3].read_resp.reg[27]))<<16 |((unsigned short)litle_endian(buf[3].read_resp.reg[28]));
                      aiu[0].Kg2 = (unsigned short)litle_endian(buf[3].read_resp.reg[29]);
                      aiu[0].K2 = (unsigned short)litle_endian(buf[3].read_resp.reg[30]);
                      aiu[0].ntcOffset0 = (unsigned short)litle_endian(buf[3].read_resp.reg[31]);
                      aiu[0].ntcGain0 = (unsigned short)litle_endian(buf[3].read_resp.reg[32]);
                      aiu[0].ntcOffset1 = (unsigned short)litle_endian(buf[3].read_resp.reg[33]);
                      aiu[0].ntcGain1 = (unsigned short)litle_endian(buf[3].read_resp.reg[34]);
                    }
                    break;
                  default:
                    // писати в RAM, FLASH або в обидва одночасно
                    aum3.task ^= aum3.req & (0x3 <<device3*2);//зняти із задач виконану
                    aum3.req ^= aum3.req & (0x3 <<device3*2);//із запитів  виконану
                    break;
                }
                break;              
            }
            aum3.err &= ~(1<<device3);//зняти ознаку помилки прийому
            aum3.err_cntr = 0;
            device3++;
        }else{
            //помилка прийому або девайс повідомив про помилку
            if(++aum3.err_cntr > 0){
                aum3.err_cntr = 0;
                aum3.err |= 1<<device3;
                aum3.task = aum3.task & ~(0x3 <<device3*2);//зняти із задач
                aum3.req = aum3.req & ~(0x3 <<device3*2);//із запитів нагло зняти всі
                device3++;
            }
        }

        if(device3 >=sizeof(Devices3)/sizeof(int))device3 = 0;

        //------------------------ відправлення запитів ------------------------------
        switch(device3){
          default:
            //AIU
            switch((aum3.task >> device3*2) & 0x03){
              case 0:
                //просто читати всі регістри
                buf[3].read_req.address = Devices3[device3];
                buf[3].read_req.function = 0x03;//регістрів
                buf[3].read_req.start_address = big_endian(0x0000);
                buf[3].read_req.quantity = big_endian(35);
                wr_uart3((char*)&buf[3].modbuf[0],6,&buf[3].modbuf[0],256);
                break;

              case 1://записати оперативні параметри
                aum3.req |= aum3.task & (3<<device3*2);//поставити ознаку відправки запиту
                buf[3].write_mult_req.address = device3+1;
                buf[3].write_mult_req.function = 0x10;//запис багатьох регістрів
                buf[3].write_mult_req.start_address = big_endian(11);
                buf[3].write_mult_req.quantity = big_endian(1);
                buf[3].write_mult_req.byte_count = 1*2;

                buf[3].write_mult_req.reg[0] = big_endian(aiu[device3].cmd);
                wr_uart3((char*)&buf[3].modbuf[0],9,&buf[0].modbuf[0],256);
                break;

              case 2://записати постійні параметри
                aum3.req |= aum3.task & (3<<device3*2);//поставити ознаку відправки запиту
                buf[3].write_mult_req.address = device3+1;
                buf[3].write_mult_req.function = 0x10;//запис багатьох регістрів
                buf[3].write_mult_req.start_address = big_endian(12);
                buf[3].write_mult_req.quantity = big_endian(23);
                buf[3].write_mult_req.byte_count = 23*2;
                
                buf[3].write_mult_req.reg[0] = big_endian(aiu[0].dacs);// = (unsigned short)litle_endian(buf[3].read_resp.reg[12]);
                buf[3].write_mult_req.reg[1] = big_endian(aiu[0].offset0>>16); buf[3].write_mult_req.reg[2] = big_endian(aiu[0].offset0);// = ((unsigned short)litle_endian(buf[3].read_resp.reg[13]))<<16 |((unsigned short)litle_endian(buf[3].read_resp.reg[14]));
                buf[3].write_mult_req.reg[3] = big_endian(aiu[0].gain0>>16); buf[3].write_mult_req.reg[4] = big_endian(aiu[0].gain0);// = ((unsigned short)litle_endian(buf[3].read_resp.reg[15]))<<16 |((unsigned short)litle_endian(buf[3].read_resp.reg[16]));
                buf[3].write_mult_req.reg[5] = big_endian(aiu[0].Kg0);// = (unsigned short)litle_endian(buf[3].read_resp.reg[17]);
                buf[3].write_mult_req.reg[6] = big_endian(aiu[0].K0);// = (unsigned short)litle_endian(buf[3].read_resp.reg[18]);
                buf[3].write_mult_req.reg[7] = big_endian(aiu[0].offset1>>16); buf[3].write_mult_req.reg[8] = big_endian(aiu[0].offset1);// = ((unsigned short)litle_endian(buf[3].read_resp.reg[19]))<<16 |((unsigned short)litle_endian(buf[3].read_resp.reg[20]));
                buf[3].write_mult_req.reg[9] = big_endian(aiu[0].gain1>>16); buf[3].write_mult_req.reg[10] = big_endian(aiu[0].gain1);// = ((unsigned short)litle_endian(buf[3].read_resp.reg[21]))<<16 |((unsigned short)litle_endian(buf[3].read_resp.reg[22]));
                buf[3].write_mult_req.reg[11] = big_endian(aiu[0].Kg1);// = (unsigned short)litle_endian(buf[3].read_resp.reg[23]);
                buf[3].write_mult_req.reg[12] = big_endian(aiu[0].K1);// = (unsigned short)litle_endian(buf[3].read_resp.reg[24]);
                buf[3].write_mult_req.reg[13] = big_endian(aiu[0].offset2>>16);buf[3].write_mult_req.reg[14] = big_endian(aiu[0].offset2);//= ((unsigned short)litle_endian(buf[3].read_resp.reg[25]))<<16 |((unsigned short)litle_endian(buf[3].read_resp.reg[26]));
                buf[3].write_mult_req.reg[15] = big_endian(aiu[0].gain2>>16);buf[3].write_mult_req.reg[16] = big_endian(aiu[0].gain2);// = ((unsigned short)litle_endian(buf[3].read_resp.reg[27]))<<16 |((unsigned short)litle_endian(buf[3].read_resp.reg[28]));
                buf[3].write_mult_req.reg[17] = big_endian(aiu[0].Kg2);// = (unsigned short)litle_endian(buf[3].read_resp.reg[29]);
                buf[3].write_mult_req.reg[18] = big_endian(aiu[0].K2);// = (unsigned short)litle_endian(buf[3].read_resp.reg[30]);
                buf[3].write_mult_req.reg[19] = big_endian(aiu[0].ntcOffset0);// = (unsigned short)litle_endian(buf[3].read_resp.reg[31]);
                buf[3].write_mult_req.reg[20] = big_endian(aiu[0].ntcGain0);// = (unsigned short)litle_endian(buf[3].read_resp.reg[32]);
                buf[3].write_mult_req.reg[21] = big_endian(aiu[0].ntcOffset1);// = (unsigned short)litle_endian(buf[3].read_resp.reg[33]);
                buf[3].write_mult_req.reg[22] = big_endian(aiu[0].ntcGain1);// = (unsigned short)litle_endian(buf[3].read_resp.reg[34]);
                
                /*
                i_ptr = (int*)&aiu[device3].dacs;
                for(i=0;i<23;i++){
                    buf[3].write_mult_req.reg[i] = big_endian((unsigned short)(*i_ptr));
                    *i_ptr++;
                }
                */
                wr_uart3((char*)&buf[3].modbuf[0],53,&buf[3].modbuf[0],256);//7+data
                break;

              case 3://записати всі одночасно
                aum3.req |= aum3.task & (3<<device3*2);//поставити ознаку відправки запиту
                buf[3].write_mult_req.address = device3+1;
                buf[3].write_mult_req.function = 0x10;//запис багатьох регістрів
                buf[3].write_mult_req.start_address = big_endian(7);
                buf[3].write_mult_req.quantity = big_endian(24);
                buf[3].write_mult_req.byte_count = 24*2;

                i_ptr = (int*)&aiu[device3];
                for(i=0;i<4;i++){
                    buf[3].write_mult_req.reg[i] = big_endian(*i_ptr);
                    *i_ptr++;
                }
                for(i=4;i<5;i++){
                    buf[3].write_mult_req.reg[i] = big_endian(*i_ptr);
                    *i_ptr++;
                }
                for(i=5;i<9;i=i+2){
                    buf[3].write_mult_req.reg[i] = big_endian(*i_ptr>>16);
                    buf[3].write_mult_req.reg[i+1] = big_endian((unsigned short)(*i_ptr)>>16);
                    *i_ptr++;
                }
                for(i=9;i<11;i++){
                    buf[3].write_mult_req.reg[i] = big_endian(*i_ptr);
                    *i_ptr++;
                }

                wr_uart3((char*)&buf[3].modbuf[0],29,&buf[3].modbuf[0],256);
                break;
            }
            break;

        }

    }//кінець
}

/******************************************************************************/
void modbus_uart3_init(void)
{
    uart3_rd_manag[0].ptr = &buf[3].modbuf[0];     //не зберігати байти
    uart3_rd_manag[0].count = 256;       //1байт-команда, 2байти-адреса
    uart3_rd_manag[0].next = 0;                 //продовження

    uart3_rd_ptr = &uart3_rd_manag[0];          //ініціалізація поінтера читання
    CRCtx3_init();
    CRCrx3_init();
    uart3_sts = 0;
    
}
/******************************************************************************/






