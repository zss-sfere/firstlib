#ifndef _INITLCM_H
#define _INITLCM_H
void InitLCM(void);
void DisplayUpdate(void);
void Printf1616(u8 ox,u8 oy,const u8 *p_char,u8 shadow);
u8 Printf816(u8 ox,u8 oy,const u8 *p_char,u8 shadow) ;
void PrintfKeyF4(u8 F1,u8 F2,u8 F3,u8 F4);
void UintToBCD5(u8 *p_char,u32 value);
void LCDClrAll(void);
void LCDClrWave(void);
void Printf1624Num5(u8 ox,u8 oy,s32 num,u8 dot);
void PrintfBMP3216(u8 ox,u8 oy,u8 bmp);
void PrintfBMP24104(u8 ox,u8 oy,u8 bmp);
void PrintfBMP2472(u8 ox,u8 oy,u8 bmp);
void Printf1624Num8(u8 ox,u8 oy,u32 num,u8 dot); 
void UintToBCD8(u8 *p_char,u32 value);
void Print816Num(u8 ox,u8 oy,u8 midchar,u8 shadow);
void DrawLineX2(u8 ox, u8 oy, u8 xl, u8 line);
void DrawLineY(u8 ox, u8 oy, u8 ly, u8 line);
void UintToBCD4(u8 *p_char,u16 value);
void Printf1624Num4(u8 ox,u8 oy,s16 num,u8 dot);
void UintToBCD3(u8 *p_char,u16 value);
void Printf816Num3(u8 ox,u8 oy,s16 num,u8 dot);
void DrawLineX(u8 ox, u8 oy, u8 lx, u8 line);
void Printf86(u8 ox,u8 oy,const u8 *p_char,u8 shadow) ; 
void Printf9(u8 ox,u8 oy,u16 n,u8 shadow);
void Printf99(u8 ox,u8 oy,u16 n,u8 shadow);
void PrintfFFHex(u8 ox,u8 oy,u16 n,u8 shadow);
void Printf999(u8 ox,u8 oy,u16 n,u8 shadow);
void Printf9999(u8 ox,u8 oy,u16 n,u8 shadow);
void Printf999999(u8 ox,u8 oy,u32 n,u8 shadow);
void Printf9999DotShadow(u8 ox,u8 oy,u16 value,u8 dot,u8 shadow);
void Printf99999999(u8 ox,u8 oy,u32 n);
void Printf1624(u8 ox,u8 oy,u8 i);
void Printf1624Num6(u8 ox,u8 oy,u32 num);
void PrintfLine(u8 ox, u8 oy, u16 len,u8 data);
void PrintfFFFFHex(u8 ox,u8 oy,u16 n,u8 shadow);
void Printf816Char(u8 ox,u8 oy, u8 *pchar, u8 k);
void Printf161624(u8 ox,u8 oy,const u8 *p_char);
void PrintfBMP820(u8 ox,u8 oy,u8 bmp);
void Printf99999(u8 ox,u8 oy,u16 n,u8 shadow);
void Printf81624Unit(u8 ox,u8 oy,u8 unit); 
void Printf816Lost(u8 ox,u8 oy,u8 k,u8 shadow);
void Printf816Unit(u8 ox,u8 oy,u8 unit, u8 shadow); 
void LCDData(u8 LCDData);

void CalPtCt(void);

extern const u16 LCDGRAY[10];

#endif
