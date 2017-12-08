/*
 * MS5803_14BA_avrlib.cpp
 *
 * Created: 21.12.2014 22:41:04
 *  Author: Jonas Mueller DF9FW
 *
 * !!! CONVERTED TO ARDUINO 26.10.2017 JM !!!
 */ 

#include <Arduino.h>
#include "MS5803_14BA_avrlib.h"
#include "i2c_functions.h"
#include <math.h>

void MS5803_RESET()
{
	twi_sendCommand(MS5803_ADDR, MS5803_CMD_RESET);	
}

uint16_t MS5803_read_SENS()
{
	uint16_t ret = twi_read16_rs(MS5803_ADDR, MS5803_CMD_SENS);
	return ret;
}

uint16_t MS5803_read_OFF()
{
	uint16_t ret = twi_read16_rs(MS5803_ADDR, MS5803_CMD_OFF);
	return ret;
}

uint16_t MS5803_read_TCS()
{
	uint16_t ret = twi_read16_rs(MS5803_ADDR, MS5803_CMD_TCS);
	return ret;
}

uint16_t MS5803_read_TCO()
{
	uint16_t ret = twi_read16_rs(MS5803_ADDR, MS5803_CMD_TCO);
	return ret;
}

uint16_t MS5803_read_T()
{
	uint16_t ret = twi_read16_rs(MS5803_ADDR, MS5803_CMD_T);
	return ret;
}

uint16_t MS5803_read_TEMPS()
{
	uint16_t ret = twi_read16_rs(MS5803_ADDR, MS5803_CMD_TEMPS);
	return ret;
}

uint16_t MS5803_read_CRC()
{
	uint16_t ret = twi_read16_rs(MS5803_ADDR, MS5803_CMD_CRC);
	return ret;
}

void wait_ms(uint8_t delayms)
{
	for(int i=0; i<delayms; i++)
		delay(1);
}

uint32_t MS5803_read_digital(unsigned char cmdreg, uint8_t delayms)
{
	uint32_t digitalData=0;
	twi_sendCommand(MS5803_ADDR, cmdreg);
	if(twi_Error_code()) return 0;
	wait_ms(delayms);
	digitalData = twi_read32_rs(MS5803_ADDR, MS5803_CMD_ADC);	
	return digitalData;
}

double MS5803_calculate_Temp(uint16_t T, uint16_t TEMPSENS, unsigned char cmdreg, unsigned char delayms)
{
	double dT=0;
	uint32_t D2=0;
	
	D2=MS5803_read_digital(cmdreg,delayms);
	if(twi_Error_code()) return 0;
	
	dT=D2-T*pow(2,8);
	double Temp=(2000+(dT*TEMPSENS)/pow(2,23))/100;
	return Temp;
}

int32_t MS5803_calculate_Pressure(uint16_t SENS, uint16_t OFF, uint16_t T, uint16_t TCO, uint16_t TCS, unsigned char cmdreg,unsigned char cmdreg2, unsigned char delayms, unsigned delayms2)
{
	uint32_t D1 = 0;
	uint32_t D2 = 0;
	int32_t P = 0;
	int32_t dT = 0.0;
	int64_t nOFF = 0.0;
	int64_t nSENS = 0.0;
	
	D2=MS5803_read_digital(cmdreg2, delayms2);
	delay(1);
	if(twi_Error_code())return 0.0;
	D1=MS5803_read_digital(cmdreg, delayms);
	if(twi_Error_code()) return 0.0;
	
	dT=D2-T*pow(2,8);
	nOFF=OFF*pow(2,16)+(dT*TCO)/pow(2,7);
	nSENS=SENS*pow(2,15)+(dT*TCS)/pow(2,8);
	
	P=(D1*nSENS/pow(2,21)-nOFF)/pow(2,15)/10;
	return P;
}
