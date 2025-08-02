#ifndef _MYMATH_H
#define _MYMATH_H
u16 abss16(s16);
u8 HexToBcdChar(u8 p);
u32 abss32(s32 p);
u16 MySqrt32(u32 M);
s16 Sinu16(u16 angle);
s16 Cosu16(u16 angle);
void Delayms(u16 time);
void Delayus(u16 time);
void MyFFT(s32 dataR[NUM_FFT], s32 dataI[NUM_FFT]);
s16 MaxInt16_p(s16 *p,s16 n);
u16 THHD_Un(u16 n,u16 Hz);
u8 MaxInt8_p(u8 *p, u8 n);
#endif

