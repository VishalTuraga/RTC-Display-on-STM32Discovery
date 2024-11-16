 /*
 * stm32f407xx_gpio.h
 *
 *  Created on: Aug 26, 2024
 *      Author: Vishal Turaga
 */

#ifndef STM32F407XX_GPIO_H_
#define STM32F407XX_GPIO_H_

#include "stm32f407xx.h"
#include <stdint.h>

/*
 * This is the configuration structure for a GPIO pin
 */
typedef struct
{
	uint8_t GPIO_PinNumber;				/*Possible values from @GPIO_PINS*/
	uint8_t GPIO_PinMode;				/*Possible values from @GPIO_PIN_MODES*/
	uint8_t GPIO_PinSpeed;
	uint8_t GPIO_PinPuPdControl;
	uint8_t GPIO_PinOPType;
	uint8_t GPIO_PinAltFunMode;
}GPIO_PinConfig_t;

/*
 * This is a handle structure for a GPIO pin
 */
typedef struct
{
	GPIO_RegDef_t *pGPIOx; /*	Pointer to hold the base address of the GPIO peripheral*/
	GPIO_PinConfig_t GPIO_PinConfig;
}GPIO_Handle_t;



/*
 * GPIO pin values
 * @GPIO_PINS
 */
#define GPIO_PIN_0					0
#define GPIO_PIN_1					1
#define GPIO_PIN_2					2
#define GPIO_PIN_3					3
#define GPIO_PIN_4					4
#define GPIO_PIN_5					5
#define GPIO_PIN_6					6
#define GPIO_PIN_7					7
#define GPIO_PIN_8					8
#define GPIO_PIN_9					9
#define GPIO_PIN_10					10
#define GPIO_PIN_11					11
#define GPIO_PIN_12					12
#define GPIO_PIN_13					13
#define GPIO_PIN_14					14
#define GPIO_PIN_15					15

/*
 * GPIO pin possible modes
 * @GPIO_PIN_MODES
 */
#define GPIO_MODE_IN				0
#define GPIO_MODE_OUT				1
#define GPIO_MODE_ALTFN				2
#define GPIO_MODE_ANALOG			3
#define GPIO_MODE_IT_FT				4	// GPIO Mode interrupt falling edge (self made mode)
#define GPIO_MODE_IT_RT				5	// GPIO Mode interrupt rising edge (self made mode)
#define GPIO_MODE_IT_RFT			6	// GPIO Mode interrpt rising edge falling edge trigger (self made mode)

/*
 * GPIO port output types
 */
#define GPIO_OUT_TYPE_PP			0
#define GPIO_OUT_TYPE_OD			1

/*
 * GPIO Port output speed
 */
#define GPIO_OUT_SPEED_LOW			0
#define GPIO_OUT_SPEED_MED			1
#define GPIO_OUT_SPEED_HIGH			2
#define GPIO_OUT_SPEED_VERYHIGH		3

/*
 * GPIO pin pull up pull down
 */
#define GPIO_PUPD_NONE				0
#define GPIO_PUPD_PULLUP			1
#define GPIO_PUPD_PULLDOWN			2

/*
 * GPIO Alternate function
 */
#define GPIO_ALTFN_AF0				0
#define GPIO_ALTFN_AF1				1
#define GPIO_ALTFN_AF2				2
#define GPIO_ALTFN_AF3				3
#define GPIO_ALTFN_AF4				4
#define GPIO_ALTFN_AF5				5
#define GPIO_ALTFN_AF6				6
#define GPIO_ALTFN_AF7				7
#define GPIO_ALTFN_AF8				8
#define GPIO_ALTFN_AF9				9
#define GPIO_ALTFN_AF10				10
#define GPIO_ALTFN_AF11				11
#define GPIO_ALTFN_AF12				12
#define GPIO_ALTFN_AF13				13
#define GPIO_ALTFN_AF14				14
#define GPIO_ALTFN_AF15				15


/**************************************************************************************************************************************
 * 														APIs supported by this driver
 * 									For more information about the APIs check the function definitions
 **************************************************************************************************************************************/
/*
 * GPIO Init and DeInit
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);

/*
 * GPIO Periheral clock contro
 */
void GPIO_ClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi);

/*
 * GPIO Pin/Port input
 */
uint8_t  GPIO_ReadPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadPort(GPIO_RegDef_t *pGPIOx);

/*
 * GPIO Pin/Port Output
 */
void GPIO_WritePin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t value);
void GPIO_WritePort(GPIO_RegDef_t *pGPIOx, uint16_t value);
void GPIO_TogglePinOutput(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);


/*
 * GPIO IRQ
 */
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);
void GPIO_IRQITConfig(uint8_t IRQNumber, uint8_t EnorDi);
void GPIO_IRQHandling(uint8_t PinNumber);


#endif /* STM32F407XX_GPIO_H_ */
