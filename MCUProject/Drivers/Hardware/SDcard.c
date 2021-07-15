#include "main.h"
#include "SDCard.h"
 
/* USER CODE BEGIN 0 */
 
/* USER CODE END 0 */
 
/* USART1 init function */
/**************************************************************************/
#define Dummy_Byte                      0xFF
/* ˽�б��� ------------------------------------------------------------------*/
SPI_HandleTypeDef hspiflash;
extern SPI_HandleTypeDef hspi1;
uint8_t  SD_Type=0;
/* function ------------------------------------------------------------------*/
/**
  * ��������: �Ӵ���Flash��ȡһ���ֽ�����
  * �������: ��
  * �� �� ֵ: uint8_t����ȡ��������
  * ˵    ����This function must be used only if the Start_Read_Sequence
  *           function has been previously called.
  */
uint8_t SPI_FLASH_ReadByte(void)
{
  uint8_t d_read,d_send[5];
	d_send[0]=Dummy_Byte;
  if(HAL_SPI_TransmitReceive(&hspiflash,d_send,&d_read,1,0xFFFFFF)!=HAL_OK)
    d_read=Dummy_Byte;
  
  return d_read;    
}
 
 
 
void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data)
{
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  
  /* Write in the DR register the data to be sent */
  SPIx->DR = Data;
}
uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx)
{
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
	
//	assert_param(IS_SPI_DIRECTION_2LINES_OR_1LINE(hspi->Init.Direction));
  /* Return the data in the DR register */
  return SPIx->DR;
}
/**
  * ��������: ������Flash��ȡд��һ���ֽ����ݲ�����һ���ֽ�����
  * �������: byte������������
  * �� �� ֵ: uint8_t�����յ�������
  * ˵    ������
  */
uint8_t SPI_FLASH_SendByte(uint8_t byte)
{
  uint8_t d_read,d_send[5];
//  if(HAL_SPI_TransmitReceive(&hspiflash,&d_send,&d_read,1,0xFFFFFF)!=HAL_OK)
//	{
//		d_read=Dummy_Byte;
//	}
   //�ȴ����ͻ�������
//    while(__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE));
    //��һ���ֽ�
//    SPI_I2S_SendData(SPI1, d_send);
	d_send[0] = byte;
	HAL_SPI_Transmit(&hspi1,d_send,1,1000);
//		HAL_SPI_Receive(&hspi1,&d_read,1,1000);
    //�ȴ����ݽ���
    while(__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_RXNE));
    //ȡ����
    d_read = SPI_I2S_ReceiveData(SPI1);
  return d_read; 
}
/*******************************************************************************
* Function Name  : SD_WaitReady
* Description    : �ȴ�SD��Ready
* Input          : None
* Output         : None
* Return         : uint8_t 
*                   0�� �ɹ�
*                   other��ʧ��
*******************************************************************************/
uint8_t SD_WaitReady(void)
{
    uint8_t r1;
    uint16_t retry;
    retry = 0;
    do
    {
        r1 = SPI_FLASH_SendByte(0xFF);
        if(retry==0xfffe)
        {
            return 1;
        }
    }while(r1!=0xFF);
    return 0;
}
/*******************************************************************************
* Function Name  : SD_SendCommand
* Description    : ��SD������һ������
* Input          : uint8_t cmd   ���� 
*                  uint32_t arg  �������
*                  uint8_t crc   crcУ��ֵ
* Output         : None
* Return         : uint8_t r1 SD�����ص���Ӧ
*******************************************************************************/
uint8_t SD_SendCommand(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    unsigned char r1;
    unsigned char Retry = 0;
    //????????
    SPI_FLASH_SendByte(0xff);
    //Ƭѡ���õͣ�ѡ��SD��
    FLASH_SPI_CS_ENABLE();
    //����
    SPI_FLASH_SendByte(cmd | 0x40);                         //�ֱ�д������
    SPI_FLASH_SendByte(arg >> 24);
    SPI_FLASH_SendByte(arg >> 16);
    SPI_FLASH_SendByte(arg >> 8);
    SPI_FLASH_SendByte(arg);
    SPI_FLASH_SendByte(crc);
    
    //�ȴ���Ӧ����ʱ�˳�
    while((r1 = SPI_FLASH_SendByte(0xFF))==0xFF)
    {
        Retry++;
        if(Retry > 200)
        {
            break;
        }
    }
    
    //�ر�Ƭѡ
    FLASH_SPI_CS_DISABLE();
    //�������϶�������8��ʱ�ӣ���SD�����ʣ�µĹ���
    SPI_FLASH_SendByte(0xFF);
    //����״ֵ̬
    return r1;
}
/*******************************************************************************
* Function Name  : SD_SendCommand_NoDeassert
* Description    : ��SD������һ������(�����ǲ�ʧ��Ƭѡ�����к������ݴ�����
* Input          : uint8_t cmd   ���� 
*                  uint32_t arg  �������
*                  uint8_t crc   crcУ��ֵ
* Output         : None
* Return         : uint8_t r1 SD�����ص���Ӧ
*******************************************************************************/
uint8_t SD_SendCommand_NoDeassert(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    unsigned char r1;
    unsigned char Retry = 0;
    //????????
    SPI_FLASH_SendByte(0xff);
    //Ƭѡ���õͣ�ѡ��SD��
    FLASH_SPI_CS_ENABLE();
    //����
    SPI_FLASH_SendByte(cmd | 0x40);                         //�ֱ�д������
    SPI_FLASH_SendByte(arg >> 24);
    SPI_FLASH_SendByte(arg >> 16);
    SPI_FLASH_SendByte(arg >> 8);
    SPI_FLASH_SendByte(arg);
    SPI_FLASH_SendByte(crc);
    //�ȴ���Ӧ����ʱ�˳�
    while((r1 = SPI_FLASH_SendByte(0xFF))==0xFF)
    {
        Retry++;
        if(Retry > 200)
        {
            break;
        }
    }
    //������Ӧֵ
    return r1;
}
void SPI_SetSpeed(uint8_t SpeedSet)
{
	hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_HARD_OUTPUT;
	//����ٶ���������0�������ģʽ����0�����ģʽ
	if(SpeedSet==SPI_SPEED_LOW)
	{
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	}
	else
	{
		hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	}
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
	
//  if(SpeedSet==SPI_SPEED_LOW)
//	{
//  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
//	}
//	else
//	{
//		hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
//	}
//	HAL_SPI_Init(&hspi1);
	
	
	//moon.mp3: 4707774 Byte size Ŀ���ļ� ��Ϊbuffer[512]     
	//speed:ʵ��������ݣ�����ٶ� 392314 Byte/S��
	//Prescaler_128, 59592 Byte/S
	//Prescaler_64, 104617 Byte/S
	//Prescaler_32, 168134 Byte/S    162337 Byte/S
	//Prescaler_16, 261543 Byte/S    247777 Byte/S
	//Prescaler_8,  313851 Byte/S    336269 Byte/S
	//Prescaler_4,  392314 Byte/S    392314 Byte/S
	//Prescaler_2,  392314 Byte/S
}
/*******************************************************************************
* Function Name  : SD_Init
* Description    : ��ʼ��SD��
* Input          : None
* Output         : None
* Return         : uint8_t 
*                  0��NO_ERR
*                  1��TIME_OUT
*                  99��NO_CARD
*******************************************************************************/
uint8_t SD_Init(void)
{
    uint16_t i;      // ����ѭ������
    uint8_t r1;      // ���SD���ķ���ֵ
    uint16_t retry;  // �������г�ʱ����
    uint8_t buff[6];
    //���û�м�⵽�����룬ֱ���˳������ش����־
//    if(!SD_DET())
//    {
//        //return 99;        
//        return STA_NODISK;  //  FatFS�����־��û�в������
//    }
    //SD���ϵ�
    //SD_PWR_ON();
    // ����ʱ���ȴ�SD���ϵ����
    //for(i=0;i<0xf00;i++);
	/******************************************************* 
	//����ط�Ҫ��һ��,����SPI�ٶ�Ϊ���١� 
	//Ϊʲô�еĿ������أ���ΪSPI��ʼ��ʱ�ǵ��ٵģ�SD����ʼ�� 
	//��ɺ�����Ϊ���٣��еĿ�ֻҪ��ʼ��һ�ξ��У������ok�� 
	//���еĿ���Ҫ��θ�λ���Ǻǣ�����ط�����һ�䣬 
	//���ֿ����ò��ɿ��� 
	*******************************************************/ 
    SPI_SetSpeed(0); //����SPI�ٶ�Ϊ���� 
    //�Ȳ���>74�����壬��SD���Լ���ʼ�����
    for(i=0;i<100;i++)
    {
        SPI_FLASH_SendByte(0xFF);
    }
    //-----------------SD����λ��idle��ʼ-----------------
    //ѭ����������CMD0��ֱ��SD������0x01,����IDLE״̬
    //��ʱ��ֱ���˳�
    retry = 0;
    do
    {
        //����CMD0����SD������IDLE״̬
        r1 = SD_SendCommand(CMD0, 0, 0x95);
        retry++;
    }while((r1 != 0x01) && (retry<200));
    //����ѭ���󣬼��ԭ�򣺳�ʼ���ɹ���or ���Գ�ʱ��
    if(retry==200)
    {
        return 1;   //��ʱ����1
    }
    //-----------------SD����λ��idle����-----------------
    //��ȡ��Ƭ��SD�汾��Ϣ
    r1 = SD_SendCommand_NoDeassert(8, 0x1aa, 0x87);
    //�����Ƭ�汾��Ϣ��v1.0�汾�ģ���r1=0x05����������³�ʼ��
    if(r1 == 0x05)
    {
				//printf("\r\n SD���汾��Ϣ:V1.0 \r\n");
        //���ÿ�����ΪSDV1.0����������⵽ΪMMC�������޸�ΪMMC
        SD_Type = SD_TYPE_V1;
        //�����V1.0����CMD8ָ���û�к�������
        //Ƭѡ�øߣ�������������
        FLASH_SPI_CS_DISABLE();
        //�෢8��CLK����SD������������
        SPI_FLASH_SendByte(0xFF);
        //-----------------SD����MMC����ʼ����ʼ-----------------
        //������ʼ��ָ��CMD55+ACMD41
        // �����Ӧ��˵����SD�����ҳ�ʼ�����
        // û�л�Ӧ��˵����MMC�������������Ӧ��ʼ��
        retry = 0;
        do
        {
            //�ȷ�CMD55��Ӧ����0x01���������
            r1 = SD_SendCommand(CMD55, 0, 0);
            if(r1 != 0x01)
            {
                return r1;  
            }
            //�õ���ȷ��Ӧ�󣬷�ACMD41��Ӧ�õ�����ֵ0x00����������200��
            r1 = SD_SendCommand(ACMD41, 0, 0);
            retry++;
        }while((r1!=0x00) && (retry<400));
        // �ж��ǳ�ʱ���ǵõ���ȷ��Ӧ
        // ���л�Ӧ����SD����û�л�Ӧ����MMC��
				
        //----------MMC�������ʼ��������ʼ------------
        if(retry==400)
        {
						//printf("\r\n SD����Ϣ: MMC�� \r\n");
            retry = 0;
            //����MMC����ʼ�����û�в��ԣ�
            do
            {
                r1 = SD_SendCommand(1, 0, 0);
                retry++;
            }while((r1!=0x00)&& (retry<400));
            if(retry==400)
            {
                return 1;   //MMC����ʼ����ʱ
            }
            //д�뿨����
            SD_Type = SD_TYPE_MMC;
        }
				else
				{
					//printf("\r\n SD����Ϣ: SD�� \r\n");
				}
        //----------MMC�������ʼ����������------------
        
        //����SPIΪ����ģʽ
        SPI_SetSpeed(1);
				SPI_FLASH_SendByte(0xFF);
        
        //��ֹCRCУ��
        /*
				r1 = SD_SendCommand(CMD59, 0, 0x01);
        if(r1 != 0x00)
        {
            return r1;  //������󣬷���r1
        }
        */    
        //����Sector Size
        r1 = SD_SendCommand(CMD16, 512, 0xff);
        if(r1 != 0x00)
        {
            return r1;  //������󣬷���r1
        }
        //-----------------SD����MMC����ʼ������-----------------
    }//SD��ΪV1.0�汾�ĳ�ʼ������
    
    //������V2.0���ĳ�ʼ��
    //������Ҫ��ȡOCR���ݣ��ж���SD2.0����SD2.0HC��
    else if(r1 == 0x01)
    {
				//printf("\r\n SD���汾��Ϣ:V2.0 \r\n");
        //V2.0�Ŀ���CMD8�����ᴫ��4�ֽڵ����ݣ�Ҫ�����ٽ���������
        buff[0] = SPI_FLASH_SendByte(0xFF);  //should be 0x00
        buff[1] = SPI_FLASH_SendByte(0xFF);  //should be 0x00
        buff[2] = SPI_FLASH_SendByte(0xFF);  //should be 0x01
        buff[3] = SPI_FLASH_SendByte(0xFF);  //should be 0xAA
     
        FLASH_SPI_CS_DISABLE();
        //the next 8 clocks
        SPI_FLASH_SendByte(0xFF);
        
        //�жϸÿ��Ƿ�֧��2.7V-3.6V�ĵ�ѹ��Χ
        if(buff[2]==0x01 && buff[3]==0xAA)
        {
            //֧�ֵ�ѹ��Χ�����Բ���
            retry = 0;
            //������ʼ��ָ��CMD55+ACMD41
    		do
    		{
    			r1 = SD_SendCommand(CMD55, 0, 0);
    			if(r1!=0x01)
    			{
    				return r1;
    			}
    			r1 = SD_SendCommand(ACMD41, 0x40000000, 0);
					if(retry>200)   
					{
							return r1;  //��ʱ�򷵻�r1״̬
					}
				}while(r1!=0);
          
				//��ʼ��ָ�����ɣ���������ȡOCR��Ϣ
				//-----------����SD2.0���汾��ʼ-----------
				r1 = SD_SendCommand_NoDeassert(CMD58, 0, 0);
				if(r1!=0x00)
				{
						return r1;  //�������û�з�����ȷӦ��ֱ���˳�������Ӧ��
				}
				//��OCRָ����󣬽�������4�ֽڵ�OCR��Ϣ
				buff[0] = SPI_FLASH_SendByte(0xFF);
				buff[1] = SPI_FLASH_SendByte(0xFF); 
				buff[2] = SPI_FLASH_SendByte(0xFF);
				buff[3] = SPI_FLASH_SendByte(0xFF);
				
				//OCR������ɣ�Ƭѡ�ø�
				FLASH_SPI_CS_DISABLE();
				SPI_FLASH_SendByte(0xFF);
				//�����յ���OCR�е�bit30λ��CCS����ȷ����ΪSD2.0����SDHC
				//���CCS=1��SDHC   CCS=0��SD2.0
				if(buff[0]&0x40)    //���CCS
				{
						SD_Type = SD_TYPE_V2HC;
						//printf("\r\n SD����Ϣ: SDHC \r\n");
				}
				else
				{
						SD_Type = SD_TYPE_V2;
						//printf("\r\n SD����Ϣ: SD2.0 \r\n");
				}
				//-----------����SD2.0���汾����-----------
				
				//����SPIΪ����ģʽ
				SPI_SetSpeed(1);  
			}
    }
    return r1;
}
/*******************************************************************************
* Function Name  : SD_ReceiveData
* Description    : ��SD���ж���ָ�����ȵ����ݣ������ڸ���λ��
* Input          : uint8_t *data(��Ŷ������ݵ��ڴ�>len)
*                  uint16_t len(���ݳ��ȣ�
*                  uint8_t release(������ɺ��Ƿ��ͷ�����CS�ø� 0�����ͷ� 1���ͷţ�
* Output         : None
* Return         : uint8_t 
*                  0��NO_ERR
*                  other��������Ϣ
*******************************************************************************/
uint8_t SD_ReceiveData(uint8_t *data, uint16_t len, uint8_t release)
{
    uint16_t retry;
    uint8_t r1;
    // ����һ�δ���
    FLASH_SPI_CS_ENABLE();
    //�ȴ�SD������������ʼ����0xFE
    retry = 0;
    do
    {
        r1 = SPI_FLASH_SendByte(0xFF);
        retry++;
        if(retry>2000)  //2000�εȴ���û��Ӧ���˳�����
        {
            FLASH_SPI_CS_DISABLE();
            return 1;
        }
    }while(r1 != 0xFE);
    //��ʼ��������
    while(len--)
    {
        *data = SPI_FLASH_SendByte(0xFF);
        data++;
    }
    //������2��αCRC��dummy CRC��
    SPI_FLASH_SendByte(0xFF);
    SPI_FLASH_SendByte(0xFF);
    //�����ͷ����ߣ���CS�ø�
    if(release == RELEASE)
    {
        //�������
        FLASH_SPI_CS_DISABLE();
        SPI_FLASH_SendByte(0xFF);
    }
    return 0;
}
/*******************************************************************************
* Function Name  : SD_GetCID
* Description    : ��ȡSD����CID��Ϣ��������������Ϣ
* Input          : uint8_t *cid_data(���CID���ڴ棬����16Byte��
* Output         : None
* Return         : uint8_t 
*                  0��NO_ERR
*                  1��TIME_OUT
*                  other��������Ϣ
*******************************************************************************/
uint8_t SD_GetCID(uint8_t *cid_data)
{
    uint8_t r1;
    //��CMD10�����CID
    r1 = SD_SendCommand(CMD10, 0, 0xFF);
    if(r1 != 0x00)
    {
        return r1;  //û������ȷӦ�����˳�������
    }
    //����16���ֽڵ�����
    SD_ReceiveData(cid_data, 16, RELEASE);
    return 0;
}
/*******************************************************************************
* Function Name  : SD_GetCSD
* Description    : ��ȡSD����CSD��Ϣ�������������ٶ���Ϣ
* Input          : uint8_t *cid_data(���CID���ڴ棬����16Byte��
* Output         : None
* Return         : uint8_t 
*                  0��NO_ERR
*                  1��TIME_OUT
*                  other��������Ϣ
*******************************************************************************/
uint8_t SD_GetCSD(uint8_t *csd_data)
{
    uint8_t r1;
    //��CMD9�����CSD
    r1 = SD_SendCommand(CMD9, 0, 0xFF);
    if(r1 != 0x00)
    {
        return r1;  //û������ȷӦ�����˳�������
    }
    //����16���ֽڵ�����
    SD_ReceiveData(csd_data, 16, RELEASE);
    return 0;
}
/*******************************************************************************
* Function Name  : SD_GetCapacity
* Description    : ��ȡSD��������
* Input          : None
* Output         : None
* Return         : uint32_t capacity 
*                   0�� ȡ�������� 
*******************************************************************************/
uint32_t SD_GetCapacity(void)
{
    uint8_t csd[16];
    uint32_t Capacity;
    uint8_t r1;
    uint16_t i;
	uint16_t temp;
    //ȡCSD��Ϣ������ڼ��������0
    if(SD_GetCSD(csd)!=0)
    {
        return 0;
    }
       
    //���ΪSDHC�����������淽ʽ����
    if((csd[0]&0xC0)==0x40)
    {
        Capacity = ((((uint32_t)csd[8])<<8) + (uint32_t)csd[9] + 1)*(uint32_t)1024;
    }
    else
    {
        //�������Ϊ���ϰ汾
        // formula of the capacity///
        //
        //  memory capacity = BLOCKNR * BLOCK_LEN
        //	
        //	BLOCKNR = (C_SIZE + 1)* MULT
        //
        //           C_SIZE_MULT+2
        //	MULT = 2
        //
        //               READ_BL_LEN
        //	BLOCK_LEN = 2
        /**********************************************/
        //C_SIZE
    	i = csd[6]&0x03;
    	i<<=8;
    	i += csd[7];
    	i<<=2;
    	i += ((csd[8]&0xc0)>>6);
    
        //C_SIZE_MULT
    	r1 = csd[9]&0x03;
    	r1<<=1;
    	r1 += ((csd[10]&0x80)>>7);
    
        //BLOCKNR
    	r1+=2;
    	temp = 1;
    	while(r1)
    	{
    		temp*=2;
    		r1--;
    	}
    	Capacity = ((uint32_t)(i+1))*((uint32_t)temp);
    
        // READ_BL_LEN
    	i = csd[5]&0x0f;
        //BLOCK_LEN
    	temp = 1;
    	while(i)
    	{
    		temp*=2;
    		i--;
    	}
        //The final result
    	Capacity *= (uint32_t)temp; 
    	//Capacity /= 512;
    }
    return (uint32_t)Capacity;
}
/*******************************************************************************
* Function Name  : SD_ReadSingleBlock
* Description    : ��SD����һ��block
* Input          : uint32_t sector ȡ��ַ��sectorֵ���������ַ�� 
*                  uint8_t *buffer ���ݴ洢��ַ����С����512byte�� 
* Output         : None
* Return         : uint8_t r1 
*                   0�� �ɹ�
*                   other��ʧ��
*******************************************************************************/
uint8_t SD_ReadSingleBlock(uint32_t sector, uint8_t *buffer)
{
	uint8_t r1;
	//����Ϊ����ģʽ
	SPI_SetSpeed(SPI_SPEED_HIGH);
	
	//�������SDHC����sector��ַת��byte��ַ
//	sector = sector<<9;
	r1 = SD_SendCommand(CMD17, sector, 0);//������
	if(r1 != 0x00)
	{
			return r1;
	}
	
	r1 = SD_ReceiveData(buffer, 512, RELEASE);
	if(r1 != 0)
	{
			return r1;   //�����ݳ���
	}
	else
	{
			return 0;
	}
}
/*******************************************************************************
* Function Name  : SD_WriteSingleBlock
* Description    : д��SD����һ��block
* Input          : uint32_t sector ������ַ��sectorֵ���������ַ�� 
*                  uint8_t *buffer ���ݴ洢��ַ����С����512byte�� 
* Output         : None
* Return         : uint8_t r1 
*                   0�� �ɹ�
*                   other��ʧ��
*******************************************************************************/
uint8_t SD_WriteSingleBlock(uint32_t sector, const uint8_t *data)
{
    uint8_t r1;
    uint16_t i;
    uint16_t retry;
    //����Ϊ����ģʽ
    SPI_SetSpeed(SPI_SPEED_HIGH);
    //�������SDHC����������sector��ַ������ת����byte��ַ
//    if(SD_Type!=SD_TYPE_V2HC)
//    {
//        sector = sector<<9;
//    }
    r1 = SD_SendCommand(CMD24, sector, 0x00);
    if(r1 != 0x00)
    {
        return r1;  //Ӧ����ȷ��ֱ�ӷ���
    }
    
    //��ʼ׼�����ݴ���
    FLASH_SPI_CS_ENABLE();
    //�ȷ�3�������ݣ��ȴ�SD��׼����
    SPI_FLASH_SendByte(0xff);
    SPI_FLASH_SendByte(0xff);
    SPI_FLASH_SendByte(0xff);
    //����ʼ����0xFE
    SPI_FLASH_SendByte(0xFE);
    //��һ��sector������
    for(i=0;i<512;i++)
    {
        SPI_FLASH_SendByte(*data++);
    }
    //��2��Byte��dummy CRC
    SPI_FLASH_SendByte(0xff);
    SPI_FLASH_SendByte(0xff);
    
    //�ȴ�SD��Ӧ��
    r1 = SPI_FLASH_SendByte(0xff);
    if((r1&0x1F)!=0x05)
    {
        FLASH_SPI_CS_DISABLE();
        return r1;
    }
    
    //�ȴ��������
    retry = 0;
    while(!SPI_FLASH_SendByte(0xff))
    {
        retry++;
        if(retry>0xfffe)        //�����ʱ��д��û����ɣ������˳�
        {
            FLASH_SPI_CS_DISABLE();
            return 1;           //д�볬ʱ����1
        }
    }
    //д����ɣ�Ƭѡ��1
    FLASH_SPI_CS_DISABLE();
    SPI_FLASH_SendByte(0xff);
    return 0;
}
/*******************************************************************************
* Function Name  : SD_ReadMultiBlock
* Description    : ��SD���Ķ��block
* Input          : uint32_t sector ȡ��ַ��sectorֵ���������ַ�� 
*                  uint8_t *buffer ���ݴ洢��ַ����С����512byte��
*                  uint8_t count ������count��block
* Output         : None
* Return         : uint8_t r1 
*                   0�� �ɹ�
*                   other��ʧ��
*******************************************************************************/
uint8_t SD_ReadMultiBlock(uint32_t sector, uint8_t *buffer, uint8_t count)
{
    uint8_t r1;
    //����Ϊ����ģʽ
    SPI_SetSpeed(SPI_SPEED_HIGH);
    
    //�������SDHC����sector��ַת��byte��ַ
//    sector = sector<<9;
    //SD_WaitReady();
    //�����������
	r1 = SD_SendCommand(CMD18, sector, 0);//������
	if(r1 != 0x00)
    {
        return r1;
    }
    //��ʼ��������
    do
    {
        if(SD_ReceiveData(buffer, 512, NO_RELEASE) != 0x00)
        {
            break;
        }
        buffer += 512;
    } while(--count);
    //ȫ��������ϣ�����ֹͣ����
    SD_SendCommand(CMD12, 0, 0);
    //�ͷ�����
    FLASH_SPI_CS_DISABLE();
    SPI_FLASH_SendByte(0xFF);
    
    if(count != 0)
    {
        return count;   //���û�д��꣬����ʣ�����
    }
    else
    {
        return 0;
    }
}
/*******************************************************************************
* Function Name  : SD_WriteMultiBlock
* Description    : д��SD����N��block
* Input          : uint32_t sector ������ַ��sectorֵ���������ַ�� 
*                  uint8_t *buffer ���ݴ洢��ַ����С����512byte��
*                  uint8_t count д���block��Ŀ
* Output         : None
* Return         : uint8_t r1 
*                   0�� �ɹ�
*                   other��ʧ��
*******************************************************************************/
uint8_t SD_WriteMultiBlock(uint32_t sector, const uint8_t *data, uint8_t count)
{
    uint8_t r1;
    uint16_t i;
    //����Ϊ����ģʽ
    SPI_SetSpeed(SPI_SPEED_HIGH);
    //�������SDHC����������sector��ַ������ת����byte��ַ
//    if(SD_Type != SD_TYPE_V2HC)
//    {
//        sector = sector<<9;
//    }
    //���Ŀ�꿨����MMC��������ACMD23ָ��ʹ��Ԥ����
    if(SD_Type != SD_TYPE_MMC)
    {
        r1 = SD_SendCommand(ACMD23, count, 0x00);
    }
    //�����д��ָ��
    r1 = SD_SendCommand(CMD25, sector, 0x00);
    if(r1 != 0x00)
    {
        return r1;  //Ӧ����ȷ��ֱ�ӷ���
    }
    
    //��ʼ׼�����ݴ���
    FLASH_SPI_CS_ENABLE();
    //�ȷ�3�������ݣ��ȴ�SD��׼����
    SPI_FLASH_SendByte(0xff);
    SPI_FLASH_SendByte(0xff);
    //--------������N��sectorд���ѭ������
    do
    {
        //����ʼ����0xFC �����Ƕ��д��
        SPI_FLASH_SendByte(0xFC);
    
        //��һ��sector������
        for(i=0;i<512;i++)
        {
            SPI_FLASH_SendByte(*data++);
        }
        //��2��Byte��dummy CRC
        SPI_FLASH_SendByte(0xff);
        SPI_FLASH_SendByte(0xff);
        
        //�ȴ�SD��Ӧ��
        r1 = SPI_FLASH_SendByte(0xff);
        if((r1&0x1F)!=0x05)
        {
            FLASH_SPI_CS_DISABLE();    //���Ӧ��Ϊ��������������ֱ���˳�
            return r1;
        }
        //�ȴ�SD��д�����
        if(SD_WaitReady()==1)
        {
            FLASH_SPI_CS_DISABLE();    //�ȴ�SD��д����ɳ�ʱ��ֱ���˳�����
            return 1;
        }
        //��sector���ݴ������
    }while(--count);
    
    //��������������0xFD
    r1 = SPI_FLASH_SendByte(0xFD);
    if(r1==0x00)
    {
        count =  0xfe;
    }
    if(SD_WaitReady())
    {
        while(1)
        {
        }
    }
    
    //д����ɣ�Ƭѡ��1
    FLASH_SPI_CS_DISABLE();
    SPI_FLASH_SendByte(0xff);
    return count;   //����countֵ�����д����count=0������count=1
}
/* USER CODE END 1 */
