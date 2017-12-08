/*
 * i2c_functions.cpp
 * 
 * Standart I2C Funtions 
 * Expects 8bit Write Address for all Operations
 *
 * Created: 31.10.2014 21:53:34
 * Author: Jonas Müller DF9FW
 *
 * !!! CONVERTED TO ARDUINO, 26.10.2017 JM !!!
 */ 

#include <Arduino.h>
#include <Wire.h>
#include "i2c_functions.h"

unsigned char i2c_Err_code=0; //Error Flag: 0->all ok 1->Error in last action

unsigned char twi_Error_code()
{
	return i2c_Err_code;
}

unsigned char twi_readregister(unsigned char addr, unsigned char reg)
{
	unsigned char ret;
	unsigned char data;
	
	i2c_Err_code=0;
	Wire.beginTransmission(Addr8to7(addr));		 // set device address and write mode
	Wire.write(reg);						     // write the command
	ret = Wire.endTransmission();				 // stop transmitting

	if ( ret ) {
		/* failed to issue start condition, possibly no device found */
		i2c_Err_code=1;
		return 1;
	}
	else
	{
		if (Wire.requestFrom(Addr8to7(addr), 1) != 1)
		{
			ret = 1;
			i2c_Err_code = 1;
			return 1;
		}

		if (Wire.available())
			data = Wire.read();
		else
		{
			ret = 1;
			i2c_Err_code = 1;
			return 1;
		}

		return data ;
	}
}

unsigned char twi_writeregister(unsigned char addr, unsigned char reg, unsigned char value)
{
	unsigned char ret;
	
	i2c_Err_code=0;
	Wire.beginTransmission(Addr8to7(addr));  // set device address and write mode
	Wire.write(reg);				         // write the register
	Wire.write(value);						 // write the value
	ret = Wire.endTransmission();			 // stop transmitting
	
	if ( ret ) {
		/* failed to issue transmission possibly no device found */
		i2c_Err_code=1;
		return 1;
	}

	return 0;
}

void twi_sendCommand(unsigned char addr, unsigned char command)
{
	unsigned char ret;

	i2c_Err_code=0;	   
	Wire.beginTransmission(Addr8to7(addr));  // set device address and write mode
	Wire.write(command);				     // write the command
	ret=Wire.endTransmission();				 // stop transmitting

	if ( ret ) {
		/* failed to issue transmission, possibly no device found */
		i2c_Err_code=1;
	}
}

uint32_t twi_read32_rs(unsigned char addr, unsigned char reg)
{
	unsigned char ret;
	unsigned char data;
	uint32_t result=0;
	
	i2c_Err_code=0;
	Wire.beginTransmission(Addr8to7(addr));		 // set device address and write mode
	Wire.write(reg);						     // write the command
	ret = Wire.endTransmission();				 // stop transmitting

	if (ret) {
		/* failed to issue start condition, possibly no device found */
		i2c_Err_code = 1;
		return 1;
	}
	else
	{
		if (Wire.requestFrom(Addr8to7(addr), 3) != 3)
		{
			ret = 1;
			i2c_Err_code = 1;
			return 1;
		}

		for (uint8_t i = 3; i > 0; i--)
		{
			if (Wire.available())
				data = Wire.read();
			else
			{
				ret = 1;
				i2c_Err_code = 1;
				return 1;
			}

			result |= ((uint32_t)data << ((i-1) * 8));
		}

		return result;
	}
}

uint16_t twi_read16_rs(unsigned char addr, unsigned char reg)
{
	unsigned char ret;
	unsigned char data;
	uint16_t result = 0;

	i2c_Err_code = 0;
	Wire.beginTransmission(Addr8to7(addr));		 // set device address and write mode
	Wire.write(reg);						     // write the command
	ret = Wire.endTransmission();				 // stop transmitting

	if (ret) {
		/* failed to issue start condition, possibly no device found */
		i2c_Err_code = 1;
		return 1;
	}
	else
	{
		if (Wire.requestFrom(Addr8to7(addr), 2) != 2)
		{
			ret = 1;
			i2c_Err_code = 1;
			return 1;
		}

		for (uint8_t i = 2; i > 0; i--)
		{
			if (Wire.available())
				data = Wire.read();
			else
			{
				ret = 1;
				i2c_Err_code = 1;
				return 1;
			}

			result |= ((uint16_t)data << ((i - 1) * 8));
		}

		return result;
	}
}

uint8_t checkDevice(uint8_t addr)
{
	Wire.beginTransmission(Addr8to7(addr));		 // set device address and write mode
	if (Wire.endTransmission())					 // check device available
		return 0;
	return 1;
}