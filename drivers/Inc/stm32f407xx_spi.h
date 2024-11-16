/*
 * stm32f407xx_spi.h
 *
 *  Created on: Sep 1, 2024
 *      Author: ASUS
 */

#ifndef INC_STM32F407XX_SPI_H_
#define INC_STM32F407XX_SPI_H_

#include "stm32f407xx.h"

typedef struct
{
	uint8_t DeviceMode;					/*DeviceMode is used to select the device to be configured either as a Master or Slave*/
	uint8_t BusConfig;					/*Bus Config is sued to select the mode of communication as full duplex, half duplex or simplex*/
	uint8_t SclkSpeed;					/*SclkSpeed is used to device the clock speed*/
	uint8_t DFF;						/*DFF can either be 8 bit or 16 bits*/
	uint8_t CPOL;						/*CPOL is used to select the clock polarity*/
	uint8_t CPHA;						/*CPHA is used to select the clock phase*/
	uint8_t SSM;						/*SSM is Slave Select Management which is ued to select what slave to communicate to*/
}SPI_Config_t;

typedef struct
{
	SPI_RegDef_t *pSPIx;				/*This holds the base address of SPIx (1,2,3) peripheral*/
	SPI_Config_t SPIConfig;				/*This holds the Config structure which is used by the user to configure the SPI peripheral*/
	uint8_t *pTxBuffer;					/*This holds the application Tx buffer address*/
	uint8_t *pRxBuffer;					/*This holds the application Rx buffer address*/
	uint32_t TxLen;						/*This holds the Tx length*/
	uint32_t RxLen;						/*This holds the Rx length*/
	uint32_t TxState;					/*This holds the Tx State*/
	uint32_t RxState;					/*This holds the Rx State*/
}SPI_Handle_t;

/*
 * SPI Application States
 */
#define SPI_READY							0
#define SPI_BUSY_IN_RX						1
#define SPI_BUSY_IN_TX						2


/*
 * SPI Device Config
 */
#define SPI_DEVICEMODE_SLAVE				0
#define SPI_DEVICEMODE_MASTER				1

/*
 * SPI Bus Config
 */
#define SPI_BUSCONFIG_FULLDUPLEX			1
#define SPI_BUSCONFIG_HALFDUPLES			2
#define SPI_BUSCONFIG_SIMPLEX				3

/*
 * SPI Speed Macros
 */
#define SPI_SCLKSPEED_FPCLK_2				0
#define SPI_SCLKSPEED_FPCLK_4				1
#define SPI_SCLKSPEED_FPCLK_8				2
#define SPI_SCLKSPEED_FPCLK_16				3
#define SPI_SCLKSPEED_FPCLK_32				4
#define SPI_SCLKSPEED_FPCLK_64				5
#define SPI_SCLKSPEED_FPCLK_128				6
#define SPI_SCLKSPEED_FPCLK_256				7

/*
 * SPI DFF
 * THIS BIT SHOULD BE WRITTEN ONLY WHEN SPI IS DISABLED (SPE = 0) FOR CORRECT OPERATION
 */
#define SPI_DFF_8BIT						0
#define SPI_DFF_16BIT						1

/*
 * SPI CPOL
 * Note: This bit should not be changed when communication is ongoing.
 */
#define SPI_CPOL_LOW_IDLE_STATE				0
#define SPI_CPOL_HIGH_IDLE_STATE			1

/*
 * SPI CPHA
 * Note: This bit should not be changed when communication is ongoing
 */
#define SPI_CPHA_TRAILING_EDGE				0
#define SPI_CPHA_LEADING_EDGE				1

/*
 * SPI SSM
 */
#define SPI_SSM_DI							0
#define SPI_SSM_EN							1

/*
 * Possible SPI Application Events
 */
#define SPI_EVENT_TX_CMPLT					1			/*Transmission is complete*/
#define SPI_EVENT_RX_CMPLT					2			/*Reception is complete*/
#define SPI_EVENT_OVR_ERR					3			/*OVR error in the SPI*/

/*
 * SPI related status flags definitions
 */
#define SPI_TXE_FLAG    					(1 << SPI_SR_TXE)
#define SPI_RXNE_FLAG   					(1 << SPI_SR_RXNE)
#define SPI_BUSY_FLAG   					(1 << SPI_SR_BSY)

/**************************************************************************************************************************************
 * 														APIs supported by this driver
 * 									For more information about the APIs check the function definitions
 **************************************************************************************************************************************/

/*
 * Enable/Disbale some bits
 */
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);
void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx);
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle);
void SPI_CloseReception(SPI_Handle_t *pSPIHandle);
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx , uint32_t FlagName);

/*
 * Enable/Disable SPI peripheral
 */
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);

/*
 * Clock control
 */
void SPI_ClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);

/*
 * SPI Init and De-Init
 */
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_Deinit(SPI_RegDef_t *pSPIx);

/*
 * SPI Data receive and Send
 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t len);

void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t len);

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t len);

uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t len);

/*
 * IRQ configuration and ISR Handling
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pSPIHandle);

/*
 * Application callback
 */
void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv);

#endif /* INC_STM32F407XX_SPI_H_ */
