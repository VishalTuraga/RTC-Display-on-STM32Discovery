/*
 * ds1307.c
 *
 *  Created on: Nov 17, 2024
 *      Author: Vishal Turaga
 */

#include "ds1307.h"

/*
 * Covert Decimal to BCD and vice versa
 */
static uint8_t BCDtobin(uint8_t BCD);
static uint8_t bintoBCD(uint8_t bin);

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
