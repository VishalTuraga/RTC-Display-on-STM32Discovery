/*
 * ds1307.c
 *
 *  Created on: Nov 17, 2024
 *      Author: Vishal Turaga
 */

/*
 *	Contains RTC related info
		I2c address
		Register address
		Data Structure to handle the information
		Function prototypes which are exposed to applications
		Application configurable items
 */

/*
 * Notes
 * 	- RTC registers are located in address locations 00h to 07h
 * 	- RAM registers are located in address locations 08h to 3Fh
 * 	- Contents of the time and calendar registers are in BCD Format
 * 	- When CH bit is set to 1, oscillator is disabled. When reset to 0, oscillator is enabled
 * 	- On first application of power, date and time are reset to 01/01/00 01 00:00:00 (MM/DD/YY DOW HH:MM:SS) (DOW -> Day of Week)
 * 	- Bit 6 of hours register is defined as the 12 hour or 24 hour mode select bit. When HIGH->12 hour mode select.
 * 	- In 12 hour mode, bit 5 is the AM/PM bit with Logic HIGH being PM
 * 	- in 24 hour mode, bit 5 is the second 10 hour bit (20 to 23 hours)
 * 	- The hours valuue must be re-entered whenever the 12/24 hour mode bit is changed
 * 	- DS1307 operates in two modes
 * 		- Slave Receiver mode (write mode for Master)
 * 		- Slave Transmitter mode (read mode for Master)
 * 	- DS1307 SLAVE ADDRESS = 0b1101000 => 0x68
 */

/*
 * Register map
 * ADDRESS	BIT7	BIT6	BIT5	BIT4	BIT3	BIT2	BIT1	BIT0	Function	Range
 * 00h		CH		<----10 Seconds---->	<---------Seconds---------->	Seconds		00-59
 * 01h		0		<----10 Minutes---->	<---------Minutes---------->	Minutes		00-59
 * 02h		0		12		10hour	10hour	<---------Minutes---------->	Hours		1-12AM/PM
 * 					24		PM/AM	10hour	<---------Minutes---------->	Hours		00-23
 * 03h		0		0		0		0		0		<-------Day-------->	Day			01-07
 * 04h		0		0		<-10 Date-->	<----------Date------------>	Date		01-31
 * 05h		0		0		0		10Mnth	<----------Month----------->	Month		01-12
 * 06h		<--------10 Year----------->	<-----------Year----------->	Year		00-99
 * 07h		OUT		0		0		SQWE	0		0		RS1		RS0		Control		-
 * 08h-3Fh																	RAM(56x8)	00h-FFh
 *
 * 0 -> always reads back as 0
 */

#include "ds1307.h"

#define RTC_DS1307_SLAVE_ADDR		0x68

/*
 * Address macros
 */
#define RTC_DS1307_REG_SECONDS		0x00
#define RTC_DS1307_REG_MINUTES		0x01
#define RTC_DS1307_REG_HOURS		0x02
#define RTC_DS1307_REG_DAY			0x03
#define RTC_DS1307_REG_DATE			0x04
#define RTC_DS1307_REG_MONTH		0x05
#define RTC_DS1307_REG_YEAR			0x06
#define RTC_DS1307_REG_CONTROL		0x07
#define RTC_DS1307_RAM_START		0x08

/*
 * Data Structure to handle the information
 */

typedef struct{
	uint8_t RTC_DS1307_Seconds;
	uint8_t RTC_DS1307_Minutes;
	uint8_t RTC_DS1307_Hours;
	uint8_t RTC_DS1307_Day;
	uint8_t RTC_DS1307_Date;
	uint8_t RTC_DS1307_Month;
	uint8_t RTC_DS1307_Year;
	uint8_t RTC_DS1307_Control;
};

/*
 * RTC_DS1307_REG_SECONDS Macros
 */
#define RTC_DS1307_REG_SECONDS_SECONDS			0
#define RTC_DS1307_REG_SECONDS_10SECONDS		4
#define RTC_DS1307_REG_SECONDS_CH				7

/*
 * RTC_DS1307_REG_MINUTES Macros
 */
#define RTC_DS1307_REG_MINUTES_MINUTES			0
#define RTC_DS1307_REG_MINUTES_10MINUTES		4

/*
 * RTC_DS1307_REG_HOURS Macros
 */
#define RTC_DS1307_REG_HOURS_HOURS				0
#define RTC_DS1307_REG_HOURS_10HOUR				4
#define RTC_DS1307_REG_HOURS_AMPM_10HOUR		5
#define RTC_DS1307_REG_HOURS_12_24				6

/*
 * RTC_DS1307_REG_DAY Macros
 */
#define RTC_DS1307_REG_DAY_DAY					0

/*
 * RTC_DS1307_REG_DATE Macros
 */
#define RTC_DS1307_REG_DATE_DATE				0
#define RTC_DS1307_REG_DATE_10DATE				0

/*
 * RTC_DS1307_REG_MONTH Macros
 */
#define RTC_DS1307_REG_MONTH_MONTH				0
#define RTC_DS1307_REG_MONTH_10MONTH			4

/*
 * RTC_DS1307_REG_YEAR Macros
 */
#define RTC_DS1307_REG_YEAR_YEAR				0
#define RTC_DS1307_REG_YEAR_10YEAR				4

/*
 * RTC_DS1307_REG_CONTROL Macros
 */
#define RTC_DS1307_REG_YEAR_RS0					0
#define RTC_DS1307_REG_YEAR_RS1					1
#define RTC_DS1307_REG_YEAR_SQWE				4
#define RTC_DS1307_REG_YEAR_OUT					7

