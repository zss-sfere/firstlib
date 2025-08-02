#include "User_Config.h"

/***************************************************
Name: void NVICConfigure(void)
Function: �ж�����
Input: None
Output: None
***************************************************/
void NVICConfigure(void)
{
	NVIC_InitTypeDef NVIC_InitStr;
	//Enable TIM2 global Interrupt
	NVIC_InitStr.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStr.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStr.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStr.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStr);
	
	//TIM3
	NVIC_InitStr.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStr.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStr.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStr.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStr);
	
	//TIM4 PWM
	NVIC_InitStr.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStr.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStr.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStr.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStr);
	
	//USART3��������
	//�������ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStr.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStr.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStr.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStr.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStr); 

	//ʹ��UART3 Tx DMA�ж�
	NVIC_InitStr.NVIC_IRQChannel = DMA1_Channel2_IRQn;  
	NVIC_InitStr.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStr.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStr.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStr);
	
	
}


/****************************************
Name: void DMAConfigure(void)
Function: DMA����
Input: None
Output: None
******************************************/
void DMAConfigure(void)
{
	DMA_InitTypeDef DMA_InitStr;

	
	//USART3 Tx DMA����
	DMA_DeInit(DMA1_Channel2);
	DMA_InitStr.DMA_PeripheralBaseAddr = 0x40004804; //USART3_BASE��DR�Ĵ���
	DMA_InitStr.DMA_MemoryBaseAddr = (u32)Usart2RxTxBuf;
	DMA_InitStr.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStr.DMA_BufferSize = 20;
	DMA_InitStr.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStr.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStr.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStr.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStr.DMA_Priority = DMA_Priority_High;
	DMA_InitStr.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel2, &DMA_InitStr);	
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE); //ʹ���ж�	
}	
