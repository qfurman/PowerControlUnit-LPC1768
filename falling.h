    
    //������� ����� �������+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    if(rpm_flag ==0){
      //������� �������������
      cap.arr[0] = RICOUNTER;//������ �����
      if(N.imp_per_rotate == 7){
        cap.lrt = cap.arr[0];//��������� ��� ������
        // ���������� ������
        cap.fr_i = cap.fr_i % 7;

        if(cap.fr_i==0){
          cap.T = cap.arr[0] - cap.fr_buf[0] ;//��� ������ ������
          cap.rpm = 60000000 / ( (long long)cap.T * 10000/1000000 );//������ �� ��.
        }
        cap.fr_buf[cap.fr_i] = cap.arr[0];
        cap.fr_d[cap.fr_i] = cap.fr_buf[cap.fr_i] - cap.fr_buf[ ((cap.fr_i+6)%7) ];
        cap.fr_i++;
      }else{
        //� ������� ������ � ����� ������
        if(N.imp_per_rotate == 1){
          cap.lrt = cap.arr[0];//��������� ��� ������
          // ���������� ������
          cap.T = cap.arr[0] - cap.fr_buf[0] ;//��� ������ ������
          cap.rpm = 60000000 / ( (long long)cap.T * 10000/1000000 );//������ �� ��.
          cap.fr_d[0] = cap.arr[0] - cap.fr_buf[0];
          cap.fr_buf[0] = cap.arr[0];
        }
      }
    }else{
      //------------------������� ������� ������---------------------------------
      //�������� � ���������
      int i;
      i = RICOUNTER - last_RicT;
      if(i < 0) i = -i;
      if(i > 2000){
        //���� �������� �� ���������� ����� ��c��
        last_RicT = RICOUNTER;

        local_tacho1++;//��� ������� �������� ���������

        if(++local_tacho >= N.imp_per_rotate){
          local_tacho = 0;
          t_rot_pre = t_rot;// ��������� ������ ���� ��� ���������� ���� ������
          t_rot = RICOUNTER;// ������ ���� � ������� ������
          TachoI = (TachoI+1) & 0xf;
          TachoBuf[TachoI] = t_rot - t_rot_pre;
        }
      }

      //------------------������� ������� ������ �����--------------------------
    }
    //������� ����� �����+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
    