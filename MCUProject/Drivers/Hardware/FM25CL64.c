/*******************************************************************************
                             铁电存储
*******************************************************************************/
#include "FM25CL64.h"
#include "stm32f1xx_hal.h"
#include "main.h"
#include "spi.h"

#define FM25CL64CS_H       HAL_GPIO_WritePin(FMCS_GPIO_Port,FMCS_Pin,GPIO_PIN_SET)   //S片选引脚输出高电平；
#define FM25CL64CS_L       HAL_GPIO_WritePin(FMCS_GPIO_Port,FMCS_Pin,GPIO_PIN_RESET) //S片选引脚输出低电平；

#define FM25CL64SIMO_H     HAL_GPIO_WritePin(FMSI_GPIO_Port,FMSI_Pin,GPIO_PIN_SET)  //SIMO引脚输出高电平；
#define FM25CL64SIMO_L     HAL_GPIO_WritePin(FMSI_GPIO_Port,FMSI_Pin,GPIO_PIN_RESET) //SIMO引脚输出低电平；

#define FM25CL64SOMI       HAL_GPIO_ReadPin(FMSO_GPIO_Port,FMSO_Pin) //SIMO引脚输入

#define FM25CL64CLK_H      HAL_GPIO_WritePin(FMCK_GPIO_Port,FMCK_Pin,GPIO_PIN_SET) //CLK引脚输出高电平；
#define FM25CL64CLK_L      HAL_GPIO_WritePin(FMCK_GPIO_Port,FMCK_Pin,GPIO_PIN_RESET) //CLK引脚输出低电平；

//#define FM25V10HOLD_H     GPIO_SetBits(GPIOD,GPIO_Pin_8)         //Hold引脚输出高电平；
//#define FM25V10HOLD_L     GPIO_ResetBits(GPIOD,GPIO_Pin_8)       //Hold引脚输出低电平；

#define FM25CL64_WREN       0x06                   //Set Write Enable Latch
#define FM25CL64_WRDI       0x04                   //Write Disable 
#define FM25CL64_RDSR       0x05                   //Read Status Register 
#define FM25CL64_WRSR       0x01                   //Write Status Register
#define FM25CL64_READ       0x03                   //Read Memory Data
#define FM25CL64_WRITE      0x02                   //Write Memory Data

//#define FM25CL64_FSTRD      0x0B                   //Fast Read Memory Data
//#define FM25CL64_SLEEP      0xB9                   //Enter Sleep Mode

//#define FM25CL64_RDID       0x9F                   //Read Device ID
//#define FM25CL64_SNR        0xC3                   //Read S/N


//发送一个字节
void FM_Send(char data)
{
    unsigned char cnt;
    
    for(cnt=8; cnt>0; cnt-- )
    {
        if(data&0x80 )
        {
            FM25CL64SIMO_H;
        }
        else
        {
            FM25CL64SIMO_L;
        }
        FM25CL64CLK_L;
        data <<= 1;
        FM25CL64CLK_H;
    }
}

//设置可以将数据写入铁电
//The WREN command must be issued prior to any write operation.
void Write_Enable(void)
{
    FM25CL64CS_L;
    FM_Send(FM25CL64_WREN);
    FM25CL64CS_H;
}

//设置不可以将数据写入铁电
void Write_Disable(void)
{
    FM25CL64CS_L;
    FM_Send(FM25CL64_WRDI);
    FM25CL64CS_H;
}

//接收一个字节
unsigned char FM_Rec(void)
{
    unsigned char cnt;
    unsigned char temp;
    
    temp = 0;
    for(cnt=8; cnt>0; cnt--)
    {
        FM25CL64CLK_L;
        temp <<= 1;
        if(FM25CL64SOMI)
        {
            temp |= 0x01;
        }
        FM25CL64CLK_H;
    }
    return(temp);
}

//单字节写FM
void Write_FM25CL64_Byte(unsigned int addr,char c)
{
    Write_Enable();
    FM25CL64CS_L;
    FM_Send(FM25CL64_WRITE);
    FM_Send(addr>>8);
    FM_Send(addr&0xff);
    FM_Send(c);
    FM25CL64CS_H;
    Write_Disable();
}


//单字节读FM
unsigned char Read_FM25CL64_Byte(unsigned int addr)
{
    char rec;
    FM25CL64CS_L;
    FM_Send(FM25CL64_READ);
    FM_Send(addr>>8);
    FM_Send(addr&0xff);
    rec = FM_Rec();
    FM25CL64CS_H;
    return rec;
}

/*
//多字节写FM
void WriteFM(unsigned int addr,void *pTx,unsigned int n)
{
    char *Pch;
    Pch = pTx;   
    while(n--)
    {
        Write_FM25CL64_Byte(addr++,*Pch++);
    }
}
//多字节读FM
void ReadFM(unsigned int addr,void *pRec,unsigned int n)
{
    char *Pch;
    Pch = pRec; 
    while(n--)
    {
        *Pch++ = Read_FM25CL64_Byte(addr++);
    }
}
*/
//多字节写FM
void WriteFM(unsigned int addr,void *pTx,unsigned int n)
{
    char *Pch;
    Pch = pTx;   
  
    Write_Enable();
    FM25CL64CS_L;
    FM_Send(FM25CL64_WRITE);
    FM_Send(addr>>8);
    FM_Send(addr&0xff);
    while(n--)
    {
        FM_Send(*Pch++);
    }
    FM25CL64CS_H;
    Write_Disable();
}
//多字节读FM
void ReadFM(unsigned int addr,void *pRec,unsigned int n)
{
    char *Pch;
    Pch = pRec; 
    
    FM25CL64CS_L;
    FM_Send(FM25CL64_READ);
    FM_Send(addr>>8);
    FM_Send(addr&0xff);
    while(n--)
    {
        *Pch++ = FM_Rec();
    }
    FM25CL64CS_H;
}

//FM 填充
void WriteFMFill(unsigned int addr,char data,unsigned int n)
{  
    Write_Enable();
    FM25CL64CS_L;
    FM_Send(FM25CL64_WRITE);
    FM_Send(addr>>8);
    FM_Send(addr&0xff);
    while(n--)
    {
        FM_Send(data);
    }
    FM25CL64CS_H;
    Write_Disable();
}

