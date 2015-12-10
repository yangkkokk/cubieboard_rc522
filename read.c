#include "pyinclude.h"
#include "MFRC522.h"
#include "rc522_spi.h"
#include <string.h>

unsigned char Passwd[6]={0xff,0xff,0xff,0xff,0xff,0xff};  //默认密码
unsigned char data[16]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n'};
unsigned char Temp[4];

int main(void)
{
  unsigned char tt1[2];
  char status1;
  unsigned char sn[4];


void  Find_Card(void)
{
    if(PcdRequest(0x52,Temp)==MI_OK)
    {
      if(Temp[0]==0x04&&Temp[1]==0x00)
          printf("MFOne-S50");
        else if(Temp[0]==0x02&&Temp[1]==0x00)
          printf("MFOne-S70");
        else if(Temp[0]==0x44&&Temp[1]==0x00)
          printf("MF-UltraLight");
        else if(Temp[0]==0x08&&Temp[1]==0x00)
          printf("MF-Pro");
        else if(Temp[0]==0x44&&Temp[1]==0x03)
          printf("MF Desire");
        else
          printf("Unknown");
        printf("SUCCESS!");
    }else{ printf("Faile!");
	printf("temp is : %d\n",Temp[0]);
	}
       if(PcdRequest(0x52,Temp)==MI_ERR) {
         printf("error con't find card\n");
         }

}








	if(SETUP_OK!=sunxi_gpio_init())
	{
	        printf("Failed to initialize GPIO\n");
	        return -1;
	}
  printf("loading spi \n");
	SPI_Init();
  printf("loading rc522\n");
  Init_rc522();
        status1=ReadRawRC(0x05);
	printf("sys is %d",status1);

  PcdReset();//复位RC522
  PcdAntennaOn();//开启天线发射 
  while(1)
  {
        status1 = PcdRequest(PICC_REQALL,tt1);
        status1 = PcdAnticoll(sn);           
/*
       if (status1==MI_ERR) {
	 printf("error con't find card\n");
         }
       if (status1==MI_ERR) {
         printf("error con't find card\n");
         }
*/
	printf("find card\n");
	Find_Card();

/*

         if (status1==MI_OK) {
//            printf("** CMD Success !\r\n");
//            printf("** Card TypeTag: %x",tt1[0]);
//            printf(" %x \r\n",tt1[1]);
            printf("** CMD Success !\r\n");
            printf("** Serial Num: %d",sn[0]);
            printf("%d",sn[1]);
            printf("%d",sn[2]);
            printf("%d \r\n",sn[3]);
	    printf("** Select Card !\r\n");

         }
*/
	
}

}
