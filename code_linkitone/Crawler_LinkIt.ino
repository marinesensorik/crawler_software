/*
 Name:		Crawler_LinkIt.ino
 Created:	11/17/2017 10:30:52 AM
 Author:	Jonas Mueller
*/

#define EXTERN

#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include "Crawler_LinkIt.h"
#include "I2C_CMPS11.h"
#include "MS5803_14BA_avrlib.h"
#include "i2c_functions.h"

// PWM Signal Parameter
int cycle = 254;                              // Anzahl der PWM Schritte
int sourceClock = PWM_SOURCE_CLOCK_13MHZ;     // PWM Grundfrequenz Quelle
int divider = PWM_CLOCK_DIV2;                 // PWM Frequenzteiler => 32KHz / 2 = 16 KHz = PWM Frequenz
	
// Global Variables
byte incomingByte = 0;
byte movement = 0;
byte velocity = 0;
byte checkSumme = 0;
byte checkSummeErrechnet;
int serialFlag = 0;
int zustand = 1;

// Communication Timeout 
uint32_t timer = 0;   
uint32_t timeStamp = 0;
uint32_t timeOut = 2000;           // ms

// Depth Sensor Coeff
uint16_t depth_SENS = 0;
uint16_t depth_OFF = 0;
uint16_t depth_TCO = 0;
uint16_t depth_TCS = 0;
uint16_t depth_TEMPS = 0;
uint16_t depth_T = 0;

void setup()                 
{
	Reg_Sensors = 0;
	voltage=0;
	current=0;
	pressure=0;
	watertemperature=0;
	pitch=0;
	roll=0;
	bearing=0;

	pinMode(dirR, OUTPUT);
	pinMode(dirL, OUTPUT);
	pinMode(pwmlR, OUTPUT);
	pinMode(pwmlL, OUTPUT);
	pinMode(pwmhR, OUTPUT);
	pinMode(pwmhL, OUTPUT);

	movement = stopp;
	velocity = 0;
	driveCrawler(movement, velocity);

	Serial.begin(baudRate);     // COM CDC
	Serial1.begin(baudRate1);   // COM BeagleBone

	Wire.begin();

	checkSensors();

	delay(250);

	if (Reg_Sensors & REG_SENSORS_MS5803)
		initDepth();
}

void loop()
{
	serialFlag = 0;
	zustand = 1;

	// COM Handler Serial1
	while (Serial1.available() > 0 && serialFlag == 0) 
	{
		incomingByte = Serial1.read();

		Serial.print("Byte received: ");	
		Serial.println(incomingByte, DEC);
		
		switch (zustand)                               
		{

		case 1:
			if (incomingByte == startByte)
			{
				zustand = zustand + 1;
				Serial.println("StartByte");
			}
		break;

		case 2:
			movement = incomingByte;
			Serial.print("Movement: ");
			Serial.println(movement);
			zustand = zustand + 1;
		break;

		case 3:
			velocity = incomingByte;
			Serial.print("Velocity: ");
			Serial.println(velocity);
			zustand = zustand + 1;
		break;

		case 4:
			checkSumme = incomingByte;
			Serial.print("Checksum: ");
			Serial.println(checkSumme);

			checkSummeErrechnet = movement^velocity;

			if (checkSumme == checkSummeErrechnet)
			{
				serialFlag = 1;
				Serial.println("Success");
				timer = millis();
			}
			else
			{
				// COM Error Handling
				Serial.println("COM Error");
				onError();
			}

			zustand = 1;
		break;

		default:
			zustand = 1;
		break;
		}

		Serial.print("SerialFlag: "); 
		Serial.println(serialFlag);  
	} 

	if (serialFlag == 1)
	{
		// Motor Control
		driveCrawler(movement, velocity);

		serialFlag = 0;
	}

	// Timeout
	timeStamp = millis();
	if ((timeStamp - timer) > timeOut)
	{
		timer = timeStamp;
		Serial.println("Timeout");
		onError();
				
					// DEBUG
					//getSensorData();
	}
}

void onError()
{
	Serial.println("System Error detected");
	movement = stopp;
	velocity = 0;
	driveCrawler(movement, velocity);
}

void driveCrawler(byte Movement, int Velocity)
{
	switch (Movement)
	{
		case stopp:
			setMotors(directionbwd, directionbwd, 0, 0);
		break;

		case forward:
			setMotors(directionfwd, directionfwd, Velocity, Velocity);
		break;

		case backward:
			setMotors(directionbwd, directionbwd, Velocity, Velocity);			
		break;

		case left:
			setMotors(directionfwd, directionbwd, Velocity, Velocity);
		break;

		case right:
			setMotors(directionbwd, directionfwd, Velocity, Velocity);
		break;

		default:
			setMotors(directionbwd, directionbwd, 0, 0);
		break;
	}
}

void setMotors(int DirR, int DirL, int PWMHR, int PWMHL)
{
	digitalWrite(dirR, DirR);
	digitalWrite(dirL, DirL);
	digitalWrite(pwmlL, HIGH);
	digitalWrite(pwmlR, HIGH);
	analogWriteAdvance(pwmhL, sourceClock, divider, cycle, PWMHL);
	analogWriteAdvance(pwmhR, sourceClock, divider, cycle, PWMHR);
}

uint16_t readBattVoltage()
{
	double vol = 0;
	uint16_t val = analogRead(batt_voltage);
	vol = val*ADC_RES_mV*4.0;
	return (uint16_t)vol;
}

uint32_t readCurrent()
{
	uint16_t val = analogRead(pin_current);
	double vol = (val*ADC_RES_mV) - (VREF_mV / 10);
	if (vol < 0.0)vol = 0.0;
	uint32_t cur = (vol * 10) / 2;
	return cur;
}

void getSensorData()
{
	if (Reg_Sensors & REG_SENSORS_CMPS11)
	{
		// Compass Unit found on TWI
		bearing = (uint16_t)(CMPS11_getBearing()*10.0);
		pitch = CMPS11_getPitch();
		roll = CMPS11_getRoll();

		Serial.print("Bearing: ");
		Serial.println(bearing, DEC);
		Serial.print("Pitch: ");
		Serial.println(pitch, DEC);
		Serial.print("Roll: ");
		Serial.println(roll, DEC);
	}

	if (Reg_Sensors & REG_SENSORS_MS5803)
	{
		// Pressure Unit found on TWI
		watertemperature = (int32_t)(MS5803_calculate_Temp(depth_T, depth_TEMPS, MS5803_CMD_D2_256, MS5803_CTIME_256)*1000.0);
		pressure = MS5803_calculate_Pressure(depth_SENS, depth_OFF, depth_T, depth_TCO, depth_TCS, MS5803_CMD_D1_256, MS5803_CMD_D2_256, MS5803_CTIME_256, MS5803_CTIME_256);

		Serial.print("WaterTemp: ");
		Serial.println(watertemperature, DEC);
		Serial.print("Pressure: ");
		Serial.println(pressure, DEC);
	}

	// Get Current
	current = readCurrent();
	Serial.print("Current: ");
	Serial.println(current, DEC);

	// Get Voltage
	voltage = readBattVoltage();
	Serial.print("Voltage: ");
	Serial.println(voltage, DEC);
}

void transmitSensorData()
{
	uint8_t txbuf[32];

	txbuf[0] = startByte;
	
	// voltage 
	txbuf[1] = voltage >> 8;
	txbuf[2] = voltage & 0xFF;

	// current
	txbuf[3] = current >> 8;
	txbuf[4] = current & 0xFF;

	// pressure
	txbuf[5] = pressure >> 8;
	txbuf[6] = pressure & 0xFF;

	// watertemp
	txbuf[7] = (int16_t)(watertemperature / 10)>>8;
	txbuf[8] = (int16_t)(watertemperature / 10) & 0xFF;

	// bearing
	txbuf[9] = bearing >> 8;
	txbuf[10] = bearing & 0xFF;

	// pitch
	txbuf[11] = pitch;

	// roll
	txbuf[12] = roll;

	// sensors found
	txbuf[13] = Reg_Sensors;

	// CRC
	txbuf[14] = txbuf[1] ^ txbuf[2] ^ txbuf[3] ^ txbuf[4] ^ txbuf[5] ^ txbuf[6] ^ txbuf[7] ^ txbuf[8] ^ txbuf[9] ^ txbuf[10] ^ txbuf[11] ^ txbuf[12] ^ txbuf[13];

	Serial1.write(txbuf, 15);
}

void checkSensors()
{
	// Check IMU available
	CMPS11_getPitch();
	if (twi_Error_code())
		Reg_Sensors &= ~REG_SENSORS_CMPS11;
	else
		Reg_Sensors |= REG_SENSORS_CMPS11;

	// Check Pressure available
	MS5803_RESET();
	if (twi_Error_code())
		Reg_Sensors &= ~REG_SENSORS_MS5803;
	else
		Reg_Sensors |= REG_SENSORS_MS5803;
}

void initDepth()
{
	depth_SENS = MS5803_read_SENS();
	delay(1);
	depth_OFF = MS5803_read_OFF();
	delay(1);
	depth_TCS = MS5803_read_TCS();
	delay(1);
	depth_TCO = MS5803_read_TCO();
	delay(1);
	depth_TEMPS = MS5803_read_TEMPS();
	delay(1);
	depth_T = MS5803_read_T();
}