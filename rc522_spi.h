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
//=============================NSS522端口==================================
#define  NSS522_0       sunxi_gpio_output(PD1,LOW)
#define  NSS522_1       sunxi_gpio_output(PD1,HIGH)  
//=============================SCK522======================================
#define  SCK522_0        sunxi_gpio_output(PD2,LOW)
#define  SCK522_1        sunxi_gpio_output(PD2,HIGH)  
//========================== MOSI522端口================================
#define  SI522_0      sunxi_gpio_output(PD3,LOW)
#define  SI522_1      sunxi_gpio_output(PD3,HIGH)  
//=============================MISO522端口=========================================

#define  SO522_PIN   sunxi_gpio_input(PD4)

//==========================RST522状态============================================
#define  RST522_0       sunxi_gpio_output(PD5,LOW)
#define  RST522_1       sunxi_gpio_output(PD5,HIGH)  
//==============================================================================
void SPI_Init(void);	//初始化管脚和SPI
void ClearBitMask(unsigned char reg,unsigned char mask); //清RC522寄存器位
unsigned char ReadRawRC(unsigned char Address);//读RC522
void WriteRawRC(unsigned char Address,unsigned char value);	//写RC522
void SetBitMask(unsigned char reg,unsigned char mask);//置RC522寄存器位
//void inerDelay_us(uchar n)
#endif
