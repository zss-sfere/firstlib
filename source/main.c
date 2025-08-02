/********************************************************************************
Program: 电力仪表PD194Z-9HY-LCD的, 用16Kb， 点阵液晶显示
Funtion: 测量3相电压3相电流多电能表，主体程序，进行电压电流功率电能
		 进行2-31次谐波分析，进行复费率电能测量，
外设功能：
USART3：通信
2DI/4DI/6DI
1AO/2AO
3DO
COM1

SPI2： 读取Fram，存储器
SPI1：读取RN7302

PCB: 9HY.X.T3.100 (主板)7
     9H.Y3.101 (下板)
     9H.Y2.104 (扩展板)
     9H.Y1.101 (电源板)

*********************************************************************************/
#include "User_Config.h"
extern void TaskOutput(void);
extern void TaskPowerQuality1(void);
extern void ReadRn7302Energy(void);
extern void TaskInput(void);
extern void TaskAlarmRecord(void);
extern void TaskAlarm(void);
int main()
{
	InitSystem();	//系统初始化
	RN7302Ok=0;
	while(1)
	{
		if(RN7302Ok<10) IWDG_ReloadCounter(); 
		if(0 != TASK_START[0])
		{
			if(TASK_START[0]&Tab_32bit[0][BIT_USART3]) TaskUsart3(); //通信1
			if(TASK_START[0]&Tab_32bit[0][BIT_USART3TX]) Usart3TxInit(); 
			
			if(TASK_START[0]&Tab_32bit[0][BIT_DISPLAYLCD]) TaskLCD(); //显示程序
			if(MeterSetData.c_set[TOUSET])
			{
				if(TASK_START[0]&Tab_32bit[0][BIT_READRTC]) TaskReadCurTime(); //读取RTC	
			}
			
			if(TASK_START[0]&Tab_32bit[0][BIT_SCANKEY]) TaskScanKey(); //按键扫描程序
			
			if(TASK_START[0]&Tab_32bit[0][BIT_KEY])  TaskKey(); //按键处理程序
		}
		
		if(0 != TASK_START[1])
		{
			if(TASK_START[1]&Tab_32bit[0][BIT_INPUT]) TaskInput(); //开关量输入
			if(TASK_START[1]&Tab_32bit[0][BIT_OUTPUT]) TaskOutput(); //继电器输出控制	
			if(TASK_START[1]&Tab_32bit[0][BIT_AOOUT]) TaskAo();	//变送输出
			if(TASK_START[1]&Tab_32bit[0][BIT_DEMAND]) TaskDemand(); //需量最大值
			if(TASK_START[1]&Tab_32bit[0][BIT_UNBLANCE]) TaskPowerQuality1(); //不平衡度等参数计算
			if(TASK_START[1]&Tab_32bit[0][BIT_UPEAK]) TaskPowerQuality2(); //波峰系数，电流K系数等与谐波相关的数据
			
			if(TASK_START[1]&Tab_32bit[0][BIT_ALARM]) TaskAlarm(); //越限报警
		}
		
		if(0 != TASK_START[2])
		{
			if(TASK_START[2]&Tab_32bit[0][BIT_UIMEASURE1]) TaskMeasure();   //计算Rn8302的电压电流功率等电量
			
			if(TASK_START[2]&Tab_32bit[0][BIT_ENERGY]) TaskEnergy();  //电能累计	
											
			if(TASK_START[2]&Tab_32bit[0][BIT_UIREAD1]) ReadRn7302UI(); //电量数据读取
			if(TASK_START[2]&Tab_32bit[0][BIT_ENREAD]) ReadRn7302Energy(); //电能数据读取
			
			if(TASK_START[2]&Tab_32bit[0][BIT_DFTU]) TaskDFTU();	//谐波电压分析			
			
			if(TASK_START[2]&Tab_32bit[0][BIT_DFTI]) TaskDFTI();	//谐波电流分析
			
			if(TASK_START[2]&Tab_32bit[0][BIT_WAVEUREAD]) TaskUIWaveRead(0);	//波形数据读取			
			if(TASK_START[2]&Tab_32bit[0][BIT_WAVEIREAD]) TaskUIWaveRead(1);	//波形数据读取
			if(TASK_START[2]&Tab_32bit[0][BIT_WAVESTART]) TaskWaveStart(); //波形
		}
		
		if(0 != TASK_START[3]) //电量校准
		{
			if(TASK_START[3]&Tab_32bit[0][BIT_CALRESET]) AdjustStart();
			if(TASK_START[3]&Tab_32bit[0][BIT_CALUI]) AdjustGainUI();
			if(TASK_START[3]&Tab_32bit[0][BIT_CALPS]) AdjustGainPS();
			if(TASK_START[3]&Tab_32bit[0][BIT_CALQ]) AdjustGainQ();
			if(TASK_START[3]&Tab_32bit[0][BIT_CALANGLE]) AdjustAngle(); //对电压进行修正
			if(TASK_START[3]&Tab_32bit[0][BIT_CALSAVE]) AdjustSave(); 
			
		}		
	}
}




/***********************************************************
注意 ADDW_HFCONST1
2015-2-6：目前谐波次数为2-31

2015-3-4: 改用低成本芯片，需将stm32f10x.h中部分数据进行更改
#define HSE_STARTUP_TIMEOUT  (uint16_t)0xffff //((uint16_t)0x0500) //< Time out for HSE start up 

2015-6-17: 芯片ST 和GD在调用Delayus时，延时时间长度不一致，
DG是正确的，ST时间会变为2倍； 调用Delayms时，时间都是正确的。原因暂时不确定。
可能会影响液晶显示时的延时。
************************************************************/
/***********************************************************
2017-6-20: 更改Calculate.c中电压电流相关计算，精度达到0.01V 0.1mA

2018/1/23: 软件版本升级为9TY.1002.181A
2018/4/2 : 线电压高报警值默认为789.8，现改为7898， 9TY.1003.184A
2018/9/13：在极值界面中标题最小值显示成最大值，软件版本升级为9TY.1004.189A
2018/12/18: 增加IAP程序, 软件升级为9TY.1005.18CA
2019/8/26: 9TY.1006.190826
2019/8/29: 更改需量极值显示错误，9TY.1007.190829
2019/10/24: 更改一些显示错误，增加通信数据保护
2020/4/15: 增加谐波次数选择, MeterSetData.c_set[]与LNFE一致，
          软件版本升级为9TY.1008.200416
2020/8/21: 按键口初始化错误，应为GPIOA
2021/8/26： 更改默认对比度为2，超出变送范围时，最大输出为25mA，软件版本升级为9TY.1009.210826
2021/9/1 ： 谐波棒图显示错误，软件版本升级为9TY.1010.210901
2021/10/12：RN7302的SPI改为IO模拟，软件版本升级为9TY.1011.211012
2022/2/10 ：波特率支持38400，软件版本升级为9TY.1012.220210
2022/4/7 ： 在主界面增加通信显示符号，9TY.1013.220407 
***********************************************************/

