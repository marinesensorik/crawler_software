/*
 * i2c_functions.h
 *
 * Standart I2C Funtions
 * 
 * Created: 31.10.2014 22:40:45
 * Author: Jonas Müller DF9FW
 */ 


#ifndef I2C_FUNCTIONS_H_
#define I2C_FUNCTIONS_H_
#endif /* I2C_FUNCTIONS_H_ */

#define I2C_WRITE			 0
#define I2C_READ			 1
#define Addr8to7(addr) addr>>1  

//Functions:

unsigned char twi_Error_code();
//Returns Error Code: 1->Error, 0->Ok

unsigned char twi_readregister(unsigned char addr, unsigned char reg);
//Returns register-value off i2c-Slave-Register
//addr is Adress of Slave
//reg is requested Register
//in case of an Error the function returns 1 and sets i2c_Err_code to 1

unsigned char twi_writeregister(unsigned char addr, unsigned char reg, unsigned char value);
//Sets register-value off i2c-Slave-Register
//addr is Adress of Slave
//reg is requested Register
//value is the 8-bit value to write
//return 1 in case of an Error, else 0

void twi_sendCommand(unsigned char addr, unsigned char command);
//Sends Command Byte to I2C Device

uint32_t twi_read32_rs(unsigned char addr, unsigned char reg);
//reads a 32 bit value with repeated start

uint16_t twi_read16_rs(unsigned char addr, unsigned char reg);
//reads a 16 bit value with repeated start

uint8_t checkDevice(uint8_t addr);
//checks the availibility of an device at IIC Address addr
