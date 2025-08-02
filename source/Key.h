#ifndef _KEY_H
#define _KEY_H

#define INPUT34		0x00
#define INPUT33		0x01
#define INPUT11		0x02


#define STEP_3_N34   0x00
#define STEP_3_N33   0x01
#define STEP_3_N11	 0x02
#define STEP_3_380V  380
#define STEP_3_100V  0x01
#define STEP_3_5A    0x00
#define STEP_3_1A    0x01


/* 校准 */
#define CAL_NO      0x00                               // 无校准
#define CAL_CLR_OK  0x01                               // 相位误差清零完成
#define CAL_UI0_OK  0x02                               // 校准电压电流完成
#define CAL_P0_OK   0x03                               // 读取有功完成
#define CAL_PH60_OK 0x04                               // 校准相位误差完成
#define CAL_I60_OK  0x05                               // 校准电流完成

#define STEP_2_CLR 	0x00	 //增益系数清零
#define STEP_2_UI	0x01	 //电压电流校准1.0
#define STEP_2_P	0x02  	 //有功功率校准1.0
#define STEP_2_Q	0x03	 //无功功率校准0.0L
#define STEP_2_PH	0x04	 //功率因数校准0.5L
#define STEP_2_END	0x05	 //校准系数保存

#define CAL_AOUT_I 0x00                                // 输出电流
#define CAL_AOUT_U 0x01                                // 输出电压

/* 按键改变量 */
#define KEY_1          0x00                                // 个位
#define KEY_10         0x01                                // 十位
#define KEY_100        0x02                                // 百位
#define KEY_1000       0x03                                // 千位
#define KEY_10000	   0x04																//小数点单位

/* 万能密码 */
#define PROG_PASSWORD  1111                            // 编程万能密码：1111
#define PROG_PASSWORD2  2011
#define CAL_PASSWORD   214                             // 校准密码：214
#define CAL_PASSWORD2   2140								   //外面客户用的校准密码

/* 保存 */
#define STEP_SAVE_YES 0x00
#define STEP_SAVE_NO  0x01

void TaskKey(void);
void TaskScanKey(void);

void KeyScanMode0 (void);
void KeyScanMode1 (void);
void KeyScanMode2 (void);
void KeyScanMode3 (void);
void KeyScanMode4 (void);

u8 KeyRd(void);
void KeyDealMainmenu(void);
void KeyChangeOption(u16 *add, u16 n,u16 m);
void KeyDealDisplay(u8 page, u8 n, u8 m);
void Change999999(u32 *p32,u8 *p8,u32 max);
void ChangeFFFF(u32 *p16,u8 *p8);
void Change99(u16 *p,u16 max);
void KeyProgramCode(void);
void KeyDealProgram(void);
void KeyProgramStep1(void);
void KeyProgramStep2(void);
void KeyProgramStep3(void);
void KeyIntoStep2(void);
void KeyIntoStep3(void);
void KeyProgramStep4(void);
void KeyProgramStep5(void);
void KeyProgramSave(void);
void KeyBackStep2(void);
void SaveData(void);
void RestoreData(void);
void KeyDealCalibrate(void);
void KeyCalStep1(void);
void KeyCalStep2(void);
void KeyCalStep3(void);
void KeyIntoCalStep2(void);
void BackCalStep2(void);
void SaveCalData(void);
void KeyCalSave(void);
void ClrEn(void);
void ClrDemand(void);
void ClrMax(void);
void ClrTime(void);
void DoFreeJudge(void);
#endif
