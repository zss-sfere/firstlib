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


/* У׼ */
#define CAL_NO      0x00                               // ��У׼
#define CAL_CLR_OK  0x01                               // ��λ����������
#define CAL_UI0_OK  0x02                               // У׼��ѹ�������
#define CAL_P0_OK   0x03                               // ��ȡ�й����
#define CAL_PH60_OK 0x04                               // У׼��λ������
#define CAL_I60_OK  0x05                               // У׼�������

#define STEP_2_CLR 	0x00	 //����ϵ������
#define STEP_2_UI	0x01	 //��ѹ����У׼1.0
#define STEP_2_P	0x02  	 //�й�����У׼1.0
#define STEP_2_Q	0x03	 //�޹�����У׼0.0L
#define STEP_2_PH	0x04	 //��������У׼0.5L
#define STEP_2_END	0x05	 //У׼ϵ������

#define CAL_AOUT_I 0x00                                // �������
#define CAL_AOUT_U 0x01                                // �����ѹ

/* �����ı��� */
#define KEY_1          0x00                                // ��λ
#define KEY_10         0x01                                // ʮλ
#define KEY_100        0x02                                // ��λ
#define KEY_1000       0x03                                // ǧλ
#define KEY_10000	   0x04																//С���㵥λ

/* �������� */
#define PROG_PASSWORD  1111                            // ����������룺1111
#define PROG_PASSWORD2  2011
#define CAL_PASSWORD   214                             // У׼���룺214
#define CAL_PASSWORD2   2140								   //����ͻ��õ�У׼����

/* ���� */
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
