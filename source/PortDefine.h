
#ifndef _PORT_H
#define _PORT_H

//按照pin的顺序定义
#define GPIO_RN7302CS1	GPIO_Pin_4	//GPIOA RN8302_1片选
#define SPI1PORT	GPIOA       //SPI1，片选

#define GPIO_BACKLIGHT	GPIO_Pin_8	//GPIOC
#define GPIO_LCDRS		GPIO_Pin_7	//GPIOC
#define GPIO_LCDCS		GPIO_Pin_12	//GPIOB
#define GPIO_LCDRST		GPIO_Pin_6	//GPIOC
#define GPIO_IF2	GPIO_Pin_14		//GPIOB

//#define GPIO_LCDSDA		GPIO_Pin_15 //GPIOB //用SPI
//#define GPIO_LCDSCL		GPIO_Pin_13 //GPIOB


#define GPIO_KEYL	GPIO_Pin_9	//GPIOC
#define GPIO_KEYR	GPIO_Pin_8	//GPIOA
#define GPIO_KEYM	GPIO_Pin_9	//GPIOA
#define GPIO_KEYE	GPIO_Pin_10	//GPIOA

//Fram
#define FMPORT GPIOD //
#define GPIO_FMCS GPIO_Pin_2 //GPIOD
#define GPIO_W25CS GPIO_Pin_12 //GPIOC

#define GPIO_RTCSCL	GPIO_Pin_11	//GPIOA
#define GPIO_RTCSDA GPIO_Pin_12  //GPIOA

//DI, DI3-DI6与原理图不同
#define GPIO_DI1	GPIO_Pin_9	//GPIOB
#define GPIO_DI2	GPIO_Pin_8	//GPIOB
#define GPIO_DI3	GPIO_Pin_6 	//GPIOB
#define GPIO_DI4	GPIO_Pin_7	//GPIOB
#define GPIO_DI5	GPIO_Pin_3	//GPIOA
#define GPIO_DI6	GPIO_Pin_2	//GPIOA




//DO
#define GPIO_DO1	GPIO_Pin_2	//GPIOB
#define GPIO_DO2	GPIO_Pin_10	//GPIOB
#define GPIO_DO3	GPIO_Pin_11 //GPIOB

//PWM
#define GPIO_PWM1	GPIO_Pin_0	//GPIOA
#define GPIO_PWM2	GPIO_Pin_1	//GPIOA


#define GPIO_USART3CS	GPIO_Pin_15 //GPIOA


#define KEY_LEFT	0x01
#define KEY_RIGHT	0x02
#define KEY_MENU	0x04
#define KEY_ENTER	0x08
#define KEY_LEFTENTER 0x09
#define KEY_MENUL	0xc4
#define KEY_MENUENTERL	0xcc

#endif
