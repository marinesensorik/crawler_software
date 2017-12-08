/*
 * I2C_CMPS11.cpp
 *
 * small i2c Library for CMPS11 Compass Module
 *
 * Created: 24.11.2014 21:00:13
 *  Author: Jonas Mueller DF9FW
 *
 * !!! CONVERTED TO ARDUINO 26.10.2017 JM !!!
 */ 
  
#include <Arduino.h>
#include <Wire.h>
#include "i2c_functions.h"
#include "I2C_CMPS11.h"

double CMPS11_getBearing()
{
	uint8_t low, high=0;
	uint16_t result=0;
	double bearing=0;
	
	low=twi_readregister(CMPS11, CMPS11_Bearing_low);
	if(twi_Error_code()) return 0.0;
	high=twi_readregister(CMPS11, CMPS11_Bearing_high);
	if(twi_Error_code()) return 0.0;
	result=uniq(low,high);
	bearing=result/10;
	return bearing;
}

int8_t CMPS11_getPitch()
{
	int8_t result=0;
	result=twi_readregister(CMPS11, CMPS11_Pitch);
	if(twi_Error_code()) return 0;
	return result;
}

int8_t CMPS11_getRoll()
{
	int8_t result=0;
	result=twi_readregister(CMPS11, CMPS11_Roll);
	if(twi_Error_code()) return 0;
	return result;
}
