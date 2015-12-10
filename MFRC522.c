#include "pyinclude.h"
#include "MFRC522.h"
#include "rc522_spi.h"
//unsigned char Passwd[6]={0xff,0xff,0xff,0xff,0xff,0xff};  //默认密码
/////////////////////////////////////////////////////////////////////
//功  能：初始化RC522函数
//参数说明: 
//返  回: 
///////////////////////////////////////////////////////////////////// 
void Init_rc522(void)
{
     PcdReset();
     PcdAntennaOff(); 
     PcdAntennaOn();  
}
/////////////////////////////////////////////////////////////////////
//功  能：寻卡
//参数说明: req_code[IN]:寻卡方式
//        0x52 = 寻感应区内所有符合14443A标准的卡
//        0x26 = 寻未进入休眠状态的卡
//      pTagType[OUT]：卡片类型代码
//        0x4400 = Mifare_UltraLight
//        0x0400 = Mifare_One(S50)
//        0x0200 = Mifare_One(S70)
//        0x0800 = Mifare_Pro(X)
//        0x4403 = Mifare_DESFire
//返  回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
   char status;  
   unsigned int  unLen;
   unsigned char ucComMF522Buf[MAXRLEN]; 

   ClearBitMask(Status2Reg,0x08);       // 该位用来指示MIFARECyptol单元接通和因此所有卡的数据通信被加密的情况        
   WriteRawRC(BitFramingReg,0x07);      //最后一个字节的所有位都不应发送
   SetBitMask(TxControlReg,0x03);       //TX1,TX2管脚的输出信号将传递经发送数据调整的13.56MHZ的能量载波信号
   
 
   ucComMF522Buf[0] = req_code;

   status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
   
   if ((status == MI_OK) && (unLen == 0x10))
   {  
     *pTagType   = ucComMF522Buf[0];        
     *(pTagType+1) = ucComMF522Buf[1];
   }
   else
   {   status = MI_ERR;   }
   
   return status;
}

/////////////////////////////////////////////////////////////////////
//功  能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返  回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////  
char PcdAnticoll(unsigned char *pSnr)
{
  char status;
  unsigned char i,snr_check=0;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 

  ClearBitMask(Status2Reg,0x08);
  WriteRawRC(BitFramingReg,0x00);     //最后一个字节的所有位都应发送     
  ClearBitMask(CollReg,0x80);         //所有接收的位在冲撞后被清除
 
  ucComMF522Buf[0] = PICC_ANTICOLL1;    //防冲撞(命令代码“93”代表要处理的射频卡UID只有4字节)
  ucComMF522Buf[1] = 0x20;            

  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

  if (status == MI_OK)
  {
  	 for (i=0; i<4; i++)
     {   
       *(pSnr+i)  = ucComMF522Buf[i];
       snr_check ^= ucComMF522Buf[i];
     }
     if (snr_check != ucComMF522Buf[i])
     {   status = MI_ERR;  }
  }
  
  SetBitMask(CollReg,0x80);
  return status;
}

/////////////////////////////////////////////////////////////////////
//功  能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返  回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr)
{
  char status;
  unsigned char i;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 
  
  ucComMF522Buf[0] = PICC_ANTICOLL1;    //防冲撞
  ucComMF522Buf[1] = 0x70;
  ucComMF522Buf[6] = 0;
  for (i=0; i<4; i++)
  {
  	ucComMF522Buf[i+2] = *(pSnr+i);
  	ucComMF522Buf[6]  ^= *(pSnr+i);
  }
  CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
  ClearBitMask(Status2Reg,0x08);

  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
  
  if ((status == MI_OK) && (unLen == 0x18))
  {   status = MI_OK;  }
  else
  {   status = MI_ERR;  }

  return status;
}

/////////////////////////////////////////////////////////////////////
//功  能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//         0x60 = 验证A密钥
//         0x61 = 验证B密钥 
//      addr[IN]：块地址
//      pKey[IN]：密码
//      pSnr[IN]：卡片序列号，4字节
//返  回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////         
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
  char status;
  unsigned int  unLen;
  unsigned char i,ucComMF522Buf[MAXRLEN]; 

  ucComMF522Buf[0] = auth_mode;
  ucComMF522Buf[1] = addr;
  for (i=0; i<6; i++)
  {  ucComMF522Buf[i+2] = *(pKey+i);   }
  for (i=0; i<6; i++)
  {  ucComMF522Buf[i+8] = *(pSnr+i);   }
  
  status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
  if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
  {   status = MI_ERR;   }
  
  return status;
}

/////////////////////////////////////////////////////////////////////
//功  能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//      pData[OUT]：读出的数据，16字节
//返  回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdRead(unsigned char addr,unsigned char *pData)
{
  char status;
  unsigned int  unLen;
  unsigned char i,ucComMF522Buf[MAXRLEN]; 

  ucComMF522Buf[0] = PICC_READ;
  ucComMF522Buf[1] = addr;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
  if ((status == MI_OK) && (unLen == 0x90))     //若执行成功，则MIFARE卡返回18字节应答比特,故unlen==0x90
  {
    for (i=0; i<16; i++)
    {  *(pData+i) = ucComMF522Buf[i];   }
  }
  else
  {   status = MI_ERR;   }
  
  return status;
}

/////////////////////////////////////////////////////////////////////
//功  能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//      pData[IN]：写入的数据，16字节
//返  回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////          
char PcdWrite(unsigned char addr,unsigned char *pData)
{
  char status;
  unsigned int  unLen;
  unsigned char i,ucComMF522Buf[MAXRLEN]; 
  
  ucComMF522Buf[0] = PICC_WRITE;
  ucComMF522Buf[1] = addr;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

  if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
  {   status = MI_ERR;   }
    
  if (status == MI_OK)
  {
    for (i=0; i<16; i++)
    {  ucComMF522Buf[i] = *(pData+i);   }
    CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
  }
  
  return status;
}
/////////////////////////////////////////////////////////////////////
//功  能：写数据到M1卡一块(包括验证密钥)
//参数说明: BlockAddr：块地址
//          Password：密钥
//          pSelsnr:卡片序列号
//          pbuffer:写入数据
//返  回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdWriteOneBlock(unsigned char BlockAddr, unsigned char *Password, unsigned char *pSelSnr, unsigned char *pbuffer)
{
    unsigned char i;
char status;
    status=PcdAuthState(PICC_AUTHENT1A,BlockAddr,Password,pSelSnr);//
    if(status!=MI_OK) {
      printf("** CMD Fail!\r\n");
      return MI_ERR;
    }
  
    status=PcdWrite(BlockAddr,pbuffer);
    if(status!=MI_OK) {
      printf("** CMD Fail!\r\n");
      return MI_ERR;
    } 

    printf("\r\n");
    printf("** Wirite Block %d Data: \r\n", BlockAddr);
    for(i=0;i<16;i++) {
      if(((i &7) == 0) &&(i != 0))  
        printf("\r\n");
      printf(" %x",pbuffer[i]);

    }

    printf("\r\n");

    return  status;
}
/////////////////////////////////////////////////////////////////////
//功  能：从M1卡读一块数据(包括验证密钥)
//参数说明: BlockAddr：块地址
//          Password：密钥
//          pSelsnr:卡片序列号
//          pbuffer:写入数据
//返  回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReadOneBlock(unsigned char BlockAddr, unsigned char *Password, unsigned char *pSelSnr, unsigned char *pbuffer)
{
    unsigned char i;
char status;
    status=PcdAuthState(PICC_AUTHENT1A,BlockAddr,Password,pSelSnr);
    if(status!=MI_OK) {
      printf("** CMD Fail!\r\n");
      return MI_ERR;
    }

    status=PcdRead(BlockAddr,pbuffer);
    if(status!=MI_OK) {
      printf("** CMD Fail!\r\n");
      return MI_ERR;
    }

    printf("\r\n"); 
    printf("** Read Block %d Data: \r\n", BlockAddr);

    for(i=0;i<16;i++) {
      if(((i &7) == 0) &&(i != 0))  
        printf("\r\n");
      printf(" %x",pbuffer[i]);

    }

    printf("\r\n");

    return  status;
}
/////////////////////////////////////////////////////////////////////
//功  能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//         0xC0 = 扣款
//         0xC1 = 充值
//      addr[IN]：钱包地址
//      pValue[IN]：4字节增(减)值，低位在前
//返  回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////         
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
  char status;
  unsigned int  unLen;
  unsigned char i,ucComMF522Buf[MAXRLEN]; 
  
  ucComMF522Buf[0] = dd_mode;
  ucComMF522Buf[1] = addr;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

  if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
  {   status = MI_ERR;   }
    
  if (status == MI_OK)
  {
    for (i=0; i<16; i++)
    {  ucComMF522Buf[i] = *(pValue+i);   }
    CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
    unLen = 0;
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
    if (status != MI_ERR)
    {  status = MI_OK;  }
  }
  
  if (status == MI_OK)
  {
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
  }
  return status;
}

/////////////////////////////////////////////////////////////////////
//功  能：备份钱包
//参数说明: sourceaddr[IN]：源地址
//      goaladdr[IN]：目标地址
//返  回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
  char status;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 

  ucComMF522Buf[0] = PICC_RESTORE;
  ucComMF522Buf[1] = sourceaddr;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

  if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
  {   status = MI_ERR;   }
  
  if (status == MI_OK)
  {
    ucComMF522Buf[0] = 0;
    ucComMF522Buf[1] = 0;
    ucComMF522Buf[2] = 0;
    ucComMF522Buf[3] = 0;
    CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
    if (status != MI_ERR)
    {  status = MI_OK;  }
  }
  
  if (status != MI_OK)
  {  return MI_ERR;   }
  
  ucComMF522Buf[0] = PICC_TRANSFER;
  ucComMF522Buf[1] = goaladdr;

  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

  if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
  {   status = MI_ERR;   }

  return status;
}


/////////////////////////////////////////////////////////////////////
//功  能：命令卡片进入休眠状态
//返  回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
  char status;
  unsigned int  unLen;
  unsigned char ucComMF522Buf[MAXRLEN]; 

  ucComMF522Buf[0] = PICC_HALT;     //休眠
  ucComMF522Buf[1] = 0;
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
  status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

  return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
//  CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
  unsigned char i,n;
  ClearBitMask(DivIrqReg,0x04);                   
  WriteRawRC(CommandReg,PCD_IDLE);                //取消当前命令
  SetBitMask(FIFOLevelReg,0x80);                  //内部FIFO的缓冲区的读与写指针以及寄存器ErrReg的BufferOvfi标志立刻被清除
  for (i=0; i<len; i++)
  {   WriteRawRC(FIFODataReg, *(pIndata+i));   }
  WriteRawRC(CommandReg, PCD_CALCCRC);            //CRC计算
  i = 0xFF;
  do 
  {
    n = ReadRawRC(DivIrqReg);               //读出中断请求标志
    i--;
  }
  while ((i!=0) && !(n&0x04));              //当CRC计算完毕后
  pOutData[0] = ReadRawRC(CRCResultRegL);   //读出CRC计算实际的LSB值
  pOutData[1] = ReadRawRC(CRCResultRegM);   //读出CRC计算实际的MSB值
}

/////////////////////////////////////////////////////////////////////
//功  能：复位RC522
//返  回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset()
{
  WriteRawRC(CommandReg,PCD_RESETPHASE);    //复位
  
  delay_us(1000);
  WriteRawRC(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
  WriteRawRC(TReloadRegL,30);         //将30装入定时器           
  WriteRawRC(TReloadRegH,0);          
  WriteRawRC(TModeReg,0x8D);
  WriteRawRC(TPrescalerReg,0x3E);
  WriteRawRC(TxAutoReg,0x40);
  
  ClearBitMask(TestPinEnReg, 0x80);//off MX and DTRQ out
  WriteRawRC(TxAutoReg,0x40);
   
  return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//功  能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//      pInData[IN]:通过RC522发送到卡片的数据
//      InLenByte[IN]:发送数据的字节长度
//      pOutData[OUT]:接收到的卡片返回数据
//      *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
char PcdComMF522(unsigned char Command, 
         unsigned char *pInData, 
         unsigned char InLenByte,
         unsigned char *pOutData, 
         unsigned int  *pOutLenBit)
{
  char status = MI_ERR;
  unsigned char irqEn   = 0x00;
  unsigned char waitFor = 0x00;
  unsigned char lastBits;
  unsigned char n;
  unsigned int i;
  switch (Command)
  {
     case PCD_AUTHENT:      //验证密钥
      irqEn   = 0x12;
      waitFor = 0x10;
      break;
     case PCD_TRANSCEIVE:   //发送并接收数据
      irqEn   = 0x77;
      waitFor = 0x30;
      break;
     default:
     break;
  }
   
  WriteRawRC(ComIEnReg,irqEn|0x80);   /*如果验证密钥,管脚IRQ上的信号与IRq位相反,允许空闲中断请求,允许错误中断请求
                                        如果发送并接受数据,管脚IRQ上的信号与IRq位相反,允许发送器中断请求,允许接受器中断请求,
                                        允许空闲中断请求,允许低警报中断请求,允许错误中断请求,允许定时器中断请求*/
  ClearBitMask(ComIrqReg,0x80);       //CommIRqReg寄存器中的屏蔽位清零
  WriteRawRC(CommandReg,PCD_IDLE);    //取消当前命令
  SetBitMask(FIFOLevelReg,0x80);      //内部FIFO的缓冲区的读与写指针以及寄存器ErrReg的BufferOvfi标志立刻被清除
  
  for (i=0; i<InLenByte; i++)
  {   WriteRawRC(FIFODataReg, pInData[i]);  }   //将数据发送写入FIFO缓冲区
  WriteRawRC(CommandReg, Command);              //执行RC522命令字
   
  
  if (Command == PCD_TRANSCEIVE)                //如果命令字为发送并接受数据
  {  SetBitMask(BitFramingReg,0x80);  }         //启动数据传输(将FIFO中的数据传入卡中)
  
  i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
  //i=65535;
  do 
  {
     n = ReadRawRC(ComIrqReg);                //读取中断请求标志寄存器的值               
     i--;
     delay_us(1000);//
  }
  while ((i!=0) && !(n&0x01) && !(n&waitFor));    /*当等待时间到时,定时器的TimerValue寄存器的值递减到零时,单CommandREG的值从其它
                                                    命令变为空闲命令时*/
  ClearBitMask(BitFramingReg,0x80);               //结束数据传输
	    
  if (i!=0)
  {  
     if(!(ReadRawRC(ErrorReg)&0x1B))          //没有发生FIFO区溢出,位冲突,奇偶校验出错,SOF出错,数据流接收到字节数出错时
     {
       status = MI_OK;
       if (n & irqEn & 0x01)
       {   status = MI_NOTAGERR;   }
       if (Command == PCD_TRANSCEIVE)         //如果命令字为发送并接受数据              
       {
         	n = ReadRawRC(FIFOLevelReg);        //读取FIFO保存的字节数
        	lastBits = ReadRawRC(ControlReg) & 0x07;   //显示最后接收到的字节的有效位的数目       
        if (lastBits)
        {   *pOutLenBit = (n-1)*8 + lastBits;   }
        else
        {   *pOutLenBit = n*8;   }
        if (n == 0)                           //如果FIFO保存的字节数为0
        {   n = 1;  }
        if (n > MAXRLEN)
        {   n = MAXRLEN;   }
        for (i=0; i<n; i++)
        {   pOutData[i] = ReadRawRC(FIFODataReg);  }      //读出卡片返回的数据
      }
     }
     else
     {   status = MI_ERR;   }
    
   }
   

   SetBitMask(ControlReg,0x80);       // stop timer now停止定时器
   WriteRawRC(CommandReg,PCD_IDLE);   //Tranceive命令不能自动中止，在任何情况下从该命令返回时必须先执行IDLE指令使RC522转入空闲态。
   return status;
}


/////////////////////////////////////////////////////////////////////
//开启天线  
//每次启动或关闭天险发射之间应至少有1ms的间隔
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn()
{
  unsigned char i;
  i = ReadRawRC(TxControlReg);
  if (!(i & 0x03))
  {
    SetBitMask(TxControlReg, 0x03);
  }
}
/////////////////////////////////////////////////////////////////////
//关闭天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff()
{
  ClearBitMask(TxControlReg, 0x03);
}
