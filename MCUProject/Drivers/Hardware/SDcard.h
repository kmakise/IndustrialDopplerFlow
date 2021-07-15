
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SDCard_H
#define __SDCard_H
#ifdef __cplusplus
 extern "C" {
#endif
 
/* Includes ------------------------------------------------------------------*/
#include "main.h"
 
/* USER CODE BEGIN Includes */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* USER CODE END Includes */
 
 
/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/

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
 
/* ��չ���� ------------------------------------------------------------------*/
extern SPI_HandleTypeDef hspiflash;
 
/* �������� ------------------------------------------------------------------*/
 
 
/* USER CODE BEGIN Private defines */
 
/* USER CODE END Private defines */
#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
 
 
/* Private define ------------------------------------------------------------*/
/* SD�����Ͷ��� */
#define SD_TYPE_MMC     0
#define SD_TYPE_V1      1
#define SD_TYPE_V2      2
#define SD_TYPE_V2HC    4
 
/* SPI�����ٶ�����*/
#define SPI_SPEED_LOW   0
#define SPI_SPEED_HIGH  1
 
/* SD�������ݽ������Ƿ��ͷ����ߺ궨�� */
#define NO_RELEASE      0
#define RELEASE         1
 
/* SD��ָ��� */
#define CMD0    0       //����λ
#define CMD9    9       //����9 ����CSD����
#define CMD10   10      //����10����CID����
#define CMD12   12      //����12��ֹͣ���ݴ���
#define CMD16   16      //����16������SectorSize Ӧ����0x00
#define CMD17   17      //����17����sector
#define CMD18   18      //����18����Multi sector
#define ACMD23  23      //����23�����ö�sectorд��ǰԤ�Ȳ���N��block
#define CMD24   24      //����24��дsector
#define CMD25   25      //����25��дMulti sector
#define ACMD41  41      //����41��Ӧ����0x00
#define CMD55   55      //����55��Ӧ����0x01
#define CMD58   58      //����58����OCR��Ϣ
#define CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00
 
 
 
 
/* Private macro -------------------------------------------------------------*/
//SD��CSƬѡʹ�ܶ˲�����
#define SD_CS_ENABLE()      GPIO_ResetBits(GPIOA,GPIO_PIN_4)   //ѡ��SD��
#define SD_CS_DISABLE()     GPIO_SetBits(GPIOA,GPIO_PIN_4)     //��ѡ��SD��
//#define SD_PWR_ON()         GPIO_ResetBits(GPIOD,GPIO_Pin_10)  //SD���ϵ�
//#define SD_PWR_OFF()        GPIO_SetBits(GPIOD,GPIO_Pin_10)    //SD���ϵ�
//#define SD_DET()           !GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) //����п�
                                                                    //1-�� 0-��
 
/* Private function prototypes -----------------------------------------------*/
void SPI_Configuration(void);
void SPI_SetSpeed(u8 SpeedSet);
 
u8 SPI_ReadWriteByte(u8 TxData);                //SPI���߶�дһ���ֽ�
u8 SD_WaitReady(void);                          //�ȴ�SD������
u8 SD_SendCommand(u8 cmd, u32 arg, u8 crc);     //SD������һ������
u8 SD_SendCommand_NoDeassert(u8 cmd, u32 arg, u8 crc);
u8 SD_Init(void);                               //SD����ʼ��
                                                //
u8 SD_ReceiveData(u8 *data, u16 len, u8 release);//SD��������
u8 SD_GetCID(u8 *cid_data);                     //��SD��CID
u8 SD_GetCSD(u8 *csd_data);                     //��SD��CSD
u32 SD_GetCapacity(void);                       //ȡSD������
 
u8 SD_ReadSingleBlock(u32 sector, u8 *buffer);  //��һ��sector
u8 SD_WriteSingleBlock(u32 sector, const u8 *buffer); //дһ��sector
u8 SD_ReadMultiBlock(u32 sector, u8 *buffer, u8 count); //�����sector
u8 SD_WriteMultiBlock(u32 sector, const u8 *data, u8 count);  //д���sector
/* USER CODE BEGIN Prototypes */
 
extern u8 SD_Init(void);
 
/* USER CODE END Prototypes */
 
#ifdef __cplusplus
}
#endif
#endif /*__SDCard_H */
