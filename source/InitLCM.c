#include "User_Config.h"
#include "WordLibrary.h"

const u16 LCDGRAY[10] = {0x410,0x414,0x418,0x41c,0x420,0x424,0x428,0x430,0x434,0x438};

void Delay_ms(u16 n)
{
    u16 i,j;
    for(j=0;j<n;j++)
    {
        for(i=0;i<2;i++)
        {
            ;
        }
    }
}
/****************************************
Name: void LCDCmd(u8 command)
Function: 液晶指令代码传送函数
Input: LCDCommand, 液晶指令码
Output: None
*****************************************/
void LCDCmd(u8 LCDCommand)
{
	GPIO_ResetBits(GPIOB, GPIO_LCDCS);  //片选拉低	
    GPIO_ResetBits(GPIOC, GPIO_LCDRS);    
	SPI2SendByte(LCDCommand);
	GPIO_SetBits(GPIOB,GPIO_LCDCS);   //片选拉高
}

/********************************************
Name: void LCDData(u8 LCDData)
Function: 液晶数据传送函数
Input:  LCDData 液晶数据
Output: None
*********************************************/
void LCDData(u8 LCDData)
{
	GPIO_ResetBits(GPIOB, GPIO_LCDCS); //片选拉低
	GPIO_SetBits(GPIOC, GPIO_LCDRS);
	SPI2SendByte(LCDData);
	GPIO_SetBits(GPIOB, GPIO_LCDCS); //片选拉高
}

void ClearLcd(void)
{
	s16 i,j;
	LCDCmd(0x30);   //扩展指令1
	LCDCmd(0x15);   //column  address set
	LCDData(0x00);   //seg256
	LCDData(0xef);   //~seg512
	LCDCmd(0x75);   // page address set
	LCDData(0x00);   //0x00
	LCDData(0x13);   //~0x20
	LCDCmd(0x5c);
    
	for(i=0;i<20;i++)
	{
        for(j=0;j<240;j++)
        {
            LCDData(0x00);
        }
	}
}

/*******************************************************
Name: void InitLCM(void)
Function: 液晶驱动芯片UC1611初始化
Input:  None
Output: None
*******************************************************/
void InitLCM(void)
{
	//液晶驱动芯片复位
	GPIO_SetBits(GPIOC, GPIO_LCDRST);
	Delay_ms(20); //
	GPIO_ResetBits(GPIOC, GPIO_LCDRST);
	Delay_ms(6000); //
	GPIO_SetBits(GPIOC, GPIO_LCDRST);
	Delay_ms(6000);
	
	LCDCmd(0X30); //扩展指令1
    LCDCmd(0X6E);  //单个IC,主控。30.
   
    LCDCmd(0x31); //扩展指令2
    LCDCmd(0XD7); //Disable Auto Read 35.
    LCDData(0x9f); ///Extension Command
    
    LCDCmd(0xe0); //Enable OTP Read 36
    LCDData(0x00); ///Extension Command
    
    
    Delay_ms(2000);
    LCDCmd(0xe3);   //OTP Up-Load
    Delay_ms(2000);
    LCDCmd(0xe1);   //OTP Control Out
				
    LCDCmd(0x30);   //扩展指令1
    
    LCDCmd(0x94);   //唤醒
    LCDCmd(0xae);   //显示关。
    Delay_ms(5000);
    LCDCmd(0x20);   //电源控制
    LCDData(0x0b);  //开内部DC--DC
    LCDCmd(0x81);   //对比度调节，客户需根据深浅改变下面两个参数。SET VOP
    if(MeterSetData.c_lcdgray<7) 
    {
    	LCDData((u8)LCDGRAY[MeterSetData.c_lcdgray]);  //00--3F  //24
    	LCDData((u8)(LCDGRAY[MeterSetData.c_lcdgray]>>8));  
//			LCDData(0x36);
//    	LCDData(0x04);  //00--05	//04
    }
    else 
    {
    	LCDData(0x30);
    	LCDData(0x04);  //00--05	//04
    }

    LCDCmd(0x31);   	// 扩展指令2
    
    LCDCmd(0x32);   //Analog Circuit Set
    LCDData(0x00);  //
    //LCDData(0x01);  //6KHz
    //LCDData(0x04);  //Bias=1/10 // 01 1/13
    LCDData(0x01);  //6KHz
    LCDData(0x01);  //Bias=1/10 // 01 1/13
    LCDCmd(0x51);   //DC-DC倍数 	
    //LCDData(0xfb);  //10倍压		//fa
    LCDData(0xfa);  //10倍压
    
    
    LCDCmd(0x30);   //扩展指令1
			
    LCDCmd(0xf0);   //显示模式
    LCDData(0x10);  //单色模式，无灰度级别。
		
    LCDCmd(0x0C);   //显示模式    //D0~D7  LSB on top
        
    LCDCmd(0xca);   //显示控制
    LCDData(0x00);  //
    LCDData(0x9f);  //1/64DUTY, 64行为例。		
    LCDData(0x00);  //

    LCDCmd(0x31);   	// 扩展指令2

    LCDCmd(0xf2);  //Temperature Range
    LCDData(0x1E);  // TA=-10
    LCDData(0x28);  // TB=0
    LCDData(0x32);  // TC=10


    LCDCmd(0xf0);   //Frame rate
    LCDData(0x16);  //
    LCDData(0x16);  //
    LCDData(0x16);  // 0X18
    LCDData(0x16);  //102Hz

    LCDCmd(0x30);   //
    LCDCmd(0xbc);   //行，列，光标扫描方向
    LCDData(0x00);  //

    LCDCmd(0xa6);  //RAM正常显示。0灭，1亮。

    LCDCmd(0x31);   //Extension Command2
    LCDCmd(0x40);   //Internal Power Supply
    ClearLcd();
    LCDCmd(0xaf);  //Display ON
    
    DisplayUpdate();
	
}

/**********************************************************
Name: void DisplayUpdate(void)
Function: 更新显示LCD
Input： None
Output: None
**********************************************************/

void DisplayUpdate(void)
{
	u16 i,j;

	LCDCmd(0x30);   //扩展指令1
	LCDCmd(0x15);   //column  address set
	LCDData(0x00);   //seg256
	LCDData(0xef);   //~seg512
	LCDCmd(0x75);   // page address set
	LCDData(0x00);   //0x00
	LCDData(0x13);   //~0x20
	LCDCmd(0x5c);
    
	for(i=0;i<20;i++)
	{
        for(j=0;j<240;j++)
        {
            LCDData(DISP_RAM[i][j]);
        }
        if(TASK_START[0]&Tab_32bit[0][BIT_USART3]) TaskUsart3(); //通信1
		if(TASK_START[0]&Tab_32bit[0][BIT_USART3TX]) Usart3TxInit(); 
	}
}

/**********************************************************
Name: void LCDClrAll(void)
Function: 清空显示器的RAM
***********************************************************/
void LCDClrAll(void)
{
	u8 i, j;
	for(i=0;i<20;i++)
  	{
    	for(j=0;j<240;j++)
    	{
     		DISP_RAM[i][j] = 0;
    	}

  	}
	
}

void LCDClrWave(void)
{
	u8 i, j;
	for(i=2;i<18;i++)
  	{
    	for(j=0;j<240;j++)
    	{
     		DISP_RAM[i][j] = 0;
    	}

  	}
}
/*********************************************************************************
Name: void Printf1616(u8 ox,u8 oy,const u8 *p_char,u8 shadow) 
Function: 显示16*16的汉字串
Input:  ox:在第几列(0-191) oy:在第几行(0-19) p_char:字符串指针 shadow:阴影是否显示
Output: None
**********************************************************************************/
void Printf1616(u8 ox,u8 oy,const u8 *p_char,u8 shadow) 
{
	u8 i;
	for(;;)
	{
	  if(*p_char==0) break;
	  else
	  {
	    if(shadow==0)
	    {
	      for(i=0;i<16;i++) DISP_RAM[oy][ox+i] = Chinese[*p_char][0][i];
	      for(i=0;i<16;i++) DISP_RAM[oy+1][ox+i] = Chinese[*p_char][1][i];  
	    }else
	    {
	      for(i=0;i<16;i++) DISP_RAM[oy][ox+i] = (u8)(~Chinese[*p_char][0][i]);
	      for(i=0;i<16;i++) DISP_RAM[oy+1][ox+i] = (u8)(~Chinese[*p_char][1][i]);  
	    }
	  }
	  ox += 16;
	  p_char++;
	}
	
}

/*********************************************************************************
Name: void Printf1624(u8 ox,u8 oy,const u8 *p_char) 
Function: 显示16*24的汉字串,为对齐16*24的数字
Input:  ox:在第几列(0-191) oy:在第几行(0-19) p_char:字符串指针
Output: None
**********************************************************************************/
void Printf161624(u8 ox,u8 oy,const u8 *p_char) 
{
	u8 i;
	for(;;)
	{
	  if(*p_char==0) break;
	  else
	  {   
	    for(i=0;i<16;i++) DISP_RAM[oy][ox+i] = (Chinese[*p_char][0][i]<<4);
	    for(i=0;i<16;i++) DISP_RAM[oy+1][ox+i] = ((Chinese[*p_char][0][i]>>4)|(Chinese[*p_char][1][i]<<4)); 
	    for(i=0;i<16;i++) DISP_RAM[oy+2][ox+i] = (Chinese[*p_char][1][i]>>4); 
	  }
	  ox += 16;
	  p_char++;
	}
	
}

/*********************************************************************************
Name: void Printf816(u8 ox,u8 oy,const u8 *p_char,u8 shadow) 
Function: 显示8*16的字符串
Input:  ox:在第几列(0-191) oy:在第几行(0-19) p_char:字符串指针 shadow:阴影是否显示
Output: ox为最后的数据的位置
**********************************************************************************/
u8 Printf816(u8 ox,u8 oy,const u8 *p_char,u8 shadow) 
{
	u8 i, k;
	for(;;)
	{
	  if(*p_char==0) break;
	  else
	  {
	  	if(0xA8 == *p_char) p_char++; //针对西班牙文中的特殊字符
	  	k=Asc0816ox[*p_char-0x20];
	    if(shadow==0)
	    {
	      for(i=0;i<k;i++) DISP_RAM[oy][ox+i] = Asc0816[*p_char-0x20][0][i];
	      for(i=0;i<k;i++) DISP_RAM[oy+1][ox+i] = Asc0816[*p_char-0x20][1][i];  
	    }else
	    {
	      for(i=0;i<k;i++) DISP_RAM[oy][ox+i] = (u8)(~Asc0816[*p_char-0x20][0][i]);
	      for(i=0;i<k;i++) DISP_RAM[oy+1][ox+i] = (u8)(~Asc0816[*p_char-0x20][1][i]);  
	    }
	  }
	  ox += k;
	  p_char++;
	}
	return ox;
}

//补充系统设置时对不起损失的部分
void Printf816Lost(u8 ox,u8 oy,u8 k,u8 shadow) 
{
	u8 i;
	if((k==0)||(k>120)) return; 
	if(shadow==0)
	{
	  for(i=0;i<k;i++) DISP_RAM[oy][ox+i] = 0;
	  for(i=0;i<k;i++) DISP_RAM[oy+1][ox+i] = 0;  
	}else
	{
	  for(i=0;i<k;i++) DISP_RAM[oy][ox+i] = 0xff;
	  for(i=0;i<k;i++) DISP_RAM[oy+1][ox+i] = 0xff;  
	}
}

/*********************************************************************************
Name: void Printf81624(u8 ox,u8 oy,const u8 *p_char) 
Function: 显示8*24的字符串, 字符用的是8*16字符，主要是解决数据显示和单位对齐的问题
Input:  ox:在第几列(0-191) oy:在第几行(0-19) p_char:字符串指针
Output: None
**********************************************************************************/
u8 Printf81624(u8 ox,u8 oy,const u8 *p_char) 
{
	u8 i, k;
	for(;;)
	{
	  if(*p_char==0) break;
	  else
	  {
	  	 k=Asc0816ox[*p_char-0x20];
		 for(i=0;i<k;i++) DISP_RAM[oy][ox+i] = (Asc0816[*p_char-0x20][0][i]<<4);
	     for(i=0;i<k;i++) DISP_RAM[oy+1][ox+i] = ((Asc0816[*p_char-0x20][0][i]>>4)|(Asc0816[*p_char-0x20][1][i]<<4));
	     for(i=0;i<k;i++) DISP_RAM[oy+2][ox+i] = (Asc0816[*p_char-0x20][1][i]>>4); 
	  }
	  ox += k;
	  p_char++;
	}
	return(ox);
}

/*********************************************************************************
Name: void Printf816Unit(u8 ox,u8 oy,const u8 *p_char,u8 shadow) 
Function: 显示12*16的单位
Input:  ox:在第几列(0-127) oy:在第几行(0-7) p_char:字符串指针 shadow:阴影是否显示
Output: None
**********************************************************************************/
void Printf816Unit(u8 ox,u8 oy,u8 midunit,u8 shadow) 
{
	u8 i;
	if(shadow==0)
	{
	  for(i=0;i<12;i++) DISP_RAM[oy][ox+i] = Unit816[midunit][0][i];
	  for(i=0;i<12;i++) DISP_RAM[oy+1][ox+i] = Unit816[midunit][1][i];  
	}else
	{
	  for(i=0;i<12;i++) DISP_RAM[oy][ox+i] = (u8)(~Unit816[midunit][0][i]);
	  for(i=0;i<12;i++) DISP_RAM[oy+1][ox+i] = (u8)(~Unit816[midunit][1][i]);  
	}
}

/*********************************************************************************
Name: void Printf81624Unit(u8 ox,u8 oy,u8 unit) 
Function: 显示8*24的单位 k M G, 字符用的是8*16字符，主要是解决数据显示和单位对齐的问题
Input:  ox:在第几列(0-191) oy:在第几行(0-19) unit:需要显示的单位
Output: None
**********************************************************************************/
void Printf81624Unit(u8 ox,u8 oy,u8 unit) 
{
	u8 i;
	for(i=0;i<12;i++) DISP_RAM[oy][ox+i] = (Unit816[unit][0][i]<<4);
	for(i=0;i<12;i++) DISP_RAM[oy+1][ox+i] = ((Unit816[unit][0][i]>>4)|(Unit816[unit][1][i]<<4));
	for(i=0;i<12;i++) DISP_RAM[oy+2][ox+i] = (Unit816[unit][1][i]>>4); 
}

//显示字符串
void Printf816Char(u8 ox,u8 oy, u8 *p_char, u8 k)
{
	u8 i, j;
	for(j=0; j<k; j++)
	{
		if(*p_char==0) break;
		for(i=0;i<8;i++) DISP_RAM[oy][ox+i] = (u8)(Asc0816[*p_char-0x20][0][i]);
	    for(i=0;i<8;i++) DISP_RAM[oy+1][ox+i] = (u8)(Asc0816[*p_char-0x20][1][i]);	
		ox += 8;
	  	p_char++;
	}	
}

//显示符号+-在电能界面
void Printf1624(u8 ox,u8 oy,const u8 i)
{
	u8 j;
	for(j=0;j<16;j++) DISP_RAM[oy][ox+j] = Number1624[i][0][j];
	for(j=0;j<16;j++) DISP_RAM[oy+1][ox+j] = Number1624[i][1][j];
    for(j=0;j<16;j++) DISP_RAM[oy+2][ox+j] = Number1624[i][2][j];
}

/*********************************************************************************
Name: void Printf86(u8 ox,u8 oy,const u8 *p_char,u8 shadow) 
Function: 显示8*8的字符串
Input:  ox:在第几列(0-191) oy:在第几行(0-19) p_char:字符串指针 shadow:阴影是否显示
Output: None
**********************************************************************************/
void Printf86(u8 ox,u8 oy,const u8 *p_char,u8 shadow) 
{
	u8 i;
	for(;;)
	{
	  if(*p_char==0) break;
	  else
	  {
	    if(shadow==0)
	    {
	      for(i=0;i<6;i++) DISP_RAM[oy][ox+i] = (u8)Asc0806[*p_char-0x20][i];
	    }else
	    {
	      for(i=0;i<6;i++) DISP_RAM[oy][ox+i] = (u8)(~Asc0806[*p_char-0x20][i]);
	    }
	  }
	  ox += 6;
	  p_char++;
	}
}


/*********************************************************************************
Name: void Print816(u8 ox,u8 oy,u8 midchar,u8 shadow) 
Function: 显示8*16的一个数字
Input:  ox:在第几列(0-191) oy:在第几行(0-19) midchar:数字 shadow:阴影是否显示
Output: None
**********************************************************************************/
void Print816Num(u8 ox,u8 oy,u8 midchar,u8 shadow) 
{
	u8 i;
	 if(shadow==0)
	 {
	   for(i=0;i<8;i++) DISP_RAM[oy][ox+i] = Asc0816[midchar+0x10][0][i];
	   for(i=0;i<8;i++) DISP_RAM[oy+1][ox+i] = Asc0816[midchar+0x10][1][i];  
	 }
	 else
	 {
	   for(i=0;i<8;i++) DISP_RAM[oy][ox+i] = (u8)(~Asc0816[midchar+0x10][0][i]);
	   for(i=0;i<8;i++) DISP_RAM[oy+1][ox+i] = (u8)(~Asc0816[midchar+0x10][1][i]);  
	 }
	
}


//电量棒图
void PrintfLine(u8 ox, u8 oy, u16 len,u8 data)
{
	u8 i;
	if(0==len)  return;	
	DISP_RAM[oy][24] = 0xff;  //零处为实线
	DISP_RAM[oy+1][24] = 0xff; 
	if(1==data)
	{
		for(i=0;i<len;i++) DISP_RAM[oy][ox+i] = 0xff;
		for(i=0;i<len;i++) DISP_RAM[oy+1][ox+i] = 0xff; 
	}
	else
	{
		for(i=0;i<len;i++) DISP_RAM[oy][ox+i] = 0x01;
		for(i=0;i<len;i++) DISP_RAM[oy+1][ox+i] = 0x80; 
		DISP_RAM[oy][ox+i-1] = 0xff;	
		DISP_RAM[oy+1][ox+i-1] = 0xff; 	 //120%处
	}
	
	
}

/*********************************************************
Name: void DrawLineX(u8 ox, u8 oy, u8 lx, u8 line)
Function: X轴方向直线
Input: ox, oy:直线起始地址 lx：直线长度(像素) line:直线位置
**********************************************************/
void DrawLineX(u8 ox, u8 oy, u8 lx, u8 line)
{
	u8 i;
	for(i=0; i<lx; i++) DISP_RAM[oy][ox+i] |= line;
}

//虚线X
void DrawLineX2(u8 ox, u8 oy, u8 xl, u8 line)
{
	u8 i, lx;
	lx = xl/2;
	for(i=0; i<lx; i++) DISP_RAM[oy][ox+(i<<1)] |= line;
}
/*********************************************************
Name: void DrawLineY(u8 ox, u8 oy, u8 ly, u8 line)
Function: X轴方向直线
Input: ox, oy:直线起始地址 lx：直线长度(8个像素) line:直线位置
**********************************************************/
void DrawLineY(u8 ox, u8 oy, u8 ly, u8 line)
{
	u8 i;
	for(i=0; i<ly; i++) 
	{
		DISP_RAM[oy+i][ox] |= line;
	}
}


/*******************************************************************
Name: void PrintfKeyF4(u8 F1,u8 F2,u8 F3,u8 F4)
Function: 4个按键显示
Input：F1,F2,F3,F4: 按键显示的内容值
*******************************************************************/
void PrintfKeyF4(u8 F1,u8 F2,u8 F3,u8 F4)
{
  u8 i;
  
  if(KeyValue&KEY_LEFT)
  {
    for(i=0;i<60;i++) DISP_RAM[18][i] = (u8)Tab_Key1648[1][F1][0][i];
    for(i=0;i<60;i++) DISP_RAM[19][i] = (u8)Tab_Key1648[1][F1][1][i];
  }
  else
  {
    for(i=0;i<60;i++) DISP_RAM[18][i] = (u8)Tab_Key1648[0][F1][0][i];
    for(i=0;i<60;i++) DISP_RAM[19][i] = (u8)Tab_Key1648[0][F1][1][i];
  }
  
  if(KeyValue&KEY_RIGHT)
  {
    for(i=0;i<60;i++) DISP_RAM[18][60+i] = (u8)Tab_Key1648[1][F2][0][i];
    for(i=0;i<60;i++) DISP_RAM[19][60+i] = (u8)Tab_Key1648[1][F2][1][i];
  }
  else
  {
    for(i=0;i<60;i++) DISP_RAM[18][60+i] = (u8)Tab_Key1648[0][F2][0][i];
    for(i=0;i<60;i++) DISP_RAM[19][60+i] = (u8)Tab_Key1648[0][F2][1][i];
  }
  if(KeyValue&KEY_MENU)
  {
  	for(i=0;i<60;i++) DISP_RAM[18][120+i] = (u8)Tab_Key1648[1][F3][0][i];
	for(i=0;i<60;i++) DISP_RAM[19][120+i] = (u8)Tab_Key1648[1][F3][1][i];
  }
  else
  {
    for(i=0;i<60;i++) DISP_RAM[18][120+i] = (u8)Tab_Key1648[0][F3][0][i];
	for(i=0;i<60;i++) DISP_RAM[19][120+i] = (u8)Tab_Key1648[0][F3][1][i];
  }
  
  if(KeyValue&KEY_ENTER)
  {
    for(i=0;i<60;i++) DISP_RAM[18][180+i] = (u8)Tab_Key1648[1][F4][0][i];
	for(i=0;i<60;i++) DISP_RAM[19][180+i] = (u8)Tab_Key1648[1][F4][1][i];
  }
  else
  {
    for(i=0;i<60;i++) DISP_RAM[18][180+i] = (u8)Tab_Key1648[0][F4][0][i];
	for(i=0;i<60;i++) DISP_RAM[19][180+i] = (u8)Tab_Key1648[0][F4][1][i];
  }
}

/******************************************************
Name: void Printf816Num3(u8 ox,u8 oy,s16 num,u8 dot)
Function: 显示字符为8*16的数字数据，显示3个数字
Input:  ox,oy:数字的位置，
num:需要显示的数据，99.9数字
dot：小数点位置
Output: None
*******************************************************/
void Printf816Num3(u8 ox,u8 oy,s16 num,u8 dot)
{
  	u8 i,j;
  	u8 bit[3];
  	u16 n;
  	n=num;
  	UintToBCD3(bit,n);
	for(i=0;i<3;i++)
	{
	  for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816[bit[i]][0][j];
	  for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816[bit[i]][1][j];

	  ox += 8;
	  if((dot+i)==2&&i!=2)
	  {
	    for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816[10][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816[10][1][j];
	  	ox += 8;
	  }	
	}
}



/******************************************************
Name: void Printf1624Num5(u8 ox,u8 oy,s32 num,u8 dot)
Function: 显示字符为16*24的数字数据，显示5个数字
Input:  ox,oy:数字的位置，
num:需要显示的数据， +/-9999数字
dot：小数点位置
Output: None
*******************************************************/
void Printf1624Num4(u8 ox,u8 oy,s16 num,u8 dot)
{
  	u8 i,j;
  	u8 bit[4];
  	u16 n;
  	
  	if(num<0)
  	{
  	  n = (u16)(-num);
  	  
  	}
  	else
  	{
  		n = (u16)num;
  	} 
//显示或清除符号位	

  	UintToBCD4(bit,n);
	for(i=0;i<4;i++)
	{
	  for(j=0;j<16;j++) DISP_RAM[oy][ox+j] = Number1624[bit[i]][0][j];
	  for(j=0;j<16;j++) DISP_RAM[oy+1][ox+j] = Number1624[bit[i]][1][j];
	  for(j=0;j<16;j++) DISP_RAM[oy+2][ox+j] = Number1624[bit[i]][2][j];
	  ox += 16;
	  if((dot+i)==3&&i!=3)
	  {
	    for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number1624Point[0][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number1624Point[0][1][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+2][ox+j] = Number1624Point[0][2][j];
	  	ox += 8;
	  }	
	}
	if(!dot) //无小数点防止上次有小数点时的显示
	{
		for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = 0x00;
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = 0x00;
	  	for(j=0;j<8;j++) DISP_RAM[oy+2][ox+j] = 0x00;
	}	
 
}


/******************************************************
Name: void Printf1624Num5(u8 ox,u8 oy,s32 num,u8 dot)
Function: 显示字符为16*24的数字数据，显示5个数字
Input:  ox,oy:数字的位置，
num:需要显示的数据， +/-99999数字
dot：小数点位置
Output: None
*******************************************************/
void Printf1624Num5(u8 ox,u8 oy,s32 num,u8 dot)
{
  	u8 i,j;
  	u8 bit[5];
  	u32 n;
  	
  	if(num<0)
  	{
  	  n = (u32)(-num);
  	  i=57; //-
  	}
  	else
  	{
  		n = (u32)num;
  		i=0;
  	} 
//显示或清除符号位	
  	for(j=0;j<16;j++) DISP_RAM[oy+1][ox-16+j] = Chinese[i][0][j];
	for(j=0;j<16;j++) DISP_RAM[oy+2][ox-16+j] = Chinese[i][1][j];	
  	
  	UintToBCD5(bit,n);
	for(i=0;i<5;i++)
	{
	  for(j=0;j<16;j++) DISP_RAM[oy][ox+j] = Number1624[bit[i]][0][j];
	  for(j=0;j<16;j++) DISP_RAM[oy+1][ox+j] = Number1624[bit[i]][1][j];
	  for(j=0;j<16;j++) DISP_RAM[oy+2][ox+j] = Number1624[bit[i]][2][j];
	  ox += 16;
	  if((dot+i)==4&&i!=4)
	  {
	    for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number1624Point[0][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number1624Point[0][1][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+2][ox+j] = Number1624Point[0][2][j];
	  	ox += 8;
	  }	
	}
	if(!dot) //无小数点防止上次有小数点时的显示
	{
		for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = 0x00;
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = 0x00;
	  	for(j=0;j<8;j++) DISP_RAM[oy+2][ox+j] = 0x00;
	}	
 
}


/*************************************
Name: void UintToBCD3(u8 *p_char,u16 value)
Function: BCD码转换，有4个BCD码数据显示
Input: *p_char:存放BCD码结果数据
       value: 需要显示的数据[0]:最高数据， [2]:最低数据
Output: None
*************************************/
void UintToBCD3(u8 *p_char,u16 value)
{
  u32 temp = value;
  u8 i;
  for(i=0;i<3;i++) 
  {
    *(p_char+2-i) = (u8)(temp%10);
    temp = (u32)(temp/10);
  }  
}

/*************************************
Name: void UintToBCD4(u8 *p_char,u16 value)
Function: BCD码转换，有4个BCD码数据显示
Input: *p_char:存放BCD码结果数据
       value: 需要显示的数据[0]:最高数据， [3]:最低数据
Output: None
*************************************/
void UintToBCD4(u8 *p_char,u16 value)
{
  u32 temp = value;
  u8 i;
  for(i=0;i<4;i++) 
  {
    *(p_char+3-i) = (u8)(temp%10);
    temp = (u32)(temp/10);
  }  
}



/*************************************
Name: void UintToBCD5(u8 *p_char,u32 value)
Function: BCD码转换，有5个BCD码数据显示
Input: *p_char:存放BCD码结果数据
       value: 需要显示的数据[0]:最高数据， [4]:最低数据
Output: None
*************************************/
void UintToBCD5(u8 *p_char,u32 value)
{
  u32 temp = value;
  u8 i;
  for(i=0;i<5;i++) 
  {
    *(p_char+4-i) = (u8)(temp%10);
    temp = (u32)(temp/10);
  }  
}

//显示运行时间小时
void Printf1624Num6(u8 ox,u8 oy,u32 num)
{
  	u8 i,j;
  	u8 bit[8];
  	u32 n;
  	
  	n = (u32)num;
//显示或清除符号位	
  
  	UintToBCD8(bit,n);
	for(i=2;i<8;i++)
	{
	  for(j=0;j<16;j++) DISP_RAM[oy][ox+j] = Number1624[bit[i]][0][j];
	  for(j=0;j<16;j++) DISP_RAM[oy+1][ox+j] = Number1624[bit[i]][1][j];
	  for(j=0;j<16;j++) DISP_RAM[oy+2][ox+j] = Number1624[bit[i]][2][j];
	  ox += 16;
	}
}


/******************************************************
Name: void Printf1624Num8(u8 ox,u8 oy,u32 num,u8 dot)
Function: 显示字符为16*24的数字数据，显示8个数字
Input:  ox,oy:数字的位置，
num:需要显示的数据， +/-99999数字
dot：小数点位置
Output: None
*******************************************************/
void Printf1624Num8(u8 ox,u8 oy,u32 num,u8 dot)
{
  	u8 i,j;
  	u8 bit[8];
  	u32 n;
  	
  	n = (u32)num;
//显示或清除符号位	
  
  	UintToBCD8(bit,n);
	for(i=0;i<8;i++)
	{
	  for(j=0;j<16;j++) DISP_RAM[oy][ox+j] = Number1624[bit[i]][0][j];
	  for(j=0;j<16;j++) DISP_RAM[oy+1][ox+j] = Number1624[bit[i]][1][j];
	  for(j=0;j<16;j++) DISP_RAM[oy+2][ox+j] = Number1624[bit[i]][2][j];
	  ox += 16;
	  if((dot+i)==7&&i!=7)
	  {
	    for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number1624Point[0][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number1624Point[0][1][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+2][ox+j] = Number1624Point[0][2][j];
	  	ox += 8;
	  }	
	}
	if(!dot) //无小数点防止上次有小数点时的显示
	{
		for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = 0x00;
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = 0x00;
	  	for(j=0;j<8;j++) DISP_RAM[oy+2][ox+j] = 0x00;
	}	
 
}


/*************************************
Name: void UintToBCD5(u8 *p_char,u32 value)
Function: BCD码转换，有8个BCD码数据显示
Input: *p_char:存放BCD码结果数据
       value: 需要显示的数据[0]:最高数据， [7]:最低数据
Output: None
*************************************/
void UintToBCD8(u8 *p_char,u32 value)
{
  u32 temp = value;
  u8 i;
  for(i=0;i<8;i++) 
  {
    *(p_char+7-i) = (u8)(temp%10);
    temp = (u32)(temp/10);
  }  
}



/************************************************
Name: void PrintfBMP3216(u8 ox,u8 oy,u8 bmp)
Function: 32*16的位图
************************************************/
void PrintfBMP3216(u8 ox,u8 oy,u8 bmp)
{
	uint16_t i,j;
	for(j=0; j<2; j++)
	{
		for(i=0;i<32;i++) DISP_RAM[oy][ox+i] = Tab_Bmp3216[bmp][j][i]; 
		oy += 1; 		
	}
}

/************************************************
Name: void PrintfBMP24104(u8 ox,u8 oy,u8 bmp)
Function: 24*104的位图
************************************************/
void PrintfBMP24104(u8 ox,u8 oy,u8 bmp)
{
	uint16_t i,j;
	for(j=0; j<3; j++)
	{
		for(i=0;i<104;i++) DISP_RAM[oy][ox+i] = Tab_BMP24104[bmp][j][i]; 
		oy += 1; 		
	}
}

//显示8*20图像，
void PrintfBMP820(u8 ox,u8 oy,u8 bmp)
{
	u8 i;
	for(i=0;i<20;i++) DISP_RAM[oy][ox+i] = LCMARK[bmp][i]; 
}

//对不能显示数据部分显示----
void PrintfBMP2472(u8 ox,u8 oy,u8 bmp)
{
	uint16_t i,j;
	for(j=0; j<3; j++)
	{
		for(i=0;i<72;i++) DISP_RAM[oy][ox+i] = Tab_BMP24104[bmp][j][i]; 
		oy += 1; 		
	}
}

/**************************************************
Name: void Printf9(u8 ox,u8 oy,u16 n,u8 shadow)
Function: 数字显示
Input:  ox,oy 数据位置 n: 数据值 shadow 0无反显 1显 
Output: None
**************************************************/
void Printf9(u8 ox,u8 oy,u16 n,u8 shadow)
{
  	u8 j;
  	u8 bit;
  	bit = (u8)n;
	if(0 == shadow)
	{
		for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816[bit][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816[bit][1][j];
	} 
	else
	{
		for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit][1][j];
	}
}


/**************************************************
Name: void Printf99(u8 ox,u8 oy,u16 n,u8 shadow)
Function: 数字显示
Input:  ox,oy 数据位置 n: 数据值 shadow 0无反显 1反显
Output: None
**************************************************/
void Printf99(u8 ox,u8 oy,u16 n,u8 shadow)
{
  u8 j;
  u8 bit[2];
  bit[0] = (u8)(n/10%10);
  bit[1] = (u8)(n%10);
  if(0 == shadow)
  {
  	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816[bit[0]][0][j];
	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816[bit[0]][1][j];
	ox += 8;
	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816[bit[1]][0][j];
	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816[bit[1]][1][j];
  
  }
  else
  {
  	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[0]][0][j];
	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[0]][1][j];
	ox += 8;
	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[1]][0][j];
	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[1]][1][j];
  }	
}

/**************************************************
Name: void PrintfFFHex(u8 ox,u8 oy,u16 n,u8 shadow)
Function: 数字显示
Input:  ox,oy 数据位置 n: 数据值 shadow 0无反显 1反显
Output: None
**************************************************/
//const u8 TABHEX[16] = "0123456789ABCDEF";
void PrintfFFHex(u8 ox,u8 oy,u16 n,u8 shadow)
{
  u8 bit[2];
  u8 j;
  bit[0] = n/16;
  bit[1] = n%16;
  if(shadow==0)
  {
  	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816F[bit[0]][0][j];
	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816F[bit[0]][1][j];
	ox += 8;
	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816F[bit[1]][0][j];
	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816F[bit[1]][1][j];
	
  }
  else if(shadow==1)
  {
  	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816F[bit[0]][0][j];
	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816F[bit[0]][1][j];
	ox += 8;
	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = (u8)(~Number816F[bit[1]][0][j]);
	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = (u8)(~Number816F[bit[1]][1][j]);
	 
  }
  else if(shadow==2)
  {
  	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = (u8)(~Number816F[bit[0]][0][j]);
	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = (u8)(~Number816F[bit[0]][1][j]);
	ox += 8;
	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816F[bit[1]][0][j];
	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816F[bit[1]][1][j];
  }
  else 
  {
  	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = (u8)(~Number816F[bit[0]][0][j]);
	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = (u8)(~Number816F[bit[0]][1][j]);
    ox += 8;
	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = (u8)(~Number816F[bit[1]][0][j]);
	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = (u8)(~Number816F[bit[1]][1][j]);
  }
}

void PrintfFFFFHex(u8 ox,u8 oy,u16 n,u8 shadow)
{
  u8 bit[4];
  u8 j,i;
  bit[0] = n/4096;
  bit[1] = n/256%16;
  bit[2] = n/16%16;
  bit[3] = n%16;
  for(i=0;i<4;i++)
  {
    if(shadow>4)
    {
    	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816F[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816F[bit[i]][1][j];	
    }   
    else if((i+shadow)!=4)
    {
    	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816F[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816F[bit[i]][1][j];
    } 
    else
    {
    	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816F[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816F[bit[i]][1][j];
    }
    ox += 8;
  }
}


/**************************************************
Name: void Printf999(u8 ox,u8 oy,u16 n,u8 shadow)
Function: 数字显示
Input:  ox,oy 数据位置 n: 数据值 shadow 0无反显 1-3位反显 4全反显
Output: None
**************************************************/
void Printf999(u8 ox,u8 oy,u16 n,u8 shadow)
{
  u8 i,j;
  u8 bit[3];
  bit[0] = (u8)(n/100%10);
  bit[1] = (u8)(n/10%10);
  bit[2] = (u8)(n%10);
  for(i=0;i<3;i++)
  {
    if(shadow>3)
    {
    	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[i]][1][j];	
    }   
    else if((i+shadow)!=3)
    {
    	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816[bit[i]][1][j];
    } 
    else
    {
    	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[i]][1][j];
    }
    ox += 8;
  }  
}



/**************************************************
Name: void Printf9999(u8 ox,u8 oy,u16 n,u8 shadow)
Function: 数字显示
Input:  ox,oy 数据位置 n: 数据值 shadow 0无反显 1-4位反显 5全反显
Output: None
**************************************************/

void Printf9999(u8 ox,u8 oy,u16 n,u8 shadow)
{
  u8 i,j;
  u8 bit[4];
  bit[0] = (u8)(n/1000%10);
  bit[1] = (u8)(n/100%10);
  bit[2] = (u8)(n/10%10);
  bit[3] = (u8)(n%10);
  for(i=0;i<4;i++)
  {
    if(shadow>4)
    {
    	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[i]][1][j];	
    }   
    else if((i+shadow)!=4)
    {
    	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816[bit[i]][1][j];
    } 
    else
    {
    	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[i]][1][j];
    }
    ox += 8;
  }  
}


void Printf99999(u8 ox,u8 oy,u16 n,u8 shadow)
{
  u8 i,j;
  u8 bit[5];
  bit[0] = (u8)(n/10000%10);
  bit[1] = (u8)(n/1000%10);
  bit[2] = (u8)(n/100%10);
  bit[3] = (u8)(n/10%10);
  bit[4] = (u8)(n%10);
  for(i=0;i<5;i++)
  {
    if(shadow>5)
    {
    	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[i]][1][j];	
    }   
    else if((i+shadow)!=5)
    {
    	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816[bit[i]][1][j];
    } 
    else
    {
    	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[i]][1][j];
    }
    ox += 8;
  }  
}


/**************************************************
Name: void Printf999999(u8 ox,u8 oy,u16 n,u8 shadow)
Function: 数字显示
Input:  ox,oy 数据位置 n: 数据值 shadow 0无反显 1-4位反显 5全反显
Output: None
**************************************************/
void Printf999999(u8 ox,u8 oy,u32 n,u8 shadow)
{
  u8 i,j;
  u8 bit[6];
  u32 bit32 = n;
  for(i=0;i<6;i++) 
  {
    bit[5-i] = (u8)(bit32%10);
    bit32 = bit32/10;
  }
  for(i=0;i<6;i++)
  {
    if(shadow>6)
    {
    	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[i]][1][j];	
    }    
    else if((i+shadow)!=6)
    {
    	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816[bit[i]][1][j];
    } 
    else 
    {
    	for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[i]][1][j];
    }
	ox += 8;	 
  }    
}

/***********************************************************************
Name: void Printf9999DotShadow(u8 ox,u8 oy,u16 value,u8 dot,u8 shadow)
Function: 打印带小数点和反显示位数据,无符号
Input:  dot 小数点位置 0无 1-3小数点位置 
		shadow 反显位 0无 1-4反显示位 5全反显示
		value 数据值
Output: None
***************************************************************************/
void Printf9999DotShadow(u8 ox,u8 oy,u16 value,u8 dot,u8 shadow)
{
  u8 i,j, midox;
  u8 bit[5];
  midox = ox;
  if(dot==1)
  {// xxx.x
    bit[0] = (u8)((value/1000)%10);
    bit[1] = (u8)(value/100%10);
    bit[2] = (u8)(value/10%10);
    bit[3] = 10; //.
    bit[4] = (u8)(value%10); 
    for(i=0;i<5;i++)
    {
      if(shadow > 4)
      {
        for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[i]][1][j];	 
      }else
      {
        for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816[bit[i]][1][j];	 
      }
      ox+=8;
    }
    ox = midox; //恢复
    //显示阴影
    if(shadow==1)
    {
      for(j=0;j<8;j++) DISP_RAM[oy][ox+j+32] = ~Number816[bit[4]][0][j];
	  for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j+32] = ~Number816[bit[4]][1][j];	   
    }
    else if(shadow==2)
    {
      for(j=0;j<8;j++) DISP_RAM[oy][ox+j+16] = ~Number816[bit[2]][0][j];
	  for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j+16] = ~Number816[bit[2]][1][j];	     
    }
    else if(shadow==3)
    {
      for(j=0;j<8;j++) DISP_RAM[oy][ox+j+8] = ~Number816[bit[1]][0][j];
	  for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j+8] = ~Number816[bit[1]][1][j];	     
    }
	else if(shadow==4)
    {
      for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[0]][0][j];
	  for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[0]][1][j];	     
    }
  }
  else if(dot==2)
  {//xx.xx
    bit[0] = (u8)((value/1000)%10);
    bit[1] = (u8)(value/100%10);
    bit[2] = 10; //.
    bit[3] = (u8)(value/10%10);
    bit[4] = (u8)(value%10); 
    
    for(i=0;i<5;i++)
    {
      if(shadow > 4)
      {
        for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[i]][1][j];	  
      }else
      {
        for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816[bit[i]][1][j];	  
      }
      ox += 8;
    }
    ox = midox;
    if(shadow==1)
    {
      for(j=0;j<8;j++) DISP_RAM[oy][ox+j+32] = ~Number816[bit[4]][0][j];
	  for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j+32] = ~Number816[bit[4]][1][j];     
    }
    else if(shadow==2)
    {
      for(j=0;j<8;j++) DISP_RAM[oy][ox+j+24] = ~Number816[bit[3]][0][j];
	  for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j+24] = ~Number816[bit[3]][1][j];      
    }
    else if(shadow==3)
    {
      for(j=0;j<8;j++) DISP_RAM[oy][ox+j+8] = ~Number816[bit[1]][0][j];
	  for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j+8] = ~Number816[bit[1]][1][j];       
    }
    else if(shadow==4)
    {
      for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[0]][0][j];
	  for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[0]][1][j];     
    }
  }
  else if(dot==3)
  {//x.xxx
    bit[0] = (u8)((value/1000)%10);
    bit[1] = 10;
    bit[2] = (u8)(value/100%10);
    bit[3] = (u8)(value/10%10);
    bit[4] = (u8)(value%10); 
    
    for(i=0;i<5;i++)
    {
      if(shadow > 4)
      {
        for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[i]][1][j];	 
      }else
      {
        for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816[bit[i]][1][j];	 
      }
      ox+=8;
    }
    ox = midox;
    if(shadow==1)
    {
      for(j=0;j<8;j++) DISP_RAM[oy][ox+j+32] = ~Number816[bit[4]][0][j];
	  for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j+32] = ~Number816[bit[4]][1][j];     
    }
    else if(shadow==2)
    {
      for(j=0;j<8;j++) DISP_RAM[oy][ox+j+24] = ~Number816[bit[3]][0][j];
	  for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j+24] = ~Number816[bit[3]][1][j];    
    }
    else if(shadow==3)
    {
      for(j=0;j<8;j++) DISP_RAM[oy][ox+j+16] = ~Number816[bit[2]][0][j];
	  for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j+16] = ~Number816[bit[2]][1][j];      
    }
    else if(shadow==4)
    {
      for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[0]][0][j];
	  for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[0]][1][j];      
    }
  }
  else
  {//xxxx
    bit[0] = (u8)((value/1000)%10);
    bit[1] = (u8)(value/100%10);
    bit[2] = (u8)(value/10%10);
    bit[3] = (u8)(value%10); 
    for(i=0;i<4;i++)
    {
      if(shadow > 4)
      {
        for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[i]][1][j];  
      }
      else
      {
        for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816[bit[i]][0][j];
	  	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816[bit[i]][1][j];
      }
      ox += 8;
    }
    ox = midox;
    if(shadow==1)
    {
      for(j=0;j<8;j++) DISP_RAM[oy][ox+j+24] = ~Number816[bit[3]][0][j];
	  for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j+24] = ~Number816[bit[3]][1][j];     
    }
    else if(shadow==2)
    {
      for(j=0;j<8;j++) DISP_RAM[oy][ox+j+16] = ~Number816[bit[2]][0][j];
	  for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j+16] = ~Number816[bit[2]][1][j];      
    }
    else if(shadow==3)
    {
      for(j=0;j<8;j++) DISP_RAM[oy][ox+j+8] = ~Number816[bit[1]][0][j];
	  for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j+8] = ~Number816[bit[1]][1][j];      
    }
    else if(shadow==4)
    {
      for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = ~Number816[bit[0]][0][j];
	  for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = ~Number816[bit[0]][1][j];      
    }
  }
}
/**************************************************
Name: void Printf99999999(u8 ox,u8 oy,u16 n)
Function: 数字显示
Input:  ox,oy 数据位置 n: 数据值 
Output: None
**************************************************/
void Printf99999999(u8 ox,u8 oy,u32 n)
{
  u8 i,j;
  u8 bit[8];
  u32 bit32 = n;
  for(i=0;i<8;i++) 
  {
    bit[7-i] = (u8)(bit32%10);
    bit32 = bit32/10;
  }
  for(i=0;i<8;i++)
  {
    for(j=0;j<8;j++) DISP_RAM[oy][ox+j] = Number816[bit[i]][0][j];
	for(j=0;j<8;j++) DISP_RAM[oy+1][ox+j] = Number816[bit[i]][1][j];
	ox += 8;	 
  }    
}
