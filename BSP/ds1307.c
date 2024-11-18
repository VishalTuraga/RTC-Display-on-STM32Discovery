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
static uint8_t BCDtoDec(uint8_t BCD);
static uint8_t DectoBCD(uint8_t Dec);
