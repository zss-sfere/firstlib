/*********************************************************************
本程序包括按键的读取操作和仪表按键的操作
*********************************************************************/
#include "User_Config.h"

extern void LCDCmd(u8 LCDCommand);
extern void Time2Init(void);
extern u8 Time2LCD;
extern u16 Time4Num2;

#define KEY_DLY_20MS  0x05
#define KEY_DLY_3S	  0xaa
#define KEY_DLY_200MS 0x25
#define KEY_CONTINUE  0xc0
#define KEY_SCAN_MODE0 0x00
#define KEY_SCAN_MODE1 0x01
#define KEY_SCAN_MODE2 0x02
#define KEY_SCAN_MODE3 0x03
#define KEY_SCAN_MODE4 0x04 



/****************************************************************************
Name: void TaskScanKey(void)
Function: 按键扫描  (带防抖功能)
Input:  None
Output: None 
****************************************************************************/ 
void TaskScanKey(void)
{
	TASK_START[0] &= Tab_32bit[1][BIT_SCANKEY];
	switch (KeyScanMode)
	{
		case 	KEY_SCAN_MODE0:
				KeyScanMode0();
				break;
		case 	KEY_SCAN_MODE1:
				KeyScanMode1();
				break;
		case 	KEY_SCAN_MODE2:
				KeyScanMode2();
				break;
		case    KEY_SCAN_MODE3:
				KeyScanMode3();
				break;	
		case    KEY_SCAN_MODE4:
			    KeyScanMode4();
				break;		
	}
	if(0x01 == KeyState) //
	{
		KeyNoEnterTime = 0;
		TASK_START[0] |= Tab_32bit[0][BIT_KEY]; //启动按键处理程序	
	}					
}
/*****************************************************
Name: u8 KeyRd(void)
Function: 读取按键IO状态 
Input:  None
Output: KEYPORT -> 按键状态
*****************************************************/
u8 KeyRd(void)
{
	u8 KEYPORT =0x00;
	u16 middledata,mid2;
	mid2 = GPIO_ReadInputData(GPIOC); 
	mid2 = (mid2>>6)&0x08;
	middledata = GPIO_ReadInputData(GPIOA);
	middledata = (((middledata>>6)&0x04)|((middledata>>8)&0x02)|((middledata>>10)&0x01)|mid2);
	KEYPORT = (u8)(~middledata&0x0f);	
	return(KEYPORT);
	
}

/************************************************************
Name: void KeyScanMode0 (void)
Function: 按键参数复位,读键值
Input:  None
Output: None
************************************************************/
void KeyScanMode0 (void)
{
	KeyValue = 0;			//按键参数复位
	KeyCtr = 0;
	
	CurrentKeyboardStatus = KeyRd ();		//读键值
	if(CurrentKeyboardStatus != 0x0f)			//有按键动作,转模式1,延时去抖
	{
		KeyScanMode = KEY_SCAN_MODE1;
		KeyCtr = KEY_DLY_20MS;
		CycNoKeyNum = 0;   //有按键则清除相应的时间累计
	}	
	else	//
	{
		KeyOther = 0;	
	}		
}

/************************************************************
Name: void KeyScanMode1 (void)
Function: 延时去抖,读键值,无按键动作,返回模式0,有按键动作, 保存键值,转模式2
Input:  None
Output: None
************************************************************/
void KeyScanMode1 (void)
{
	if (--KeyCtr == 0)			//延时去抖
	{
		CurrentKeyboardStatus = KeyRd ();		//读键
		if (CurrentKeyboardStatus == 0) 
		{
			KeyScanMode = KEY_SCAN_MODE0;//无按键动作,转模式0
			KeyOther = 0;
			return;
		}	
		KeyValue = CurrentKeyboardStatus;		//有按键动作,保存键值	
		//点亮背光
		GPIO_SetBits(GPIOC, GPIO_BACKLIGHT);
		Time2Light = 0;
		if((KeyValue == KEY_LEFT) ||(KeyValue == KEY_RIGHT) ||(KeyValue == KEY_ENTER))
		{
			KeyCtr = KEY_DLY_200MS;
			KeyState = 0x01;
			KeyValue = KeyValue;				//键释放，单击有效
			KeyScanMode = KEY_SCAN_MODE4;	//转模式0,重新开始扫描
		}
		else
		{
			KeyCtr = KEY_DLY_3S;			//延时3秒
			KeyScanMode  = KEY_SCAN_MODE2;
			CycNoKeyNum = 0;   
		}		
		//有按键则清除相应的时间累计
	}
}

/************************************************************
Name: void KeyScanMode2 (void)
Function: 延时3秒,若按键无效,则为单击,若按键有效,转模式3
Input:  None
Output: None
************************************************************/
void KeyScanMode2 (void)
{
	CurrentKeyboardStatus = KeyRd ();		//读键值
	if(CurrentKeyboardStatus != (KeyValue & 0x0f))	
	{
		KeyState = 0x01;
		KeyValue = KeyValue;				//键释放，单击有效
		KeyScanMode = KEY_SCAN_MODE0;	//转模式0,重新开始扫描
	}
	else 
	{
		if (--KeyCtr == 0)		//延时
		{
			KeyState = 0x01;
	    	KeyValue |= KEY_CONTINUE;				//连击3s有效
			KeyScanMode = KEY_SCAN_MODE3;	//转模式3,判断是否为连击
		    KeyCtr = KEY_DLY_3S;
		    CycNoKeyNum = 0;   //有按键则清除相应的时间累计
		 }
	}
}

/************************************************************
Name: void KeyScanMode3 (void)
Function: 延时5秒,若按键无效,则为长击3秒,若有效,转模式4
Input:  None
Output: None
*************************************************************/
void KeyScanMode3 (void)
{
	if (--KeyCtr != 0)
	{
		CurrentKeyboardStatus = KeyRd ();
		if (CurrentKeyboardStatus != (KeyValue & 0x0f))
		{
			KeyState = 0x01;
	    	KeyValue |= KEY_CONTINUE;				//连击3s有效
			KeyScanMode  = KEY_SCAN_MODE0;	//转模式0
			CycNoKeyNum = 0;   //有按键则清除相应的时间累计
		}
	}
	else 
	{
		KeyScanMode = KEY_SCAN_MODE0;
	}
}
/*********************************************************
Name: void KeyScanMode4()
Function: 按键连按方式
Input:  None
Output: None
*********************************************************/
void KeyScanMode4()
{
	CurrentKeyboardStatus = KeyRd ();		//读键值
	if (CurrentKeyboardStatus != (KeyValue & 0x0f))
	{
		KeyScanMode = KEY_SCAN_MODE0;	//转模式0,重新开始扫描
	}
	else
	{
		if(--KeyCtr != 0)
		{
		}
		else
		{
			KeyCtr = KEY_DLY_200MS;
			KeyState = 0x01;
			KeyValue = KeyValue;				//键释放，单击有效
			KeyScanMode = KEY_SCAN_MODE4;	//转模式0,重新开始扫描
		}			
	}		
}



/**************************************************************
Name: void TaskKey(void)
Function: 按键处理程序
Input:  None
Output: None
**************************************************************/
void TaskKey(void)
{
//	u8 constkeypage = 0;
	if(TASK_START[0]&Tab_32bit[0][BIT_KEY]) TASK_START[0] &= Tab_32bit[1][BIT_KEY];
	TASK_START[0] |= Tab_32bit[0][BIT_DISPLAYLCD]; //启动液晶显示程序 
	 
	
    if (KeyState) //如果有按键按下
    {
        KeyState = 0;
        if(LCDDark)
        {
        	LCDDark=0;
        	return;	
        }	     
    }
    
    //按键处理程序
    if (KeyMainmenu)
    {
        KeyDealMainmenu();    // 主菜单显示操作 
    } 
    else
    {
    	if((PAGE_EL == KeyPage)||PAGE_ELBAR == KeyPage) //
    	{	
    		if(INPUT33 == MeterSetData.c_input[0])	//三相三线电量显示界面操作
			{
				KeyDealDisplay(KeyPage,DISPLAY_33UL,DISPLAY_33PQSALL); 	
			}		
			else if(INPUT11 == MeterSetData.c_input[0])	//单相电量显示界面操作
			{
				KeyDealDisplay(KeyPage,DISPLAY_11UN,DISPLAY_11PQSALL); 
			}	
			else	//三相四线电量显示界面操作
			{
				KeyDealDisplay(KeyPage,DISPLAY_34UN,DISPLAY_34PF); 
			}
    	}
    	else if(PAGE_EN == KeyPage)
    	{
    		if(MeterSetData.c_set[TOUSET]) KeyDealDisplay(KeyPage,DISPLAY_EN,DISPLAY_TOU12);  //电能界面
    		else KeyDealDisplay(KeyPage,DISPLAY_EN,DISPLAY_EQFOUR);  //电能界面
    	}
    	else if(PAGE_QUALITY == KeyPage)
    	{
    		KeyDealDisplay(KeyPage,DISPLAY_QUAPHASE,DISPLAY_THDI6); //电能质量	
    	}
    	else if(PAGE_WAVE == KeyPage) //实时波形
    	{
    		KeyDealDisplay(KeyPage,DISPLAY_QUAUWAVE,DISPLAY_QUAIWAVE);	
    	}
    	else if(PAGE_DEMAND == KeyPage) //需量记录
    	{
    		KeyDealDisplay(KeyPage,DISPLAY_DEMANDMAX1,DISPLAY_DEMANDPre2);
    	}	
    	else if(PAGE_MAX == KeyPage) //极值记录
    	{
    		if(INPUT33 == MeterSetData.c_input[0])	//三相三线最大显示界面操作
			{
				KeyDealDisplay(KeyPage,DISPLAY_MAX33UL,DISPLAY_AVG33PQS); 	
			}		
			else if(INPUT11 == MeterSetData.c_input[0])	//单相最大值显示界面操作
			{
				KeyDealDisplay(KeyPage,DISPLAY_MAX11UI,DISPLAY_MIN11PQS); 
			}	
			else	//三相四线最大值显示界面操作
			{
				KeyDealDisplay(KeyPage,DISPLAY_MAX34UN,DISPLAY_AVG34PQS); 
			}	
    	}	
    	else if(PAGE_MODULE == KeyPage) //扩展模块界面 
    	{
    		KeyDealDisplay(KeyPage,MODULEDI,MODULEREV);
    	}	
    	else if(PAGE_PR == KeyPage) //编程界面
    	{
    		KeyDealProgram();	
    	}	
    	LCDClrAll();		
    }
    
    Time2LCD = 50; //修正按键阴影变化的时间	
     	
}	

/***************************************************************
Name: void KeyDealMainmenu(void)
Function: 主菜单按键操作
Input:  None
Output: None
****************************************************************/
void KeyDealMainmenu(void)
{
    if (KeyValue == KEY_LEFT || KeyValue == KEY_RIGHT)
    {
    	KeyChangeOption(&DisplayItem,PAGE_EL,PAGE_PR);		
    }
    else if (KeyValue == KEY_ENTER)                            // ENTER
    {   
        KeyMainmenu = 0;                                                      
        if (PAGE_EL == DisplayItem)
        {
          	KeyPage = PAGE_EL;                                       // 显示电量
			if(INPUT11 == MeterSetData.c_input[0])	//单相
        	{
        		DisplayItem = DISPLAY_11UN;         // 电量显示操作
        	}
        	else if(INPUT33 == MeterSetData.c_input[0])//三相三线
        	{
        		DisplayItem = DISPLAY_33UL;         // 电量显示操作
        	}
        	else	//三相四线
        	{
        		DisplayItem = DISPLAY_34UN;         // 电量显示操作
        	}	
        }
        else if (PAGE_EN == DisplayItem) //进入电能界面
        {
            KeyPage = PAGE_EN; 
            DisplayItem = DISPLAY_EN;
        }
        else if(PAGE_QUALITY == DisplayItem) //进入电能质量
        {
        	KeyPage = PAGE_QUALITY;
        	DisplayItem = DISPLAY_QUAPHASE; //	
        }
        else if(PAGE_WAVE == DisplayItem) //进行电能质量界面
        {
        	KeyPage = PAGE_WAVE;
        	DisplayItem = DISPLAY_QUAUWAVE;	 //电压波形界面
        }
        else if(PAGE_DEMAND == DisplayItem) //需量界面
        {
        	KeyPage = PAGE_DEMAND; 
        	DisplayItem = DISPLAY_DEMANDMAX1;
        }
        else if(PAGE_MAX == DisplayItem) //极值记录
        {
        	KeyPage = PAGE_MAX;                                       // 显示电量
			if(INPUT11 == MeterSetData.c_input[0]) DisplayItem = DISPLAY_MAX11UI; 		//单相
        	else if(INPUT33 == MeterSetData.c_input[0]) DisplayItem = DISPLAY_MAX33UL; //三相三线
        	else DisplayItem = DISPLAY_MAX34UN;	//三相四线
        }	
        else if(PAGE_MODULE == DisplayItem) //模块
        {
        	KeyPage = PAGE_MODULE;
        	DisplayItem = MODULEDI;        		
        }	
        else if(PAGE_PR == DisplayItem)
        {
        	KeyPage = PAGE_PR;                                       // 显示编程界面
            ProgState = STEP_CODE;                            // 密码输入窗口
            ProgState2 = 0;
            KEY_XY[0][0] = 0;   // 在用户密码界面
            KEY_XY[0][1] = 1;
        }
  		
  		LCDClrAll();
    }
}



/****************************************************************************
Name: void KeyChangeOption(u16 *add, u16 n,u16 m) 
Function: 左右修改选项
Input:  add--修改的选项地址,n--选项最小值,m--选项最大值
Output: None
****************************************************************************/
void KeyChangeOption(u16 *add, u16 n,u16 m) 
{   
    if (KeyValue == KEY_LEFT)                                 // <-
    {
        if (*add < n + 1) *add = m;
        else (*add)--; 
    }
    else if(KeyValue == KEY_RIGHT)                            // ->
    { 
        if (*add > m - 1) *add = n;
        else (*add)++;      
    }    
}


/****************************************************************************
Name: void KeyDealDisplay(u8 page, u8 n, u8 m)    
Function: 电量显示/电能显示/谐波显示操作 
Input: page--显示界面,n--开始界面,m--结束界面
Output: None
****************************************************************************/     
void KeyDealDisplay(u8 page, u8 n, u8 m)                                        
{
	if(KEY_LEFT == KeyValue)                                 // <-
	{
	  if (DisplayItem < n+1) DisplayItem = m;
	  else DisplayItem--;  	
	  if(PAGE_QUALITY==KeyPage)
	  {
	  	if(5==MeterSetData.c_set[THDSET]) //63次谐波
	  	{
	  	}
	  	else if(4==MeterSetData.c_set[THDSET]) //51次谐波
	  	{
	  		//跳过非法数据界面
	  		if(DISPLAY_THDI6==DisplayItem) DisplayItem=DISPLAY_THDI5;
	  		if(DISPLAY_THDU6==DisplayItem) DisplayItem=DISPLAY_THDU5;
	  		if((DisplayItem>DISPLAY_THD47)&&(DisplayItem<DISPLAY_THDU1)) DisplayItem=	DISPLAY_THD47;
	  	}
	  	else //31次谐波
	  	{
	  		//跳过非法数据界面
	  		if((DisplayItem>DISPLAY_THDI3)) DisplayItem=DISPLAY_THDI3;
	  		if(DISPLAY_THDU6==DisplayItem) DisplayItem=DISPLAY_THDU3;
	  		if((DisplayItem>DISPLAY_THD27)&&(DisplayItem<DISPLAY_THDU1)) DisplayItem=	DISPLAY_THD27;
	  	}			
	  }  	
	}
	else if(KEY_RIGHT == KeyValue)                           // ->
	{
	    if (DisplayItem > m-1) DisplayItem = n;
	    else DisplayItem++;
	    if(PAGE_QUALITY==KeyPage)
		  {
		  	if(5==MeterSetData.c_set[THDSET]) //63次谐波
		  	{
		  	}
		  	else if(4==MeterSetData.c_set[THDSET]) //51次谐波
		  	{
		  		//跳过非法数据界面
		  		if(DisplayItem>DISPLAY_THDI5) DisplayItem=DISPLAY_QUAPHASE;
		  		if(DISPLAY_THDU6==DisplayItem) DisplayItem=DISPLAY_THDI1;
		  		if((DisplayItem>DISPLAY_THD47)&&(DisplayItem<DISPLAY_THDU1)) DisplayItem=	DISPLAY_THDU1;
		  	}
		  	else //31次谐波
		  	{
		  		//跳过非法数据界面
		  		if((DisplayItem>DISPLAY_THDI3)) DisplayItem=DISPLAY_QUAPHASE;
		  		if((DisplayItem>DISPLAY_THD27)&&(DisplayItem<DISPLAY_THDU1))  DisplayItem=DISPLAY_THDU1;
		  		if((DisplayItem>DISPLAY_THDU3)&&(DisplayItem<DISPLAY_THDI1))  DisplayItem=	DISPLAY_THDI1;
		  	}	
		  } 	
	}	      
  	else if(KEY_MENU == KeyValue)                            // MENU
  	{
     	KeyMainmenu = 1;   
     	if(PAGE_ELBAR == page) page = PAGE_EL;                                   
      	DisplayItem =  page;                 // 返回主菜单界面
  	}
  	else if(KEY_ENTER == KeyValue)
  	{
  		if(PAGE_EL==page) KeyPage=PAGE_ELBAR;
  		else if(PAGE_ELBAR==page) KeyPage=PAGE_EL; 	
  		
  	}	

}

/**********************************************************
Name: void KeyDealProgram(void)
Function: 编程操作界面
Input:  None
Output: None
**********************************************************/
void KeyDealProgram(void)
{
	switch(ProgState)
	{
		case STEP_CODE:
			KeyProgramCode();	// 密码输入操作
			break;
		case STEP_1:
			KeyProgramStep1();
			break;
		case STEP_2:
			KeyProgramStep2(); 	// 第二级菜单操作
			break;	
		case STEP_3:
			KeyProgramStep3();  // 第三级菜单操作
			break;	
		case STEP_4:
			KeyProgramStep4();	//第四菜单操作（继电器输出）
			break;
		case STEP_5:
			KeyProgramStep5();	//第五级菜单操作(继电器操作)
			break;
		case STEP_SAVE:
			KeyProgramSave();                             // 保存菜单操作
			break;	
		case STEP_CAL:	
			KeyDealCalibrate();                           // 校准菜单操作
			break;	
		default:
			break;		 
				
			
	}
	
}

/**********************************************************
Name: void KeyProgramCode(void)
Funcion: 密码输入炒作
Input:  None
Output: None
**********************************************************/
void KeyProgramCode(void)
{
	u8 i;
	if(0 == ProgState2) //在密码菜单选项
	{
		if(KEY_ENTER == KeyValue)
		{
			ProgState2=1;
			KEY_XY[2][0] = 0x00;
			KEY_XY[2][1] = 9999;
			KeyMult = 0; 	
		}	
		else if(KEY_MENU == KeyValue)
		{
			KeyMainmenu = 1;                                      
		}	
		else
		{
			Change99((u16 *)&KEY_XY[0][0], KEY_XY[0][1]);	
		}	
	}
	else if(1 == ProgState2) //在密码修改界面
	{
		if (KeyValue == KEY_ENTER)                    // ENTER
	    {
	        if ((0x00 == KEY_XY[0][0]) && ((KEY_XY[2][0] == MeterSetData.c_password)||(KEY_XY[2][0] == PROG_PASSWORD)))     // 编程密码
	        {
	             ProgState = STEP_1;
	             KEY_XY[0][0] = STEP_1_SYS;
	             KEY_XY[0][1] = STEP_1_CLR; //
	             for(i=0; i<3; i++) KeyClr[i]=0;
	             
	        }
	        else if ((KEY_XY[2][0] == CAL_PASSWORD) && (0x01 == KEY_XY[0][0]))            // 校准密码
	        {
	             ProgState = STEP_CAL;
	             CalState = STEP_1;
	             KEY_XY[0][0] = CAL_1_CAL;
	             KEY_XY[0][1] = CAL_1_LIMMIT;
				 DAAjustMark = 0xaa; //进入校准界面
	        }
	    }
	    else if (KeyValue == KEY_MENU)                     // MENU
	    {
	    	ProgState2 = 0;                  
	    }
	    else
	    {
	        Change999999(&KEY_XY[2][0],&KeyMult, KEY_XY[2][1]);              // 设置密码
	    }			
	}		
}

/******************************************************
Name: void KeyProgramStep1(void)
Function: 系统设置第一级菜单操作
Input:  None
Output: None
*******************************************************/
void KeyProgramStep1(void)
{
	if(KEY_MENU == KeyValue)
	{
		ProgState = STEP_SAVE;
		KEY_XY[2][0] = 0;
		KEY_XY[2][1] = 1;
	}
	else if(KEY_ENTER == KeyValue) KeyIntoStep2();
	else
	{
		Change99((u16 *)KEY_XY[0], (u16)KEY_XY[0][1]);
		if((0==MeterSetData.c_set[TOUSET])&&(STEP_1_TOU==KEY_XY[0][0]))
		{
			if(KEY_LEFT==KeyValue) KEY_XY[0][0]--;
			else if(KEY_RIGHT==KeyValue) KEY_XY[0][0]++;	
		}	
	}			
}

/*******************************************************
Name: void KeyIntoStep2(void)
Function: 进入第二级菜单
Input:  None
Output: None
*******************************************************/
void KeyIntoStep2(void)
{
	ProgState = STEP_2;
	KEY_XY[1][0] = 0;
	if(STEP_1_SYS == KEY_XY[0][0]) KEY_XY[1][1]=STEP_2_DISPLAY;
	else if(STEP_1_INPUT == KEY_XY[0][0]) KEY_XY[1][1] = 4;
	else if(STEP_1_COMM == KEY_XY[0][0]) KEY_XY[1][1] = 3;
	else if(STEP_1_DEMAND == KEY_XY[0][0]) KEY_XY[1][1]=1; 	
	else if(STEP_1_OUTPUT == KEY_XY[0][0])
	{
		if(0==MeterSetData.c_set[DOSET]) ProgState=STEP_1; //关闭不进入系统设置
		else KEY_XY[1][1]=2;
	} 
	else if(STEP_1_AOOUT==KEY_XY[0][0]) 
	{
		if(0==MeterSetData.c_set[AOSET]) ProgState=STEP_1; //关闭不进入系统设置
		else
		{	
			if(2==MeterSetData.c_set[AOSET]) KEY_XY[1][1]=1;
			else KEY_XY[1][1]=0;
		}
	}	
	else if(STEP_1_ALARM==KEY_XY[0][0]) //系统报警
	{
		KEY_XY[1][1]=STEP_2_ALARMIUN;
	}	
	else if(STEP_1_TOU==KEY_XY[0][0]) KEY_XY[1][1]=STEP_2_TIME;
	else if(STEP_1_CLR==KEY_XY[0][0]) KEY_XY[1][1]=2;
	
}


/*********************************************************
Name: void KeyProgramStep2(void)
Function: 系统设置第二级菜单操作
*********************************************************/
void KeyProgramStep2(void)
{
	if(KEY_MENU == KeyValue)
	{
		ProgState = STEP_1;	
	}	
	else if(KEY_ENTER == KeyValue)
	{
		KeyIntoStep3();
	}
	else
	{
		Change99((u16 *)KEY_XY[1], (u16)KEY_XY[1][1]);
	}		
		
}


/*******************************************************
Name: void KeyIntoStep3(void)
Function: 进入第三级菜单,将修改参数赋值给KEY_XY[2][0]
Input:  None
Output: None
*******************************************************/
void KeyIntoStep3(void)
{
	ProgState = STEP_3;
	KeyMult = 0; //按键位置
	if(STEP_1_SYS == KEY_XY[0][0]) 
	{
		switch(KEY_XY[1][0])
		{
			case STEP_2_CODE:
				KEY_XY[2][0]= MeterSetData.c_password;
				KEY_XY[2][1]=9999;
				break;
			case STEP_2_LIGHT:
				KEY_XY[2][0]=MeterSetData.c_lcdlight;
				KEY_XY[2][1]=255;
				break;
			case STEP_2_GARY:
				KEY_XY[2][0]=MeterSetData.c_lcdgray;
				KEY_XY[2][1]=9;
				break;
			case STEP_2_LANGUAGE:
				KEY_XY[2][0]=MeterSetData.c_lcdlanguage;
				KEY_XY[2][1]=1;
				break;
			case STEP_2_DISPLAY:
				KEY_XY[2][0] = MeterSetData.c_powerview;
				KEY_XY[2][1] = POWERMAX;
				break;
			default:
				break;
		}
	}
	else if(STEP_1_INPUT == KEY_XY[0][0]) 
	{
		switch(KEY_XY[1][0])
		{
			case STEP_2_INPUT:
				KEY_XY[2][0]=MeterSetData.c_input[0];
				KEY_XY[2][1]=INPUT11;
				break;
			case STEP_2_PT2:
				KEY_XY[2][0]=MeterSetData.c_pt2;
				KEY_XY[2][1]=690;
				break;
			case STEP_2_CT2:
				KEY_XY[2][0]=MeterSetData.c_ct2;
				KEY_XY[2][1]=6;
				break;
			case STEP_2_PT1:
				KEY_XY[2][0]=MeterSetData.c_pt1;
				KEY_XY[2][1]=999999;
				break;
			case STEP_2_CT1:
				KEY_XY[2][0]=MeterSetData.c_ct1;
				KEY_XY[2][1]=999999;
				break;
			default:
				break;
		}
	}
	else if(STEP_1_COMM == KEY_XY[0][0])
	{
		switch(KEY_XY[1][0])
		{
			case STEP_2_SN:
				KEY_XY[2][0]=MeterSetData.c_com1[0];
				KEY_XY[2][1]=247;
				break;
			case STEP_2_BAUD:
				KEY_XY[2][0]=MeterSetData.c_com1[1];
				KEY_XY[2][1]=5;
				break;
			case STEP_2_DATA:
				KEY_XY[2][0]=MeterSetData.c_com1[2];
				KEY_XY[2][1]=3;
				break;
			case STEP_2_PROT:
				KEY_XY[2][0]=MeterSetData.c_com1[3];
				KEY_XY[2][1]=1;
				break;
			default:
				break;	
		}
	}
	else if(STEP_1_DEMAND == KEY_XY[0][0]) //需量设置
	{
		if(0==KEY_XY[1][0])
		{
			KEY_XY[2][0] = 1;
			KEY_XY[2][1] = 3;	
		}
		else
		{
			KEY_XY[2][0] = MeterSetData.c_MaxMinTime;
			KEY_XY[2][1] = 9999;
		}	
	}
	else if(STEP_1_OUTPUT == KEY_XY[0][0]) //继电器输出设置
	{ 
		if(0==MeterSetData.c_set[DOSET]) ProgState = STEP_1;
		else
		{	
			KEY_XY[2][0]=MeterSetData.c_DO[KEY_XY[1][0]][0];
			KEY_XY[2][1]=STEP_3_DOREM;
		}
	}
	else if(STEP_1_AOOUT==KEY_XY[0][0]) 
	{
		if(0==MeterSetData.c_set[AOSET]) ProgState = STEP_1;
		else
		{
			KEY_XY[2][0] = 0;
			if(STEP_4_AOOFF==MeterSetData.c_AO[KEY_XY[1][0]][0]) KEY_XY[2][1]=0; //变送类型为关闭模式不能选择后面对项目
			else KEY_XY[2][1] = STEP_2_AOFS;	
		}
	}
	else if(STEP_1_ALARM==KEY_XY[0][0])
	{
		KEY_XY[2][0]=0;
		if(KEY_XY[1][0]>=STEP_2_ALARMPF) KEY_XY[2][1] = STEP_31_ALARMDELAY;
		else KEY_XY[2][1] = STEP_3_ALARMDELAY;		
	}		
	else if(STEP_1_CLR == KEY_XY[0][0])
	{
		KEY_XY[2][0] = KeyClr[KEY_XY[1][0]];
		KEY_XY[2][1] = 1;
			
	}
	else if(STEP_1_TOU==KEY_XY[0][0])
	{
		if(0==MeterSetData.c_set[TOUSET])
		{
			ProgState = STEP_1;	
		}
		else
		{
			KEY_XY[2][0]=0;
			switch(KEY_XY[1][0])
			{
				case STEP_2_FSD1:
				case STEP_2_FSD2:
				case STEP_2_MONS:
					KEY_XY[2][1] = STEP_3_SD12;
					break;	
				case STEP_2_COPY:
					KEY_XY[2][1] = STEP_3_COPYH;
					break;
				case STEP_2_TIME:
					KEY_XY[2][1] = 5;
					break;
			}			
		}
	}
	else if(STEP_1_CLR==KEY_XY[0][0])
	{
		KEY_XY[2][0] = KeyClr[KEY_XY[1][0]];
		KEY_XY[2][1]=1;	
	}		
}

/*********************************************************
Name: void KeyProgramStep3(void)
Function: 系统设置第三级菜单操作
Input:  None
Output: None
*********************************************************/
void KeyProgramStep3(void)
{
	if(KEY_MENU==KeyValue) //返回上一级菜单
	{
		ProgState = STEP_2;	
		if((STEP_1_SYS==KEY_XY[0][0])&&(STEP_2_GARY == KEY_XY[1][0]))
		{
			LCDCmd(0x81);  //对比度设置
			LCDData((u8)LCDGRAY[MeterSetData.c_lcdgray]);  //00--3F  //24
    		LCDData((u8)(LCDGRAY[MeterSetData.c_lcdgray]>>8));  
		}	
	}	
	else if(KEY_ENTER == KeyValue) //确认件操作
	{
		KeyBackStep2();	
	}
	else //左右键修改数据
	{
		if((STEP_1_DEMAND == KEY_XY[0][0])&&(STEP_2_DEMAND==KEY_XY[1][0]))
		{
		 //光标不在第一个项目选项
		 	Change99((u16 *)KEY_XY[2],KEY_XY[2][1]);
		 	if((0 == KEY_XY[1][0])&&(0==KEY_XY[2][0]))
		 	{
		 		if(KEY_LEFT==KeyValue) KEY_XY[2][0] = 3;
		 		else KEY_XY[2][0] = 1;	
		 	}		
		}	
		else if((STEP_1_AOOUT==KEY_XY[0][0])||(STEP_1_ALARM==KEY_XY[0][0])||(STEP_1_TOU==KEY_XY[0][0])) 
		{
			Change99((u16 *)KEY_XY[2],KEY_XY[2][1]);
		}
		else
		{
			Change999999(&KEY_XY[2][0],&KeyMult, KEY_XY[2][1]); 
		 	if((STEP_1_SYS==KEY_XY[0][0])&&(STEP_2_GARY == KEY_XY[1][0]))
		 	{
		 		LCDCmd(0x81);  //对比度设置
		 		LCDData((u8)LCDGRAY[KEY_XY[2][0]]);  //00--3F  //24
    			LCDData((u8)(LCDGRAY[KEY_XY[2][0]]>>8));  
		 	}	
		 	if((STEP_1_COMM==KEY_XY[0][0])&&(0==KEY_XY[1][0])&&(0==KEY_XY[2][0])) KEY_XY[2][0]=1;	 	
		}			
	}	
}


/*********************************************************
Name: void KeyBackStep2(void)
Function: 返回第二级菜单或进入第四级菜单
*********************************************************/
void KeyBackStep2(void)
{
	u8 i;

	ProgState = STEP_2;
	if(STEP_1_SYS==KEY_XY[0][0])
	{
		ProgState = STEP_2;
		switch(KEY_XY[1][0])
		{
			case STEP_2_CODE:
				MeterSetData.c_password=(u16)KEY_XY[2][0];
				break;
			case STEP_2_LIGHT:
				MeterSetData.c_lcdlight=(u8)KEY_XY[2][0];
				break;
			case STEP_2_GARY:
				MeterSetData.c_lcdgray=(u8)KEY_XY[2][0];
				break;
			case STEP_2_LANGUAGE:
				MeterSetData.c_lcdlanguage = (u8)KEY_XY[2][0];
				break;
			case STEP_2_DISPLAY:
				MeterSetData.c_powerview = (u8)KEY_XY[2][0];
				break;
			default:
				break; 	
		}
	}		
	else if(STEP_1_INPUT == KEY_XY[0][0])
	{
		ProgState = STEP_2;	
		switch(KEY_XY[1][0])
		{
			case STEP_2_INPUT:
				MeterSetData.c_input[0]=KEY_XY[2][0];
				break;
			case STEP_2_PT2:
				if(KEY_XY[2][0]>0) MeterSetData.c_pt2=KEY_XY[2][0];
				break;
			case STEP_2_CT2:
				if(KEY_XY[2][0]>0) MeterSetData.c_ct2=KEY_XY[2][0];
				break;
			case STEP_2_PT1:
				if(KEY_XY[2][0]>0) MeterSetData.c_pt1=KEY_XY[2][0];
				break;
			case STEP_2_CT1:
				if(KEY_XY[2][0]>0) MeterSetData.c_ct1=KEY_XY[2][0];
				break;
			default:
				break;
		}
	}	
	else if(STEP_1_COMM == KEY_XY[0][0])
	{
		ProgState = STEP_2;	
		if(0==KEY_XY[1][0])
		{
			if(KEY_XY[2][0]>0) MeterSetData.c_com1[KEY_XY[1][0]]=KEY_XY[2][0];
		}
		else 	MeterSetData.c_com1[KEY_XY[1][0]]=KEY_XY[2][0];
	}
	else if(STEP_1_DEMAND==KEY_XY[0][0]) //需量设置
	{
		if(KEY_XY[1][0]==STEP_2_DEMAND)
		{
			ProgState=STEP_4;
			KEY_XY[3][0] = MeterSetData.c_demand[KEY_XY[1][0]][KEY_XY[2][0]];
			if(0==KEY_XY[2][0]) KEY_XY[3][1] = 0; //项目,暂时只到S
			else if(1==KEY_XY[2][0]) KEY_XY[3][1] = 1; //模式
			else if(2==KEY_XY[2][0]) KEY_XY[3][1] = 9999;
			else if(3==KEY_XY[2][0]) KEY_XY[3][1] = 30;
		}
		else
		{
			ProgState = STEP_2;	
			MeterSetData.c_MaxMinTime=KEY_XY[2][0];
		}	
	}	
	else if(STEP_1_OUTPUT==KEY_XY[0][0])
	{
		//根据继电器输出的设置，配置相应的操作
		if(STEP_3_DOOFF==KEY_XY[2][0]) //继电器关闭
		{
			ProgState=STEP_2;
			MeterSetData.c_DO[KEY_XY[1][0]][0]=KEY_XY[2][0];	
		}		
		else 
		{
			ProgState=STEP_4;
			KEY_XY[3][0]=0;
			KeyMult = 0;
			MeterSetData.c_DO[KEY_XY[1][0]][0]=KEY_XY[2][0]; 	//保存模式设置
			if(STEP_3_DOREM==KEY_XY[2][0]) KEY_XY[3][1]=0; 		//遥控模式
			else
			{
				if(MeterSetData.c_DO[KEY_XY[1][0]][2]>STEP_5_ITHDL) KEY_XY[3][1]=STEP_4_ITEM;	//开关量联动
				else KEY_XY[3][1]=STEP_4_DELAY;	//报警设置	
			} 
		}		
	}
	else if(STEP_1_AOOUT==KEY_XY[0][0]) //变送输出
	{
		ProgState=STEP_4;
		KEY_XY[3][0]=MeterSetData.c_AO[KEY_XY[1][0]][KEY_XY[2][0]];
		if(STEP_2_AOMODE==KEY_XY[2][0]) KEY_XY[3][1]=STEP_4_AO412;
		else if(STEP_2_AOITEM==KEY_XY[2][0]) KEY_XY[3][1]=STEP_4_AOF;
		else if(STEP_2_AODS==KEY_XY[2][0]) KEY_XY[3][1]=9999;
		else if(STEP_2_AOFS==KEY_XY[2][0]) KEY_XY[3][1]=9999;
	}
	else if(STEP_1_ALARM==KEY_XY[0][0]) //越限报警设置
	{
		ProgState = STEP_4;
		KeyMult=0;
		if(STEP_2_ALARMUN==KEY_XY[1][0]) //进入相电压报警界面
		{
			if(STEP_3_ALARMMODE==KEY_XY[2][0])
			{
				KEY_XY[3][0] = (MeterSetData.c_AlarmEnable[0]>>BITALARM_UNH)&1;
				KEY_XY[3][1] = 1;
			}	
			else if(KEY_XY[2][0]<STEP_3_ALARM2MODE)
			{
				KEY_XY[3][0] = MeterSetData.c_AlarmUn[KEY_XY[2][0]-STEP_3_ALARMVALUEH];
				KEY_XY[3][1] = 9999;
			}
			else if(KEY_XY[2][0]<STEP_3_ALARMVALUEL)
			{
				KEY_XY[3][0] = (MeterSetData.c_AlarmEnable[0]>>BITALARM_UNL)&1;
				KEY_XY[3][1] = 1;
			}
			else 
			{
				KEY_XY[3][0] = MeterSetData.c_AlarmUn[KEY_XY[2][0]-STEP_3_ALARMVALUEHYSH];
				KEY_XY[3][1] = 9999;
			}		
		}
		else if(STEP_2_ALARMUL==KEY_XY[1][0]) //进入线电压报警界面
		{
			if(STEP_3_ALARMMODE==KEY_XY[2][0])
			{
				KEY_XY[3][0] = (MeterSetData.c_AlarmEnable[0]>>BITALARM_ULH)&1;
				KEY_XY[3][1] = 1;
			}	
			else if(KEY_XY[2][0]<STEP_3_ALARM2MODE)
			{
				KEY_XY[3][0] = MeterSetData.c_AlarmUl[KEY_XY[2][0]-STEP_3_ALARMVALUEH];
				KEY_XY[3][1] = 9999;
			}
			else if(KEY_XY[2][0]<STEP_3_ALARMVALUEL)
			{
				KEY_XY[3][0] = (MeterSetData.c_AlarmEnable[0]>>BITALARM_ULL)&1;
				KEY_XY[3][1] = 1;
			}
			else 
			{
				KEY_XY[3][0] = MeterSetData.c_AlarmUl[KEY_XY[2][0]-STEP_3_ALARMVALUEHYSH];
				KEY_XY[3][1] = 9999;
			}		
		}
		else if(STEP_2_ALARMI==KEY_XY[1][0]) //进入电流报警界面
		{
			if(STEP_3_ALARMMODE==KEY_XY[2][0])
			{
				KEY_XY[3][0] = (MeterSetData.c_AlarmEnable[0]>>BITALARM_IH)&1;
				KEY_XY[3][1] = 1;
			}	
			else if(KEY_XY[2][0]<STEP_3_ALARM2MODE)
			{
				KEY_XY[3][0] = MeterSetData.c_AlarmI[KEY_XY[2][0]-STEP_3_ALARMVALUEH];
				KEY_XY[3][1] = 9999;
			}
			else if(KEY_XY[2][0]<STEP_3_ALARMVALUEL)
			{
				KEY_XY[3][0] = (MeterSetData.c_AlarmEnable[0]>>BITALARM_IL)&1;
				KEY_XY[3][1] = 1;
			}
			else 
			{
				KEY_XY[3][0] = MeterSetData.c_AlarmI[KEY_XY[2][0]-STEP_3_ALARMVALUEHYSH];
				KEY_XY[3][1] = 9999;
			}		
		}
		else if(STEP_2_ALARMF==KEY_XY[1][0]) //进入频率报警界面
		{
			if(STEP_3_ALARMMODE==KEY_XY[2][0])
			{
				KEY_XY[3][0] = (MeterSetData.c_AlarmEnable[0]>>BITALARM_FH)&1;
				KEY_XY[3][1] = 1;
			}	
			else if(KEY_XY[2][0]<STEP_3_ALARM2MODE)
			{
				KEY_XY[3][0] = MeterSetData.c_AlarmF[KEY_XY[2][0]-STEP_3_ALARMVALUEH];
				KEY_XY[3][1] = 9999;
			}
			else if(KEY_XY[2][0]<STEP_3_ALARMVALUEL)
			{
				KEY_XY[3][0] = (MeterSetData.c_AlarmEnable[0]>>BITALARM_FL)&1;
				KEY_XY[3][1] = 1;
			}
			else 
			{
				KEY_XY[3][0] = MeterSetData.c_AlarmF[KEY_XY[2][0]-STEP_3_ALARMVALUEHYSH];
				KEY_XY[3][1] = 9999;
			}		
		}
		else if(STEP_2_ALARMP==KEY_XY[1][0]) //进入功率报警界面
		{
			if(STEP_3_ALARMMODE==KEY_XY[2][0])
			{
				KEY_XY[3][0] = (MeterSetData.c_AlarmEnable[0]>>BITALARM_PH)&1;
				KEY_XY[3][1] = 1;
			}	
			else if(KEY_XY[2][0]<STEP_3_ALARM2MODE)
			{
				KEY_XY[3][0] = MeterSetData.c_AlarmP[KEY_XY[2][0]-STEP_3_ALARMVALUEH];
				KEY_XY[3][1] = 9999;
			}
			else if(KEY_XY[2][0]<STEP_3_ALARMVALUEL)
			{
				KEY_XY[3][0] = (MeterSetData.c_AlarmEnable[0]>>BITALARM_PL)&1;
				KEY_XY[3][1] = 1;
			}
			else 
			{
				KEY_XY[3][0] = MeterSetData.c_AlarmP[KEY_XY[2][0]-STEP_3_ALARMVALUEHYSH];
				KEY_XY[3][1] = 9999;
			}		
		}
		else if(STEP_2_ALARMPF==KEY_XY[1][0]) //进入功率因数报警界面
		{
			if(STEP_3_ALARMMODE==KEY_XY[2][0])
			{
				KEY_XY[3][0] = (MeterSetData.c_AlarmEnable[0]>>BITALARM_PFL)&1;
				KEY_XY[3][1] = 1;
			}	
			else 
			{
				KEY_XY[3][0] = MeterSetData.c_AlarmPF[KEY_XY[2][0]-STEP_31_ALARMVALUEH];
				KEY_XY[3][1] = 9999;
			}		
		}
		else if(STEP_2_ALARMTHDU==KEY_XY[1][0]) //进入电压总谐波畸变率
		{
			if(STEP_3_ALARMMODE==KEY_XY[2][0])
			{
				KEY_XY[3][0] = (MeterSetData.c_AlarmEnable[0]>>BITALARM_THDUH)&1;
				KEY_XY[3][1] = 1;
			}	
			else 
			{
				KEY_XY[3][0] = MeterSetData.c_AlarmTHDU[KEY_XY[2][0]-STEP_31_ALARMVALUEH];
				KEY_XY[3][1] = 9999;
			}		
		}
		else if(STEP_2_ALARMTHDI==KEY_XY[1][0]) //进入电流总谐波畸变率
		{
			if(STEP_3_ALARMMODE==KEY_XY[2][0])
			{
				KEY_XY[3][0] = (MeterSetData.c_AlarmEnable[1]>>(BITALARM_THDIH-BITALARM_THDIH))&1;
				KEY_XY[3][1] = 1;
			}	
			else 
			{
				KEY_XY[3][0] = MeterSetData.c_AlarmTHDI[KEY_XY[2][0]-STEP_31_ALARMVALUEH];
				KEY_XY[3][1] = 9999;
			}		
		}
		else if(STEP_2_ALARMUUN==KEY_XY[1][0]) //进入电压不平衡度高报警
		{
			if(STEP_3_ALARMMODE==KEY_XY[2][0])
			{
				KEY_XY[3][0] = (MeterSetData.c_AlarmEnable[1]>>(BITALARM_UUNH-BITALARM_THDIH))&1;
				KEY_XY[3][1] = 1;
			}	
			else 
			{
				KEY_XY[3][0] = MeterSetData.c_AlarmUun[KEY_XY[2][0]-STEP_31_ALARMVALUEH];
				KEY_XY[3][1] = 9999;
			}		
		}
		else if(STEP_2_ALARMIUN==KEY_XY[1][0]) //进入电流不平衡度高报警
		{
			if(STEP_3_ALARMMODE==KEY_XY[2][0])
			{
				KEY_XY[3][0] = (MeterSetData.c_AlarmEnable[1]>>(BITALARM_IUNH-BITALARM_THDIH))&1;
				KEY_XY[3][1] = 1;
			}	
			else 
			{
				KEY_XY[3][0] = MeterSetData.c_AlarmIun[KEY_XY[2][0]-STEP_31_ALARMVALUEH];
				KEY_XY[3][1] = 9999;
			}		
		}
	}		
	else if(STEP_1_TOU==KEY_XY[0][0]) //复费率部分设置
	{
		ProgState = STEP_4;
		if((STEP_2_FSD1==KEY_XY[1][0])||(STEP_2_FSD2==KEY_XY[1][0])) //复费率1
		{
			KEY_XY[3][0] = 0;
			KEY_XY[3][1] = 2;
		}
		else if(STEP_2_MONS==KEY_XY[1][0]) //月费率选择
		{
			KEY_XY[3][0] = sd_Data.c_mons[KEY_XY[2][0]];
			KEY_XY[3][1] = 1;
		}
		else if(STEP_2_COPY==KEY_XY[1][0]) //抄表日
		{
			if(0==KEY_XY[2][0]) KEY_XY[3][1] = 28;
			else if(1==KEY_XY[2][0]) KEY_XY[3][1]=23;
			KEY_XY[3][0] = sd_Data.c_copyday[KEY_XY[2][0]];
		}	
		else if(STEP_2_TIME==KEY_XY[1][0])	
		{
			if(0==KEY_XY[2][0]) KEY_XY[3][1] = 99; //年
			else if(1==KEY_XY[2][0]) KEY_XY[3][1] = 12; //月
			else if(2==KEY_XY[2][0]) KEY_XY[3][1] = 31; //日
			else if(3==KEY_XY[2][0]) KEY_XY[3][1] = 23; //时
			else KEY_XY[3][1] = 59; //分， 秒
			for(i=0; i<6; i++) KeyTime[i] = sd_Data.c_time[i]; //
			KEY_XY[3][0] = KeyTime[KEY_XY[2][0]];		
		}	
	}			
	else if(STEP_1_CLR == KEY_XY[0][0]) //清除同步
	{
		KeyClr[KEY_XY[1][0]] = KEY_XY[2][0];	
	}	
	else //无效或不需设置界面则不响应Eneter按键操作
	{	
	}	
}

/*************************************************************
Name: void KeyProgramStep4(void)
Function: 系统设置第四级菜单,目前只有继电器输出需要该深度
*************************************************************/
void KeyProgramStep4(void)
{
	if(KEY_MENU==KeyValue) //返回第3级菜单
	{
		ProgState=STEP_3;
		if(STEP_1_OUTPUT==KEY_XY[0][0])
		{
//			KEY_XY[2][0]=MeterSetData.c_DO[KEY_XY[1][0]][0];
//			KEY_XY[2][1]=STEP_3_DOEN;
		}
	}	
	else if(KEY_ENTER==KeyValue) //进入第5级菜单
	{
		if(STEP_1_OUTPUT==KEY_XY[0][0])
		{
			ProgState=STEP_5;
			if(STEP_3_DOREM==MeterSetData.c_DO[KEY_XY[1][0]][0]) //遥控模式
			{
				KEY_XY[4][0]=MeterSetData.c_DO[KEY_XY[1][0]][1]; //脉宽
				KEY_XY[4][1]=9999;
			}		
			else if(STEP_3_DOALR==MeterSetData.c_DO[KEY_XY[1][0]][0]) //报警模式
			{
				KEY_XY[4][0]=MeterSetData.c_DO[KEY_XY[1][0]][KEY_XY[3][0]+1];
				KeyMult = 0;
				switch(KEY_XY[3][0])
				{
					case STEP_4_ITEM:
						if(1==MeterSetData.c_set[DISET]) KEY_XY[4][1] = STEP_5_DIN20; //报警项目
						else if(2==MeterSetData.c_set[DISET]) KEY_XY[4][1] = STEP_5_DIN40; //报警项目
						else if(3==MeterSetData.c_set[DISET]) KEY_XY[4][1] = STEP_5_DIN60; //报警项目
						else KEY_XY[4][1] = STEP_5_ITHDL; //报警项目
						break;	
					case STEP_4_PULSE:
						KEY_XY[4][1]=9999;
						break;
					case STEP_4_VALUE:
						KEY_XY[4][1]=9999;
						break;
					case STEP_4_HYS:
						KEY_XY[4][1]=9999;
						break;
					case STEP_4_DELAY:
						KEY_XY[4][1]=9999;
						break;
					default:
						break;
				}
			}
		}
		else if(STEP_1_DEMAND==KEY_XY[0][0]) //需量更改界面
		{
			ProgState = STEP_3;
			if(0 == KEY_XY[3][0])
			{
				if(2==KEY_XY[2][0]) KEY_XY[3][0] = 60;
				else if(3==KEY_XY[2][0]) KEY_XY[3][0] = 15;	
			}	
			MeterSetData.c_demand[KEY_XY[1][0]][KEY_XY[2][0]] = KEY_XY[3][0];
		}
		else if(STEP_1_AOOUT==KEY_XY[0][0]) //变送输出
		{
			ProgState=STEP_3;
			MeterSetData.c_AO[KEY_XY[1][0]][KEY_XY[2][0]] = KEY_XY[3][0];	
			if(STEP_2_AOMODE==KEY_XY[2][0])
			{
				if(STEP_4_AOOFF==KEY_XY[3][0]) KEY_XY[2][1]=STEP_2_AOMODE;
				else KEY_XY[2][1]=STEP_2_AOFS;	
			}	
		}
		else if(STEP_1_ALARM==KEY_XY[0][0]) //报警参数
		{
			ProgState=STEP_3;
			if(STEP_2_ALARMUN==KEY_XY[1][0]) //相电压报警界面
			{
				if(STEP_3_ALARMMODE==KEY_XY[2][0])
				{
					MeterSetData.c_AlarmEnable[0] &= ~(1<<BITALARM_UNH);
					MeterSetData.c_AlarmEnable[0] |= (KEY_XY[3][0]<<BITALARM_UNH);
				}	
				else if(KEY_XY[2][0]<STEP_3_ALARM2MODE)
				{
					MeterSetData.c_AlarmUn[KEY_XY[2][0]-STEP_3_ALARMVALUEH]=KEY_XY[3][0];
				}
				else if(KEY_XY[2][0]<STEP_3_ALARMVALUEL)
				{
					MeterSetData.c_AlarmEnable[0] &= ~(1<<BITALARM_UNL);
					MeterSetData.c_AlarmEnable[0] |= (KEY_XY[3][0]<<BITALARM_UNL);
				}
				else 
				{
					MeterSetData.c_AlarmUn[KEY_XY[2][0]-STEP_3_ALARMVALUEHYSH]=KEY_XY[3][0];
				}		
			}
			else if(STEP_2_ALARMUL==KEY_XY[1][0]) //进入线电压报警界面
			{
				if(STEP_3_ALARMMODE==KEY_XY[2][0])
				{
					MeterSetData.c_AlarmEnable[0] &= ~(1<<BITALARM_ULH);
					MeterSetData.c_AlarmEnable[0] |= (KEY_XY[3][0]<<BITALARM_ULH);
				}	
				else if(KEY_XY[2][0]<STEP_3_ALARM2MODE)
				{
					MeterSetData.c_AlarmUl[KEY_XY[2][0]-STEP_3_ALARMVALUEH]=KEY_XY[3][0];
				}
				else if(KEY_XY[2][0]<STEP_3_ALARMVALUEL)
				{
					MeterSetData.c_AlarmEnable[0] &= ~(1<<BITALARM_ULL);
					MeterSetData.c_AlarmEnable[0] |= (KEY_XY[3][0]<<BITALARM_ULL);
				}
				else 
				{
					MeterSetData.c_AlarmUl[KEY_XY[2][0]-STEP_3_ALARMVALUEHYSH]=KEY_XY[3][0];
				}		
			}
			else if(STEP_2_ALARMI==KEY_XY[1][0]) //进入电流报警界面
			{
				if(STEP_3_ALARMMODE==KEY_XY[2][0])
				{
					MeterSetData.c_AlarmEnable[0] &= ~(1<<BITALARM_IH);
					MeterSetData.c_AlarmEnable[0] |= (KEY_XY[3][0]<<BITALARM_IH);
				}	
				else if(KEY_XY[2][0]<STEP_3_ALARM2MODE)
				{
					MeterSetData.c_AlarmI[KEY_XY[2][0]-STEP_3_ALARMVALUEH]=KEY_XY[3][0];
				}
				else if(KEY_XY[2][0]<STEP_3_ALARMVALUEL)
				{
					MeterSetData.c_AlarmEnable[0] &= ~(1<<BITALARM_IL);
					MeterSetData.c_AlarmEnable[0] |= (KEY_XY[3][0]<<BITALARM_IL);
				}
				else 
				{
					MeterSetData.c_AlarmI[KEY_XY[2][0]-STEP_3_ALARMVALUEHYSH]=KEY_XY[3][0];
				}		
			}
			else if(STEP_2_ALARMF==KEY_XY[1][0]) //进入频率报警界面
			{
				if(STEP_3_ALARMMODE==KEY_XY[2][0])
				{
					MeterSetData.c_AlarmEnable[0] &= ~(1<<BITALARM_FH);
					MeterSetData.c_AlarmEnable[0] |= (KEY_XY[3][0]<<BITALARM_FH);
				}	
				else if(KEY_XY[2][0]<STEP_3_ALARM2MODE)
				{
					MeterSetData.c_AlarmF[KEY_XY[2][0]-STEP_3_ALARMVALUEH]=KEY_XY[3][0];
				}
				else if(KEY_XY[2][0]<STEP_3_ALARMVALUEL)
				{
					MeterSetData.c_AlarmEnable[0] &= ~(1<<BITALARM_FL);
					MeterSetData.c_AlarmEnable[0] |= (KEY_XY[3][0]<<BITALARM_FL);
				}
				else 
				{
					MeterSetData.c_AlarmF[KEY_XY[2][0]-STEP_3_ALARMVALUEHYSH]=KEY_XY[3][0];
				}		
			}
			else if(STEP_2_ALARMP==KEY_XY[1][0]) //进入功率报警界面
			{
				if(STEP_3_ALARMMODE==KEY_XY[2][0])
				{
					MeterSetData.c_AlarmEnable[0] &= ~(0x0015<<BITALARM_PH); //PQS同时控制
					if(KEY_XY[3][0]) MeterSetData.c_AlarmEnable[0] |= (0x0015<<BITALARM_PH);
				}	
				else if(KEY_XY[2][0]<STEP_3_ALARM2MODE)
				{
					MeterSetData.c_AlarmP[KEY_XY[2][0]-STEP_3_ALARMVALUEH]=KEY_XY[3][0];
				}
				else if(KEY_XY[2][0]<STEP_3_ALARMVALUEL)
				{
					MeterSetData.c_AlarmEnable[0] &= ~(0x0015<<BITALARM_PL); //PQS同时控制
					if(KEY_XY[3][0]) MeterSetData.c_AlarmEnable[0] |= (0x0015<<BITALARM_PL);
				}
				else 
				{
					MeterSetData.c_AlarmP[KEY_XY[2][0]-STEP_3_ALARMVALUEHYSH]=KEY_XY[3][0];
				}		
			}
			else if(STEP_2_ALARMPF==KEY_XY[1][0]) //进入功率因数报警界面
			{
				if(STEP_3_ALARMMODE==KEY_XY[2][0])
				{
					MeterSetData.c_AlarmEnable[0] &= ~(1<<BITALARM_PFL);
					MeterSetData.c_AlarmEnable[0] |= (KEY_XY[3][0]<<BITALARM_PFL);
				}	
				else 
				{
					MeterSetData.c_AlarmPF[KEY_XY[2][0]-STEP_31_ALARMVALUEH]=KEY_XY[3][0];
				}		
			}
			else if(STEP_2_ALARMTHDU==KEY_XY[1][0]) //进入电压总谐波畸变率
			{
				if(STEP_3_ALARMMODE==KEY_XY[2][0])
				{
					MeterSetData.c_AlarmEnable[0] &= ~(1<<BITALARM_THDUH);
					MeterSetData.c_AlarmEnable[0] |= (KEY_XY[3][0]<<BITALARM_THDUH);
				}	
				else 
				{
					MeterSetData.c_AlarmTHDU[KEY_XY[2][0]-STEP_31_ALARMVALUEH]=KEY_XY[3][0];
				}		
			}
			else if(STEP_2_ALARMTHDI==KEY_XY[1][0]) //进入电流总谐波畸变率
			{
				if(STEP_3_ALARMMODE==KEY_XY[2][0])
				{
					MeterSetData.c_AlarmEnable[1] &= ~(1<<(BITALARM_THDIH-BITALARM_THDIH));
					MeterSetData.c_AlarmEnable[1] |= (KEY_XY[3][0]<<(BITALARM_THDIH-BITALARM_THDIH));
				}	
				else 
				{
					MeterSetData.c_AlarmTHDI[KEY_XY[2][0]-STEP_31_ALARMVALUEH]=KEY_XY[3][0];
				}		
			}
			else if(STEP_2_ALARMUUN==KEY_XY[1][0]) //进入电压不平衡度高报警
			{
				if(STEP_3_ALARMMODE==KEY_XY[2][0])
				{
					MeterSetData.c_AlarmEnable[1] &= ~(1<<(BITALARM_UUNH-BITALARM_THDIH));
					MeterSetData.c_AlarmEnable[1] |= (KEY_XY[3][0]<<(BITALARM_UUNH-BITALARM_THDIH));
				}	
				else 
				{
					MeterSetData.c_AlarmUun[KEY_XY[2][0]-STEP_31_ALARMVALUEH]=KEY_XY[3][0];
				}		
			}
			else if(STEP_2_ALARMIUN==KEY_XY[1][0]) //进入电流不平衡度高报警
			{
				if(STEP_3_ALARMMODE==KEY_XY[2][0])
				{
					MeterSetData.c_AlarmEnable[1] &= ~(1<<(BITALARM_IUNH-BITALARM_THDIH));
					MeterSetData.c_AlarmEnable[1] |= (KEY_XY[3][0]<<(BITALARM_IUNH-BITALARM_THDIH));
				}	
				else 
				{
					MeterSetData.c_AlarmIun[KEY_XY[2][0]-STEP_31_ALARMVALUEH]=KEY_XY[3][0];
				}		
			}
		}	
		else if(STEP_1_TOU==KEY_XY[0][0]) //复费率相关数据
		{
			if((STEP_2_FSD1==KEY_XY[1][0])||(STEP_2_FSD2==KEY_XY[1][0])) //费率1 费率2
			{
				ProgState=STEP_5;
				if(KEY_XY[3][0]<2) KEY_XY[4][0] = sd_Data.c_sd[KEY_XY[1][0]-STEP_2_FSD1][KEY_XY[2][0]][KEY_XY[3][0]];	
				else KEY_XY[4][0] = sd_Data.c_type[KEY_XY[1][0]-STEP_2_FSD1][KEY_XY[2][0]];
				if(KEY_XY[3][0]<1) KEY_XY[4][1]=23;
				else if(KEY_XY[3][0]<2) KEY_XY[4][1]=59;
				else KEY_XY[4][1]=3;
			}	
			else if(STEP_2_MONS==KEY_XY[1][0]) //月费率选择
			{
				ProgState=STEP_3;
				sd_Data.c_mons[KEY_XY[2][0]]=KEY_XY[3][0];
			}
			else if(STEP_2_COPY==KEY_XY[1][0]) //抄表日
			{
				ProgState=STEP_3;
				if(0==KEY_XY[2][0])
				{
					if(KEY_XY[3][0]>0) sd_Data.c_copyday[KEY_XY[2][0]]=KEY_XY[3][0];	
				}	
				else sd_Data.c_copyday[KEY_XY[2][0]]=KEY_XY[3][0];
			}	
			else if(STEP_2_TIME==KEY_XY[1][0]) //时间
			{
				ProgState=STEP_3;
				KeyTime[KEY_XY[2][0]]=KEY_XY[3][0];
				if(0==KeyTime[1]) KeyTime[1]=1;
				if(0==KeyTime[2]) KeyTime[2]=1;
				TaskWriteCurTime(&KeyTime[0]); //需立即执行
    			TASK_START[0] |= Tab_32bit[0][BIT_READRTC];
			}		
		}	
	}
	else //选择项目
	{
		if(STEP_1_OUTPUT==KEY_XY[0][0]) Change99((u16 *)KEY_XY[3], (u16)KEY_XY[3][1]);	
		else if((STEP_1_DEMAND==KEY_XY[0][0])||(STEP_1_AOOUT==KEY_XY[0][0])||(STEP_1_ALARM==KEY_XY[0][0]))
		{
			Change999999(&KEY_XY[3][0],&KeyMult, KEY_XY[3][1]); 
		}
		else if(STEP_1_TOU==KEY_XY[0][0]) //复费率设置
		{
			if(KEY_XY[1][0]>STEP_2_MONS) Change999999(&KEY_XY[3][0],&KeyMult, KEY_XY[3][1]); 
			else Change99((u16 *)KEY_XY[3], (u16)KEY_XY[3][1]);		
			if(STEP_2_TIME==KEY_XY[1][0])			
			{
				if(1==KEY_XY[2][0])
				{
					if((0==KEY_XY[3][0])&&(KEY_LEFT==KeyValue)) KEY_XY[3][0] = 1;	
					if((0==KEY_XY[3][0])&&(KEY_RIGHT==KeyValue)) KEY_XY[3][0] = 12;	
				}	
				if(2==KEY_XY[2][0])
				{
					if((0==KEY_XY[3][0])&&(KEY_LEFT==KeyValue)) KEY_XY[3][0] = 1;	
					if((0==KEY_XY[3][0])&&(KEY_RIGHT==KeyValue)) KEY_XY[3][0] = 31;	
				}		
			}
			else if(STEP_2_COPY==KEY_XY[1][0])
			{
				if(0==KEY_XY[2][0])
				{
					if((0==KEY_XY[3][0])&&(KEY_LEFT==KeyValue)) KEY_XY[3][0] = 1;	
					if((0==KEY_XY[3][0])&&(KEY_RIGHT==KeyValue)) KEY_XY[3][0] = 28;		
				}	
			}		
		}			
	}	
}

/********************************************************
Name: void KeyProgramStep5(void)
Function: 系统设置第五级菜单
Input:  None
Output: None
********************************************************/
void KeyProgramStep5(void)
{
	if(KEY_MENU==KeyValue)	//取消键
	{
		ProgState = STEP_4;
	}
	else if(KEY_ENTER==KeyValue)  //确认键
	{
		ProgState = STEP_4; //返回上一级菜单
		if(STEP_1_OUTPUT==KEY_XY[0][0]) 
		{
			MeterSetData.c_DO[KEY_XY[1][0]][KEY_XY[3][0]+1]=KEY_XY[4][0];
			if(STEP_3_DOALR==MeterSetData.c_DO[KEY_XY[1][0]][0])
			{	
				if(MeterSetData.c_DO[KEY_XY[1][0]][2]>STEP_5_ITHDL) KEY_XY[3][1]=STEP_4_ITEM;	//开关量联动
				else KEY_XY[3][1]=STEP_4_DELAY;	//报警设置	
			}
		}
		else if(STEP_1_TOU==KEY_XY[0][0]) //复费率设置,只有复费率时段和费率类型
		{
			if(KEY_XY[3][0]<2) sd_Data.c_sd[KEY_XY[1][0]-STEP_2_FSD1][KEY_XY[2][0]][KEY_XY[3][0]] = KEY_XY[4][0];
			else sd_Data.c_type[KEY_XY[1][0]-STEP_2_FSD1][KEY_XY[2][0]]=KEY_XY[4][0];
			//排除第一个点数据非零情况
			if((KEY_XY[3][0]<2)&&(0==KEY_XY[2][0])) sd_Data.c_sd[KEY_XY[1][0]-STEP_2_FSD1][KEY_XY[2][0]][KEY_XY[3][0]]=0;
				
		}	
	}
	else  //左右修改操作
	{
		Change999999(&KEY_XY[4][0],&KeyMult, KEY_XY[4][1]); 
	}			
}
//返回到进入前到界面
void ReturnMenu(void)
{
	KeyMainmenu=1;	
}

/********************************************************
Name: void KeyProgramSave(void)
Function: 设置保存操作
Input:  None
Output: None
********************************************************/
void KeyProgramSave(void)
{
	if(KEY_ENTER == KeyValue)
	{
		if(0 == KEY_XY[2][0]) SaveData(); //保存
		else RestoreData(); //恢复数据
		ReturnMenu();
		CalPtCt(); //计算变比		
	}
	else if(KEY_MENU == KeyValue) ProgState = STEP_1;
	else Change99((u16 *)&KEY_XY[2][0], KEY_XY[2][1]);		
}
/*********************************************************************
Name: void SaveData(void)
Function: 数据存储
**********************************************************************/
void SaveData(void)
{
	u8 *ptr_tmp;

	//系统设置数据
	ptr_tmp = (u8 *)&MeterSetData.c_password;
	SPI3Puts(0x0070,44,ptr_tmp);
	SPI3Puts(0x0470,44,ptr_tmp);
	
	ptr_tmp = (u8 *)&MeterSetData.c_DO[0];
	SPI3Puts(0x00a0,56,ptr_tmp);
	SPI3Puts(0x04a0,56,ptr_tmp);	
	
	ptr_tmp = (u8 *)&MeterSetData.c_AO[0][0];
	SPI3Puts(0x00dc,16,ptr_tmp);
	SPI3Puts(0x04dc,16,ptr_tmp);
	
	//复费率时段设置
	ptr_tmp = (u8 *)&sd_Data.c_sd[0][0][0];
	SPI3Puts(0x02ce, 0x54, ptr_tmp);	
	SPI3Puts(0x06ce, 0x54, ptr_tmp);	 
	//抄表日记录，主要是减少存储时间
	ptr_tmp =  (u8 *)&(sd_Data.c_copyday[0]);
	SPI3Puts(0x0324, 0x04, ptr_tmp);	
	SPI3Puts(0x0724, 0x04, ptr_tmp);
	InitDLT645Data();	//初始化DLT645中系统设置数据
	
	//需量设置
	ptr_tmp = (u8 *)&MeterSetData.c_demand[0][0];
	SPI3Puts(0x032a,0x0a, ptr_tmp);	
	SPI3Puts(0x072a,0x0a, ptr_tmp);
	
	//越限报警值
	ptr_tmp = (u8 *)&MeterSetData.c_AlarmUn[0];		
	SPI3Puts(0x03aa,84, ptr_tmp);	
	SPI3Puts(0x07aa,84, ptr_tmp);
	//
	if(1==KeyClr[0]) ClrEn();
	if(1==KeyClr[1]) ClrDemand();
	if(1==KeyClr[2]) ClrMax();
	
	//更改脉冲常数
	ModifyPulseConst();
	Cal_AO(0);
    Cal_AO(1); 
  Uart3Init();   
	DoFreeJudge();		
}



/*********************************************************************
Name: void RestoreData(void)
Function: 恢复数据
**********************************************************************/
void RestoreData(void)
{
	u8 *ptr_tmp;

	//系统设置数据
	ptr_tmp = (u8 *)&MeterSetData.c_password;
	if(SPI3Gets(0x0070,44,ptr_tmp)) ;
	else if(SPI3Gets(0x0470,44,ptr_tmp)) {};
	
	ptr_tmp = (u8 *)&MeterSetData.c_DO[0];
	if(SPI3Gets(0x00a0,56,ptr_tmp));
	else if(SPI3Gets(0x04a0,56,ptr_tmp)) {};	
	
	ptr_tmp = (u8 *)&MeterSetData.c_AO[0][0];
	if(SPI3Gets(0x00dc,16,ptr_tmp));
	else if(SPI3Gets(0x04dc,16,ptr_tmp)) {};
	
	//复费率时段设置
	ptr_tmp = (u8 *)&sd_Data.c_sd[0][0][0];
	if(SPI3Gets(0x02ce, 0x54, ptr_tmp));	
	else if(SPI3Gets(0x06ce, 0x54, ptr_tmp)) {};	 
	//抄表日记录，主要是减少存储时间
	ptr_tmp =  (u8 *)&(sd_Data.c_copyday[0]);
	if(SPI3Gets(0x0324, 0x04, ptr_tmp));	
	else if(SPI3Gets(0x0724, 0x04, ptr_tmp)) {};
	
	//需量设置
	ptr_tmp = (u8 *)&MeterSetData.c_demand[0][0];
	if(SPI3Gets(0x032a,0x0a, ptr_tmp));	
	else if(SPI3Gets(0x072a,0x0a, ptr_tmp)) {};	
}

//清除电能
void ClrEn(void)
{
	u8 i,j, *ptr_tmp;
	//电能清零部分
	for(i=0; i<4; i++)
    {
    	MeterData.EN[i] = 0x00;
    	MeterData.BakEN[i]=0;
    	MeterData.ENF[i]=0;
    	MeterData.VarhFour[i] = 0x00;
    	tem_Wh_P[i] = 0x00;
	    tem_Wh_N[i] = 0x00;
	    tem_Varh_P[i] = 0x00;
	    tem_Varh_N[i] = 0x00;
    	tem_VarhFour[i]=0;
    }
    tem_VAh = 0;
    MeterData.EN[4] = 0x00;
    for(i=0; i<3; i++)
    {
    	MeterData.EPP[i]=0;	
    	MeterData.EPN[i]=0;	
    	MeterData.EQP[i]=0;	
    	MeterData.EQN[i]=0;	
    }
    
    //月复费率电能
    for(j=0; j<13; j++)
    {
    	for(i=0; i<5; i++)
    	{
    		MonthEnergy[0].l_EPPTou[j][i]=0;
    		MonthEnergy[1].l_EPPTou[j][i]=0;
    	} 
    }
    //双向电能
    ptr_tmp = (u8 *)(&MeterData.EN[0]); 
   	SPI3Puts(0x002c,20,ptr_tmp);
	SPI3Puts(0x042c,20,ptr_tmp); 
	//复费率电能
	ptr_tmp = (u8 *)&MonthEnergy[0].l_EPPTou[0][0];
	SPI3Puts(0x0100,20,ptr_tmp);
	SPI3Puts(0x0500,20,ptr_tmp);	
	SaveEnergyMonth();
	
	//四象限无功电能
	ptr_tmp = (u8 *)(&MeterData.VarhFour[0]); 
	SPI3Puts(0x0042,16,ptr_tmp);
	SPI3Puts(0x0442,16,ptr_tmp);
	//备用电能			
	ptr_tmp = (u8 *)(&MeterData.BakEN[0]);
	SPI3Puts(0x0054,16,ptr_tmp);
	SPI3Puts(0x0454,16,ptr_tmp);
	
	//基波双向电能
	ptr_tmp = (u8 *)(&MeterData.ENF[0]); 
	SPI3Puts(0x020a,16,ptr_tmp);
	SPI3Puts(0x060a,16,ptr_tmp);
	
	//分相有功电能
	ptr_tmp = (u8 *)&MeterData.EPP[0];
	SPI3Puts(0x0296,12,ptr_tmp);
	SPI3Puts(0x0696,12,ptr_tmp);	
	
	ptr_tmp = (u8 *)&MeterData.EPN[0];
	SPI3Puts(0x02a4,12,ptr_tmp);
	SPI3Puts(0x06a4,12,ptr_tmp);	
	
	ptr_tmp = (u8 *)&MeterData.EQP[0];
	SPI3Puts(0x02B2,12,ptr_tmp);
	SPI3Puts(0x06B2,12,ptr_tmp);
	
	ptr_tmp = (u8 *)&MeterData.EQN[0];
	SPI3Puts(0x02c0,12,ptr_tmp);
	SPI3Puts(0x06c0,12,ptr_tmp);
	
	//累计时间清除
	ClrTime();	
}


void ClrDemand(void)
{
	u8 i,j, *ptr_tmp;
	//需量清除
	for(j=0; j<6; j++)
	{
		for(i=0; i<8; i++) DemandData[j][i]=0;
		DemandPieceNum[j]=0;
		DemandTimeNum[j]=0;
		DemandResult[j]=0;
	}
	for(j=0; j<1; j++) //保存需量最大值
	{
		ptr_tmp = (u8 *)&DemandData[j+2][0];
		SPI3Puts(MAXDEMANDADDR[0][j],12,ptr_tmp);
		SPI3Puts(MAXDEMANDADDR[1][j],12,ptr_tmp);	
	}
		
}

//清除极值
void ClrMax(void)
{
	u8 i, *ptr_tmp;
	s16 *p16;
	p16 = (s16 *)&MeterMax.UnMax[0];  
	for(i=0; i<24; i++) {*p16++ = 0;}
	ptr_tmp = (u8 *)&MeterMax.UnMax[0];			
	SPI3Puts(MAXVALUEADDR[0][0],48,ptr_tmp);
	SPI3Puts(MAXVALUEADDR[1][0],48,ptr_tmp);
	
	p16 = (s16 *)&MeterMax.UnMin[0]; 
	for(i=0; i<24; i++) {*p16++ = 9999;}
	ptr_tmp = (u8 *)&MeterMax.UnMin[0];	
	SPI3Puts(MAXVALUEADDR[0][1],48,ptr_tmp);
	SPI3Puts(MAXVALUEADDR[1][1],48,ptr_tmp);	
}




void ClrTime(void)
{
	u8 i, *ptr_tmp;
	for(i=0; i<5; i++) MeterTimeRecord[i] = 0;
	ptr_tmp = (u8 *)&MeterTimeRecord[0];
	SPI3Puts(0x0344,20, ptr_tmp);	
	SPI3Puts(0x0744,20, ptr_tmp);	
}
/***********************************************************************
Name: void KeyDealCalibrate(void)
Function: 校准设置
Input:  None
Output: None
***********************************************************************/
void KeyDealCalibrate(void)
{
	switch(CalState)
	{
		case STEP_1:
			KeyCalStep1(); //校准第一级菜单
			break;
		case STEP_2:
			KeyCalStep2(); //校准第二级菜单
			break;
		case STEP_3:
			KeyCalStep3();	//校准第三级菜单
			break;
		case STEP_SAVE:
			KeyCalSave(); //校准保存菜单
			break;
		default:
			break;
	}
}

/*********************************************************************
Name: void KeyCalStep1(void)
Function: 校准第一级菜单
Input:  None
Output: None
*********************************************************************/
void KeyCalStep1(void)
{
	if(KEY_MENU == KeyValue)
	{
		CalState = STEP_SAVE;
		KEY_XY[2][0] = 0;
		KEY_XY[2][1] = 1;
	}
	else if(KEY_ENTER == KeyValue) KeyIntoCalStep2();
	else
	{
		Change99((u16 *)KEY_XY[0], (u16)KEY_XY[0][1]);
	}	
}

/*************************************************
Name: void KeyIntoCalStep2(void)
Function: 
*************************************************/
void KeyIntoCalStep2(void)
{
	CalState = STEP_2;
	KEY_XY[1][0] = 0;
	switch(KEY_XY[0][0])
	{
		case CAL_1_CAL: 
			KEY_XY[1][1] = STEP_2_END;		
			break;
		case CAL_1_AO1:
		case CAL_1_AO2:
			KEY_XY[1][1]=1;
			break;	
		case CAL_1_AOS:
			KEY_XY[1][1]=0;
			break;
		case CAL_1_MENABLE:
			KEY_XY[1][1]=THDSET;
			break;
		case CAL_1_LIMMIT:
			KEY_XY[1][1]=1;
			break;		
		default:
			break;
		
	}	
	
}

/*********************************************************************
Name: void KeyCalStep2(void)
Function:校准第二级菜单 
*********************************************************************/
void KeyCalStep2(void)
{
	if(CAL_1_CAL == KEY_XY[0][0])
	{
		if(KEY_MENU == KeyValue)	//
    	{
    		CalState = STEP_1;                         // 退出到校准第一级采单
    	}
    	else if(KEY_ENTER == KeyValue)
    	{
			if(STEP_2_CLR == KEY_XY[1][0])
			{
				TASK_START[3] |= (Tab_32bit[0][KEY_XY[1][0]]);	
				KEY_XY[1][0]++;	 	
			}	
			else if(STEP_2_UI == KEY_XY[1][0])
			{
				
				//判断电压电流是否在范围内
				if((RMS_Un[0]<30000)||(RMS_Un[1]<30000)||(RMS_Un[2]<30000)) return;
				if((RMS_Un[0]>46000)||(RMS_Un[1]>46000)||(RMS_Un[2]>46000)) return;
				if((RMS_I[0]<40000)||(RMS_I[1]<40000)||(RMS_I[2]<40000)) return;
				if((RMS_I[0]>70000)||(RMS_I[1]>70000)||(RMS_I[2]>70000)) return;	
				TASK_START[3] |= (Tab_32bit[0][KEY_XY[1][0]]);	
				KEY_XY[1][0]++;	 
			}	
			else if(STEP_2_P == KEY_XY[1][0])
			{
				//判断信号是否异常
				if((RMS_P[0] < 16000)||(RMS_P[1] < 16000)||(RMS_P[2] < 16000)) return;	
				if((RMS_P[0] > 22000)||(RMS_P[1] > 22000)||(RMS_P[2] > 22000)) return;		
				TASK_START[3] |= (Tab_32bit[0][KEY_XY[1][0]]);	
				KEY_XY[1][0]++;	 
			}	
			else if(STEP_2_Q == KEY_XY[1][0])
			{
				//判断信号是否异常
				if((RMS_Q[0] < 16000)||(RMS_Q[1] < 16000)||(RMS_Q[2] < 16000)) return;		
				if((RMS_Q[0] > 22000)||(RMS_Q[1] > 22000)||(RMS_Q[2] > 22000)) return;
				TASK_START[3] |= (Tab_32bit[0][KEY_XY[1][0]]);	
				KEY_XY[1][0]++;	 
			}	
			else if(STEP_2_PH == KEY_XY[1][0])
			{
				if((RMS_P[0] < 8000)||(RMS_P[1] < 8000)||(RMS_P[2] < 8000)) return;
				if((RMS_P[0] > 11000)||(RMS_P[1] > 11000)||(RMS_P[2] > 11000)) return;	
				TASK_START[3] |= (Tab_32bit[0][KEY_XY[1][0]]);
				//判断相角是否在合理的范围内, 为1.0时一半
				KEY_XY[1][0]++;	 
			}	
			else if(STEP_2_END == KEY_XY[1][0])
			{
				TASK_START[3] |= (Tab_32bit[0][BIT_CALSAVE]);
            	CalState = STEP_1;		
			}			
		}	
	}	
	else 
	{
		if(KEY_MENU == KeyValue)	//
    	{
    		CalState = STEP_1;                         // 退出到校准第一级采单
    	}
    	else if(KEY_ENTER == KeyValue)
    	{
    		CalState = STEP_3;  
			switch(KEY_XY[0][0])
			{
				case CAL_1_AO1: //PWM
					CalState = STEP_3;
					KEY_XY[2][0] = AdjustAo[KEY_XY[0][0] - CAL_1_AO1][KEY_XY[1][0]];
					KEY_XY[2][1] = 0x5fff;
					TIM2->CCR1 = KEY_XY[2][0];	
					KeyMult = 0;
					break;
				case CAL_1_AO2: //PWM
					CalState = STEP_3;
					KEY_XY[2][0] = AdjustAo[KEY_XY[0][0] - CAL_1_AO1][KEY_XY[1][0]];
					KEY_XY[2][1] = 0x5fff;
					TIM2->CCR2 = KEY_XY[2][0];	
					KeyMult = 0;
					break;
				case CAL_1_AOS:
					KEY_XY[2][0] = MeterSetData.c_DASelect ;
					KEY_XY[2][1] = 1;
					break;
				case CAL_1_MENABLE:
					KEY_XY[2][0]=MeterSetData.c_set[KEY_XY[1][0]];
					if(CAL_2_AOEN==KEY_XY[1][0]) KEY_XY[2][1] = 2;
					else if(CAL_2_DIEN==KEY_XY[1][0]) KEY_XY[2][1] = 3;
					else if(CAL_2_THDEN==KEY_XY[1][0]) KEY_XY[2][1] = 5;	
					else KEY_XY[2][1] = 1;
					if(COM2SET==KEY_XY[1][0]) CalState = STEP_2; 
					break;
				case CAL_1_LIMMIT: //门限值
					KEY_XY[2][0]=MeterSetData.UIViewLimit[KEY_XY[1][0]];
					KEY_XY[2][1] = 5000;
					break;
				default:
					CalState = STEP_1;
					break;
			}
		}
    	else
    	{
    		Change99((u16 *)KEY_XY[1], (u16)KEY_XY[1][1]);	
    	}	
			
	}	
}

/*********************************************************************
Name: void KeyCalStep3(void)
Function:校准第三级菜单 
*********************************************************************/
void KeyCalStep3(void)
{
	if((CAL_1_AO1==KEY_XY[0][0])||(CAL_1_AO2==KEY_XY[0][0]))
	{
		if(KEY_ENTER==KeyValue)
		{
			BackCalStep2();
		}
		else
		{
			ChangeFFFF(KEY_XY[2],&KeyMult);
			if(KEY_XY[0][0]==CAL_1_AO1) TIM2->CCR1 = KEY_XY[2][0];	
			else if(KEY_XY[0][0]==CAL_1_AO2) TIM2->CCR2 = KEY_XY[2][0];	
				
		}		
	}
	else
	{
		if(KEY_MENU==KeyValue)
		{
			CalState = STEP_2;	
		}
		else if(KEY_ENTER==KeyValue)
		{
			BackCalStep2();
		}
		else
		{
			Change999999(&KEY_XY[2][0],&KeyMult, KEY_XY[2][1]); 	
		}		
	}	
			
	
}

//返回到第二级菜单
void BackCalStep2(void)
{
	CalState = STEP_2;	
	switch(KEY_XY[0][0])
	{
		case CAL_1_AO1: //PWM
		case CAL_1_AO2: //PWM
			AdjustAo[KEY_XY[0][0]-CAL_1_AO1][KEY_XY[1][0]] = KEY_XY[2][0];	
			break;
		case CAL_1_AOS:
			MeterSetData.c_DASelect =KEY_XY[2][0];
			break;
		case CAL_1_MENABLE:
			MeterSetData.c_set[KEY_XY[1][0]-CAL_2_DIEN] = KEY_XY[2][0];
			if(MeterSetData.c_set[THDSET]<3) MeterSetData.c_set[THDSET]=3; //防止小数据出现
			break;
		case CAL_1_LIMMIT:
			MeterSetData.UIViewLimit[KEY_XY[1][0]]= KEY_XY[2][0];
			break;
		default:
			CalState = STEP_1;
			break;
	}
	
}

/************************************************************

************************************************************/
void KeyCalSave(void)
{
	if(KEY_ENTER == KeyValue)
	{
		if(0 == KEY_XY[2][0]) //保存
		{
			SaveCalData();
		}
		ReturnMenu();	
		DAAjustMark = 0; //退出校准菜单	
		ProgState = 0;
	}
	else if(KEY_MENU == KeyValue)
	{
		CalState = STEP_1;  
	}
	else
	{
		Change99((u16 *)&KEY_XY[2][0], KEY_XY[2][1]);	
	}		
}

//保存校准参数
void SaveCalData(void)
{
	u8 *ptr_tmp;
	ptr_tmp = (u8 *)&(MeterSetData.c_password);  
	SPI3Puts(0x0070,44,ptr_tmp);
	SPI3Puts(0x0470,44,ptr_tmp);
	
	ptr_tmp = (u8 *)&MeterSetData.UIViewLimit[0];
	SPI3Puts(0x006a,4,ptr_tmp);
	SPI3Puts(0x046a,4,ptr_tmp);	
	
	ptr_tmp = (u8 *)&AdjustAo[0][0];
	SPI3Puts(0x00ee,8,ptr_tmp);
	SPI3Puts(0x04ee,8,ptr_tmp);	
	
	if(MeterSetData.c_set[AOSET]) Time2Init(); 
	Cal_AO(0);
	Cal_AO(1); 
}

/**********************************************
Name: void Change99(u16 *p,u16 max)
Function: 编程菜单切换
Input:  *p:修改参数，max：最大值
Output: None
***********************************************/
void Change99(u16 *p,u16 max)
{
  if(KEY_LEFT==KeyValue)
  {
  	if(*p==0) *p = max;
    else (*p)--;
  }
  else if(KEY_RIGHT == KeyValue)
  {
    if(*p>=max) *p = 0;
    else (*p)++;
  }
}

/****************************************
Name: void Change999999(u32 *p32,u32 *p8,u32 max)
Function: 数据修改，修改位数包括2/3/4/6
****************************************/
void Change999999(u32 *p32,u8 *p8,u32 max)
{
  u8 i;
  u8 bit[6] = {0,0,0,0,0,0};
  u8 maxbit;
  u32 temp32 = *p32;
  if(max<1000) maxbit = 2;
  else if(max<10000) maxbit = 3;
  else if(max<100000) maxbit = 4;
  else maxbit = 5;
  if(max<101)
  {
    if(KEY_LEFT == KeyValue)
    {
      if(*p32==max) *p32 = 0;
      else (*p32)++;
    }
    else if(KEY_RIGHT == KeyValue)
    {
      if(*p32==0) *p32 = max;
      else (*p32)--;
    }
    if(*p32>max) *p32 = 0;
  }
  else if(max<1000000)
  {//0-999999
    for(i=0;i<(maxbit+1);i++)
    {
      bit[i] = (uint8_t)(temp32%10);
      temp32 = (u32)(temp32/10);
    }
    if(KEY_LEFT == KeyValue)
    {
      if(*p8<maxbit) (*p8)++;
      else *p8 = 0; 
    }
    if(KEY_RIGHT == KeyValue)
    {
      if(bit[*p8]<9) bit[*p8]++;
      else bit[*p8] = 0;
    }
    temp32 = (u32)(bit[0] + bit[1]*10 + bit[2]*100 + bit[3]*1000 + bit[4]*10000 + bit[5]*100000);
    if(temp32>max) *p32 = 0;
    else *p32 = temp32;
  }else *p32 = 0;
}

/****************************************
Name: void ChangeFFFF(u32 *p16,u16 *p8,)
Function: 数据修改，
****************************************/
void ChangeFFFF(u32 *p32,u8 *p8)
{
 	u8 i;
  	u16 bit[4] = {0,0,0,0};
  	u8 maxbit;
  	u32 temp32 = *p32;
	maxbit = 3;
	for(i=0;i<(maxbit+1);i++)
    {
      bit[i] = (u8)((temp32>>(i<<2))&0x0f);
    }
    if(KEY_LEFT == KeyValue)
    {
      if(*p8<maxbit) (*p8)++;
      else *p8 = 0; 
    }
    if(KEY_RIGHT == KeyValue)
    {
      if(bit[*p8]<15) bit[*p8]++;
      else bit[*p8] = 0;
    }
    if(KEY_MENU==KeyValue)
    {
    	if(bit[*p8]>0) bit[*p8]--;
      	else bit[*p8] = 15;
    }	
    *p32 = (u16)(bit[0] + (bit[1]<<4) + (bit[2]<<8) + (bit[3]<<12));

}

