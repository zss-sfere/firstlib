
#include "User_Config.h"
extern void Time2Init(void);
extern void Time3Init(void);
extern void Time4Init(void);
extern void NVICConfigure(void);
extern void DMAConfigure(void);
extern void InitFram(void);
void LCDDisplayInit(void);
extern void LCDCmd(u8 LCDCommand);
extern void RTCCheck(void);

u8 const POWEREL[3][8]=
{
	{DISPLAY_34UN,DISPLAY_34I,DISPLAY_34P,DISPLAY_EN,DISPLAY_THDTHD,DISPLAY_QUAUWAVE,DISPLAY_DEMANDMAX1,DISPLAY_MAX34UN},
	{DISPLAY_33UL,DISPLAY_33I,DISPLAY_33PQSALL,DISPLAY_EN,DISPLAY_THDTHD,DISPLAY_QUAUWAVE,DISPLAY_DEMANDMAX1,DISPLAY_MAX33UL},
	{DISPLAY_11UN,DISPLAY_11UN,DISPLAY_11PQSALL,DISPLAY_EN,DISPLAY_THDTHD,DISPLAY_QUAUWAVE,DISPLAY_DEMANDMAX1,DISPLAY_MAX11UI},	
};

//ϵͳʱ���ж�
void SysTick_Handler(void)
{
	//ms�ۼ�
	Time1000ms++;
	if(Time1000ms>999)
	{
		Time1000ms = 0;
		if(MeterSetData.c_set[TOUSET])
		{	
			sd_Data.c_time[5]++;
			if((PAGE_MODULE==KeyPage)&&(MODULERTC==DisplayItem))  //ʱ����ʾʵʱ
			{
				TASK_START[0] |= Tab_32bit[0][BIT_DISPLAYLCD];
				LCDUpdate = 0x01;		
			}
			if(sd_Data.c_time[5]>59) 
			{
				sd_Data.c_time[5] = 0;	
				TASK_START[0] |= Tab_32bit[0][BIT_READRTC];
			}
		}
		else TASK_START[0] &= Tab_32bit[1][BIT_READRTC];
	}	
	//��ʱ6ms����
	if(UsartReceiveFinish)
	{
		UsartDelayTime++;
		if(UsartDelayTime>8)
		{
			UsartDelayTime = 0;
			UsartReceiveFinish=0;
			TASK_START[0]|= Tab_32bit[0][BIT_USART3TX];  //����ͨ�Ŵ�������	
		}		
	}	
			//�����жϷ���30ms�����û�н�������ǿ�ƽ���
		if(TASK_START[0]&Tab_32bit[0][BIT_USART3TXEND]) 
		{
			Usart2TxFinish++;	
			TaskUsart3TxEnd(); //DMA��������ж�
		}

}


//ϵͳʱ������ 1ms, ��ʱ����
void SysTickConfig(void)
{
	SysTick_CounterCmd(SysTick_Counter_Disable); //ֹͣϵͳ��ʱ��
	SysTick_ITConfig(DISABLE); //ֹͣsystick�ж�
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //
	SysTick_SetReload((u32)8999); //1ms(��72MHzΪ����)
	SysTick_ITConfig(ENABLE);
	SysTick_CounterCmd(SysTick_Counter_Enable); //ֹͣϵͳ��ʱ��
}


/*******************************************
Name: void RCConfigure(void)
Function: //��ʼ��ʱ��
*******************************************/

void RCCConfigure(void)
{
	ErrorStatus HSEStatus;	//�ȴ�ʱ���ȶ�
	RCC_DeInit();	//ʱ�ӹ�������
	RCC_HSEConfig(RCC_HSE_ON); //�����ⲿ����
	HSEStatus = RCC_WaitForHSEStartUp(); //�ȴ��ⲿʱ�Ӿ���	
	if(SUCCESS == HSEStatus)
	{
	//AHB��Ҫ�����ⲿ�洢��ʱ��.PB2����AD,�߼�TIM1,SPI,EXTI,AFIO, PB1:Uart2/3,SPI2,IIC��IWDG
		RCC_HCLKConfig(RCC_SYSCLK_Div1);	//AHBʹ��ϵͳʱ��
		RCC_PCLK2Config(RCC_HCLK_Div1); //APB2(����)ΪHCLK
		RCC_PCLK1Config(RCC_HCLK_Div2); //APB1(����)ΪHCLK/2
		
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
		RCC_PLLCmd(ENABLE); //Enable PLL
		while(RESET == RCC_GetFlagStatus(RCC_FLAG_PLLRDY))
		{	
		}
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //����PLLΪϵͳʱ��
		while(0x08  != RCC_GetSYSCLKSource()) //ȷ��PLLΪϵͳʱ��
		{
		}
	}	
	else
	{	
	}	
}


/***************************************************
Name: void InitPort(void)
Function: IO���г�ʼ��
Input:  None
Output: None
****************************************************/
void InitPort(void)
{
	GPIO_InitTypeDef GPIO_InitStr;
	//��λGPIO
 	GPIO_DeInit(GPIOA);
 	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC); 
	GPIO_DeInit(GPIOD);
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3,ENABLE); //USART3
	//USART3 IO ����
	GPIO_InitStr.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOC, &GPIO_InitStr);	//TX��ʼ��
	GPIO_InitStr.GPIO_Pin = GPIO_Pin_11; 
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������
	GPIO_Init(GPIOC, &GPIO_InitStr);  //RX��ʼ��
	
	//ͨ��Ƭѡ
	GPIO_InitStr.GPIO_Pin = GPIO_USART3CS;
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStr);
	
//SPI1 ��ȡ����оƬRN8302	
/*
	GPIO_InitStr.GPIO_Pin = (GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7); //SPI1
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStr);
*/
  GPIO_InitStr.GPIO_Pin = (GPIO_Pin_5|GPIO_Pin_7); //SPI1
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStr);
	GPIO_InitStr.GPIO_Pin = (GPIO_Pin_6); //SPI1
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStr);
	
	GPIO_InitStr.GPIO_Pin = (GPIO_RN7302CS1); //Ƭѡ
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SPI1PORT, &GPIO_InitStr);
	GPIO_SetBits(SPI1PORT, GPIO_RN7302CS1);

//SPI2���� ������ʾҺ��
	GPIO_InitStr.GPIO_Pin = (GPIO_Pin_13|GPIO_Pin_15); //SPI2
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStr);
	
//SPI3
	GPIO_InitStr.GPIO_Pin = (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5); //SPI3
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStr);

//Fram Ƭѡ 
	GPIO_InitStr.GPIO_Pin = GPIO_FMCS;
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(FMPORT, &GPIO_InitStr);
	GPIO_SetBits(FMPORT, GPIO_FMCS); //Ƭѡ����
	
//W25	
	GPIO_InitStr.GPIO_Pin = GPIO_W25CS;
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStr);
	GPIO_SetBits(GPIOC, GPIO_W25CS); //Ƭѡ����

//��������
	GPIO_InitStr.GPIO_Pin = GPIO_KEYL;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC,&GPIO_InitStr);
	
	GPIO_InitStr.GPIO_Pin = (GPIO_KEYR|GPIO_KEYE|GPIO_KEYM);
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA,&GPIO_InitStr);

//����������
	GPIO_InitStr.GPIO_Pin = (GPIO_DI1|GPIO_DI2|GPIO_DI3|GPIO_DI4);
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB,&GPIO_InitStr);
	GPIO_InitStr.GPIO_Pin = (GPIO_DI5|GPIO_DI6);
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStr);
//�̵������
	GPIO_InitStr.GPIO_Pin = GPIO_DO1|GPIO_DO2|GPIO_DO3;
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB,&GPIO_InitStr);
	
	GPIO_SetBits(GPIOB,GPIO_DO1);
	GPIO_SetBits(GPIOB,GPIO_DO2);
	GPIO_SetBits(GPIOB,GPIO_DO3);
	
//IIC��ʼ��
 	GPIO_InitStr.GPIO_Pin = GPIO_RTCSCL;
 	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
 	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIOA, &GPIO_InitStr); 	
 	GPIO_InitStr.GPIO_Pin = GPIO_RTCSDA;
 	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
 	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_OD; //��©ģʽ��ʵ��������˫��IO����
 	GPIO_Init(GPIOA, &GPIO_InitStr);		
	//SCL SDA����
 	GPIO_SetBits(GPIOA, GPIO_RTCSCL);
 	GPIO_SetBits(GPIOA, GPIO_RTCSDA);

	
//PWM
	GPIO_InitStr.GPIO_Pin = GPIO_PWM1|GPIO_PWM2;
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStr);
	
//Һ������

	GPIO_InitStr.GPIO_Pin = GPIO_LCDRS|GPIO_LCDRST|GPIO_BACKLIGHT; //Ƭѡ
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStr);
	
	GPIO_InitStr.GPIO_Pin = GPIO_LCDCS|GPIO_IF2; //
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStr);
	
	
	GPIO_ResetBits(GPIOB, GPIO_IF2); //ѡ��4��SPI
//	GPIO_SetBits(GPIOB, GPIO_IF2); //ѡ��3��SPI
	GPIO_SetBits(GPIOB,GPIO_LCDCS);
	
	GPIO_SetBits(GPIOC, GPIO_BACKLIGHT);

}



/***************************************************************
Function Name: SystemInit
Function: �Ǳ��ϵ��ʼ��������PLL�� GPIO��IIC��UART��SPI�� FRAM��ϵͳ�Լ�ȹ���
Input:  None
Output: None

*****************************************************************/

void InitSystem(void)
{
	RCCConfigure(); ////PLL	

	//WatchDog	Enable
	RCC_LSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)==RESET);
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_64);  
	IWDG_SetReload(1600); //2.5s
	IWDG_ReloadCounter();
	IWDG_Enable();

	//ʹ������ʱ�ӣ�����ʵ����Ҫ��ʹ����Ӧ������ʱ�ӣ�RCC_APB2Periph_AFIO
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO |RCC_APB2Periph_SPI1|RCC_APB2Periph_USART1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //JTAG-DPʧ�� SW_DPʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4|RCC_APB1Periph_SPI2|RCC_APB1Periph_SPI3|RCC_APB1Periph_USART3, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //DMA1,DMA2ʱ��ʹ��
	
	NVICConfigure(); //�ж�ʹ��
	DMAConfigure();	 //DMA��ʼ��
	InitPort(); //IO��ʼ��	
	
	SPI1Init();	//SPI1��ʼ��������Rn7302
	SPI2Init(); //��ʾLCD
	SPI3Init(); //SPI3��ʼ������ȡ�����DataFlash
	
	Time4Init(); //����ʱ��Ƭ����
	Time3Init(); //����RN8302��ȡ���	
	//��ʼ��һ���Աȶ�
	MeterSetData.c_lcdgray=3;
	//Һ����ʼ��
	InitLCM(); //Һ����ʼ��
	//�����ȡ
	InitFram();
	RTCCheck(); // ����RTC����
	//���³�ʼ��Һ���Աȶ�
	LCDCmd(0x81);   //�Աȶȵ��ڣ��ͻ��������ǳ�ı���������������SET VOP
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
	if(MeterSetData.c_set[TOUSET]) TaskReadCurTime(); //��ȡʱ��
	
	if(MeterSetData.c_set[AOSET]) Time2Init(); // 
	IWDG_ReloadCounter();
	
	Uart3Init(); //ͨ�ų�ʼ��
	
	//��ʼ������оƬ
	RN7302Init();	//RN8302��ʼ��
	RN7302Init();
	
	IWDG_ReloadCounter();
	//������ʱ
	LCDClrAll();
	Printf816(48,10,"loading . ",0);
	//������ʾ
	DisplayUpdate();
	IWDG_ReloadCounter();
	Delayms(400);	
	TASK_START[2] |= Tab_32bit[0][BIT_UIREAD1]; 
	ReadRn7302UI(); //�������ݶ�ȡ
	IWDG_ReloadCounter();
	Delayms(400);	
	TASK_START[2] |= Tab_32bit[0][BIT_UIREAD1]; 
	ReadRn7302UI(); //�������ݶ�ȡ
	Printf816(48,10,"loading . .",0);
	DisplayUpdate();
	IWDG_ReloadCounter();
	TASK_START[2]|=Tab_32bit[0][BIT_UIMEASURE1];
	TaskMeasure();   //����Rn8302�ĵ�ѹ�������ʵȵ���
	Printf816(48,10,"loading . . .",0);
	DisplayUpdate();
	IWDG_ReloadCounter();
	Delayms(600);
	LCDClrAll();	
	SysTickConfig(); 
	LCDDisplayInit();	
	if(MeterSetData.c_lcdlight>9) Time2Light = MeterSetData.c_lcdlight - 5;
	InitDLT645Data();	//��ʼ��DLT645��ϵͳ��������
}



/****************************************************************************
* ��������: LCDDisplayInit(void)
* ��    ��: LCD��ʾ�����ʼ�� 
* ��    ��: 
****************************************************************************/
void LCDDisplayInit(void)
{
	//����������ʾ����
	LCDUpdate= 1;
	KeyMainmenu = 0;
	if(MeterSetData.c_powerview>7) MeterSetData.c_powerview=0;	//2018/6/28 15:11:44
	if(MeterSetData.c_powerview<3)
	{
		KeyPage = PAGE_EL;
		DisplayItem = POWEREL[MeterSetData.c_input[0]][MeterSetData.c_powerview];
	}
	else if(MeterSetData.c_powerview<8)
	{
		KeyPage = PAGE_EN+(MeterSetData.c_powerview-3);
		DisplayItem = POWEREL[MeterSetData.c_input[0]][MeterSetData.c_powerview];
	}
	else
	{
		KeyPage = PAGE_EL;
		DisplayItem = POWEREL[MeterSetData.c_input[0]][0];	
	}
	  
}

