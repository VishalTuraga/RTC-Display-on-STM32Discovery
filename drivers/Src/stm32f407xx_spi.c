/*
 * stm32f407xx_spi.c
 *
 *  Created on: Sep 3, 2024
 *      Author: Vishal Turaga
 */
#include "stm32f407xx_spi.h"

static void SPI_TXE_ITHANDLE(SPI_Handle_t *pSPIHandle);
static void SPI_RXNE_ITHANDLE(SPI_Handle_t *pSPIHandle);
static void SPI_OVR_ERR_ITHANDLE(SPI_Handle_t *pSPIHandle);


/*
 * Enable/Disable SPI Bits
 */

/*************************************************************************************************
 * @fn				- SPI_SSIConfig
 *
 * @brief			-
 *
 * @param[in]		-
 * @param[in]		-
 * @param[in[		-
 *
 * @return			-
 *
 * @Note			-
 *
 *************************************************************************************************/
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		SPI2->CR1 |= (1 << SPI_CR1_SSI);
	}
	else
	{
		SPI2->CR1 &= ~(1 << SPI_CR1_SSI);

	}
}

/*************************************************************************************************
 * @fn				- SPI_SSOEConfig
 *
 * @brief			-
 *
 * @param[in]		-
 * @param[in]		-
 * @param[in[		-
 *
 * @return			-
 *
 * @Note			-
 *
 *************************************************************************************************/
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		SPI2->CR1 |= (1 << SPI_CR2_SSOE);
	}
	else
	{
		SPI2->CR1 &= ~(1 << SPI_CR2_SSOE);
	}
}

/*
 * Enable/Disable SPI Peripheral
 */
/*************************************************************************************************
 * @fn				- SPI_PeriheralControl
 *
 * @brief			-
 *
 * @param[in]		-
 * @param[in]		-
 * @param[in[		-
 *
 * @return			-
 *
 * @Note			-
 *
 *************************************************************************************************/
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	}
	else if(EnOrDi == DISABLE)
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
	}
}

/*
 * Clock control
 */
/*************************************************************************************************
 * @fn				- SPI_ClockControl
 *
 * @brief			-
 *
 * @param[in]		-
 * @param[in]		-
 * @param[in[		-
 *
 * @return			-
 *
 * @Note			-
 *
 *************************************************************************************************/
//#define SPI1							((GPIO_RegDef_t*)SPI1_BASEADDR)
void SPI_ClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pSPIx == SPI1)
		{
			SPI1_CLK_EN();
		}
		else if(pSPIx == SPI2)
		{
			SPI2_CLK_EN();
		}
		else if(pSPIx == SPI3)
		{
			SPI3_CLK_EN();
		}
	}
	else
	{
		if(pSPIx == SPI1)
		{
			SPI1_CLK_DI();
		}
		else if(pSPIx == SPI2)
		{
			SPI2_CLK_DI();
		}
		else if(pSPIx == SPI3)
		{
			SPI3_CLK_DI();
		}
	}
}

/*
 * SPI Init and De-Init
 */
/*************************************************************************************************
 * @fn				- SPI_Init
 *
 * @brief			-
 *
 * @param[in]		-
 * @param[in]		-
 * @param[in[		-
 *
 * @return			-
 *
 * @Note			-
 *
 *************************************************************************************************/
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
	//	uint8_t DeviceMode;					/*DeviceMode is used to select the device to be configured either as a Master or Slave*/
	//	uint8_t BusConfig;					/*Bus Config is sued to select the mode of communication as full duplex, half duplex or simplex*/
	//	uint8_t SclkSpeed;					/*SclkSpeed is used to device the clock speed*/
	//	uint8_t DFF;						/*DFF can either be 8 bit or 16 bits*/
	//	uint8_t CPOL;						/*CPOL is used to select the clock polarity*/
	//	uint8_t CPHA;						/*CPHA is used to select the clock phase*/
	//	uint8_t SSM;						/*SSM is Slave Select Management which is ued to select what slave to communicate to*/

	uint32_t temp = 0;

	// enable peripheral clock
	SPI_ClockControl(pSPIHandle->pSPIx, ENABLE);

	// 1. Configure the device mode
	temp = pSPIHandle->SPIConfig.BusConfig << 2;

	// 2. Configure the Bus communication type
	if(pSPIHandle->SPIConfig.BusConfig == SPI_BUSCONFIG_FULLDUPLEX)
	{
		//Clear BIDIMODE (bit 15)
		temp &= ~(1 << SPI_CR1_BIDIMODE);
	}
	else if(pSPIHandle->SPIConfig.BusConfig == SPI_BUSCONFIG_HALFDUPLES)
	{
		//Set BIDIMODE (bit 15)
		temp |= (1 << SPI_CR1_BIDIMODE);
	}
	else if(pSPIHandle->SPIConfig.BusConfig == SPI_BUSCONFIG_SIMPLEX)
	{
		//clear BIDIMODE (bit 15) and set RXONLY (bit 10)
		temp &= ~(1 << SPI_CR1_BIDIMODE);
		temp |= (1 << SPI_CR1_RXONLY);
	}

	// 3. Configure the Clock Speed
	temp |= pSPIHandle->SPIConfig.SclkSpeed << SPI_CR1_BR;

	// 4. DFF configuration
	temp |= pSPIHandle->SPIConfig.DFF << SPI_CR1_DFF;

	// 5. Configure CPOL
	temp |= pSPIHandle->SPIConfig.CPOL << SPI_CR1_CPOL;

	// 6. Configure CPHA
	temp |= pSPIHandle->SPIConfig.CPHA << SPI_CR1_CPHA;


	pSPIHandle->pSPIx->CR1 |= temp;

}

/*************************************************************************************************
 * @fn				- SPI_Deinit
 *
 * @brief			-
 *
 * @param[in]		-
 * @param[in]		-
 * @param[in[		-
 *
 * @return			-
 *
 * @Note			-
 *
 *************************************************************************************************/
void SPI_Deinit(SPI_RegDef_t *pSPIx)
{
	if(pSPIx == SPI1)
	{
		RCC->APB2RSTR |= (1 << 12);
		RCC->APB2RSTR &= ~(1 << 12);
	}
	else if(pSPIx == SPI2)
	{
		RCC->APB1RSTR |= (1 << 14);
		RCC->APB1RSTR &= ~(1 << 14);
	}
	else if(pSPIx == SPI3)
	{

		RCC->APB1RSTR |= (1 << 15);
		RCC->APB1RSTR &= ~(1 << 15);
	}
}

/*
 * SPI Data receive and Send
 */
/*************************************************************************************************
 * @fn				- SPI_SendData
 *
 * @brief			-
 *
 * @param[in]		-
 * @param[in]		-
 * @param[in[		-
 *
 * @return			-
 *
 * @Note			-
 *
 *************************************************************************************************/
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t len)
{
	while(len > 0)
	{
		while(!(pSPIx->SR & (1<<SPI_SR_TXE)));
		if((pSPIx->CR1 & (1 << SPI_CR1_DFF)))
		{
			// shift register is 16 bits
			pSPIx->DR = *(uint16_t*)pTxBuffer;
			len-=2;
			(uint16_t*)pTxBuffer++ 	;
		}
		else
		{
			// shift register is 8 bits
			pSPIx->DR = *pTxBuffer;
			len--;
			pTxBuffer++;
		}

	}

}

/*************************************************************************************************
 * @fn				- SPI_ReceiveData
 *
 * @brief			-
 *
 * @param[in]		-
 * @param[in]		-
 * @param[in[		-
 *
 * @return			-
 *
 * @Note			-
 *
 *************************************************************************************************/
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t len)
{
	while(len > 0)
	{
		while(!(pSPIx->SR & (1<<SPI_SR_RXNE)));
		if((pSPIx->CR1 & (1 << SPI_CR1_DFF)) == SPI_DFF_8BIT)
		{
			// shift register is 16 bits
			*(uint16_t*)pRxBuffer = pSPIx->DR;
			len-=2;
			(uint16_t*)pRxBuffer++;
		}
		else
		{
			// shift register is 8 bits
			*pRxBuffer = pSPIx->DR;
			len--;
			pRxBuffer++;
		}

	}
}

/*
 * IRQ configuration and ISR Handling
 */
/*************************************************************************************************
 * @fn				- SPI_IRQInterruptConfig
 *
 * @brief			-
 *
 * @param[in]		-
 * @param[in]		-
 * @param[in[		-
 *
 * @return			-
 *
 * @Note			-
 *
 *************************************************************************************************/
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	// processor side configuration
	if(EnorDi == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			// program ISER1
			*NVIC_ISER0 |= (1<<IRQNumber);
		}
		else if (IRQNumber >= 32 && IRQNumber <=63)
		{
			*NVIC_ISER1 |= (1<<(IRQNumber%32));
		}
		else if (IRQNumber >= 64 && IRQNumber <= 96)
		{
			*NVIC_ISER2 |= (1<<(IRQNumber%64));
		}
	}
	else
	{
		if(IRQNumber <= 31)
		{
			// program ISER1
			*NVIC_ICER0 |= (1<<IRQNumber);
		}
		else if (IRQNumber >= 32 && IRQNumber <=63)
		{
			*NVIC_ICER1 |= (1<<(IRQNumber%32));
		}
		else if (IRQNumber >= 64 && IRQNumber <= 96)
		{
			*NVIC_ICER2 |= (1<<(IRQNumber%64));
		}
	}
}

/*************************************************************************************************
 * @fn				- SPI_IRQPriorityConfig
 *
 * @brief			-
 *
 * @param[in]		-
 * @param[in]		-
 * @param[in[		-
 *
 * @return			-
 *
 * @Note			-
 *
 *************************************************************************************************/
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// NOTE: Only the first 4 bits (starting from MSB) are implemented in each of the 8 bits assigned.
	uint8_t iprx 			= IRQNumber/4;
	uint8_t iprx_section 	= IRQNumber%4;
	uint8_t shift_amount	= (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_IPR_BASEADDR + (iprx))	|= (IRQPriority << shift_amount);
}

/*************************************************************************************************
 * @fn				- SPI_IRQHandling
 *
 * @brief			-
 *
 * @param[in]		-
 * @param[in]		-
 * @param[in[		-
 *
 * @return			-
 *
 * @Note			-
 *
 *************************************************************************************************/
static void SPI_TXE_ITHANDLE(SPI_Handle_t *pSPIHandle)
{
	if((pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF)) == SPI_DFF_8BIT)
	{
		// shift register is 16 bits
		pSPIHandle->pSPIx->DR = *((uint16_t*)pSPIHandle->pTxBuffer);
		pSPIHandle->TxLen-=2;
		(uint16_t*)pSPIHandle->pTxBuffer++;
	}
	else
	{
		// shift register is 8 bits
		pSPIHandle->pSPIx->DR = *(uint8_t*)pSPIHandle->pTxBuffer;
		pSPIHandle->TxLen--;
		pSPIHandle->pTxBuffer++;
	}

	if(!pSPIHandle->TxLen)
	{
		SPI_CloseTransmission(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT); // this has to be implemented in the application
	}

}

static void SPI_RXNE_ITHANDLE(SPI_Handle_t *pSPIHandle)
{
	if((pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF)) == SPI_DFF_8BIT)
	{
		// shift register is 16 bits
		*((uint16_t*)pSPIHandle->pRxBuffer) = (uint16_t)pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen-=2;
		pSPIHandle->pRxBuffer++;
		pSPIHandle->pRxBuffer++;
	}
	else
	{
		// shift register is 8 bits
		*(pSPIHandle->pRxBuffer) = (uint8_t)pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen--;
		pSPIHandle->pRxBuffer++;
	}

	if(!pSPIHandle->RxLen)
	{
		SPI_CloseReception(pSPIHandle);

		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_CMPLT); // this has to be implemented in the application
	}
}

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx , uint32_t FlagName)
{
	if(pSPIx->SR & FlagName)
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}

static void SPI_OVR_ERR_ITHANDLE(SPI_Handle_t *pSPIHandle)
{
	uint8_t temp;
	// 1. clear the OVR flag
	if(pSPIHandle->TxState != SPI_BUSY_IN_TX)
	{
		temp = pSPIHandle->pSPIx->DR;
		temp = pSPIHandle->pSPIx->DR;
	}
	(void)temp; // doing this so that we don't get a warning that temp is not used

	// 2. inform the application
	SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_OVR_ERR); // this has to be implemented in the application
}

void SPI_IRQHandling(SPI_Handle_t *pSPIHandle)
{
	uint8_t temp1, temp2;
	temp1 = (pSPIHandle->pSPIx->SR  & (1<<SPI_SR_TXE));
	temp2 = (pSPIHandle->pSPIx->CR2 & (1<<SPI_CR2_TXEIE));

	if(temp1 & temp2)
	{
		// handle TXE
		SPI_TXE_ITHANDLE(pSPIHandle);
	}

	temp1 = (pSPIHandle->pSPIx->SR  & (1<<SPI_SR_RXNE));
	temp2 = (pSPIHandle->pSPIx->CR2 & (1<<SPI_CR2_RXNEIE));

	if(temp1 & temp2)
	{
		// handle TXE
		SPI_RXNE_ITHANDLE(pSPIHandle);
	}

	// We are only checking for overrun flag in this course
	temp1 = (pSPIHandle->pSPIx->SR  & (1<<SPI_SR_OVR));
	temp2 = (pSPIHandle->pSPIx->CR2 & (1<<SPI_CR2_ERRIE));

	if(temp1 & temp2)
	{
		// handle TXE
		SPI_OVR_ERR_ITHANDLE(pSPIHandle);
	}
}

/*************************************************************************************************
 * @fn				- SPI_SendDataIT
 *
 * @brief			-
 *
 * @param[in]		-
 * @param[in]		-
 * @param[in[		-
 *
 * @return			-
 *
 * @Note			-
 *
 *************************************************************************************************/
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t len)
{
	uint8_t state = pSPIHandle->TxState;

	if(state != SPI_BUSY_IN_TX)
	{
		// 1. Save the Tx buffer address and length info in some global variables
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = len;

		// 2. Mark the SPI state as busy in trnasmission so that no other code can take over same SPI peripheral until transmission is over
		pSPIHandle->TxState = SPI_BUSY_IN_TX;

		// 3. Enable TXIEIE Control bit to get interrupt whenever TXE flag is set in SR
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);

		// 4. Data transmission will be handled by the ISR code
	}
	return state;
}

/*************************************************************************************************
 * @fn				- SPI_ReceiveDataIT
 *
 * @brief			-
 *
 * @param[in]		-
 * @param[in]		-
 * @param[in[		-
 *
 * @return			-
 *
 * @Note			-
 *
 *************************************************************************************************/
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t len)
{
	uint8_t state = pSPIHandle->RxState;

	if(state != SPI_BUSY_IN_RX)
	{
		// 1. Save the Tx buffer address and length info in some global variables
		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen = len;

		// 2. Mark the SPI state as busy in trnasmission so that no other code can take over same SPI peripheral until transmission is over
		pSPIHandle->RxState = SPI_BUSY_IN_RX;

		// 3. Enable TXIEIE Control bit to get interrupt whenever TXE flag is set in SR
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);

		// 4. Data transmission will be handled by the ISR code
	}
	return state;
}

void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{
	// if tx len is zero, close the SPI communication and inform the application that tx is over
	// 1. disable the TXEIE bit
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);
	// 2. reset the buffers
	pSPIHandle->pTxBuffer = NULL;
	pSPIHandle->TxLen = 0;
	pSPIHandle->TxState = SPI_READY;
	// 3. inform the application
}
void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);

	pSPIHandle->pRxBuffer = NULL;
	pSPIHandle->RxLen = 0;
	pSPIHandle->RxState = SPI_READY;
}

__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv)
{
	// weak implementation so that can use it
}
