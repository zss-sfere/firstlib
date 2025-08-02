#ifndef _LCD_H
#define _LCD_H

//扩展功能常量定义
#define DISET 0
#define DOSET  1
#define AOSET  2
#define COMSET 3
#define COM2SET 4
#define TOUSET 5 
#define CLASSSET	6  //电能精度， 0： 0.5， 1:0.2S		
#define THDSET 7

//宏定义 按键显示
#define K_NO	0x00
#define K_UP	0x01
#define K_DOWN	0x02
#define K_LEFT	0x03
#define K_RIGHT	0x04
#define K_ENTER	0x05
#define K_ESC	0x06
#define K_MENU  0x07
#define K_MENUE 0x08
#define K_ENTERE 0x09
#define K_ESCE	0x0a
#define K_EDIT	0x0b
#define K_EDITE	0x0c
#define K_OK	0x0d
#define K_OKE	0x0e
#define K_SWITCH 0x0f
#define K_SWITCHE 0x10
#define K_FREE	0x11
#define K_FREEE 0x12


#define PAGE_EL 	0x00
#define PAGE_EN 	0x01
#define PAGE_QUALITY 	0x02
#define PAGE_WAVE 0x03
#define PAGE_DEMAND	0x04
#define PAGE_MAX	0x05
#define PAGE_MODULE 0x06
#define PAGE_PR		0x07

#define PAGE_ELBAR	0x10



/* 显示菜单分类 */
//电量
//三相四线
#define DISPLAY_34UN      	0x00    // 显示相电压
#define DISPLAY_34UL	  	0x01	// 显示线电压
#define DISPLAY_34I      	0x02    // 显示电流
#define DISPLAY_34P      	0x03    // 显示有功功率
#define DISPLAY_34Q	   		0x04    //
#define DISPLAY_34S			0x05
#define DISPLAY_34PF		0x06

//三相三线
#define DISPLAY_33UL   		0x10     // 显示线电压
#define DISPLAY_33I    		0x11     // 显示电流
#define DISPLAY_33PQSALL	0x12     // 总功率和功率因数

//单相
#define DISPLAY_11UN		0x20     //单相电压电流和频率
#define DISPLAY_11PQSALL    0x21	 //总功率和功率因数

//电能
#define DISPLAY_EN		0x00  //双向电能
#define DISPLAY_EQFOUR 	0x01  //四象限无功电能
#define DISPLAY_TOU		0x02  //正向有功总复费率电能
#define DISPLAY_TOU1	0x03  //本月复费率电能
#define DISPLAY_TOU2    0x04  //上1月复费率电能
#define DISPLAY_TOU3	0x05  //上2月复费率电能
#define DISPLAY_TOU4	0x06  //上3月复费率电能
#define DISPLAY_TOU5	0x07  //上4月复费率电能
#define DISPLAY_TOU6	0x08  //上5月复费率电能
#define DISPLAY_TOU7	0x09  //上6月复费率电能
#define DISPLAY_TOU8	0x0a  //上7月复费率电能
#define DISPLAY_TOU9	0x0b  //上8月复费率电能
#define DISPLAY_TOU10	0x0c  //上9月复费率电能
#define DISPLAY_TOU11	0x0d  //上10月复费率电能
#define DISPLAY_TOU12	0x0e  //上11月复费率电能

//谐波
#define DISPLAY_QUAPHASE 0x00 //电压电流相角
#define DISPLAY_QUAUIM	 0x01 //电压序分量
#define DISPLAY_QUAIIM	 0x02 //电流序分量
#define DISPLAY_QUAUPEAK 0x03 //电压波峰系数
#define DISPLAY_QUAIK	0x04  //电流K系数
#define DISPLAY_THDTHD  0x05 //总谐波畸变率
#define DISPLAY_THD2	0x06//分次谐波
#define DISPLAY_THD7	0x07
#define DISPLAY_THD12	0x08
#define DISPLAY_THD17 	0x09
#define DISPLAY_THD22	0x0a
#define DISPLAY_THD27	0x0b
#define DISPLAY_THD32 0x0c
#define DISPLAY_THD37 0x0d
#define DISPLAY_THD42 0x0e
#define DISPLAY_THD47	0x0f
#define DISPLAY_THD52	0x10
#define DISPLAY_THD57 0x11
#define DISPLAY_THD62	0x12
#define DISPLAY_THDU1 0x13
#define DISPLAY_THDU2	0x14
#define DISPLAY_THDU3	0x15
#define DISPLAY_THDU4	0x16
#define DISPLAY_THDU5 0x17
#define DISPLAY_THDU6	0x18
#define DISPLAY_THDI1	0x19
#define DISPLAY_THDI2	0x1a
#define DISPLAY_THDI3	0x1b
#define DISPLAY_THDI4	0x1c
#define DISPLAY_THDI5	0x1d
#define DISPLAY_THDI6	0x1e


//电能质量界面
#define DISPLAY_QUAUWAVE 0x00 //电压波形
#define DISPLAY_QUAIWAVE 0x01 //电流波形


//需量界面
#define DISPLAY_DEMANDMAX1	0x00
#define DISPLAY_DEMANDMAX2	0x01
#define DISPLAY_DEMANDNOW1	0x02
#define DISPLAY_DEMANDNOW2	0x03
#define DISPLAY_DEMANDPre1	0x04
#define DISPLAY_DEMANDPre2	0x05

//极值界面
//三相四线
#define DISPLAY_MAX34UN	0x00
#define DISPLAY_MAX34UL 0x01
#define DISPLAY_MAX34I	0x02
#define DISPLAY_MAX34PQS	0x03
#define DISPLAY_MIN34UN	0x04
#define DISPLAY_MIN34UL 0x05
#define DISPLAY_MIN34I	0x06
#define DISPLAY_MIN34PQS	0x07
#define DISPLAY_AVG34UI	0x08
#define DISPLAY_AVG34PQS 0x09

//三相三线
#define DISPLAY_MAX33UL	0x10
#define DISPLAY_MAX33I	0x11
#define DISPLAY_MAX33PQS	0x12
#define DISPLAY_MIN33UL	0x13
#define DISPLAY_MIN33I	0x14
#define DISPLAY_MIN33PQS	0x15
#define DISPLAY_AVG33UI	0x16
#define DISPLAY_AVG33PQS 0x17

//单相
#define DISPLAY_MAX11UI	0x20
#define DISPLAY_MAX11PQS	0x21
#define DISPLAY_MIN11UI	0x22
#define DISPLAY_MIN11PQS	0x23


//扩展模块
#define MODULEDI	0x00		//开关量熟人
#define MODULEDO	0x01		//继电器输出
#define MODULERTC	0x02		//时钟
#define MODULESYS	0x03		//系统状态
#define MODULEREV	0x04    	//软件版本


/* 编程菜单分类 */
#define STEP_CODE    0x00   // 密码输入界面
#define STEP_1       0x01   // 第一级菜单界面
#define STEP_2       0x02   // 第二级菜单界面
#define STEP_3       0x03   // 第三级菜单界面
#define STEP_4		 0x04	// 第四级菜单界面	
#define STEP_5		 0x05	//第五级菜单界面
#define STEP_SAVE    0x06   // 保存设置界面
#define STEP_CAL     0x07   // 校准菜单界面
#define STEP_ERR	 0x08			//错误显示


//第一级菜单
#define STEP_1_SYS   	0
#define STEP_1_INPUT	1
#define STEP_1_COMM		2
#define STEP_1_DEMAND	3
#define STEP_1_OUTPUT	4
#define STEP_1_AOOUT	5
#define STEP_1_ALARM    6
#define STEP_1_TOU      7
#define STEP_1_CLR		8


//第二\三级菜单 
//sys
#define STEP_2_CODE		0
#define STEP_2_LIGHT	1
#define STEP_2_GARY		2
#define STEP_2_LANGUAGE 3
#define STEP_2_DISPLAY	4

#define POWERU	0
#define POWERI 	1
#define	POWERP 	2
#define POWEREN	3
#define POWERTHD 4
#define POWERWAVE 5
#define POWERDEMAND 6
#define POWERMAX 7

//input
#define STEP_2_INPUT	0
#define STEP_2_PT2		1
#define STEP_2_PT1		2
#define STEP_2_CT2		3
#define STEP_2_CT1		4
//com1
#define STEP_2_SN	0
#define STEP_2_BAUD	1
#define STEP_2_DATA 2
#define STEP_2_PROT 3  //Protocol

//Demand 
#define STEP_2_DEMAND 0
#define STEP_2_MAXMINTIME 1

//DO
#define STEP_2_DO1	0
#define STEP_2_DO2	1
#define STEP_2_DO3  2
 
#define STEP_3_DOOFF	0
#define STEP_3_DOALR	1
#define STEP_3_DOREM	2

//第四级菜单
//DO
#define STEP_4_PULSE	0
#define STEP_4_ITEM		1
#define STEP_4_VALUE	2
#define STEP_4_HYS		3
#define STEP_4_DELAY	4

//第五级菜单
//DO
#define STEP_5_UAH	0
#define STEP_5_UAL	1
#define STEP_5_UBH	2
#define STEP_5_UBL	3
#define STEP_5_UCH	4
#define STEP_5_UCL	5
#define STEP_5_UNH	6
#define STEP_5_UNL	7
#define STEP_5_UABH	8
#define STEP_5_UABL	9
#define STEP_5_UBCH	10
#define STEP_5_UBCL	11
#define STEP_5_UCAH	12
#define STEP_5_UCAL	13
#define STEP_5_ULH	14
#define STEP_5_ULL	15
#define STEP_5_UNAVGH	16
#define STEP_5_UNAVGL	17
#define STEP_5_ULAVGH	18
#define STEP_5_ULAVGL	19
#define STEP_5_IAH	20
#define STEP_5_IAL	21
#define STEP_5_IBH	22
#define STEP_5_IBL	23
#define STEP_5_ICH	24
#define STEP_5_ICL	25
#define STEP_5_IH	26
#define STEP_5_IL	27
#define STEP_5_IAVGH	28
#define STEP_5_IAVGL	29
#define STEP_5_INH	30
#define STEP_5_INL	31
#define STEP_5_PH	32
#define STEP_5_PL	33
#define STEP_5_QH	34
#define STEP_5_QL	35
#define STEP_5_SH	36
#define STEP_5_SL	37
#define STEP_5_PFH	38
#define STEP_5_PFL	39
#define STEP_5_FH	40
#define STEP_5_FL	41
#define STEP_5_UNBH	42
#define STEP_5_UNBL	43
#define STEP_5_INBH	44
#define STEP_5_INBL	45
#define STEP_5_UTHDH	46
#define STEP_5_UTHDL	47
#define STEP_5_ITHDH	48
#define STEP_5_ITHDL	49
#define STEP_5_ALARM1	50
#define STEP_5_ALARM0   51
#define STEP_5_DINN1	52
#define STEP_5_DINN0    53
#define STEP_5_DIN11	54
#define STEP_5_DIN10	55
#define STEP_5_DIN21	56
#define STEP_5_DIN20	57
#define STEP_5_DIN31	58
#define STEP_5_DIN30	59
#define STEP_5_DIN41	60
#define STEP_5_DIN40	61
#define STEP_5_DIN51	62
#define STEP_5_DIN50	63
#define STEP_5_DIN61	64
#define STEP_5_DIN60	65

#define STEP_3_DOITMEM  STEP_5_ALARM1
//AO Menu
#define STEP_2_AOMODE	0x00
#define STEP_2_AOITEM	0x01
#define STEP_2_AODS		0x02
#define STEP_2_AOFS		0x03

//AO变送类型
#define STEP_4_AOOFF	0
#define STEP_4_AO420	1
#define STEP_4_AO020	2
#define STEP_4_AO412	3

//AO变送项目
#define STEP_4_AOUA		0
#define STEP_4_AOUB		1 
#define STEP_4_AOUC		2 
#define STEP_4_AOUAB	3 
#define STEP_4_AOUBC	4 
#define STEP_4_AOUCA	5 
#define STEP_4_AOIA		6 
#define STEP_4_AOIB		7 
#define STEP_4_AOIC		8 
#define STEP_4_AOI0		9 
#define STEP_4_AOPA		10
#define STEP_4_AOPB		11
#define STEP_4_AOPC		12
#define STEP_4_AOP		13
#define STEP_4_AOQA		14
#define STEP_4_AOQB		15
#define STEP_4_AOQC		16
#define STEP_4_AOQ		17
#define STEP_4_AOSA		18
#define STEP_4_AOSB		19
#define STEP_4_AOSC		20
#define STEP_4_AOS		21
#define STEP_4_AOPFA	22
#define STEP_4_AOPFB	23
#define STEP_4_AOPFC	24
#define STEP_4_AOPF		25
#define STEP_4_AOF		26

//越线报警
//第二级菜单
#define STEP_2_ALARMUN 0
#define STEP_2_ALARMUL 1
#define STEP_2_ALARMI 2
#define STEP_2_ALARMF 3
#define STEP_2_ALARMP 4
#define STEP_2_ALARMPF 5
#define STEP_2_ALARMTHDU 6
#define STEP_2_ALARMTHDI 7
#define STEP_2_ALARMUUN 8
#define STEP_2_ALARMIUN 9

//第三级菜单
#define STEP_3_ALARMMODE 0
#define STEP_3_ALARMVALUEH 1
#define STEP_3_ALARMVALUEHYSH 2
#define STEP_3_ALARM2MODE 3
#define STEP_3_ALARMVALUEL 4
#define STEP_3_ALARMVALUEHYS 5
#define STEP_3_ALARMDELAY 6

//
#define STEP_31_ALARMMODE 0
#define STEP_31_ALARMVALUEH 1
#define STEP_31_ALARMVALUEHYSH 2
#define STEP_31_ALARMDELAY 3


//TOU
#define STEP_2_FSD1	0
#define STEP_2_FSD2 1
#define STEP_2_MONS 2
#define STEP_2_COPY 3
#define STEP_2_TIME 4

#define STEP_3_SD1      0x00
#define STEP_3_SD12     0x0B


#define STEP_3_COPYD    0x00            // 日
#define STEP_3_COPYH    0x01            // 时

// 第四级菜单 
#define STEP_4_TYPE1    0x00            // 尖
#define STEP_4_TYPE2    0x01            // 峰
#define STEP_4_TYPE3    0x02            // 平
#define STEP_4_TYPE4    0x03            // 谷

//清零菜单
#define STEP_2_CLREN 0x00
#define STEP_2_CLRDMD 0x01
#define STEP_2_CLRMAX 0x02


//校准菜单

#define CAL_1_CAL	0		//电量校准
#define CAL_1_AO1  1
#define CAL_1_AO2	2
#define CAL_1_AOS  3
#define CAL_1_MENABLE 4
#define CAL_1_LIMMIT  5 //门限值

#define STEP_2_CLR 	0x00	 //增益系数清零
#define STEP_2_UI	0x01	 //电压电流校准1.0
#define STEP_2_P	0x02  	 //有功功率校准1.0
#define STEP_2_Q	0x03	 //无功功率校准0.0L
#define STEP_2_PH	0x04	 //功率因数校准0.5L
#define STEP_2_END	0x05	 //校准系数保存

#define CAL_2_DIEN	0
#define CAL_2_DOEN	1
#define CAL_2_AOEN	2
#define CAL_2_COMEN	3
#define CAL_2_PULEN	4
#define CAL_2_TOUEN	5
#define CAL_2_CLASSEN 6
#define CAL_2_THDEN 7

#define CAL_2_LIMU 0
#define CAL_2_LIMI 1


void TaskLCD(void);
void LCDDisplayMenu(void);
void LCDDisplayUn(void);
void LCDDisplayEL(void);
void LCDDisplayUl(void);
void LCDDisplayI(void);
void LCDDisplayPQSData(s32 *p);
void LCDDisplayP(void);
void LCDDisplayQ(void);
void LCDDisplayS(void);
void LCDDisplayPF(void);
void LCDDisplayUI(void);
void LCDDisplayPQS(void);
u32 MUL3DIV1_32_2(u32 MUL1, u32 DIV1);
void LCDDisplayEN(void);
void LCDDisplayQuality(void);
void LCDDisplayUpeakIk(void);
void LCDDisplayWave(void);
void LCDDisplayUIWave(void);
void LCDDisplayPhase(void);
void LCDDisplayImbalance(void);
void LCDDisplayImbalanceU(void);
void LCDDisplayImbalanceI(void);
void LCDDisplayDemand(void);
void LCDDisplaDemandI(void);
void LCDDisplayDemandP(void);
//void LCDDisplayDemandSelf(void);
void LCDDisplayMax(void);
void LCDMaxMin34UN(void);
void LCDMaxMin34UL(void);
void LCDMaxMin34I(void);
void LCDMaxMinP(void);
void LCDMaxMinQ(void);
void LCDMaxMinS(void);
void LCDMaxPQSData(s16 *p);
void LCDMaxPQSAllData(s16 *midp16);
void LCDMaxMinPQS(void);
void LCDMaxMinUI(void);
void LCDDisplayDI(void);
void LCDDisplayDO(void);
void LCDProgramAO(void);
void LCDProgramAlarm(void);
void LCDProgramTOU(void);
void LCDAvgPQS(void);
void LCDAvgUI(void);

void LCDDisplayCalibrate(void);
void LCDCalStep1(void);
void LCDCalStep2(void);
void LCDUICal(void);
void LCDCalAo(void);
void LCDCalAoS(void);
void LCDCalMEn(void);
void LCDCalLimmit(void);

void LCDProgramAlarm(void);
void LCDDisplaySysState(void);

//以后要删除部分

void LCDDisplayProgram(void); 
void LCDProgramCode(void);
void LCDProgramStep1(void);
void LCDProgramStep2(void);
void LCDProgramStep3(void);
void LCDProgramSys(void);
void LCDProgramInput(void);
void LCDProgramComm(void);
void LCDProgramDO(void);
void LCDProgramDOStep45(void);
void LCDProgramDemand(void);
void LCDProgramModule(void);
void LCDProgramClr(void);
void LCDProgramSave(void);
void LCDDisplayModule(void);
void LCDDiplayDO(void);
void LCDDisplayRTC(void);
void LCDDisplay4DI(void);
void LCDDisplay2InmA(void);
void LCDDisplay2Pt100(void);
void LCDDisplay2Rt100(void);
void LCDDisplay2OutmA(void);
void LCDDisplay2DI2DO(void);
void LCDDisplay1DI(void);
void LCDDisplayXCOM(u8 n);

void LCDXXCal(void);
void LCDUICal(void);
void LCDProgramModuleTime(void);
void LCDProgramTime(u8 midtime[7], u8 shadow);
void LCDProgramModuleRT100(void);
void LCDProgramModule2AO(void);
void LCDProgramModule2DI2IS(void);
void LCDProgramModule4DI(void);
void LCDProgramModule2DO(void);
void LCDProgramModule1DI(void);
void LCDProgramModuleMx(void);
void LCDDisplayUDataBar(u16 *p);
void LCDDisplayUlDataBar(u16 *p);
void LCDDisplayUnBar(void);
void LCDDisplayUlBar(void);
void LCDDisplayIDataBar(u16 *p);
void LCDDisplayIBar(void);
void LCDDisplayPQSDataBar(s16 *p);
void LCDDisplayPBar(void);
void LCDDisplayQBar(void);
void LCDDisplaySBar(void);
void LCDDisplayPFDataBar(s16 *p);
void LCDDisplayPFBar(void);
void LCDDisplayUIData(void);
void LCDDisplayUIBar(void);
void LCDDisplayPQSDataAllBar(void);
void LCDDisplayPQSBar(void);
void LCDDisplayELBar(void);
void LCDProgramModuleDP(void);
void LCDProgramModuleTCPIP(void);
void LCDCalDM2(void);
void LCDCalDM3(void);
void LCDCalDM4(void);
void LCDCalDM5(void);
u8 Printf81624(u8 ox,u8 oy,const u8 *p_char);
void LCDDisplayDemandU(s16 midRMS_U, u8 oy);
void LCDDemandIData(s16 *midp16, u8 oy, u8 m);
void LCDMaxUData(u16 *p, u8 LNum);
void LCDProgramModuleRS485(void);
#endif
