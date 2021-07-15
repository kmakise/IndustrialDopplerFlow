 
/********************************************************************
                       VI2C_C51.C
  �˳�����I2C����ƽ̨������ʽ�����ƽ̨���ĵײ��C�ӳ���,�緢������
����������,Ӧ��λ����,���ṩ�˼���ֱ����������Ĳ������������ܷ����
���û��������Ӳ���չ.....  
  
    ע��:�����ǲ��������ʱ�ķ�������SCL����,�̶Ը߾���Ƶ��Ҫ�� 
һ�����޸�....(������1us��������,������Ƶ��ҪС��12MHZ)

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



#define  uchar unsigned char /*�궨��*/
#define  uint  unsigned int
   
#define I2C_WR		0
#define I2C_RD		1
#define	ACK			0
#define	NACK		1
#define PCF8563_ADD	(0xA2)
                      
static uchar ack;	      

/*******************************************************************
                     �����ߺ���               
����ԭ��: void  Start_I2c();  
����:       ����I2C����,������I2C��ʼ����.
  
********************************************************************/
static void Start_I2c(void)
{
    CFG_IIC;
    
    SETSDA;   /*������ʼ�����������ź�*/
    _Nop();
    SETSCL;
    _Nop();    /*��ʼ��������ʱ�����4.7us,��ʱ*/
    _Nop();
    _Nop();
    _Nop();
    _Nop();    
    CLRSDA;   /*������ʼ�ź�*/
    _Nop();    /* ��ʼ��������ʱ�����4��s*/
    _Nop();
    _Nop();
    _Nop();
    _Nop();       
    CLRSCL;   /*ǯסI2C���ߣ�׼�����ͻ�������� */
    _Nop();
    _Nop();
}

/*******************************************************************
                      �������ߺ���               
����ԭ��: void  Stop_I2c();  
����:       ����I2C����,������I2C��������.
  
********************************************************************/
static void Stop_I2c(void)
{
    CLRSDA;  /*���ͽ��������������ź�*/
    _Nop();   /*���ͽ���������ʱ���ź�*/
    SETSCL;  /*������������ʱ�����4��s*/
    _Nop();
    _Nop();
    _Nop();
    _Nop();
    _Nop();
    SETSDA;  /*����I2C���߽����ź�*/
    _Nop();
    _Nop();
    _Nop();
    _Nop();
}

/*******************************************************************
                 �ֽ����ݴ��ͺ���               
����ԭ��: void  SendByte(uchar c);
����:  ������c���ͳ�ȥ,�����ǵ�ַ,Ҳ����������,�����ȴ�Ӧ��,����
     ��״̬λ���в���.(��Ӧ����Ӧ��ʹack=0 ��)     
     ��������������ack=1; ack=0��ʾ��������Ӧ����𻵡�
********************************************************************/
static void  SendByte(uchar c)
{
    uchar BitCnt;
    
    for(BitCnt=0;BitCnt<8;BitCnt++)  /*Ҫ���͵����ݳ���Ϊ8λ*/
    {
        if(c&0x80) SETSDA;
        else       CLRSDA;
        c <<= 1;          
        SETSCL;               /*��ʱ����Ϊ�ߣ�֪ͨ��������ʼ��������λ*/
        _Nop(); 
        _Nop();               /*��֤ʱ�Ӹߵ�ƽ���ڴ���4��s*/
        _Nop();
        _Nop();
        _Nop();         
        CLRSCL; 
    }
    
    _Nop();
    _Nop();
    SETSDA;               /*8λ��������ͷ������ߣ�׼������Ӧ��λ*/
    _Nop();
    _Nop();   
    SETSCL;
    _Nop();
    _Nop();
    _Nop();
    if(READBITSDA)ack=0;     
    else ack=1;        /*�ж��Ƿ���յ�Ӧ���ź�*/
    CLRSCL;
    _Nop();
    _Nop();
}


/*******************************************************************
                 �ֽ����ݴ��ͺ���               
����ԭ��: uchar  RcvByte();
����:  �������մ���������������,���ж����ߴ���(����Ӧ���ź�)��
     ���������Ӧ������  
********************************************************************/	
static uchar  RcvByte(void)
{
    uchar retc;
    uchar BitCnt;
    
    retc=0; 
    SETSDA;             /*��������Ϊ���뷽ʽ*/
    for(BitCnt=0;BitCnt<8;BitCnt++)
    {
        _Nop();           
        CLRSCL;       /*��ʱ����Ϊ�ͣ�׼����������λ*/
        _Nop();
        _Nop();         /*ʱ�ӵ͵�ƽ���ڴ���4.7��s*/
        _Nop();
        _Nop();
        _Nop();
        SETSCL;       /*��ʱ����Ϊ��ʹ��������������Ч*/
        _Nop();
        _Nop();
        retc=retc<<1;
        if(READBITSDA)retc=retc+1; /*������λ,���յ�����λ����retc�� */
        _Nop();
        _Nop(); 
    }
    CLRSCL;    
    _Nop();
    _Nop();
    return(retc);
}


/********************************************************************
                     Ӧ���Ӻ���
ԭ��:  void Ack_I2c(bit a);
 
����:����������Ӧ���ź�,(������Ӧ����Ӧ���ź�)
********************************************************************/
static void Ack_I2c(char a)
{
    if(a==0)CLRSDA;     /*�ڴ˷���Ӧ����Ӧ���ź� */
    else SETSDA;
    _Nop();
    _Nop();
    _Nop();      
    SETSCL;
    _Nop();
    _Nop();              /*ʱ�ӵ͵�ƽ���ڴ���4��s*/
    _Nop();
    _Nop();
    _Nop();  
    CLRSCL;                /*��ʱ���ߣ�ǯסI2C�����Ա��������*/
    _Nop();
    _Nop();    
}

/*******************************************************************
                    �����ӵ�ַ���������ֽ����ݺ���               
����ԭ��: bit  ISendByte(uchar sla,ucahr c);  
����:     ���������ߵ����͵�ַ�����ݣ��������ߵ�ȫ����,��������ַsla.
           �������1��ʾ�����ɹ��������������
ע�⣺    ʹ��ǰ�����ѽ������ߡ�
*******************************************************************
static char ISendByte(uchar sla,uchar c)
{
   Start_I2c();               //��������
   SendByte(sla);            //����������ַ
     if(ack==0)return(0);
   SendByte(c);               //��������
     if(ack==0)return(0);
  Stop_I2c();                 // ��������
  return(1);
}

*/

/*******************************************************************
                    �����ӵ�ַ�������Ͷ��ֽ����ݺ���               
����ԭ��: bit  ISendStr(uchar sla,uchar suba,ucahr *s,uchar no);  
����:     ���������ߵ����͵�ַ���ӵ�ַ,���ݣ��������ߵ�ȫ����,������
          ��ַsla���ӵ�ַsuba������������sָ������ݣ�����no���ֽڡ�
           �������1��ʾ�����ɹ��������������
ע�⣺    ʹ��ǰ�����ѽ������ߡ�
*******************************************************************
static char ISendStr(uchar sla,uchar suba,uchar *s,uchar no)
{
   uchar i;

   Start_I2c();               //��������
   SendByte(sla);            //����������ַ
     if(ack==0)return(0);
   SendByte(suba);            //���������ӵ�ַ
     if(ack==0)return(0);

   for(i=0;i<no;i++)
    {   
     SendByte(*s);               //��������
       if(ack==0)return(0);
     s++;
    } 
 Stop_I2c();                 //��������
  return(1);
}
*/

/*******************************************************************
                    �����ӵ�ַ�������ֽ����ݺ���               
����ԭ��: bit  IRcvByte(uchar sla,ucahr *c);  
����:     ���������ߵ����͵�ַ�������ݣ��������ߵ�ȫ����,��������
          ַsla������ֵ��c.
           �������1��ʾ�����ɹ��������������
ע�⣺    ʹ��ǰ�����ѽ������ߡ�
*******************************************************************
static char IRcvByte(uchar sla,uchar *c)
{
   Start_I2c();                //��������
   SendByte(sla+1);           //����������ַ
     if(ack==0)return(0);
   *c=RcvByte();               //��ȡ����
     Ack_I2c(1);               //���ͷǾʹ�λ
  Stop_I2c();                  //�������� 
  return(1);
}
*/

/*******************************************************************
                    �����ӵ�ַ������ȡ���ֽ����ݺ���               
����ԭ��: bit  ISendStr(uchar sla,uchar suba,ucahr *s,uchar no);  
����:     ���������ߵ����͵�ַ���ӵ�ַ,�����ݣ��������ߵ�ȫ����,������
          ��ַsla���ӵ�ַsuba�����������ݷ���sָ��Ĵ洢������no���ֽڡ�
           �������1��ʾ�����ɹ��������������
ע�⣺    ʹ��ǰ�����ѽ������ߡ�
*******************************************************************
static char IRcvStr(uchar sla,uchar suba,uchar *s,uchar no)
{
   uchar i;

   Start_I2c();               //��������
   SendByte(sla);            //����������ַ
     if(ack==0)return(0);
   SendByte(suba);            //���������ӵ�ַ
     if(ack==0)return(0);

   Start_I2c();
   SendByte(sla+1);
      if(ack==0)return(0);

   for(i=0;i<no-1;i++)
    {   
     *s=RcvByte();               //��������
      Ack_I2c(0);                //���;ʹ�λ  
     s++;
    } 
   *s=RcvByte();
    Ack_I2c(1);                 //���ͷ�Ӧλ
 Stop_I2c();                    //�������� 
  return(1);
}
*/
//-------дPCF8563-----------
char WritePCF8563(unsigned char addr,unsigned char *buf,uchar leth)
{
    Start_I2c();               /*��������*/
    SendByte(PCF8563_ADD);            /*����������ַ*/
    if(ack==0)return(0);
    SendByte((uchar)addr);            /*���������ӵ�ַ*/
    if(ack==0)return(0);
    while(leth--)
    {
        SendByte(*buf++);               /*��������*/
        if(ack==0)return(0);
    }
    Stop_I2c();                 /*��������*/ 
    return 1;
}
//-------��PCF8563-----------
char ReadPCF8563(unsigned char addr,unsigned char *buf,uchar leth)
{
    Start_I2c();               /*��������*/
    SendByte(PCF8563_ADD);            /*����������ַ*/
    if(ack==0)return(0);
    SendByte((uchar)addr);            /*���������ӵ�ַ*/
    if(ack==0)return(0);
    
    Start_I2c();                /*��������*/
    SendByte(PCF8563_ADD+1);           /*����������ַ*/
    if(ack==0)return(0);
    
    while(leth--)
    {
        *buf++=RcvByte();               /*��ȡ����*/
        if(leth) Ack_I2c(0);               /*���;ʹ�λ*/
        else     Ack_I2c(1);                 /*���ͷ�Ӧλ*/
    }	
    Stop_I2c();                 /*��������*/ 
    return 1;
}

void ReadTime(void)
{
    unsigned char ch[7];
    ReadPCF8563(0x02,&ch[0],7);
    //���ε���Чλ
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



