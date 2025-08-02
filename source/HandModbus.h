#ifndef _HANDMODBUS_H
#define _HANDMODBUS_H

u8 UsartRead(void);
void UsartReply(void);
void TaskUsart3(void);
void TaskUsart3TxEnd(void);
u8 ReadCoilSta(void);
u8 ReadInputSta(void);
u8 ForceSingleBit(void);
u8 ForceMultiBit(void);
u8 UsartWrite06(void);
u8 UsartWrite(void);
u8 UserDefine(u8 *ptr);
u8 WriteData(u8 *ptr, u16 addr, u16 num);
u8 UsartFileRead(void);

void DLT645Dealwith(void);
#endif
