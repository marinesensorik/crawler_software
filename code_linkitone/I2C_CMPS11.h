/*
 * I2C_CMPS11.h
 *
 * small i2c Library for CMPS11 Compass Module
 *
 * Created: 24.11.2014 20:52:37
 *  Author: Jonas Mueller DF9FW
 *
 * !!! CONVERTED TO ARDUINO 26.10.2017 JM !!!
 */ 

#ifndef I2C_CMPS11_H_
#define I2C_CMPS11_H_

#ifndef CMPS11
#define CMPS11 0xC0
#endif

#define uniq(LOW,HEIGHT)	((HEIGHT << 8)|LOW)			  // Create 16 bit number from two bytes
#define LOW_BYTE(x)        	(x & 0xff)					  // Get low byte from 16 bit number
#define HIGH_BYTE(x)       	((x >> 8) & 0xff)			  // Get high byte from 16 bit number

#define CMPS11_Bearing_high	0x02
#define CMPS11_Bearing_low	0x03

#define CMPS11_Pitch		0x04

#define CMPS11_Roll			0x05

#endif /* I2C_CMPS11_H_ */

double	 CMPS11_getBearing();
int8_t	 CMPS11_getPitch();
int8_t	 CMPS11_getRoll();