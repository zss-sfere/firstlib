
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

//系统时钟中断
void SysTick_Handler(void)
{
	//ms累计
	Time1000ms++;
	if(Time1000ms>999)
	{
		Time1000ms = 0;
		if(MeterSetData.c_set[TOUSET])
		{	
			sd_Data.c_time[5]++;
			if((PAGE_MODULE==KeyPage)&&(MODULERTC==DisplayItem))  //时钟显示实时
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
	//延时6ms发送
	if(UsartReceiveFinish)
	{
		UsartDelayTime++;
		if(UsartDelayTime>8)
		{
			UsartDelayTime = 0;
			UsartReceiveFinish=0;
			TASK_START[0]|= Tab_32bit[0][BIT_USART3TX];  //启动通信处理任务	
		}		
	}	
			//发送中断发生30ms后，如果没有结束，则强制结束
		if(TASK_START[0]&Tab_32bit[0][BIT_USART3TXEND]) 
		{
			Usart2TxFinish++;	
			TaskUsart3TxEnd(); //DMA发送完成判断
		}

}


//系统时钟配置 1ms, 暂时不用
void SysTickConfig(void)
{
	SysTick_CounterCmd(SysTick_Counter_Disable); //停止系统定时器
	SysTick_ITConfig(DISABLE); //停止systick中断
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //
	SysTick_SetReload((u32)8999); //1ms(以72MHz为基础)
	SysTick_ITConfig(ENABLE);
	SysTick_CounterCmd(SysTick_Counter_Enable); //停止系统定时器
}


/*******************************************
Name: void RCConfigure(void)
Function: //初始化时钟
*******************************************/

void RCCConfigure(void)
{
	ErrorStatus HSEStatus;	//等待时钟稳定
	RCC_DeInit();	//时钟管理重置
	RCC_HSEConfig(RCC_HSE_ON); //开开外部晶振
	HSEStatus = RCC_WaitForHSEStartUp(); //等待外部时钟就绪	
	if(SUCCESS == HSEStatus)
	{
	//AHB主要负责外部存储器时钟.PB2负责AD,高级TIM1,SPI,EXTI,AFIO, PB1:Uart2/3,SPI2,IIC，IWDG
		RCC_HCLKConfig(RCC_SYSCLK_Div1);	//AHB使用系统时钟
		RCC_PCLK2Config(RCC_HCLK_Div1); //APB2(高速)为HCLK
		RCC_PCLK1Config(RCC_HCLK_Div2); //APB1(低速)为HCLK/2
		
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
		RCC_PLLCmd(ENABLE); //Enable PLL
		while(RESET == RCC_GetFlagStatus(RCC_FLAG_PLLRDY))
		{	
		}
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //设置PLL为系统时钟
		while(0x08  != RCC_GetSYSCLKSource()) //确认PLL为系统时钟
		{
		}
	}	
	else
	{	
	}	
}


/***************************************************
Name: void InitPort(void)
Function: IO进行初始化
Input:  None
Output: None
****************************************************/
void InitPort(void)
{
	GPIO_InitTypeDef GPIO_InitStr;
	//复位GPIO
 	GPIO_DeInit(GPIOA);
 	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC); 
	GPIO_DeInit(GPIOD);
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3,ENABLE); //USART3
	//USART3 IO 配置
	GPIO_InitStr.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStr);	//TX初始化
	GPIO_InitStr.GPIO_Pin = GPIO_Pin_11; 
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStr);  //RX初始化
	
	//通信片选
	GPIO_InitStr.GPIO_Pin = GPIO_USART3CS;
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStr);
	
//SPI1 读取计量芯片RN8302	
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
	
	GPIO_InitStr.GPIO_Pin = (GPIO_RN7302CS1); //片选
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SPI1PORT, &GPIO_InitStr);
	GPIO_SetBits(SPI1PORT, GPIO_RN7302CS1);

//SPI2设置 驱动显示液晶
	GPIO_InitStr.GPIO_Pin = (GPIO_Pin_13|GPIO_Pin_15); //SPI2
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStr);
	
//SPI3
	GPIO_InitStr.GPIO_Pin = (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5); //SPI3
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStr);

//Fram 片选 
	GPIO_InitStr.GPIO_Pin = GPIO_FMCS;
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(FMPORT, &GPIO_InitStr);
	GPIO_SetBits(FMPORT, GPIO_FMCS); //片选拉高
	
//W25	
	GPIO_InitStr.GPIO_Pin = GPIO_W25CS;
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStr);
	GPIO_SetBits(GPIOC, GPIO_W25CS); //片选拉高

//按键输入
	GPIO_InitStr.GPIO_Pin = GPIO_KEYL;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC,&GPIO_InitStr);
	
	GPIO_InitStr.GPIO_Pin = (GPIO_KEYR|GPIO_KEYE|GPIO_KEYM);
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA,&GPIO_InitStr);

//开关量输入
	GPIO_InitStr.GPIO_Pin = (GPIO_DI1|GPIO_DI2|GPIO_DI3|GPIO_DI4);
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB,&GPIO_InitStr);
	GPIO_InitStr.GPIO_Pin = (GPIO_DI5|GPIO_DI6);
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStr);
//继电器输出
	GPIO_InitStr.GPIO_Pin = GPIO_DO1|GPIO_DO2|GPIO_DO3;
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB,&GPIO_InitStr);
	
	GPIO_SetBits(GPIOB,GPIO_DO1);
	GPIO_SetBits(GPIOB,GPIO_DO2);
	GPIO_SetBits(GPIOB,GPIO_DO3);
	
//IIC初始化
 	GPIO_InitStr.GPIO_Pin = GPIO_RTCSCL;
 	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
 	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIOA, &GPIO_InitStr); 	
 	GPIO_InitStr.GPIO_Pin = GPIO_RTCSDA;
 	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
 	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_OD; //开漏模式，实现真正的双向IO功能
 	GPIO_Init(GPIOA, &GPIO_InitStr);		
	//SCL SDA拉高
 	GPIO_SetBits(GPIOA, GPIO_RTCSCL);
 	GPIO_SetBits(GPIOA, GPIO_RTCSDA);

	
//PWM
	GPIO_InitStr.GPIO_Pin = GPIO_PWM1|GPIO_PWM2;
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStr);
	
//液晶控制

	GPIO_InitStr.GPIO_Pin = GPIO_LCDRS|GPIO_LCDRST|GPIO_BACKLIGHT; //片选
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStr);
	
	GPIO_InitStr.GPIO_Pin = GPIO_LCDCS|GPIO_IF2; //
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStr);
	
	
	GPIO_ResetBits(GPIOB, GPIO_IF2); //选择4线SPI
//	GPIO_SetBits(GPIOB, GPIO_IF2); //选择3线SPI
	GPIO_SetBits(GPIOB,GPIO_LCDCS);
	
	GPIO_SetBits(GPIOC, GPIO_BACKLIGHT);

}



/***************************************************************
Function Name: SystemInit
Function: 仪表上电初始化，包括PLL， GPIO，IIC，UART，SPI， FRAM，系统自检等功能
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

	//使能外设时钟，根据实际需要在使能相应的外设时钟，RCC_APB2Periph_AFIO
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO |RCC_APB2Periph_SPI1|RCC_APB2Periph_USART1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //JTAG-DP失能 SW_DP使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4|RCC_APB1Periph_SPI2|RCC_APB1Periph_SPI3|RCC_APB1Periph_USART3, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //DMA1,DMA2时钟使能
	
	NVICConfigure(); //中断使能
	DMAConfigure();	 //DMA初始化
	InitPort(); //IO初始化	
	
	SPI1Init();	//SPI1初始化，连接Rn7302
	SPI2Init(); //显示LCD
	SPI3Init(); //SPI3初始化，读取铁电和DataFlash
	
	Time4Init(); //程序时间片划分
	Time3Init(); //控制RN8302读取间隔	
	//初始化一个对比度
	MeterSetData.c_lcdgray=3;
	//液晶初始化
	InitLCM(); //液晶初始化
	//铁电读取
	InitFram();
	RTCCheck(); // 检验RTC类型
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
	if(MeterSetData.c_set[TOUSET]) TaskReadCurTime(); //读取时钟
	
	if(MeterSetData.c_set[AOSET]) Time2Init(); // 
	IWDG_ReloadCounter();
	
	Uart3Init(); //通信初始化
	
	//初始化计量芯片
	RN7302Init();	//RN8302初始化
	RN7302Init();
	
	IWDG_ReloadCounter();
	//开机延时
	LCDClrAll();
	Printf816(48,10,"loading . ",0);
	//更新显示
	DisplayUpdate();
	IWDG_ReloadCounter();
	Delayms(400);	
	TASK_START[2] |= Tab_32bit[0][BIT_UIREAD1]; 
	ReadRn7302UI(); //电量数据读取
	IWDG_ReloadCounter();
	Delayms(400);	
	TASK_START[2] |= Tab_32bit[0][BIT_UIREAD1]; 
	ReadRn7302UI(); //电量数据读取
	Printf816(48,10,"loading . .",0);
	DisplayUpdate();
	IWDG_ReloadCounter();
	TASK_START[2]|=Tab_32bit[0][BIT_UIMEASURE1];
	TaskMeasure();   //计算Rn8302的电压电流功率等电量
	Printf816(48,10,"loading . . .",0);
	DisplayUpdate();
	IWDG_ReloadCounter();
	Delayms(600);
	LCDClrAll();	
	SysTickConfig(); 
	LCDDisplayInit();	
	if(MeterSetData.c_lcdlight>9) Time2Light = MeterSetData.c_lcdlight - 5;
	InitDLT645Data();	//初始化DLT645中系统设置数据
}



/****************************************************************************
* 函数名称: LCDDisplayInit(void)
* 功    能: LCD显示界面初始化 
* 参    数: 
****************************************************************************/
void LCDDisplayInit(void)
{
	//开机进入显示界面
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

