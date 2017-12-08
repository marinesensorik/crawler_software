#pragma once

#ifndef EXTERN
#define EXTERN extern
#endif // !EXTERN

// COM Baud
#define baudRate 9600
#define baudRate1 115200

// Pinout LinkIt One
#define tx 0
#define rx 1
#define dirR 2
#define dirL 8
#define pwmlR 4
#define pwmlL 10
#define pwmhR 3
#define pwmhL 9
#define batt_voltage A0
#define pin_current A1

// Movement Commands
#define forward  1
#define backward 2
#define left 4
#define forwardLeft 5
#define right 8
#define forwardRight 9
#define stopp 16
#define reset 255

#define directionfwd LOW
#define directionbwd HIGH

// Startbyte
#define startByte	240

// ADC
#define VREF_mV						5000.0f
#define ADC_RES_Steps				1024
#define ADC_RES_mV					(double)(VREF_mV/(ADC_RES_Steps-1))

// Sensors Avaibility Register Definition 
#define REG_SENSORS_CMPS11			(1<<0)
#define REG_SENSORS_MS5803			(1<<1)

// Externals
EXTERN uint16_t voltage;
EXTERN uint16_t current;
EXTERN int32_t pressure;
EXTERN int32_t  watertemperature;
EXTERN int pitch;
EXTERN int roll;
EXTERN uint16_t bearing;

EXTERN uint8_t Reg_Sensors;
