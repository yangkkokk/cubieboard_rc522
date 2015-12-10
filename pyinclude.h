/*
*		func: cubieboard python GPIO Driver definition
*		author : SUNROC1
*		date: 8/8/2013
*       	qq: 1083380935
*		all rights reserved !
*/
#ifndef _PYINCLUDE_H_
#define _PYINCLUDE_H_

#include <stdlib.h>
#include <stdio.h>

#include "gpio_lib.h"

extern void delay_us(unsigned int nus);
extern void delay_ms(unsigned int nms);

#define PD0    SUNXI_GPD(0)
#define PD1    SUNXI_GPD(1)
#define PD2    SUNXI_GPD(2)
#define PD3    SUNXI_GPD(3)
#define PD4    SUNXI_GPD(4)
#define PD5    SUNXI_GPD(5)
#define PD6    SUNXI_GPD(6)
#define PD7    SUNXI_GPD(7)
#define PD8    SUNXI_GPD(8)
#define PD9    SUNXI_GPD(9)
#define PD10    SUNXI_GPD(10)
#define PD11    SUNXI_GPD(11)
#define PD12    SUNXI_GPD(12)
#define PD13    SUNXI_GPD(13)
#define PD14    SUNXI_GPD(14)
#define PD15    SUNXI_GPD(15)
#define PD16    SUNXI_GPD(16)
#define PD17    SUNXI_GPD(17)
#define PD18    SUNXI_GPD(18)
#define PD19    SUNXI_GPD(19)
#define PD20    SUNXI_GPD(20)
#define PD21    SUNXI_GPD(21)
#define PD22    SUNXI_GPD(22)
#define PD23    SUNXI_GPD(23)
#define PD24    SUNXI_GPD(24)
#define PD25    SUNXI_GPD(25)
#define PD26    SUNXI_GPD(26)
#define PD27    SUNXI_GPD(27)

#define MISO    SUNXI_GPE(3)
#define MOSI    SUNXI_GPE(2)
#define SCK     SUNXI_GPE(1)
#define CS      SUNXI_GPE(0)


#endif
