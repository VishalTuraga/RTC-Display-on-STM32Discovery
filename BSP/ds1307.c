/*
 * ds1307.c
 *
 *  Created on: Nov 17, 2024
 *      Author: Vishal Turaga
 */

#include "ds1307.h"

I2C_Handle_t rtcHandle;

static void RTC_DS1307_I2C_PinConfig();
static void RTC_SD1307_I2C_Config();

static void RTC_DS1307_I2C_PinConfig()
{
	GPIO_Handle_t sda, scl;
	memset(&sda,0,sizeof(sda));
	memset(&scl,0,sizeof(scl));

	sda.pGPIOx = RTC_DS1307_I2C_PORT;
	sda.GPIO_PinConfig.GPIO_PinNumber = RTC_DS1307_I2C_SDA;
	sda.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	sda.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_ALTFN_AF4;
	sda.GPIO_PinConfig.GPIO_PinPuPdControl = RTC_Ds1307_I2C_PUPD;
	sda.GPIO_PinConfig.GPIO_PinSpeed = RTC_DS1307_I2C_PIN_SPEED;
	sda.GPIO_PinConfig.GPIO_PinOPType = GPIO_OUT_TYPE_OD;

	GPIO_Init(&sda);

	scl.pGPIOx = RTC_DS1307_I2C_PORT;
	scl.GPIO_PinConfig.GPIO_PinNumber = RTC_DS1307_I2C_SCL;
	scl.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	scl.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_ALTFN_AF4;
	scl.GPIO_PinConfig.GPIO_PinSpeed = RTC_DS1307_I2C_PIN_SPEED;
	scl.GPIO_PinConfig.GPIO_PinOPType = GPIO_OUT_TYPE_OD;
	scl.GPIO_PinConfig.GPIO_PinPuPdControl = RTC_Ds1307_I2C_PUPD;

	GPIO_Init(&scl);
}

static void RTC_SD1307_I2C_Config()
{
	rtcHandle.pI2Cx = RTC_DS1307_I2C;
	rtcHandle.I2C_Config.I2C_ACKControl = I2C_ACKCTRL_ACK_EN;
	rtcHandle.I2C_Config.I2C_DeviceAddress = RTC_DS1307_SLAVE_ADDR;
	rtcHandle.I2C_Config.I2C_SCLSpeed = RTC_DS1307_I2C_SPEED;

	I2C_Init(&rtcHandle);
}

/*
 * Initialize the RTC module
 */
/*********************************************************************
 * @fn      		  - RTC_DS1307_Init
 *
 * @brief             -
 *
 * @param[in]         -
 *
 * @return            -  none
 *
 * @Note              -  none

 *********************************************************************/
uint8_t RTC_DS1307_Init(void)
{
	// 1. Iniitialize the GPIO pins for I2C
	RTC_DS1307_I2C_PinConfig();

	// 2. Initialize the I2C pins

	// 3. Enable the I2C pins

}

/*
 * set and get Time
 */
/*********************************************************************
 * @fn      		  - RTC_DS1307_setTime
 *
 * @brief             -
 *
 * @param[in]         -
 *
 * @return            -  none
 *
 * @Note              -  none

 *********************************************************************/
void RTC_DS1307_setTime(RTC_Handle_time_t *timeHandle)
{

}

/*********************************************************************
 * @fn      		  - RTC_DS1307_getTime
 *
 * @brief             -
 *
 * @param[in]         -
 *
 * @return            -  none
 *
 * @Note              -  none

 *********************************************************************/
void RTC_DS1307_getTime(RTC_Handle_time_t *timeHandle)
{

}

/*
 * set and get Full Date
 */
/*********************************************************************
 * @fn      		  - RTC_DS1307_setFullDate
 *
 * @brief             -
 *
 * @param[in]         -
 *
 * @return            -  none
 *
 * @Note              -  none

 *********************************************************************/
void RTC_DS1307_setFullDate(RTC_Handle_date_t *dateHandle)
{

}

/*********************************************************************
 * @fn      		  - RTC_DS1307_getFullDate
 *
 * @brief             -
 *
 * @param[in]         -
 *
 * @return            -  none
 *
 * @Note              -  none

 *********************************************************************/
void RTC_DS1307_getFullDate(RTC_Handle_date_t *dateHandle)
{

}

/*
 * Covert Decimal to BCD and vice versa
 */
/*********************************************************************
 * @fn      		  - BCDtobin
 *
 * @brief             - This function converts BCD value to binary
 *
 * @param[in]         -
 *
 * @return            -  none
 *
 * @Note              -  none

 *********************************************************************/
static uint8_t BCDtobin(uint8_t BCD)
{

}

/*********************************************************************
 * @fn      		  - BCDtobin
 *
 * @brief             - This function converts BCD value to binary
 *
 * @param[in]         -
 *
 * @return            -  none
 *
 * @Note              -  none

 *********************************************************************/
static uint8_t bintoBCD(uint8_t bin)
{

}

/*********************************************************************
 * @fn      		  - BCDtobin
 *
 * @brief             - This function converts BCD value to binary
 *
 * @param[in]         -
 *
 * @return            -  none
 *
 * @Note              -  none

 *********************************************************************/
static uint8_t BCDtobin(uint8_t BCD)
{
	uint8_t bin;
	uint8_t A0,A1;

	A1 = (uint8_t)((bin >> 4)*10);
	A0 = (bin & (uint8_t)0x0F);
	return (A1+A0);
}

/*********************************************************************
 * @fn      		  - bintoBCD
 *
 * @brief             - This function converts Binary to BCD
 *
 * @param[in]         -
 *
 * @return            -  none
 *
 * @Note              -  none

 *********************************************************************/
static uint8_t bintoBCD(uint8_t bin)
{
	uint8_t bcd;
	uint8_t A0,A1;

	bcd = bin;
	if(bin > 9)
	{
		A1 = bin/10; // gives the digit in tens place
		A0 = bin%10; // gives the digit in ones place

		bcd = (A1 << 4) | A0; //shift the tens digit by 4 digits and add the ones digit if the first 4 bits
	}

	return bcd;
}
