#include "pyinclude.h"

void delay_us(unsigned int nus)
{
	usleep(nus);
}

void delay_ms(unsigned int nms)
{
	usleep(nms*1000);
}

