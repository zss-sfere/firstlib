#include "User_Config.h"

const u16 FRAMAJUSTADDR[2] =
{
	0x0000, 0x0400,
}; 
const u16 MAXVALUEADDR[2][2]=
{
	0x0220, 0x0252, 
	0x0620, 0x0652, 
};

const u16 MAXDEMANDADDR[2][1]=
{
	0x0284, 
	0x0684, 
};

//
void FramErrorLCD(void)
{
	Printf816(16, 8,"Fram Err",0);
	Printf816(16, 11,"Press LEFT + ENTER Key",0);	
	DisplayUpdate();
}

void InitFram()
{
	u8 *ptr_tmp, i,j, IICIIC, keytemp, readnum;
	s16 *p16;
	u32 *p32;
	
	//Rn7302_1_2_3У׼ϵ��
	ptr_tmp = (u8 *)&(AdjustData.UGain[0]); 
	IICIIC = 1;
	while(IICIIC)
	{
		if(SPI3Gets(FRAMAJUSTADDR[0],42,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets(FRAMAJUSTADDR[1],42,ptr_tmp)) IICIIC = 0;
		else	//ȱʡ����
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				p16 = &(AdjustData.UGain[0]);  
				for(i=0; i<21; i++) {*p16++ = 0x00;}
				SPI3Puts(FRAMAJUSTADDR[0],42,ptr_tmp);
				SPI3Puts(FRAMAJUSTADDR[1],42,ptr_tmp);
				IICIIC = 0;			
			}
			else FramErrorLCD();	//��ʾ����ֱ����λ	
		}
	}	
	
	//�������ݶ�ȡ, �˴����Խ�ʡ������
	ptr_tmp = (u8 *)(&MeterData.EN[0]); //˫�����
	IICIIC = 1;
	while(IICIIC)
	{
		if(SPI3Gets(0x002c,20,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets(0x042c,20,ptr_tmp)) IICIIC = 0;	
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{ 
				for(i=0; i<5; i++) {MeterData.EN[i] = 0x00;}
				SPI3Puts(0x002c,20,ptr_tmp);
				SPI3Puts(0x042c,20,ptr_tmp);
				IICIIC = 0;			
			}
			else FramErrorLCD();	//��ʾ����ֱ����λ	
		}		
	}
	//�������޹�����
	ptr_tmp = (u8 *)(&MeterData.VarhFour[0]); 
	IICIIC = 1;
	while(IICIIC)
	{
		if(SPI3Gets(0x0042,16,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets(0x0442,16,ptr_tmp)) IICIIC = 0;	
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{ 
				for(i=0; i<4; i++) {MeterData.VarhFour[i] = 0x00;}
				SPI3Puts(0x0042,16,ptr_tmp);
				SPI3Puts(0x0442,16,ptr_tmp);
				IICIIC = 0;			
			}
			else FramErrorLCD();	//��ʾ����ֱ����λ
		}		
	}
	
	//���õ���
	ptr_tmp = (u8 *)(&MeterData.BakEN[0]);
	IICIIC = 1;
	while(IICIIC)
	{
		if(SPI3Gets( 0x0054,16,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets( 0x0454,16,ptr_tmp)) IICIIC = 0;	
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{ 
				for(i=0; i<4; i++) {MeterData.BakEN[i] = 0x00;}
				SPI3Puts(0x0054,16,ptr_tmp);
				SPI3Puts(0x0454,16,ptr_tmp);
				IICIIC = 0;			
			}
			else
			{
				FramErrorLCD();	//��ʾ����ֱ����λ	
			}
		}		
	}
	ptr_tmp = (u8 *)&MeterSetData.UIViewLimit[0];
	IICIIC= 1;
	while(IICIIC)
	{
		if(SPI3Gets(0x006a,4,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets(0x046a,4,ptr_tmp)) IICIIC = 0;
		else	//ȱʡ����
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				MeterSetData.UIViewLimit[0] = 1000; //10.00V
				MeterSetData.UIViewLimit[1] = 100; //10.0mA
				SPI3Puts(0x006a,4,ptr_tmp);
				SPI3Puts(0x046a,4,ptr_tmp);	
				IICIIC = 0;	
			}
			else FramErrorLCD();	//��ʾ����ֱ����λ	
		}
	}
	
	//ϵͳ����
	ptr_tmp = (u8 *)&(MeterSetData.c_password);  
	IICIIC = 1;
	while(IICIIC)
	{
		if(SPI3Gets(0x0070,44,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets(0x0470,44,ptr_tmp)) IICIIC = 0;
		else	//ȱʡ����
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				MeterSetData.c_password = 0x01;
				MeterSetData.c_lcdlight = 60; //����ʱ��
				MeterSetData.c_lcdgray = 3;
				MeterSetData.c_cyc = 0;
				MeterSetData.c_lcdlanguage = 0;
				MeterSetData.c_powerview = 0;
				
				MeterSetData.c_pulseselect=0; //û������
				MeterSetData.c_DASelect = 0; //��������ѡ��
				MeterSetData.c_input[0] = 0;
				MeterSetData.c_input[1] = 0;
				MeterSetData.c_pt1 = 380;
				MeterSetData.c_ct1 = 5;
				MeterSetData.c_pt2 = 380;
				MeterSetData.c_ct2 = 5;
				MeterSetData.c_com1[0] = 0x01;
				MeterSetData.c_com1[1] = 0x03; //baud 9600
				MeterSetData.c_com1[2] = 0x00;
				MeterSetData.c_com1[3] = 0x00;
				MeterSetData.c_com2[0] = 0x02;
				MeterSetData.c_com2[1] = 0x03; //baud 9600
				MeterSetData.c_com2[2] = 0x00;
				MeterSetData.c_com2[3] = 0x00;
				MeterSetData.c_set[DISET] = 2; //DI
				MeterSetData.c_set[DOSET] = 1; //DO
				MeterSetData.c_set[AOSET] = 1; //AO 
				MeterSetData.c_set[COMSET] = 1; //ͨ��ѡ��
				MeterSetData.c_set[COM2SET] = 0; //�ڶ�·ͨ��
				MeterSetData.c_set[TOUSET] = 0;
				MeterSetData.c_set[CLASSSET] =0;
				MeterSetData.c_set[THDSET]=3; //0:15 1: 15 2:21 3:31 4:51; 5:63 
				SPI3Puts(0x0070,44,ptr_tmp);
				SPI3Puts(0x0470,44,ptr_tmp);	
				IICIIC = 0;
			}
			else
			{
				FramErrorLCD();	//��ʾ����ֱ����λ	
			}		
		}		
	}
	
		//���̵����������
	ptr_tmp = (u8 *)(&MeterSetData.c_DO[0][0]);  
	IICIIC = 1;
	while(IICIIC)
	{
		if(SPI3Gets(0x00a0,56,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets( 0x04a0,56,ptr_tmp)) IICIIC = 0;
		else	//ȱʡ����
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				for(i = 0; i < 5; i++)
				{
					MeterSetData.c_DO[0][i] = 0;
					MeterSetData.c_DO[1][i] = 0;
					MeterSetData.c_DO[2][i] = 0;
					MeterSetData.c_DO[3][i] = 0;
				}
				for(i=0; i<8; i++)
				{
					MeterSetData.c_DI[i] = 0;	
				}
				MeterSetData.c_DO[0][0] = 2;
				MeterSetData.c_DO[1][0] = 2;
				MeterSetData.c_DO[2][0] = 2;
				MeterSetData.c_DO[3][0] = 2;
				
				MeterSetData.c_DO[0][5] = 300;
				MeterSetData.c_DO[1][5] = 300;
				MeterSetData.c_DO[2][5] = 300;
				MeterSetData.c_DO[3][5] = 300;
				SPI3Puts(0x00a0,56,ptr_tmp);
				SPI3Puts(0x04a0,56,ptr_tmp);	
				IICIIC = 0;
			}
			else
			{
				FramErrorLCD();	//��ʾ����ֱ����λ	
			}		
		}		
	}
	

	//ģ������������
	ptr_tmp = (u8 *)&MeterSetData.c_AO[0][0];
	IICIIC = 1;
	while(IICIIC)
	{
		if(SPI3Gets(0x00dc,16,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets( 0x04dc,16,ptr_tmp)) IICIIC = 0;	
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				IICIIC = 0;
				for(i=0; i<2; i++)
				{
					MeterSetData.c_AO[i][0] = 0x01;
					MeterSetData.c_AO[i][1] = 0x06+i*3; //Ia,Pa
					MeterSetData.c_AO[i][2] = 0x00;
					MeterSetData.c_AO[i][3] = 5000;
				}
				SPI3Puts(0x00dc,16,ptr_tmp);
				SPI3Puts(0x04dc,16,ptr_tmp);		
			}
			else FramErrorLCD();	//��ʾ����ֱ����λ	
		}	
	}

	//ģ��������У׼
	ptr_tmp = (u8 *)&AdjustAo[0][0];
	IICIIC = 1;
	while(IICIIC)
	{
		if(SPI3Gets(0x00ee,8,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets( 0x04ee,8,ptr_tmp)) IICIIC = 0;	
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				IICIIC = 0;
				for(i=0; i<2; i++)
				{
					AdjustAo[i][0] = 0x0Cda;
					AdjustAo[i][1] = 0x3d40;
				}
				SPI3Puts(0x00ee,8,ptr_tmp);
				SPI3Puts(0x04ee,8,ptr_tmp);		
			}
			else FramErrorLCD();	//��ʾ����ֱ����λ	
		}	
	}
	
	//�������й������ʵ���
	ptr_tmp = (u8 *)&MonthEnergy[0].l_EPPTou[0][0];
	IICIIC=1;
	while(IICIIC)
	{
		if(SPI3Gets(0x0100,20,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets(0x0500,20,ptr_tmp)) IICIIC = 0;	
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				IICIIC = 0;
				for(i=0; i<5; i++) MonthEnergy[0].l_EPPTou[0][i]=0;
				SPI3Puts(0x0100,20,ptr_tmp);
				SPI3Puts(0x0500,20,ptr_tmp);		
			}
			else FramErrorLCD();	//��ʾ����ֱ����λ	
		}	
	}
	//���������й������ʵ���
	ptr_tmp = (u8 *)&MonthEnergy[0].l_EPPTou[1][0];
	IICIIC=1;
	while(IICIIC)
	{
		if(SPI3Gets(0x0116,20,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets(0x0516,20,ptr_tmp)) IICIIC = 0;	
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				IICIIC = 0;
				for(i=0; i<5; i++)
				{
					MonthEnergy[0].l_EPPTou[1][i]=0;
				}
				SPI3Puts(0x0116,20,ptr_tmp);
				SPI3Puts(0x0516,20,ptr_tmp);		
			}
			else FramErrorLCD();	//��ʾ����ֱ����λ	
		}	
	}
	//��1��-11�¸����ʵ���
	ptr_tmp = (u8 *)&MonthEnergy[0].l_EPPTou[2][0];
	IICIIC=1;
	while(IICIIC)
	{
		if(SPI3Gets(0x012c,220,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets(0x052c,220,ptr_tmp)) IICIIC = 0;	
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				IICIIC = 0;
				p32 = &MonthEnergy[0].l_EPPTou[2][0];  
				for(i=0; i<55; i++) {*p32++ = 0x00;}
				SPI3Puts(0x012c,220,ptr_tmp);
				SPI3Puts(0x052c,220,ptr_tmp);		
			}
			else FramErrorLCD();	//��ʾ����ֱ����λ	
		}	
	}
	
	//����˫�����
	ptr_tmp = (u8 *)(&MeterData.ENF[0]); 
	IICIIC = 1;
	while(IICIIC)
	{
		if(SPI3Gets(0x020a,16,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets(0x060a,16,ptr_tmp)) IICIIC = 0;	
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{ 
				for(i=0; i<4; i++) {MeterData.ENF[i] = 0x00;}
				SPI3Puts(0x020a,16,ptr_tmp);
				SPI3Puts(0x060a,16,ptr_tmp);
				IICIIC = 0;			
			}
			else FramErrorLCD();	//��ʾ����ֱ����λ	
		}		
	}
	
//������ֵ
	ptr_tmp = (u8 *)&MeterMax.UnMax[0];
	IICIIC=1;
	readnum=0;
	while(IICIIC)
	{
		//���ֵ
		if(SPI3Gets(MAXVALUEADDR[0][0],48,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets( MAXVALUEADDR[1][0],48,ptr_tmp)) IICIIC = 0;	
		else
		{
			keytemp = KeyRd();
			if((keytemp == (KEY_LEFT|KEY_ENTER))||(keytemp == (KEY_LEFT|KEY_RIGHT))||(readnum>2))
			{
				IICIIC = 0;
				p16 = (s16 *)&MeterMax.UnMax[0];  
				for(i=0; i<24; i++) {*p16++ = 0;}
				
				SPI3Puts(MAXVALUEADDR[0][0],48,ptr_tmp);
				SPI3Puts(MAXVALUEADDR[1][0],48,ptr_tmp);		
			}
			else readnum++;	//��ʾ����ֱ����λ	
		}	
	}
	
	//��Сֵ
	ptr_tmp = (u8 *)&MeterMax.UnMin[0];
	IICIIC=1;
	readnum=0;
	while(IICIIC)
	{
		//��Сֵ
		if(SPI3Gets( MAXVALUEADDR[0][1],48,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets( MAXVALUEADDR[1][1],48,ptr_tmp)) IICIIC = 0;	
		else
		{
			keytemp = KeyRd();
			if((keytemp == (KEY_LEFT|KEY_ENTER))||(keytemp == (KEY_LEFT|KEY_RIGHT))||(readnum>2))
			{
				IICIIC = 0;
				p16 = (s16 *)&MeterMax.UnMin[0]; 
				for(i=0; i<24; i++) {*p16++ = 9999;}	
				SPI3Puts(MAXVALUEADDR[0][1],48,ptr_tmp);
				SPI3Puts(MAXVALUEADDR[1][1],48,ptr_tmp);		
			}
			else readnum++;	//��ʾ����ֱ����λ	
		}	
	}


	
	//������ֵ MAXDEMANDADDR
	for(j=0; j<1; j++)
	{
		ptr_tmp = (u8 *)&DemandData[j+2][0];
		IICIIC=1;
		while(IICIIC)
		{
			//���ֵ
			if(SPI3Gets( MAXDEMANDADDR[0][j],12,ptr_tmp)) IICIIC = 0;
			else if(SPI3Gets( MAXDEMANDADDR[1][j],12,ptr_tmp)) IICIIC = 0;	
			else
			{
				keytemp = KeyRd();
				if((keytemp == (KEY_LEFT|KEY_ENTER))||(keytemp == (KEY_LEFT|KEY_RIGHT))||(readnum>2))
				{
					IICIIC = 0;
					p16 = (s16 *)&DemandData[j+2][0];  
					for(i=0; i<6; i++) {*p16++ = 0;}
					
					SPI3Puts(MAXDEMANDADDR[0][j],12,ptr_tmp);
					SPI3Puts(MAXDEMANDADDR[1][j],12,ptr_tmp);		
				}
				else readnum++;	//��ʾ����ֱ����λ	
			}	
		}
	}
	//����������ݣ�
	//�����й�����
	ptr_tmp = (u8 *)&MeterData.EPP[0];
	IICIIC=1;
	while(IICIIC)
	{
		if(SPI3Gets(0x0296,12,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets( 0x0696,12,ptr_tmp)) IICIIC = 0;	
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				IICIIC = 0;
				for(i=0; i<3; i++) {MeterData.EPP[i] = 0;}
				
				SPI3Puts(0x0296,12,ptr_tmp);
				SPI3Puts(0x0696,12,ptr_tmp);		
			}
			else FramErrorLCD();	//��ʾ����ֱ����λ	
		}	
	}
	
	ptr_tmp = (u8 *)&MeterData.EPN[0];
	IICIIC=1;
	while(IICIIC)
	{
		if(SPI3Gets(0x02a4,12,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets( 0x06a4,12,ptr_tmp)) IICIIC = 0;	
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				IICIIC = 0;
				for(i=0; i<3; i++) {MeterData.EPN[i] = 0;}
				
				SPI3Puts(0x02a4,12,ptr_tmp);
				SPI3Puts(0x06a4,12,ptr_tmp);		
			}
			else FramErrorLCD();	//��ʾ����ֱ����λ	
		}	
	}
	
	ptr_tmp = (u8 *)&MeterData.EQP[0];
	IICIIC=1;
	while(IICIIC)
	{
		if(SPI3Gets(0x02B2,12,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets( 0x06B2,12,ptr_tmp)) IICIIC = 0;	
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				IICIIC = 0;
				for(i=0; i<3; i++) {MeterData.EQP[i] = 0;}
				
				SPI3Puts(0x02B2,12,ptr_tmp);
				SPI3Puts(0x06B2,12,ptr_tmp);		
			}
			else FramErrorLCD();	//��ʾ����ֱ����λ	
		}	
	}
	
	ptr_tmp = (u8 *)&MeterData.EQN[0];
	IICIIC=1;
	while(IICIIC)
	{
		if(SPI3Gets(0x02c0,12,ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets( 0x06c0,12,ptr_tmp)) IICIIC = 0;	
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				IICIIC = 0;
				for(i=0; i<3; i++) {MeterData.EQN[i] = 0;}
				
				SPI3Puts(0x02c0,12,ptr_tmp);
				SPI3Puts(0x06c0,12,ptr_tmp);		
			}
			else FramErrorLCD();	//��ʾ����ֱ����λ	
		}	
	}
	
	//������ʱ������
	ptr_tmp = (u8 *)&sd_Data.c_sd[0][0][0];
	IICIIC = 1;
	while(IICIIC)
	{
		if(SPI3Gets(0x02ce, 0x54, ptr_tmp))  IICIIC = 0;
		else if(SPI3Gets(0x06ce, 0x54, ptr_tmp)) IICIIC = 0;
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				IICIIC = 0;	
				for(i=0; i<12; i++)
				{
					sd_Data.c_sd[0][i][0] = 0;
					sd_Data.c_sd[0][i][1] = 0;
					sd_Data.c_sd[1][i][0] = 0;
					sd_Data.c_sd[1][i][1] = 0;
					sd_Data.c_type[0][i] = 0;
					sd_Data.c_type[1][i] = 0;
					sd_Data.c_mons[i] = 0;
				}
				SPI3Puts(0x02ce, 0x54, ptr_tmp);	
				SPI3Puts(0x06ce, 0x54, ptr_tmp);	 
			}
			else FramErrorLCD(); //����ι��			
		}	
										  
	}
	//�����ռ�¼����Ҫ�Ǽ��ٴ洢ʱ��
	ptr_tmp =  (u8 *)&(sd_Data.c_copyday[0]);
	IICIIC = 1;
	while(IICIIC)
	{
		if(SPI3Gets(0x0324, 4, ptr_tmp))  IICIIC = 0;
		else if(SPI3Gets(0x0724, 4, ptr_tmp)) IICIIC = 0;
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				IICIIC = 0;	
				sd_Data.c_copyday[0] = 1;	//������
				sd_Data.c_copyday[1] = 0;	//hour
				sd_Data.c_copyday[2] = 0;	//�ϴγ����·�
				sd_Data.c_copyday[3] = 0;   //�ϴγ�����
				SPI3Puts(0x0324, 0x04, ptr_tmp);	
				SPI3Puts(0x0724, 0x04, ptr_tmp);	 
			}
			else FramErrorLCD(); //����ι��			
		}	
	}	
	//��������
	IICIIC = 1;
	ptr_tmp = (u8 *)&MeterSetData.c_demand[0][0];
	while(IICIIC)
	{
		if(SPI3Gets(0x032a, 0x0a, ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets(0x072a, 0x0a, ptr_tmp)) IICIIC = 0;
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				IICIIC= 0;
				MeterSetData.c_demand[0][0] = 0;
				MeterSetData.c_demand[0][1] = 0;
				MeterSetData.c_demand[0][2] = 60;
				MeterSetData.c_demand[0][3] = 5;
				MeterSetData.c_MaxMinTime=15;
				
				SPI3Puts(0x032a,0x0a, ptr_tmp);	
				SPI3Puts(0x072a,0x0a, ptr_tmp);
			}	
			else FramErrorLCD(); //����ι��
		}		
	}
	
	//�Ǳ�ʱ���ۼ�
	IICIIC = 1;
	ptr_tmp = (u8 *)&MeterTimeRecord[0];
	while(IICIIC)
	{
		if(SPI3Gets(0x0344, 20, ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets(0x0744, 20, ptr_tmp)) IICIIC = 0;
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				IICIIC= 0;
				for(i=0; i<5; i++) MeterTimeRecord[i] = 0;
				SPI3Puts(0x0344,20, ptr_tmp);	
				SPI3Puts(0x0744,20, ptr_tmp);
			}	
			else FramErrorLCD(); //����ι��
		}		
	}
	
	//��ѹ�����ϸ���
	IICIIC = 1;
	ptr_tmp = (u8 *)&MeterSetData.c_vgoodvalue[0];
	while(IICIIC)
	{
		if(SPI3Gets(0x035a, 8, ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets(0x075a, 8, ptr_tmp)) IICIIC = 0;
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				IICIIC= 0;
				MeterSetData.c_vgoodvalue[0] = 3900;
				MeterSetData.c_vgoodvalue[1] = 570;
				MeterSetData.c_fgoodvalue[0] = 5100;
				MeterSetData.c_fgoodvalue[1] = 4900;
				
				SPI3Puts(0x035a,8, ptr_tmp);	
				SPI3Puts(0x075a,8, ptr_tmp);
			}	
			else FramErrorLCD(); //����ι��
		}		
	}
	
	//Խ�ޱ���ֵ
	IICIIC = 1;
	ptr_tmp = (u8 *)&MeterSetData.c_AlarmUn[0];
	while(IICIIC)
	{
		if(SPI3Gets(0x03aa, 84, ptr_tmp)) IICIIC = 0;
		else if(SPI3Gets(0x07aa, 84, ptr_tmp)) IICIIC = 0;
		else
		{
			keytemp = KeyRd();
			if(keytemp == (KEY_LEFT|KEY_ENTER))
			{
				IICIIC= 0;
				
				MeterSetData.c_AlarmUn[0]=4560; //U�߱���
				MeterSetData.c_AlarmUn[1]=0; //������
				MeterSetData.c_AlarmUn[2]=0; //�ͱ���
				MeterSetData.c_AlarmUn[3]=0; //������
				MeterSetData.c_AlarmUn[4]=3; //��ʱʱ��
				
				MeterSetData.c_AlarmUl[0]=7898; //�߱���
				MeterSetData.c_AlarmUl[1]=0; //������
				MeterSetData.c_AlarmUl[2]=0; //�ͱ���
				MeterSetData.c_AlarmUl[3]=0; //������
				MeterSetData.c_AlarmUl[4]=3; //��ʱʱ��
				
				MeterSetData.c_AlarmI[0]=6000; //�߱���
				MeterSetData.c_AlarmI[1]=0; //������
				MeterSetData.c_AlarmI[2]=0; //�ͱ���
				MeterSetData.c_AlarmI[3]=0; //������
				MeterSetData.c_AlarmI[4]=3; //��ʱʱ��
				
				MeterSetData.c_AlarmF[0]=5100; //�߱���
				MeterSetData.c_AlarmF[1]=0; //������
				MeterSetData.c_AlarmF[2]=4900; //�ͱ���
				MeterSetData.c_AlarmF[3]=0; //������
				MeterSetData.c_AlarmF[4]=3; //��ʱʱ��
				
				MeterSetData.c_AlarmP[0]=6840; //�߱���
				MeterSetData.c_AlarmP[1]=0; //������
				MeterSetData.c_AlarmP[2]=0; //�ͱ���
				MeterSetData.c_AlarmP[3]=0; //������
				MeterSetData.c_AlarmP[4]=3; //��ʱʱ��
				
				MeterSetData.c_AlarmPF[0]=0; //�ͱ���
				MeterSetData.c_AlarmPF[1]=0; //������
				MeterSetData.c_AlarmPF[2]=3; //��ʱʱ��
				
				MeterSetData.c_AlarmTHDU[0]=2000; //�߱���
				MeterSetData.c_AlarmTHDU[1]=0; //������
				MeterSetData.c_AlarmTHDU[2]=3; //��ʱʱ��
				
				MeterSetData.c_AlarmTHDI[0]=4000; //�߱���
				MeterSetData.c_AlarmTHDI[1]=0; //������  
				MeterSetData.c_AlarmTHDI[2]=3; //��ʱʱ��  
				
				MeterSetData.c_AlarmUun[0]=100; //�߱���
				MeterSetData.c_AlarmUun[1]=0; //������  
				MeterSetData.c_AlarmUun[2]=3; //��ʱʱ��  
				
				MeterSetData.c_AlarmIun[0]=100; //�߱���
				MeterSetData.c_AlarmIun[1]=0; //������  
				MeterSetData.c_AlarmIun[2]=3; //��ʱʱ��  
				
				MeterSetData.c_AlarmEnable[0]=0xffff;                                       
				MeterSetData.c_AlarmEnable[1]=0xffff;
				  
				SPI3Puts(0x03aa,84, ptr_tmp);	
				SPI3Puts(0x07aa,84, ptr_tmp);
			}	
			else FramErrorLCD(); //����ι��
		}	
		
		//Խ�ޱ�������ֵ
		IICIIC = 1;
		ptr_tmp = (u8 *)&MeterSetData.MeterNo[0];
		while(IICIIC)
		{
			if(SPI3Gets(0x0336, 12, ptr_tmp)) IICIIC = 0;
			else if(SPI3Gets(0x0736, 12, ptr_tmp)) IICIIC = 0;
			else
			{
				keytemp = KeyRd();
				if((keytemp == (KEY_LEFT&KEY_ENTER))||(keytemp == (KEY_LEFT&KEY_RIGHT)))
				{
					IICIIC= 0;
					for(i=0; i<12; i++)
					{
						MeterSetData.MeterNo[i]=0x31;	
					}
					
					SPI3Puts(0x0336,12, ptr_tmp);	
					SPI3Puts(0x0736,12, ptr_tmp);
				}	
				else FramErrorLCD(); //����ι��
			}		
		}
		
	}	
	//�Ի���״̬���г�ʼ��
	SystemCheck(); //ϵͳ�������
	
	MeterDio.i_Do = 0;
	MeterDio.i_Di = 0;
	Cal_AO(0);
	Cal_AO(1);  
	CalPtCt(); //������
	  	
}

//ϵͳ�������
void SystemCheck(void)
{
	u8 i;
	if(MeterSetData.c_lcdgray>7) MeterSetData.c_lcdgray=3;
	if(MeterSetData.c_cyc>1) MeterSetData.c_cyc=0;
	if(MeterSetData.c_lcdlanguage>1) MeterSetData.c_lcdlanguage=0;
	if(MeterSetData.c_powerview>POWERMAX) MeterSetData.c_powerview = 0;				
	if((MeterSetData.c_com1[0]==0)||(MeterSetData.c_com1[0]>247)) MeterSetData.c_com1[0]=1;
	if(MeterSetData.c_com1[1]>5) MeterSetData.c_com1[1] = 3;	
	if(MeterSetData.c_com1[2]>3) MeterSetData.c_com1[2] = 0;	
	if(MeterSetData.c_com1[3]>1) MeterSetData.c_com1[3] = 0;	
	
	if((MeterSetData.c_com2[0]==0)||(MeterSetData.c_com2[0]>247)) MeterSetData.c_com1[0]=1;
	if(MeterSetData.c_com2[1]>5) MeterSetData.c_com2[1] = 3;	
	if(MeterSetData.c_com2[2]>3) MeterSetData.c_com2[2] = 0;	
	if(MeterSetData.c_com2[3]>1) MeterSetData.c_com2[3] = 0;
		
	if(MeterSetData.c_input[3]>1) MeterSetData.c_input[3]=0;
	if(MeterSetData.c_input[0]>2) MeterSetData.c_input[0]=0;	
	if(MeterSetData.c_input[1]>1) MeterSetData.c_input[1]=0;
	if((MeterSetData.c_pt1==0)||(MeterSetData.c_pt1>999999)) MeterSetData.c_pt1=380;
	if((MeterSetData.c_pt2==0)||(MeterSetData.c_pt2>690)) MeterSetData.c_pt2=380;	
	if((MeterSetData.c_ct1==0)||(MeterSetData.c_ct1>999999)) MeterSetData.c_ct1=5;
	if((MeterSetData.c_ct2==0)||(MeterSetData.c_ct2>6)) MeterSetData.c_ct2=5;		
	for(i=0; i<2; i++)
	{
		if(MeterSetData.c_AO[i][0]>3) MeterSetData.c_AO[i][0]=1;
		if(MeterSetData.c_AO[i][1]>26) MeterSetData.c_AO[i][0]=6;
	}
	for(i=0; i<4; i++)
	{
		if(MeterSetData.c_DO[i][0]>2)	MeterSetData.c_DO[i][0]=2;
		if(MeterSetData.c_DO[i][1]>9999)	MeterSetData.c_DO[i][1]=0;
		if(MeterSetData.c_DO[i][2]>STEP_5_DIN60)	MeterSetData.c_DO[i][2]=0;	
		if(MeterSetData.c_DO[i][5]>9999)	MeterSetData.c_DO[i][5]=0;		
	}
	MeterSetData.c_demand[0][0] = 0;
	if(MeterSetData.c_demand[0][1]>1) MeterSetData.c_demand[0][1]=0;
	if(MeterSetData.c_demand[0][1]>9999) MeterSetData.c_demand[0][2]=60;	
	if(MeterSetData.c_demand[0][1]>30) MeterSetData.c_demand[0][2]=15;		
	
	for(i=0; i<12; i++)
	{
		if(sd_Data.c_sd[0][i][0]>23) sd_Data.c_sd[0][i][0]=0;  //ʱ	
		if(sd_Data.c_sd[0][i][1]>59) sd_Data.c_sd[0][i][1]=0;	 //��
		if(sd_Data.c_sd[1][i][0]>23) sd_Data.c_sd[1][i][0]=0;  //ʱ	
		if(sd_Data.c_sd[1][i][1]>59) sd_Data.c_sd[1][i][1]=0;	 //��
			
		if(sd_Data.c_type[0][i]>3) sd_Data.c_type[0][i]=0;	//��������
		if(sd_Data.c_type[1][i]>3) sd_Data.c_type[1][i]=0;		
		if(sd_Data.c_mons[i]>3) sd_Data.c_mons[i]=0;	//

		if((sd_Data.c_copyday[0]>28)||(0==sd_Data.c_copyday[0])) sd_Data.c_copyday[0]=1;			
		if(sd_Data.c_copyday[1]>23)	sd_Data.c_copyday[1]=0;
	}
	
	if(MeterSetData.c_set[THDSET]<3) MeterSetData.c_set[THDSET]=3;
	
		
}

//������
void CalPtCt(void)
{
	PtResult = (float)MeterSetData.c_pt1/MeterSetData.c_pt2;  //��ѹ���
	CtResult = (float)MeterSetData.c_ct1/MeterSetData.c_ct2;  //�������
	PtCtResult = (float)MeterSetData.c_pt1/MeterSetData.c_pt2*MeterSetData.c_ct1/MeterSetData.c_ct2; //��ѹ�������		
}
