/*
 * stm32f407xx_gpio_driver.c
 *
 *  Created on: Aug 26, 2024
 *      Author: Vishal Turaga
 */

#include "stm32f407xx_gpio.h"
#include <stdint.h>

/*
 * GPIO Periheral clock control
 */

/*************************************************************************************************
 * @fn				- GPIO_ClockControl
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
void GPIO_ClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_CLK_EN();
		}
		else if(pGPIOx == GPIOB)
		{
			GPIOB_CLK_EN();
		}
		else if(pGPIOx == GPIOC)
		{
			GPIOC_CLK_EN();
		}
		else if(pGPIOx == GPIOD)
		{
			GPIOD_CLK_EN();
		}
		else if(pGPIOx == GPIOE)
		{
			GPIOE_CLK_EN();
		}
		else if(pGPIOx == GPIOF)
		{
			GPIOF_CLK_EN();
		}
		else if(pGPIOx == GPIOG)
		{
			GPIOG_CLK_EN();
		}
		else if(pGPIOx == GPIOH)
		{
			GPIOH_CLK_EN();
		}
		else if(pGPIOx == GPIOI)
		{
			GPIOI_CLK_EN();
		}
	}
	else
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_CLK_DI();
		}
		else if(pGPIOx == GPIOB)
		{
			GPIOB_CLK_DI();
		}
		else if(pGPIOx == GPIOC)
		{
			GPIOC_CLK_DI();
		}
		else if(pGPIOx == GPIOD)
		{
			GPIOD_CLK_DI();
		}
		else if(pGPIOx == GPIOE)
		{
			GPIOE_CLK_DI();
		}
		else if(pGPIOx == GPIOF)
		{
			GPIOF_CLK_DI();
		}
		else if(pGPIOx == GPIOG)
		{
			GPIOG_CLK_DI();
		}
		else if(pGPIOx == GPIOH)
		{
			GPIOH_CLK_DI();
		}
		else if(pGPIOx == GPIOI)
		{
			GPIOI_CLK_DI();
		}
	}
}

/*
 * GPIO Init and DeInit
 */
/*************************************************************************************************
 * @fn				- GPIO_Init
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
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
	// Enable peripheral clock
	GPIO_ClockControl(pGPIOHandle->pGPIOx, ENABLE);

	uint32_t temp = 0;
	// 1. Configure the mode of GPIO pin
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
	{
		// Non interrupt mode
		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2* pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
		pGPIOHandle->pGPIOx->MODER &= ~(0x3 << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
		pGPIOHandle->pGPIOx->MODER |= temp;
	}
	else
	{
		pGPIOHandle->pGPIOx->MODER &= ~(0x3 << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT)
		{
			// 1. Configure the FTSR
			// first we clear the corresponding RTSR bit so that we only have FTSR
			EXTI->RTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
		else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT)
		{
			// 1. Configure the RTSR
			// first we clear the corresponding FTSR bit so that we only have RTSR
			EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
		else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT)
		{
			// 1. Configure both FTSR and RTSR
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}

		// 2. configure the GPIO port selection in SYSCFG_EXTICR
		// Calculate the register number EXTICR1/EXTICR2/EXTICR3/EXTICR4
		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber/4);
		uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);
		SYSCFG_CLK_EN();
		SYSCFG->EXTICR[temp] |= (portcode << (4*(pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)%4));

		// 3. Enable the EXTI interrupt delivery using IMR
		EXTI->IMR |= 1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber;

	}

	// 2. Configure the speed
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3 << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OSPEEDR |= temp;

	// 3. Configure the pupd settings
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->PUPDR &= ~(0x3 << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->PUPDR |= temp;

	// 4. Conigure the optype
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->OTYPER &= ~(0x1 << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OTYPER |= temp;

	// 5. configure the alt function
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN)
	{
		uint32_t pinpos = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber%8;

		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4*pinpos));
		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber <= GPIO_PIN_7)
		{
			// use AFL

			pGPIOHandle->pGPIOx->AFRL |= temp;
		}
		else
		{
			// use AFH
			pGPIOHandle->pGPIOx->AFRH |= temp;
		}
	}
}

/*************************************************************************************************
 * @fn				- GPIO_DeInit
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
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
	if(pGPIOx == GPIOA)
	{
		RCC->AHB1RSTR |= (1 << 0);
		RCC->AHB1RSTR &= ~(1 << 0);
	}
	else if(pGPIOx == GPIOB)
	{
		RCC->AHB1RSTR |= (1 << 1);
		RCC->AHB1RSTR &= ~(1 << 1);
	}
	else if(pGPIOx == GPIOB)
	{
		RCC->AHB1RSTR |= (1 << 2);
		RCC->AHB1RSTR &= ~(1 << 2);
	}
	else if(pGPIOx == GPIOB)
	{
		RCC->AHB1RSTR |= (1 << 3);
		RCC->AHB1RSTR &= ~(1 << 3);
	}
	else if(pGPIOx == GPIOB)
	{
		RCC->AHB1RSTR |= (1 << 4);
		RCC->AHB1RSTR &= ~(1 << 4);
	}
	else if(pGPIOx == GPIOB)
	{
		RCC->AHB1RSTR |= (1 << 5);
		RCC->AHB1RSTR &= ~(1 << 5);
	}
	else if(pGPIOx == GPIOB)
	{
		RCC->AHB1RSTR |= (1 << 6);
		RCC->AHB1RSTR &= ~(1 << 6);
	}
	else if(pGPIOx == GPIOB)
	{
		RCC->AHB1RSTR |= (1 << 7);
		RCC->AHB1RSTR &= ~(1 << 7);
	}
	else if(pGPIOx == GPIOB)
	{
		RCC->AHB1RSTR |= (1 << 8);
		RCC->AHB1RSTR &= ~(1 << 8);
	}
}

/*
 * GPIO Pin/Port input
 */

/*************************************************************************************************
 * @fn				- GPIO_ReadPin
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
uint8_t GPIO_ReadPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	return (uint8_t)((pGPIOx->IDR >> PinNumber) & 0x00000001);
}

/*************************************************************************************************
 * @fn				- GPIO_ReadPort
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
uint16_t GPIO_ReadPort(GPIO_RegDef_t *pGPIOx)
{
	return (uint16_t)(pGPIOx->IDR);
}
/*
 * GPIO Pin/Port Output
 */

/*************************************************************************************************
 * @fn				- GPIO_WritePin
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
void GPIO_WritePin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t value)
{
	if(value == GPIO_PIN_RESET)
	{
		// write 1 to the output data register at the bit filed corresponding to the pin number
		pGPIOx->ODR |= (1 << PinNumber);
	}
	else
	{
		// write 0
		pGPIOx->ODR &= ~(1<<PinNumber);
	}
}

/*************************************************************************************************
 * @fn				- GPIO_WritePort
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
void GPIO_WritePort(GPIO_RegDef_t *pGPIOx, uint16_t value)
{
	pGPIOx->ODR = value;
}

/*************************************************************************************************
 * @fn				- GPIO_TogglePinOutput
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
void GPIO_TogglePinOutput(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	pGPIOx->ODR ^= (1<<PinNumber);
}

/*
 * GPIO IRQ
 */
/*************************************************************************************************
 * @fn				- GPIO_IRQConfig
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
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority)
{
	// NOTE: Only the first 4 bits (starting from MSB) are implemented in each of the 8 bits assigned.
	uint8_t iprx 			= IRQNumber/4;
	uint8_t iprx_section 	= IRQNumber%4;
	uint8_t shift_amount	= (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_IPR_BASEADDR + (iprx))	|= (IRQPriority << shift_amount);
}

/*************************************************************************************************
 * @fn				- GPIO_IRQConfig
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
void GPIO_IRQITConfig(uint8_t IRQNumber, uint8_t EnorDi)
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
 * @fn				- GPIO_IRQHandling
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
void GPIO_IRQHandling(uint8_t PinNumber)
{
	// clear the EXTI PR register corresponding to the pin number
	if(EXTI->PR & (1<<PinNumber))
	{
		// the interrupt is pended so we clear the register bit
		EXTI->PR |= (1 << PinNumber);
	}
}

