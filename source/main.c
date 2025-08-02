/********************************************************************************
Program: �����Ǳ�PD194Z-9HY-LCD��, ��16Kb�� ����Һ����ʾ
Funtion: ����3���ѹ3���������ܱ�������򣬽��е�ѹ�������ʵ���
		 ����2-31��г�����������и����ʵ��ܲ�����
���蹦�ܣ�
USART3��ͨ��
2DI/4DI/6DI
1AO/2AO
3DO
COM1

SPI2�� ��ȡFram���洢��
SPI1����ȡRN7302

PCB: 9HY.X.T3.100 (����)7
     9H.Y3.101 (�°�)
     9H.Y2.104 (��չ��)
     9H.Y1.101 (��Դ��)

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
	InitSystem();	//ϵͳ��ʼ��
	RN7302Ok=0;
	while(1)
	{
		if(RN7302Ok<10) IWDG_ReloadCounter(); 
		if(0 != TASK_START[0])
		{
			if(TASK_START[0]&Tab_32bit[0][BIT_USART3]) TaskUsart3(); //ͨ��1
			if(TASK_START[0]&Tab_32bit[0][BIT_USART3TX]) Usart3TxInit(); 
			
			if(TASK_START[0]&Tab_32bit[0][BIT_DISPLAYLCD]) TaskLCD(); //��ʾ����
			if(MeterSetData.c_set[TOUSET])
			{
				if(TASK_START[0]&Tab_32bit[0][BIT_READRTC]) TaskReadCurTime(); //��ȡRTC	
			}
			
			if(TASK_START[0]&Tab_32bit[0][BIT_SCANKEY]) TaskScanKey(); //����ɨ�����
			
			if(TASK_START[0]&Tab_32bit[0][BIT_KEY])  TaskKey(); //�����������
		}
		
		if(0 != TASK_START[1])
		{
			if(TASK_START[1]&Tab_32bit[0][BIT_INPUT]) TaskInput(); //����������
			if(TASK_START[1]&Tab_32bit[0][BIT_OUTPUT]) TaskOutput(); //�̵����������	
			if(TASK_START[1]&Tab_32bit[0][BIT_AOOUT]) TaskAo();	//�������
			if(TASK_START[1]&Tab_32bit[0][BIT_DEMAND]) TaskDemand(); //�������ֵ
			if(TASK_START[1]&Tab_32bit[0][BIT_UNBLANCE]) TaskPowerQuality1(); //��ƽ��ȵȲ�������
			if(TASK_START[1]&Tab_32bit[0][BIT_UPEAK]) TaskPowerQuality2(); //����ϵ��������Kϵ������г����ص�����
			
			if(TASK_START[1]&Tab_32bit[0][BIT_ALARM]) TaskAlarm(); //Խ�ޱ���
		}
		
		if(0 != TASK_START[2])
		{
			if(TASK_START[2]&Tab_32bit[0][BIT_UIMEASURE1]) TaskMeasure();   //����Rn8302�ĵ�ѹ�������ʵȵ���
			
			if(TASK_START[2]&Tab_32bit[0][BIT_ENERGY]) TaskEnergy();  //�����ۼ�	
											
			if(TASK_START[2]&Tab_32bit[0][BIT_UIREAD1]) ReadRn7302UI(); //�������ݶ�ȡ
			if(TASK_START[2]&Tab_32bit[0][BIT_ENREAD]) ReadRn7302Energy(); //�������ݶ�ȡ
			
			if(TASK_START[2]&Tab_32bit[0][BIT_DFTU]) TaskDFTU();	//г����ѹ����			
			
			if(TASK_START[2]&Tab_32bit[0][BIT_DFTI]) TaskDFTI();	//г����������
			
			if(TASK_START[2]&Tab_32bit[0][BIT_WAVEUREAD]) TaskUIWaveRead(0);	//�������ݶ�ȡ			
			if(TASK_START[2]&Tab_32bit[0][BIT_WAVEIREAD]) TaskUIWaveRead(1);	//�������ݶ�ȡ
			if(TASK_START[2]&Tab_32bit[0][BIT_WAVESTART]) TaskWaveStart(); //����
		}
		
		if(0 != TASK_START[3]) //����У׼
		{
			if(TASK_START[3]&Tab_32bit[0][BIT_CALRESET]) AdjustStart();
			if(TASK_START[3]&Tab_32bit[0][BIT_CALUI]) AdjustGainUI();
			if(TASK_START[3]&Tab_32bit[0][BIT_CALPS]) AdjustGainPS();
			if(TASK_START[3]&Tab_32bit[0][BIT_CALQ]) AdjustGainQ();
			if(TASK_START[3]&Tab_32bit[0][BIT_CALANGLE]) AdjustAngle(); //�Ե�ѹ��������
			if(TASK_START[3]&Tab_32bit[0][BIT_CALSAVE]) AdjustSave(); 
			
		}		
	}
}




/***********************************************************
ע�� ADDW_HFCONST1
2015-2-6��Ŀǰг������Ϊ2-31

2015-3-4: ���õͳɱ�оƬ���轫stm32f10x.h�в������ݽ��и���
#define HSE_STARTUP_TIMEOUT  (uint16_t)0xffff //((uint16_t)0x0500) //< Time out for HSE start up 

2015-6-17: оƬST ��GD�ڵ���Delayusʱ����ʱʱ�䳤�Ȳ�һ�£�
DG����ȷ�ģ�STʱ����Ϊ2���� ����Delaymsʱ��ʱ�䶼����ȷ�ġ�ԭ����ʱ��ȷ����
���ܻ�Ӱ��Һ����ʾʱ����ʱ��
************************************************************/
/***********************************************************
2017-6-20: ����Calculate.c�е�ѹ������ؼ��㣬���ȴﵽ0.01V 0.1mA

test git 
***********************************************************/

