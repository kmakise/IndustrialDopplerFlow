 
/********************************************************************
                       VI2C_C51.C
  此程序是I2C操作平台（主方式的软件平台）的底层的C子程序,如发送数据
及接收数据,应答位发送,并提供了几个直接面对器件的操作函数，它很方便的
与用户程序连接并扩展.....  
  
    注意:函数是采用软件延时的方法产生SCL脉冲,固对高晶振频率要作 
一定的修改....(本例是1us机器周期,即晶振频率要小于12MHZ)

********************************************************************/
#include "pcf8563.h"
#include "golbaldata.h"
#include "main.h"

#define SETSCL HAL_GPIO_WritePin(RTCSCL_GPIO_Port,RTCSCL_Pin,GPIO_PIN_SET)
#define CLRSCL HAL_GPIO_WritePin(RTCSCL_GPIO_Port,RTCSCL_Pin,GPIO_PIN_RESET)
#define SETSDA HAL_GPIO_WritePin(RTCSDA_GPIO_Port,RTCSDA_Pin,GPIO_PIN_SET)
#define CLRSDA HAL_GPIO_WritePin(RTCSDA_GPIO_Port,RTCSDA_Pin,GPIO_PIN_RESET)
#define READBITSDA HAL_GPIO_ReadPin(RTCSDA_GPIO_Port,RTCSDA_Pin)
#define CFG_IIC //P8OUT &= ~(1<<0); P7OUT &= ~(1<<3)
#define _Nop() for(int i = 0;i < 100;i++); 



#define  uchar unsigned char /*宏定义*/
#define  uint  unsigned int
   
#define I2C_WR		0
#define I2C_RD		1
#define	ACK			0
#define	NACK		1
#define PCF8563_ADD	(0xA2)
                      
static uchar ack;	      

/*******************************************************************
                     起动总线函数               
函数原型: void  Start_I2c();  
功能:       启动I2C总线,即发送I2C起始条件.
  
********************************************************************/
static void Start_I2c(void)
{
    CFG_IIC;
    
    SETSDA;   /*发送起始条件的数据信号*/
    _Nop();
    SETSCL;
    _Nop();    /*起始条件建立时间大于4.7us,延时*/
    _Nop();
    _Nop();
    _Nop();
    _Nop();    
    CLRSDA;   /*发送起始信号*/
    _Nop();    /* 起始条件锁定时间大于4μs*/
    _Nop();
    _Nop();
    _Nop();
    _Nop();       
    CLRSCL;   /*钳住I2C总线，准备发送或接收数据 */
    _Nop();
    _Nop();
}

/*******************************************************************
                      结束总线函数               
函数原型: void  Stop_I2c();  
功能:       结束I2C总线,即发送I2C结束条件.
  
********************************************************************/
static void Stop_I2c(void)
{
    CLRSDA;  /*发送结束条件的数据信号*/
    _Nop();   /*发送结束条件的时钟信号*/
    SETSCL;  /*结束条件建立时间大于4μs*/
    _Nop();
    _Nop();
    _Nop();
    _Nop();
    _Nop();
    SETSDA;  /*发送I2C总线结束信号*/
    _Nop();
    _Nop();
    _Nop();
    _Nop();
}

/*******************************************************************
                 字节数据传送函数               
函数原型: void  SendByte(uchar c);
功能:  将数据c发送出去,可以是地址,也可以是数据,发完后等待应答,并对
     此状态位进行操作.(不应答或非应答都使ack=0 假)     
     发送数据正常，ack=1; ack=0表示被控器无应答或损坏。
********************************************************************/
static void  SendByte(uchar c)
{
    uchar BitCnt;
    
    for(BitCnt=0;BitCnt<8;BitCnt++)  /*要传送的数据长度为8位*/
    {
        if(c&0x80) SETSDA;
        else       CLRSDA;
        c <<= 1;          
        SETSCL;               /*置时钟线为高，通知被控器开始接收数据位*/
        _Nop(); 
        _Nop();               /*保证时钟高电平周期大于4μs*/
        _Nop();
        _Nop();
        _Nop();         
        CLRSCL; 
    }
    
    _Nop();
    _Nop();
    SETSDA;               /*8位发送完后释放数据线，准备接收应答位*/
    _Nop();
    _Nop();   
    SETSCL;
    _Nop();
    _Nop();
    _Nop();
    if(READBITSDA)ack=0;     
    else ack=1;        /*判断是否接收到应答信号*/
    CLRSCL;
    _Nop();
    _Nop();
}


/*******************************************************************
                 字节数据传送函数               
函数原型: uchar  RcvByte();
功能:  用来接收从器件传来的数据,并判断总线错误(不发应答信号)，
     发完后请用应答函数。  
********************************************************************/	
static uchar  RcvByte(void)
{
    uchar retc;
    uchar BitCnt;
    
    retc=0; 
    SETSDA;             /*置数据线为输入方式*/
    for(BitCnt=0;BitCnt<8;BitCnt++)
    {
        _Nop();           
        CLRSCL;       /*置时钟线为低，准备接收数据位*/
        _Nop();
        _Nop();         /*时钟低电平周期大于4.7μs*/
        _Nop();
        _Nop();
        _Nop();
        SETSCL;       /*置时钟线为高使数据线上数据有效*/
        _Nop();
        _Nop();
        retc=retc<<1;
        if(READBITSDA)retc=retc+1; /*读数据位,接收的数据位放入retc中 */
        _Nop();
        _Nop(); 
    }
    CLRSCL;    
    _Nop();
    _Nop();
    return(retc);
}


/********************************************************************
                     应答子函数
原型:  void Ack_I2c(bit a);
 
功能:主控器进行应答信号,(可以是应答或非应答信号)
********************************************************************/
static void Ack_I2c(char a)
{
    if(a==0)CLRSDA;     /*在此发出应答或非应答信号 */
    else SETSDA;
    _Nop();
    _Nop();
    _Nop();      
    SETSCL;
    _Nop();
    _Nop();              /*时钟低电平周期大于4μs*/
    _Nop();
    _Nop();
    _Nop();  
    CLRSCL;                /*清时钟线，钳住I2C总线以便继续接收*/
    _Nop();
    _Nop();    
}

/*******************************************************************
                    向无子地址器件发送字节数据函数               
函数原型: bit  ISendByte(uchar sla,ucahr c);  
功能:     从启动总线到发送地址，数据，结束总线的全过程,从器件地址sla.
           如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
*******************************************************************
static char ISendByte(uchar sla,uchar c)
{
   Start_I2c();               //启动总线
   SendByte(sla);            //发送器件地址
     if(ack==0)return(0);
   SendByte(c);               //发送数据
     if(ack==0)return(0);
  Stop_I2c();                 // 结束总线
  return(1);
}

*/

/*******************************************************************
                    向有子地址器件发送多字节数据函数               
函数原型: bit  ISendStr(uchar sla,uchar suba,ucahr *s,uchar no);  
功能:     从启动总线到发送地址，子地址,数据，结束总线的全过程,从器件
          地址sla，子地址suba，发送内容是s指向的内容，发送no个字节。
           如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
*******************************************************************
static char ISendStr(uchar sla,uchar suba,uchar *s,uchar no)
{
   uchar i;

   Start_I2c();               //启动总线
   SendByte(sla);            //发送器件地址
     if(ack==0)return(0);
   SendByte(suba);            //发送器件子地址
     if(ack==0)return(0);

   for(i=0;i<no;i++)
    {   
     SendByte(*s);               //发送数据
       if(ack==0)return(0);
     s++;
    } 
 Stop_I2c();                 //结束总线
  return(1);
}
*/

/*******************************************************************
                    向无子地址器件读字节数据函数               
函数原型: bit  IRcvByte(uchar sla,ucahr *c);  
功能:     从启动总线到发送地址，读数据，结束总线的全过程,从器件地
          址sla，返回值在c.
           如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
*******************************************************************
static char IRcvByte(uchar sla,uchar *c)
{
   Start_I2c();                //启动总线
   SendByte(sla+1);           //发送器件地址
     if(ack==0)return(0);
   *c=RcvByte();               //读取数据
     Ack_I2c(1);               //发送非就答位
  Stop_I2c();                  //结束总线 
  return(1);
}
*/

/*******************************************************************
                    向有子地址器件读取多字节数据函数               
函数原型: bit  ISendStr(uchar sla,uchar suba,ucahr *s,uchar no);  
功能:     从启动总线到发送地址，子地址,读数据，结束总线的全过程,从器件
          地址sla，子地址suba，读出的内容放入s指向的存储区，读no个字节。
           如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
*******************************************************************
static char IRcvStr(uchar sla,uchar suba,uchar *s,uchar no)
{
   uchar i;

   Start_I2c();               //启动总线
   SendByte(sla);            //发送器件地址
     if(ack==0)return(0);
   SendByte(suba);            //发送器件子地址
     if(ack==0)return(0);

   Start_I2c();
   SendByte(sla+1);
      if(ack==0)return(0);

   for(i=0;i<no-1;i++)
    {   
     *s=RcvByte();               //发送数据
      Ack_I2c(0);                //发送就答位  
     s++;
    } 
   *s=RcvByte();
    Ack_I2c(1);                 //发送非应位
 Stop_I2c();                    //结束总线 
  return(1);
}
*/
//-------写PCF8563-----------
char WritePCF8563(unsigned char addr,unsigned char *buf,uchar leth)
{
    Start_I2c();               /*启动总线*/
    SendByte(PCF8563_ADD);            /*发送器件地址*/
    if(ack==0)return(0);
    SendByte((uchar)addr);            /*发送器件子地址*/
    if(ack==0)return(0);
    while(leth--)
    {
        SendByte(*buf++);               /*发送数据*/
        if(ack==0)return(0);
    }
    Stop_I2c();                 /*结束总线*/ 
    return 1;
}
//-------读PCF8563-----------
char ReadPCF8563(unsigned char addr,unsigned char *buf,uchar leth)
{
    Start_I2c();               /*启动总线*/
    SendByte(PCF8563_ADD);            /*发送器件地址*/
    if(ack==0)return(0);
    SendByte((uchar)addr);            /*发送器件子地址*/
    if(ack==0)return(0);
    
    Start_I2c();                /*启动总线*/
    SendByte(PCF8563_ADD+1);           /*发送器件地址*/
    if(ack==0)return(0);
    
    while(leth--)
    {
        *buf++=RcvByte();               /*读取数据*/
        if(leth) Ack_I2c(0);               /*发送就答位*/
        else     Ack_I2c(1);                 /*发送非应位*/
    }	
    Stop_I2c();                 /*结束总线*/ 
    return 1;
}

void ReadTime(void)
{
    unsigned char ch[7];
    ReadPCF8563(0x02,&ch[0],7);
    //屏蔽掉无效位
    ch[0] &= 0x7f;
    ch[1] &= 0x7f;
    ch[2] &= 0x3f;
    ch[3] &= 0x3f;
    ch[4] &= 0x07;
    ch[5] &= 0x1f;
    //ch[6] &= 0xff;
    Time.sec  = (ch[0]&0x0f)+(ch[0]>>4)*10;       
    Time.min  = (ch[1]&0x0f)+(ch[1]>>4)*10;
    Time.hour = (ch[2]&0x0f)+(ch[2]>>4)*10;      
    Time.day  = (ch[3]&0x0f)+(ch[3]>>4)*10; 
    Time.mon  = (ch[5]&0x0f)+(ch[5]>>4)*10;  
    Time.year = (ch[6]&0x0f)+(ch[6]>>4)*10;
}

void WriteTime(void)
{
    unsigned char ch[7];
    ch[0] = Time.sec%10 + Time.sec/10*16;
    ch[1] = Time.min%10 + Time.min/10*16;
    ch[2] = Time.hour%10 + Time.hour/10*16;
    ch[3] = Time.day%10 + Time.day/10*16;
    ch[4] = 0x00;
    ch[5] = Time.mon%10 + Time.mon/10*16;
    ch[6] = Time.year%10 + Time.year/10*16;
    
    WritePCF8563(0x02,&ch[0],7);
}



