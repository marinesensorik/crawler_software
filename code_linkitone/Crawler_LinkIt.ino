/*
 Name:		Crawler_LinkIt.ino
 Created:	11/17/2017 10:30:52 AM
 Author:	Jonas Mueller
*/

#include <Arduino.h>

// COM Baud
#define baudRate 96000
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
unsigned long timer = 0;   
unsigned long timeStamp = 0;
int timeOut = 2000;           // ms

void setup()                 
{
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
				zustand = 1;
				Serial.println("COM Error");
				onError();
			}
		break;

		default:
		break;
		}

		Serial.print("SerialFlag: "); 
		Serial.println(serialFlag);  
	} 

	// Motor Control
	driveCrawler(movement, velocity);

	// Timeout
	timeStamp = millis(); 
	if ((timeStamp - timer) > (unsigned long)timeOut) 
	{
		timer = timeStamp;
		Serial.println("Timeout");
		onError();
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
			if (serialFlag == 1)
			{
				for (int x = 0; x <= Velocity; x++)
				{
					setMotors(directionfwd, directionfwd, x, x);
					delay(10); 
				}
			}
			setMotors(directionfwd, directionfwd, Velocity, Velocity);
		break;

		case backward:
			if (serialFlag == 1)
			{
				for (int x = 0; x <= Velocity; x++)
				{
					setMotors(directionbwd, directionbwd, x, x);
					delay(10); 
				}
			}
			setMotors(directionbwd, directionbwd, Velocity, Velocity);			
		break;

		case left:
			if (serialFlag == 1)
			{
				for (int x = 0; x <= Velocity; x++)
				{
					setMotors(directionfwd, directionbwd, x, x);
					delay(10); 
				}
			}
			setMotors(directionfwd, directionbwd, Velocity, Velocity);
		break;

		case right:
			if (serialFlag == 1)
			{
				for (int x = 0; x <= Velocity; x++)
				{
					setMotors(directionbwd, directionfwd, x, x);
					delay(10); 
				}
			}
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
