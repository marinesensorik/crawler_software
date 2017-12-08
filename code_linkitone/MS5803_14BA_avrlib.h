/*
 * MS5803_14BA_avrlib.h
 *
 * Created: 21.12.2014 22:44:30
 *  Author: Jonas Mueller DF9FW
 * 
 * should work with all MS5803 Sensors !
 *
 * !!! CONVERTED TO ARDUINO 26.10.2017 JM !!!
 *
 */ 

#ifndef MS5803_14BA_AVRLIB_H_
#define MS5803_14BA_AVRLIB_H_

#define MS5803_ADDR			0xEC

// Commands
#define MS5803_CMD_RESET	0x1E
#define MS5803_CMD_D1_256	0x40
#define MS5803_CMD_D1_512	0x42	
#define MS5803_CMD_D1_1024	0x44
#define MS5803_CMD_D1_2048	0x46
#define MS5803_CMD_D1_4096	0x48
#define MS5803_CMD_D2_256	0x50
#define MS5803_CMD_D2_512	0x52
#define MS5803_CMD_D2_1024	0x54
#define MS5803_CMD_D2_2048	0x56
#define MS5803_CMD_D2_4096	0x58
#define MS5803_CMD_ADC		0x00

// PROM Coefficients
#define MS5803_CMD_SENS		0xA2
#define MS5803_CMD_OFF		0xA4
#define MS5803_CMD_TCS		0xA6
#define MS5803_CMD_TCO		0xA8
#define MS5803_CMD_T		0xAA
#define MS5803_CMD_TEMPS	0xAC
#define MS5803_CMD_CRC		0xAE

// Conversion Times in ms
#define MS5803_CTIME_256	1
#define MS5803_CTIME_512	2
#define MS5803_CTIME_1024	3
#define MS5803_CTIME_2048	5
#define MS5803_CTIME_4096	10

#endif /* MS5803_14BA_AVRLIB_H_ */

// Prototypes

void MS5803_RESET();
// Perform Sensor Reset

// Read Coefficients
uint16_t MS5803_read_SENS();
uint16_t MS5803_read_OFF();
uint16_t MS5803_read_TCS();
uint16_t MS5803_read_TCO();
uint16_t MS5803_read_T();
uint16_t MS5803_read_TEMPS();
uint16_t MS5803_read_CRC();

void wait_ms(uint8_t delayms);
// Simple Delay Function

uint32_t MS5803_read_digital(unsigned char cmdreg, uint8_t delayms);
// Read Digital Values, Example: uint32_t test = MS5803_read_digital(MS5803_CMD_D1_256, MS5803_CTIME_256);

double MS5803_calculate_Temp(uint16_t T, uint16_t TEMPSENS, unsigned char cmdreg, unsigned char delay);
// Calculate Temp in °C

int32_t MS5803_calculate_Pressure(uint16_t SENS, uint16_t OFF, uint16_t T, uint16_t TCO, uint16_t TCS, unsigned char cmdreg,unsigned char cmdreg2, unsigned char delay, unsigned delay2);
// Calculate Pressure in mBar