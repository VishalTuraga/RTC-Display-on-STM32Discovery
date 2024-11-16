/*
 * stm32f407x_i2c.c
 *
 *  Created on: Sep 15, 2024
 *      Author: Vishal Turaga
 */


#include "stm32f407xx_i2c.h"

static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);
static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandl);
static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle);

void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi);


static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr &= ~(1);
	pI2Cx->DR = SlaveAddr;

}

static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr |= 1;
	pI2Cx->DR = SlaveAddr;
}

/*************************************************************************************************
 * @fn				- I2C_ManageAcking
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
void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_ACK);
	}
	else if(EnOrDi == DISABLE)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_ACK);
	}
}

static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle)
{
	uint32_t dummyRead;
	// check if device is master mode or slave mode
	if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
	{
		// device in master mode
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			if(pI2CHandle->RxSize == 1)
			{
				// disable the ACK
				I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);

				// clear ADDR flag
				dummyRead = pI2CHandle->pI2Cx->SR1;
				dummyRead = pI2CHandle->pI2Cx->SR2;
				(void)dummyRead;
			}
		}
		else
		{
			// clear ADDR flag
			dummyRead = pI2CHandle->pI2Cx->SR1;
			dummyRead = pI2CHandle->pI2Cx->SR2;
			(void)dummyRead;
		}
	}
	else
	{
		// device is slave mode
		// clear ADDR flag
		dummyRead = pI2CHandle->pI2Cx->SR1;
		dummyRead = pI2CHandle->pI2Cx->SR2;
		(void)dummyRead;
	}


}

/*
 * Enable/Disable I2C Peripheral
 */
/*************************************************************************************************
 * @fn				- I2C_PeriheralControl
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
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_PE);
	}
	else if(EnOrDi == DISABLE)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_PE);
	}
}


/*************************************************************************************************
 * @fn				- I2C_ClockControl
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
void I2C_ClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pI2Cx == I2C1)
		{
			I2C1_CLK_EN();
		}
		else if(pI2Cx == I2C2)
		{
			I2C2_CLK_EN();
		}
		else if(pI2Cx == I2C3)
		{
			I2C3_CLK_EN();
		}
	}
	else
	{
		if(pI2Cx == I2C1)
		{
			I2C1_CLK_DI();
		}
		else if(pI2Cx == I2C2)
		{
			I2C2_CLK_DI();
		}
		else if(pI2Cx == I2C3)
		{
			I2C3_CLK_DI();
		}
	}
}


/*
 * I2C Init and De-Init
 */
/*************************************************************************************************
 * @fn				- I2C_Init
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
void I2C_Init(I2C_Handle_t *pI2CHandle)
{
	I2C_PeripheralControl(pI2CHandle->pI2Cx, ENABLE);
	uint32_t tempreg = 0;
	//1. Configure the mode (Standard or fast)
	uint16_t ccr_value = 0;
	tempreg = 0;
	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM_KHZ)
	{
		// mode is standard
		ccr_value = RCC_GetPCLK1Value() / (2 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
		tempreg |= (ccr_value & 0xFFF);
	}
	else
	{
		// mode is fast mode
		tempreg |= (1 << I2C_CCR_FS);
		tempreg |= (pI2CHandle->I2C_Config.I2C_FMDutyCycle << I2C_CCR_DUTY);
		if(pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_DUTYCYCLE_2)
		{
			// Tlow = 2*Thigh
			ccr_value = (RCC_GetPCLK1Value() / (3 * pI2CHandle->I2C_Config.I2C_SCLSpeed));

		}
		else
		{
			//  Tlow = 1.7*Thigh
			ccr_value = (RCC_GetPCLK1Value() / (25 * pI2CHandle->I2C_Config.I2C_SCLSpeed));
		}
		tempreg |= (ccr_value & 0xFFF);
	}
	pI2CHandle->pI2Cx->CCR = tempreg;

	//2. Configure the speed of the serial clock. Configure the FREQ field of CR2
	tempreg = 0;
	tempreg = RCC_GetPCLK1Value()/1000000U;
	pI2CHandle->pI2Cx->CR2 |= (tempreg & 0x3F);

	//3. Configure the device address (if the device is behaving as slave)
	// configuring for only 7 bit mode
	tempreg = pI2CHandle->I2C_Config.I2C_DeviceAddress << 1;
	pI2CHandle->pI2Cx->OAR1 |= (1 << 14);
	pI2CHandle->pI2Cx->OAR1 = tempreg;

	//4. Enable the acking
	pI2CHandle->pI2Cx->CR1 |= (pI2CHandle->I2C_Config.I2C_ACKControl << I2C_CR1_ACK);

	//5. Configure the rise time for I2C pins
	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM_KHZ)
	{
		// mode is standard
		// here trsie = (Fclk * Trisemax) + 1.
		// For I2C, trisemax is 1000ns or 1microsecond and in frequency terms it is 1 MHz. Hence we are dividing with 1 MHz
		tempreg = (RCC_GetPCLK1Value() / 1000000U) + 1;
	}
	else
	{
		// mode is fast mode
		// trise max for fast mode is 300ns
		tempreg = ((RCC_GetPCLK1Value() * 300) / 1000000000U) + 1;
	}

	pI2CHandle->pI2Cx->TRISE = (tempreg & 0x3F);

	// CCR calculations


}

/*************************************************************************************************
 * @fn				- I2C_Deinit
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
void I2C_Deinit(I2C_RegDef_t *pI2Cx)
{
	if(pI2Cx == I2C1)
	{
		RCC->APB1RSTR |= (1 << 21);
		RCC->APB1RSTR &= ~(1 << 21);
	}
	else if(pI2Cx == I2C2)
	{
		RCC->APB1RSTR |= (1 << 22);
		RCC->APB1RSTR &= ~(1 << 22);
	}
	else if(pI2Cx == I2C3)
	{
		RCC->APB1RSTR |= (1 << 23);
		RCC->APB1RSTR &= ~(1 << 23);
	}
}

/*************************************************************************************************
 * @fn				- I2C_GetFlagStatus
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
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx , uint32_t FlagName)
{
	if((pI2Cx->SR1 & FlagName) || (pI2Cx->SR2 & FlagName))
	{
		return FLAG_SET;
	}
	return FLAG_RESET;
}

/*
 * I2C send and receive data
 */

/*************************************************************************************************
 * @fn				- I2C_MasterSendData
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
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *TxBuffer, uint8_t len, uint8_t SlaveAddr, uint8_t Sr)
{
	// 1. Generate the start condition
	pI2CHandle->pI2Cx->CR1 |= (1 << I2C_CR1_START);

	// 2. Check if the start bit is set and then Read the SR1 register to clear the start bit
	while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_SR1_SB) == FLAG_SET));
	//uint32_t temp = pI2CHandle->pI2Cx->SR1;

	// 3. Send the address of slave with transmission byte (0)
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr &= ~(1);
	pI2CHandle->pI2Cx->DR = SlaveAddr;

	// 4. ADDR bit is set if it receives an ACK
	if(I2C_GetFlagStatus(pI2CHandle->pI2Cx,(1 << I2C_SR1_ADDR)))
	{
		// The ADDR bit is set which means that the master received an ack. Now we should reset this ADDR bit
		// read SR1 and SR2 to clear this bit
		I2C_ClearADDRFlag(pI2CHandle);
	}

	// 5. Send data till len becomes zero. We don't have to check for ack every time as it is handled by the hardware
	while(len)
	{
		// wait till Txe is 1 indicating that DR is empty and ready to be filled with data
		while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx,(1 << I2C_SR1_TxE))))
		{
			pI2CHandle->pI2Cx->DR = *TxBuffer;
			TxBuffer++;
			len--;
		}
	}

	// 6. Close the communication
	// 6.1 wait for Txe = 1 and BTF = 1 before generating the stop condition
	while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx,(1 << I2C_SR1_TxE))));
	while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx,(1 << I2C_SR1_BTF))));

	// 6.2 Generate the stop condition (if repeated start isn't enabled)
	if(Sr == I2C_NO_SR)
		pI2CHandle->pI2Cx->CR1 |= (1 << I2C_CR1_STOP);
}

/*************************************************************************************************
 * @fn				- I2C_MasterReceiveData
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

/*
 * I2C send and receive data
 */

void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *RxBuffer, uint8_t len, uint8_t SlaveAddr, uint8_t Sr)
{
	uint32_t temp;
	// 1. Initiate the start condition
	pI2CHandle->pI2Cx->CR1 |= (1 << I2C_CR1_START);

	// 2. Confirm if the start bit is set
	while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx,I2C_SR1_SB)));

	// 3. Send Address bit
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr |= 1;
	pI2CHandle->pI2Cx->DR = SlaveAddr;

	// 4. check if the ADDR flag is set. Wait until its set
	while(!(I2C_GetFlagStatus(pI2CHandle->pI2Cx,(1 << I2C_SR1_ADDR))));

	// 5. Send data. If len = 1 or if len > 1
	if(len == 1)
	{
		// a. Disable ack
		pI2CHandle->pI2Cx->CR1 &= ~(1 << I2C_CR1_ACK);

		// c. clear addr bit. Read SR1 followed by SR2
		I2C_ClearADDRFlag(pI2CHandle);

		// d. wait till RXNE is set
		while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, (1 << I2C_SR1_RxNE)));

		// b. send stop condition if repeated start is disabled
		if(Sr == I2C_NO_SR)
			pI2CHandle->pI2Cx->CR1 |= (1 << I2C_CR1_STOP);

		// e. Read the data from the data register
		*RxBuffer = pI2CHandle->pI2Cx->DR;
		RxBuffer++;
		len--;
	}
	if(len > 1 )
	{
		// a. clear the address bit
		temp = pI2CHandle->pI2Cx->SR1;
		temp = pI2CHandle->pI2Cx->SR2;
		(void)temp;

		// b. receive data
		while(len > 0)
		{
			// d. wait till RXNE becomes 1
			while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, (1 << I2C_SR1_RxNE)));

			if(len == 2)
			{
				// c. when len == 2,
				// c.1 clear ACK
				pI2CHandle->pI2Cx->CR1 &= ~(1 << I2C_CR1_ACK);

				// c.2 set STOP condition if repeated start is disabled
				if(Sr == I2C_NO_SR)
					pI2CHandle->pI2Cx->CR1 |= (1 << I2C_CR1_STOP);
			}

			*RxBuffer = pI2CHandle->pI2Cx->DR;

			len--;
			RxBuffer++;
		}

	}

	// 7. Renable acking
	if(pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACKCTRL_ACK_EN)
		pI2CHandle->pI2Cx->CR1 |= (1 << I2C_CR1_ACK);

}

/*************************************************************************************************
 * @fn				- I2C_MasterSendDataIT
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
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *TxBuffer, uint8_t len, uint8_t SlaveAddr, uint8_t Sr)
{
	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pTxBuffer =TxBuffer;
		pI2CHandle->TxLen = len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implement code to Generate START Condition
		pI2CHandle->pI2Cx->CR1 |= ( 1 << I2C_CR1_START);

		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);


		//Implement the code to enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);
	}
	return busystate;
}

/*************************************************************************************************
 * @fn				- I2C_MasterReceiveDataIT
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
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *RxBuffer, uint8_t len, uint8_t SlaveAddr, uint8_t Sr)
{
	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pRxBuffer = RxBuffer;
		pI2CHandle->RxLen = len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_RX;
		pI2CHandle->RxSize = len; //Rxsize is used in the ISR code to manage the data reception
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implement code to Generate START Condition
		pI2CHandle->pI2Cx->CR1 |= ( 1 << I2C_CR1_START);

		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);


		//Implement the code to enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);

	}

	return busystate;

}

/*************************************************************************************************
 * @fn				- I2C_IRQInterruptConfig
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
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
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
 * @fn				- I2C_IRQPriorityConfig
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
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	// NOTE: Only the first 4 bits (starting from MSB) are implemented in each of the 8 bits assigned.
	uint8_t iprx 			= IRQNumber/4;
	uint8_t iprx_section 	= IRQNumber%4;
	uint8_t shift_amount	= (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_IPR_BASEADDR + (iprx))	|= (IRQPriority << shift_amount);
}

static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if(pI2CHandle->TxLen > 0)
	{
		// 1. load the data into DR
		pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer);

		// 2. decrement the tx len
		pI2CHandle->TxLen--;

		// 3. increment the buffer addr
		pI2CHandle->pTxBuffer++;
	}
}

static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
	{
		if(pI2CHandle->RxSize == 1)
		{
			*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;

			pI2CHandle->RxLen--;
		}

		if(pI2CHandle->RxSize > 1)
		{
			if(pI2CHandle->RxLen == 2)
			{
				// clear the ack bit
				I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);
			}

			// read DR
			*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
			pI2CHandle->pRxBuffer++;
			pI2CHandle->RxLen--;
		}

		if(pI2CHandle->RxLen == 0)
		{
			// close the i2c data reception and notify the application
			// 1. generate the stop condition
			if(pI2CHandle->Sr == I2C_NO_SR)
				pI2CHandle->pI2Cx->CR1 |= (1 << I2C_CR1_STOP);

			// 2. close the i2c rx
			I2C_CloseReceiveData(pI2CHandle);

			// 3. notify the application
			I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_RX_CMPLT);
		}
	}
}
void I2C_CloseSendData(I2C_Handle_t *pI2CHandle)
{
	// disbale ITBUFEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITBUFEN);

	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pTxBuffer = NULL;
	pI2CHandle->TxLen = 0;
}

void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle)
{
	// disbale ITBUFEN Control Bit
	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITBUFEN);

	pI2CHandle->pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pRxBuffer = NULL;
	pI2CHandle->RxLen = 0;
	pI2CHandle->RxSize = 0;
	if(pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACKCTRL_ACK_EN)
		I2C_ManageAcking(pI2CHandle->pI2Cx, ENABLE);
}

/*************************************************************************************************
 * @fn				- I2C_EV_IRQHandling
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
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	//Interrupt handling for both master and slave mode of a device

	uint32_t temp1, temp2, temp3;

	temp1 = pI2CHandle->pI2Cx->CR2 & ( 1 << I2C_CR2_ITEVTEN);
	temp2 = pI2CHandle->pI2Cx->CR2 & ( 1 << I2C_CR2_ITBUFEN);
	temp3 = pI2CHandle->pI2Cx->SR1 * ( 1 << I2C_SR1_SB);

	//1. Handle For interrupt generated by SB event
	//	Note : SB flag is only applicable in Master mode
	if(temp1 && temp3)
	{
		// SB flag is set
		// interrept generated because of SB event (won't be executed in the Slave Mode)
		// we will execute the addr phase in this block
		// if i2c application state is busy in tx, then we send addr with LSB as write
		// else we send addr with LSB as write
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}
		else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}
	}

	temp3 = pI2CHandle->pI2Cx->SR1 * ( 1 << I2C_SR1_ADDR);
	//2. Handle For interrupt generated by ADDR event
	//Note : When master mode : Address is sent
	//		 When Slave mode   : Address matched with own address
	if(temp1 && temp3)
	{
		// ADDR flag is set
		I2C_ClearADDRFlag(pI2CHandle);
	}

	temp3 = pI2CHandle->pI2Cx->SR1 * ( 1 << I2C_SR1_BTF);
	//3. Handle For interrupt generated by BTF(Byte Transfer Finished) event
	if(temp1 && temp3)
	{
		// BTF flag is set
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			// make sure that TXE is also set
			if(pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_TxE))
			{
				// BTF and TXE both are set

				if(pI2CHandle->TxLen == 0)
				{
					//1. close the transmission
					if(pI2CHandle->Sr == I2C_NO_SR)
						pI2CHandle->pI2Cx->CR1 |= ( 1 << I2C_CR1_STOP);

					//2. Reset all the member elements of the handle structure
					I2C_CloseSendData(pI2CHandle);

					//3. notify the app about transmission complete
					I2C_ApplicationEventCallback(pI2CHandle,I2C_EV_TX_CMPLT);
				}
			}
		}
		else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			// nothing to do here
			;
		}
	}

	temp3 = pI2CHandle->pI2Cx->SR1 & ( 1 << I2C_SR1_STOPF);
	//4. Handle For interrupt generated by STOPF event
	// Note : Stop detection flag is applicable only slave mode . For master this flag will never be set
	if(temp1 && temp3)
	{
		// STOPF flag is set
		// cleared by reading SR1 reg followed by writing to CR1
		// read is done when read read the value of the stopf bit to temp3
		pI2CHandle->pI2Cx->CR1 |= 0x0000;

		// Notify the application that STOP ws detected
		I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_STOP);
	}

	temp3 = pI2CHandle->pI2Cx->SR1 * ( 1 << I2C_SR1_TxE);
	//5. Handle For interrupt generated by TXE event
	if(temp1 && temp2 && temp3)
	{
		// TXE flag is set
		if(pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_MSL)) // checking if device is master
		{
			// we have to do the data transmission
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
			{
				I2C_MasterHandleTXEInterrupt(pI2CHandle);
			}
		}
		else
		{
			// for slave
			if(pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_TRA))
				I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_REQ);
		}
	}

	temp3 = pI2CHandle->pI2Cx->SR1 * ( 1 << I2C_SR1_RxNE);
	//6. Handle For interrupt generated by RXNE event
	if(temp1 && temp2 && temp3)
	{
		// RXNE flag is set
		// we have to do the data Reception
		if(pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_MSL)) // checking if device is master
		{
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
				I2C_MasterHandleRXNEInterrupt(pI2CHandle);
		}
		else
		{
			// for slave
			if(pI2CHandle->pI2Cx->SR2 & ( 1 << I2C_SR2_TRA))
				I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_RCV);
		}

	}
}

/*************************************************************************************************
 * @fn				- I2C_ER_IRQHandling
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
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	uint32_t temp1,temp2;

	//Know the status of  ITERREN control bit in the CR2
	temp2 = (pI2CHandle->pI2Cx->CR2) & ( 1 << I2C_CR2_ITERREN);


	/***********************Check for Bus error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1<< I2C_SR1_BERR);
	if(temp1  && temp2 )
	{
		//This is Bus error

		//Implement the code to clear the buss error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_BERR);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_BERR);
	}

	/***********************Check for arbitration lost error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_ARLO );
	if(temp1  && temp2)
	{
		//This is arbitration lost error

		//Implement the code to clear the arbitration lost error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_ARLO);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_ARLO);
	}

	/***********************Check for ACK failure  error************************************/

	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_AF);
	if(temp1  && temp2)
	{
		//This is ACK failure error

		//Implement the code to clear the ACK failure error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_AF);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_AF);
	}

	/***********************Check for Overrun/underrun error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_OVR);
	if(temp1  && temp2)
	{
		//This is Overrun/underrun

		//Implement the code to clear the Overrun/underrun error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_OVR);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_OVR);
	}

	/***********************Check for Time out error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_TIMEOUT);
	if(temp1  && temp2)
	{
		//This is Time out error

		//Implement the code to clear the Time out error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_TIMEOUT);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_TIMEOUT);
	}

}

/*************************************************************************************************
 * @fn				- I2C_SlaveSendData
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
void I2C_SlaveSendData(I2C_RegDef_t *pI2Cx, uint8_t data)
{
	pI2Cx->DR = data;
}

/*************************************************************************************************
 * @fn				- I2C_SlaveReceiveData
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
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2Cx)
{
	return (uint8_t)pI2Cx->DR;
}
