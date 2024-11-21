/*
 * ds1307.c
 *
 *  Created on: Nov 17, 2024
 *      Author: Vishal Turaga
 */

#include "ds1307.h"
#include <stdint.h>
#include <string.h>

I2C_Handle_t rtcHandle;

static void RTC_DS1307_I2C_PinConfig(void);
static void RTC_DS1307_I2C_Config(void);
static void RTC_DS1307_write(uint8_t data, uint8_t addr);
static uint8_t RTC_DS1307_read(uint8_t addr);
static uint8_t bintoBCD(uint8_t bin);
static uint8_t BCDtobin(uint8_t BCD);

/*
 * Helper functions
 */
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

static void RTC_DS1307_I2C_Config()
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
 * @return            - if it returns 1 -> init failed (as CH bit is 1)
 * 					  -	if it returns 0 -> init succeess (as CH bit is 0)
 *
 * @Note              -  none

 *********************************************************************/
uint8_t RTC_DS1307_Init(void)
{
	// 1. Iniitialize the GPIO pins for I2C
	RTC_DS1307_I2C_PinConfig();

	// 2. Initialize the I2C pins
	RTC_DS1307_I2C_Config();

	// 3. Enable the I2C peripheral
	I2C_PeripheralControl(RTC_DS1307_I2C, ENABLE);

	// 4. By default, the CH pin is set to 1. To start the clock, we should make CH = 0
	RTC_DS1307_write(0x00,RTC_DS1307_REG_SECONDS);

	// 5. Read back clock halt bit to confirm if it is really set to 0
	uint8_t clockState = RTC_DS1307_read(RTC_DS1307_REG_SECONDS);

	return ((clockState >> 7) & 0x1);

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
	// Set the seconds
	uint8_t secondBCD = bintoBCD(timeHandle->seconds);

	// set the minutes
	uint8_t minuteBCD = bintoBCD(timeHandle->minutes);

	// set the hours
	uint8_t hourBCD = bintoBCD(timeHandle->hours);
	uint8_t temp = 0;

	RTC_DS1307_write(secondBCD & 0x7F, RTC_DS1307_REG_SECONDS);

	RTC_DS1307_write(minuteBCD, RTC_DS1307_REG_MINUTES);

	if(timeHandle->timeFormat == RTC_DS1307_TIME_FORMAT_24HRS)
	{
		temp = 0 << 6; // resetting the bit 6 to make is 24 hour
		temp |= ((hourBCD/10) << 4);
		temp |= hourBCD%10;
	}
	else
	{
		temp = 1 << 6; // setting the bit 6 to make is 12 hour
		if(timeHandle->timeFormat == RTC_DS1307_TIME_FORMAT_12HRS_PM) // setting/resetting the 5th bit based on AM PM
		{
			temp |= 1 << 5;
		}
		else
		{
			temp &= ~(1 << 5);
		}
		temp |= ((hourBCD/10) << 4);
		temp |= (hourBCD%10);
	}
	RTC_DS1307_write(temp,RTC_DS1307_REG_HOURS);
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
	uint8_t value;

	// fetch seconds data
	value = RTC_DS1307_read(RTC_DS1307_REG_SECONDS);
	// clear the CH bit as it is not relevant to us when we fetch the seconds value
	value &= ~(1 << RTC_DS1307_REG_SECONDS_CH);

	timeHandle->seconds = BCDtobin(value);

	// fetch minutes data
	value = RTC_DS1307_read(RTC_DS1307_REG_MINUTES);
	timeHandle->minutes = BCDtobin(value);

	//fetch hours data
	value = RTC_DS1307_read(RTC_DS1307_REG_HOURS);
	// check if 6th bit is set.
	if(value & (1 << 6)) // if bit 6 is set, then its 12 hour clock
	{
		if(value  & (1 << 5)) // if bit 5 is set, then it is PM
		{
			timeHandle->timeFormat = RTC_DS1307_TIME_FORMAT_12HRS_PM;
		}
		else
		{
			timeHandle->timeFormat = RTC_DS1307_TIME_FORMAT_12HRS_AM;
		}
		// get rid of bit 6 and bit 5 and fetch the hours data
		value &= ~(0x3 << RTC_DS1307_REG_HOURS_AMPM_10HOUR);
	}
	else
	{
		timeHandle->timeFormat = RTC_DS1307_TIME_FORMAT_24HRS;
		// get rid of bit 6 and fetch the hours data
		value &= ~(1 << RTC_DS1307_REG_HOURS_12_24);
	}
	timeHandle->hours = BCDtobin(value);
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
	RTC_DS1307_write(dateHandle->date, RTC_DS1307_REG_DATE);

	RTC_DS1307_write(dateHandle->month, RTC_DS1307_REG_MONTH);

	RTC_DS1307_write(dateHandle->year, RTC_DS1307_REG_YEAR);

	RTC_DS1307_write(dateHandle->day, RTC_DS1307_REG_DAY);
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
	uint8_t value;

	// Fetch the day
	value = RTC_DS1307_read(RTC_DS1307_REG_DAY);
	dateHandle->day = value;

	// fetch the date
	value = RTC_DS1307_read(RTC_DS1307_REG_DATE);
	dateHandle->date = BCDtobin(value);

	// fetch the month
	value = RTC_DS1307_read(RTC_DS1307_REG_MONTH);
	dateHandle->month = BCDtobin(value);

	// fetch the date
	value = RTC_DS1307_read(RTC_DS1307_REG_YEAR);
	dateHandle->year = BCDtobin(value);


}

/*********************************************************************
 * @fn      		  - RTC_DS1307_read
 *
 * @brief             -
 *
 * @param[in]         -
 *
 * @return            -  none
 *
 * @Note              -  none

 *********************************************************************/
uint8_t RTC_DS1307_read(uint8_t addr)
{
	/* slave will start sending the data from where an address pointer is pointing to
	 * Therefore, we should first put initialize a pointer to the position from which we want to read (before we perform the read)
	 */

	uint8_t data;

	// 1. Data write to set the pointer to the address from which we want to read
	I2C_MasterSendData(&rtcHandle, &addr, 1, RTC_DS1307_SLAVE_ADDR, 0);

	// 2. Now we read from the data in the address held in addr and store in the variable data
	I2C_MasterReceiveData(&rtcHandle, &data, 1, RTC_DS1307_SLAVE_ADDR, 0);

	return data;
}

/*********************************************************************
 * @fn      		  - RTC_DS1307_write
 *
 * @brief             -
 *
 * @param[in]         -
 *
 * @return            -  none
 *
 * @Note              -  none

 *********************************************************************/
void RTC_DS1307_write(uint8_t data, uint8_t addr)
{
	uint8_t tx[2];
	tx[0] = addr;
	tx[1] = data;

	I2C_MasterSendData(&rtcHandle, tx, 2, RTC_DS1307_SLAVE_ADDR, 0);
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
	uint8_t A0,A1;

	A1 = (uint8_t)((BCD >> 4)*10);
	A0 = (BCD & (uint8_t)0x0F);
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
