#include "User_Config.h"
void LCDDisplayRev(void);
#define UMLIMIT	100  //10.00V 屏蔽值
#define IMLIMIT	5	 //5.0mA 屏蔽值
void Return_Measure(void);
void LCDDisplayCOM(void);
u8 const WEEKDAYC[7][4]; 
u8 const WEEKDAYE[7][10];
u8 const LCD2AOTITLE2C[4][3];

u8 const REV[1][23]={"Version: 1013"};

const u8 MainTitleC[1][4]=
{
	{44,45,46,0},  //主菜单
};
const u8 MainTitleE[1][7]=
{
	" MENU ",	 
};

//
const u8 MainMenuC[8][5]=
{
	{ 6, 7, 8, 9, 0}, //实时测量
	{10,11,43, 9, 0}, //电能计量
	{10,11,20, 9, 0}, //电能质量
	{6,7,17,22, 0}, //实时波形
	{25, 9,12,13, 0}, //需量记录
	{26,27,12,13, 0}, //极值记录
	{28,29,52,11, 0}, //扩展功能
	{38,39,40,41, 0}, //系统设置	
};

const u8 MainMenuE[8][14] =
{
	"Measuring",
	"Energy",
	"Power Quality",
	"Waveform",
	"Demand",
	"Max/Min",
	"Other",
	"Setup",
};

//电量界面中文标题
const u8 ELTITLEC[9][5] =
{
	{23,10,48, 0},	//相电压 0
	{49,10,48, 0},//线电压 1
	{10,50, 0},//电流  2
	{51,52,52,15, 0},//有功功率 3
	{53,52,52,15, 0},//无功功率 4
	{54,104,52,15, 0},//视在功率 5
	{52,15,55,56, 0},//功率因数 6
    {10,48,10,50,0}, //电压电流  7
    {52,15, 0},     //功率      8 
};

//电量界面英文标题
const u8 ELTITLEE[9][16]=
{
	"L-N Voltage",
	"L-L Voltage",
	"Current",
	"Active Power",
	"Reactive Power",
	"Apparent Power",
	"Power Factor",
	"Voltage Current",
	"Power"	,
} ;
												 
//电量指示符
const u8 ELUNTITLE[4][3] =
{
	"V1","V2","V3","F ",	
};
const u8 ELULTITLE[4][4] =
{
	"U12","U23","U31","F ",	
};
const u8 ELUITITLE[3][2] =
{
	"U","I","F",	
};
const u8 ELITITLE[4][3] =
{
	"I1", "I2",	"I3","In"
};
const u8 ELPTITLE[4][3]=
{
	"P1","P2","P3",	"P",	
};
const u8 ELQTITLE[4][3]=
{
	"Q1","Q2","Q3","Q",
};
const u8 ELSTITLE[4][3]=
{
	"S1","S2","S3","S",	
};
const u8 ELPFTITLE[4][4]=
{
	"PF1","PF2","PF3","PF",
};
const u8 ELPQSTITLE[4][3]=
{
	"P", "Q", "S","PF",
};
const u8 ELPQSUNIT[4][4] =
{
	"W", "var", "VA", "  "	
};

//电量显示界面正负号
const u8 POSTNEG[2][2]=
{
	{42,0}, //空个
	{57,0},	//-
};
const u8 ELUNITKMG[4][2]=
{
	" ",
	"k",
	"M",
	"G",	
};


/****************************************************************
Name: void TaskLCD(void)
Function: 液晶界面显示
Input:  None
Output: None
*****************************************************************/
u16 timetest[2];
void TaskLCD(void)
{
	if(TASK_START[0]&Tab_32bit[0][BIT_DISPLAYLCD]) TASK_START[0] &= Tab_32bit[1][BIT_DISPLAYLCD];
	
	//关闭背光
	if((0!=MeterSetData.c_lcdlight)&&(Time2Light>=MeterSetData.c_lcdlight))
	{
		Time2Light = 0;
		GPIO_ResetBits(GPIOC, GPIO_BACKLIGHT);
		LCDDark=1;
	}
	//菜单界面
	if(1 == KeyMainmenu)
	{
		LCDDisplayMenu(); //主菜单界面显示	
		LCDDisplayCOM();
	}
	else //在具体的测量界面
	{
		switch(KeyPage)
		{
			case PAGE_EL: //电量显示
				LCDDisplayCOM();
				LCDDisplayEL();
				break;
			case PAGE_ELBAR: //棒图显示
				LCDDisplayELBar();
				break;
			case PAGE_EN: //电能显示
				LCDDisplayCOM();
				LCDDisplayEN(); 
				break;
			case PAGE_QUALITY: //电能质量
				
				LCDDisplayQuality();
				//LCDDisplayCOM();
				break;
			case PAGE_WAVE: //电能质量
				
				LCDDisplayWave();
				//LCDDisplayCOM();
				break;
			case PAGE_DEMAND:	//需量显示
				//LCDDisplayCOM();
				LCDDisplayDemand();
				break;
			case PAGE_MAX:	//极值显示
				//LCDDisplayCOM();
				LCDDisplayMax();	
				break;
			case PAGE_MODULE: //扩展模块
				//LCDDisplayCOM();
				LCDDisplayModule();
				break;
			case PAGE_PR: //编程界面
				LCDDisplayProgram(); 
				if(KeyNoEnterTime > 240)	//在主菜单，如果时间大于4分钟则会返回
			    {
			    	Return_Measure();
			    	KeyNoEnterTime = 0;
			    	TASK_START[0] |= Tab_32bit[0][BIT_KEY]; //启动按键处理程序	
			    }
			 	break;
			default:
				break;	
		}	
	}	
			
	//更新显示
	DisplayUpdate();	
			
}


//在不按键情况下，返回测量界面
void Return_Measure(void)
{
    KeyMainmenu = 0;
    if(MeterSetData.c_input[0] == STEP_3_N11)
    {
    	DisplayItem = DISPLAY_11UN;	
    }	
    else if(MeterSetData.c_input[0] == STEP_3_N33)
    {
    	DisplayItem = DISPLAY_33UL;
    }
    else
    {
    	DisplayItem = DISPLAY_34UN;	
    }		
    
  	KeyPage = PAGE_EL;
  	LCDDisplayEL();
}

u8 const COM_STATE[2][2]=
{
	{224,0},
	{42,0},	
};
uint16_t comnum;
void LCDDisplayCOM(void)
{
	s16 midint16;
	
		//判断通信状态是否需要显示
    if(Com1Enable == 0x01)
    {
    	comnum++;
    	midint16 =  Time10ms - Com1Time;
    	if(midint16 < 0) midint16 += 0x7000;	
    	if(midint16 > 400)	//4S关闭闪烁功能
    	{
    		Com1Enable = 0x00;	//清除通信标志位
    		comnum=0;
    	}	
    	if(comnum%2) //显示通信符号
    	{
    		Printf1616(220, 2, COM_STATE[0], 0);
    	}	
    	else //不显示通信符合
    	{
    		Printf1616(220, 2, COM_STATE[1], 0);
    	}	
    	
    }
}


/********************************************
Name: void TopShadow(void)
Function: 顶部阴影显示
Input:  None
Output: None
********************************************/
void TopShadow(void)
{
	u8 i;
	for(i=0; i<240; i++) //顶部阴影
	{
		DISP_RAM[0][i]=0xff;
		DISP_RAM[1][i]=0xff;	
	}	
}

/************************************************************
Name: void LCDDisplayMenu(void)

*************************************************************/
void LCDDisplayMenu(void)
{
	u8 i, ox, oy, shadow;
	//中英文字符共用部分
	//顶部阴影
	TopShadow();

	if(MeterSetData.c_lcdlanguage==1) //英文界面和西班牙文
	{
		Printf816(96, 0, MainTitleE[0],1); //主菜单
		oy = 4;
		for(i=0; i<4; i++)
		{
			if(i==DisplayItem) shadow = 1;  //有阴影显示
			else shadow = 0;	
			
			ox=Printf816(16, oy, MainMenuE[i], shadow);
			Printf816Lost(ox,oy,(112-ox), shadow);	
			oy += 0x02;
		}
		oy = 4; 
		for(i=4; i<8; i++)	//右边一列
		{
			if(i==DisplayItem) shadow = 1;  //有阴影显示
			else shadow = 0;
			ox=Printf816(144, oy, MainMenuE[i],shadow);
			Printf816Lost(ox,oy,(216-ox), shadow);
			oy += 0x02;
		}
		//四个按键
		PrintfKeyF4(K_UP,K_DOWN,K_NO,K_ENTERE);
	}
	else //中文界面
	{
		//主菜单
		Printf1616(96, 0, MainTitleC[0],1);
		oy = 4;
		for(i=0; i<4; i++)  //左边一列
		{
			if(i==DisplayItem) shadow = 1;  //有阴影显示
			else shadow = 0;	
			Printf1616(24, oy, MainMenuC[i], shadow);
			oy += 0x02;
		}
		
		oy = 4; 
		for(i=4; i<8; i++)	//右边一列
		{
			if(i==DisplayItem) shadow = 1;  //有阴影显示
			else shadow = 0;
			Printf1616(144, oy, MainMenuC[i],shadow);
			oy += 0x02;
		}
		//四个按键
		PrintfKeyF4(K_UP,K_DOWN,K_NO,K_ENTER);
	}
}

/******************************************************
Name: void LCDDisplayEL(void)
Function: 电量界面显示
******************************************************/
void LCDDisplayEL(void)
{

	u8 m=0;
	//共用部分显示
	TopShadow(); //顶部阴影显示
	if(1==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_UP,K_DOWN,K_MENUE,K_SWITCHE); 	//四个按键
	else PrintfKeyF4(K_UP,K_DOWN,K_MENU,K_SWITCH); 	//四个按键		
	//根据功率情况显示容性或感性
	if((0==RMS_Un[0])&&(0==RMS_Un[1])&&(0==RMS_Un[0])) m=0;	
	else
	{
		if(RMS_P[3]>0)
		{
			if(RMS_Q[3]>0) m=1;
			else if(RMS_Q[3]<0) m=2;
			else m=0;
		}	
		else
		{
			if(RMS_Q[3]>0) m=2;
			else if(RMS_Q[3]<0) m=1;
			else m=0;	
		}	
	}	
	PrintfBMP820(170,3,m);	
	
	switch(DisplayItem)
	{
		case DISPLAY_34UN:
			LCDDisplayUn();
			break;
		case DISPLAY_34UL:
		case DISPLAY_33UL:
			LCDDisplayUl();
			break;
		case DISPLAY_34I:
		case DISPLAY_33I:
			LCDDisplayI();
			break;
		case DISPLAY_34P:
			LCDDisplayP();
			break;
		case DISPLAY_34Q:	
			LCDDisplayQ();
			break;
		case DISPLAY_34S:	
			LCDDisplayS();
			break;
		case DISPLAY_34PF:
			LCDDisplayPF();
			break;
		case DISPLAY_11UN:
			LCDDisplayUI();
			break;
		case DISPLAY_33PQSALL:
		case DISPLAY_11PQSALL:
			LCDDisplayPQS();
			break;
		default:
			break;
				
	}	

}

/******************************************************
Name: void LCDDisplayELBar(void)
Function: 电量棒图界面显示
******************************************************/
void LCDDisplayELBar(void)
{
	//共用部分显示
	TopShadow(); //顶部阴影显示
	if(0==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_UP,K_DOWN,K_MENU,K_SWITCH); 	//四个按键	
	else PrintfKeyF4(K_UP,K_DOWN,K_MENUE,K_SWITCHE); 	//四个按键
	switch(DisplayItem)
	{
		case DISPLAY_34UN:
			LCDDisplayUnBar();
			break;
		case DISPLAY_34UL:
		case DISPLAY_33UL:
			LCDDisplayUlBar();
			break;
		case DISPLAY_34I:
		case DISPLAY_33I:
			LCDDisplayIBar();
			break;
		case DISPLAY_34P:
			LCDDisplayPBar();
			break;
		case DISPLAY_34Q:
			LCDDisplayQBar();
			break;
		case DISPLAY_34S:
			LCDDisplaySBar();
			break;
		case DISPLAY_34PF:
			LCDDisplayPFBar();
			break;
		case DISPLAY_11UN:
			LCDDisplayUIBar();
			break;
		case DISPLAY_33PQSALL:
		case DISPLAY_11PQSALL:
			LCDDisplayPQSBar();
			break;
		default:
			break;
				
	}	
}

/*********************************************************
Name: void LCDDisplayVoltage(s32 *p)
Function: 三个电压数据显示
Input:  *p 电压数据指针
Output: None
**********************************************************/
void LCDDisplayUData(s32 *p)
{
	s32 tempint1, *midp;                                           // 临时整型变量
    u32 tempint2;                                           // 临时除数
    s32 templong;                                           // 临时长整型变量
    u8  PointPosition, unit; 
    u8 oy, i; 
    //找出最大的值 
    
	midp = p;
	tempint1 = *midp++;
	if ((*midp) > tempint1) 
	{
	    tempint1 = *midp;
	}
	midp++;	
	if ((*midp) > tempint1) 
	{
		tempint1 = *midp;
	}		
    templong = (u32)(((float)tempint1) * PtResult / 100000);       // 乘以变比后再判断
    if (templong < 1) 
    {
        tempint2 = 1;
        PointPosition = 2;                             // XXX.XX 小数点位置1
        unit = 0;
    }
    else if (templong < 9) 
    {
        tempint2 = 10;
        PointPosition = 1;                             // XXXX.X 无小数点
        unit = 0;
    }
    else if (templong < 90) 
    {
        tempint2 = 100;
        PointPosition = 3;                             // XX.XXX 小数点位置2
        unit = 1;                   // "K"
    }
    else if (templong < 900) 
    {
        tempint2 = 1000;
        PointPosition = 2;                             // XXX.XX 小数点位置1
        unit = 1;                   // "K"
    }
    else if(templong<9900)
    {
        tempint2 = 10000;
        PointPosition = 1;                             // XXXX.X 无小数点
        unit = 1;                 // "K"
    }
    else
    {
    	tempint2 = 100000;
      PointPosition = 0;                             // XXXXX 无小数点
      unit = 1;                 // "K"
    }	
    
    oy = 4;
    for(i=0; i<3; i++)
    {
    	tempint1 = (s32)(((float)(*p++)) * PtResult / tempint2);
    	Printf1624Num5(56, oy, tempint1, PointPosition);
    	oy += 3;
    }
    //F
    if(0 == RMS_Un[0]) PrintfBMP24104(40,oy,0);
    else Printf1624Num5(56, oy, RMS_F, 3);  //xx.xxxHz  
    //显示单位 单位V
	oy = 4;
	if(1==unit)
	{
		for(i=0; i<3; i++)
		{
			Printf81624(160, oy, "kV");
			oy += 3;
		}
	}
	else
	{
		for(i=0; i<3; i++)
		{
			Printf81624(160, oy, " V");
			oy += 3;
		}
	}		
	Printf81624(168, oy, "Hz");
	
}

/**********************************************************
Name: void LCDDisplayUn(void)
Fuctiong: 三相四线相电压界面
Input： None
OutPut: None
***********************************************************/
void LCDDisplayUn(void)
{
	u8 i,oy;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[0],1); //
	else Printf1616(1, 0, ELTITLEC[0], 1); //相电压瞬时值
	Printf816(208,0,"1.1",1); //1.1 
	//显示字符 
	oy=4;
	for(i=0; i<4; i++)
	{
		Printf81624(8, oy, (u8 *)ELUNTITLE[i]); //V1 V2 V3 F
		oy += 3;	
	}
	//显示数据和单位
	LCDDisplayUData(&(RMS_Un[0])); 
	
}

/***********************************************************
Name: void LCDDisplayUl(void)
Fuctiong: 三相四线线电压界面
Input： None
OutPut: None
***********************************************************/
void LCDDisplayUl(void)
{
	u8 i,oy;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[1],1); //
	else Printf1616(1, 0, ELTITLEC[1], 1); //线电压瞬时值
	if(INPUT34 == MeterSetData.c_input[0]) Printf816(208,0,"1.2",1); //1.2 
	else if(INPUT33 == MeterSetData.c_input[0]) Printf816(208,0,"1.1",1); //1.1 
	//显示字符 
	oy=4;
	for(i=0; i<4; i++)
	{
		Printf81624(8, oy, ELULTITLE[i]); //Uab Ubc Uca F
		oy += 3;
	}
	//显示数据和单位
	LCDDisplayUData(&(RMS_Ul[0])); 
}

/*********************************************************
Name: void LCDDisplayIData(u32 midRMS_I[3])
Function: 三相电流数据显示
Input: None
Output: None
**********************************************************/
void LCDDisplayIData(void)
{
	s32 tempint1, *midp, *p;                                           // 临时整型变量
    u32 tempint2;                                           // 临时除数
    s32 templong;                                           // 临时长整型变量
    u8  PointPosition, unit; 
    u8 oy, i,n;
    
    if(INPUT34==MeterSetData.c_input[0]) n=4;  
    else n=3;
	//找出最大的值
	midp = &RMS_I[0];
	p = midp;
    tempint1 = *midp++;
    if ((*midp) > tempint1) 
    {
        tempint1 = *midp;
    }
    midp++;
    
    if ((*midp) > tempint1) 
    {
        tempint1 = *midp;
    }
    
    if(INPUT34==MeterSetData.c_input[0])
    {
    	midp++;
    
	    if ((*midp) > tempint1) 
	    {
	        tempint1 = *midp;
	    }	
    }	
    	
    
    templong = (u32)(((float)tempint1) * CtResult / 100000);       // 乘以变比后再判断
    if (templong < 1) 
    {
        tempint2 = 1;
        PointPosition = 4;                             // X.XXXX 小数点位置1
        unit = 0;
    }
    else if (templong < 9) 
    {
        tempint2 = 10;
        PointPosition = 3;                             // XX.XXX 无小数点
        unit = 0;
    }
    else if (templong < 90) 
    {
        tempint2 = 100;
        PointPosition = 2;                             // XXX.XX 小数点位置2
        unit = 0;                   // 
    }
    else if (templong < 900) 
    {
        tempint2 = 1000;
        PointPosition = 1;                             // XXXX.X 小数点位置1
        unit = 0;                   // 
    }
    else if (templong < 9000) 
    {
        tempint2 = 10000;
        PointPosition = 3;                             // XX.XXX 无小数点
        unit = 1;                 // "K"
    }
    else
    {
    	tempint2 = 100000;
        PointPosition = 2;                             // XXX.XX 无小数点
        unit = 1; 
    }	
    
    

    // Ia Ib (Ic)
    oy = 4;
    for(i=0; i<n; i++)
    {
    	tempint1 = (s32)(((float)(*p++)) * CtResult / tempint2);
    	Printf1624Num5(56, oy, tempint1, PointPosition);
    	oy += 3;
	}

    
    //显示单位 A/KA
	oy = 4;
	if(1==unit)
	{
		for(i=0; i<n; i++)
		{
			Printf81624(160, oy, "kA");
			oy += 3;
		}
		
	}
	else
	{
		for(i=0; i<n; i++)
		{
			Printf81624(160, oy, " A");
			oy += 3;
		}
	}		

	
}


/************************************************************
Name: void LCDDisplayI(void)
Function: 三相电流显示
Input:  None
Output: None
************************************************************/
void LCDDisplayI(void)
{
	u8 i, oy, n=3;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[2],1); //
	else Printf1616(1, 0, ELTITLEC[2], 1); //电流瞬时值
	if(INPUT34 == MeterSetData.c_input[0])
	{
		Printf816(208,0,"1.3",1); //1.3 
		n=4;	
	}
	else Printf816(208,0,"1.2",1); //1.2 
	//显示字符 
	oy=4;
	for(i=0; i<n; i++)
	{
		Printf81624(8, oy, ELITITLE[i]); //Ia Ib Ic 或In
		oy += 3;
	}
	//显示数据和单位
	LCDDisplayIData(); 
}


/**********************************************
Name: void LCDDisplayPQSData(void)
Function: 4个功率数据显示
Input:  *p:指向显示数据的头地址
Output: None
**********************************************/
void LCDDisplayPQSData(s32 *p)
{
	s32 tempint1;                                      // 临时整型变量
    s32 templong2;                                     // 临时除数
    s32 templong;                                      // 临时长整型变量
    u8 PointPosition, unit, k;                                 // 小数点位置   
	u8 i, oy;
	
	tempint1 = RMS_S[3];
    templong = (u32)((float)tempint1 * PtCtResult/ 100000);    // 乘以变比后再判断
    if (templong < 1) 
    {
        templong2 = 1;                                    
        PointPosition = 1;   // XXXX.X 无小数点
        unit = 0;            //无单位
    }
    else if (templong < 9) 
    {
        templong2 = 10;
        PointPosition = 3;                             // XX.XXX 小数点位置2
        unit = 1;                   // "K"
    }
    else if (templong < 90) 
    {
        templong2 = 100;
        PointPosition = 2;                             // XXX.XX 小数点位置1
        unit = 1;                   // "K"
    }
    else if (templong < 900) 
    {
        templong2 = 1000;
        PointPosition = 1;                             // XXXX.X 小数点位置0
        unit = 1;                   // "K"
    }
    else if (templong < 9000) 
    {
        templong2 = 10000;
        PointPosition = 3;                             // XX.XXX 小数点位置2
        unit = 2;                   // "M"
    }
    else if (templong < 90000) 
    {
        templong2 = 100000;
        PointPosition = 2;                             // XXX.XX 小数点位置1
        unit = 2;                   // "M"
    }
    else if (templong < 900000) 
    {
        templong2 = 1000000;
        PointPosition = 1;                             // XXXX.X 小数点位置0
        unit = 2;                   // "M"
    }
    else if (templong < 9000000) 
    {
        templong2 = 10000000;
        PointPosition = 3;                             // XX.XXX 小数点位置2
        unit = 3;                   // "G"
    }
    else if (templong < 90000000) 
    {
        templong2 = 100000000;
        PointPosition = 2;                             // XXX.XX 小数点位置1
        unit = 3;                   // "G"
    }
    else
    {
    	templong2 = 1000000000;
        PointPosition = 1;                             // XXXX.X 小数点位置1
        unit = 3;                   // "G" 
    }	
    
    oy = 4;
    for(i=0; i<4; i++)
   	{ 
    	tempint1 = (s32)((float)(abss32(*p)) * PtCtResult / templong2); // P
		Printf1624Num5(56, oy, tempint1, PointPosition);   
		if((*p) < 0)  k=1;   
		else k=0;
		Printf1616(40, oy,POSTNEG[k],0); //符号显示
		oy += 3;
		p++;
	}	
	//单位显示
	oy = 4;
	for(i=0; i<4; i++)
	{
		Printf81624Unit(160, oy, unit);
		oy += 3;
	}	   
}

/**********************************************
Name: void LCDDisplayP(void)
Function: 三相四线有功功率显示
Input:  None
Output: None
***********************************************/
void LCDDisplayP(void)
{
	u8 i, oy;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[3],1); //
	else Printf1616(1, 0, ELTITLEC[3], 1); //有功功率瞬时值
	Printf816(208,0,"1.4",1); //1.4 
	//显示字符 
	oy=4;
	for(i=0; i<4; i++)
	{
		Printf81624(8, oy, ELPTITLE[i]); //Pa Pb Pc P
		Printf81624(172, oy, "W"); //单位“W”
		oy += 3;	
	}
	//显示数据和单位
	LCDDisplayPQSData(RMS_P);
		
}

/**********************************************
Name: void LCDDisplayQ(void)
Function: 三相四线无功功率显示
Input:  None
Output: None
***********************************************/
void LCDDisplayQ(void)
{
	u8 i, oy;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[4],1); //
	else Printf1616(1, 0, ELTITLEC[4], 1); //无功功率瞬时值
	Printf816(208,0,"1.5",1); //1.5 
	//显示字符 
	oy=4;
	for(i=0; i<4; i++)
	{
		Printf81624(8, oy, ELQTITLE[i]); //Qa Qb Qc Q
		Printf81624(172, oy, "var"); //单位“var”
		oy += 3;
	}
	//显示数据和单位
	LCDDisplayPQSData(RMS_Q);	
}

/**********************************************
Name: void LCDDisplayS(void)
Function: 三相四线视在功率显示
Input:  None
Output: None
***********************************************/
void LCDDisplayS(void)
{
	u8 i, oy;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[5],1); //
	else Printf1616(1, 0, ELTITLEC[5], 1); //无功功率瞬时值
	Printf816(208,0,"1.6",1); //1.6 
	//显示字符 
	oy=4;
	for(i=0; i<4; i++)
	{
		Printf81624(8, oy, ELSTITLE[i]); //Sa Sb Sc S
		Printf81624(172, oy, "VA"); //单位“VA”
		oy += 3;	
	}
	//显示数据和单位
	LCDDisplayPQSData(RMS_S);	
}

/*******************************************************
Name: void LCDDisplayPFData(void)
Function: 功率因数数据显示
Input:  None
Output: None
********************************************************/
void LCDDisplayPFData(void)
{
	s32 tempint1;    // 临时整型变量
    u8  k;           //  
    s32 *p;
	p = RMS_PF;
	if(0 == RMS_Un[0])
	{
		PrintfBMP24104(40,4,0);
	}
	else
	{		
	    tempint1 = abss32(*p);
		Printf1624Num5(56, 4, tempint1, 4);   
		if((*p) < 0)  k=1;   
		else k=0;
		Printf1616(40, 4,POSTNEG[k],0); //符号显示
	}
		
	p++;	
	if(0 == RMS_Un[1]) PrintfBMP24104(40,7,0);
	else
	{
		tempint1 = abss32(*p);
		Printf1624Num5(56, 7, tempint1, 4);
		if((*p) < 0)  k=1;
		else k=0;
		Printf1616(40, 7,POSTNEG[k],0); //符号显示
	}
	
	p++;
	if(0==RMS_Un[2]) PrintfBMP24104(40,10,0);	
	else
	{
		tempint1 = abss32(*p);
		Printf1624Num5(56, 10, tempint1, 4);
		if((*p) < 0)  k=1;
		else k=0;
		Printf1616(40, 10,POSTNEG[k],0); //符号显示
	}

	
	p++;
	if((0==RMS_Un[0])&&(0==RMS_Un[1])&&(0==RMS_Un[2])) PrintfBMP24104(40,13,0);
	else
	{	
		tempint1 = abss32(*p);
		Printf1624Num5(56, 13, tempint1, 4);
		if((*p) < 0)  k=1;
		else k=0;
		Printf1616(40, 13,POSTNEG[k],0); //符号显示
	}
		   
}

/*******************************************************
Name: void LCDDisplayPF(void)
Function: 功率因数显示
********************************************************/
void LCDDisplayPF(void)
{
	u8 i, oy;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[6],1); //
	else Printf1616(1, 0, ELTITLEC[6], 1); //无功功率瞬时值
	Printf816(208,0 ,"1.7",1); //1.7 
	//显示字符 
	oy=4;
	for(i=0; i<4; i++)
	{
		Printf81624(8, oy, ELPFTITLE[i]); //PFa PFb PFc PF
		oy += 3;	
	}
	//显示数据和单位
	LCDDisplayPFData();	
	
}

/*********************************************************
Name: void LCDDisplayUIData(void)
Function: 在单相界面下显示电压电流和频率值
Input:  None
Output: None
**********************************************************/
void LCDDisplayUIData(void)
{
	s32 tempint1;                                           // 临时整型变量
    u32 tempint2;                                           // 临时除数
    s32 templong;                                           // 临时长整型变量
    u8  PointPosition, unit; 
	//找出最大的值
	tempint1 = RMS_Un[0];
    templong = (u32)((float)tempint1 * PtResult / 100000);       // 乘以变比后再判断
    if (templong < 1) 
    {
        tempint2 = 1;
        PointPosition = 2;                             // XXX.XX 小数点位置1
        unit = 0;
    }
    else if (templong < 9) 
    {
        tempint2 = 10;
        PointPosition = 1;                             // XXXX.X 无小数点
        unit = 0;
    }
    else if (templong < 90) 
    {
        tempint2 = 100;
        PointPosition = 3;                             // XX.XXX 小数点位置2
        unit = 1;                   // "K"
    }
    else if (templong < 900) 
    {
        tempint2 = 1000;
        PointPosition = 2;                             // XXX.XX 小数点位置1
        unit = 1;                   // "K"
    }
    else
    {
        tempint2 = 10000;
        PointPosition = 1;                             // XXXX.X 无小数点
        unit = 1;                 // "K"
    }
    // Ua
    tempint1 = (s32)(((float)RMS_Un[0]) * PtResult / tempint2);
    Printf1624Num5(56, 4, tempint1, PointPosition);
    //单位显示
	if(1==unit) Printf81624(160, 4, "kV");
	else Printf81624(160, 4, " V");
    
//I
    tempint1 = RMS_I[0];
    templong = (u32)((float)tempint1 * CtResult / 100000);       // 乘以变比后再判断
    if (templong < 1) 
    {
        tempint2 = 1;
        PointPosition = 4;                             // X.XXXX 小数点位置1
        unit = 0;
    }
    else if (templong < 9) 
    {
        tempint2 = 10;
        PointPosition = 3;                             // XX.XXX 无小数点
        unit = 0;
    }
    else if (templong < 90) 
    {
        tempint2 = 100;
        PointPosition = 2;                             // XXX.XX 小数点位置2
        unit = 0;                   // 
    }
    else if (templong < 900) 
    {
        tempint2 = 1000;
        PointPosition = 1;                             // XXXX.X 小数点位置1
        unit = 0;                   // 
    }
    else if (templong < 9000) 
    {
        tempint2 = 10000;
        PointPosition = 3;                             // XX.XXX 无小数点
        unit = 1;                 // "K"
    }
    else if (templong < 90000) 
    {
    	tempint2 = 100000;
        PointPosition = 2;                             // XXX.XX 无小数点
        unit = 1; 
    }	
    else
    {
    	tempint2 = 1000000;
        PointPosition = 1;                             // XXX.XX 无小数点
        unit = 1; 
    }	
    
    // Ia
    tempint1 = (s32)(((float)RMS_I[0]) * CtResult / tempint2);
    Printf1624Num5(56, 7, tempint1, PointPosition);    
	if(1==unit) Printf81624(160, 7, "kA");
	else Printf81624(160, 7, " A");
		
    //F
    if(0 == RMS_Un[0]) PrintfBMP24104(40,10,0);
    else Printf1624Num5(56, 10, RMS_F, 3);  //xx.xxxHz
    		
	Printf81624(168, 10, "Hz");
}

/**********************************************************
Name: void LCDDisplayUI(void)
Fuctiong: 单相电压电流评论界面
Input： None
OutPut: None
***********************************************************/
void LCDDisplayUI(void)
{
	u8 i,oy;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[7],1); //
	else Printf1616(1, 0, ELTITLEC[7], 1); //相电压
	Printf816(208,0,"1.1",1); //1.1 
	//显示字符 
	oy=4;
	for(i=0; i<3; i++)
	{
		Printf81624(8, oy, (u8 *)ELUITITLE[i]); //U I F
		oy += 3;
	}
	//显示数据和单位
	LCDDisplayUIData(); 
	
}

/**********************************************
Name: void LCDDisplayPQSDataAll(void)
Function: 3个功率数据显示
Input:  *p:指向显示数据的头地址
Output: None
**********************************************/
void LCDDisplayPQSDataAll(void)
{
	s32 tempint1;                                      // 临时整型变量
    s32 templong2;                                     // 临时除数
    s32 templong;                                      // 临时长整型变量
    u8 PointPosition, unit, k;                                 // 小数点位置   
	u8 i, oy;
	
	tempint1 = RMS_S[3];
    templong = (u32)((float)tempint1 * PtCtResult/ 100000);    // 乘以变比后再判断
    if (templong < 1) 
    {
        templong2 = 1;                                    
        PointPosition = 1;   // XXXX.X 无小数点
        unit = 0;            //无单位
    }
    else if (templong < 9) 
    {
        templong2 = 10;
        PointPosition = 3;                             // XX.XXX 小数点位置2
        unit = 1;                   // "K"
    }
    else if (templong < 90) 
    {
        templong2 = 100;
        PointPosition = 2;                             // XXX.XX 小数点位置1
        unit = 1;                   // "K"
    }
    else if (templong < 900) 
    {
        templong2 = 1000;
        PointPosition = 1;                             // XXXX.X 小数点位置0
        unit = 1;                   // "K"
    }
    else if (templong < 9000) 
    {
        templong2 = 10000;
        PointPosition = 3;                             // XX.XXX 小数点位置2
        unit = 2;                   // "M"
    }
    else if (templong < 90000) 
    {
        templong2 = 100000;
        PointPosition = 2;                             // XXX.XX 小数点位置1
        unit = 2;                   // "M"
    }
    else if (templong < 900000) 
    {
        templong2 = 1000000;
        PointPosition = 1;                             // XXXX.X 小数点位置0
        unit = 2;                   // "M"
    }
    else if (templong < 9000000) 
    {
        templong2 = 10000000;
        PointPosition = 3;                             // XX.XXX 小数点位置2
        unit = 3;                   // "G"
    }
    else if (templong < 90000000) 
    {
        templong2 = 100000000;
        PointPosition = 2;                             // XXX.XX 小数点位置1
        unit = 3;                   // "G"
    }
    else
    {
    	templong2 = 1000000000;
        PointPosition = 1;                             // XXXX.X 小数点位置1
        unit = 3;                   // "G" 
    }	
    
    tempint1 = (s32)(((float)abss32(RMS_P[3])) * PtCtResult / templong2); // P
	Printf1624Num5(56, 4, tempint1, PointPosition);   
	if((RMS_P[3]) < 0)  k=1;   
	else k=0;
	Printf1616(40, 4,POSTNEG[k],0); //符号显示
		
	tempint1 = (s32)(((float)abss32(RMS_Q[3])) * PtCtResult / templong2); // Q
	Printf1624Num5(56, 7, tempint1, PointPosition);
	if((RMS_Q[3]) < 0)  k=1;
	else k=0;
	Printf1616(40, 7,POSTNEG[k],0); //符号显示
		
	tempint1 = (s32)(((float)abss32(RMS_S[3])) * PtCtResult / templong2); // S
	Printf1624Num5(56, 10, tempint1, PointPosition);
	if((RMS_S[3]) < 0)  k=1;
	else k=0;
	Printf1616(40, 10,POSTNEG[k],0); //符号显示
	
	if((0==RMS_Un[0])&&(0==RMS_Un[1])&&(0==RMS_Un[2])) PrintfBMP24104(40,13,0);
	else
	{	
		tempint1 = abss32(RMS_PF[3]); //PF
		Printf1624Num5(56, 13, tempint1, 4);
		if((RMS_PF[3]) < 0)  k=1;
		else k=0;
		Printf1616(40, 13,POSTNEG[k],0); //符号显示
	}
	//单位显示
	oy = 15;
	for(i=0; i<3; i++)
	{
		Printf81624Unit(160, oy, unit);
		oy -= 3;
	}	   
}

/**********************************************
Name: void LCDDisplayPQS(void)
Function: 总功率显示
Input:  None
Output: None
***********************************************/
void LCDDisplayPQS(void)
{
	u8 i, oy;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[3],1); //
	else Printf1616(1, 0, ELTITLEC[8], 1); //功率瞬时值
	if(INPUT11 == MeterSetData.c_input[0]) Printf816(208,0,"1.2",1); //1.2 
	else if(INPUT33 == MeterSetData.c_input[0]) Printf816(208,0,"1.3",1); //1.3 
	else Printf816(208,0,"1.4",1);
	//显示字符 
	oy=4;
	for(i=0; i<4; i++)
	{
		Printf81624(8, oy, ELPQSTITLE[i]); //P Q S PF
		Printf81624(172, oy, ELPQSUNIT[i]); //单位W var VA
		oy += 3;
	}
	//显示数据和单位
	LCDDisplayPQSDataAll();
		
}


////

/*********************************************************
Name: void LCDDisplayUDataBar(s16 *p)
Function: 三个电压棒图显示
Input:  *p 电压数据指针
Output: None
**********************************************************/
void LCDDisplayUDataBar(u16 *p)
{
	u16 tempint1, LNum=3;
    u8 oy, i;  
    
    oy = 3;
    for(i=0; i<LNum; i++)
    {
    	tempint1 =(u16)(((u32)(*p++)) *10/MeterSetData.c_pt2 );
	    Printf999(190,oy,tempint1,0);
	    Printf816(214,oy,"%",0);
	    if(tempint1>158) tempint1 = 158;
	    PrintfLine((25),oy,tempint1,1);
	    PrintfLine((25+tempint1),oy,(159-tempint1),0);
	    oy += 4;
	}
}

void LCDDisplayUlDataBar(u16 *p)
{
	u16 tempint1;
    u8 oy, i;  
    oy = 3;
    for(i=0; i<3; i++)
    {
	    tempint1 =(u16)(((float)(*p++))*5.773672/MeterSetData.c_pt2 );
	    Printf999(190,oy,tempint1,0);
	    Printf816(214,oy,"%",0);
	    if(tempint1>158) tempint1 = 158;
	    PrintfLine((25),oy,tempint1,1);
	    PrintfLine((25+tempint1),oy,(159-tempint1),0);
	    oy += 4;
	}
}

//电量棒图公共显示
void InstantBarPublic(u8 m)
{
	u8 oy,i;
	oy = 3;
	for(i=0; i<m; i++)
	{
		DrawLineX(0,oy,24,0x01);
		DrawLineX(0,(oy+1),24,0x80);
		oy += 4;	
	}	
	Printf86(22,17,"0",0);
	Printf86(39,17,"20",0);
	Printf86(69,17,"50",0);
	Printf86(117,17,"100",0);
	Printf86(167,17,"150",0);
	
	DrawLineX(0,17,239,0x01); //长横线
	DrawLineY(24,3,14,0xaa); //0%
	DrawLineY(44,3,14,0xaa); //20% 竖线
	DrawLineY(74,3,14,0xaa); //50%
	DrawLineY(124,3,14,0xaa); //100%
	DrawLineY(174,3,14,0xaa); //150%
	
}
/**********************************************************
Name: void LCDDisplayUnBar(void)
Fuctiong: 三相四线相电压棒图界面
Input： None
OutPut: None
***********************************************************/
void LCDDisplayUnBar(void)
{
	u8 i,oy;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[0],1); //
	else Printf1616(1, 0, ELTITLEC[0], 1); //相电压
	Printf816(208,0,"1.1",1); //1.1 
	//显示字符 
	oy=3;
	for(i=0; i<3; i++)
	{
		Printf816(0, oy, (u8 *)ELUNTITLE[i], 0); //V1 V2 V3 
		oy += 4;
	}
	
	LCDDisplayUDataBar(MeterData.Un); 
	InstantBarPublic(3); 
	//显示棒图
	
	
}

/***********************************************************
Name: void LCDDisplayUlBar(void)
Fuctiong: 三相四线线电压棒图界面
Input： None
OutPut: None
***********************************************************/
void LCDDisplayUlBar(void)
{
	u8 i,oy;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[1],1); //
	else Printf1616(1, 0, ELTITLEC[1], 1); //线电压
	if(INPUT34 == MeterSetData.c_input[0]) Printf816(208,0,"1.2",1); //1.2 
	else if(INPUT33 == MeterSetData.c_input[0]) Printf816(208,0,"1.1",1); //1.1 
	//显示字符 
	oy=3;
	for(i=0; i<3; i++)
	{
		Printf816(0, oy, ELULTITLE[i], 0); //V12 V23 V31
		oy += 4;
	}
	
	//显示数据棒图
	if(INPUT34 == MeterSetData.c_input[0]) LCDDisplayUlDataBar(MeterData.Ul);
	else if(INPUT33 == MeterSetData.c_input[0]) LCDDisplayUDataBar(MeterData.Ul);
	 
	InstantBarPublic(3); 
	
}

/*********************************************************
Name: void LCDDisplayIDataBar(void)
Function: 三相电流棒图显示
Input: None
Output: None
**********************************************************/
void LCDDisplayIDataBar(u16 *p)
{
	u16 tempint1;
    u8 oy, i;  
    oy = 3;
    for(i=0; i<3; i++)
    {
	    tempint1 =(u16)(((u32)(*p++)) /MeterSetData.c_ct2/10 );
	    Printf999(190,oy,tempint1,0);
	    Printf816(214,oy,"%",0);
	    if(tempint1>158) tempint1 = 158;
	    PrintfLine((25),oy,tempint1,1);
	    PrintfLine((25+tempint1),oy,(159-tempint1),0);
	    oy += 4;
	}
}


/************************************************************
Name: void LCDDisplayIBar(void)
Function: 三相电流棒图显示
Input:  None
Output: None
************************************************************/
void LCDDisplayIBar(void)
{
	u8 i, oy;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[2],1); //
	else Printf1616(1, 0, ELTITLEC[2], 1); //电流
	if(INPUT34 == MeterSetData.c_input[0]) Printf816(208,0,"1.3",1); //1.3 
	else Printf816(208,0,"1.2",1); //1.2 
	//显示字符 
	oy=3;
	for(i=0; i<3; i++)
	{
		Printf816(0, oy, ELITITLE[i], 0); //Ia Ib Ic
		oy += 4;
	}
	//显示数据棒图
	LCDDisplayIDataBar(MeterData.I); 
	InstantBarPublic(3); 
	
}


/**********************************************
Name: void LCDDisplayPQSDataBar(void)
Function: 4个功率数据棒图显示
Input:  *p:指向显示数据的头地址
Output: None
**********************************************/
void LCDDisplayPQSDataBar(s16 *p)
{
	
	u16 tempint1;
	u32 midpt;
    u8 oy, i; 
    midpt =  ((u32)MeterSetData.c_pt2)*MeterSetData.c_ct2;
    oy = 3;
    for(i=0; i<4; i++)
	{
		tempint1 =(u16)(((u32)((abss16)(*p++)))*100/midpt );
	    
	    Printf999(190,oy,tempint1,0);
	    Printf816(214,oy,"%",0);
	    if(tempint1>158) tempint1 = 158;
	    PrintfLine((25),oy,tempint1,1);
	    PrintfLine((25+tempint1),oy,(159-tempint1),0);
	    oy += 4;
	}
}

/**********************************************
Name: void LCDDisplayPBar(void)
Function: 三相四线有功功率棒图显示
Input:  None
Output: None
***********************************************/
void LCDDisplayPBar(void)
{
	u8 i, oy;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[3],1); //
	else Printf1616(1, 0, ELTITLEC[3], 1); //有功功率瞬时值
	Printf816(208,0,"1.4",1); //1.4 	
	//显示字符 
	oy=3;
	for(i=0; i<4; i++)
	{
		Printf816(0, oy, ELPTITLE[i], 0); //Pa Pb Pc P
		oy += 4;	
	}
	//显示数据棒图
	LCDDisplayPQSDataBar(MeterData.P);
	InstantBarPublic(4); 
		
}

/**********************************************
Name: void LCDDisplayQBar(void)
Function: 三相四线无功功率棒图显示
Input:  None
Output: None
***********************************************/
void LCDDisplayQBar(void)
{
	u8 i, oy;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[4],1); //
	else Printf1616(1, 0, ELTITLEC[4], 1); //无功功率
	Printf816(208,0,"1.5",1); //1.5
		 	
	//显示字符 
	oy=3;
	for(i=0; i<4; i++)
	{
		Printf816(0, oy, ELQTITLE[i], 0); //Qa Qb Qc Q
		oy += 4;
	}
	//显示数据棒图
	LCDDisplayPQSDataBar(MeterData.Q);	
	InstantBarPublic(4); 
}

/**********************************************
Name: void LCDDisplaySBar(void)
Function: 三相四线视在功率显示
Input:  None
Output: None
***********************************************/
void LCDDisplaySBar(void)
{
	u8 i, oy;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[5],1); //
	else Printf1616(1, 0, ELTITLEC[5], 1); //视在功率
	Printf816(208,0,"1.6",1); //1.6 	
	//显示字符 
	oy=3;
	for(i=0; i<4; i++)
	{
		Printf816(0, oy, ELSTITLE[i], 0); //Sa Sb Sc S
		oy += 4;	
	}
	//显示数据和单位
	LCDDisplayPQSDataBar(MeterData.S);	
	InstantBarPublic(4); 
}

/*******************************************************
Name: void LCDDisplayPFDataBar(s16 *p)
Function: 功率因数数据显示
Input:  None
Output: None
********************************************************/
void LCDDisplayPFDataBar(s16 *p)
{
	u16 tempint1;
    u8 oy, i;  
    oy = 3;
    for(i=0; i<4; i++)
    {
    	tempint1 =(u16)(((u32)((abss16)(*p++))) /10 );
		Printf999(190,oy,tempint1,0);
		Printf816(214,oy,"%",0);
		if(tempint1>158) tempint1 = 158;
		PrintfLine((25),oy,tempint1,1);
		PrintfLine((25+tempint1),oy,(159-tempint1),0);
		oy += 4;
	}	
}

/*******************************************************
Name: void LCDDisplayPFBar(void)
Function: 功率因数显示
********************************************************/
void LCDDisplayPFBar(void)
{
	u8 i, oy;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[6],1); //
	else Printf1616(1, 0, ELTITLEC[6], 1); //无功功率瞬时值
	Printf816(208,0,"1.7",1); //1.7 	
	//显示字符 
	oy=3;
	for(i=0; i<4; i++)
	{
		Printf816(0, oy, ELPFTITLE[i], 0); //PFa PFb PFc PF
		oy += 4;	
	}
	//显示数据和单位
	LCDDisplayPFDataBar(MeterData.PF);	
	InstantBarPublic(4);
}

/*********************************************************
Name: void LCDDisplayUIDataBar(void)
Function: 在单相界面下显示电压电流棒图
Input:  None
Output: None
**********************************************************/
void LCDDisplayUIDataBar(void)
{
	u16 tempint1;
    u8 oy; 
    oy = 3;
    tempint1 =(u16)(((u32)(MeterData.Un[0])) *10/MeterSetData.c_pt2);
	Printf999(190,oy,tempint1,0);
	Printf816(214,oy,"%",0);
	if(tempint1>158) tempint1 = 158;
	PrintfLine((25),oy,tempint1,1);
	PrintfLine((25+tempint1),oy,(159-tempint1),0);
	
    oy = 7;
    tempint1 =(u16)(((u32)(MeterData.I[0]))/(MeterSetData.c_ct2*10));
	Printf999(190,oy,tempint1,0);
	Printf816(214,oy,"%",0);
	if(tempint1>158) tempint1 = 158;
	PrintfLine((25),oy,tempint1,1);
	PrintfLine((25+tempint1),oy,(159-tempint1),0);
     

}

/**********************************************************
Name: void LCDDisplayUnBar(void)
Fuctiong: 单相电压电流棒图
Input： None
OutPut: None
***********************************************************/
void LCDDisplayUIBar(void)
{
	u8 i,oy;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[7],1); //
	else Printf1616(1, 0, ELTITLEC[7], 1); //相电压
	Printf816(208,0,"1.1",1); //1.1 
	//显示字符 
	oy=3;
	for(i=0; i<2; i++)
	{
		Printf816(0, oy, (u8 *)ELUITITLE[i], 0); //U I
		oy += 4;
	}
	//显示数据和单位
	LCDDisplayUIDataBar(); 
	InstantBarPublic(2);
}

/**********************************************
Name: void LCDDisplayPQSDataAllBar(void)
Function: 3个功率数据棒图显示
Input:  *p:指向显示数据的头地址
Output: None
**********************************************/
void LCDDisplayPQSDataAllBar(void)
{
	u16 tempint1;
	u32 midpt;
    u8 oy, i; 
    s16 *p;
    p = &MeterData.P[3];
    if(INPUT11 == MeterSetData.c_input[0]) midpt =  ((u32)MeterSetData.c_pt2)*MeterSetData.c_ct2;
    else if(INPUT33 == MeterSetData.c_input[0]) midpt = (u32)(((float)MeterSetData.c_pt2)*MeterSetData.c_ct2*1.732);
    oy = 3;
    for(i=0; i<4; i++)
    {
    	if(3==i) tempint1 = (((abss16(*p)))/10);
    	else  tempint1 =(u16)(((u32)((abss16)(*p)))*100/midpt );
	    Printf999(190,oy,tempint1,0);
	    Printf816(214,oy,"%",0);
	    if(tempint1>158) tempint1 = 158;
	    PrintfLine((25),oy,tempint1,1);
	    PrintfLine((25+tempint1),oy,(159-tempint1),0);
	    oy += 4;
	    p += 4;
	} 
	
	  
}

/**********************************************
Name: void LCDDisplayPQSBar(void)
Function: 总功率显示
Input:  None
Output: None
***********************************************/
void LCDDisplayPQSBar(void)
{
	u8 i, oy;
	//头标题
	if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ELTITLEE[3],1); //
	else Printf1616(1, 0, ELTITLEC[8], 1); //功率
	if(INPUT11 == MeterSetData.c_input[0]) Printf816(208,0,"1.2",1); //1.2 
	else if(INPUT33 == MeterSetData.c_input[0]) Printf816(208,0,"1.3",1); //1.3 
	else Printf816(208,0,"1.4",1);
	//显示字符 
	oy=3;
	for(i=0; i<4; i++)
	{
		Printf816(8, oy, ELPQSTITLE[i], 0); //P Q S PF
		oy += 4;
	}
	//显示数据和单位
	LCDDisplayPQSDataAllBar();
	InstantBarPublic(4);	
}
//##############################


/*****************************************************************
Name: void LCDDisplayENData(void)
Function: 电能数据显示
Input: oy:位置， *p:数据指针
Output: None
*****************************************************************/
void LCDDisplayENData(u8 oy, u32 *p)
{
	u32 templong;
    u32 templong1;
    u32 templong2;
    u8 PointPosition;
	u8 unit; 
	templong1 = *p;
	templong = MUL3DIV1_32_2(templong1,100000000);  // 乘以变比后再判断
    if (templong < 1) 
    {
        templong2 = 1;                                    
        PointPosition = 3;                              // XXXXX.XXX
 		unit = 0x01;
    }
    else if (templong < 9) 
    {
        templong2 = 10;
        PointPosition = 2;                              // XXXXXX.XX
        unit = 0x01;
    }
    else if (templong < 90) 
    {
        templong2 = 100;
        PointPosition = 1;                              // XXXXXXX.X
        unit = 0x01;
    }
    else if (templong < 900) 
    {
        templong2 = 1000;
        PointPosition = 3;                              // XXXXX.XXX
        unit = 0x02;             // "M"W.h
    }
    else if (templong < 9000) 
    {
        templong2 = 10000;
        PointPosition = 2;                              // XXXXXX.XX
        unit = 0x02;               // "M"W.h
    }
    else if (templong < 90000) 
    {
        templong2 = 100000;
        PointPosition = 1;                              // XXXXXXX.X
        unit = 0x02;              // "M"W.h
    }
    else
    {
        templong2 = 1000000;
        PointPosition = 3;                              // XXXXX.XXX
        unit = 0x03;              // "G"W.h
    }

    templong = MUL3DIV1_32_2(templong1,templong2);
    Printf1624Num8(24, oy, templong, PointPosition);  
    Printf81624Unit(167, oy, unit); //单位
	
		
}


//电能界面字符
const u8 ENTITLEC[5][10]=
{
	{76,77,10,11,0}, //双向电能
	{79,80,81,53,52,10,11,0}, //四象限无功电能
	{86,14,15,10,11,0}, //复费率电能
	{218,63,86,14,15,10,11,0}, //本月复费率电能
	{197,42,63,86,14,15,10,11,0}, //上 月复费率电能
};
const u8 ENTITLEE[8][30] =
{
	"Total Import/Export Energy",
	"4 Quadrant Reactive Energy",
	"Total Active Energy",
	"Present Month Active Energy",
	"Past   st Month Active Energy",
	"Past   nd Month Active Energy",
	"Past   rd Month Active Energy",
	"Past   th Month Active Energy",
};


const u8 ENEN1TITLE[4]=
{
	10,11,10,11,
};
const u8 ENEN1UNIT[4][5] =
{
	"Wh","Wh","varh","varh"	
};
const u8 ENEN2TITLE[4]=
{
	12,13,14,15
};
const u8 ENEN3TITLE[15][5]=
{
	"2.1","2.2","2.3","2.4",
	"2.5","2.6","2.7","2.8",
	"2.9","2.10","2.11","2.12",
	"2.13","2.14","2.15",
};

//复费率标志
const u8 ENEN4TITL[5][2] =
{
	{78,0,}, //总
	{58,0,}, //尖
	{59,0,}, //峰
	{97,0,}, //平
	{60,0,}, //谷
};

const u8 ENEN4TITLE[5][3] =
{
	"T ","T1","T2","T3","T4",
};


/*******************************************************************
Name: void LCDDisplayEN(void)
Function: 电能数据显示
Input:  None
Output: None
*******************************************************************/
void LCDDisplayEN(void)
{
	u8 i, oy;
	TopShadow(); //顶部阴影显示
	if(1==MeterSetData.c_lcdlanguage)  PrintfKeyF4(K_UP,K_DOWN,K_MENUE,K_NO); 	//四个按键
	else PrintfKeyF4(K_UP,K_DOWN,K_MENU,K_NO); 	//四个按键
	if(DisplayItem<9) Printf816(216,0,(u8 *)ENEN3TITLE[DisplayItem],1); //2.1 
	else Printf816(212,0,(u8 *)ENEN3TITLE[DisplayItem],1); //2.1 
	//头标题
	if(DisplayItem>DISPLAY_TOU1)
	{
		if(1 == MeterSetData.c_lcdlanguage) 
		{
			if((DisplayItem<DISPLAY_TOU5)) Printf816(1,0,ENTITLEE[DisplayItem],1); //
//			else if(DisplayItem>DISPLAY_TOU11) Printf816(1,0,ENTITLEE[4],1); //
			else Printf816(1,0,ENTITLEE[7],1); //
			Printf99(32,0,(DisplayItem-DISPLAY_TOU1),1);
		}
		else 
		{	
			Printf1616(1, 0, ENTITLEC[4], 1); //
			Printf99(17,0,(DisplayItem-DISPLAY_TOU1),1);
		}
	}	
	else
	{
		if(1 == MeterSetData.c_lcdlanguage) Printf816(1,0,ENTITLEE[DisplayItem],1); //
		else Printf1616(1, 0, ENTITLEC[DisplayItem], 1); //双向总电能
	}	
	switch(DisplayItem)
	{
		case DISPLAY_EN: //双向电能
			//显示字符 
			oy=4;
			for(i=0; i<4; i++)
			{
				Printf1624(4, oy, ENEN1TITLE[i]); //+-+-
				Printf81624(179,oy,(u8 *)ENEN1UNIT[i]); //W
				LCDDisplayENData(oy, &MeterData.EN[i]);  //显示数据和单位
				oy += 3;
			}
			break;
		case DISPLAY_EQFOUR: //四象限无功电能
			oy=4;
			for(i=0; i<4; i++)
			{
				Printf1624(1, oy, ENEN2TITLE[i]); //1234
				Printf81624(179,oy,(u8 *)ENEN1UNIT[2]); //varh	
				LCDDisplayENData(oy,&MeterData.VarhFour[i]);  //显示四象限无功电能数据
				oy += 3;
			}
			break;
		default: //复费率电能
			oy=2;
			for(i=0; i<5; i++)
			{
				if(1==MeterSetData.c_lcdlanguage) Printf81624(4, oy, (u8 *)ENEN4TITLE[i]); 
				else Printf161624(4, oy, (u8 *)ENEN4TITL[i]); 	
				Printf81624(179,oy,(u8 *)ENEN1UNIT[0]); //Wh	
				LCDDisplayENData(oy,&MonthEnergy[0].l_EPPTou[DisplayItem-DISPLAY_TOU][i]);  //显示四象限无功电能数据
				oy += 3;
			}	
			break;
		
	}
	
	
	
}


////谐波
const u8 THD1TITLEC[9][7] =
{
	{10,48,10,50,23,24,0}, //电压电流相位
	{10,48,92,18,9,0}, //电压序分量
	{10,50,92,18,9,0}, //电流序分量
	{10,48,17,59,38,56,0}, //电压波峰系数
	{10,50,0,38,56,0}, //电流K系数
	{78,16,17,87,88,15,0},  //总谐波畸变率
	{16,17,89,51,15,0}, //谐波含有率
	{10,48,16,17,90,91,0},	//电压谐波棒图
	{10,50,16,17,90,91,0},	//电流谐波棒图
} ;
const u8 THD1TITLEE[9][26]=
{
	"Phase Angle",
	"Voltage Unbalance",
	"Current Unbalance",
	"Crest Factor",
	"K Factor",
	"Total Harmonic Distortion",
	"Harmonic Ratio",
	"Harmonic Spectrum(V)",
	"Harmonic Spectrum(I)",
};

const u8 QUA2TITLE[1][2]=
{
	{93,0},	//Φ
};
const u8 QUA3TITLEC[2][4][5]=
{
 {	{84,92,10,48,0}, //正序电压
	{94,92,10,48,0}, //负序电压
	{95,92,10,48,0}, //零序电压
	{96,97,98,99,0}, //不平衡度
 },	
{	{84,92,10,50,0}, //正序电流
	{94,92,10,50,0}, //负序电流
	{95,92,10,50,0}, //零序电流
	{96,97,98,99,0}, //不平衡度
},	
};
const u8 QUA3TITLEE[2][4][11]=
{
	"Pos.-Seq.",
	"Neg.-Seq.",
	"Zero-seq.",
	"Vunb     ",
	
	"Pos.-Seq.",
	"Neg.-Seq.",
	"Zero-seq.",
	"Iunb     ",	
};

const u8 QUA4TITLEE[2][3][6]=
{
	"UKPR1",
	"UKPR2",
	"UKPR3",
	
	"IK1",
	"IK2",
	"IK3",
};

const u8 THD2TITLE[6][3] =
{
	"V1","V2","V3",
	"I1","I2","I3",	
};
const u8 THD3TITLE[6][6][3] =
{
	{"02","04","06","08","10","12",},
	{"14","16","18","20","22","24",},
	{"26","28","30","32","34","36",},
	{"38","40","42","44","46","48",},
	{"50","52","54","56","58","60"},
	{"62",},
};


/**************************************************************************
Name: void LCDDisplayQuality(void)
Function: 谐波数据和波形显示，谐波波形只显示前面主要的一部分数据
Input:  None
Output: None
***************************************************************************/
void LCDDisplayQuality(void)
{
	u8 i, j, m, n, k,l, ox, oy;
	u16 dataa,datab,datac, midDisplayItem;

	TopShadow(); //顶部阴影显示
	if(1==MeterSetData.c_lcdlanguage) //英文显示
	{
		if(DisplayItem<=DISPLAY_THDTHD) Printf816(1,0,&THD1TITLEE[DisplayItem][0],1);
		else if(DisplayItem < DISPLAY_THDU1) Printf816(1,0,&THD1TITLEE[6][0],1);
		else if(DisplayItem < DISPLAY_THDI1) Printf816(1,0,&THD1TITLEE[7][0],1);		
		else Printf816(1,0,&THD1TITLEE[8][0],1);
		PrintfKeyF4(K_UP,K_DOWN,K_MENUE,K_NO); 	//四个按键
	}
	else  //中文显示
	{
		if(DISPLAY_QUAIK==DisplayItem)
		{
			Printf1616(1,0,&THD1TITLEC[DisplayItem][0],1);	
			Printf816(33,0,"K",1);
			Printf1616(41,0,&THD1TITLEC[DisplayItem][3],1);	
		}	
		else if(DisplayItem<=DISPLAY_THDTHD) Printf1616(1,0,&THD1TITLEC[DisplayItem][0],1);
		else if(DisplayItem < DISPLAY_THDU1) Printf1616(1,0,&THD1TITLEC[6][0],1);
		else if(DisplayItem < DISPLAY_THDI1) Printf1616(1,0,&THD1TITLEC[7][0],1);	
		else Printf1616(1,0,&THD1TITLEC[8][0],1);	
		PrintfKeyF4(K_UP,K_DOWN,K_MENU,K_NO); 	//四个按键	
	}	
	//显示页数3.xx
	Printf816(208,0,"3.",1);	
	if(5==MeterSetData.c_set[THDSET])
	{
		midDisplayItem=DisplayItem;		
	}	
	else if(4==MeterSetData.c_set[THDSET]) //51次谐波
	{
		midDisplayItem=DisplayItem;	
		if(DisplayItem>=DISPLAY_THDI1) midDisplayItem -= 4;
		else if(DisplayItem>=DISPLAY_THDU1) midDisplayItem -= 3;
	}	
	else //31次
	{
		midDisplayItem=DisplayItem;	
		if(DisplayItem>=DISPLAY_THDI1) midDisplayItem -= 10;
		else if(DisplayItem>=DISPLAY_THDU1) midDisplayItem -= 7;
	}	
	if(midDisplayItem<9) Print816Num(220,0,(midDisplayItem+1),1);
	else if(midDisplayItem<19)
	{
		Printf816(220,0,"1",1);
		Print816Num(228,0,(midDisplayItem-9),1);	
	}
	else if(midDisplayItem<29) 
	{
		Printf816(220,0,"2",1);
		Print816Num(228,0,(midDisplayItem-19),1);	
	}
	else 
	{
		Printf816(220,0,"3",1);
		Print816Num(228,0,(midDisplayItem-29),1);		
	}	
	
	if(DisplayItem==DISPLAY_QUAPHASE) LCDDisplayPhase();  //相角
	else if(DisplayItem<=DISPLAY_QUAIIM) LCDDisplayImbalance(); 	//序分量	
	else if(DisplayItem<=DISPLAY_QUAIK) LCDDisplayUpeakIk(); //电压波峰和电流波峰系数	
	else if(DisplayItem<DISPLAY_THD2) //总谐波畸变率
	{
		Printf816(52,2,"THDU%",0);
		Printf816(164,2,"THDI%",0);
		
		Printf816(4,5,"1",0);
		Printf816(4,9,"2",0);
		Printf816(4,13,"3",0);		
		//显示数据
		oy = 5;
		for(i=0; i<3; i++)
		{
			Printf1624Num4(36, oy, MeterQuality.UTHD[i]/10,1);
			Printf1624Num4(148, oy, MeterQuality.ITHD[i]/10,1);
			
			oy += 4;	
		}
		//X2
		oy = 4;
		for(i=0; i<4; i++)
		{
			DrawLineX2(0, oy, 240, 0x01);
			oy += 4;
		}
		//Y
		DrawLineY(16, 2, 14, 0xAA);
		DrawLineY(128,2, 14, 0xaa);
			
	}
	else if(DisplayItem <DISPLAY_THDU1)
	{
		//显示谐波次数, 在63次谐波时，最后一个界面是2行数据
		if((5==MeterSetData.c_set[THDSET])&&(DISPLAY_THD62==DisplayItem)) k=2;
		else k=5;
		m=((DisplayItem-DISPLAY_THD2)*5) + 2;
		ox = 37;
		for(i=0; i<k; i++)
		{
			//显示谐波次数
			j = m/10;
			Print816Num(ox, 2, j, 0);
			j = m%10;
			Print816Num((ox+8),2, j, 0);
			ox += 42;
			m += 1;
		}
		oy = 4;  //显示V1...I3
		for(i=0; i<6; i++)
		{
			Printf816(0x02,oy,(u8 *)&THD2TITLE[i][0],0);
			oy += 2;
		}
		//显示谐波数据
		m = ((DisplayItem-DISPLAY_THD2)*5); //显示数据的首地址
		ox = 29;
		oy = 4;
		for(i=0; i<k; i++)
		{
			for(j=0; j<6; j++)
			{
				Printf816Num3(ox, oy,(HarmonicData[m][j]/10),1);
				oy += 2;	
			}
			m += 1;
			ox += 42;
			oy = 4;
		}
	
		//画线XY
		//X2
		oy = 4;
		for(i=0; i<7; i++)
		{
			DrawLineX2(0, oy, 240, 0x01);
			oy += 2;
		}
		//Y
		ox = 24;
		for(i=0; i<5; i++)
		{
			DrawLineY(ox, 2, 14, 0xAA);	
			ox += 42;
		}		
			
	}
	else //棒图
	{
		//清除棒图区
		for(i=2; i<18; i++)
		{
			for(j=0; j<240; j++) DISP_RAM[i][j]=0;	
		}

		//根据界面觉得显示数据标尺
		if(DisplayItem<DISPLAY_THDI1) j = DisplayItem - DISPLAY_THDU1;
		else j = DisplayItem - DISPLAY_THDI1;
		if(0==j) Printf86(32, 17, "01", 0);	//显示01
		
		if (((DISPLAY_THDU6==DisplayItem)||(DISPLAY_THDI6==DisplayItem)))	//最后一页
		{
			Printf86(48, 17, "62", 0);
			l=3;
		}	
		else if (((DISPLAY_THDU5==DisplayItem)||(DISPLAY_THDI5==DisplayItem))&&(4==MeterSetData.c_set[THDSET]))	//51次谐波时，最后一页
		{
			Printf86(48, 17, "50", 0);
			l=3;
		}	
		else if (((DISPLAY_THDU3==DisplayItem)||(DISPLAY_THDI3==DisplayItem))&&(3==MeterSetData.c_set[THDSET])) //31次谐波最后一页
		{
			Printf86(48, 17, "26", 0);
			Printf86(80, 17, "28", 0);
			Printf86(112, 17, "30", 0);
			l=7;
		}
		else
		{	
			ox = 48;
			for(i=0; i<6; i++)
			{
				Printf86(ox, 17, (u8 *)&THD3TITLE[j][i][0], 0);	
				ox += 32;
			}
			l=12;
		}
			
		Printf86(0x00,3,"100",0);
		Printf86(0x08,8,"60",0);
		Printf86(0x08,13,"20",0);
		
		DrawLineX(27,3,4, 0x10); //100
		DrawLineX(28,6,3,0x01); //80
		DrawLineX(28,8,3,0x10); //60
		DrawLineX(28,11,3,0x01);  //40
		DrawLineX(28,13,3,0x10);  //20
		DrawLineX(24, 16,215, 0x01);
		DrawLineY(31, 3, 13, 0xff);
		DrawLineY(31,16, 1, 0xfe);
		
		//数据显示
		ox = 34;
		if(DisplayItem<DISPLAY_THDI1) m=(DisplayItem - DISPLAY_THDU1)*12; //电压棒图界面 
		else m=(DisplayItem - DISPLAY_THDI1)*12; 
			
		for(i=0; i<l; i++)
		{
			if(DisplayItem<DISPLAY_THDI1) //电压棒图界面
			{
				if(0==m) //含有基波的界面
				{
					if(0==i)
					{
						if(MeterData.Un[0]>=UMLIMIT) dataa = 100;
						else dataa = 0;
						if(MeterData.Un[1]>=UMLIMIT) datab = 100;
						else datab = 0;
						if(MeterData.Un[2]>=UMLIMIT) datac = 100;
						else datac = 0;	
					}
					else
					{
						dataa = (HarmonicData[i-1][0]/100);
						datab = (HarmonicData[i-1][1]/100);
						datac = (HarmonicData[i-1][2]/100);
					}			
				}
				else
				{
					dataa = (HarmonicData[m+i-1][0]/100);
					datab = (HarmonicData[m+i-1][1]/100);
					datac = (HarmonicData[m+i-1][2]/100);
				}		
				
			}
			else //电流棒图界面
			{	
				if(0==m) //含有基波的界面
				{
					if(0==i)
					{
						if(MeterData.I[0]>=IMLIMIT) dataa = 100;
						else dataa = 0;
						if(MeterData.I[1]>=IMLIMIT) datab = 100;
						else datab = 0;
						if(MeterData.I[2]>=IMLIMIT) datac = 100;
						else datac = 0;	
					}
					else
					{
						dataa = (HarmonicData[i-1][3]/100);
						datab = (HarmonicData[i-1][4]/100);
						datac = (HarmonicData[i-1][5]/100);
					}			
				}
				else
				{
					dataa = (HarmonicData[m+i-1][3]/100);
					datab = (HarmonicData[m+i-1][4]/100);
					datac = (HarmonicData[m+i-1][5]/100);
				}			
			}
			if(dataa>110) dataa = 110;
			if(datab>110) datab = 110;
			if(datac>110) datac = 110;
			if(dataa<1) dataa = 1;
			if(datab<1) datab = 1;	
			if(datac<1) datac = 1;
			//A相棒图
			k = dataa/8;
			oy=15;
			while(k)
			{
				DISP_RAM[oy][ox] = 0xff;
				DISP_RAM[oy][ox+1] = 0xff;	
				k--;
				oy--;
			}
			k=dataa%8;
			if(k)
			{
				n=0;
				for(j=0; j<k; j++)
				{
					n |= (u8)(1<<(7-j));	
				}
				DISP_RAM[oy][ox] = n;
				DISP_RAM[oy][ox+1] = n;	
			}
			ox += 4;
		
			//B相棒图
			k = datab/8;
			oy=15;
			while(k)
			{
				DISP_RAM[oy][ox] = 0xff;
				DISP_RAM[oy][ox+1] = 0xff;	
				k--;
				oy--;
			}
			k=datab%8;
			if(k)
			{
				n=0;
				for(j=0; j<k; j++)
				{
					n |= (u8)(1<<(7-j));	
				}
				DISP_RAM[oy][ox] = n;
				DISP_RAM[oy][ox+1] = n;	
			}
			ox += 4;
				
			//C相棒图
			k = datac/8;
			oy=15;
			while(k)
			{
				DISP_RAM[oy][ox] = 0xff;
				DISP_RAM[oy][ox+1] = 0xff;	
				k--;
				oy--;
			}
			k=datac%8;
			if(k)
			{
				n=0;
				for(j=0; j<k; j++)
				{
					n |= (u8)(1<<(7-j));	
				}
				DISP_RAM[oy][ox] = n;
				DISP_RAM[oy][ox+1] = n;	
			}
			ox += 8;
		}	
	}	
}




/**************************************************************************
Name: void LCDDisplayPhase(void)
Function: 电压电流相位角显示
Input:  None
Output: None
**************************************************************************/

void LCDDisplayPhase(void)
{
	u8 i, oy;
	Printf1616(59,2,&QUA2TITLE[0][0],0);
	Printf1616(176,2,&QUA2TITLE[0][0],0);
	Printf816(75,2,"V",0);
	Printf816(192,2,"I",0);
	Printf816(4,5,"1",0);
	Printf816(4,9,"2",0);
	Printf816(4,13,"3",0);
	
	//显示数据
	oy = 5;
	for(i=0; i<3; i++)
	{
		Printf1624Num4(36, oy, MeterQuality.YU[i],1);
		Printf1624Num4(148, oy, MeterQuality.YI[i],1);
		oy += 4;	
	}
	
	//X2
	oy = 4;
	for(i=0; i<4; i++)
	{
		DrawLineX2(0, oy, 240, 0x01);
		oy += 4;
	}
	//Y
	DrawLineY(16, 2, 14, 0xAA);
	DrawLineY(128,2, 14, 0xaa);
	
}


/*********************************************************
Name: void LCDDisplayImbalanceU(void)
Function: 电压序分量显示
Input:  None
Output: None
**********************************************************/
void LCDDisplayImbalanceU(void)
{
	u32 tempint1;                                           // 临时整型变量
    u16 tempint2,*midp,*p;                                           // 临时除数
    u32 templong;                                           // 临时长整型变量
    u8  PointPosition, unit; 
    u8 oy, i;  
	//找出最大的值
	p = &MeterQuality.VoltageVU[0];
	midp = p;
    tempint1 = *midp++;
    if ((*midp) > tempint1) 
    {
        tempint1 = *midp;
    }
    midp++;
    
    if ((*midp) > tempint1) 
    {
        tempint1 = *midp;
    }
    
    templong = (u32)((float)tempint1 * PtResult / 10000);       // 乘以变比后再判断
    if (templong < 1) 														 
    {
        tempint2 = 1;
        PointPosition = 1;                             // XXX.X 小数点位置1
        unit = 0;
    }
    else if (templong < 9) 
    {
        tempint2 = 10;
        PointPosition = 3;                             // X.XXX kV
        unit = 1;
    }
    else if (templong < 90) 
    {
        tempint2 = 100;
        PointPosition = 2;                             // XX.XX 小数点位置2
        unit = 1;                   // "K"
    }
    else if (templong < 900) 
    {
        tempint2 = 1000;
        PointPosition = 1;                             // XXX.X 小数点位置1
        unit = 1;                   // "K"
    }
    else
    {
        tempint2 = 10000;
        PointPosition = 3;                             // X.XXX 无小数点
        unit = 2;                 // "M"
    }
    // 
    tempint1 = (u16)((float)(*p++) * PtResult / tempint2);
    Printf1624Num4(104, 2, tempint1, PointPosition);
    //
    tempint1 = (u16)((float)(*p++) * PtResult / tempint2);
    Printf1624Num4(104, 5, tempint1, PointPosition);
    // 
    tempint1 = (u16)((float)(*p++) * PtResult / tempint2);
    Printf1624Num4(104, 8, tempint1, PointPosition);	

	oy = 2;
	if(2==unit)
	{
		for(i=0; i<3; i++)
		{
			Printf81624(188, oy, "MV");
			oy += 3;
		}
	}
	else if(1==unit)
	{
		for(i=0; i<3; i++)
		{
			Printf81624(188, oy, "kV");
			oy += 3;
		}
	}
	else
	{
		for(i=0; i<3; i++)
		{
			Printf81624(188, oy, " V");
			oy += 3;
		}
	}		
	
}



/*********************************************************
Name: void LCDDisplayImbalanceI(void)
Function: 电流序分量显示
Input: None
Output: None
**********************************************************/
void LCDDisplayImbalanceI(void)
{
	u32 tempint1;
	u16 *midp, *p;                                           // 临时整型变量
    u32 tempint2;                                           // 临时除数
    u32 templong;                                           // 临时长整型变量
    u8  PointPosition, unit; 
    u8 oy, i;  
	//找出最大的值
	midp = &MeterQuality.CurrentIU[0];
	p = midp;
    tempint1 = (u32)(*midp++);
    if ((*midp) > tempint1) 
    {
        tempint1 = *midp;
    }
    midp++;
    
    if ((*midp) > tempint1) 
    {
        tempint1 = *midp;
    }
    
    templong = (u32)((float)tempint1 * CtResult / 10000);       // 乘以变比后再判断
    if (templong < 1) 
    {
        tempint2 = 1;
        PointPosition = 3;                             // X.XXX 小数点位置1
        unit = 0;
    }
    else if (templong < 9) 
    {
        tempint2 = 10;
        PointPosition = 2;                             // XX.XX 无小数点
        unit = 0;
    }
    else if (templong < 90) 
    {
        tempint2 = 100;
        PointPosition = 1;                             // XXX.X 小数点位置2
        unit = 0;                   // 
    }
    else if (templong < 900) 
    {
        tempint2 = 1000;
        PointPosition = 3;                             // X.XXX 小数点位置1
        unit = 1;                   // 
    }
    else if (templong < 9000) 
    {
        tempint2 = 10000;
        PointPosition = 2;                             // XX.XX 无小数点
        unit = 1;                 // "K"
    }
    else if (templong < 90000) 
    {
        tempint2 = 100000;
        PointPosition = 1;                             // XXX.X 
        unit = 1; 
    }
    else if (templong < 900000) 
    {
        tempint2 = 1000000;
        PointPosition = 3;                             // X.XXX M 
        unit = 2; 
    }
    else
    {
    	tempint2 = 10000000;
        PointPosition = 2;                             // XX.XX M无小数点
        unit = 2; 
    }	
    
    // 
    tempint1 = (u16)((float)(*p++) * CtResult / tempint2);
    Printf1624Num4(104, 2, tempint1, PointPosition);
    // 
    tempint1 = (u16)((float)(*p++) * CtResult / tempint2);
    Printf1624Num4(104, 5, tempint1, PointPosition);    
    //
    tempint1 = (u16)((float)(*p++) * CtResult / tempint2);
    Printf1624Num4(104, 8, tempint1, PointPosition);	
    
    //显示单位 A/KA
	oy = 2;
	if(2==unit)
	{
		for(i=0; i<3; i++)
		{
			Printf81624(188, oy, "MA");
			oy += 3;
		}
	}
	else if(1==unit)
	{
		for(i=0; i<3; i++)
		{
			Printf81624(188, oy, "kA");
			oy += 3;
		}
		
	}
	else
	{
		for(i=0; i<3; i++)
		{
			Printf81624(188, oy, " A");
			oy += 3;
		}
	}		

	
}


/**************************************************************************
Name: void LCDDisplayImbalance(void)
Function: 电压电流序分量显示
Input:  None
Output: None
**************************************************************************/
void LCDDisplayImbalance(void)
{  
	u8 oy,i,k;
	//显示标题
	k= DisplayItem - DISPLAY_QUAUIM;
	if(k>1) k=0;
	if(1==MeterSetData.c_lcdlanguage) //英文菜单
	{	
		oy = 3;
		for(i=0; i<4; i++)
		{
			Printf816(0x00, oy, QUA3TITLEE[k][i], 0);
			oy += 3;
		}
	}	
	else //菜单
	{
		oy = 3;
		for(i=0; i<4; i++)
		{
			Printf1616(0x00, oy, QUA3TITLEC[k][i], 0);
			oy += 3;
		}
		
	}	
	//显示数据
	if(DISPLAY_QUAUIM == DisplayItem)  //电压序分量显示
	{
		LCDDisplayImbalanceU();
		Printf1624Num4(104, 11, MeterQuality.VoltageVU[3], 1);
	}
	else //电流序分量显示
	{
		LCDDisplayImbalanceI();
		Printf1624Num4(104, 11, MeterQuality.CurrentIU[3], 1);
	}	
	//显示%
	Printf81624(196, 11, "%");
	//X2
	oy = 5;
	for(i=0; i<4; i++)
	{
		DrawLineX2(0, oy, 240, 0x01);
		oy += 3;
	}
	//Y
	DrawLineY(88, 2, 12, 0xAA);
}


//电压波峰系数和电流K系数
void LCDDisplayUpeakIk(void)
{
	u8 oy,i;
	u16 *midp16;
	oy = 3;
	for(i=0; i<3; i++)
	{
		Printf816(0x00, oy, QUA4TITLEE[DisplayItem-DISPLAY_QUAUPEAK][i], 0);
		oy += 3;
	}
	
	if(DISPLAY_QUAUPEAK==DisplayItem) midp16 = &MeterQuality.UCF[0];
	else midp16 = &MeterQuality.IKFactor[0];
		 
	oy=2;
	for(i=0; i<3; i++)
	{
		Printf1624Num4(104, oy, *midp16++, 3);
		oy += 3;
	}
		 
	//X2
	oy = 5;
	for(i=0; i<3; i++)
	{
		DrawLineX2(0, oy, 240, 0x01);
		oy += 3;
	}
	//Y
	DrawLineY(88, 2, 9, 0xAA);
}

//电能质量
const u8 QUA1TITLEC[2][8] =
{
	{10,48,17,22,0}, //电压波形
	{10,50,17,22,0}, //电流波形

};
const u8 QUA1TITLEE[2][13]=
{
	"Waveform(V)",
	"Waveform(I)",
};



/***********************************************
Name: void LCDDisplayWave(void)
Function: 电能质量参数显示，包括：电压电流波形
************************************************/
void LCDDisplayWave(void)
{
	TopShadow(); //顶部阴影显示
	
	if(0==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_UP,K_DOWN,K_MENU,K_NO); 	//四个按键
	else PrintfKeyF4(K_UP,K_DOWN,K_MENUE,K_NO); 	//四个按键
	if(1==MeterSetData.c_lcdlanguage) //英文显示
	{
		Printf816(1,0,&QUA1TITLEE[DisplayItem][0],1);	
	}
	else  //中文显示
	{
		Printf1616(1,0,&QUA1TITLEC[DisplayItem][0],1);
	}	
	//显示页数4.x
	Printf816(208,0,"4.",1);	
	Print816Num(220,0,(DisplayItem+1),1);
	LCDDisplayUIWave();
}

/****************************************************************************
Name: void LCDDisplayUIWave()
Function: 电压电流实时波形显示,波形数据-50~50，应根据量程进行采样数据的移位处理
Input:  None
Output: None
****************************************************************************/
u8 const TABLINE[8]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};


void LCDDisplayUIWave(void)
{
	u8 ox, oy;
	u8 i,j,k,m,n;
	u8 wavebuf[3][64];
	u8 h0,h1;
	if(DISPLAY_QUAUWAVE == DisplayItem) //电压波形
	{
		//根据量程进行
		if(MeterSetData.c_pt2>260) m=8;
		else if(MeterSetData.c_pt2>150) m=7;
		else m=6;
		for(i=0; i<64; i++)
		{
			wavebuf[0][i] = (u8)((UIWaveData[0][i<<1]>>m)+80);
			wavebuf[1][i] = (u8)((UIWaveData[1][i<<1]>>m)+80);
			wavebuf[2][i] = (u8)((UIWaveData[2][i<<1]>>m)+80);
		}
	}	
	else //电流波形
	{
		if(MeterSetData.c_ct2>=3) m=8;
		else if(MeterSetData.c_ct2>=2) m=7;
		else m=6;
		for(i=0; i<64; i++)
		{
			wavebuf[0][i] = (u8)((UIWaveData[3][i<<1]>>m)+80);
			wavebuf[1][i] = (u8)((UIWaveData[4][i<<1]>>m)+80);
			wavebuf[2][i] = (u8)((UIWaveData[5][i<<1]>>m)+80);
		}
	}	
	//清除波形显示去
	LCDClrWave();

	n=3;
	if(INPUT33 == MeterSetData.c_input[0])
	{
		if((DISPLAY_QUAIWAVE == DisplayItem)&&(1==IBInput)) m=3;
		else m=2;	
	}
	else if(INPUT11 == MeterSetData.c_input[0]) m=1;
	else m=3;
	
	for(i=0; i<n; i++)
	{
		//标准abc符号
		oy = MaxInt8_p(wavebuf[i],63);
		oy += 8; 
		oy = oy>>3;
		ox = OxAddr + 88;
		if(0 == i) 
		{
			if(INPUT33 == MeterSetData.c_input[0]) Printf86(ox, oy, "1",0);
			else Printf86(ox,oy, "1",0);	
		}
		if(1==i) Printf86(ox, oy, "2",0);
		if(2==i)
		{
			if(INPUT33 == MeterSetData.c_input[0]) Printf86(ox, oy, "3",0);
			else Printf86(ox,oy, "3",0);	
		}
		if(2==m) i++;	//如果是三相三线，跳过不需要的B相
		else if(1==m) i+=2;
	}
	//波形显示
	for(i=0; i<n; i++)
	{
		//第一个周波
		h0=wavebuf[i][0];
		for(j=1; j<64; j++)
		{
			h1 = wavebuf[i][j];
			if((h1<24)||(h1>132)) continue;
			ox=j+24;
			
			if(h1>h0)
			{
				for(k=0; k<(h1-h0);k++)	
				{
					DISP_RAM[(h0+k)>>3][ox] |= TABLINE[(h0+k)%8];	
				}
			}
			else if(h1<h0)
			{
				for(k=0;k<(h0-h1);k++)
				{
					DISP_RAM[(h1+k)>>3][ox] |= TABLINE[(h1+k)%8];		
				}	
			}	
			else
			{
				DISP_RAM[(h0)>>3][ox] |= TABLINE[(h0)%8];
			}	
			
			h0=h1;
		}
		//第二个周波
		for(j=0; j<64; j++)
		{
			h1 = wavebuf[i][j];
			if((h1<24)||(h1>132)) continue;
			ox=j+88;
			
			if(h1>h0)
			{
				for(k=0; k<(h1-h0);k++)	
				{
					DISP_RAM[(h0+k)>>3][ox] |= TABLINE[(h0+k)%8];	
				}
			}
			else if(h1<h0)
			{
				for(k=0;k<(h0-h1);k++)
				{
					DISP_RAM[(h1+k)>>3][ox] |= TABLINE[(h1+k)%8];		
				}	
			}	
			else
			{
				DISP_RAM[(h0)>>3][ox] |= TABLINE[(h0)%8];
			}	
			h0=h1;
		}
		
		//第三个周波
		for(j=0; j<64; j++)
		{
			h1 = wavebuf[i][j];
			if((h1<24)||(h1>132)) continue;
			ox=j+152;
			
			if(h1>h0)
			{
				for(k=0; k<(h1-h0);k++)	
				{
					DISP_RAM[(h0+k)>>3][ox] |= TABLINE[(h0+k)%8];	
				}
			}
			else if(h1<h0)
			{
				for(k=0;k<(h0-h1);k++)
				{
					DISP_RAM[(h1+k)>>3][ox] |= TABLINE[(h1+k)%8];		
				}	
			}	
			else
			{
				DISP_RAM[(h0)>>3][ox] |= TABLINE[(h0)%8];
			}	
			h0=h1;
		}	
			
		if(2==m) i++;	//如果是三相三线，跳过不需要的B相
		else if(1==m) i+=2;
	}	

	//横线和竖线标度
	DrawLineX(20,10,218,0x01);
	DrawLineY(24,3,14,0xff);
	
		
}


u8 const LCDDEMANDTITLE1C[3][7]=
{
	{204,205,25,9,0},	//最大需量
	{206,207,64,190,25,9,0}, 	//当前周期需量
	{197,64,190,25,9,0},	//上周期需量
};
u8 const LCDDEMANDTITLE1E[3][23]=
{
	{"Max Demand"},	{"Present Period Demand"},{"Previous Period Demand"}
};

/******************************************************************************
Name: void LCDDisplayDemand(void)
Function: 需量显示，包括电流功率和两个设置变量最大需量值和当前需量值，共6页显示
Input:  None
Output: None
******************************************************************************/
void LCDDisplayDemand(void)
{
	u8 m;
	TopShadow(); //顶部阴影显示
	
	if(DisplayItem<DISPLAY_DEMANDNOW1) m=0;
	else if(DisplayItem<DISPLAY_DEMANDPre1) m= 1;
	else m=2;
	if(1==MeterSetData.c_lcdlanguage) //英文 
	{
		Printf816(1,0,LCDDEMANDTITLE1E[m],1);
		PrintfKeyF4(K_UP,K_DOWN,K_MENUE,K_NO); 	//四个按键
		
	}	
	else  //中文显示 
	{
		Printf1616(1,0,LCDDEMANDTITLE1C[m],1);
		PrintfKeyF4(K_UP,K_DOWN,K_MENU,K_NO); 	//四个按键
	}
	
	//显示页数5.x
	Printf816(208,0,"5.",1);	
	Print816Num(220,0,(DisplayItem+1),1);
	switch(DisplayItem)
	{
		case DISPLAY_DEMANDMAX1: //电流需量
		case DISPLAY_DEMANDNOW1:	
		case DISPLAY_DEMANDPre1:
			LCDDisplaDemandI();
			break;
		case DISPLAY_DEMANDMAX2: //功率需量
		case DISPLAY_DEMANDNOW2:	
		case DISPLAY_DEMANDPre2:
			LCDDisplayDemandP();
			break;
		default:
			break;
			
	}
	
}


/*********************************************************
Name: void LCDDemandIData(u32 midRMS_I[3])
Function: 显示单相电流显示
Input: u32 midRMS_I, u8 oy
Output: None
**********************************************************/
void LCDDemandIData(s16 *midp16, u8 oy, u8 m)
{
	s32 tempint1;                                           // 临时整型变量
    u32 tempint2;                                           // 临时除数
    s32 templong;                                           // 临时长整型变量
    u8  PointPosition, unit,i; 
    s16 *midp;
    midp = midp16;
	//找出最大的值
	tempint1=*midp16;
	if(3==m)
	{
		midp++;
		if(*midp>tempint1) tempint1 = *midp;
		midp++;
		if(*midp>tempint1) tempint1 = *midp;
	}
	else if(2==m)
	{
		midp++;
		if(*midp>tempint1) tempint1 = *midp;
	}		

    templong = (u32)((float)tempint1 * CtResult / 10000);       // 乘以变比后再判断
    if (templong < 1) 
    {
        tempint2 = 1;
        PointPosition = 3;                             // X.XXX 小数点位置1
        unit = 0;
    }
    else if (templong < 9) 
    {
        tempint2 = 10;
        PointPosition = 2;                             // XX.XX 无小数点
        unit = 0;
    }
    else if (templong < 90) 
    {
        tempint2 = 100;
        PointPosition = 1;                             // XXX.X 小数点位置2
        unit = 0;                   // 
    }
    else if (templong < 900) 
    {
        tempint2 = 1000;
        PointPosition = 3;                             // X.XXX 小数点位置1
        unit = 1;                   // 
    }
    else if (templong < 9000) 
    {
        tempint2 = 10000;
        PointPosition = 2;                             // XX.XX 无小数点
        unit = 1;                 // "K"
    }
    else if (templong < 90000) 
    {
    	tempint2 = 100000;
        PointPosition = 1;                             // XXX.X 无小数点
        unit = 1; 
    }	
     else if (templong < 900000) 
    {
    	tempint2 = 1000000;
        PointPosition = 3;                             // X.XXX M无小数点
        unit = 2; 
    }
    else
    {
    	tempint2 = 10000000;
        PointPosition = 2;                             // XX.XX M无小数点
        unit = 2; 
    }
     	
	for(i=0; i<m; i++)
	{
		tempint1 = (u16)(((float)(*midp16++)) * CtResult / tempint2);
    	Printf1624Num4(56, oy, tempint1, PointPosition);	
    	if(2==unit) Printf81624(144, oy, "MA");	//显示单位 A/KA
    	else if(1==unit) Printf81624(144, oy, "kA");	//显示单位 A/KA
		else Printf81624(144, oy, " A");
    	oy += 3;
	}	
}

/************************************************************
Name: LCDDisplayDemandU(s16 midRMS_U, u8 oy)
Function: 在需量界面显示电压需量
************************************************************/
void LCDDisplayDemandU(s16 midRMS_U, u8 oy)
{
	s32 tempint1 ;                                           // 临时整型变量
    u16 tempint2;                                           // 临时除数
    s32 templong;                                           // 临时长整型变量
    u8  PointPosition, unit; 

	//找出最大的值
    tempint1 = midRMS_U;
    templong = (u32)((float)tempint1 * PtResult / 10000);       // 乘以变比后再判断
    if (templong < 1) 														 
    {
        tempint2 = 1;
        PointPosition = 1;                             // XXX.X 小数点位置1
        unit = 0;
    }
    else if (templong < 9) 
    {
        tempint2 = 10;
        PointPosition = 3;                             // X.XXX 无小数点
        unit = 1;
    }
    else if (templong < 90) 
    {
        tempint2 = 100;
        PointPosition = 2;                             // XX.XX 小数点位置2
        unit = 1;                   // "K"
    }
    else if (templong < 900) 
    {
        tempint2 = 1000;
        PointPosition = 1;                             // XXX.X 小数点位置1
        unit = 1;                   // "K"
    }
    else 
    {
        tempint2 = 10000;
        PointPosition = 3;                             // X.XXX 无小数点
        unit = 2;                 // "M"
    }
    // Ua/Uab
    tempint1 = (u16)((float)(midRMS_U) * PtResult / tempint2);
    Printf1624Num4(56, oy, tempint1, PointPosition);
    //显示单位 单位V
	if(1==unit) Printf81624(144, oy, "kV");
	else if(2==unit) Printf81624(144, oy, "MV");
	else Printf81624(144, oy, " V");
}

/***********************************************************

************************************************************/
void LCDDemandP(s16 *midp16,u8 oy, u8 m)
{
	s32 tempint1;                                      // 临时整型变量
    s32 templong2;                                     // 临时除数
    s32 templong;                                      // 临时长整型变量
    u8 PointPosition, unit, k,i;                                 // 小数点位置
    tempint1 = DemandData[2][5];   
    templong = (u32)((float)tempint1 * PtCtResult/ 10000);    // 乘以变比后再判断
    if (templong < 1) 
    {
        templong2 = 1;                                    
        PointPosition = 3;   // X.XXX无小数点
        unit = 1;            //无单位
    }
    else if (templong < 9) 
    {
        templong2 = 10;
        PointPosition = 2;                             // XX.XX 小数点位置2
        unit = 1;                   // "K"
    }
    else if (templong < 90) 
    {
        templong2 = 100;
        PointPosition = 1;                             // XXX.X 小数点位置1
        unit = 1;                   // "K"
    }
    else if (templong < 900) 
    {
        templong2 = 1000;
        PointPosition = 3;                             // X.XXX 小数点位置0
        unit = 2;                   // "M"
    }
    else if (templong < 9000) 
    {
        templong2 = 10000;
        PointPosition = 2;                             // XX.XX 小数点位置2
        unit = 2;                   // "M"
    }
    else if (templong < 90000) 
    {
        templong2 = 100000;
        PointPosition = 1;                             // XXX.X 小数点位置1
        unit = 2;                   // "M"
    }
    else if (templong < 900000) 
    {
        templong2 = 1000000;
        PointPosition = 3;                             // X.XXX小数点位置0
        unit = 3;                   // "G"
    }
    else if (templong < 9000000) 
    {
        templong2 = 10000000;
        PointPosition = 2;                             // XX.XX 小数点位置2
        unit = 3;                   // "G"
    }
    else
    {
        templong2 = 100000000;
        PointPosition = 1;                             // XXX.X 小数点位置1
        unit = 3;                   // "G"
    }

	for(i=0; i<m; i++)
	{
		tempint1 = (u16)(((float)abss16(*midp16)) * PtCtResult / templong2); // P
		Printf1624Num4(56, oy, tempint1, PointPosition);   
		if((*midp16) < 0)  k=1;   
		else k=0;
		midp16++;
		Printf1616(40, oy,POSTNEG[k],0); //符号显示
		Printf81624Unit(148, oy, unit); //单位显示 
		oy += 3;
	}
 
}


/***********************************************************
Name: void LCDDisplaDemandI(void)
Function:  
Input: None
Output: None
***********************************************************/
void LCDDisplaDemandI(void)
{
	s16 *midp16;
	u8 i,oy, LNum=3;
	if(DISPLAY_DEMANDMAX1==DisplayItem) midp16 = &DemandData[2][0]; //最大值电流需量
	else  if(DISPLAY_DEMANDNOW1==DisplayItem) midp16 = &DemandData[0][0]; //当前电流需量
	else midp16 = &DemandData[1][0]; //上一个周期电流需量
	if(INPUT11 == MeterSetData.c_input[0]) //单相
	{
		Printf81624(8, 7, ELITITLE[0]); //Ia
		LCDDemandIData(midp16, 7,1);
	}
	else //三相三线/三相四线
	{
		//显示字符 
		oy=4;
		for(i=0; i<3; i++)
		{
			Printf81624(8, oy, ELITITLE[i]); //Ia Ib Ic
			oy += 3;	
		}
		//显示数据和单位
		LCDDemandIData(midp16,4,LNum); 
	}		
}

/***********************************************************
Name: void LCDDisplayDemandP(void)
Function: 功率需量显示，区分最大值和当前值
Input:  None
Output: None
***********************************************************/
void LCDDisplayDemandP(void)
{
	s16 *midp16;
	u8 i,oy;
	if(DISPLAY_DEMANDMAX2==DisplayItem) midp16 = &DemandData[2][3]; //功率最大需量
	else if(DISPLAY_DEMANDNOW2==DisplayItem) midp16 = &DemandData[0][3]; //功率当前需量
	else midp16 = &DemandData[1][3]; //功率上一个周期需量
	
	//显示字符 
	oy=4;
	for(i=0; i<3; i++)
	{
		Printf81624(8, oy, ELPQSTITLE[i]); //P Q S
		Printf81624(160, oy, ELPQSUNIT[i]); //单位W var VA
		oy += 3;
	}
	LCDDemandP(midp16,4,3); //显示功率数据
}


u8 const LCDMAXTITLE1C[3][4]=
{
	{204,205,27,0},	//最大值
	{204,208,27,0},	//最小值
	{97,223,27,0}, //平均值
};
u8 const LCDMAXTITLE1E[3][14]=
{
	"Max Value","Min Value","Average Value",
};
/*************************************************************
Name: void LCDDisplayMax(void)
Function: 最大值最小值显示，显示Un/Ul/F/I/P/Q/S
**************************************************************/
void LCDDisplayMax(void)
{
	u8 m;
	TopShadow(); //顶部阴影显示
	if(INPUT33 == MeterSetData.c_input[0])
	{
		if(DisplayItem<DISPLAY_MIN33UL) m=0; //最大值
		else if(DisplayItem<DISPLAY_AVG33UI) m=1; //最小值
		else m=2;  //平均值
	}
	else if(INPUT11 == MeterSetData.c_input[0])
	{
		if(DisplayItem<DISPLAY_MIN11UI) m=0; //最大值
		else m=1; //最小值
	}
	else
	{
		if(DisplayItem<DISPLAY_MIN34UN) m=0; //最大值
		else if(DisplayItem<DISPLAY_AVG34UI) m=1; //最小值	
		else m=2; //平均值
	}			
	if(1==MeterSetData.c_lcdlanguage) //英文显示
	{
		Printf816(1,0,LCDMAXTITLE1E[m],1);
		PrintfKeyF4(K_UP,K_DOWN,K_MENUE,K_NO); 	//四个按键
		
	}
	else  //中文显示
	{
		Printf1616(1,0,LCDMAXTITLE1C[m],1);
		PrintfKeyF4(K_UP,K_DOWN,K_MENU,K_NO); 	//四个按键
	}	
		
	//显示页数6.x
	Printf816(208,0,"6.",1);	
	if(INPUT33 == MeterSetData.c_input[0]) Print816Num(220,0,(DisplayItem-0x0f),1);	
	else if(INPUT11==MeterSetData.c_input[0]) Print816Num(220,0,(DisplayItem-0x1f),1);			
	else 
	{
		if(DisplayItem<9) Print816Num(220,0,(DisplayItem+1),1);	
		else Printf816(220,0,"10",1);
	}
	
	switch(DisplayItem)
	{
		case DISPLAY_MAX34UN:
		case DISPLAY_MIN34UN:
			LCDMaxMin34UN();
			break;
		case DISPLAY_MAX34UL:
		case DISPLAY_MAX33UL:
		case DISPLAY_MIN34UL:
		case DISPLAY_MIN33UL:
			LCDMaxMin34UL();
			break;
		case DISPLAY_MAX34I:
		case DISPLAY_MAX33I:
		case DISPLAY_MIN34I:
		case DISPLAY_MIN33I:	
			LCDMaxMin34I();
			break;
		case DISPLAY_MAX34PQS:
		case DISPLAY_MIN34PQS:
		case DISPLAY_MAX33PQS:
		case DISPLAY_MIN33PQS:
		case DISPLAY_MAX11PQS:
		case DISPLAY_MIN11PQS:
			LCDMaxMinPQS();
			break;
		case DISPLAY_MAX11UI:
		case DISPLAY_MIN11UI:
			LCDMaxMinUI();
			break;
		case DISPLAY_AVG34UI:
		case DISPLAY_AVG33UI:
			LCDAvgUI();	
			break;
		case DISPLAY_AVG34PQS:
		case DISPLAY_AVG33PQS:
			LCDAvgPQS();
			break;	
		
		default:
			break;		
	}
}

//极值三相线电压显示
void LCDMaxUData(u16 *p, u8 LNum)
{
	u16 tempint1, *midp;           // 临时整型变量
    u16 tempint2;                  // 临时除数
    s32 templong;                  // 临时长整型变量
    u8  PointPosition, unit; 
    u8 oy, i;  
	//找出最大的值
	midp = p;
	tempint1 = *midp++;
	if ((*midp) > tempint1) 
	{
	    tempint1 = *midp;
	}
	midp++;
	if ((*midp) > tempint1) 
	{
	    tempint1 = *midp;
	}		

    templong = (u32)((float)tempint1 * PtResult / 10000);       // 乘以变比后再判断
    if (templong < 1) 
    {
        tempint2 = 1;
        PointPosition = 1;                             // XXX.X 小数点位置1
        unit = 0;
    }
    else if (templong < 9) 
    {
        tempint2 = 10;
        PointPosition = 3;                             // X.XXX 无小数点
        unit = 1;
    }
    else if (templong < 90) 
    {
        tempint2 = 100;
        PointPosition = 2;                             // XX.XX 小数点位置2
        unit = 1;                   // "K"
    }
    else if (templong < 900) 
    {
        tempint2 = 1000;
        PointPosition = 1;                             // XXX.X 小数点位置1
        unit = 1;                   // "K"
    }
    else
    {
        tempint2 = 10000;
        PointPosition = 3;                             // X.XXX 无小数点
        unit = 2;                 // "M"
    }
    // Ua/Uab
    tempint1 = (u16)((float)(*p++) * PtResult / tempint2);
    Printf1624Num4(56, 4, tempint1, PointPosition);
    // Ub/Ubc
    tempint1 = (u16)((float)(*p++) * PtResult / tempint2);
    Printf1624Num4(56, 7, tempint1, PointPosition);
    
    // UC/Uca
	tempint1 = (u16)((float)(*p++) * PtResult / tempint2);
	Printf1624Num4(56, 10, tempint1, PointPosition);	
	
    //显示单位 单位V
	oy = 4;
	if(2 == unit)
	{
		for(i=0; i<LNum; i++)
		{
			Printf81624(144, oy, "MV");
			oy += 3;
		}
	}	
	else if(1==unit)
	{
		for(i=0; i<LNum; i++)
		{
			Printf81624(144, oy, "kV");
			oy += 3;
		}
	}
	else
	{
		for(i=0; i<LNum; i++)
		{
			Printf81624(144, oy, " V");
			oy += 3;
		}
	}			
}

//功率因数极值显示
void LCDMaxPFData(u8 oy, s16 midPF)
{
	u16 tempint1;
	u8 k;
	Printf81624(7,oy,"PF");
	tempint1 = (abss16(midPF));
	Printf1624Num4(56, oy, tempint1, 3);
	if((midPF) < 0)  k=1;
	else k=0;
	Printf1616(40, oy ,POSTNEG[k],0); //符号显示	
}

//频率极值显示
void LCDMaxFData(u8 oy, s16 midF)
{
	u16 tempint1;
	Printf81624(8,oy,"F");
	Printf81624(152,(oy), "Hz");
	tempint1 = midF;
	Printf1624Num4(56, oy, tempint1, 2);
}
//最大最小相电压值 
void LCDMaxMin34UN(void)
{
	u8 i,oy, LNum=3;
	//显示字符 
	oy=4;
	for(i=0; i<LNum; i++)
	{
		Printf81624(8, oy, (u8 *)ELUNTITLE[i]); //V1 V2 V3
		oy += 3;
	}
	if(DISPLAY_MAX34UN==DisplayItem) LCDMaxUData(&MeterMax.UnMax[0],LNum); 	
	else LCDMaxUData(&MeterMax.UnMin[0],LNum); 	
	
}
//极值三相线电压显示
void LCDMaxMin34UL(void)
{
	u8 i,oy;
	oy=4;
	for(i=0; i<3; i++)
	{
		Printf81624(8, oy, (u8 *)ELULTITLE[i]); //V1 V2 V3
		oy += 3;
	}
	if((DISPLAY_MAX34UL==DisplayItem)||(DISPLAY_MAX33UL==DisplayItem))
	{
		LCDMaxUData(&MeterMax.UlMax[0],3);
		LCDMaxFData(13, MeterMax.FMax);
	}
	else
	{
		LCDMaxUData(&MeterMax.UlMin [0],3);
		LCDMaxFData(13,MeterMax.FMin);
	} 
}

//三相电流极值
void LCDMaxMin34I(void)
{
	u8 i, oy, LNum=3;
	oy=4;
	for(i=0; i<LNum; i++)
	{
		Printf81624(8, oy, ELITITLE[i]); //I1 I2 I3
		oy += 3;
	}
	//显示数据和单位
	if((DISPLAY_MAX34I==DisplayItem)||(DISPLAY_MAX33I==DisplayItem))
	{
		LCDDemandIData((s16 *)&MeterMax.IMax[0],4,LNum); 	
	}
	else LCDDemandIData((s16 *)&MeterMax.IMin[0],4,LNum);  
}

const u8 PQSUNIT[3][4][5]=
{
	{" W","kW","MW","GW"},
	{" var","kvar","Mvar","Gvar"},
	{" VA","kVA","MVA","GVA"},	
};

void LCDMaxPQSAllData(s16 *midp16)
{
	s32 tempint1;                                      // 临时整型变量
    s32 templong2;                                     // 临时除数
    s32 templong;                                      // 临时长整型变量
    u8 PointPosition, unit, k,i;                                 // 小数点位置
    u8 oy, m;
    
    tempint1 = MeterMax.SMax;   
    templong = (u32)((float)tempint1 * PtCtResult/ 10000);    // 乘以变比后再判断
    if (templong < 1) 
    {
        templong2 = 1;                                    
        PointPosition = 3;   // X.XXX无小数点
        unit = 1;            //"K"
    }
    else if (templong < 9) 
    {
        templong2 = 10;
        PointPosition = 2;                             // XX.XX 小数点位置2
        unit = 1;                   // "K"
    }
    else if (templong < 90) 
    {
        templong2 = 100;
        PointPosition = 1;                             // XXX.X 小数点位置1
        unit = 1;                   // "K"
    }
    else if (templong < 900) 
    {
        templong2 = 1000;
        PointPosition = 3;                             // X.XXX 小数点位置0
        unit = 2;                   // "M"
    }
    else if (templong < 9000) 
    {
        templong2 = 10000;
        PointPosition = 2;                             // XX.XX 小数点位置2
        unit = 2;                   // "M"
    }
    else if (templong < 90000) 
    {
        templong2 = 100000;
        PointPosition = 1;                             // XXX.X 小数点位置1
        unit = 2;                   // "M"
    }
    else if (templong < 900000) 
    {
        templong2 = 1000000;
        PointPosition = 3;                             // X.XXX小数点位置0
        unit = 3;                   // "G"
    }
    else if (templong < 9000000) 
    {
        templong2 = 10000000;
        PointPosition = 2;                             // XX.XX 小数点位置2
        unit = 3;                   // "G"
    }
    else
    {
        templong2 = 100000000;
        PointPosition = 1;                             // XXX.X 小数点位置1
        unit = 3;                   // "G"
    }
	oy = 4;
   	m = 3;
	for(i=0; i<m; i++)
	{
		tempint1 = (u16)((float)(abss16(*midp16)) * PtCtResult / templong2); // P
		Printf1624Num4(56, oy, tempint1, PointPosition);   
		if((*midp16) < 0)  k=1;   
		else k=0;
		midp16+=1;
		Printf1616(40, oy,POSTNEG[k],0); //符号显示
		Printf81624(148, oy, PQSUNIT[i][unit]); //单位显示 
		oy += 3;
	}
 
}

//单相和三相三相功率最大最小值
void LCDMaxMinPQS(void)
{
	u8 i,oy;
	//显示字符 
	oy=4;
	for(i=0; i<3; i++)
	{
		Printf81624(8, oy, ELPQSTITLE[i]); //P Q S
		oy += 3;
	}
	if((DISPLAY_MAX34PQS==DisplayItem)||(DISPLAY_MAX33PQS==DisplayItem)||(DISPLAY_MAX11PQS==DisplayItem))
	{
		LCDMaxPQSAllData(&MeterMax.PMax[3]); //
		LCDMaxPFData(13, MeterMax.PFMax);
	}
	else
	{
		LCDMaxPQSAllData(&MeterMax.PMin[3]); //显示功率数据	 
		LCDMaxPFData(13, MeterMax.PFMin);
	} 
	
}

//单相电压电流最大最小值
void LCDMaxMinUI(void)
{
	Printf81624(8, 4, "U"); //U
	Printf81624(8, 7, "I"); //I

	if(DISPLAY_MAX11UI == DisplayItem)
	{
		LCDDisplayDemandU(MeterMax.UnMax[0], 4);
		LCDDemandIData((s16 *)&MeterMax.IMax[0],7,1);
		LCDMaxFData(10, MeterMax.FMax);
	}
	else
	{
		LCDDisplayDemandU(MeterMax.UnMin[0], 4);
		LCDDemandIData((s16 *)&MeterMax.IMin[0],7,1);
		LCDMaxFData(10, MeterMax.FMin);	
	}		
}

//平均电压电流值
void LCDAvgUI(void)
{
	if(INPUT33==MeterSetData.c_input[0])
	{
		Printf81624(8, 4, "U"); //U
		Printf81624(8, 7, "I"); //I
		
		LCDDisplayDemandU(MeterQuality.Ulavg, 4);
		LCDDemandIData((s16 *)&MeterQuality.Iavg,7,1);
		
		
	}
	else
	{
		Printf81624(8, 4, "V"); //V
		Printf81624(8, 7, "U"); //U
		Printf81624(8, 10, "I"); //I
		
		LCDDisplayDemandU(MeterQuality.Unavg, 4);
		LCDDisplayDemandU(MeterQuality.Ulavg, 7);
		LCDDemandIData((s16 *)&MeterQuality.Iavg,10,1);
	}		
}

//平均功率值
void LCDAvgPQS(void)
{
	Printf81624(8, 4, "P"); //Un
	Printf81624(8, 7, "Q"); //Ul
	Printf81624(8, 10, "S"); //I
	
	LCDMaxPQSAllData(&MeterQuality.Pavg); //显示功率数据	 
}

u8 const LCDMODULETITLE1E[5][15]=
{
	"Digital Inputs",
	"Relay Outputs",
	"System Time",
	"System State",
	"Version",

};
u8 const LCDMODULETITLE1C[5][6] =
{
	{109,110,9,107,108,0}, //开关量输入
	{183,10,184,107,111,0}, //继电器输出
	{38,39,7,194,0}, //系统时间
	{38,39,181,182,0}, //系统状态
	{65,33,217,218,0},	//软件版本	
};

u8 const LCDMODULETITLE2C[1][6]=
{53,28,29,30,31,0}; //无扩展模块 
u8 const LCDMODULETITLE2E[1][11]=
{ "No Module "};
void LCDDisplayModule(void)
{
	TopShadow(); //顶部阴影显示
	//标题部分
	if(1==MeterSetData.c_lcdlanguage) //英文
	{
		Printf816(1,0,LCDMODULETITLE1E[DisplayItem],1);
		PrintfKeyF4(K_UP,K_DOWN,K_MENUE,K_NO); 	//四个按键
	}
	else //中文
	{
		Printf1616(1,0,LCDMODULETITLE1C[DisplayItem],1);
		PrintfKeyF4(K_UP,K_DOWN,K_MENU,K_NO); 	//四个按键
	}		
	Printf816(208, 0, "7.",1);
	Print816Num(220, 0, (u8)(DisplayItem+1),1);
	switch(DisplayItem)
	{
		case MODULEDI:
			if(0==MeterSetData.c_set[DISET]) //有开关量模块
			{
				if(1==MeterSetData.c_lcdlanguage) Printf816(32, 8,LCDMODULETITLE2E[0],0);
				else Printf1616(32, 8, LCDMODULETITLE2C[0], 0);
			}
			else
			{
				LCDDisplayDI();
			}	
			break;
		case MODULEDO:
			if(0==MeterSetData.c_set[DOSET]) //有继电器输出模块
			{
				if(1==MeterSetData.c_lcdlanguage) Printf816(32, 8,LCDMODULETITLE2E[0],0);
				else Printf1616(32, 8, LCDMODULETITLE2C[0], 0);
			}
			else
			{
				LCDDisplayDO();
			}
			break;
		case MODULERTC:
			if(0==MeterSetData.c_set[TOUSET]) //有继电器输出模块
			{
				if(1==MeterSetData.c_lcdlanguage) Printf816(32, 8,LCDMODULETITLE2E[0],0);
				else Printf1616(32, 8, LCDMODULETITLE2C[0], 0);
			}
			else
			{
				LCDDisplayRTC();
			}
			break;
		case MODULESYS:
			LCDDisplaySysState();
			break;
		case MODULEREV:
			LCDDisplayRev();
			break;	
	
		default:
			break;
	}	
}

u8 const LCDDOXTITLE1C[1][3]= { 112,172,0}; //通道
u8 const LCDDOXTITLE3C[1][3]= {181,182,0};	//状态
u8 const NOTITLE[6][3]={"01","02","03","04","05","06"};

void LCDDisplayDI(void)
{
	u8 i, oy, n;
	if(1==MeterSetData.c_lcdlanguage) //英文
	{
		Printf816(8,2," No. ",0);
		Printf816(112,2,"Status",0);	
	}
	else
	{
		Printf1616(8,2,LCDDOXTITLE1C[0],0); //通道
		Printf1616(112,2,LCDDOXTITLE3C[0],0); //状态
	}
	oy = 4;
	if(1==MeterSetData.c_set[DISET]) n=2;
	else if(3==MeterSetData.c_set[DISET]) n=6;
	else n=4;	
	for(i=0; i<n; i++)
	{
		Printf816(16,oy,NOTITLE[i],0);
		PrintfBMP3216(112,oy,((MeterDio.i_Di>>i)&0x01)); //状态显示			
		oy += 2;
	}

	//画线
	//X2
	oy = 2;
	for(i=0; i<(n+2); i++)
	{
		DrawLineX(0, oy, 239, 0x01);
		oy += 2;
	}
	//Y
	DrawLineY(48, 2, (2+n*2), 0xff);
}

//继电器输出状态显示
void LCDDisplayDO(void)
{
	u8 i, oy, n;
	if(1==MeterSetData.c_lcdlanguage) //英文
	{
		Printf816(8,2," No. ",0);
		Printf816(112,2,"Status",0);	
	}
	else
	{
		Printf1616(8,2,LCDDOXTITLE1C[0],0); //通道
		Printf1616(112,2,LCDDOXTITLE3C[0],0); //状态
	}
	oy = 4;
	n=3;	
	for(i=0; i<n; i++)
	{
		Printf816(16,oy,NOTITLE[i],0);
		PrintfBMP3216(112,oy,((MeterDio.i_Do>>i)&0x01)); //状态显示			
		oy += 2;
	}

	//画线
	//X2
	oy = 2;
	for(i=0; i<(n+2); i++)
	{
		DrawLineX(0, oy, 239, 0x01);
		oy += 2;
	}
	//Y
	DrawLineY(48, 2, (2+n*2), 0xff);
}

u8 const WEEKDAYC[7][4] =
{
	{189,190,191,0},{189,190,160,0},{189,190,161,0},{189,190,162,0}, //星期天-三
	{189,190,163,0},{189,190,192,0},{189,190,193,0}
}; 
u8 const WEEKDAYE[7][10] =
{
	" Sunday  "," Monday  "," Tuesday ", 
	"Wednesday","Thursday "," Friday  ","Saturday ",
};
//时间显示
void LCDDisplayRTC(void)
{
	u8 i, ox;
	if(1==MeterSetData.c_lcdlanguage)
	{
		Printf816(80,12,WEEKDAYE[sd_Data.c_time[6]],0);
	} 
	else
	{
		Printf1616(88,12,WEEKDAYC[sd_Data.c_time[6]],0);
	}	
	//显示时间
	ox = 80;
	for(i=0; i<3; i++)
	{
		Printf99(ox,7,sd_Data.c_time[i],0);
		Printf99(ox,9,sd_Data.c_time[i+3],0);
		ox += 24;
	}
	ox = 96;
	for(i=0; i<2; i++)
	{
		Printf816(ox,7,"-",0);
		Printf816(ox,9,":",0);
		ox += 24;
	}
	
}


u8 const LCDMODULESYSC[12][7]=
{
	{23,10,48, 0},	//相电压 0
	{49,10,48, 0},//线电压 1
	{10,50, 0},//电流  2
	{73,15,0}, //频率 3
	{51,52,52,15, 0},//有功功率 4
	{53,52,52,15, 0},//无功功率 5
	{54,104,52,15, 0},//视在功率 6
	{52,15,55,56, 0},//功率因数 7
	{10,48,78,16,17,0}, //电压总谐波
	{10,50,78,16,17,0}, //电流总谐波
	{10,48,96,97,98,99,0}, //电压不平衡度
	{10,50,96,97,98,99,0}, //电流不平衡度
};


u8 const LCDMODULESYSE[12][8]=
{
	"Vln",
	"Vll",
	" I ",
	" F ", //Frequency
	" P ",
	" Q ",
	
	" S  ",
	" PF ",
	"THDU",
	"THDI",
	"Vunb",
	"Iunb", //Unbalance
};

u8 const LCDMOSYSSTATE1E[4][6]=
{
	"OK   ",
	"High ",
	"Low  ",
	"Error",	
};

u8 const LCDMOSYSSTATE1C[4][4]=
{
	{84,141,42,0}, //正常
	{74,166,167,0}, //高报警
	{75,166,167,0}, //低报警
	{116,141,42,0}, //异常	
};

u8 const LCDMOSYSSTATE2E[2][6]=
{
	"OK   ",
	"Low  ",
};

u8 const LCDMOSYSSTATE2C[4][4]=
{
	{84,141,42,0}, //正常
	{75,166,167,0}, //低报警
};

/***********************************************
Name: void LCDDisplaySysState(void)
Function: 系统状态显示
Input: None
Output: None
**********************************************/
void LCDDisplaySysState(void)
{
	u8 oy, i, midu8;
	if(1==MeterSetData.c_lcdlanguage) //英文
	{
		oy=2; 
		for(i=0; i<6; i++)
		{
			Printf816(24,oy,LCDMODULESYSE[i],0);	
			Printf816(128,oy,LCDMODULESYSE[i+6],0);
			oy += 2;
		}
		
		//显示系统状态
		oy=2;
		for(i=0; i<6; i++)
		{
			midu8=(AlarmState>>(2*i))&0x03;
			Printf816(64,oy, LCDMOSYSSTATE1E[midu8],0);
			oy+=2;
		}
		//总视在功率
		midu8=(AlarmState>>(2*6))&0x03;
		Printf816(184,2, LCDMOSYSSTATE1E[midu8],0);
		//功率因数
		midu8=(AlarmState>>(14))&0x01;
		Printf816(184,4, LCDMOSYSSTATE2E[midu8],0);
		//
		oy=6;
		for(i=0; i<4; i++)
		{
			midu8=(AlarmState>>(15+i))&0x01;
			Printf816(184,oy, LCDMOSYSSTATE1E[midu8],0);	
			oy+=2;
		}
		
		//画线
		oy = 2;
		for(i=0; i<7; i++)
		{
			DrawLineX(0, oy, 239, 0x01);
			oy += 2;
		}	
		//Y
		DrawLineY(60, 2, 12, 0xff);
		DrawLineY(119, 2, 12, 0xff);
		DrawLineY(176, 2, 12, 0xff);
	}
	else
	{
		oy=2; 
		for(i=0; i<4; i++)
		{
			Printf1616(0,oy,LCDMODULESYSC[i],0);	
			Printf1616(120,oy,LCDMODULESYSC[i+4],0);
			Printf1616(0,oy+8,LCDMODULESYSC[i+8],0);	
			oy += 2;
		}
		
		
		//显示系统状态
		oy=2;
		for(i=0; i<4; i++)
		{
			midu8=(AlarmState>>(2*i))&0x03;
			Printf1616(72,oy, LCDMOSYSSTATE1C[midu8],0);
			oy+=2;
		}
		oy=2;
		for(i=4; i<7; i++)
		{
			midu8=(AlarmState>>(2*i))&0x03;
			Printf1616(192,oy, LCDMOSYSSTATE1C[midu8],0);
			oy+=2;
		}
		//功率因数
		midu8=(AlarmState>>(14))&0x01;
		Printf1616(192,8, LCDMOSYSSTATE2C[midu8],0);
		
		//
		oy=10;
		for(i=0; i<4; i++)
		{
			midu8=(AlarmState>>(15+i))&0x01;
			Printf1616(128,oy, LCDMOSYSSTATE1C[midu8],0);	
			oy+=2;
		}
		
		//画线
		oy = 4;
		for(i=0; i<7; i++)
		{
			DrawLineX(0, oy, 240, 0x01);
			oy += 2;
		}
		//Y
		DrawLineY(64, 2, 8, 0xff);
		DrawLineY(120, 2, 8, 0xff);
		DrawLineY(184, 2, 8, 0xff);
		DrawLineY(98, 10, 8, 0xff);
	}		
}


/************************************************
Name: void LCDDisplayRev(void)
Function: 软件版本号
*************************************************/
void LCDDisplayRev(void)
{
	Printf816(24, 8, REV[0],0);
}

/*************************************************************
Name: void LCDProgram(void)
Function: 编程菜单显示
Input:  None
Output: None
*************************************************************/
void LCDDisplayProgram(void)
{
	switch(ProgState)
	{
		case STEP_CODE:
			LCDProgramCode();                             // 显示密码输入界面
			break;
		case STEP_1:
			LCDProgramStep1();                            // 显示第一级菜单界面
			break;
		case STEP_2:
			LCDProgramStep2();                            // 显示第二级菜单界面
			break;
		case STEP_3:
		case STEP_4: //针对DO设置
		case STEP_5: //DO设置
			LCDProgramStep3();                            // 显示第三级菜单界面
			break;
	
		case STEP_SAVE:
			LCDProgramSave();                             // 显示保存菜单界面
			break;
		case STEP_CAL:
			LCDDisplayCalibrate();                        // 显示校准菜单界面
			break;	
		default:
			
			break;							
		
	}
}

/**************************************************************************
Name: void LCDProgramCode(void)
Function: 显示密码界面
Input:  None
Output: None
**************************************************************************/
u8 const LCDPRCODETITLE1C[3][5]=
{
	{34,35,40,41,0}, //用户设置
	{102,83,100,101,0}, //厂家维护	
	{107,108,126,127}, //输入密码
};

void LCDProgramCode(void)
{
	u8 ox, oy, i;
	if(1==MeterSetData.c_lcdlanguage) //英文菜单
	{
		Printf816(96,5,"Password",0);
		ox=Printf816(0,8,"User    ",(KEY_XY[0][0]-1));
		Printf816Lost(ox, 8, (64-ox),(KEY_XY[0][0]-1));
		ox=Printf816(0,10,"Admin   ",KEY_XY[0][0]);
		Printf816Lost(ox, 10, (64-ox),KEY_XY[0][0]);	
		if(0 == ProgState2) PrintfKeyF4(K_UP,K_DOWN,K_MENUE,K_ENTERE);	//四个按键
		else PrintfKeyF4(K_LEFT,K_UP,K_ESCE,K_ENTERE);	//四个按键
		
		
	}
	else //中文
	{
		Printf1616(88,5,LCDPRCODETITLE1C[2],0); //输入密码
		Printf1616(0,8,LCDPRCODETITLE1C[0],(KEY_XY[0][0]-1));
		Printf1616(0,10,LCDPRCODETITLE1C[1],KEY_XY[0][0]);
		if(0 == ProgState2) PrintfKeyF4(K_UP,K_DOWN,K_MENU,K_ENTER);	//四个按键
		else PrintfKeyF4(K_LEFT,K_UP,K_ESC,K_ENTER);	//四个按键
	}
	
	if(1==ProgState2) //在密码输入界面
	{
		if(0 == KEY_XY[0][0]) Printf9999(80,8,(u16)KEY_XY[2][0],(KeyMult+1));
		else Printf9999(80,10,(u16)KEY_XY[2][0],(KeyMult+1));	
	}
	//X
	oy = 7;
	for(i=0; i<3; i++)
	{
		DrawLineX(0, oy, 240, 0x80);
		oy += 2;
	}
	//Y
	DrawLineY(64, 8, 4, 0xff);
			
}


//编程界面1菜单
u8 const LCDPR1TITLE1C[1][5]=
{
	103,56,40,41,0,//参数设置	
};


u8 const LCDPR1TITLE2C[9][6]=
{
	{38,39,103,56,0},	//系统参数
	{105,106,107,108,0},	//信号输入
	{112,105,40,41,0},	//通信设置
	{25,9,40,41,0}, //需量设置
	{109,110,107,111,0}, //开关输出
	{30,185,107,111,0}, //变送输出
	{166,167,40,41,0}, //报警设置
	{14,15,40,41,0}, //费率设置
	{113,95,114,115,0}, //清零同步
};
u8 const LCDPR1TITLE2E[9][14]=
{
	"Basic Setup",
	"Signal Inputs",
	"COM1 Port",
	"Demand Setup",
	"Relay Outputs",
	"Analog Output",
	"Alarm Setup",
	"Tariff Setup",
	"Reset Data",
};

u8 const LCDPR1TETLE3[2][2]=
{
	{72,0}, //向下箭头
	{71,0}, //向上箭头
};
/**************************************************************************
Name: void LCDProgramStep1(void)
Function: 编程界面第一级菜单
Input:  None
Output: None
**************************************************************************/
void LCDProgramStep1(void)
{
	u8 ox,oy,i,k;
	if(1==MeterSetData.c_lcdlanguage) //西文
	{
		Printf816(0,0,"Setup",0);
		
		if(KEY_XY[0][0]>STEP_1_ALARM) //第二屏
		{
			oy = 4;
			for(i=7; i<9; i++)
			{
				if((0==MeterSetData.c_set[TOUSET])&&(STEP_1_TOU==i))
				{}
				else
				{
					k=0;
					if(i==KEY_XY[0][0]) k=1;
					ox=Printf816(0,oy,LCDPR1TITLE2E[i],k);
					Printf816Lost(ox, oy, (104-ox),k);	
					oy += 2;	
				}
			}
			Printf1616(44, 2, LCDPR1TETLE3[1],0); //向上剪头
		}
		else //第一屏
		{
			oy = 2;
			for(i=0; i<7; i++)
			{
				k=0;
				if(i==KEY_XY[0][0]) k=1;
				ox=Printf816(0,oy,LCDPR1TITLE2E[i],k);
				Printf816Lost(ox, oy, (104-ox),k);	
				oy += 2;
			}
			Printf1616(44, 16, LCDPR1TETLE3[0],0); //向下剪头
		}		
		
		PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_ENTERE);	//四个按键
		//画线
		DrawLineX(0,1,240,0x80);
		oy=3;
		for(i=0; i<8; i++)
		{
			DrawLineX(0, oy, 104, 0x80);
			oy += 2;	
		}	
		DrawLineY(104,2,16,0xff);			
	}
	else //中文
	{
		Printf1616(0,0,LCDPR1TITLE1C[0],0); //用户系统设置参数
		
		if(KEY_XY[0][0]>STEP_1_ALARM) //第二屏
		{
			oy = 4;
			for(i=7; i<9; i++)
			{
				if((0==MeterSetData.c_set[TOUSET])&&(STEP_1_TOU==i))
				{}
				else
				{
					k=0;
					if(i==KEY_XY[0][0]) k=1;	
					Printf1616(0,oy,LCDPR1TITLE2C[i],k);
					oy += 2;	
				}
			}
			Printf1616(24, 2, LCDPR1TETLE3[1],0); //向上剪头
		}
		else //第一屏
		{
			oy = 2;
			for(i=0; i<7; i++)
			{
				k=0;
				if(i==KEY_XY[0][0]) k=1;	
				Printf1616(0,oy,LCDPR1TITLE2C[i],k);
				oy += 2;
			}
			Printf1616(24, 16, LCDPR1TETLE3[0],0); //向下剪头
		}	
		PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_ENTER);	//四个按键
		//画线
		DrawLineX(0,1,240,0x80);
		oy=3;
		for(i=0; i<8; i++)
		{
			DrawLineX(0, oy, 64, 0x80);
			oy += 2;	
		}
		DrawLineY(64,2,16,0xff);			
	}
	
}


/**************************************************************************
Name: void LCDProgramStep2(void)
Function: 编程界面第一级菜单
Input:  None
Output: None
**************************************************************************/
void LCDProgramStep2(void)
{
	if(1==MeterSetData.c_lcdlanguage) // 西文菜单
	{
		Printf816(0,0,LCDPR1TITLE2E[KEY_XY[0][0]],0);
		PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_EDITE);	//四个按键
	}
	else //中文菜单
	{
		Printf1616(0,0,(u8 *)LCDPR1TITLE2C[KEY_XY[0][0]],0);
		PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_EDIT);	//四个按键
	}
	switch(KEY_XY[0][0])
	{
		case STEP_1_SYS: //系统参数
			LCDProgramSys();
			break;
		case STEP_1_INPUT: //信号输入
			LCDProgramInput();
			break;
		case STEP_1_COMM: //通信设置
			LCDProgramComm();
			break;
		case STEP_1_DEMAND: //需量设置
			LCDProgramDemand();
			break;
		case STEP_1_OUTPUT: //继电器设置
			LCDProgramDO();
			break;
		case STEP_1_AOOUT: //变送输出设置
			LCDProgramAO();
			break;
		case STEP_1_ALARM: //报警设置界面
			LCDProgramAlarm();
			break;
		case STEP_1_TOU: //复费率设置
			LCDProgramTOU();
			break;

		case STEP_1_CLR:	//清零操作
			LCDProgramClr();
			break;
		default:
			break;
	}	
	//画线
	DrawLineX(0,1,239,0x80);	
	
}


/**************************************************************************
Name: void LCDProgramStep3(void)
Function: 编程界面第三级菜单
Input:  None
Output: None
**************************************************************************/
void LCDProgramStep3(void)
{
	if(ProgState < STEP_4)
	{	
		if(1==MeterSetData.c_lcdlanguage) // 英文菜单
		{
			Printf816(0,0,LCDPR1TITLE2E[KEY_XY[0][0]],0);
			PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_ENTERE);	//四个按键
		}
		else //中文菜单
		{
			Printf1616(0,0,LCDPR1TITLE2C[KEY_XY[0][0]],0);
			PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_ENTER);	//四个按键
		}
	}
	switch(KEY_XY[0][0])
	{
		case STEP_1_SYS: //系统参数
			LCDProgramSys();
			break;
		case STEP_1_INPUT: //信号输入
			LCDProgramInput();
			break;
		case STEP_1_COMM: //通信设置
			LCDProgramComm();
			break;
		case STEP_1_DEMAND: //需量设置
			LCDProgramDemand();
			break;
		case STEP_1_OUTPUT: //继电器设置
			LCDProgramDO();
			break;
		case STEP_1_AOOUT: //变送输出设置
			LCDProgramAO();
			break;
		case STEP_1_ALARM: //报警设置界面
			LCDProgramAlarm();
			break;
		case STEP_1_TOU: //复费率设置
			LCDProgramTOU();
			break;
		case STEP_1_CLR:	//清零操作
			LCDProgramClr();
			break;
		default:
			break;
	}	
	//画线
	DrawLineX(0,1,239,0x80);
		
	
}

u8 const LCDSYSTITLE1C[5][6]=
{
	{34,35,126,127,42,0},	//用户密码
	{128,129,130,131,42,0},	//背光控制
	{134,135,99,42,0}, 		//对比度
	{38,39,136,137,42,0},	//系统语言
	{120,121,132,133,42,0},	//默认显示
};

u8 const LCDSYSTITLE1E[5][13]=
{
	"Password","Backlight",
	"Contrast","Language",
	"DefaultDisp."  //BootDisp.
};
u8 const LCDSYSTITLE2C[1][3]=
{141,142,0	};//常亮
u8 const LCDSYSTITLE2E[1][5]=
{"Hold",};
u8 const LCDSYSTITLE3C[1][4]=
{
	{138,42,139,0}, //中文
};
u8 const LCDSYSTITLE3E[1][10]=
{"English"};
u8 const LCDSYSTITLE4C[1][2]=
{143,0}; //秒

u8 const LCDSYSTITLE5E[8][9] =
{ 
	"U      ","I      ", "P      ","Energy ",
	"THD    ","Waveform","Demand ","Max.   "
};

/**************************************************************************
Name: void LCDProgramSys(void)
Function: 编程界面系统参数显示
Input:  None
Output: None
**************************************************************************/
void LCDProgramSys(void)
{
	u8 ox,oxk, oy,i;
	if(1==MeterSetData.c_lcdlanguage) //西文
	{
		oy = 2;
		for(i=0; i<5; i++)
		{
			if(i==KEY_XY[1][0])
			{
				ox=Printf816(0,oy,LCDSYSTITLE1E[i],1);
				oxk=1;
			}
			else 
			{
				ox=Printf816(0,oy,LCDSYSTITLE1E[i],0);
				oxk=0;
			}
			Printf816Lost(ox, oy, (80-ox),oxk);	
			oy += 2;
		}	
		Printf816(120,4, "s",0);
		if(0==MeterSetData.c_lcdlight) Printf816(144,4,"Hold",0);
		
	}
	else //中文
	{
		oy = 2;
		for(i=0; i<5; i++)
		{
			if(i==KEY_XY[1][0])Printf1616(0,oy,LCDSYSTITLE1C[i],1);
			else Printf1616(0,oy,LCDSYSTITLE1C[i],0);
			oy += 2;
		}	
		Printf1616(120,4, LCDSYSTITLE4C[0],0); //秒
		if(0==MeterSetData.c_lcdlight) Printf1616(144,4,LCDSYSTITLE2C[0],0); //常亮
		
	}
	Printf9999(88,2,MeterSetData.c_password,0);
	Printf999(88,4,MeterSetData.c_lcdlight,0);
	Printf9(88,6,MeterSetData.c_lcdgray,0);
	//中文/English
	if(1==MeterSetData.c_lcdlanguage) 
	{
		ox=Printf816(88,8,LCDSYSTITLE3E[0],0);
		Printf816Lost(ox, 8, (136-ox),0);	
	}
	else Printf1616(88,8,LCDSYSTITLE3C[0],0);
	
	ox=Printf816(88,10,LCDSYSTITLE5E[MeterSetData.c_powerview],0);
	Printf816Lost(ox, 10, (153-ox),0);
	
	//按键在第三级菜单
	if(STEP_3 == ProgState)
	{
		//根据所在界面和中英文确认下面的按键显示
		if(1==MeterSetData.c_lcdlanguage)
		{
			if(KEY_XY[1][0]>STEP_2_LIGHT) PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_OKE);
			else PrintfKeyF4(K_LEFT,K_UP,K_ESCE,K_OKE);				
		}
		else
		{
			if(KEY_XY[1][0]>STEP_2_LIGHT) PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_OK);
			else PrintfKeyF4(K_LEFT,K_UP,K_ESC,K_OK);
		}		
		
		if(STEP_2_CODE == KEY_XY[1][0]) Printf9999(88,2,(u16)KEY_XY[2][0],(KeyMult+1));
		else if(STEP_2_LIGHT == KEY_XY[1][0]) 
		{
			Printf999(88,4,(u16)KEY_XY[2][0],(KeyMult+1));
			if(0==(u8)KEY_XY[2][0])
			{
				if(1==MeterSetData.c_lcdlanguage) Printf816(144,4,"Hold",0);
				else  Printf1616(144,4,LCDSYSTITLE2C[0],0); //常亮
			}	
		}
		else if(STEP_2_GARY == KEY_XY[1][0])
		{
			Printf9(88,6,(u8)KEY_XY[2][0],(KeyMult+1));
		} 
		else if(STEP_2_LANGUAGE == KEY_XY[1][0])
		{
			//中文/English
			if(1==KEY_XY[2][0]) 
			{
				ox=Printf816(88,8,LCDSYSTITLE3E[0],1);
				Printf816Lost(ox, 8, (136-ox),1);	
			}
			else Printf1616(88,8,LCDSYSTITLE3C[0],1);
		}
		else if(STEP_2_DISPLAY==KEY_XY[1][0])
		{	
			ox=Printf816(88,10,LCDSYSTITLE5E[KEY_XY[2][0]],1);
			Printf816Lost(ox, 10, (153-ox),1);
		}		
	}
	//画线	
	oy=3;
	for(i=0; i<5; i++)
	{
		DrawLineX(0, oy, 239, 0x80);
		oy += 2;	
	}	
	DrawLineY(80,2,10,0xff);		
}


u8 const LCDINPUTTITLE1C[5][6]=
{
	{144,49,145,146,42,0},	//接线方式
	{147,148,10,48,42,0}, 	//额定电压
	{149,150,10,48,42,0},	//初次电压
	{147,148,10,50,42,0},	//额定电流
	{149,150,10,50,42,0}, 	//初次电流	
};
u8 const LCDINPUTTITLE1E[5][13]=
{
	"Wiring      ","PT Secondary","PT Primary  ",
	"CT Secondary",	"CT Primary  ",	
};

u8 const LCDINPUTTITLE2[4][5]=
{
	"3P4W","3P3W","1P2W","1P3W",	
};
/**************************************************************************
Name: void LCDProgramInput(void)
Function: 编程界面信号输入显示
Input:  None
Output: None
**************************************************************************/
void LCDProgramInput(void)
{
	u8 ox,oy,i, oxk;
	if(1==MeterSetData.c_lcdlanguage) //西文
	{
		oy = 2;
		for(i=0; i<5; i++)
		{
			if(i==KEY_XY[1][0])
			{
				ox=Printf816(0,oy,LCDINPUTTITLE1E[i],1);
				oxk=1;
			}
			else 
			{
				ox=Printf816(0,oy,LCDINPUTTITLE1E[i],0);	
				oxk=0;
			}
			Printf816Lost(ox, oy, (96-ox),oxk);
			oy += 2;
		}	
		
	}
	else //中文
	{
		oy = 2;
		for(i=0; i<5; i++)
		{
			if(i==KEY_XY[1][0])Printf1616(0,oy,LCDINPUTTITLE1C[i],1);
			else Printf1616(0,oy,LCDINPUTTITLE1C[i],0);
			oy += 2;
		}	
	}
	Printf816(112,2,LCDINPUTTITLE2[MeterSetData.c_input[0]],0);
	Printf999(112,4,MeterSetData.c_pt2,0);
	Printf999999(112,6,MeterSetData.c_pt1,0);
	Printf9(112,8,MeterSetData.c_ct2,0);
	Printf999999(112,10,MeterSetData.c_ct1,0);
	Printf816(140,4, "V",0);	
	Printf816(164,6,"V",0);
	Printf816(124,8,"A",0);
	Printf816(164,10,"A",0);
	//按键在第三级菜单
	if(STEP_3 == ProgState)
	{
		//根据所在界面和中英文确认下面的按键显示
		if(1==MeterSetData.c_lcdlanguage)
		{
			if((STEP_2_INPUT==KEY_XY[1][0])||(STEP_2_CT2==KEY_XY[1][0])) PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_OKE);
			else PrintfKeyF4(K_LEFT,K_UP,K_ESCE,K_OKE);				
		}
		else
		{
			if((STEP_2_INPUT==KEY_XY[1][0])||(STEP_2_CT2==KEY_XY[1][0])) PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_OK);
			else PrintfKeyF4(K_LEFT,K_UP,K_ESC,K_OK);
		}		
		
		if(STEP_2_INPUT == KEY_XY[1][0]) Printf816(112,2,LCDINPUTTITLE2[KEY_XY[2][0]],1);
		else if(STEP_2_PT2 == KEY_XY[1][0]) Printf999(112,4,(u16)KEY_XY[2][0],(KeyMult+1));	
		else if(STEP_2_PT1 == KEY_XY[1][0]) Printf999999(112,6,KEY_XY[2][0],(KeyMult+1));	
		else if(STEP_2_CT2 == KEY_XY[1][0]) Printf9(112,8,(u8)KEY_XY[2][0],(KeyMult+1));	
		else if(STEP_2_CT1 == KEY_XY[1][0]) Printf999999(112,10,KEY_XY[2][0],(KeyMult+1));	
	}
	//画线	
	
	oy=3;
	for(i=0; i<5; i++)
	{
		DrawLineX(0, oy, 239, 0x80);
		oy += 2;	
	}	
	if(1==MeterSetData.c_lcdlanguage) DrawLineY(96,2,10,0xff);		
	else DrawLineY(80,2,10,0xff);		
}


u8 const LCDCOMMTITLE1C[4][5]=
{
	{151,152,153,154,0},	//从站地址
	{17,155,15,42,0}, //波特率
	{56,156,157,146,0},	//数据格式
	{112,105,117,118,0}, //通信协议
};
u8 const LCDCOMMTITLE1E[4][10]=
{
	"Address  ","BautRate ","Parity   ",
	"Protocol ",
};

u8 const LCDCOMMTITLE2[8][10]=
{
	"1200 bps ", "2400 bps ","4800 bps ", "9600 bps ", 
	"19200 bps", "38400 bps","57600 bps","115200bps"
};
u8 const LCDCOMMTITLE3[4][4]=
{"N81","E81","O81","N82",};

u8 const LCDCOMMTITLE4[4][12]=
{
	"Modbus-RTU",
	"DL/T 645   ",
};
/**************************************************************************
Name: void LCDProgramComm(void)
Function: 编程界面通信设置显示
Input:  None
Output: None
**************************************************************************/
void LCDProgramComm(void)
{
	u8 ox, oxk,oy,i;
	if(1==MeterSetData.c_lcdlanguage) //西文
	{
		oy = 2;
		for(i=0; i<4; i++)
		{
			if(i==KEY_XY[1][0])
			{
				ox=Printf816(0,oy,LCDCOMMTITLE1E[i],1);
				oxk=1;
			}
			else 
			{
				ox=Printf816(0,oy,LCDCOMMTITLE1E[i],0);				
				oxk=0;
			}	
			Printf816Lost(ox, oy, (72-ox),oxk);	
			oy += 2;
		}	
		
	}
	else //中文
	{
		oy = 2;
		for(i=0; i<4; i++)
		{
			if(i==KEY_XY[1][0])Printf1616(0,oy,LCDCOMMTITLE1C[i],1);
			else Printf1616(0,oy,LCDCOMMTITLE1C[i],0);
			oy += 2;
		}	
	}
	Printf999(88,2,MeterSetData.c_com1[0],0);
	Printf816(88,4,LCDCOMMTITLE2[MeterSetData.c_com1[1]],0);
	Printf816(88,6,LCDCOMMTITLE3[MeterSetData.c_com1[2]],0);
	Printf816(88,8,LCDCOMMTITLE4[MeterSetData.c_com1[3]],0);

	//按键在第三级菜单
	if(STEP_3 == ProgState)
	{
		//根据所在界面和中英文确认下面的按键显示
		if(1==MeterSetData.c_lcdlanguage)
		{
			if(KEY_XY[1][0]>STEP_2_SN) PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_OKE);
			else PrintfKeyF4(K_LEFT,K_UP,K_ESCE,K_OKE);				
		}
		else
		{
			if(KEY_XY[1][0]>STEP_2_SN) PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_OK);
			else PrintfKeyF4(K_LEFT,K_UP,K_ESC,K_OK);
		}		
		
		if(STEP_2_SN==KEY_XY[1][0]) Printf999(88,2,(u16)KEY_XY[2][0],(KeyMult+1));
		else if(STEP_2_BAUD==KEY_XY[1][0]) Printf816(88,4,LCDCOMMTITLE2[KEY_XY[2][0]],1);
		else if(STEP_2_DATA==KEY_XY[1][0])Printf816(88,6,LCDCOMMTITLE3[KEY_XY[2][0]],1);
		else if(STEP_2_PROT==KEY_XY[1][0])Printf816(88,8,LCDCOMMTITLE4[KEY_XY[2][0]],1);
	}
	//画线	
	oy=3;
	for(i=0; i<4; i++)
	{
		DrawLineX(0, oy, 239, 0x80);
		oy += 2;	
	}	
	if(1==MeterSetData.c_lcdlanguage) DrawLineY(72,2,8,0xff);		
	else DrawLineY(64,2,8,0xff);			
}

u8 const LCDPRDEMANDTITLE1[1][5]=
{"1--6",};
u8 const LCDPRDEMANDTITLE2E[2][7]=
{"Slid  ","Fixed"};

u8 const LCDPRDEMANDTITLE2C[2][3]=
{
	{198,199,0}, //滑差
	{186,148,0}, //固定	
};

u8 const LCDDOTITLE1C[2][3]=
{
	{112,172,0}, //通 道
	{7,194,0}, //时间
};	

u8 const LCD2AOTITLE2C[4][3]=
{
	{30,146,0}, //模式
	{174,175,0}, //项目
	{196,81,0}, //下限
	{197,81,0},	//上限
};

u8 const LCDMAXTIMETITLE1C[1][7]=
{
	{26,27,200,220,40,41,0},//极值区间设置
};
/**************************************************************************
Name: void LCDProgramDemand(void)
Function: 编程界面需量设置显示
Input:  None
Output: None
**************************************************************************/
void LCDProgramDemand(void)
{
	u8 oy, i, m;
	if(1==MeterSetData.c_lcdlanguage) //英文菜单
	{
		Printf816(8,2,"No.",0);	
		Printf816(40,2,"Item",0);
		Printf816(88,2,"Mode",0);
		Printf816(88,4,LCDPRDEMANDTITLE2E[MeterSetData.c_demand[0][1]],0);
		Printf816(0,7,"Max/Min Time Interval",0);
		
	}
	else //中文菜单
	{
		Printf1616(0,2,LCDDOTITLE1C[0],0);  	//通道
		Printf1616(40,2,LCD2AOTITLE2C[1],0); 	//项目
		Printf1616(88,2,LCD2AOTITLE2C[0],0);	//模式
		Printf1616(88,4,LCDPRDEMANDTITLE2C[MeterSetData.c_demand[0][1]],0);
		Printf1616(0,7,LCDMAXTIMETITLE1C[0],0); //极值区间设置
		
	}
	Printf816(144,2,"t(s)",0);
	Printf816(198,2,"T(*t)",0);
	
	
	
	oy = 4;
	if(0==KEY_XY[1][0]) m=1;
	else m=0;
	Printf816(0,oy,LCDPRDEMANDTITLE1[0],m); //1--6,
	if(1==KEY_XY[1][0]) m=1;
	else m=0;
	if(1==MeterSetData.c_lcdlanguage) Printf816(0,9,"Time",m);
	else Printf1616(0,9, LCDDOTITLE1C[1],m); //时间
	Printf9999(56,9,MeterSetData.c_MaxMinTime,0);
	Printf816(96,9,"min",0);
			
	//变送项目
	Printf816(36,oy," IPQS",0);
	//滑差
	Printf9999(140,oy,MeterSetData.c_demand[0][2],0);
	//周期
	Printf99(204,oy,MeterSetData.c_demand[0][3],0); 
	
	//第三级菜单
	if((STEP_3 == ProgState)&&(0==KEY_XY[1][0]))
	{
		oy = 4;
		if(1==MeterSetData.c_lcdlanguage)
		{
			PrintfKeyF4(K_LEFT,K_RIGHT,K_ESCE,K_EDITE);	//四个按键	
			//Mode	
			if(1==KEY_XY[2][0])
			{
				Printf816(88,oy,LCDPRDEMANDTITLE2E[MeterSetData.c_demand[0][1]],1);
			} 
		}
		else
		{
			PrintfKeyF4(K_LEFT,K_RIGHT,K_ESC,K_EDIT);	//四个按键	
			//模式
			if(1==KEY_XY[2][0]) Printf1616(88,oy,LCDPRDEMANDTITLE2C[MeterSetData.c_demand[KEY_XY[1][0]][1]],1);
		}
		if(2==KEY_XY[2][0]) Printf9999(140,oy,MeterSetData.c_demand[KEY_XY[1][0]][2],5);
		if(3==KEY_XY[2][0]) Printf99(204,oy,MeterSetData.c_demand[KEY_XY[1][0]][3],1);
				
	}
	
	if((STEP_3 == ProgState)&&(1==KEY_XY[1][0]))
	{
		Printf9999(56,9,KEY_XY[2][0],KeyMult+1);	
		if(1==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_LEFT,K_UP,K_ESCE,K_OKE);	//四个按键	
		else PrintfKeyF4(K_LEFT,K_UP,K_ESC,K_OK);	//四个按键	
	}
	
	
	
	//第四级菜单
	if(STEP_4 == ProgState)
	{
		oy = 4;
		if(1==MeterSetData.c_lcdlanguage)
		{
			Printf816(0,0,LCDPR1TITLE2E[KEY_XY[0][0]],0);//Demand Set
			PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_OKE);	//四个按键	
			//Mode
			if(1==KEY_XY[2][0])
			{
				Printf816(88,oy,LCDPRDEMANDTITLE2E[KEY_XY[3][0]],1);
			} 
		}
		else
		{
			Printf1616(0,0,LCDPR1TITLE2C[KEY_XY[0][0]],0); //需量设置
			if(2==KEY_XY[2][0]) PrintfKeyF4(K_LEFT,K_UP,K_ESC,K_OK);	//四个按键	
			else PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_OK);	//四个按键	
			//模式
			if(1==KEY_XY[2][0]) Printf1616(88,oy,LCDPRDEMANDTITLE2C[KEY_XY[3][0]],1);
		}
		if(2==KEY_XY[2][0]) Printf9999(140,oy,KEY_XY[3][0],(KeyMult+1));
		if(3==KEY_XY[2][0]) Printf99(204,oy,KEY_XY[3][0],1);
	}	
	//画线
	oy=3;
	for(i=0; i<2; i++)
	{
		DrawLineX(0, oy, 239, 0x80);
		DrawLineX(0, oy+5, 239, 0x80);
		oy += 2;	
	}	
	DrawLineY(32,2,4,0xff);
	DrawLineY(80,2,4,0xff);
	DrawLineY(128,2,4,0xff);
	DrawLineY(184,2,4,0xff);
	DrawLineY(32,9,2,0xff);
}




u8 const LCDDOTITLE2C[3][4]=
{
	{158,160,159,0},	//第一路
	{158,161,159,0}, 	//第二路
	{158,162,159,0}, 	//第三路	
};
u8 const LCDDOTITLE2E[4][9]=
{
	"   01   ","   02   ","   03   ","   04   ",	
};
u8 const LCDDOTITLE3E[4][3]=
{
	"01","02","03","04",	
};

u8 const LCDDOTITLE3EE[4][5] =
{
	" 01 "," 02 "," 03 "," 04 ",
};

u8 const LCDDOTITLE3C[1][5]=
{
	30,42,42,146,0,//模式
};

u8 const LCDDOTITLE4C[3][5]=
{
	{42,110,164,42,0}, //关闭
	{166,167,168,169,0}, //报警联动
	{112,105,165,130,0}, //通信遥控
};
u8 const LCDDOTITLE4E[3][9]=
{
	" Off    ", 
	" Alarm  ",
	" Remote ",
};
/**************************************************************************
Name: void LCDProgramDO(void)
Function: 编程界面继电器设置显示
Input:  None
Output: None
**************************************************************************/
void LCDProgramDO(void)
{
	u8 oy,i;
	
	if(ProgState<STEP_4) //在DO选择界面STEP_2,STEP_3
	{
		if(1==MeterSetData.c_lcdlanguage) //英文
		{
			Printf816(16,2,"No.",0);
			Printf816(88,2, "Mode",0); 
			oy = 4;
			for(i=0; i<3; i++)
			{
				if(i==KEY_XY[1][0])Printf816(0,oy,LCDDOTITLE2E[i],1); //01/02/03
				else Printf816(0,oy,LCDDOTITLE2E[i],0);
				Printf816(88,oy,LCDDOTITLE4E[MeterSetData.c_DO[i][0]],0);	
				oy += 2;
			}	
		}
		else //中文
		{
			Printf1616(16,2,LCDDOTITLE1C[0],0); //通道
			Printf1616(88,2, LCDDOTITLE3C[0],0); //模式
			oy = 4;
			for(i=0; i<3; i++)
			{                                      
				if(i==KEY_XY[1][0])Printf816(0,oy,LCDDOTITLE2E[i],1); //第一路/第二路
				else Printf816(0,oy,LCDDOTITLE2E[i],0);
				Printf1616(88,oy,LCDDOTITLE4C[MeterSetData.c_DO[i][0]],0);	//工作模式
				oy += 2;
			}	
		}
		//按键在第三级菜单
		if(STEP_3 == ProgState)
		{
			//根据所在界面和中英文确认下面的按键显示
			if(1==MeterSetData.c_lcdlanguage) //西文 界面
			{
				PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_OKE);	
				oy=4;
				for(i=0; i<3; i++)
				{
					if(i==KEY_XY[1][0])
					{
						Printf816(88,oy,LCDDOTITLE4E[KEY_XY[2][0]],1); //英文
					} 	
					oy += 2;
				}			
			}
			else //中文界面
			{
				PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_OK);
				oy=4;
				for(i=0; i<3; i++)
				{
					if(i==KEY_XY[1][0]) Printf1616(88,oy,LCDDOTITLE4C[KEY_XY[2][0]],1);		
					oy += 2;
				}	
			}		
		}
		//画线	
		oy=3;
		for(i=0; i<4; i++)
		{
			DrawLineX(0, oy, 239, 0x80);
			oy += 2;	
		}	
		DrawLineY(64,2,8,0xff);
		
	}	
	else //STEP_4,STEP_5界面
	{
		
		LCDProgramDOStep45(); //	
	}	
}

u8 const LCDDOALRTITEL1C[5][5] =
{
	{170,42,42,173,0}, //脉宽
	{174,42,42,175,0}, //项目
	{56,42,42,27,0}, //数值
	{176,42,42,177,0}, //回滞	
	{178,42,42,7,0}, //延时
};
u8 const LCDDOALRTITEL1E[6][9]=
{
	"Pulse   ","Item    ","Value   ",
	"Hys     ","Delay   ",
};

u8 const LCDDOALRTITEL11E[4][7]=
{
	"Pulse ","Item  ",
	"Source","Mode  ",
};
u8 const LCDDOALRTITLE2[67][10]=
{ 
	"V1    > ","V1    < ","V2    > ","V2    < ",	//0
	"V3    > ","V3    < ","Vln   > ","Vln   < ",	//4
	"U12   > ","U12   < ","U23   > ","U23   < ",	//8
	"U31   > ","U31   < ","Ull   > ","Ull   < ",	//12
	"Vlnavg> ","Vlnavg< ","Ullavg > ","Ullavg < ",	//16
	"I1    > ","I1    < ","I2    > ","I2    < ",	//20
	"I3    > ","I3    < ",		 	 	 		 //24
	"I     > ","I     < ","Iavg  > ","Iavg  < ",	//26
	"In    > ","In    < ","P     > ","P     < ",	//30
	"Q     > ","Q     < ","S     > ","S     < ",	//34
	"PF    > ","PF    < ","F     > ","F     < ",	//38
	"Vunb  > ","Vunb  < ","Iunb  > ","Iunb  < ",	//42
	"THDu  > ","THDu  < ","THDi  > ","THDi  < ",	//46
	"Alarm-1 ","Alarm-0 ","DIx-1   ","DIx-0   ",    //50
	"DI1-1   ","DI1-0   ","DI2-1   ","DI2-0   ",	//54
	"DI3-1   ","DI3-0   ","DI4-1   ","DI4-0   ",	//58
	"DI5-1   ","DI5-0   ","DI6-1   ","DI6-0   ",	//62	
	"ERROR ",
};



/*************************************************************************
Name: void LCDProgramDOStep45(void)
Function: 在继电器输出的第四第五级菜单显示
Input:  None
Output: None
**************************************************************************/
void LCDProgramDOStep45(void)
{
	u8 i,oy,m, ox, oxk;
	u16 midu16[6]; //
	//根据所在界面确定要显示的数据,
	if(STEP_1_OUTPUT==KEY_XY[0][0])
	{
		for(i=0; i<6; i++) midu16[i]=MeterSetData.c_DO[KEY_XY[1][0]][i+1]; 	
				if(1==MeterSetData.c_lcdlanguage) // 英文菜单
		{
			//标题 CH01 Alarm
			Printf816(0,0,"No.",0);
			Printf816(24,0,LCDDOTITLE3E[KEY_XY[1][0]],0); //01/02
			Printf816(40,0," ",0); //空格
			Printf816(48,0,LCDDOTITLE4E[KEY_XY[2][0]],0); //remote alarm  En Pulse	
			
			if(STEP_5 == ProgState) 
			{
				if(((midu16[1]>STEP_5_ITHDL)&&(STEP_4_PULSE!=KEY_XY[3][0]))||(STEP_4_ITEM==KEY_XY[3][0])) 
				{
					PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_OKE);	//四个按键	
				}
				else PrintfKeyF4(K_LEFT,K_UP,K_ESCE,K_OKE);	//四个按键
			}	
			else PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_EDITE);	//四个按键
				
		}
		else //中文菜单
		{
			//第一路通道通信遥控
			Printf1616(0,0,LCDDOTITLE2C[KEY_XY[1][0]],0); //第一路/第二路
			Printf1616(48,0,LCDDOTITLE4C[KEY_XY[2][0]],0); //工作模式
			if(STEP_5 == ProgState) 
			{
				if(((midu16[1]>STEP_5_ITHDL)&&(STEP_4_PULSE!=KEY_XY[3][0]))||(STEP_4_ITEM==KEY_XY[3][0])) 
				{
					PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_OK);	//四个按键
				}
				else PrintfKeyF4(K_LEFT,K_UP,K_ESC,K_OK);	//四个按键
			}	
			else PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_EDIT);	//四个按键
		}	
	
		//不同继电器输出设置
		if(STEP_3_DOREM==KEY_XY[2][0]) //遥控模式显示
		{
			if(1==MeterSetData.c_lcdlanguage) //英文
			{
				ox=Printf816(0,2,LCDDOALRTITEL1E[0],1); //Pulse
				Printf816Lost(ox, 2, (64-ox),1);	
			}
			else
			{
				Printf1616(0,2,LCDDOALRTITEL1C[0],1); //脉宽
			}	
			//显示脉冲宽度数据
			Printf9999(80,2,midu16[0],0);
			Printf816(112,2,"*100ms",0);
			
			if(STEP_5 == ProgState)
			{
				Printf9999(80,2,(u16)KEY_XY[4][0],(KeyMult+1));
			}			
			//画线	
			DrawLineX(0, 3, 239, 0x80);
			DrawLineY(64,2,2,0xff);
		}		
		else if(STEP_3_DOALR==KEY_XY[2][0]) //报警联动
		{
			//界面显示
			if(1==MeterSetData.c_lcdlanguage) //英文
			{
				oy = 2;
				if(midu16[1]>STEP_5_ITHDL)
				{
					for(i=0; i<2; i++)
					{
						if(i==KEY_XY[3][0]) 
						{
							ox=Printf816(0,oy,LCDDOALRTITEL11E[i],1);
							oxk=1;
						}
						else 
						{
							ox=Printf816(0,oy,LCDDOALRTITEL11E[i],0);
							oxk=0;
						}	
						Printf816Lost(ox, oy, (64-ox),oxk);	
						oy += 2;
					}
				}
				else
				{
					for(i=0; i<6; i++)
					{
						if(i==KEY_XY[3][0]) 
						{
							ox=Printf816(0,oy,LCDDOALRTITEL1E[i],1);
							oxk=1;
						}
						else 
						{
							ox=Printf816(0,oy,LCDDOALRTITEL1E[i],0);	
							oxk=0;
						}	
						Printf816Lost(ox, oy, (64-ox),oxk);	
						oy += 2;
					}	
				}		
			}
			else //中文
			{
				oy = 2;
				if(midu16[1]>STEP_5_ITHDL) //开关量联动
				{
					for(i=0; i<2; i++)
					{
						if(i==KEY_XY[3][0]) Printf1616(0,oy,LCDDOALRTITEL1C[i],1);
						else Printf1616(0,oy,LCDDOALRTITEL1C[i],0);
						oy += 2;
					}
				}
				else //报警
				{
					for(i=0; i<5; i++)
					{
						if(i==KEY_XY[3][0]) Printf1616(0,oy,LCDDOALRTITEL1C[i],1);
						else Printf1616(0,oy,LCDDOALRTITEL1C[i],0);
						oy += 2;
					}	
				}
			}			
			//数据显示
			Printf9999(80,2,midu16[0],0);	//显示脉冲宽度数据
			Printf816(112,2,"*100ms",0);
			Printf816(80,4,LCDDOALRTITLE2[midu16[1]],0); //项目
			
			if(midu16[1]<STEP_3_DOITMEM) //
			{
				m=0;
				if(midu16[1]<STEP_5_IAH) m=1; //电压
				else if(midu16[1]<STEP_5_PH) m=3; //电流显示
				else if(midu16[1]<STEP_5_PFH) m=0; //功率
				else if(midu16[1]<STEP_5_FH) m=3; //功率因数	
				else if(midu16[1]<STEP_5_UNBH) m=2;	//频率
				else if(midu16[1]<STEP_5_UTHDH) m=1; //不平衡度
				else if(midu16[1]<STEP_5_ALARM1) m=2;//谐波数据
					
				Printf9999DotShadow(80,6,midu16[2],m,0);
				Printf9999DotShadow(80,8,midu16[3],m,0);	
				if(midu16[1]<STEP_5_IAH) 
				{
					Printf816(136, 6, "V", 0);
					Printf816(136, 8, "V", 0);	
				}
				else if(midu16[1]<STEP_5_PH)
				{
					Printf816(136, 6, "A",0);
					Printf816(136, 8, "A",0);
				} 
				else if(midu16[1]<STEP_5_QH)
				{
					Printf816(136, 6, "W",0);
					Printf816(136, 8, "W",0);
				}
				else if(midu16[1]<STEP_5_SH)
				{
					Printf816(136, 6, "var",0);
					Printf816(136, 8, "var",0);
				}
				else if(midu16[1]<STEP_5_PFH)
				{
					Printf816(136, 6, "VA",0);
					Printf816(136, 8, "VA",0);
				}	
				else if(midu16[1]<STEP_5_FH) 
				{
					Printf816(136, 6, " ",0);
					Printf816(136, 8, " ",0);
				}	
				else if(midu16[1]<STEP_5_UNBH) 
				{
					Printf816(136, 6, "Hz",0);
					Printf816(136, 8, "Hz",0);
				}
				else if(midu16[1]<STEP_5_ALARM1) 
				{
					Printf816(136, 6, "%",0);
					Printf816(136, 8, "%",0);
				}
			}
			
			//针对报警部分具有的设置
			if(midu16[1]<STEP_5_ALARM1) //
			{
				//延时
				Printf9999(80,10,midu16[4],0);
				Printf816(112,10,"*100ms",0);
			}	
			
			//在第5级菜单
			if(STEP_5 == ProgState)
			{
				if(STEP_4_PULSE == KEY_XY[3][0]) Printf9999(80,2,KEY_XY[4][0],(KeyMult+1));	//显示脉冲宽度数据
				if(STEP_4_ITEM == KEY_XY[3][0]) Printf816(80,4,LCDDOALRTITLE2[KEY_XY[4][0]],1); //项目
				//报警值
				if(STEP_4_VALUE == KEY_XY[3][0])
				{
					if(midu16[1]<STEP_5_ALARM1) //
					{
						if(midu16[1]<STEP_5_IAH) //电压
						{
							Printf9999DotShadow(80,6,KEY_XY[4][0],1,(KeyMult+1)); 
						}	
						else if(midu16[1]<STEP_5_PH)//电流显示
						{
							Printf9999DotShadow(80,6,KEY_XY[4][0],3,(KeyMult+1)); 
						}
						else if(midu16[1]<STEP_5_PFH) //功率
						{
							Printf9999DotShadow(80,6,KEY_XY[4][0],0,(KeyMult+1)); 
						}
						else if(midu16[1]<STEP_5_FH) //功率因数
						{
							Printf9999DotShadow(80,6,KEY_XY[4][0],3,(KeyMult+1)); 
						}
						else if(midu16[1]<STEP_5_UNBH) //频率
						{
							Printf9999DotShadow(80,6,KEY_XY[4][0],2,(KeyMult+1)); 
						}
						else if(midu16[1]<STEP_5_UTHDH) //不平衡度
						{
							Printf9999DotShadow(80,6,KEY_XY[4][0],3,(KeyMult+1)); 
						}
						else if(midu16[1]<STEP_5_ALARM1) //谐波数据
						{
							Printf9999DotShadow(80,6,KEY_XY[4][0],2,(KeyMult+1)); 
						}
	
					}			
				}
				//针对报警部分具有的设置
				//回滞量
				if(STEP_4_HYS == KEY_XY[3][0])
				{
					if(midu16[1]<STEP_5_ALARM1) //
					{
						if(midu16[1]<STEP_5_IAH) //电压
						{
							Printf9999DotShadow(80,8,KEY_XY[4][0],1,(KeyMult+1)); 
						}	
						else if(midu16[1]<STEP_5_PH)//电流显示
						{
							Printf9999DotShadow(80,8,KEY_XY[4][0],3,(KeyMult+1)); 
						}
						else if(midu16[1]<STEP_5_PFH) //功率
						{
							Printf9999DotShadow(80,8,KEY_XY[4][0],0,(KeyMult+1)); 
						}
						else if(midu16[1]<STEP_5_FH) //功率因数
						{
							Printf9999DotShadow(80,8,KEY_XY[4][0],3,(KeyMult+1)); 
						}
						else if(midu16[1]<STEP_5_UNBH) //频率
						{
							Printf9999DotShadow(80,8,KEY_XY[4][0],2,(KeyMult+1)); 
						}
						else if(midu16[1]<STEP_5_UTHDH) //不平衡度
						{
							Printf9999DotShadow(80,8,KEY_XY[4][0],3,(KeyMult+1)); 
						}
						else if(midu16[1]<STEP_5_ALARM1) //谐波数据
						{
							Printf9999DotShadow(80,8,KEY_XY[4][0],2,(KeyMult+1)); 
						}
					}				
				}
				//延时
				if(STEP_4_DELAY == KEY_XY[3][0])
				{
					Printf9999(80,10,KEY_XY[4][0],(KeyMult+1));
				}				
			}	
			//画线	
			oy=3;
			for(i=0; i<5; i++)
			{
				DrawLineX(0, oy, 239, 0x80);
				oy += 2;	
			}	
			DrawLineY(64,2,10,0xff);
		
		}		
	}

}




u8 const LCDAOTITLE1C[1][6]=
{30,185,9,107,111,0};//模拟量输出


u8 const LCD2AOTITLE2E[4][5] =
{
	"Mode","Item","Zero","Full",	
};
//项目
u8 const LCD2AOTITLE3[27][4] =
{
	"V1 ","V2 ","V3 ","U12","U23","U31",
	"I1 ","I2 ","I3 ","In ","P1 ","P2 ","P3 ",
	"P  ","Q1 ","Q2 ","Q3 ","Q  ","S1 ","S2 ",
	"S3 ","S  ","PF1","PF2","PF3","PF ","F  ",
};
u8 const LCD2AOTITLE4[4][10] =
{
	" off     ","4-20mA   ","0-20mA   ", "4-12-20mA"	
};

u8 const LCD2AOTITLE4V[4][10] =
{
	" off   ","1-5V   ","0-5V   ", "1-3-5V "	
};

u8 const LCD2AOTITLE5[2][4] =
{
	" 01 "," 02 "	
}  ;
/************************************************************************
Name: void LCDProgramAO(void)
Function: 变送输出设置
Input:  None
Output: None
STEP_2界面：
 01模拟量变送输出 1/2
	项目
	模式
	下限
	上限

STEP_3界面：
        01模拟量变送输出
项目
模式
下限
上限

STEP_4界面E：
        01 Analog Output
Item
Mode
Zero
Full
  
************************************************************************/
const u8 UpDownMark[1][2]=
{
	119,0	
};

void LCDProgramAO(void)
{
	u8 i, m, oy, ox;
	s16 mids16[6];
	TopShadow(); //顶部阴影显示
	if(1==MeterSetData.c_lcdlanguage) //西文
	{
		Printf816(0,0,LCD2AOTITLE5[KEY_XY[1][0]],1); // 01/02
		Printf816(32,0,"Analog Output",1);
		//提示符
		if(STEP_4==ProgState)
		{
			if(KEY_XY[2][0]<STEP_2_AODS) PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_OKE);
			else PrintfKeyF4(K_LEFT,K_UP,K_ESCE,K_OKE);
		}
		else if(STEP_3==ProgState) PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_EDITE); 
		else 
		{
			
			if(2==MeterSetData.c_set[AOSET]) PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_ENTERE);	//四个按键
			else PrintfKeyF4(K_NO,K_NO,K_ESCE,K_ENTERE);	//四个按键
		}
	}
	else //中文
	{
		Printf1616(0,0,LCDDOTITLE2C[KEY_XY[1][0]],1); //第一路/第二路
		Printf1616(48,0,LCDAOTITLE1C[0],1); //模拟量输出
		//提示符
		if(STEP_4==ProgState)
		{
			if(KEY_XY[2][0]<STEP_2_AODS) PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_OK);
			else PrintfKeyF4(K_LEFT,K_UP,K_ESC,K_OK);
		}
		else if(STEP_3==ProgState) PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_EDIT);
		else 
		{
			if(2==MeterSetData.c_set[AOSET]) PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_ENTER);	//四个按键	
			else PrintfKeyF4(K_NO,K_NO,K_ESC,K_ENTER);	//四个按键	
		}
	}
	
	if(2==MeterSetData.c_set[AOSET]) Printf1616(220,0,UpDownMark[0],1); //是否需要进行切换通道符号
	else Printf816(220,0,"  ",1);
	
	
	for(i=0; i<4; i++)
	{
		mids16[i] = MeterSetData.c_AO[KEY_XY[1][0]][i];	
	}
	if(1==MeterSetData.c_lcdlanguage) //西文
	{
		oy = 2;
		for(i=0; i<4; i++)
		{
			ox=Printf816(0,oy,LCD2AOTITLE2E[i],0);
			Printf816Lost(ox, oy, (40-ox),0);	
			oy += 2;
		}		
	}
	else //中文
	{
		oy = 2;
		for(i=0; i<4; i++)
		{
			Printf1616(0,oy,LCD2AOTITLE2C[i],0);
			oy += 2;
		}
	}
	//显示变送项目和数据
	if(1==MeterSetData.c_DASelect) Printf816(64,2,LCD2AOTITLE4V[mids16[0]],0);	//模式
	else Printf816(64,2,LCD2AOTITLE4[mids16[0]],0);	//模式
	if(0==mids16[0]) //OFF模式
	{
		Printf816(64,4,"         ",0);
		Printf816(64,6,"         ",0);
		Printf816(64,8,"         ",0);
	}
	else
	{
		Printf816(64,4,LCD2AOTITLE3[mids16[1]],0); //项目 
		if(mids16[1]<STEP_4_AOIA) m=1; //电压
		else if(mids16[1]<STEP_4_AOPA) m=3; //电流显示
		else if(mids16[1]<STEP_4_AOPFA) m=0;  //功率
		else if(mids16[1]<STEP_4_AOF) m=3; //功率因数
		else m=2; // 频率
		
		if(mids16[1]<STEP_4_AOIA) //电压
		{
			Printf816(112,6,"V",0); //	
			Printf816(112,8,"V",0); //	
		}
		else if(mids16[1]<STEP_4_AOPA) //电流显示
		{
			Printf816(112,6,"A",0); //	
			Printf816(112,8,"A",0); //	
		}
		else if(mids16[1]<STEP_4_AOQA)  //有功功率
		{
			Printf816(112,6,"W",0); //
			Printf816(112,8,"W",0); //	
		}
		else if(mids16[1]<STEP_4_AOSA)  //无功功率
		{
			Printf816(112,6,"var",0); //		
			Printf816(112,8,"var",0); //		
		}
		else if(mids16[1]<STEP_4_AOPFA)  //视在功率
		{
			Printf816(112,6,"VA",0); //		
			Printf816(112,8,"VA",0); //		
		}
		else if(mids16[1]<STEP_4_AOF) //
		{
			
		}
		else if(mids16[1]==STEP_4_AOF)	
		{
			Printf816(112,6,"Hz",0); //		
			Printf816(112,8,"Hz",0); //		
		}	
		Printf9999DotShadow(64,6,mids16[2],m,0); 	
		Printf9999DotShadow(64,8,mids16[3],m,0); 		
	}	
	
	if(ProgState>=STEP_3)
	{
		if(1==MeterSetData.c_lcdlanguage) //西文
		{
			oy = 2;
			for(i=0; i<4; i++)
			{
				if(i==KEY_XY[2][0])
				{	
					ox=Printf816(0,oy,LCD2AOTITLE2E[i],m);
					Printf816Lost(ox, oy, (40-ox),m);	
				}
				oy += 2;
			}
						
		}
		else //中文
		{
			oy = 2;
			for(i=0; i<4; i++)
			{
				if(i==KEY_XY[2][0]) m=1;
				else m=0;
				Printf1616(0,oy,LCD2AOTITLE2C[i],m);
				oy += 2;
			}
				
		}	
	}
	
	
	if(STEP_4 == ProgState) //在参数修改界面
	{
		if(STEP_2_AOMODE == KEY_XY[2][0])
		{
			if(1==MeterSetData.c_DASelect)  Printf816(64,2,LCD2AOTITLE4V[KEY_XY[3][0]],1);	//模式 
			else 
			{
				ox=Printf816(64,2,LCD2AOTITLE4[KEY_XY[3][0]],1);	//模式 
				Printf816Lost(ox, 2, (142-ox),1);	//增加
			}	
		}
		else if(STEP_2_AOITEM == KEY_XY[2][0])
		{
			ox= Printf816(64,4,LCD2AOTITLE3[KEY_XY[3][0]],1); //项目
			Printf816Lost(ox, 4, (92-ox),1);	//增加
		}	
		else
		{
			//小数点位置
			if(mids16[1]<STEP_4_AOIA) m=1; //电压
			else if(mids16[1]<STEP_4_AOPA) m=3; //电流显示
			else if(mids16[1]<STEP_4_AOPFA) m=0;  //功率
			else if(mids16[1]<STEP_4_AOF) m=3; //功率因数
			else m=2; // 频率
			
			if(STEP_2_AODS==KEY_XY[2][0]) Printf9999DotShadow(64,6,KEY_XY[3][0],m,(KeyMult+1)); 
			else if(STEP_2_AOFS==KEY_XY[2][0]) Printf9999DotShadow(64,8,KEY_XY[3][0],m,(KeyMult+1)); 
		}			
	}
	//画线
	oy=3;
	for(i=0; i<4; i++)
	{
		DrawLineX(0, oy, 239, 0x80);
		oy += 2;	
	}
	if(1==MeterSetData.c_lcdlanguage) DrawLineY(40,2,8,0xff);	
	else DrawLineY(32,2,8,0xff);	
			
}

/***********************************************************
报警参数界面

***********************************************************/
u8 const LCDAlarmTITLE1C[10][9]=
{
	{23,10,48,166,167,0}, //相电压报警
	{49,10,48,166,167,0}, //线电压报警
	{10,50,166,167,42,0}, //电流报警
	{73,15,166,167,42,0}, //频率报警
	{52,15,166,167,42,0}, //功率报警
	
	{52,15,55,56,166,167,42,42,0}, //功率因数报警
	{10,48,78,16,17,166,167,42,0}, //电压总谐波报警
	{10,50,78,16,17,166,167,42,0}, //电流总谐波报警
	{10,48,96,97,98,99,166,167,0}, //电压不平衡度报警
	{10,50,96,97,98,99,166,167,0}, //电流不平衡度报警
};

u8 const LCDALARMTITLE1E[10][14]=
{
	"Phase V Alarm",
	"Line V Alarm",
	"Current Aalrm",
	"Freq. Alarm", //Frequency
	"Power Alarm",
	
	"PF Alarm",
	"THDU Alarm",
	"THDI Alarm",
	"U Unb. Alarm",
	"I Unb. Alarm", //Unbalance
};

u8 const LCDALARMTITLE1EAll[10][24]= //全称显示
{
	"Phase Voltage Alarm",
	"Line Voltage Alarm",
	"Current Aalrm",
	"Frequency Alarm",
	"Power Alarm",
	
	"Power Factor Alarm",
	"THDU Alarm",
	"THDI Alarm",
	"Voltage Unbalance Alarm",
	"Current Unbalance Alarm", //Unbalance
};

u8 const LCDALARMTITLE2C[3][4]=
{
	{74,166,167,0}, //高报警
	{75,166,167,0},	//低报警
	{7,42,194,0}, //时间
};

u8 const LCDALARMTITLE3C[7][4]=
{
	{30,42,146,0},//模 式
	{166,167,27,0},//报警值
	{176,177,9,0}, //回滞量	
	{30,42,146,0},//模 式
	{166,167,27,0},//报警值
	{176,177,9,0}, //回滞量	
	{178,42,7,0}, //延 时
};

u8 const LCDALARMTITLE31C[4][4]=
{
	{30,42,146,0},//模 式
	{166,167,27,0},//报警值
	{176,177,9,0}, //回滞量	
	{178,42,7,0}, //延 时
};

u8 const LCDALARMTITLE3E[7][6]=
{
	"Mode",
	"Value",
	"Hys",	
	"Mode",
	"Value",
	"Hys",
	"Delay",
};

u8 const LCDALARMTITLE31E[4][6]=
{
	"Mode",
	"Value",
	"Hys",	
	"Delay",
};

u8 const LCDALARMOFFON[2][4]=
{
	"off",
	"on ",	
};

u8 const LCDALARMUNIT[10][3]=
{
	"V","V","A","Hz","W"," ","%","%","%","%",
};

//报警设置界面
void LCDProgramAlarm(void)
{
	u8 ox, oy, m;
	s16 i;
	s16 *ps16, mids16;
	u32 midu32;

	TopShadow(); //顶部阴影显示
	Printf1616(220,0,UpDownMark[0],1);
	//标题
	if(1==MeterSetData.c_lcdlanguage)
	{
		Printf816(0,0,LCDALARMTITLE1EAll[KEY_XY[1][0]],1);
		PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_ENTERE);
	}
	else
	{
		Printf1616(0,0,LCDAlarmTITLE1C[KEY_XY[1][0]],1);
		PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_ENTER);	
	}	
	
	if(KEY_XY[1][0]<STEP_2_ALARMPF) //高低报警参数
	{
		
		if(1==MeterSetData.c_lcdlanguage)
		{
			Printf816(0,3,"High",0);
			Printf816(0,5,"Alarm",0);
			Printf816(0,9,"Low",0);
			Printf816(0,11,"Alarm",0);
			Printf816(0,14,"Time",0);
			oy=2;
			for(i=0; i<7; i++)
			{
				ox=Printf816(64,oy,LCDALARMTITLE3E[i],m);	
				oy += 2;
			}
		}
		else
		{
			Printf1616(0,4,LCDALARMTITLE2C[0],0); //高报警
			Printf1616(0,10,LCDALARMTITLE2C[1],0); //低报警
			Printf1616(0,14,LCDALARMTITLE2C[2],0); //时间
			oy=2;
			for(i=0; i<7; i++)
			{
				Printf1616(64,oy,LCDALARMTITLE3C[i],m);	
				oy += 2;
			}
			
		}
		
		//数据显示
		//高报警模式
		midu32 = MeterSetData.c_AlarmEnable[1];
		midu32 = (midu32<<16)+MeterSetData.c_AlarmEnable[0];
		mids16 = (midu32>>(KEY_XY[1][0]*2))&1; 	//on off	
		Printf816(128, 2,LCDALARMOFFON[mids16],0);
		//低报警模式
		mids16 = (midu32>>(KEY_XY[1][0]*2+1))&1; 		
		Printf816(128, 8,LCDALARMOFFON[mids16],0);
					
		if(KEY_XY[1][0]<STEP_2_ALARMI) m=1; //电压值
		else if(KEY_XY[1][0]<STEP_2_ALARMF) m=3; //电流值
		else if(KEY_XY[1][0]<STEP_2_ALARMP) m=2; //F
		else if(KEY_XY[1][0]<STEP_2_ALARMPF) m=0; //P
		else m=0;
		ps16=(s16 *)&MeterSetData.c_AlarmUn[0];
		ps16= ps16+5*KEY_XY[1][0];
		Printf9999DotShadow(128,4,*ps16,m,0); 	//高报警值
		Printf9999DotShadow(128,6,*(ps16+1),m,0);  //回滞量
		Printf9999DotShadow(128,10,*(ps16+2),m,0); 	   //低报警值
		Printf9999DotShadow(128,12,*(ps16+3),m,0); 	//回滞量
		Printf9999(128,14,*(ps16+4),0); //延时时间
		oy=4;
		for(i=0; i<4; i++)
		{
			Printf816(176,oy,LCDALARMUNIT[KEY_XY[1][0]],0);
			oy+=2;
			if(1==i) oy+=2;
		}	
		Printf816(176,14,"s",0);
	
		//画线
		DrawLineX(0, 7, 64, 0x80);
		DrawLineX(0, 13, 64, 0x80);
		DrawLineX(0, 15, 64, 0x80);
		oy=3;
		for(i=0; i<7; i++)
		{
			DrawLineX(64, oy, 175, 0x80);
			oy+=2;
		}
		
		DrawLineY(63,2,14,0xff);	
		DrawLineY(112,2,14,0xff);
	}
	else //只有一个报警，低报警 高报警
	{
		if(1==MeterSetData.c_lcdlanguage)
		{
			if(STEP_2_ALARMPF==KEY_XY[1][0]) Printf816(0,3,"Low",0); 
			else Printf816(0,3,"High",0);
			Printf816(0,5,"Alarm",0);
			Printf816(0,8,"Time",0);
			oy=2;
			for(i=0; i<4; i++)
			{
				ox=Printf816(64,oy,LCDALARMTITLE31E[i],m);	
				oy += 2;
			}
		}
		else
		{
			if(STEP_2_ALARMPF==KEY_XY[1][0]) Printf1616(0,4,LCDALARMTITLE2C[1],0); //低报警
			else Printf1616(0,4,LCDALARMTITLE2C[0],0); //高报警
			Printf1616(0,8,LCDALARMTITLE2C[2],0); //时间
			oy=2;
			for(i=0; i<4; i++)
			{
				Printf1616(64,oy,LCDALARMTITLE31C[i],m);	
				oy += 2;
			}
		}
		
		//数据显示
		//模式
		midu32 = MeterSetData.c_AlarmEnable[1];
		midu32 = (midu32<<16)+MeterSetData.c_AlarmEnable[0];
		mids16 = (midu32>>(KEY_XY[1][0]+9))&1; 	//on off	
		Printf816(128, 2,LCDALARMOFFON[mids16],0);
		
		if(KEY_XY[1][0]<STEP_2_ALARMTHDU) m=3; //功率因数
		else if(KEY_XY[1][0]<STEP_2_ALARMUUN) m=2; //总谐波畸变率
		else m=1;	//不平衡度
		ps16=(s16 *)&MeterSetData.c_AlarmPF[0];
		ps16= ps16+3*(KEY_XY[1][0]-STEP_2_ALARMPF);
		Printf9999DotShadow(128,4,*ps16,m,0); 	//高报警值
		Printf9999DotShadow(128,6,*(ps16+1),m,0);  //回滞量
		Printf9999(128,8,*(ps16+2),0); //延时时间
		oy=4;
		for(i=0; i<2; i++)
		{
			Printf816(176,oy,LCDALARMUNIT[KEY_XY[1][0]],0);
			oy+=2;
		}	
		Printf816(176,8,"s",0);
	
		//画线
		DrawLineX(0, 7, 64, 0x80);
		DrawLineX(0, 9, 64, 0x80);
		oy=3;
		for(i=0; i<4; i++)
		{
			DrawLineX(64, oy, 175, 0x80);
			oy+=2;
		}
		
		DrawLineY(63,2,8,0xff);	
		DrawLineY(112,2,8,0xff);	
	}
	
	if(ProgState>=STEP_3)
	{
		if(KEY_XY[1][0]<STEP_2_ALARMPF) //高低报警参数
		{
			if(1==MeterSetData.c_lcdlanguage)
			{
				
				oy=2;
				for(i=0; i<7; i++)
				{
					if(i==KEY_XY[2][0])
					{	
						ox=Printf816(64,oy,LCDALARMTITLE3E[i],1);	
						Printf816Lost(ox, oy, (112-ox),1);
					}
					oy += 2;
				}
			}
			else
			{
				
				oy=2;
				for(i=0; i<7; i++)
				{
					if(i==KEY_XY[2][0])
					{
						Printf1616(64,oy,LCDALARMTITLE3C[i],1);
					}
					oy += 2;
				}
				
			}
			//报警数据显示
			
			
		}
		else if(KEY_XY[1][0]<STEP_2_ALARMTHDU) //低报警
		{
			if(1==MeterSetData.c_lcdlanguage)
			{
				
				oy=2;
				for(i=0; i<4; i++)
				{
					if(i==KEY_XY[2][0])
					{	
						ox=Printf816(64,oy,LCDALARMTITLE3E[i+3],1);	
						Printf816Lost(ox, oy, (112-ox),1);
						break;
					}
//					if(0==i) oy+=4; //跳到低报警部分
					oy += 2;
				}
			}
			else
			{
				
				oy=2;
				for(i=0; i<4; i++)
				{
					if(i==KEY_XY[2][0])
					{
						Printf1616(64,oy,LCDALARMTITLE3C[i+3],1);
						break;
					}
//					if(0==i) oy+=4; //跳到低报警部分
					oy += 2;
				}
			}
		}
		else //高报警
		{
			if(1==MeterSetData.c_lcdlanguage)
			{
				
				oy=2;
				for(i=0; i<4; i++)
				{
					if(i==KEY_XY[2][0])
					{	
						ox=Printf816(64,oy,LCDALARMTITLE3E[i],1);	
						Printf816Lost(ox, oy, (112-ox),1);
						break;
					}
					oy += 2;
				}
			}
			else
			{
				
				oy=2;
				for(i=0; i<4; i++)
				{
					if(i==KEY_XY[2][0])
					{
						Printf1616(64,oy,LCDALARMTITLE3C[i],1);
						break;
					}
					oy += 2;
				}
				
			}
		}	
	}	

			
	
	
	//在修改参数界面
	if(STEP_4==ProgState)
	{	
		if(KEY_XY[1][0]<STEP_2_ALARMPF) //高低报警参数
		{
			//数据显示
			if(KEY_XY[1][0]<STEP_2_ALARMI) m=1; //电压值
			else if(KEY_XY[1][0]<STEP_2_ALARMF) m=3; //电流值
			else if(KEY_XY[1][0]<STEP_2_ALARMP) m=2; //F
			else if(KEY_XY[1][0]<STEP_2_ALARMPF) m=0; //P
			else m=0;	
			//高报警模式
			if(STEP_3_ALARMMODE==KEY_XY[2][0]) Printf816(128, 2,LCDALARMOFFON[KEY_XY[3][0]],1);	
			else if(STEP_3_ALARM2MODE==KEY_XY[2][0]) Printf816(128, 8,LCDALARMOFFON[KEY_XY[3][0]],1); //低报警模式	
			else if(STEP_3_ALARMVALUEH==KEY_XY[2][0]) Printf9999DotShadow(128,4,KEY_XY[3][0],m,KeyMult+1); 	//高报警值
			else if(STEP_3_ALARMVALUEHYSH==KEY_XY[2][0]) Printf9999DotShadow(128,6,KEY_XY[3][0],m,KeyMult+1);  //回滞量
			else if(STEP_3_ALARMVALUEL==KEY_XY[2][0])Printf9999DotShadow(128,10,KEY_XY[3][0],m,KeyMult+1); 	   //低报警值
			else if(STEP_3_ALARMVALUEHYS==KEY_XY[2][0])Printf9999DotShadow(128,12,KEY_XY[3][0],m,KeyMult+1); 	//回滞量
			else if(STEP_3_ALARMDELAY==KEY_XY[2][0])Printf9999(128,14,KEY_XY[3][0],KeyMult+1); //延时时间
			//
			if((STEP_3_ALARMMODE==KEY_XY[2][0])||(STEP_3_ALARM2MODE==KEY_XY[2][0]))
			{
				if(1==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_OKE);
				else PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_OK);	
			}
			else
			{
				if(1==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_LEFT,K_UP,K_ESCE,K_OKE);
				else PrintfKeyF4(K_LEFT,K_UP,K_ESC,K_OK);	
			}		 
		}	
		else //只有一个报警
		{
			//数据显示
			if(KEY_XY[1][0]<STEP_2_ALARMTHDU) m=3; //功率因数
			else if(KEY_XY[1][0]<STEP_2_ALARMUUN) m=2; //总谐波畸变率
			else m=1;	//不平衡度
			//模式
			if(STEP_31_ALARMMODE==KEY_XY[2][0]) Printf816(128, 2,LCDALARMOFFON[KEY_XY[3][0]],1);
			else if(STEP_31_ALARMVALUEH==KEY_XY[2][0]) Printf9999DotShadow(128,4,KEY_XY[3][0],m,KeyMult+1); 	//高报警值
			else if(STEP_31_ALARMVALUEHYSH==KEY_XY[2][0]) Printf9999DotShadow(128,6,KEY_XY[3][0],m,KeyMult+1);  //回滞量
			else if(STEP_31_ALARMDELAY==KEY_XY[2][0]) Printf9999(128,8,KEY_XY[3][0],KeyMult+1); //延时时间
						
			if(STEP_31_ALARMMODE==KEY_XY[2][0])
			{
				if(1==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_OKE);
				else PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_OK);	
			}
			else
			{
				if(1==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_LEFT,K_UP,K_ESCE,K_OKE);
				else PrintfKeyF4(K_LEFT,K_UP,K_ESC,K_OK);	
			}
		}
		
		
			
	}
	//画线
	if(KEY_XY[1][0]<STEP_2_ALARMPF) //高低报警参数
	{
		//画线
		DrawLineX(0, 7, 64, 0x80);
		DrawLineX(0, 13, 64, 0x80);
		DrawLineX(0, 15, 64, 0x80);
		oy=3;
		for(i=0; i<7; i++)
		{
			DrawLineX(64, oy, 175, 0x80);
			oy+=2;
		}
		
		DrawLineY(63,2,14,0xff);	
		DrawLineY(112,2,14,0xff);
	}		
	else 
	{
		//画线
		DrawLineX(0, 7, 64, 0x80);
		DrawLineX(0, 9, 64, 0x80);
		oy=3;
		for(i=0; i<4; i++)
		{
			DrawLineX(64, oy, 175, 0x80);
			oy+=2;
		}
		
		DrawLineY(63,2,8,0xff);	
		DrawLineY(112,2,8,0xff);	
	}		
		
}

			
u8 const LCDTOUTITLE1C [5][5]=
{
	{191,14,15,160,0}, //"日费率一",
	{191,14,15,161,0},//"日费率二",
	{63,14,15,42,0},//"月费率 ",
	{66,67,7,194,0},//"抄表时间",
	{38,39,7,194,0},//"系统时间  ",	
};
u8 const LCDTOUTITLE1E[5][14]=
{
	"#1 Day Tariff",	
	"#2 Day Tariff",
	"Month Tariff",
	"AMR Time  ",
	"System Time",
};

u8 const LCDTOUFEENUM[12][3]=
{
	"01","02","03","04","05","06",
	"07","08","09","10","11","12",	
};

u8 const LCDTOUFEETITLE1C[2][3]=
{
	{7,194,0}, //时间
	{14,15,0}, //费率	
};

u8 const LCDTOUFEETITLE2C[4][2]=
{
	{58,0}, //尖	
	{59,0}, //峰
	{97,0}, //平
	{60,0}, //谷
};
u8 const LCDTOUFEETITLE2E[4][3]=
{
	"T1","T2","T3","T4",
};

u8 const LCDTOUMONTHTITLC1[2][5]=
{
	{63,68,0}, //月份
	{191,14,15,0}, //日费率	
} ;

u8 const LCDTOUMONTHTITLE1[2][11]=
{
	"Month", "Day Tariff",
};

u8 const LCDTOUMONTHTITLC2[2][4]=
{
	{14,15,160,0}, //费率一
	{14,15,161,0}, //费率二
};

u8 const LCDTOUMONTHTITLE2[2][3]=
{
	"#1",
	"#2",	
};

//抄表日
u8 const LCDTOUCOPYDAYTITLC1[2][2]=
{
	{191,0},
	{7,0},
};

void LCDProgramTOU(void)
{
	u8 ox, oy,i,m;
	if(ProgState<STEP_3) 
	{
		if(1==MeterSetData.c_lcdlanguage)
		{
			oy = 2;
			for(i=0; i<5; i++)
			{
				if(i==KEY_XY[1][0]) m=1;
				else m = 0;
				ox=Printf816(0,oy,LCDTOUTITLE1E[i], m); //01/02
				Printf816Lost(ox, oy, (104-ox),m);	
				oy += 2;
			}	
		}
		else
		{
			oy = 2;
			for(i=0; i<5; i++)
			{
				if(i==KEY_XY[1][0]) m=1;
				else m = 0;
				Printf1616(0,oy,LCDTOUTITLE1C[i], m); //01/02
				oy += 2;
			}	
		} 
		
		
		//画线
		oy=3;
		for(i=0; i<5; i++)
		{
			DrawLineX(0, oy, 239, 0x80);
			oy += 2;	
		}
		if(1==MeterSetData.c_lcdlanguage) DrawLineY(104,2,10,0xff);
		else DrawLineY(64,2,10,0xff);
		
		if(1==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_EDITE);
		else PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_EDIT);

	}
	else //在STEP_3/4/5界面
	{
		//在第一行显示对应对内容
		if(1==MeterSetData.c_lcdlanguage)
		{
			Printf816(0,0,LCDTOUTITLE1E[KEY_XY[1][0]],0);	
		}
		else
		{
			Printf1616(0,0,LCDTOUTITLE1C[KEY_XY[1][0]],0);
		}	
		
		if((STEP_2_FSD1==KEY_XY[1][0])||(STEP_2_FSD2==KEY_XY[1][0])) //
		{
			Printf816(1,2, "No.", 0); //标题部分
			Printf816(121,2,"No.",0);
			if(1==MeterSetData.c_lcdlanguage)
			{
				Printf816(32,2,"Time",0);
				Printf816(152,2,"Time",0);
				Printf816(78,2,"Tariff",0);	
				Printf816(198,2,"Tariff",0);
				
			}
			else 
			{
				Printf1616(32,2,LCDTOUFEETITLE1C[0],0); //时间
				Printf1616(152,2,LCDTOUFEETITLE1C[0],0); //时间
				Printf1616(80,2,LCDTOUFEETITLE1C[1],0); //费率
				Printf1616(200,2,LCDTOUFEETITLE1C[1],0); //费率
			}
			//01-12时段选择
			oy=4;
			for(i=0; i<6; i++)
			{
				if(i==KEY_XY[2][0]) m=1;
				else m=0;
				Printf816(4,oy,LCDTOUFEENUM[i],m);				
				oy+=2;
			}		
			oy=4;
			for(i=6; i<12; i++)
			{
				if(i==KEY_XY[2][0]) m=1;
				else m=0;
				Printf816(124,oy,LCDTOUFEENUM[i],m);
				oy+=2;
			}
			//对应时段对时间和费率
			oy=4;
			for(i=0; i<6; i++)
			{
				Printf99(28, oy, sd_Data.c_sd[KEY_XY[1][0]-STEP_2_FSD1][i][0],0);
				Printf99(52, oy, sd_Data.c_sd[KEY_XY[1][0]-STEP_2_FSD1][i][1],0);	
				Printf816(44,oy,":",0);
				if(1==MeterSetData.c_lcdlanguage) //费率类型
				{
					Printf816(88, oy,LCDTOUFEETITLE2E[sd_Data.c_type[KEY_XY[1][0]-STEP_2_FSD1][i]],0);
				}
				else
				{
					Printf1616(88, oy,LCDTOUFEETITLE2C[sd_Data.c_type[KEY_XY[1][0]-STEP_2_FSD1][i]],0);
				}		
				oy += 2;
			}
			
			oy=4;
			for(i=6; i<12; i++)
			{
				Printf99(148, oy, sd_Data.c_sd[KEY_XY[1][0]-STEP_2_FSD1][i][0],0);
				Printf99(172, oy, sd_Data.c_sd[KEY_XY[1][0]-STEP_2_FSD1][i][1],0);	
				Printf816(164,oy,":",0);
				if(1==MeterSetData.c_lcdlanguage) //费率类型
				{
					Printf816(208, oy,LCDTOUFEETITLE2E[sd_Data.c_type[KEY_XY[1][0]-STEP_2_FSD1][i]],0);
				}
				else
				{
					Printf1616(208, oy,LCDTOUFEETITLE2C[sd_Data.c_type[KEY_XY[1][0]-STEP_2_FSD1][i]],0);
				}
				oy += 2;
			}
			if(STEP_4==ProgState) //复费率选择
			{
				//对应时段对时间和费率
				oy=4;
				for(i=0; i<6; i++)
				{
					if(i==KEY_XY[2][0])
					{	
						if(0==KEY_XY[3][0]) Printf99(28, oy, sd_Data.c_sd[KEY_XY[1][0]-STEP_2_FSD1][i][0],1);
						else if(1==KEY_XY[3][0]) Printf99(52, oy, sd_Data.c_sd[KEY_XY[1][0]-STEP_2_FSD1][i][1],1);	
						else
						{
							if(1==MeterSetData.c_lcdlanguage) //费率类型
							{
								Printf816(88, oy,LCDTOUFEETITLE2E[sd_Data.c_type[KEY_XY[1][0]-STEP_2_FSD1][i]],1);
							}
							else
							{
								Printf1616(88, oy,LCDTOUFEETITLE2C[sd_Data.c_type[KEY_XY[1][0]-STEP_2_FSD1][i]],1);
							}
						}
						break;	
					}	
					oy += 2;
				}
				
				oy=4;
				for(i=6; i<12; i++)
				{
					if(i==KEY_XY[2][0])
					{	
						if(0==KEY_XY[3][0]) Printf99(148, oy, sd_Data.c_sd[KEY_XY[1][0]-STEP_2_FSD1][i][0],1);
						else if(1==KEY_XY[3][0]) Printf99(172, oy, sd_Data.c_sd[KEY_XY[1][0]-STEP_2_FSD1][i][1],1);	
						else
						{
							if(1==MeterSetData.c_lcdlanguage) //费率类型
							{
								Printf816(208, oy,LCDTOUFEETITLE2E[sd_Data.c_type[KEY_XY[1][0]-STEP_2_FSD1][i]],1);
							}
							else
							{
								Printf1616(208, oy,LCDTOUFEETITLE2C[sd_Data.c_type[KEY_XY[1][0]-STEP_2_FSD1][i]],1);
							}
						}
					}
					oy += 2;
				}	
			}
			
			//第五级界面
			if(STEP_5==ProgState)
			{
				//对应时段对时间和费率
				oy=4;
				for(i=0; i<6; i++)
				{
					if(i==KEY_XY[2][0])
					{	
						if(0==KEY_XY[3][0]) Printf99(28, oy, KEY_XY[4][0],1);
						else if(1==KEY_XY[3][0]) Printf99(52, oy, KEY_XY[4][0],1);	
						else
						{
							if(1==MeterSetData.c_lcdlanguage) //费率类型
							{
								Printf816(88, oy,LCDTOUFEETITLE2E[KEY_XY[4][0]],1);
							}
							else
							{
								Printf1616(88, oy,LCDTOUFEETITLE2C[KEY_XY[4][0]],1);
							}
						}
						break;	
					}	
					oy += 2;
				}
				
				oy=4;
				for(i=6; i<12; i++)
				{
					if(i==KEY_XY[2][0])
					{	
						if(0==KEY_XY[3][0]) Printf99(148, oy, KEY_XY[4][0],1);
						else if(1==KEY_XY[3][0]) Printf99(172, oy, KEY_XY[4][0],1);	
						else
						{
							if(1==MeterSetData.c_lcdlanguage) //费率类型
							{
								Printf816(208, oy,LCDTOUFEETITLE2E[KEY_XY[4][0]],1);
							}
							else
							{
								Printf1616(208, oy,LCDTOUFEETITLE2C[KEY_XY[4][0]],1);
							}
						}
					}
					oy += 2;
				}	
			}		
		
			//横线
			oy=3;
			for(i=0; i<7; i++)
			{
				DrawLineX(0, oy, 239, 0x80);
				oy += 2;	
			}
			DrawLineY(24,2,14,0xff);
			DrawLineY(72,2,14,0xff);
			DrawLineY(120,2,14,0xff);
			DrawLineY(144,2,14,0xff);
			DrawLineY(192,2,14,0xff);
			
			//	
			if(STEP_3==ProgState) 
			{
				if(1==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_EDITE);
				else PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_EDIT);
			}
			else if(STEP_4==ProgState)
			{
				if(1==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_LEFT,K_RIGHT,K_ESCE,K_EDITE);
				else PrintfKeyF4(K_LEFT,K_RIGHT,K_ESC,K_EDIT);
			}
			else if(STEP_5==ProgState) 
			{
				if(1==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_OKE);
				else PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_OK);
			}	
		}	
		else if(STEP_2_MONS==KEY_XY[1][0]) //月费率选择
		{
			//标题
			if(1==MeterSetData.c_lcdlanguage)
			{
				Printf816(8,2,LCDTOUMONTHTITLE1[0],0);
				Printf816(56,2,LCDTOUMONTHTITLE1[1],0);
				Printf816(128,2,LCDTOUMONTHTITLE1[0],0);
				Printf816(176,2,LCDTOUMONTHTITLE1[1],0);
			}	
			else
			{
				Printf1616(12,2,LCDTOUMONTHTITLC1[0],0);
				Printf1616(64,2,LCDTOUMONTHTITLC1[1],0);
				Printf1616(132,2,LCDTOUMONTHTITLC1[0],0);
				Printf1616(188,2,LCDTOUMONTHTITLC1[1],0);
			}
			//月份
			oy=4;
			for(i=0; i<6; i++)
			{
				if(i==KEY_XY[2][0]) m=1;
				else m=0;
				Printf816(20,oy,LCDTOUFEENUM[i],m);
				oy+=2;
			}		
			oy=4;
			for(i=6; i<12; i++)
			{
				if(i==KEY_XY[2][0]) m=1;
				else m=0;
				Printf816(140,oy,LCDTOUFEENUM[i],m);
				oy+=2;
			}
			//每月费率选择
			if(1==MeterSetData.c_lcdlanguage)
			{
				oy=4;
				for(i=0; i<6; i++) //
				{
					Printf816(80,oy, LCDTOUMONTHTITLE2[sd_Data.c_mons[i]],0);
					oy += 2;	
				}	
				oy=4;
				for(i=6; i<12; i++)
				{
					Printf816(200,oy, LCDTOUMONTHTITLE2[sd_Data.c_mons[i]],0);
					oy += 2;	
				}
				
			}
			else //
			{
				oy=4;
				for(i=0; i<6; i++) //
				{
					Printf1616(64,oy, LCDTOUMONTHTITLC2[sd_Data.c_mons[i]],0);
					oy += 2;	
				}	
				oy=4;
				for(i=6; i<12; i++)
				{
					Printf1616(188,oy, LCDTOUMONTHTITLC2[sd_Data.c_mons[i]],0);
					oy += 2;	
				}
			}	
			
			if(STEP_4==ProgState) //
			{
				if(1==MeterSetData.c_lcdlanguage)
				{
					oy=4;
					for(i=0; i<6; i++) //
					{
						
						if(i==KEY_XY[2][0]) Printf816(80,oy, LCDTOUMONTHTITLE2[KEY_XY[3][0]],1);
						oy += 2;	
					}	
					oy=4;
					for(i=6; i<12; i++)
					{
						if(i==KEY_XY[2][0]) Printf816(200,oy, LCDTOUMONTHTITLE2[KEY_XY[3][0]],1);
						oy += 2;	
					}
					
				}
				else //
				{
					oy=4;
					for(i=0; i<6; i++) //
					{
						if(i==KEY_XY[2][0]) Printf1616(64,oy, LCDTOUMONTHTITLC2[KEY_XY[3][0]],1);
						oy += 2;	
					}	
					oy=4;
					for(i=6; i<12; i++)
					{
						if(i==KEY_XY[2][0]) Printf1616(188,oy, LCDTOUMONTHTITLC2[KEY_XY[3][0]],1);
						oy += 2;	
					}
				}	
			}	
				
			//横线
			oy=3;
			for(i=0; i<7; i++)
			{
				DrawLineX(0, oy, 239, 0x80);
				oy += 2;	
			}
			DrawLineY(55,2,14,0xff);
			DrawLineY(120,2,14,0xff);
			DrawLineY(175,2,14,0xff);
			
			if(STEP_3==ProgState) 
			{
				if(1==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_EDITE);
				else PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_EDIT);
			}
			else if(STEP_4==ProgState)
			{
				if(1==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_OKE);
				else PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_OK);
			}
		}	
		else if(STEP_2_COPY==KEY_XY[1][0]) //抄表日
		{
			if(1==MeterSetData.c_lcdlanguage) 
			{
				Printf816(88,8,"Day",0);
				Printf816(152,8,"Hour",0);	
			}
			else
			{
				Printf1616(88,8,LCDTOUCOPYDAYTITLC1[0],0);
				Printf1616(152,8,LCDTOUCOPYDAYTITLC1[1],0);	
			}	
			if(0==KEY_XY[2][0]) m=1;
			else m=0;
			Printf99(64,8,sd_Data.c_copyday[0],m);
			if(0==KEY_XY[2][0]) m=0;
			else m=1;
			Printf99(128,8,sd_Data.c_copyday[1],m);
			
			if(STEP_4==ProgState)
			{
				if(0==KEY_XY[2][0])
				{
					Printf99(64,8,KEY_XY[3][0],1);
				}
				else if(1==KEY_XY[2][0])
				{
					Printf99(128,8,KEY_XY[3][0],1);
				}		
			}
			if(STEP_3==ProgState) 
			{
				if(1==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_LEFT,K_RIGHT,K_ESCE,K_EDITE);
				else PrintfKeyF4(K_LEFT,K_RIGHT,K_ESC,K_EDIT);
			}
			else if(STEP_4==ProgState)
			{
				if(1==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_OKE);
				else PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_OK);
			}	
			
		}	
		else if(STEP_2_TIME==KEY_XY[1][0]) //时间
		{
			ox=88;
			Printf816(104,6, "-",0);
			Printf816(128,6,"-",0);
			Printf816(105,9, ":",0);
			Printf816(129,9,":",0);
			for(i=0; i<3; i++)
			{
				if(i==KEY_XY[2][0]) m=1;
				else m=0;
				Printf99(ox, 6, sd_Data.c_time[i], m);
				if((i+3)==KEY_XY[2][0]) m=1;
				else m=0;
				Printf99(ox, 9, sd_Data.c_time[i+3], m);
				ox +=24;
			}
			//在第四级菜单中
			if(STEP_4==ProgState)
			{	
				ox=88;
				for(i=0; i<3; i++)
				{
					if(i==KEY_XY[2][0]) Printf99(ox, 6, KEY_XY[3][0], 1);
					if((i+3)==KEY_XY[2][0]) Printf99(ox, 9, KEY_XY[3][0], 1);
					ox += 24;
				}
			}
			if(STEP_3==ProgState) 
			{
				if(1==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_LEFT,K_RIGHT,K_ESCE,K_EDITE);
				else PrintfKeyF4(K_LEFT,K_RIGHT,K_ESC,K_EDIT);
			}
			else if(STEP_4==ProgState)
			{
				if(1==MeterSetData.c_lcdlanguage) PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_OKE);
				else PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_OK);
			}
			
		}	
	}			
}


u8 const LCDPRCLRTITLE1E[7][13]=
{
	"Reset Energy","Reset Demand",
	"Reset MaxMin","Reset Event ",
};
u8 const LCDPRCLRTITLE1C[7][5]=
{
	{113,201,10,11,0},	//清除电能
	{113,201,25,9,0}, 	//清除需量
	{113,201,26,27,0}, 	//清除极值
	{113,201,32,33,0}, 	//清除事件
};
u8 const LCDPRCLRTITLE2E[2][2]=
{
	{202,0},	//选择空
	{203,0}		//选择中	
};
/**************************************************************************
Name: void LCDProgramClr(void)
Function: 编程界面清零操作显示
Input:  None
Output: None
**************************************************************************/
void LCDProgramClr(void)
{
	u8 i,oy,ox, oxk;
	if(1==MeterSetData.c_lcdlanguage)
	{
		oy = 2;
		for(i=0; i<3; i++)
		{
			if(i==KEY_XY[1][0]) 
			{
				ox=Printf816(0, oy,LCDPRCLRTITLE1E[i],1);	
				oxk=1;
			}
			else 
			{
				ox=Printf816(0, oy,LCDPRCLRTITLE1E[i],0);					
				oxk=0;
			}	
			Printf816Lost(ox, oy, (96-ox),oxk);	
			oy += 2;
		}
		DrawLineY(96,2,6,0xff);
	}
	else
	{
		oy = 2;
		for(i=0; i<3; i++)
		{
			if(i==KEY_XY[1][0]) Printf1616(0, oy,LCDPRCLRTITLE1C[i],1);	
			else Printf1616(0, oy,LCDPRCLRTITLE1C[i],0);	
			oy += 2;
		}
		DrawLineY(64,2,6,0xff);	
	}
	//方框显示
	oy = 2;
	for(i=0; i<3; i++)
	{
		Printf1616(148,oy,LCDPRCLRTITLE2E[KeyClr[i]],0);	
		oy += 2;
	}
	if(STEP_3==ProgState)
	{
		if(MeterSetData.c_lcdlanguage) PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_OKE);
		else PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_OK);
		oy = 2 + 2*KEY_XY[1][0];
		Printf1616(148,oy,LCDPRCLRTITLE2E[KEY_XY[2][0]],11);
	}	
	//画线	
	oy=3;
	for(i=0; i<3; i++)
	{
		DrawLineX(0, oy, 239, 0x80);
		oy += 2;	
	}			
}


u8 const LCDSAVETITLE1[1][5]=
{
	122,123,40,41,0,//保存设置	
};
u8 const LCDSAVETITLE2[2][2]=
{
	{124,0},
	{125,0},	
};
/***************************************************************************
Name: void LCDProgramSave(void)
Function: 系统退出界面
Input:  None
Output: None
***************************************************************************/
void LCDProgramSave(void)
{
	if(1==MeterSetData.c_lcdlanguage) //英文
	{
		Printf816(64,7,"Save Settings?",0);
		Printf816(64,9,"Yes",0);
		Printf816(64,11,"No",0);		
		PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_OKE);	//四个按键
	}
	else //中文
	{
		Printf1616(64,7,(u8 *)LCDSAVETITLE1[0],0); //保存设置
		Printf1616(64,9,(u8 *)LCDSAVETITLE2[0],0);  //是
		Printf1616(64,11,(u8 *)LCDSAVETITLE2[1],0);
		PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_OK);	//四个按键
	}	
	if(0 == KEY_XY[2][0]) PrintfBMP3216(96,9,2); //手型符号
	else PrintfBMP3216(96,11,2);
		
}






/**************************************************************
Name: void LCDDisplayCalibrate(void)
Function: 校准菜单
**************************************************************/
void LCDDisplayCalibrate(void)
{
	switch(CalState)
	{
		case STEP_1:	
			LCDCalStep1();
			break;
		case STEP_2:
		case STEP_3:
			LCDCalStep2();
			break;
		case STEP_SAVE:
			LCDProgramSave();
			break;
		default:
			break;
	}	
}

u8 const LCDCALTITLE1[6][4]=
{
	"CAL","AO1","AO2","AOS","MOD","LIM"	
};
//校准第一级菜单
void LCDCalStep1(void)
{
	u8 ox, oy, i,m;
	Printf816(48, 0, "System Cal", 1);
	oy = 2;
	for(i=0; i<6; i++)
	{
		m=0;
		if(i==KEY_XY[0][0]) m=1;
		ox=Printf816(0,oy,LCDCALTITLE1[i],m);
		Printf816Lost(ox, oy, (32-ox),m);
		oy += 2;	
	}
	PrintfKeyF4(K_UP,K_DOWN,K_ESCE,K_ENTERE);	//四个按键
	//画线
	DrawLineX(0,1,239,0x80);
	oy=3;
	for(i=0; i<6; i++)
	{
		DrawLineX(0, oy, 32, 0x80);
		oy += 2;	
	}	
	DrawLineY(32,2,12,0xff);		
}

//校准第二级菜单
void LCDCalStep2(void)
{
	switch(KEY_XY[0][0])
	{
		case CAL_1_CAL:
			LCDUICal();
			break;
		case CAL_1_AO1:
		case CAL_1_AO2:
			LCDCalAo();
			break;
		case CAL_1_AOS:
			LCDCalAoS();
			break;
		case CAL_1_MENABLE:
			LCDCalMEn();
			break;
		case CAL_1_LIMMIT:
			LCDCalLimmit();
			break;
		default:
			break;	
	}
	DrawLineX(0,1,239,0x80);	
}

u8 const LCDUICALTITLE[6][5]=
{
	"Clr ","UI  ","P   ", "Ph90","Ph60","Save"	
};
u8 const LCDUICALTITLE2[12][5]=
{
	"Ua:","Ub:","Uc:","Ia:","Ib:","Ic:",
	" Pa:"," Pb:"," Pc:","PFa:","PFb:","PFc:",
}; 
//电量校准
void LCDUICal(void)
{
	u8 ox,oy, i,k;
	Printf816(48, 0, "System Cal(380V/5A)", 1);
	PrintfKeyF4(K_NO,K_NO,K_ESCE,K_ENTERE);	//四个按键
	oy = 2;
	for(i=0; i<6; i++)
	{
		k=0;
		if(i==KEY_XY[1][0]) k=1;
		ox=Printf816(0, oy, LCDUICALTITLE[i],k);
		Printf816Lost(ox, oy, (32-ox),k);
		oy += 2;	
	}	
	oy=3;
	for(i=0; i<6; i++)
	{
		DrawLineX(0, oy, 32, 0x80);
		oy += 2;	
	}	
	DrawLineY(32,2,12,0xff);
	
	oy=3;	
	for(i=0; i<6; i++)
	{
		Printf816(48, oy, LCDUICALTITLE2[i],0);	
		Printf816(144, oy, LCDUICALTITLE2[i+6],0);	
		oy+=2;
	}
	oy=3;
	for(i=0; i<3; i++)
	{
		Printf9999DotShadow(72,oy,MeterData.Un[i],1,0);
		Printf9999DotShadow(72,oy+6,(u16)MeterData.I[i],3,0);	
		oy += 2;
	}
	
	oy=3;
	for(i=0; i<3; i++)
	{
		Printf9999DotShadow(176,oy,abss16(MeterData.P[i]),0,0);
		Printf9999DotShadow(176,oy+6,abss16(MeterData.PF[i]),3,0);	
		oy += 2;	
	}
	
	
	
		
	
}

//变送校准界面
void LCDCalAo(void)
{
	u8 oy, i, m;
	Printf816(0, 0, "AO  Cal", 0);
	Printf9(21,0,(KEY_XY[0][0]-CAL_1_AO1+1),0);
	if(0==KEY_XY[1][0]) m=1;
	else m=0;
	Printf816(0,2," 4mA",m);
	if(1==KEY_XY[1][0]) m=1;
	else m=0;
	Printf816(0,4,"20mA",m);
	
	PrintfFFFFHex(64, 2, AdjustAo[KEY_XY[0][0]-CAL_1_AO1][0],0);
	PrintfFFFFHex(64, 4, AdjustAo[KEY_XY[0][0]-CAL_1_AO1][1],0);	
	PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_ENTER);	//四个按键
	
	if(STEP_3 == CalState)
	{
		PrintfKeyF4(K_LEFT,K_UP,K_DOWN,K_OK);	//四个按键
		oy = 2;
		for(i=0; i<2; i++)
		{
			if(i==KEY_XY[1][0]) PrintfFFFFHex(64, oy, KEY_XY[2][0], KeyMult+1);
			oy += 2;
		}	
	}	
	
	//画线
	oy = 3;
	for(i=0; i<2; i++)
	{
		DrawLineX(0,oy, 239, 0x80);
		oy += 2;	
	}
	DrawLineY(36,2,4,0xff);
}

u8 const CAL_DAS_LABEL2[5][6]=
{
	"20mA"," 5V  "," 5mA","-5mA"," 10V ",		
};

//变送选择界面
void LCDCalAoS(void)
{
	u8 i,oy,ox;
	Printf816(0, 0, "AO Select", 0);
	Printf816(0,2,"AOS ",1);
	ox=Printf816(64, 2,CAL_DAS_LABEL2[MeterSetData.c_DASelect],0);
	Printf816Lost(ox, 2, (48-ox),0);
	if(STEP_3==CalState)
	{
		ox=Printf816(64, 2,CAL_DAS_LABEL2[KEY_XY[2][0]],1);
		Printf816Lost(ox, 2, (40-ox),1);
	}
	//画线
	oy = 3;
	for(i=0; i<1; i++)
	{
		DrawLineX(0,oy, 239, 0x80);
		oy += 2;	
	}
	DrawLineY(32,2,2,0xff);
	PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_ENTER);	//四个按键
}

u8 const CALMODELLABEL1[8][6]=
{
	"DI","DO","AO","COM","COM2","TOU","CLASS","THD"	
};
u8 const CALMODLABEL2[2][8]=
{
	"Disable" ,"Enable ", 
};

u8 const CALMODLABEL3[4][2]=
{
	"0","2","4","6",
};
u8 const CALMODLABEL4[2][5]=
{
 "0.5S","0.2S",
};
u8 const CALMODLABEL5[6][3]=
{
 "15","15","21","31","51","63",
};
//模块使能界面
void LCDCalMEn(void)
{
	u8 i,oy, ox, m;
	Printf816(0, 0, "Model Enable", 0);
	oy=2;
	for(i=0; i<8; i++)
	{
		if(i==KEY_XY[1][0]) m=1;
		else m=0;
		ox=Printf816(0, oy, CALMODELLABEL1[i],m);	
		Printf816Lost(ox, oy, (40-ox),m);
		oy += 2;
	}
	Printf816(64, 2, CALMODLABEL3[MeterSetData.c_set[DISET]],0);
	Printf816(64,4, CALMODLABEL2[MeterSetData.c_set[DOSET]],0);
	Printf9(64, 6, MeterSetData.c_set[AOSET],0);
	Printf816(64,8, CALMODLABEL2[MeterSetData.c_set[COMSET]],0);
	Printf816(64,10, "----",0);
	Printf816(64,12, CALMODLABEL2[MeterSetData.c_set[TOUSET]],0);
	Printf816(64,14, CALMODLABEL4[MeterSetData.c_set[CLASSSET]],0);
	Printf816(64,16, CALMODLABEL5[MeterSetData.c_set[THDSET]],0);
	if(STEP_3==CalState)
	{
		if(CAL_2_DIEN==KEY_XY[1][0]) Printf816(64, 2, CALMODLABEL3[KEY_XY[2][0]],1);
		if(CAL_2_DOEN==KEY_XY[1][0]) Printf816(64,4, CALMODLABEL2[KEY_XY[2][0]],1);
		if(CAL_2_AOEN==KEY_XY[1][0]) Printf9(64, 6, KEY_XY[2][0],1);
		if(CAL_2_COMEN==KEY_XY[1][0]) Printf816(64,8, CALMODLABEL2[KEY_XY[2][0]],1);
		if(CAL_2_PULEN==KEY_XY[1][0]) Printf816(64,10, CALMODLABEL2[KEY_XY[2][0]],1);
		if(CAL_2_TOUEN==KEY_XY[1][0]) Printf816(64,12, CALMODLABEL2[KEY_XY[2][0]],1);
		if(CAL_2_CLASSEN==KEY_XY[1][0]) Printf816(64,14, CALMODLABEL4[KEY_XY[2][0]],1);
		if(CAL_2_THDEN==KEY_XY[1][0]) Printf816(64,16, CALMODLABEL5[KEY_XY[2][0]],1);	
	}	
	
	oy = 3;
	for(i=0; i<8; i++)
	{
		DrawLineX(0,oy, 239, 0x80);
		oy += 2;	
	}
	DrawLineY(40,2,16,0xff);
	PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_ENTER);	//四个按键
}

void LCDCalLimmit(void)
{
	u8 oy,ox, i, m;
	Printf816(0, 0, "U I Limit", 0);
	if(0==KEY_XY[1][0]) m=1;
	else m=0;
	ox=Printf816(0,2," U ",m);
	Printf816Lost(ox, 2, (36-ox),m);
	if(1==KEY_XY[1][0]) m=1;
	else m=0;
	ox=Printf816(0,4," I ",m);
	Printf816Lost(ox, 4, (36-ox),m);
	
	Printf9999DotShadow(64, 2, MeterSetData.UIViewLimit[0],2,0);
	Printf9999DotShadow(64, 4, MeterSetData.UIViewLimit[1],1,0);
	Printf816(112, 2, "V",0);	
	Printf816(112, 4, "mA",0);
	PrintfKeyF4(K_UP,K_DOWN,K_ESC,K_ENTER);	//四个按键
	
	if(STEP_3 == CalState)
	{
		PrintfKeyF4(K_LEFT,K_UP,K_MENU,K_OK);	//四个按键
		if(0==KEY_XY[1][0]) Printf9999DotShadow(64, 2, KEY_XY[2][0],2, KeyMult+1);	
		if(1==KEY_XY[1][0]) Printf9999DotShadow(64, 4, KEY_XY[2][0],1, KeyMult+1);		
	}	
	Printf816(104, 7, "Warn",0);
	Printf816(32, 10, "Don't Modify the Value.",0);
	Printf816(32, 12, "U Limit value is 10.00V.",0);
	Printf816(32,14, "I Limit value is 10.0mA.",0);
	
	//画线
	oy = 3;
	for(i=0; i<2; i++)
	{
		DrawLineX(0,oy, 239, 0x80);
		oy += 2;	
	}
	DrawLineY(36,2,4,0xff);
}


/****************************************************************************
Name: u32 MUL3DIV1_32_2(u32 MUL1, u32 DIV1)
Function: 电能数据显示运算 
Input: MUL1 -> 电能数据  DIV1 -> 分频系数
****************************************************************************/
u32 MUL3DIV1_32_2(u32 MUL1, u32 DIV1)
{
    u32 x;
    u32 x2;
    x= MUL1;                    
    x = (u32)((float)x * MeterSetData.c_pt1 /MeterSetData.c_pt2*CtResult/ DIV1);
    x2 = x ;
    return x2;
} 
