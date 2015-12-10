#ifndef __rc522_spi__
#define __rc522_spi__

#include <stdlib.h>
#include <stdio.h>

#include "pyinclude.h"

typedef unsigned char uchar;
typedef unsigned int  uint;

#define   SDA5221		sunxi_gpio_set_cfgpin(PD0,OUTPUT)
#define   NSS522		sunxi_gpio_set_cfgpin(PD1,OUTPUT)
#define   SCK522		sunxi_gpio_set_cfgpin(PD2,OUTPUT)
#define   SI522			sunxi_gpio_set_cfgpin(PD3,OUTPUT)
#define   SO522			sunxi_gpio_set_cfgpin(PD4,INPUT)
#define   RST522		sunxi_gpio_set_cfgpin(PD5,OUTPUT)

//==============================================================================
#define  NSS5221_0          sunxi_gpio_output(PD0,LOW)
#define  NSS5221_1          sunxi_gpio_output(PD0,HIGH)  
//=============================NSS522�˿�==================================
#define  NSS522_0       sunxi_gpio_output(PD1,LOW)
#define  NSS522_1       sunxi_gpio_output(PD1,HIGH)  
//=============================SCK522======================================
#define  SCK522_0        sunxi_gpio_output(PD2,LOW)
#define  SCK522_1        sunxi_gpio_output(PD2,HIGH)  
//========================== MOSI522�˿�================================
#define  SI522_0      sunxi_gpio_output(PD3,LOW)
#define  SI522_1      sunxi_gpio_output(PD3,HIGH)  
//=============================MISO522�˿�=========================================

#define  SO522_PIN   sunxi_gpio_input(PD4)

//==========================RST522״̬============================================
#define  RST522_0       sunxi_gpio_output(PD5,LOW)
#define  RST522_1       sunxi_gpio_output(PD5,HIGH)  
//==============================================================================
void SPI_Init(void);	//��ʼ���ܽź�SPI
void ClearBitMask(unsigned char reg,unsigned char mask); //��RC522�Ĵ���λ
unsigned char ReadRawRC(unsigned char Address);//��RC522
void WriteRawRC(unsigned char Address,unsigned char value);	//дRC522
void SetBitMask(unsigned char reg,unsigned char mask);//��RC522�Ĵ���λ
//void inerDelay_us(uchar n)
#endif
