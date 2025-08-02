
/******************************************************************
Describe: 完成通信程序相关任务

TaskUsart3: 目前只开放USART1

*******************************************************************/
#include "User_Config.h"
#define DLTMARKLEN 103		//DLT645命令数量
#define DLTMARKLEN07 76
u8 ReadData04(u8 *ptr, u16 addr, u16 midnum);
extern void Time2Init(void);
float calf_u(s32 tmp_dl);
float calf_i(s32 tmp_dl);
float calf_p(s32 tmp_pq);
float calf_dn(u32 tmp_dn);
float calf_cf(s16 tmp_cf,s16 tmp_dev);
float calf_cf2(s32 tmp_cf,s16 tmp_dev);
extern void LCDCmd(u8 LCDCommand);

#define SOEFILE		0x00
#define UHIGHFILE 	0x01
#define ULOWFILE 	0x02
#define IHIGHFILE	0x03  //电压中断文件
#define ILOWFILE	0x04
#define PHIGHFILE	0x05
#define PLOWFILE	0x06


const u8 UartAddrCode10[32] = 
{
	"9TY.1013.220407"
};
const u8 UartAddrCode11[32] = 
{
	"PD194Z-9TY"
};


//DLT645 常量表 1997
//DLT645命令
const u16 DLT645IdName[DLTMARKLEN] = {
	
	0x9010,0x9011,0x9012,0x9013,0x9014,0x901F,0x9020,0x902F,0x9110,0x911F,0x9120,0x912F, //12
	0x9130,0x9140,0x9150,0x9160,	//四象限无功电能 4
	0x9410,0x9411,0x9412,0x9413,0x9414,0x941F,0x9810,0x9811,0x9812,0x9813,0x9814,0x981F, //12上月，上上月复费率电能 12
	0x9FFF,
//瞬时数据	
	0xb611,0xb612,0xb613,0xb61f, 0xb691,0xb692,0xb693,0xb69f, 0xb621,0xb622,0xb623,0xb62f, // 电压，电流 12
	0xb630,0xb631,0xb632,0xb633,0xb63f, 0xb640,0xb641,0xb642,0xb643, 0xb64f, //有功功率，无功功率 10
	0xb650,0xb651,0xb652,0xb653,0xb65f, 0xb660,0xb661,0xb662,0xb663, 0xb66f, //后四个为视在功率, 数据类型是自己定义的 8
//频率
	0xb680,	0xb6ff, //频率 总电量 2
//可读可修改部分	
	0xc010,0xc011,0xc032,0xc033,0xc034,	//时间， 仪表号，5	
	0xc331,0xc332,0xc333,0xc334,0xc335,0xc336,0xc337,0xc338,0xc339,0xc33a,0xc33b,0xc33c,	//复费率1 12
	0xc341,0xc342,0xc343,0xc344,0xc345,0xc346,0xc347,0xc348,0xc349,0xc34a,0xc34b,0xc34c,	//复费率2 12
	0xc117,															//抄表日 1
//常数
	0xc030, 0xc031,	//2
//电表运行状态B1	
	0xc020,	//1
//谐波部分, 数据类型是自己定义的
	0xb711,0xb712,0xb713,0xb714,0xb715,0xb716, 0xb71f,	//THD 7		
		

	};

//相对应的地址偏移量
const u16 DLT645IdAddr[DLTMARKLEN] = {
//电能数据
	0x0000,0x0004,0x0008,0x000c,0x0010,0x0000,0x0014,0x0014,0x0018,0x0018,0x001c,0x001c, //12
	0x0020,0x0024,0x0028,0x002c,	                                                     //4
	0x0030,0x0034,0x0038,0x003c,0x0040,0x0030,0x0044,0x0048,0x004c,0x0050,0x0054,0x0044, //12
	0x0000, //所有电能数据
//瞬时数据
	0x0058,0x005a,0x005c,0x0058,0x005e,0x0060,0x0062,0x005e,0x0064,0x0066,0x0068,0x0064, 				//9 电压电流
	0x006a,0x006d,0x0070,0x0073,0x006a,0x0076,0x0078,0x007a,0x007c,0x0076,	//10 有功功率，无功功率
	0x007e,0x0080,0x0082,0x0084,0x007e,0x0086,0x0088,0x008a,0x008c,0x0086, 	//8 功率因数，视在功率,
	0x008e, 0x0058, // 频率, 总电量
//可读可修改部分	
	0x0090,0x0094,0x0097, 0x0097,0x0097, 		//5
	0x009d,0x00a0,0x00a3,0x00a6,0x00a9,0x00ac,0x00af,0x00b2,0x00b5,0x00b8,0x00bb,0x00be,	//12
	0x00c1,0x00c4,0x00c7,0x00ca,0x00cd,0x00d0,0x00d3,0x00d6,0x00d9,0x00dc,0x00df,0x00e2,	//12
	0x00e5,
//常数部分
	0x00e8,	0x00e8,
//电表运行状态B1
	0x00eb,
//总谐波畸变率
	0x011b,0x011d,0x011f,			//3
	0x0121,0x0123,0x0125,0x011b,  	//4	
};
	
//对应的长度
const u8 DLT645IdLen[DLTMARKLEN] ={
	
	4, 4, 4, 4, 4,20, 4, 4, 4, 4, 4, 4,  //12
	4, 4, 4, 4,  						 //4	
	4, 4, 4, 4, 4,20, 4, 4, 4, 4, 4, 20, //12
	88, 								 //1
//瞬时数据
	2, 2, 2, 6, 2, 2, 2, 6, 2, 2, 2, 6,	 //12
	3, 3, 3, 3, 12, 2, 2, 2, 2, 8,	     //10
	2, 2, 2, 2, 8,  2, 2, 2, 2, 8,       //10
	2, 56,					             //2
//可读可修改部分						
    4, 3, 6, 6,6,						 //5
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,	 //12
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,	 //12
	2, 									 //1
//常数部分
	3, 3,								 //2
//电表运行状态B1
	1,                                   //1
//总谐波畸变率
	2, 2, 2, 2, 2, 2, 12,				 //7

};	

//DLT645-2007协议
const u32 DLT645IdName07[DLTMARKLEN07] =
{
//电能数据
	//电能数据
	0x00010000, 0x00010100, 0x00010200, 0x00010300, 0x00010400, 0x0001ff00,	0x00020000, 0x00030000, 0x00040000, //正向电能，总费率电能 其余双向电能
	0x00050000,	0x00080000,	0x00060000,	0x00070000, //四象限无功电能 
	0x00010001, 0x00010101,0x00010201,0x00010301,0x00010401,0x0001ff01,0x00010002, 0x00010102,0x00010202,0x00010302,0x00010402, 0x0001ff02,               //复费率电能上月，上上月
	
//电量数据
	0x02010100, 0x02010200, 0x02010300, 0x0201ff00, //Un 4
	0x020c0100, 0x020c0200, 0x020c0300, 0x020cff00, //Ul 4
	0x02020100, 0x02020200, 0x02020300, 0x0202ff00, //I  4                          
	0x02030000, 0x02030100, 0x02030200, 0x02030300, 0x0203ff00, //P 5
	0x02040000, 0x02040100, 0x02040200, 0x02040300, 0x0204ff00, //Q 5
	0x02050000, 0x02050100, 0x02050200, 0x02050300, 0x0205ff00, //S 5
	0x02060000, 0x02060100, 0x02060200, 0x02060300, 0x0206ff00, //PF 5
	0x02800002,   												//F 1

//可读可修改部分
	0x04000101, 0x04000102,	0x04000401,	//日期 时间 通信地址 3
//复费率数据(两套费率时段)
	0x04010001, 0x04020001,
//抄表日 电能脉冲(2) 
	0x04000b01,	0x04000409, 0x0400040a, //3
//状态值 	
    0x04000501, 0x04000502, //2
//总谐波畸变率
	0x02080100, 0x02080200, 0x02080300, 0x0208ff00, //THDUa,THDUb,THDUc, THDU   
	0x02090100, 0x02090200, 0x02090300, 0x0209ff00, //THDIa,THDIb,THDIc, THDI
	
			
};
const u16 DLT645IdAddr07[DLTMARKLEN07] =
{
//电能数据
	0x0000, 0x0004, 0x0008, 0x000c, 0x0010, 0x0000, 0x0014, 0x0018, 0x001c, //9
	0x0020, 0x0024, 0x0028, 0x002c,  //4
	0x0030, 0x0034, 0x0038, 0x003c, 0x0040, 0x0030, 0x0044,0x0048,0x004c,0x0050, 0x0054, 0x0044, //12
//电量数据
	0x010f,0x0111,0x0113,0x010f,  //Un 4	
	0x0115,0x0117,0x0119,0x0115,  //Ul 4
	0x00ec,0x00ef,0x00f2,0x00ec, //I  4                          
	0x006a,0x006d,0x0070,0x0073,0x006a,  //P 5
	0x00f5,0x00f8,0x00fb,0x00fe,0x00f5,  //Q 5
	0x00101,0x0104,0x0107,0x010a,0x0101,  //S 5
	0x007e,0x0080,0x0082,0x0084,0x007e,  //PF 5
	0x008e,                                   //F 1	
//可读可修改部分
	0x0090,0x0094,0x0097,//日期 时间 通信地址 3
//复费率数据(两套费率时段)
	0x009d, 0x00c1,				//2
//抄表日 电能脉冲(2)
	0x00e5,	0x00e8, 0x00e8, //3	
//状态值 	
	0x010d, 0x0127,          //2
//总谐波
	0x011b,0x011d,0x011f,0x011b,	//4
	0x0121,0x0123,0x0125,0x0121,  	//4	
};	
const u8 DLT645IdLen07[DLTMARKLEN07] =
{
//电能数据	
	4,4,4,4,4,20,	4,	4,	4,												//5
	4,	4,	4,	4,	
	4,4,4,4,4,20, 4,4,4,4,4,20,	
//电量数据	
	2,	2,	2,	6,	//Un  4
	2,	2,	2,	6,	//Ul  4
	3,	3,	3,	9,	//I   4
	3,	3,	3,	3,	12,	//P  5
	3,	3,	3,	3,	12,	//Q  5
	3,	3,	3,	3,	12,	//S  5
	2,	2,	2,	2,	8,	//PF 5
	2,					//F  1
//可读可修改部分
	4,	3,	6,	        //3
//复费率设置
	36,36,              //2																				
//抄表日 电能脉冲(2) 
	2,  3,	3,          //3
//状态值 
	2,2,	                //2
//总谐波畸变率
	2,  2,  2, 6,       //4
	2,  2,  2, 6,		//4										
};



/***********************************************************
Name: void TaskUsart3(void)
Function: Usart3通信
***********************************************************/
void TaskUsart3(void)
{
	u8 DLT645Begin, *ptr_tmp;
	u8 crcchar, i;
	u8 recchar;
	u16 crc, rec;
	if(TASK_START[0]&Tab_32bit[0][BIT_USART3]) TASK_START[0] &= Tab_32bit[1][BIT_USART3];
	
	if(0==MeterSetData.c_set[COMSET]) //通信关闭
	{
			if((Usart2RxTxBuf[1]==0x0e)&&(Usart2RxTxBuf[2]==0xc2)&&(Usart2RxTxBuf[3]==0x2c)&&(Usart2RxTxBuf[4]==0xac))
			{
				MeterSetData.c_set[COMSET] = 1;
				ptr_tmp = (u8 *)&(MeterSetData.c_password);  
				SPI3Puts(0x0070,44,ptr_tmp);
				SPI3Puts(0x0470,44,ptr_tmp);	
			}	
			Usart2RxTxNum=0;
			return;
	}	
	
	//判断长度是否正确
	if(Usart2RxTxNum < 0x05)
	{
		ResetUsart3();
		return;	
	}
	if(1==MeterSetData.c_com1[3]) //DLT645协议
	{
		RenewDLTAddr(); //从相同的地址读取仪表地址和baud，每次判断时需重新计算地址和baud
		
		DLT645Begin = 0;
	//判断校验码是否正确
	//查出开始地址
		for(i=0; i < Usart2RxTxNum; i++)
		{
			if(0x68 == Usart2RxTxBuf[i])
			{
				DLT645Begin = i;
				break;	
			}		
		}
		Usart2RxTxNum -= DLT645Begin;
		for(i=0; i<Usart2RxTxNum; i++)
		{
			Usart2RxTxBuf[i] = Usart2RxTxBuf[i+DLT645Begin];	
		}
		i = 0x33;
		//地址小于247，只用低字节的两个byte
		if((Usart2RxTxBuf[1] == DLT645Data.MeterNum[0])&&(Usart2RxTxBuf[2] == DLT645Data.MeterNum[1]))
		{
			i = 0x00;	
		}	
		if((0x99 == Usart2RxTxBuf[1])&&(0x99 == Usart2RxTxBuf[2])&&(0x99 == Usart2RxTxBuf[3])&&(0x99 == Usart2RxTxBuf[4])&&(0x99 == Usart2RxTxBuf[5])&&(0x99 == Usart2RxTxBuf[6]))
		{
			//判断是否更改通信地址命令
			i = 0x00;	
		}
		if((0xAA == Usart2RxTxBuf[1])&&(0xAA == Usart2RxTxBuf[2])&&(0xAA == Usart2RxTxBuf[3])&&(0xAA == Usart2RxTxBuf[4])&&(0xAA == Usart2RxTxBuf[5])&&(0xAA == Usart2RxTxBuf[6]))
		{
			i = 0x00;	
		}
			
		if(0x33 == i)
		{
			ResetUsart3();
			return;	
		}	
		//校验码检验 
		crcchar = 0x00;
		for(i=0; i<(Usart2RxTxBuf[9] + 10); i++)
		{
			crcchar += Usart2RxTxBuf[i];	
		}
		recchar = Usart2RxTxBuf[i];
		//judge the crcchar is right or not
		if(crcchar != recchar)
		{
			ResetUsart3();	//can use send data to 
			return;		
		}
		Com1Enable = 0x01;	//表示接收到正确数据，可以显示通信标识
		Com1Time =  Time10ms;
		
		
		
		Check2Usart = 0;
		GPIO_ResetBits(GPIOA, GPIO_USART3CS);  //发送控制IO口	
		//将数据装换为DLT数据格式
		DLT645Dealwith();
		
		Com1TxTime = 0; //发送时间
		Com1TxEnable = 1;
	}
	else
	{
		
		//判断地址是否正确
		if((0x00 == Usart2RxTxBuf[0])||(Usart2RxTxBuf[0]==MeterSetData.c_com1[0])||(0xff == Usart2RxTxBuf[0]))
		{
			rec = (Usart2RxTxBuf[Usart2RxTxNum - 2]<<8) | Usart2RxTxBuf[Usart2RxTxNum - 1];
		}
		else 
		{
			ResetUsart3();
			return;	
		}	
		crc = Cal_CRC(Usart2RxTxBuf,(Usart2RxTxNum - 2));
		if(crc != rec)  //判断校验码是否正确
		{
			ResetUsart3();
			return;	
		}	
		
		Com1Enable = 0x01;
	    Com1Time =  Time10ms;
	    
	    
	    Com1TxTime = 0; //发送时间
		Com1TxEnable = 1;
		
		Check2Usart = 0;
		GPIO_ResetBits(GPIOA, GPIO_USART3CS);  //发送控制IO口	
		UsartReply(); //
	}
}



/*************************************
Name; UsartReply(void)
Function: 通信应答，通用的TaskUsart1
Input: None

**************************************/
void UsartReply(void)
{
	u8 ErrCode = 0;
	u16 crc16;
	switch(Usart2RxTxBuf[1])
	{
		case 0x01:  //继电器状态
			ErrCode = ReadCoilSta();
			break;	
		case 0x02:  //开关量输入状态
			ErrCode = ReadInputSta();
			break;
		case 0x03: //电量数据和系统数据
		case 0x04:
			ErrCode = UsartRead();			
			break;
		case 0x05:  //遥控单个继电器
			ErrCode = ForceSingleBit();
			break;
		case 0x06:	//写单个寄存器
			ErrCode = UsartWrite06();
			break;
		case 0x0E:  	//自定义指令
			ErrCode = UserDefine(&Usart2RxTxBuf[2]);
			break;
		case 0x0F: //控制多个继电器
			ErrCode = ForceMultiBit();
			break;
		case 0x10: //写多个寄存器
			ErrCode = UsartWrite();
			break;
		default:
			ErrCode = 1;
			break;
	}
	if(0 != ErrCode)  //通信数据存在错误
	{
		Usart2RxTxBuf[1] |= 0x80;
		Usart2RxTxBuf[2] = ErrCode;
		crc16 = Cal_CRC(Usart2RxTxBuf,3);
		Usart2RxTxBuf[3] = (u8)(crc16>>8);
		Usart2RxTxBuf[4] = (u8)(crc16);
		Usart2TxNum = 0x05;
	}
	UsartReceiveFinish = 1;
	UsartDelayTime = 0;
			
}

const u8 USARTTXTIMEDELAY[6] = {100,60,40,20,20,20};  //20个时间发生时间
/********************************************
Name: u8 UserDefine(u8 *ptr)
Function: 用户自定义命令
*******************************************/
u8 UserDefine(u8 *ptr)
{
	u8 ErrCode = 0;
	u8 i, *ptr_tmp; 
	u16 crc16,  com;
	u32 miduint32;
	com=(Usart2RxTxBuf[2])*256+ Usart2RxTxBuf[3];
	if(0xaa00 == com)
	{//进入校准
		DAAjustMark = 0xaa;	
	}	
	else if(0x5500 == com)
	{//退出校准
		DAAjustMark = 0x00;
	}	
	else if(0xA0 == Usart2RxTxBuf[2])
	{//交流校准
		if(0 == Usart2RxTxBuf[3])  //清校准系数
		{
			TASK_START[3] |= (Tab_32bit[0][BIT_CALRESET]);	
		}	
		else if(1 == Usart2RxTxBuf[3]) //校准电压电流
		{
			TASK_START[3] |= (Tab_32bit[0][BIT_CALUI]);	
		}	
		else if(2 == Usart2RxTxBuf[3])  //校准PS
		{
			TASK_START[3] |= (Tab_32bit[0][BIT_CALPS]);
		}
		else if(3 == Usart2RxTxBuf[3]) //校准Q
		{
			TASK_START[3] |= (Tab_32bit[0][BIT_CALQ]);
		}	
		else if(4 == Usart2RxTxBuf[3]) //校准相角
		{
			TASK_START[3] |= (Tab_32bit[0][BIT_CALANGLE]);
		}	
		else if(5 == Usart2RxTxBuf[3]) //校准保存
		{
			TASK_START[3] |= Tab_32bit[0][BIT_CALSAVE];
		}	
			
		
		for(i=0; i < 10; i++)  //发送的数据不包括CRC16
		{
			Usart2RxTxBuf[i] = Usart2RxTxBuf[i];	
		}
		crc16=Cal_CRC(Usart2RxTxBuf, i);    
		Usart2RxTxBuf[i++]=(u8)(crc16>>8);  
		Usart2RxTxBuf[i++]= crc16&0xFF;  
		Usart2TxNum = i; //需要发送的数据个数
	}
		else if(0xaa55==com)  //跳转只IAP
	{
		if(0!=Usart2RxTxBuf[0])
		{
			Usart2RxTxBuf[0]=0x55;
			Usart2RxTxBuf[1]=0xaa;
			Usart2RxTxBuf[2]=0x00;
			Usart2RxTxBuf[3]=0x0e;
			Usart2RxTxBuf[4]=0x57;
			Usart2RxTxBuf[5]=0x00;
			Usart2RxTxBuf[6]=0x00;
			Usart2RxTxBuf[7]=0x57;
			//回送数据
			Usart2TxNum=8;
			DMA1_Channel2->CNDTR = Usart2TxNum;  //需要发送的数据
			USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
			DMA_Cmd(DMA1_Channel2, ENABLE);
			USART_Cmd(USART3, ENABLE); 
			Delayms(USARTTXTIMEDELAY[MeterSetData.c_com1[1]%6]);
		}
		
			BKP_WriteBackupRegister(BKP_DR1, 0x1225);
			__set_FAULTMASK(1);
			NVIC_SystemReset();
		
	}	
	else if(((Usart2RxTxBuf[0] == MeterSetData.c_com1[0])&&(0xA1 == *ptr)) || ((Usart2RxTxBuf[0] == 0x00)&&(0xA1 == *ptr)))	//AD变送输出校准
	{
		com=(*(ptr + 4))*256+*(ptr+5);	//变送数据
		if(0x01 == *(ptr + 1))
		{
			switch(*(ptr + 3))	//
			{
				case 0x01:	//第一路变送 4mA
					TIM2->CCR1= com;
					AdjustAo[0][0] = com;
					break;
				case 0x04:	//20mA	
					TIM2->CCR1= com;
					AdjustAo[0][1] = com;
					break;
				default:
					break;
			}			
		}
		if(0x02 == *(ptr + 1))
		{
			switch(*(ptr + 3))	//
			{
				case 0x01:	//第一路变送 4mA
					TIM2->CCR2= com;
					AdjustAo[1][0] = com;
					break;
				case 0x04:	//20mA	
					TIM2->CCR2= com;
					AdjustAo[1][1] = com;
					break;
				default:
					break;
			}			
		}
		else if(0x0f == *(ptr + 1))	//校准保存
		{
			//保存AO校准参数
			ptr_tmp = (u8 *)&AdjustAo[0][0];
			SPI3Puts(0x00ee,8,ptr_tmp);
			SPI3Puts(0x04ee,8,ptr_tmp);	              
		}					
	}
	
	else if((Usart2RxTxBuf[0] == MeterSetData.c_com1[0])&&(0xA4 == *ptr))	//四象限电能预置
	{
		if(0x00 == *(ptr+1))	//预置正向有功无功电能 
		{
			miduint32 = (u32)*(ptr + 2)*0x1000000 + (u32)*(ptr + 3)*0x10000 + (u16)*(ptr + 4)*0x100 + *(ptr + 5);
			MeterData.EN[0] = miduint32;
			miduint32 = (u32)*(ptr + 6)*0x1000000 + (u32)*(ptr + 7)*0x10000 + (u16)*(ptr + 8)*0x100 + *(ptr + 9);
			MeterData.EN[2] = miduint32;
			
		}
		else if(0x01 == *(ptr + 1))	//预置反向有功无功电能
		{
			miduint32 = (u32)*(ptr + 2)*0x1000000 + (u32)*(ptr + 3)*0x10000 + (u16)*(ptr + 4)*0x100 + *(ptr + 5);
			MeterData.EN[1] = miduint32;
			miduint32 = (u32)*(ptr + 6)*0x1000000 + (u32)*(ptr + 7)*0x10000 + (u16)*(ptr + 8)*0x100 + *(ptr + 9);
			MeterData.EN[3] = miduint32;
		}
		ptr_tmp = (u8 *)(&MeterData.EN[0]); //正向有功电能
    	SPI3Puts(0x002c,20,ptr_tmp);
		SPI3Puts(0x042c,20,ptr_tmp);	
	}
	else if((0xA300 == com)||(0xA502 == com))	//RTC校时,暂时不做处理
	{
		if((0==*(ptr+3))||(*(ptr+3)>12)) return (3);  //月
		if((0==*(ptr+4))||(*(ptr+3)>31)) return (3);  //日
		if((*(ptr+5)>23)) return (3);	//时
		if((*(ptr+6)>59)) return (3);	//分
		if((*(ptr+7)>59)) return (3);	//秒
		sd_Data.c_time[0] = *(ptr+2);	//
		sd_Data.c_time[1] = *(ptr+3);
		sd_Data.c_time[2] = *(ptr+4);
		sd_Data.c_time[3] = *(ptr+5);
		sd_Data.c_time[4] = *(ptr+6);
		sd_Data.c_time[5] = *(ptr+7);
		sd_Data.c_time[6] = *(ptr+8);
		TaskWriteCurTime(&sd_Data.c_time[0]); //需立即执行
		TASK_START[0] |= Tab_32bit[0][BIT_READRTC];
		
	}	
	
	else if((Usart2RxTxBuf[0] == MeterSetData.c_com1[0])&&(0xA800 == com)) //模块选择
	{
		MeterSetData.c_set[0] = *(ptr + 2);
		MeterSetData.c_set[1] = *(ptr + 3);
		MeterSetData.c_set[2] = *(ptr + 4);
		MeterSetData.c_set[3] = *(ptr + 5); //
		MeterSetData.c_set[4] = *(ptr + 6);
		MeterSetData.c_set[5] = *(ptr + 7); //
		MeterSetData.c_set[6] = *(ptr + 8);
		MeterSetData.c_set[7] = *(ptr + 9); //
		ptr_tmp = (u8 *)&(MeterSetData.c_password);  
		SPI3Puts(0x0070,44,ptr_tmp);
		SPI3Puts(0x0470,44,ptr_tmp);
		if(MeterSetData.c_set[AOSET]) Time2Init();
		Cal_AO(0);
    	Cal_AO(1);  
		//电能脉冲配置
		EnPulseEnable();
	}
	else if(0xA766 == com)
	{
		if(Usart2RxTxBuf[0]==MeterSetData.c_com1[0])
		{
			if(0x11 == *(ptr+2)) //清除所有电能、需量和事件记录
			{
				//电能清零部分
				ClrEn();// 电能清除
				ClrDemand(); //需量清除
				ClrMax(); //清除极值
//    			ClrRecord();//清除事件记录	
//   			ClrSysRecord(); //清除系统事件记录
			}					
		}	
	}	
	else if(0xd000 == com) //设置仪表序列号
	{
		for(i=0; i<12; i++)
		{
			MeterSetData.MeterNo[i]=*(ptr + 2+i);	
		}	
		ptr_tmp = (u8 *)&MeterSetData.MeterNo[0];
		SPI3Puts(0x0336,12, ptr_tmp);	
		SPI3Puts(0x0736,12, ptr_tmp);
	}		
	else
	{
		ErrCode = 1;	
	}
	Usart2TxNum = Usart2RxTxNum;
	return ErrCode;	
}

/************************************************
Name: void ReadCoilSta(void)
Function: 继电器状态读取
************************************************/
u8 ReadCoilSta(void)
{
	u8 ErrCode = 0x00;
	u8 midnum, j=0xff;
	u16  addr, num, crc16;
	addr = ((Usart2RxTxBuf[2]<<8)|Usart2RxTxBuf[3]);
	num = ((Usart2RxTxBuf[4]<<8)|Usart2RxTxBuf[5]);
	if((num>=0x01)&&(num<8)) //继电器输出通信设计最多为4路	
	{
		if((addr+num)<0x04)
		{
			midnum = MeterDio.i_Do >> addr;
			j = (j<<num); //不需要的位屏蔽
			midnum &= (~j);
			Usart2RxTxBuf[2] = 0x01;
			Usart2RxTxBuf[3] = midnum;
			midnum = 0x04;
			crc16=Cal_CRC(Usart2RxTxBuf, (midnum));  
			Usart2RxTxBuf[midnum]=(u8)(crc16>>8);  
			Usart2RxTxBuf[midnum+1]= crc16&0xFF;  
			Usart2TxNum = midnum+2; //需要发送的数据个数 
			ErrCode = 0;
		}
		else
		{
			ErrCode = 0x02;
		}		
	}
	else
	{
		ErrCode = 0x03;
	}	
	return (ErrCode);
}

/***********************************************
Name: u8 ReadInputSta(void)
Function: 读取开关量输入状态
***********************************************/
u8 ReadInputSta(void)
{
	u8 ErrCode = 0x00;
	u8 midnum, j=0xff;
	u16  addr, num, crc16;
	addr = ((Usart2RxTxBuf[2]<<8)|Usart2RxTxBuf[3]);
	num = ((Usart2RxTxBuf[4]<<8)|Usart2RxTxBuf[5]);
	if((num>=0x01)&&(num<8)) //继电器输出通信设计最多为8路	
	{
		if((addr+num)<0x08)
		{
			midnum = MeterDio.i_Di >> addr;
			j = (j<<num); //不需要的位屏蔽
			midnum &= (~j);
			Usart2RxTxBuf[2] = 0x01;
			Usart2RxTxBuf[3] = midnum;
			midnum = 0x04;
			crc16=Cal_CRC(Usart2RxTxBuf, (midnum));  
			Usart2RxTxBuf[midnum]=(u8)(crc16>>8);  
			Usart2RxTxBuf[midnum+1]= crc16&0xFF;  
			Usart2TxNum = midnum+2; //需要发送的数据个数 
			ErrCode = 0;
		}
		else
		{
			ErrCode = 0x02;
		}		
	}
	else
	{
		ErrCode = 0x03;
	}	
	return (ErrCode);
}





/************************************************
Name: void UsartRead(void)
Function: 通信读命名 0304
*************************************************/
u8 UsartRead(void)
{
	u8 ErrCode = 0;
	u8 midnum;
	u16  addr, num, crc16;
	u8  *ptr;
	addr = ((Usart2RxTxBuf[2]<<8)|Usart2RxTxBuf[3]);
	num = ((Usart2RxTxBuf[4]<<8)|Usart2RxTxBuf[5]);
	
	if((1<=num)&&(num<=0x7d))
	{	
		if(addr < 0xfffe)
		{		
			ptr = &Usart2RxTxBuf[2];
			midnum = num;
			*ptr++ = (num<<1); //数据长度
			ReadData04(ptr, addr, midnum);
			midnum = (num<<1) + 3;
			crc16=Cal_CRC(Usart2RxTxBuf, (midnum));  
			Usart2RxTxBuf[midnum]=(uint8_t)(crc16>>8);  
			Usart2RxTxBuf[midnum+1]= crc16&0xFF;  
			Usart2TxNum = midnum+2; //需要发送的数据个数 
			ErrCode = 0;
		}
		else
		{
			ErrCode = 0x02;	//地址错误	
		}	
	}
	else
	{
		ErrCode = 0x03; //长度错误	
	}
		
	return (ErrCode);	
}

/*********************************************************************
Name: void ReadData04(u8 *ptr, u16 addr, u16 midnum)
Function: 配置03/04命令中读取的数据

*********************************************************************/
u8 ReadData04(u8 *ptr, u16 addr, u16 midnum)
{
	u16 i;
	S16_REGS_DATA mids16;
	FLOAT_BYTE temp;
	S32_REGS_DATA templ;
	u32 *ptru32;
	s32 *ptrs32;
	s16 *ptrs16;
	u16 *ptru16;
	u8 *ptru8;
	
	for(i=0; i<midnum; i++)
	{
		if(addr<0x06)
		{
			switch(addr)
			{
				case 0x00:
					mids16.bit8[1] = MeterSetData.c_set[DISET];
					mids16.bit8[0] = MeterSetData.c_set[DOSET];
					break;	
				case 0x01:
					mids16.bit8[1] = MeterSetData.c_set[AOSET];
					mids16.bit8[0] = MeterSetData.c_set[COMSET];
					break;	
				case 0x02:
					mids16.bit8[1] = MeterSetData.c_set[COM2SET];
					mids16.bit8[0] = MeterSetData.c_set[TOUSET];
					break;	
				case 0x03:
					mids16.bit8[1] = MeterSetData.c_set[CLASSSET];
					mids16.bit8[0] = MeterSetData.c_set[THDSET];
					break;	
				default:
					mids16.bit16 = 0;
					break;
			}
		} 
		else if(addr<0x0036) //主体测量参数, 为防止数据的不连续性，暂时不简该段写程序 2014-10-17
		{
			if(addr<0x12)
			{
				ptrs32 = &RMS_Un[0];
				temp.floatData = calf_u(*(ptrs32+((addr-0x06)>>1))); //电压数据	
			}
			else if(addr<0x18) temp.floatData = calf_i(RMS_I[(addr-0x12)>>1]);  //三相电流和零序电流
			else if(addr<0x28)
			{
				ptrs32 = &RMS_P[0];
				temp.floatData = calf_p(*(ptrs32+((addr-0x18)>>1))); //功率
			}
			else if(addr<0x2a)
			{
				temp.floatData = calf_p(RMS_S[3]); //总视在功率	
			}
			else if(addr<0x2c) temp.floatData = calf_cf(MeterData.PF[3], 1000); //功率因数
			else if(addr<0x36)
			{
				switch(addr)
				{
					case 0x2c: //频率
					case 0x2d:
						temp.floatData = calf_cf2(RMS_F,1000);
						break;
					case 0x2e: //正向有功电能
					case 0x2f:
						temp.floatData = calf_dn(MeterData.EN[0]);
						break;
					case 0x30:	//反向有功电能
					case 0x31:
						temp.floatData = calf_dn(MeterData.EN[1]);
						break;
					case 0x32: //正向无功电能
					case 0x33:
						temp.floatData = calf_dn(MeterData.EN[2]);
						break;
					case 0x34:	//反向无功电能
					case 0x35:
						temp.floatData = calf_dn(MeterData.EN[3]);
						break;
					default:
						break;				
				}
			}
			if(addr%2) mids16.bit16 = (s16) temp.s16f[0]; //低字
			else mids16.bit16 = (s16) temp.s16f[1];	 //高字
		}
		else if(addr<0x100)
		{
			if(addr<0x3a)
			{
				switch(addr)
				{
					case 0x36:
						mids16.bit16 = MeterDio.i_Do;
						break;	
					case 0x37:
						mids16.bit16 = MeterDio.i_Di;
						break;
					default:
						mids16.bit16 = 0; 
						break;
				}	
			}
			else if(addr<0x3d) //分相功率因数
			{
				ptrs16 = (s16 *)&MeterData.PF[0];
				mids16.bit16 = *(ptrs16+(addr-0x3a));	
			}
			else if(addr<0x46) //电压电流
			{
				ptrs16 = (s16 *)&MeterData.Un[0];
				mids16.bit16 = *(ptrs16+(addr-0x3d));
			}	
			else if(addr<0x52) //功率等其余电量
			{
				ptrs16 = &MeterData.P[0];
				mids16.bit16 = *(ptrs16+(addr-0x46));
			}
			else if(addr<0x53) mids16.bit16 = MeterData.PF[3];
			else if(addr<0x54) mids16.bit16 = MeterData.F;
			else if(addr<0x76) //电能 四象限无功电能,备用电能,//基波双向电能
			{
				ptru32 = &MeterData.EN[0];
				templ.bit32 = *(ptru32+((addr-0x54)>>1));
				if(addr%2) mids16.bit16 = (s16) templ.bit16[0]; //低字
				else mids16.bit16 = (s16) templ.bit16[1];	 //高字		
			}
			else if(addr<0x7e) 	mids16.bit16 = 0;
			else if(addr<0x100) //正向有功复费率电能
			{
				ptru32 = &MonthEnergy[0].l_EPPTou[0][0];
				templ.bit32 = *(ptru32+((addr-0x7e)>>1));
				if(addr%2) mids16.bit16 = (s16) templ.bit16[0]; //低字
				else mids16.bit16 = (s16) templ.bit16[1];	 //高字		
			}	
		}
		else if(addr<0x130) //最大最小值
		{
			ptrs16 = (s16 *)&MeterMax.UnMax[0];
			mids16.bit16 = *(ptrs16+(addr-0x100));	
		}
		else if(addr<0x1c0) mids16.bit16 = 0;
		else if(addr<0x1d8) //需量值
		{
			ptrs16 = (s16 *)&DemandData[0][0];
			mids16.bit16 = *(ptrs16+(addr-0x1c0));
		}
		else if(addr<0x200) mids16.bit16 = 0;	
		else if(addr<0x22c) //电网质量参数1
		{
			ptrs16=(s16 *)&MeterQuality.YU[0];
			mids16.bit16 = *(ptrs16+(addr-0x200));
		}
		
		else if(addr<0x240)
		{
			switch(addr)
			{
				case 0x22c:
				case 0x22d:
					templ.bit32=AlarmState;	
					break;

				case 0x22e:
				case 0x22f:
					templ.bit32 = MeterTimeRecord[0]; //仪表运行时间
					break;
				case 0x230:
				case 0x231:
					templ.bit32 = MeterTimeRecord[1]; //负荷运行时间
					break;
				case 0x232:
				case 0x233:
					templ.bit32 = MeterTimeRecord[2]; //开关量输入使能时仪表运行时间
					break;
				default:
					templ.bit32=0;
					break;	
			}
			if(addr%2) mids16.bit16 = (s16) templ.bit16[0]; //低字
			else mids16.bit16 = (s16) templ.bit16[1];	 //高字		
		}
		else if(addr<0x250) //基波功率部分
		{
			ptrs16 = (s16 *)&MeterQuality.FP[0];
			mids16.bit16 = *(ptrs16+(addr-0x240));
		}
		else if(addr<0x3d6)
		{
			if(addr<0x262)
			{
				ptrs16=(s16 *)&MeterQuality.UTHD[0];
				mids16.bit16 = *(ptrs16+(addr-0x250));					
			}
			else 
			{
				ptrs16 = (s16 *)&HarmonicData[0][0];
				mids16.bit16 = *(ptrs16+(addr-0x262));		
			}
		}
		else if(addr<0x3fc) mids16.bit16 = 0;
		else if(addr<0x400) //变送输出值
		{
			ptrs16 = (s16 *)&AdjustAo[0][0];
			mids16.bit16 = *(ptrs16+(addr-0x3fc));	
		}	
		else if(addr<0x418) //分相电能
		{
			ptru32 = &MeterData.EPP[0];
			templ.bit32 = *(ptru32+((addr-0x400)>>1));	
			if(addr%2) mids16.bit16 = (s16) templ.bit16[0]; //低字
			else mids16.bit16 = (s16) templ.bit16[1];	 //高字	
		}
		else if(addr<0x06b4) mids16.bit16 = 0;	
		else if(addr<0x6b8)  //RTC时钟
		{
			mids16.bit8[1] = sd_Data.c_time[(addr-0x6b4)<<1];	
			mids16.bit8[0] = sd_Data.c_time[1+((addr-0x6b4)<<1)];
		}			
 		else if(addr<0x0700) mids16.bit16 = 0;
		else if(addr<0x0710) 
		{
			ptru8 = (u8 *)&UartAddrCode10[0];
			mids16.bit8[1] = *(ptru8+((addr-0x700)<<1));
			mids16.bit8[0] = *(ptru8+((addr-0x700)<<1)+1);		
		}
		else if(addr<0x0800) mids16.bit16 = 0;
		else if(addr<0x0899) 
		{
			if(addr<0x812)
			{
				switch(addr)
				{
					case 0x801:
						mids16.bit8[1] = MeterSetData.c_lcdgray;
						mids16.bit8[0] = 0;
						break;
					case 0x802:
						mids16.bit8[1] = MeterSetData.c_cyc;
						mids16.bit8[0] = MeterSetData.c_lcdlanguage;
						break;
					case 0x803:
						mids16.bit8[1] = MeterSetData.c_powerview;
						mids16.bit8[0] = MeterSetData.c_lcdlight;
						break;
					case 0x804: // 通信设置
						mids16.bit8[1] = MeterSetData.c_com1[0];
						mids16.bit8[0] = MeterSetData.c_com1[1];
						break;
					case 0x805:
						mids16.bit8[1] = MeterSetData.c_com1[2];
						mids16.bit8[0] = MeterSetData.c_com1[3];
						break;
					case 0x808:
						mids16.bit8[1] = MeterSetData.c_input[0];
						mids16.bit8[0] = MeterSetData.c_input[1];
						break;
					case 0x80A: //二次电压
						mids16.bit16 = MeterSetData.c_pt2;
						break;
					case 0x80B: //二次电流
						mids16.bit16 = MeterSetData.c_ct2;
						break;	
					case 0x80E: //一次侧电压
					case 0x80F:
						templ.bit32 = MeterSetData.c_pt1;
						if(addr%2) mids16.bit16 = templ.bit16[0];
						else mids16.bit16 = templ.bit16[1];
						break;
					case 0x810: //一次侧电流
					case 0x811:
						templ.bit32 = MeterSetData.c_ct1;
						if(addr%2) mids16.bit16 = templ.bit16[0];
						else mids16.bit16 = templ.bit16[1];
						break;
					default:
						mids16.bit16 = 0; 
						break;
						
				}	
			}
			else if(addr<0x816)
			{
				mids16.bit16 = MeterSetData.c_AO[0][addr-0x812];	
			}			
			else if(addr<0x81a) mids16.bit16 = MeterSetData.c_AO[1][addr-0x816];
			else if(addr<0x82c)
			{
				ptrs16 = (s16 *)&MeterSetData.c_DO[0][0];
				mids16.bit16 = *(ptrs16+(addr-0x81a));
			}
			else if(addr<0x82f) //开关量输入设置
			{
				mids16.bit8[1] = MeterSetData.c_DI[(addr-0x82c)<<1];
				mids16.bit8[0] = MeterSetData.c_DI[1+((addr-0x82c)<<1)];	
			}	
			else if(addr<0x834) {}
			else if(addr<0x839) //需量设置和极值间隔
			{
				ptru16 = (u16 *)&MeterSetData.c_demand[0][0];
				mids16.bit16 = *(ptru16+(addr-0x834));
			}
			else if(addr<0x840){}
			else if(addr<0x844) //合格率设置值
			{
				ptru16 = (u16 *)&MeterSetData.c_vgoodvalue[0];	
				mids16.bit16 = *(ptru16+(addr-0x840));	
			}	
			else if(addr<0x86F) //复费率设置
			{
				ptru8 = &sd_Data.c_sd[0][0][0];
				mids16.bit8[1] = *(ptru8+((addr-0x844)<<1));
				mids16.bit8[0] = *(ptru8+((addr-0x844)<<1)+1);	
			}
			else if(addr<0x0899) //报警参数设置
			{
				ptru16 = (u16 *)&MeterSetData.c_AlarmUn[0];
				mids16.bit16 = *(ptru16+(addr-0x86f));
			}	
				 
		}
		else if(addr<0xff00) mids16.bit16 = 0;
		else 
		{
			if(addr<0xff10)
			{
				ptru8 = (u8 *)&UartAddrCode10[0];
				mids16.bit8[1] = *(ptru8+((addr-0xff00)<<1));
				mids16.bit8[0] = *(ptru8+((addr-0xff00)<<1)+1);	
			}			
			else if(addr<0xffe0) mids16.bit16 = 0;
			else if(addr<0xfff0)
			{
				ptru8 = (u8 *)&UartAddrCode11[0];
				mids16.bit8[1] = *(ptru8+((addr-0xffe0)<<1));
				mids16.bit8[0] = *(ptru8+((addr-0xffe0)<<1)+1);		
			}	
			else if(addr<0xfff6)
			{
				ptru8 = (u8 *)&MeterSetData.MeterNo[0];
				mids16.bit8[1] = *(ptru8+((addr-0xfff0)<<1));
				mids16.bit8[0] = *(ptru8+((addr-0xfff0)<<1)+1);		
			}	
			else mids16.bit16 = 0;	
		}	
					
		*(ptr++)= mids16.bit8[1];
		*(ptr++)= mids16.bit8[0];
		
		addr++; //通信地址增加		
	}	
	
	return 0;

}	

/***************************************************
Name: u8 ForceSingleBit(void)
Function: 单个继电器输出设
****************************************************/
u8 ForceSingleBit(void)
{
	u8 ErrCode = 0x00;
	u8 midnum;
	u16  addr, num, crc16;
	addr = ((Usart2RxTxBuf[2]<<8)|Usart2RxTxBuf[3]);
	num = ((Usart2RxTxBuf[4]<<8)|Usart2RxTxBuf[5]);
	if((0x00==num)||(0xff00==num)) //继电器输出通信设计最多为2路	
	{
		if((addr)<0x04) //支持4路继电器出差
		{
			if(0x02 == MeterSetData.c_DO[addr][0])
			{
				if(0xff00 == num) MeterDio.i_Do |= (0x01<<addr);	
				else MeterDio.i_Do &= (0xff-(0x01<<addr));
			}				
			midnum = 0x06;
			crc16=Cal_CRC(Usart2RxTxBuf, (midnum));  
			Usart2RxTxBuf[midnum]=(u8)(crc16>>8);  
			Usart2RxTxBuf[midnum+1]= crc16&0xFF;  
			Usart2TxNum = midnum+2; //需要发送的数据个数 
			ErrCode = 0;
		}
		else
		{
			ErrCode = 0x02;
		}		
	}
	else
	{
		ErrCode = 0x03;
	}	
	return (ErrCode);
	
}

/***************************************************
Name: u8 ForceMultiBit(void)
Function: 继电器输出设置
****************************************************/
u8 ForceMultiBit(void)
{
	u8 ErrCode = 0x00;
	u8 midnum, i;
	u16  addr, num, crc16;
	addr = ((Usart2RxTxBuf[2]<<8)|Usart2RxTxBuf[3]);
	num = ((Usart2RxTxBuf[4]<<8)|Usart2RxTxBuf[5]);
	midnum = Usart2RxTxBuf[7]<<addr;
	if((num>=0x01)&&(num<4)) //继电器输出通信设计最多为4路	
	{
		if((addr+num)<0x04)
		{
			for(i=0;i<num;i++,addr++)
			{
				if(0x02 == MeterSetData.c_DO[addr][0]) //遥控方式	
				{
					MeterDio.i_Do &= (~(1<<addr)); //清除相应的继电器状态位
					MeterDio.i_Do |= (midnum&(1<<addr));	//设置相应的状态位	
				}	
			}
			midnum = 0x06;
			crc16=Cal_CRC(Usart2RxTxBuf, (midnum));  
			Usart2RxTxBuf[midnum]=(u8)(crc16>>8);  
			Usart2RxTxBuf[midnum+1]= crc16&0xFF;  
			Usart2TxNum = midnum+2; //需要发送的数据个数 
			ErrCode = 0;
		}
		else
		{
			ErrCode = 0x02;
		}		
	}
	else
	{
		ErrCode = 0x03;
	}	
	return (ErrCode);
}

/***************************************************
Name: void UsartWrite(u8 UsartS)
Function: 通信写命令06
****************************************************/
u8 UsartWrite06(void)
{
	u8 *ptr;
	u8 ErrCode=0;
	u16  addr, num;
	
	addr = ((Usart2RxTxBuf[2]<<8)|Usart2RxTxBuf[3]);
	num = 1;
	
	if(addr < 0x899)
	{
		ptr = &Usart2RxTxBuf[4];
		ErrCode=WriteData(ptr, addr, num);	
	}
	else
	{
		ErrCode = 0x02;	//地址错误
	}
	
	return(ErrCode);	
}


/***************************************************
Name: void UsartWrite(u8 UsartS)
Function: 通信写命令
****************************************************/
u8 UsartWrite(void)
{
	u8 *ptr;
	u8 ErrCode=0;
	u16 addr, num;	

	addr = ((Usart2RxTxBuf[2]<<8)|Usart2RxTxBuf[3]);
	num = ((Usart2RxTxBuf[4]<<8)|Usart2RxTxBuf[5]);
	
	if(((num>=1)&&(num<=0x78))&&(Usart2RxTxBuf[5]*2 == Usart2RxTxBuf[6]))
	{
		if(addr < 0x899)
		{
			ptr = &Usart2RxTxBuf[7];
			ErrCode=WriteData(ptr, addr, num);
		}
		else
		{
			ErrCode = 0x02;	//地址错误
		}			
	}	
	else
	{
		ErrCode = 0x03; //长度错误	
	}	
	
	
	return(ErrCode);	
}

const u16 AOLIMMIT[4]={4, 27, 10000,10000};
const u16 DOLIMMIT[6]={3, 10000,66,10000,10000,10000,};
const u16 DEMANLIMMITLOW[5]={0, 0, 1, 1, 0}; 
const u16 DEMANLIMMITHIGH[5]={1, 2, 10000, 31, 10000}; 

u8 ComChange=0;
u8 WriteData(u8 *ptr, u16 addr, u16 num)
{
	u16 i, midnum, crc16, midaddr=addr;
	u16 midu16;
	u16 *ptru16;
	S32_REGS_DATA templ;
	u32 midct1=MeterSetData.c_ct1, midpt1=MeterSetData.c_pt1;
	u8 lcdgraychange=0;
	u8 midlanguage = MeterSetData.c_lcdlanguage;
	u8 *ptr_tmp;
	u8 modifyset[7]={0,0,0,0,0,0,0};
	u8 RTCModify=0;
	u8 ErrCode=0;
  midnum = num;
  for(i=0; i<midnum; i++)
	{
		if(addr<0x6b4)
		{	
		}
		else if(addr<0x6b8)
		{
		}	
		else if(addr<0x800)
		{
		}
		else if(addr<0x812)
		{
			switch(addr)
			{
				case 0x801:
					if(*(ptr+2*i)<8) {}
					else ErrCode = 3;	
					if(*(ptr+2*i+1)<4) {}
					else ErrCode = 3;		
						
					break;
				case 0x802: //循环设置和报警值
					if(*(ptr+2*i)<2) {}
					else ErrCode = 3;	
					if(*(ptr+2*i+1)<2) {}
					else ErrCode = 3;	
					break;
				case 0x803: //上电显示
					if(*(ptr+2*i)<(POWERMAX+1)){}
					else ErrCode = 3;	
					break;
				case 0x804: // 通信设置
					if((*(ptr+2*i)!=0)&&(*(ptr+2*i)<248)) {}
					else ErrCode = 3;	
					if(*(ptr+2*i+1)<6){}
					else ErrCode = 3;	
					break;
				case 0x805:
					if(*(ptr+2*i)<4) {}
					else ErrCode = 3;	
					if(*(ptr+2*i+1)<2) {}
					else ErrCode = 3;	
					break;
				case 0x808:
					if(*(ptr+2*i)<3) {}
					else ErrCode = 3;	
					if(*(ptr+2*i+1)<2) {}
					else ErrCode = 3;	
					break;
				case 0x80A: //二次电压
					midu16 = (u16)(*(ptr+2*i));
					midu16= (midu16<<8)+(*(ptr+2*i+1));
					if((midu16>0)&&(midu16<691)) {}
					else ErrCode = 3;	
					break;
				case 0x80B: //二次电流
					midu16 = (u16)(*(ptr+2*i));
					midu16= (midu16<<8)+(*(ptr+2*i+1));
					if((midu16>0)&&(midu16<7)) {}
					else ErrCode = 3;	
					break;	
				case 0x80E: //一次侧电压
				case 0x80F:
					if(addr%2) //低字
					{
						templ.bit8[1] = *(ptr+(i<<1));
						templ.bit8[0] = *(ptr+(i<<1)+1);
						midpt1 &= 0xffff0000;
						templ.bit16[1]=0;
						midpt1 += templ.bit32;
					}
					else //高字
					{
						templ.bit8[3] = *(ptr+(i<<1));
						templ.bit8[2] = *(ptr+(i<<1)+1);
						templ.bit16[0] = 0;
						midpt1 &= (0xffff);
						midpt1 += templ.bit32;
					}
					break;
				case 0x810: //一次侧电流
				case 0x811:
					if(addr%2) //低字
					{
						templ.bit8[1] = *(ptr+(i<<1));
						templ.bit8[0] = *(ptr+(i<<1)+1);
						midct1 &= 0xffff0000;
						templ.bit16[1]=0;
						midct1 += templ.bit32;
					}
					else //高字
					{
						templ.bit8[3] = *(ptr+(i<<1));
						templ.bit8[2] = *(ptr+(i<<1)+1);
						templ.bit16[0] = 0;
						midct1 &= (0xffff);
						midct1 += templ.bit32;
					}
					break;
				default:
					
					break;
			}	

		}	
		else if(addr<0x816) //AO1设置参数
		{
			midu16 = (u16)(*(ptr+2*i));
			midu16 = (midu16<<8)+(*(ptr+2*i+1));
			if(midu16<AOLIMMIT[addr-0x812]) {}
			else ErrCode = 3;	
		}
		else if(addr<0x81a) 
		{
			midu16 = (u16)(*(ptr+2*i));
			midu16 = (midu16<<8)+(*(ptr+2*i+1));
			if(midu16<AOLIMMIT[addr-0x816]) {}
			else ErrCode = 3;	
		}	
		else if(addr<0x832) //继电器输出
		{
			midu16 = (u16)(*(ptr+2*i));
			midu16 = (midu16<<8)+(*(ptr+2*i+1));
			if(midu16<DOLIMMIT[(addr-0x81a)%6]){}
			else ErrCode = 3;	
		}
		else if(addr<0x834) //开关量输入设置
		{

		}	
		else if(addr<0x838) //需量设置和极值间隔
		{
			midu16 = (u16)(*(ptr+2*i));
			midu16 = (midu16<<8)+(*(ptr+2*i+1));
			if((midu16>=DEMANLIMMITLOW[addr-0x834])&&(midu16<DEMANLIMMITHIGH[addr-0x834]))
			{}
			else ErrCode = 3;	
		}
		else if(addr<0x840){}
		else if(addr<0x844) //合格率设置值
		{
			midu16 = (u16)(*(ptr+2*i));
			midu16 = (midu16<<8)+(*(ptr+2*i+1));
			if(midu16<10000)
			{}
			else ErrCode = 3;		
		}
		else if(addr<0x86F) //复费率设置
		{
			ptr_tmp = &sd_Data.c_sd[0][0][0];
			if(addr<0x85c) //复费率时段
			{
				if(*(ptr+2*i)<24) {}
				else ErrCode = 3;	
				if(*(ptr+1+2*i)<60) {}
			else ErrCode = 3;	
			}	
			else if(addr<0x868) //复费率类型
			{
				if(*(ptr+2*i)<4) {}
				else ErrCode = 3;	
				if(*(ptr+1+2*i)<4) {}
				else ErrCode = 3;	
			}	
			else if(addr<0x86E)//月费率选择
			{
				if(*(ptr+2*i)<2) {}
				else ErrCode = 3;	
				if(*(ptr+1+2*i)<2) {}
				else ErrCode = 3;	
			}	
			else
			{
				if((*(ptr+2*i)>0)&&(*(ptr+2*i)<29)) {}
				else ErrCode = 3;	
				if(*(ptr+1+2*i)<24) {}
				else ErrCode = 3;		
			}	
		}	
		else if(addr<0x899) //报警参数
		{
			
			midu16 = (u16)(*(ptr+2*i));
			midu16= (midu16<<8)+(*(ptr+2*i+1));
			ptru16 = (u16 *)&MeterSetData.c_AlarmUn[0];
			if(addr<0x897) //报警值
			{
				if(midu16<10000) {}
				else ErrCode = 3;	
			}	
			else
			{
			}	
		}	
		addr++;	
		if(3==ErrCode) break;
	}
	if((0!=midpt1)&&(midpt1<1000000))	{}
	else ErrCode = 3;	
	if((0!=midct1)&&(midct1<1000000))	{}
	else ErrCode = 3;	
	
  if(3==ErrCode) return (ErrCode);
  
  addr = midaddr;
	midnum = num;
	for(i=0; i<midnum; i++)
	{
		if(addr<0x6b4)
		{	
		}
		else if(addr<0x6b8)
		{
			Time1000ms=0;
			sd_Data.c_time[((addr-0x6b4)<<1)] = *(ptr+2*i);	
			sd_Data.c_time[((addr-0x6b4)<<1)+1] = *(ptr+2*i+1);	
			RTCModify=1;
		}	
		else if(addr<0x800)
		{
		}
		else if(addr<0x812)
		{
			switch(addr)
			{
				case 0x801:
					if(*(ptr+2*i)<8) MeterSetData.c_lcdgray = *(ptr+2*i);
					lcdgraychange=1;
					break;
				case 0x802: //循环设置和报警值
					if(*(ptr+2*i)<2) MeterSetData.c_cyc = *(ptr+2*i);
					if(*(ptr+2*i+1)<2) MeterSetData.c_lcdlanguage = *(ptr+2*i+1);
					break;
				case 0x803: //上电显示
					if(*(ptr+2*i)<(POWERMAX+1))MeterSetData.c_powerview = *(ptr+2*i);
					MeterSetData.c_lcdlight = *(ptr+2*i+1);
					break;
				case 0x804: // 通信设置
					if((*(ptr+2*i)!=0)&&(*(ptr+2*i)<248)) MeterSetData.c_com1[0] = *(ptr+2*i);
					MeterSetData.c_com1[1] = *(ptr+2*i+1);
					ComChange = 1;
					break;
				case 0x805:
					if(*(ptr+2*i)<4) MeterSetData.c_com1[2] = *(ptr+2*i);
					if(*(ptr+2*i+1)<2) MeterSetData.c_com1[3] = *(ptr+2*i+1);
					ComChange = 1;
					break;
				case 0x808:
					if(*(ptr+2*i)<3) MeterSetData.c_input[0] = *(ptr+2*i);
					if(*(ptr+2*i+1)<2) MeterSetData.c_input[1] = *(ptr+2*i+1);
					break;
				case 0x80A: //二次电压
					midu16 = (u16)(*(ptr+2*i));
					midu16= (midu16<<8)+(*(ptr+2*i+1));
					if((midu16>0)&&(midu16<691)) MeterSetData.c_pt2 = midu16;
					break;
				case 0x80B: //二次电流
					midu16 = (u16)(*(ptr+2*i));
					midu16= (midu16<<8)+(*(ptr+2*i+1));
					if((midu16>0)&&(midu16<7)) MeterSetData.c_ct2=midu16;
					break;	
				case 0x80E: //一次侧电压
				case 0x80F:
					if(addr%2) //低字
					{
						templ.bit8[1] = *(ptr+(i<<1));
						templ.bit8[0] = *(ptr+(i<<1)+1);
						midpt1 &= 0xffff0000;
						templ.bit16[1]=0;
						midpt1 += templ.bit32;
					}
					else //高字
					{
						templ.bit8[3] = *(ptr+(i<<1));
						templ.bit8[2] = *(ptr+(i<<1)+1);
						templ.bit16[0] = 0;
						midpt1 &= (0xffff);
						midpt1 += templ.bit32;
					}
					break;
				case 0x810: //一次侧电流
				case 0x811:
					if(addr%2) //低字
					{
						templ.bit8[1] = *(ptr+(i<<1));
						templ.bit8[0] = *(ptr+(i<<1)+1);
						midct1 &= 0xffff0000;
						templ.bit16[1]=0;
						midct1 += templ.bit32;
					}
					else //高字
					{
						templ.bit8[3] = *(ptr+(i<<1));
						templ.bit8[2] = *(ptr+(i<<1)+1);
						templ.bit16[0] = 0;
						midct1 &= (0xffff);
						midct1 += templ.bit32;
					}
					break;
				default:
					
					break;
			}	
			modifyset[0] = 1;			
		}	
		else if(addr<0x816) //AO1设置参数
		{
			midu16 = (u16)(*(ptr+2*i));
			midu16 = (midu16<<8)+(*(ptr+2*i+1));
			if(midu16<AOLIMMIT[addr-0x812]) MeterSetData.c_AO[0][addr-0x812]=midu16;
			modifyset[1] = 1;	
		}
		else if(addr<0x81a) 
		{
			midu16 = (u16)(*(ptr+2*i));
			midu16 = (midu16<<8)+(*(ptr+2*i+1));
			if(midu16<AOLIMMIT[addr-0x816]) MeterSetData.c_AO[1][addr-0x816] = midu16;
			modifyset[1] = 1;	
		}	
		else if(addr<0x82c) //继电器输出
		{
			midu16 = (u16)(*(ptr+2*i));
			midu16 = (midu16<<8)+(*(ptr+2*i+1));
			if(midu16<DOLIMMIT[(addr-0x81a)%6])
			{
				ptru16 = (u16 *)&MeterSetData.c_DO[0][0];
				*(ptru16+(addr-0x81a)) = midu16;
			}	
			modifyset[2] = 1;	
		}
		else if(addr<0x82f) //开关量输入设置
		{
			MeterSetData.c_DI[(addr-0x82c)<<1] = *(ptr+2*i);
			MeterSetData.c_DI[1+((addr-0x82c)<<1)] = *(ptr+2*i+1);	
			modifyset[2] = 1;	
		}	
		else if(addr<0x834) {}
		else if(addr<0x839) //需量设置和极值间隔
		{
			midu16 = (u16)(*(ptr+2*i));
			midu16 = (midu16<<8)+(*(ptr+2*i+1));
			if((midu16>=DEMANLIMMITLOW[addr-0x834])&&(midu16<DEMANLIMMITHIGH[addr-0x834]))
			{
				ptru16 = (u16 *)&MeterSetData.c_demand[0][0];
				*(ptru16+(addr-0x834)) = midu16;
			}	
			modifyset[3] = 1;	
		}
		else if(addr<0x840){}
		else if(addr<0x844) //合格率设置值
		{
			midu16 = (u16)(*(ptr+2*i));
			midu16 = (midu16<<8)+(*(ptr+2*i+1));
			if(midu16<10000)
			{
				ptru16 = (u16 *)&MeterSetData.c_vgoodvalue[0];	
				*(ptru16+(addr-0x840)) = midu16;
				modifyset[4] = 1;	
			}	
		}
		else if(addr<0x86F) //复费率设置
		{
			ptr_tmp = &sd_Data.c_sd[0][0][0];
			if(addr<0x85c) //复费率时段
			{
				if(*(ptr+2*i)<24) *(ptr_tmp+((addr-0x844)<<1)) = *(ptr+2*i);
				if(*(ptr+1+2*i)<60) *(ptr_tmp+((addr-0x844)<<1)+1) = *(ptr+1+2*i);
			}	
			else if(addr<0x868) //复费率类型
			{
				if(*(ptr+2*i)<4) *(ptr_tmp+((addr-0x844)<<1)) = *(ptr+2*i);
				if(*(ptr+1+2*i)<4) *(ptr_tmp+((addr-0x844)<<1)+1) = *(ptr+1+2*i);
			}	
			else if(addr<0x86E)//月费率选择
			{
				if(*(ptr+2*i)<2) *(ptr_tmp+((addr-0x844)<<1)) = *(ptr+2*i);
				if(*(ptr+1+2*i)<2) *(ptr_tmp+((addr-0x844)<<1)+1) = *(ptr+1+2*i);	
			}	
			else
			{
				if((*(ptr+2*i)>0)&&(*(ptr+2*i)<29)) *(ptr_tmp+((addr-0x844)<<1)) = *(ptr+2*i);
				if((*(ptr+1+2*i)<24)) *(ptr_tmp+((addr-0x844)<<1)+1) = *(ptr+1+2*i);	
			}	
			
			modifyset[5] = 1;
		}	
		else if(addr<0x899) //报警参数
		{
			
			midu16 = (u16)(*(ptr+2*i));
			midu16= (midu16<<8)+(*(ptr+2*i+1));
			ptru16 = (u16 *)&MeterSetData.c_AlarmUn[0];
			if(addr<0x897) //报警值
			{
				if(midu16<10000) *(ptru16+(addr-0x86f))=midu16;
			}	
			else
			{
				*(ptru16+(addr-0x86f))=midu16;
			}	
			modifyset[6] = 1;		
		}	
		addr++;	
	}
	//判断数据是否在合理范围内
	if(MeterSetData.c_com1[1]>5) MeterSetData.c_com1[1] = 0x03;
	if(MeterSetData.c_com1[2]>3) MeterSetData.c_com1[2] = 0x00;
	
	ModbusNow = 1;  //防止在存储时条用通信处理程序
	
	if(RTCModify)
	{
		TaskWriteCurTime((u8 *)&sd_Data.c_time[0]);		
//		TASK_START[0] |= Tab_32bit[0][BIT_READRTC];
	}	
	
	//保存设置数据		
	//系统设置
	if(modifyset[0])
	{
		if(midlanguage != MeterSetData.c_lcdlanguage) LCDClrAll();
		if((0!=midpt1)&&(midpt1<1000000))	MeterSetData.c_pt1 = midpt1;
		if((0!=midct1)&&(midct1<1000000))	MeterSetData.c_ct1 = midct1;
		ptr_tmp = (u8 *)&(MeterSetData.c_password);  
		SPI3Puts(0x0070,44,ptr_tmp);
		SPI3Puts(0x0470,44,ptr_tmp);	
		CalPtCt(); //计算变比
		InitDLT645Data();	//初始化DLT645中系统设置数据	
	}
	
	if(modifyset[1])
	{
		ptr_tmp = (u8 *)&MeterSetData.c_AO[0][0];
		SPI3Puts(0x00dc,16,ptr_tmp);
		SPI3Puts(0x04dc,16,ptr_tmp);
	}
	//继电器输出设置
	if(modifyset[2])
	{
		ptr_tmp = (u8 *)&MeterSetData.c_DO[0];
		SPI3Puts(0x00a0,56,ptr_tmp);
		SPI3Puts(0x04a0,56,ptr_tmp);	
		DoFreeJudge();
	}
	if(modifyset[3])
	{
	//需量设置
		ptr_tmp = (u8 *)&MeterSetData.c_demand[0][0];
		SPI3Puts(0x032a,0x0a, ptr_tmp);	
		SPI3Puts(0x072a,0x0a, ptr_tmp);
	}
	if(modifyset[4])
	{
		ptr_tmp = (u8 *)&MeterSetData.c_vgoodvalue[0];
		SPI3Puts(0x035a,8, ptr_tmp);	
		SPI3Puts(0x075a,8, ptr_tmp);
	}
	if(modifyset[5])
	{
		sd_Data.c_sd[0][0][0] =0;
		sd_Data.c_sd[0][0][1] =0;
		sd_Data.c_sd[1][0][0] =0;
		sd_Data.c_sd[1][0][1] =0;
		//复费率时段设置
		ptr_tmp = (u8 *)&sd_Data.c_sd[0][0][0];
		SPI3Puts(0x02ce, 0x54, ptr_tmp);	
		SPI3Puts(0x06ce, 0x54, ptr_tmp);	 
		//抄表日记录，主要是减少存储时间
		ptr_tmp =  (u8 *)&(sd_Data.c_copyday[0]);
		SPI3Puts(0x0324, 0x04, ptr_tmp);	
		SPI3Puts(0x0724, 0x04, ptr_tmp);
		InitDLT645Data();	//初始化DLT645中系统设置数据
	}
	
	if(modifyset[6]) //报警值
	{
		ptr_tmp = (u8 *)&MeterSetData.c_AlarmUn[0];
		SPI3Puts(0x03aa,84, ptr_tmp);	
		SPI3Puts(0x07aa,84, ptr_tmp);	
	}	
	
	midnum = 6;
	crc16=Cal_CRC(Usart2RxTxBuf, (midnum));  
	Usart2RxTxBuf[midnum]=(uint8_t)(crc16>>8);  
	Usart2RxTxBuf[midnum+1]= crc16&0xFF;  
	Usart2TxNum = midnum+2; //需要发送的数据个数 
	
	ModbusNow = 0;  //防止在存储时条用通信处理程序	
	if(lcdgraychange) 
	{
		//重新初始化液晶对比度
		LCDCmd(0x81);   //对比度调节，客户需根据深浅改变下面两个参数。SET VOP
	    if(MeterSetData.c_lcdgray<7) 
	    {
	    	LCDData((u8)LCDGRAY[MeterSetData.c_lcdgray]);  //00--3F  //24
	    	LCDData((u8)(LCDGRAY[MeterSetData.c_lcdgray]>>8));  
	    }
	    else 
	    {
	    	LCDData(0x30);
	    	LCDData(0x04);  //00--05	//04
	    }
	}
	return ErrCode;	
}



/*********************************************************
DLT645通信处理部分

**********************************************************/

//读数据处理
void DLT645DataRead(void)
{
	u8 i, ierr, idlen;
	u16 idname, mididname;
	u8 *p, crcnum;
	//把初始地址赋值给
	Usart2RxTxBuf[8] = 0x81;	//控制码
	idname = Usart2RxTxBuf[11];
	mididname = Usart2RxTxBuf[10];
	idname = ((idname - 0x33)<<0x08)|((mididname-0x33)&0xff);
	//数据转换
	DLTDataChange97();

	//找出对应命名码
	for(i = 0; i < DLTMARKLEN; i++)
	{
		if(idname == DLT645IdName[i])
		{
			ierr = i;
			break;	
		}			
		ierr = 0xff;
	}
	//如果没有符合要求的指令，则不返回命令
	if(0xff == ierr)
	{
		ResetUsart3();
		return;	
	}	
	p = &(DLT645Data.EPP[0]);
	p += DLT645IdAddr[i];
	idlen = DLT645IdLen[i];
	
	Usart2RxTxBuf[9] = idlen + 0x02;	//数据长度
//	Usart2RxTxBuf[10]  //数据标识
//  Usart2RxTxBuf[11] 		
	for(i=0; i<idlen; i++)
	{
		Usart2RxTxBuf[12+i] = *(p+i)+0x33;	
	}
	idlen += 12;
	crcnum = 0x00;
	for(i=0; i<idlen;i++)
	{
		crcnum += Usart2RxTxBuf[i];	
	}
	Usart2RxTxBuf[idlen] = crcnum;
	Usart2RxTxBuf[idlen+1] = 0x16;
	Usart2TxNum = idlen + 2;
	UsartReceiveFinish = 1;
		UsartDelayTime = 0;	//发送初始化
	
}
/********************************************
//DLT645写数据
数据格式：
Re: 68 A0...A5 68 04 L DI0DI1 P0P1 C0C1 N1...Nm CS 16
其中： DI0DI1标识码   P0P1 C0C1: 权限，目前暂时没有使用 


***************************************/
void DLT645DataWrite(void)
{
	u8 i, idlen,*pmidchar;
	u16 idname,mididname;
	u8 middlechar, wherechar;
  	u8 *ptr_tmp; 
	
	Usart2RxTxBuf[8] = 0x84;
	Usart2RxTxBuf[9] = 0x00;
	idname = Usart2RxTxBuf[11];
	mididname = Usart2RxTxBuf[10];
	idname = ((idname-0x33)<<0x08)|((mididname-0x33)&0xff);
	
	idlen = Usart2RxTxBuf[9];	//得到数据长度
	if(idlen > 0x20)
	{
		ResetUsart3();
		return;	
	}	
	//判断需写入的数据
	switch(idname)
	{
		case 0xc010:	//日期		
	//跳过编程权限和编程密码4个byte
			//修改日期
			for(i=0; i<4; i++)
			{
				Usart2RxTxBuf[16+i] = Usart2RxTxBuf[16+i]-0x33;	
				DLT645Data.Data[i] = Usart2RxTxBuf[16+i];
			}
			sd_Data.c_time[6] = (Usart2RxTxBuf[16]&0x0f); //星期
			sd_Data.c_time[2] = (Usart2RxTxBuf[17]&0x0f) + (Usart2RxTxBuf[17]>>0x04)*10;	//日
			sd_Data.c_time[1] = (Usart2RxTxBuf[18]&0x0f) + (Usart2RxTxBuf[18]>>0x04)*10;	//月
			sd_Data.c_time[0] = (Usart2RxTxBuf[19]&0x0f) + (Usart2RxTxBuf[19]>>0x04)*10;	//年
			TaskWriteCurTime(&sd_Data.c_time[0]); 			  	
			break;
		case 0xc011:	//时间			
	//跳过编程权限和编程密码4个byte
			for(i=0; i<3; i++)
			{
				Usart2RxTxBuf[16+i] = Usart2RxTxBuf[16+i]-0x33;
				DLT645Data.Time[i] = Usart2RxTxBuf[16+i];	
			}
			
			sd_Data.c_time[5] = (Usart2RxTxBuf[16]&0x0f) + (Usart2RxTxBuf[16]>>0x04)*10;	//秒
			sd_Data.c_time[4] = (Usart2RxTxBuf[17]&0x0f) + (Usart2RxTxBuf[17]>>0x04)*10;	//分
			sd_Data.c_time[3] = (Usart2RxTxBuf[18]&0x0f) + (Usart2RxTxBuf[18]>>0x04)*10;	//时
			TaskWriteCurTime(&sd_Data.c_time[0]);
			break;
		case 0xc331:	//复费率1
		case 0xc332:	//复费率2
		case 0xc333:	//复费率3
		case 0xc334:	//复费率4
		case 0xc335:	//复费率5
		case 0xc336:	//复费率6
		case 0xc337:	//复费率7
		case 0xc338:	//复费率8
		case 0xc339:	//复费率9
		case 0xc33a:	//复费率10
		case 0xc33b:	//复费率11
		case 0xc33c:	//复费率12
	//跳过编程权限和编程密码4个byte
			middlechar = idname - 0xc331;	//得出费率几
			for(i=0; i<0x03; i++)
			{
				Usart2RxTxBuf[16+i] = Usart2RxTxBuf[16+i]-0x33;	
			}	
			pmidchar = &DLT645Data.TimePeriod1[0];
			pmidchar += 3*middlechar;	//
			*pmidchar++ = Usart2RxTxBuf[16];//赋值给DLT645表中的数据
			*pmidchar++ = Usart2RxTxBuf[17];
			*pmidchar++ = Usart2RxTxBuf[18];
			sd_Data.c_sd[0][middlechar][0] = (Usart2RxTxBuf[18]&0x0f) + (Usart2RxTxBuf[18]>>0x04)*10;
			sd_Data.c_sd[0][middlechar][1] = (Usart2RxTxBuf[17]&0x0f) + (Usart2RxTxBuf[17]>>0x04)*10;
			sd_Data.c_type[0][middlechar] = (Usart2RxTxBuf[16]&0x0f);
			ptr_tmp = (u8 *)&sd_Data.c_sd[0][0][0];
			SPI3Puts(0x02ce, 0x54, ptr_tmp);	
			SPI3Puts(0x06ce, 0x54, ptr_tmp);
			break;	
		case 0xc341:	//复费率1
		case 0xc342:	//复费率2
		case 0xc343:	//复费率3
		case 0xc344:	//复费率4
		case 0xc345:	//复费率5
		case 0xc346:	//复费率6
		case 0xc347:	//复费率7
		case 0xc348:	//复费率8
		case 0xc349:	//复费率9
		case 0xc34a:	//复费率10
		case 0xc34b:	//复费率11
		case 0xc34c:	//复费率12
	//跳过编程权限和编程密码4个byte
			middlechar = idname - 0xc331;	//得出费率几
			for(i=0; i<0x03; i++)
			{
				Usart2RxTxBuf[16+i] = Usart2RxTxBuf[16+i]-0x33;	
			}	
			pmidchar = &DLT645Data.Time2Period1[0];
			pmidchar += 3*middlechar;	//
			*pmidchar++ = Usart2RxTxBuf[16];//赋值给DLT645表中的数据
			*pmidchar++ = Usart2RxTxBuf[17];
			*pmidchar++ = Usart2RxTxBuf[18];
			sd_Data.c_sd[1][middlechar][0] = (Usart2RxTxBuf[18]&0x0f) + (Usart2RxTxBuf[18]>>0x04)*10;
			sd_Data.c_sd[1][middlechar][1] = (Usart2RxTxBuf[17]&0x0f) + (Usart2RxTxBuf[17]>>0x04)*10;
			sd_Data.c_type[1][middlechar] = (Usart2RxTxBuf[16]&0x0f);
			ptr_tmp = (u8 *)&sd_Data.c_sd[0][0][0];
			SPI3Puts(0x02ce, 0x54, ptr_tmp);	
			SPI3Puts(0x06ce, 0x54, ptr_tmp);
			break;
		case 0xc117:	//抄表日
	//跳过编程权限和编程密码4个byte
			middlechar = Usart2RxTxBuf[17]-0x33;//抄表日期
			DLT645Data.MonthPassDay[1] = middlechar;
			sd_Data.c_copyday[0] = (middlechar&0x0f) + (middlechar>>0x04)*10;
			middlechar = Usart2RxTxBuf[16]-0x33;
			DLT645Data.MonthPassDay[0] = middlechar;
			sd_Data.c_copyday[1] = (middlechar&0x0f) + (middlechar>>0x04)*10;
			ptr_tmp =  (u8 *)&(sd_Data.c_copyday[0]);
			SPI3Puts(0x0324, 0x04, ptr_tmp);	
			SPI3Puts(0x0724, 0x04, ptr_tmp);	
			break;
		case 0xc032:	//仪表号 目前支持最大247的地址，其余的为AA
//		case 0xc033:	//用户号
//		case 0xc034:	//设备号	
	//跳过编程权限和编程密码4个byte
			middlechar = Usart2RxTxBuf[16] - 0x33;
			DLT645Data.MeterNum[0] = middlechar;
			DLT645Data.MeterNum[1] = Usart2RxTxBuf[17] - 0x33; 
			MeterSetData.c_com1[0] = (middlechar&0x0f) + (middlechar>>0x04)*10 + (Usart2RxTxBuf[17]-0x33)*100;	
			ptr_tmp = (u8 *)&(MeterSetData.c_password);  
			SPI3Puts(0x0070,44,ptr_tmp);
			SPI3Puts(0x0470,44,ptr_tmp);
			break;
		default:
			ResetUsart3();
			return;
			break;			
	}
	wherechar = 0x00;
	for(i=0; i<9; i++)
	{
		wherechar += Usart2RxTxBuf[i];
	}
	Usart2RxTxBuf[10] = wherechar;
	Usart2RxTxBuf[11] = 0x16;
	Usart2TxNum = 0x0c;
	UsartReceiveFinish = 1;
		UsartDelayTime = 0;

	
}

//广播校时
void DLT645TimeAjust(void)
{
	u8 i;	
	for(i=0; i<6; i++)
	{
		Usart2RxTxBuf[10+i] = Usart2RxTxBuf[10+i]-0x33;	
		Usart2RxTxBuf[10+i] = (Usart2RxTxBuf[10+i]&0x0f) + (Usart2RxTxBuf[10+i]>>0x04)*10;
	}	
	
	sd_Data.c_time[0] = Usart2RxTxBuf[15];	//
	sd_Data.c_time[1] = Usart2RxTxBuf[14];
	sd_Data.c_time[2] = Usart2RxTxBuf[13];
	sd_Data.c_time[3] = Usart2RxTxBuf[12];
	sd_Data.c_time[4] = Usart2RxTxBuf[11];
	sd_Data.c_time[5] = Usart2RxTxBuf[10];
	TaskWriteCurTime(&sd_Data.c_time[0]); 
	//不返回数据
	ResetUsart3();
	return;
}

//DLT645设备地址改变
void DLT645EquipmentAddrChange(void)
{
	u8 i;
	u8 crcnum;
  	u8 *ptr_tmp; 
  	if(KEY_ENTER==KeyValue)	
  	{
  	//有确认键按下，则认为修改该仪表地址
  		Usart2RxTxBuf[0] = 0x68;
  		for(i=0; i<6; i++)
  		{
  			Usart2RxTxBuf[i+1] = Usart2RxTxBuf[i+0x0a] - 0x33;	
  			DLT645Data.MeterNum[i] = Usart2RxTxBuf[i+1];
  		}
  		Usart2RxTxBuf[7] = 0x68;
  		Usart2RxTxBuf[8] = 0x8A;
  		Usart2RxTxBuf[9] = 0x00;
  		//求发送数据的校验和
  		crcnum = 0x00;
  		for(i=0; i<10; i++)
  		{
  			crcnum += Usart2RxTxBuf[i];	
  		}
  		Usart2RxTxBuf[10] = crcnum;
  		Usart2RxTxBuf[11] = 0x16;  //停止符
  		//保存设备地址
		MeterSetData.c_com1[0] = (Usart2RxTxBuf[1]&0x0f) + (Usart2RxTxBuf[1]>>0x04)*10 + Usart2RxTxBuf[2]*100;
		Usart2TxNum = 0x0c;
	  	UsartReceiveFinish = 1;
		UsartDelayTime = 0;
  		ptr_tmp = (u8 *)&(MeterSetData.c_password);  
		SPI3Puts(0x0070,44,ptr_tmp);
		SPI3Puts(0x0470,44,ptr_tmp);
  	}
  	else //无确认键按下，则不修改
  	{
  		ResetUsart3();
  		return;	
  	}
			
}

//改变DLT645通信的Baud, 需要密码
void DLT645BaudChange(void)
{
	u8 i;
	u8 crcnum, m;
  	u8 *ptr_tmp; 	
	i = Usart2RxTxBuf[10] - 0x33;
	switch(i)
	{
		case 0x00:
			m = 0x00;	//1200
			break;
		case 0x10:
			m = 0x01;	//2400
			break;
		case 0x20:
			m = 0x02;	//4800
			break;
		case 0x40:
			m = 0x03;	//9600
			break;
		case 0x80:
			m = 0x04;	//19200
			break;
		default:
			m=0xff;
  			break;				
	}
	if(0xff==m)
	{
		ResetUsart3();
	}
	else
	{
		Usart2RxTxBuf[8] = 0x8C;
		crcnum = 0x00;
	  	for(i=0; i<11; i++)
	  	{
	  		crcnum += Usart2RxTxBuf[i];	
	  	}
	  	Usart2RxTxBuf[11] = crcnum;
	  	Usart2RxTxBuf[12] = 0x16;  //停止符
	  	
	  	Usart2TxNum = 0x0d;
	  	UsartReceiveFinish = 1;
		UsartDelayTime = 0;
	  	
	  	ptr_tmp = (u8 *)&(MeterSetData.c_password);  
		SPI3Puts(0x0070,44,ptr_tmp);
		SPI3Puts(0x0470,44,ptr_tmp);		
		
	}		

}

/***********************************************************************
//DLT645-2007协议
//读数据处理
读数据格式(最基本一种)：
Receive(HEX)：  68 A0 A1 A2 A3 A4 A5 68  11      04    DI0 DI1 DI2 DI3  CS 16 
                   地址域               控制码 数据长度   数据标示
Sned(HEX):     68 A0 A1 A2 A3 A4 A5 68  91      L     DI0 DI1 DI2 DI3 N1..Nm CS 16
                                     控制码 (04+m)                     m   

**********************************************************************/
void DLT645DataRead07(void)
{
	u8 i, ierr, idlen;
	u32 idname, mididname;
	u8 *p, crcnum;
	
	PtResult = (float)MeterSetData.c_pt1/MeterSetData.c_pt2;  //电压变比
	CtResult = (float)MeterSetData.c_ct1/MeterSetData.c_ct2;  //电流变比
	PtCtResult = (float)MeterSetData.c_pt1/MeterSetData.c_pt2*MeterSetData.c_ct1/MeterSetData.c_ct2; //电压电流变比		
	
	//把初始地址赋值给
	Usart2RxTxBuf[8] = 0x91;	//控制码
	idname = Usart2RxTxBuf[11];
	mididname = Usart2RxTxBuf[10];
	idname = (((idname - 0x33)&0xFF)<<0x08)|((mididname-0x33)&0xff);
	mididname = (Usart2RxTxBuf[12]-0x33)&0xff;
	idname |= (mididname << 0x10);
	mididname = ((Usart2RxTxBuf[13]-0x33)&0xff);
	idname |= (mididname << 0x18);

	DLTDataChange07();
	//找出对应命名码
	for(i = 0; i < DLTMARKLEN07; i++)
	{
		if(idname == DLT645IdName07[i])
		{
			ierr = i;
			break;	
		}			
		ierr = 0xff;
	}
	//如果没有符合要求的指令，则不返回命令
	if(0xff == ierr)
	{
		ResetUsart3();
		return;	
	}	
	p = &(DLT645Data.EPP[0]);
	p += DLT645IdAddr07[i];
	idlen = DLT645IdLen07[i];
	
	Usart2RxTxBuf[9] = idlen + 0x04;	//数据长度
//	Usart2RxTxBuf[10]  //数据标识
//  Usart2RxTxBuf[11] 		
	for(i=0; i<idlen; i++)
	{
		Usart2RxTxBuf[14+i] = *(p+i)+0x33;	
	}
	idlen += 14;
		
	crcnum = 0x00;
	for(i=0; i<idlen;i++)		//校验累计从0x68开始
	{
		crcnum += Usart2RxTxBuf[i];	
	}
	Usart2RxTxBuf[idlen] = crcnum;
	Usart2RxTxBuf[idlen+1] = 0x16;
	Usart2TxNum = idlen + 2;
	UsartReceiveFinish = 1;
		UsartDelayTime = 0;	//发送初始化	
}
/********************************************************************
//DLT645-2007写数据 
写数据格式
Receive： 68 A0A1A2A3A4A5 68 14   L    DI0DI1DI3DI4 PA P0P1P2 C0C1C2C3 N1...Nm CS 16
	其中：L= 04 +04(密码) + 04(操作操作者代码) + m(数据长度)
	PA：密码权限 P0P1P2:密码   C0...C4: 操作者代码
    目前设置密码为：01 01 01 01  操作者代码： 02 02 02 02
Send：  68 A0...A5 68 94 00 CS 16  


********************************************************************/
void DLT645DataWrite07(void)
{
	u8 i, idlen,*pmidchar ;
	u32 idname,mididname;
	u8 middlechar, wherechar;
  	u8 *ptr_tmp; 
	
	idlen = Usart2RxTxBuf[9];	//得到数据长度, 该判断无用
	if(idlen > 0x41)	//主要复费率写数据太长
	{
		ResetUsart3();
		return;
	}	
	
	Usart2RxTxBuf[8] = 0x94;
	Usart2RxTxBuf[9] = 0x00;
	idname = Usart2RxTxBuf[11];
	mididname = Usart2RxTxBuf[10];
	idname = (((idname - 0x33)&0xFF)<<0x08)|((mididname-0x33)&0xff);
	mididname = (Usart2RxTxBuf[12]-0x33)&0xff;
	idname |= (mididname << 0x10);
	mididname = ((Usart2RxTxBuf[13]-0x33)&0xff);
	idname |= (mididname << 0x18);
	/*
//判断密码是否正确
	if((0x34 == Usart2RxTxBuf[14])&&(0x34 == Usart2RxTxBuf[15])&&(0x34 == Usart2RxTxBuf[16])&&(0x34 == Usart2RxTxBuf[17]))
	{
	}
	else
	{
		ResetUsart3();
		return;	
	}		
//判断校验码是否正确
	if((0x35 == Usart2RxTxBuf[18])||(0x35 == Usart2RxTxBuf[19])&&(0x35 == Usart2RxTxBuf[20])&&(0x35 == Usart2RxTxBuf[21]))
	{	
	}
	else
	{
		ResetUsart3();
		return;
	}		
	*/
	//判断需写入的数据
	switch(idname)
	{
		case 0x04000101:	//日期		
	//跳过编程权限和编程密码8个byte
			//修改日期
			for(i=0; i<4; i++)
			{
				Usart2RxTxBuf[22+i] = Usart2RxTxBuf[22+i]-0x33;	
				DLT645Data.Data[i] = Usart2RxTxBuf[22+i];
			}
			sd_Data.c_time[6] = (Usart2RxTxBuf[22]&0x0f); //星期
			sd_Data.c_time[2] = (Usart2RxTxBuf[23]&0x0f) + (Usart2RxTxBuf[23]>>0x04)*10;	//日
			sd_Data.c_time[1] = (Usart2RxTxBuf[24]&0x0f) + (Usart2RxTxBuf[24]>>0x04)*10;	//月
			sd_Data.c_time[0] = (Usart2RxTxBuf[25]&0x0f) + (Usart2RxTxBuf[25]>>0x04)*10;	//年
			TaskWriteCurTime(&sd_Data.c_time[0]); 		  	
			break;
		case 0x04000102:	//时间			
	//跳过编程权限和编程密码8个byte
			for(i=0; i<3; i++)
			{
				Usart2RxTxBuf[22+i] = Usart2RxTxBuf[22+i]-0x33;
				DLT645Data.Time[i] = Usart2RxTxBuf[22+i];	
			}
			
			sd_Data.c_time[5] = (Usart2RxTxBuf[22]&0x0f) + (Usart2RxTxBuf[22]>>0x04)*10;	//秒
			sd_Data.c_time[4] = (Usart2RxTxBuf[23]&0x0f) + (Usart2RxTxBuf[23]>>0x04)*10;	//分
			sd_Data.c_time[3] = (Usart2RxTxBuf[24]&0x0f) + (Usart2RxTxBuf[24]>>0x04)*10;	//时
			TaskWriteCurTime(&sd_Data.c_time[0]); 
			break;
		case 0x04010001:	//复费率1-12
	
	//跳过编程权限和编程密码8个byte
			idlen -= 12;	//减去标示码和数据得到需要的数据长度
			
			for(i=0; i<idlen; i++)
			{
				Usart2RxTxBuf[22+i] = Usart2RxTxBuf[22+i]-0x33;	
			}	
			pmidchar = &DLT645Data.TimePeriod1[0];
			idlen = idlen/3;
			if(idlen>12)	//防止非法数据写入
			{
				idlen = 12;	
			}	
			for(i=0; i<idlen; i++)
			{
				*pmidchar++ = Usart2RxTxBuf[22+3*i];//赋值给DLT645表中的数据
				*pmidchar++ = Usart2RxTxBuf[23+3*i];
				*pmidchar++ = Usart2RxTxBuf[24+3*i];
				sd_Data.c_sd[0][i][0] = (Usart2RxTxBuf[24+3*i]&0x0f) + (Usart2RxTxBuf[24+3*i]>>0x04)*10;
				sd_Data.c_sd[0][i][1] = (Usart2RxTxBuf[23+3*i]&0x0f) + (Usart2RxTxBuf[23+3*i]>>0x04)*10;
				sd_Data.c_type[0][i] = (Usart2RxTxBuf[22+3*i]&0x0f);
			}			
			//防止非法数据写入
			sd_Data.c_sd[0][0][0] = 0;
			sd_Data.c_sd[0][0][1] = 0;
			
			ptr_tmp = (u8 *)&sd_Data.c_sd[0][0][0];
			SPI3Puts(0x02ce, 0x54, ptr_tmp);	
			SPI3Puts(0x06ce, 0x54, ptr_tmp);
			
			break; 
		case 0x04020001:	//复费率1-12
	
	//跳过编程权限和编程密码8个byte
			idlen -= 12;	//减去标示码和数据得到需要的数据长度
			
			for(i=0; i<idlen; i++)
			{
				Usart2RxTxBuf[22+i] = Usart2RxTxBuf[22+i]-0x33;	
			}	
			pmidchar = &DLT645Data.TimePeriod1[0];
			idlen = idlen/3;
			if(idlen>12)	//防止非法数据写入
			{
				idlen = 12;	
			}	
			for(i=0; i<idlen; i++)
			{
				*pmidchar++ = Usart2RxTxBuf[22+3*i];//赋值给DLT645表中的数据
				*pmidchar++ = Usart2RxTxBuf[23+3*i];
				*pmidchar++ = Usart2RxTxBuf[24+3*i];
				sd_Data.c_sd[1][i][0] = (Usart2RxTxBuf[24+3*i]&0x0f) + (Usart2RxTxBuf[24+3*i]>>0x04)*10;
				sd_Data.c_sd[1][i][1] = (Usart2RxTxBuf[23+3*i]&0x0f) + (Usart2RxTxBuf[23+3*i]>>0x04)*10;
				sd_Data.c_type[1][i] = (Usart2RxTxBuf[22+3*i]&0x0f);
			}			
			//防止非法数据写入
			sd_Data.c_sd[1][0][0] = 0;
			sd_Data.c_sd[1][0][1] = 0;
			
			ptr_tmp = (u8 *)&sd_Data.c_sd[0][0][0];
			SPI3Puts(0x02ce, 0x54, ptr_tmp);	
			SPI3Puts(0x06ce, 0x54, ptr_tmp);
			
			break;		
		case 0x04000b01:	//抄表日
	//跳过编程权限和编程密码8个byte
			middlechar = Usart2RxTxBuf[23]-0x33;//抄表日期
			DLT645Data.MonthPassDay[1] = middlechar;
			sd_Data.c_copyday[0] = (middlechar&0x0f) + (middlechar>>0x04)*10;
			middlechar = Usart2RxTxBuf[22]-0x33;
			DLT645Data.MonthPassDay[0] = middlechar;
			sd_Data.c_copyday[1] = (middlechar&0x0f) + (middlechar>>0x04)*10;
			
			ptr_tmp =  (u8 *)&(sd_Data.c_copyday[0]);
			SPI3Puts(0x0324, 0x04, ptr_tmp);	
			SPI3Puts(0x0724, 0x04, ptr_tmp);
			
			
			break;
		case 0x04000401:	//仪表号 目前支持最大247的地址，其余的为AA
//		case 0xc033:	//用户号
//		case 0xc034:	//设备号	
	//跳过编程权限和编程密码4个byte
			middlechar = Usart2RxTxBuf[22] - 0x33;
			DLT645Data.MeterNum[0] = middlechar;
			DLT645Data.MeterNum[1] = Usart2RxTxBuf[23] - 0x33; 
			MeterSetData.c_com1[0] = (middlechar&0x0f) + (middlechar>>0x04)*10 + (Usart2RxTxBuf[23]-0x33)*100;
			ptr_tmp = (u8 *)&(MeterSetData.c_password);  
			SPI3Puts(0x0070,44,ptr_tmp);
			SPI3Puts(0x0470,44,ptr_tmp);
			break;
		default:
			ResetUsart3();
			return;
			break;			
	}
	wherechar = 0x00;
	for(i=0; i<9; i++)
	{
		wherechar += Usart2RxTxBuf[i];
	}
	Usart2RxTxBuf[10] = wherechar;
	Usart2RxTxBuf[11] = 0x16;
	Usart2TxNum = 0x0c;
	UsartReceiveFinish = 1;
		UsartDelayTime = 0;	
}

/***********************************************************
DLT645通信地址读取

*************************************************************/
void DLT645AddrRead07(void)
{
	u8 i;
	u8 crcnum;
	Usart2RxTxBuf[0] = 0x68;
	Usart2RxTxBuf[8] = 0x93;	//控制码
	Usart2RxTxBuf[9] = 0x06;	//数据长度
  	for(i=0; i<6; i++)
  	{
  		Usart2RxTxBuf[i+0x01] = DLT645Data.MeterNum[i];
  		Usart2RxTxBuf[i+0x0a] = DLT645Data.MeterNum[i]+0x33;
  	}
  	//求发送数据的校验和
  	crcnum = 0x00;
  	for(i=0; i<16; i++)
  	{
  		crcnum += Usart2RxTxBuf[i];	
  	}
  	Usart2RxTxBuf[16] = crcnum;
  	Usart2RxTxBuf[17] = 0x16;
  	Usart2TxNum = 0x12;
  	UsartReceiveFinish = 1;
		UsartDelayTime = 0;
}

/**********************************************
//DLT645通信地址写入

**********************************************/
void DLT645EquipmentAddrChange07(void)
{

	u8 i;
	u8 crcnum;	
  	u8 *ptr_tmp; 
  	if(KEY_ENTER==KeyValue)	
  	{
  	//有确认键按下，则认为修改该仪表地址
  		Usart2RxTxBuf[0] = 0x68;
  		for(i=0; i<6; i++)
  		{
  			Usart2RxTxBuf[i+1] = Usart2RxTxBuf[i+0x0a] - 0x33;	
  			DLT645Data.MeterNum[i] = Usart2RxTxBuf[i+1];
  		}
  		Usart2RxTxBuf[7] = 0x68;
  		Usart2RxTxBuf[8] = 0x95;
  		Usart2RxTxBuf[9] = 0x00;
  		//求发送数据的校验和
  		crcnum = 0x00;
  		for(i=0; i<10; i++)
  		{
  			crcnum += Usart2RxTxBuf[i];	
  		}
  		Usart2RxTxBuf[10] = crcnum;
  		Usart2RxTxBuf[11] = 0x16;  //停止符
  		//保存设备地址
		MeterSetData.c_com1[0] = (Usart2RxTxBuf[1]&0x0f) + (Usart2RxTxBuf[1]>>0x04)*10 + Usart2RxTxBuf[2]*100;
		Usart2TxNum = 0x0c;
	  	UsartReceiveFinish = 1;
		UsartDelayTime = 0;
  		ptr_tmp = (u8 *)&(MeterSetData.c_password);  
		SPI3Puts(0x0070,44,ptr_tmp);
		SPI3Puts(0x0470,44,ptr_tmp);
  	}
  	else //无确认键按下，则不修改
  	{
  		ResetUsart3();
  		return;	
  	}
			
}

/**************************************************
Name: void DLT645Dealwith(void)
Function: DLT645数据读取
Input:  None
Output: None
***************************************************/
void DLT645Dealwith(void)
{
	//判断命令类型
	switch(Usart2RxTxBuf[8])
	{
		case 0x01: 	//读数据
			DLT645DataRead();
			break;	
		case 0x04:	//写数据，需要密码
			DLT645DataWrite();
			break;	
		case 0x08:	//广播校时, 不需要密码
			DLT645TimeAjust();
			break;	
		case 0x0a:	//更改仪表地址，不需要密码
			DLT645EquipmentAddrChange();
			break;	
		case 0x0c:	//更改通信速度， 不需要密码
			DLT645BaudChange();
			break;	
		//case 0x10:	//需量清零, 暂时不增加改功能
		//
		//	break;
//DLT645-2007协议
		case 0x11:	//读数据
			DLT645DataRead07();
			break;	
		case 0x13:	//读通信地址
			DLT645AddrRead07();
			break;
		case 0x14:	//写数据
			DLT645DataWrite07();
			break;	
		case 0x15:	//更改通信地址
			DLT645EquipmentAddrChange07();
			break;
		default:
			ResetUsart3();
			break;	
	}	
}



/******************************************************************************
名称: float calf_u(u16 tmp_dl)
功能: 计算电压浮点值
******************************************************************************/
float calf_u(s32 tmp_dl)
{
    float rst_dl;
    rst_dl = ((float)tmp_dl)*PtResult/100;	
	
	return(rst_dl);
}
//针对电流为0.1mA
float calf_i(s32 tmp_dl)
{
	 float rst_dl;
	rst_dl = ((float)tmp_dl)*CtResult/10000;
	return(rst_dl);	
}

/******************************************************************************
名称: float calf_p(s32 tmp_pq)
功能: 计算功率数据

*******************************************************************************/
float calf_p(s32 tmp_pq)
{
    float rst_pq;
	rst_pq = ((float)tmp_pq)* PtCtResult/10000;
	return(rst_pq);
}


/******************************************************************************
名称: float calf_dn(u32 tmp_dn)
功能: 电能的float数据

*******************************************************************************/
float calf_dn(u32 tmp_dn)
{
    float rst_dn;
	rst_dn = ((float)tmp_dn) * PtCtResult/1000;
	return(rst_dn);
}
/******************************************************************************
名称: float calf_cf(s16 tmp_cf,s16 tmp_dev)
功能: 计算功率因数和频率等值

*******************************************************************************/
float calf_cf(s16 tmp_cf,s16 tmp_dev)
{
    float rst_cf;
	rst_cf = (((float)tmp_cf)/tmp_dev);
	return(rst_cf);
}

/******************************************************************************
名称: float calf_cf(s16 tmp_cf,s16 tmp_dev)
功能: 计算功率因数和频率等值

*******************************************************************************/
float calf_cf2(s32 tmp_cf,s16 tmp_dev)
{
    float rst_cf;
	rst_cf = (((float)tmp_cf)/tmp_dev);
	return(rst_cf);
}




