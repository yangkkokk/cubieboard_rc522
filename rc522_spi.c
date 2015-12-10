#include "pyinclude.h"
#include "rc522_spi.h"
extern volatile unsigned char RxByte,RxFlag;
/////////////////////////////////////////////////////////////////////
//功  能：初始化管脚和SPI
/////////////////////////////////////////////////////////////////////
void SPI_Init(void)
{
NSS522;
SCK522;
SI522;
SO522;
RST522;
 //   WDTCTL = WDTPW + WDTHOLD;             //Stop watchdog timer
 //   BCSCTL1 = CALBC1_1MHZ;
 //   DCOCTL = CALDCO_1MHZ;

 //   _DINT();
     
//    P1DIR = 0xE9;
 //   P1OUT = 0xFF;
      
//  __delay_cycles(75);
//  __bis_SR_register(LPM0_bits + GIE); 
}
/////////////////////////////////////////////////////////////////////
//功  能：读RC632寄存器
//参数说明：Address[IN]:寄存器地址
//返  回：读出的值
//94 14 28 94  --0x14
/////////////////////////////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char Address)
{
     unsigned char i, ucAddr;
     unsigned char ucResult = 0;

     SCK522_0;
     NSS522_0;
     ucAddr = ((Address<<1)&0x7E)|0x80;

     for (i=8; i>0; i--) {
         if ((ucAddr&0x80) == 0x80)
         {   SI522_1;   }
         else
         {   SI522_0;   }
         SCK522_1;
         ucAddr <<= 1;
         SCK522_0;
     }

     for (i=8; i>0; i--) {
         SCK522_1;
         ucResult <<= 1;
         ucResult |= SO522;
         SCK522_0;
     }

     NSS522_1;
     SCK522_1;
     return ucResult;
}

/////////////////////////////////////////////////////////////////////
//功  能：写RC632寄存器
//参数说明：Address[IN]:寄存器地址
//      value[IN]:写入的值
/////////////////////////////////////////////////////////////////////
void WriteRawRC(unsigned char Address, unsigned char value)
{  
    unsigned char i, ucAddr;

    SCK522_0;
    NSS522_0;
    ucAddr = ((Address<<1)&0x7E);

    for (i=8; i>0; i--) {
        if ((ucAddr&0x80) == 0x80)
        {   SI522_1;   }
        else
        {   SI522_0;   }
        SCK522_1;
        ucAddr <<= 1;
        SCK522_0;
    }

    for (i=8; i>0; i--) {
        if ((value&0x80) == 0x80)
        {   SI522_1;   }
        else
        {   SI522_0;   }
        SCK522_1;
        value <<= 1;
        SCK522_0;
    }
    NSS522_1;
    SCK522_1;
}

/////////////////////////////////////////////////////////////////////
//功  能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//      mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
  char tmp = 0x0;
  tmp = ReadRawRC(reg);
  WriteRawRC(reg, tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//功  能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//      mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
  char tmp = 0x0;
  tmp = ReadRawRC(reg);
  WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 
/*
void inerDelay_us(uchar n)
{
        for(n;n>0;n--);
}
*/
