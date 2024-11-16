/*
 * stm32f407xx_i2c.h
 *
 *  Created on: Sep 15, 2024
 *      Author: Vishal Turaga
 */

#ifndef INC_STM32F407XX_I2C_H_
#define INC_STM32F407XX_I2C_H_

#include "stm32f407xx.h"

/*
 * Configuration structure for the I2C peripheral
 */
typedef struct
{
	uint32_t 	I2C_SCLSpeed;
	uint8_t 	I2C_DeviceAddress;
	uint8_t 	I2C_ACKControl;
	uint16_t	I2C_FMDutyCycle;
}I2C_Config_t;

typedef struct
{
	I2C_Config_t 	I2C_Config;
	I2C_RegDef_t 	*pI2Cx;
	uint8_t			*pTxBuffer; 	/* To store the application Tx buffer address */
	uint8_t			*pRxBuffer;		/* To store the application Rx buffer address */
	uint32_t		TxLen;			/* To store the application Tx Len*/
	uint32_t		RxLen;			/* To store the application Rx Len*/
	uint8_t			TxRxState;		/* To store the Communication state*/
	uint8_t			DevAddr;		/* To store the slave/device address*/
	uint32_t 		RxSize;			/* To store the Rx size*/
	uint8_t			Sr;				/*To store repeated start value*/
}I2C_Handle_t ;

#define I2C_NO_SR			 	RESET
#define I2C_SR					SET

/*
 * I2C Application states
 */
#define I2C_READY				0
#define I2C_BUSY_IN_RX			1
#define I2C_BUSY_IN_TX			2

/*
 * @I2C_SCLSpeed
 */
#define I2C_SCL_SPEED_SM		0		/*Standard speed mode*/
#define I2C_SCL_SPEED_FM		1		/*Fast speed mode*/
#define I2C_SCL_SPEED_SM_KHZ	100000		/*Standard speed mode*/
#define I2C_SCL_SPEED_FM_4KHZ	400000		/*Fast speed mode*/
#define I2C_SCL_SPEED_FM_2KHZ	200000		/*Fast speed mode*/

/*
 * @I2C_ACKControl
 */
#define I2C_ACKCTRL_ACK_DI		0
#define I2C_ACKCTRL_ACK_EN		1

/*
 * @I2C_FMDutyCycle
 */
#define I2C_DUTYCYCLE_2			0
#define I2C_DUTYCYCLE_16_9		1

/*
 * I2C Application Event Macros
 */
#define I2C_EV_TX_CMPLT			0
#define I2C_EV_RX_CMPLT			1
#define I2C_EV_STOP				2

/*
 * I2C Application Error Macros
 */

#define I2C_ERROR_BERR			3
#define I2C_ERROR_ARLO			4
#define I2C_ERROR_AF			5
#define I2C_ERROR_OVR			6
#define I2C_ERROR_TIMEOUT		7
#define I2C_EV_DATA_REQ			8
#define I2C_EV_DATA_RCV			9


/**************************************************************************************************************************************
 * 														APIs supported by this driver
 * 									For more information about the APIs check the function definitions
 **************************************************************************************************************************************/

/*
 * Enable/Disbale some bits
 */
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx , uint32_t FlagName);

/*
 * Enable/Disable I2C peripheral
 */
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi);

/*
 * Clock control
 */
void I2C_ClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi);

/*
 * I2C Init and De-Init
 */
void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_Deinit(I2C_RegDef_t *pI2Cx);

/*
 * I2C Data receive and Send
 */

void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *TxBuffer, uint8_t len, uint8_t SlaveAddr, uint8_t Sr);
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *RxBuffer, uint8_t len, uint8_t SlaveAddr, uint8_t Sr);

/*
 * I2C Data receive and Send with Interrupt Functionality
 */
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *TxBuffer, uint8_t len, uint8_t SlaveAddr, uint8_t Sr);
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *RxBuffer, uint8_t len, uint8_t SlaveAddr, uint8_t Sr);

/*
 * IRQ configuration and ISR Handling
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle);
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle);
void I2C_CloseSendData(I2C_Handle_t *pI2CHandle);
void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle);

/*
 * Slave Data send and Receive
 */
void I2C_SlaveSendData(I2C_RegDef_t *pI2Cx, uint8_t data);
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2Cx);


/*
 * Application callback
 */
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv);



#endif /* INC_STM32F407XX_I2C_H_ */
