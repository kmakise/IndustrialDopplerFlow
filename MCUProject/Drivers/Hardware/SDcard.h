
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SDCard_H
#define __SDCard_H
#ifdef __cplusplus
 extern "C" {
#endif
 
/* Includes ------------------------------------------------------------------*/
#include "main.h"
 
/* USER CODE BEGIN Includes */
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* USER CODE END Includes */
 
 
/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/

#define FLASH_SPIx                                 SPI1
#define FLASH_SPIx_RCC_CLK_ENABLE()                __HAL_RCC_SPI1_CLK_ENABLE()
#define FLASH_SPIx_RCC_CLK_DISABLE()               __HAL_RCC_SPI1_CLK_DISABLE()
 
#define FLASH_SPI_GPIO_ClK_ENABLE()                __HAL_RCC_GPIOA_CLK_ENABLE() 
#define FLASH_SPI_GPIO_PORT                        GPIOA
#define FLASH_SPI_SCK_PIN                          GPIO_PIN_5
#define FLASH_SPI_MISO_PIN                         GPIO_PIN_6
#define FLASH_SPI_MOSI_PIN                         GPIO_PIN_7
 
#define FLASH_SPI_CS_CLK_ENABLE()                  __HAL_RCC_GPIOA_CLK_ENABLE()    
#define FLASH_SPI_CS_PORT                          GPIOA
#define FLASH_SPI_CS_PIN                           GPIO_PIN_4
 
#define FLASH_SPI_CS_ENABLE()                      HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_Pin, GPIO_PIN_RESET)
#define FLASH_SPI_CS_DISABLE()                     HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_Pin, GPIO_PIN_SET)
 
/* 扩展变量 ------------------------------------------------------------------*/
extern SPI_HandleTypeDef hspiflash;
 
/* 函数声明 ------------------------------------------------------------------*/
 
 
/* USER CODE BEGIN Private defines */
 
/* USER CODE END Private defines */
#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
 
 
/* Private define ------------------------------------------------------------*/
/* SD卡类型定义 */
#define SD_TYPE_MMC     0
#define SD_TYPE_V1      1
#define SD_TYPE_V2      2
#define SD_TYPE_V2HC    4
 
/* SPI总线速度设置*/
#define SPI_SPEED_LOW   0
#define SPI_SPEED_HIGH  1
 
/* SD传输数据结束后是否释放总线宏定义 */
#define NO_RELEASE      0
#define RELEASE         1
 
/* SD卡指令表 */
#define CMD0    0       //卡复位
#define CMD9    9       //命令9 ，读CSD数据
#define CMD10   10      //命令10，读CID数据
#define CMD12   12      //命令12，停止数据传输
#define CMD16   16      //命令16，设置SectorSize 应返回0x00
#define CMD17   17      //命令17，读sector
#define CMD18   18      //命令18，读Multi sector
#define ACMD23  23      //命令23，设置多sector写入前预先擦除N个block
#define CMD24   24      //命令24，写sector
#define CMD25   25      //命令25，写Multi sector
#define ACMD41  41      //命令41，应返回0x00
#define CMD55   55      //命令55，应返回0x01
#define CMD58   58      //命令58，读OCR信息
#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00
 
 
 
 
/* Private macro -------------------------------------------------------------*/
//SD卡CS片选使能端操作：
#define SD_CS_ENABLE()      GPIO_ResetBits(GPIOA,GPIO_PIN_4)   //选中SD卡
#define SD_CS_DISABLE()     GPIO_SetBits(GPIOA,GPIO_PIN_4)     //不选中SD卡
//#define SD_PWR_ON()         GPIO_ResetBits(GPIOD,GPIO_Pin_10)  //SD卡上电
//#define SD_PWR_OFF()        GPIO_SetBits(GPIOD,GPIO_Pin_10)    //SD卡断电
//#define SD_DET()           !GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) //检测有卡
                                                                    //1-有 0-无
 
/* Private function prototypes -----------------------------------------------*/
void SPI_Configuration(void);
void SPI_SetSpeed(u8 SpeedSet);
 
u8 SPI_ReadWriteByte(u8 TxData);                //SPI总线读写一个字节
u8 SD_WaitReady(void);                          //等待SD卡就绪
u8 SD_SendCommand(u8 cmd, u32 arg, u8 crc);     //SD卡发送一个命令
u8 SD_SendCommand_NoDeassert(u8 cmd, u32 arg, u8 crc);
u8 SD_Init(void);                               //SD卡初始化
                                                //
u8 SD_ReceiveData(u8 *data, u16 len, u8 release);//SD卡读数据
u8 SD_GetCID(u8 *cid_data);                     //读SD卡CID
u8 SD_GetCSD(u8 *csd_data);                     //读SD卡CSD
u32 SD_GetCapacity(void);                       //取SD卡容量
 
u8 SD_ReadSingleBlock(u32 sector, u8 *buffer);  //读一个sector
u8 SD_WriteSingleBlock(u32 sector, const u8 *buffer); //写一个sector
u8 SD_ReadMultiBlock(u32 sector, u8 *buffer, u8 count); //读多个sector
u8 SD_WriteMultiBlock(u32 sector, const u8 *data, u8 count);  //写多个sector
/* USER CODE BEGIN Prototypes */
 
extern u8 SD_Init(void);
 
/* USER CODE END Prototypes */
 
#ifdef __cplusplus
}
#endif
#endif /*__SDCard_H */
