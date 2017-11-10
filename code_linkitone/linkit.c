/************************************************ 
* Programm:  Motorsteuerung LinkIT 
* Autor:     Hauke Haake 
* Datum:     02.01.2016
 * Version:   1.2.1 
*  
* Neu: 
* Timer zur Programmüberwachung 
* Schleife zum langsamen Anfahren 
* ----------------------------------------------- 
* Kommunikation mit BeagleBone über Serial1 
* Ansteuerung der Pins für die Motorcontroller
* Ausgabe der Diagnose Funktion über Serial 
* Reset der Motoren 1 - 6 
************************************************/

//Pinbelegung des LinkIt
#define tx 0
#define rx 1
#define dirR 2
#define dirL 8
#define pwmlR 4
#define pwmlL 10
#define pwmhR 3
#define pwmhL 9
#define reset1 5
#define reset2 11
#define reset3 6
#define reset4 12
#define reset5 7
#define reset6 13

//PWM Signal Parameter
int cycle = 255;                              // Anzahl der PWM Schritte
int sourceClock = PWM_SOURCE_CLOCK_32KHZ;     // PWM Grundfrequenz Quelle
int divider = PWM_CLOCK_DIV2;                 // PWM Frequenzteiler => 32KHz / 2 = 16 KHz = PWM Frequenz

//Serielle Kommunikation
#define baudRate 9600
#define baudRate1 9600
byte incomingByte = 0;
byte startByte = 240;       // Muss vorher festgelegt werden
byte movement = 0;
byte velocity = 0;
byte checkSumme = 0;
byte checkSummeErrechnet;
int serialFlag = 0;
int zustand = 1;

//Motorsteuerung
#define forward  1
#define backward 2
#define left 4
#define forwardLeft 5
#define right 8
#define forwardRight 9
#define stopp 16
#define reset 255

int radius = 2;             // Kurvenradius in Meter
int width = 0.3;            // Breite des Fahrzeugs Meter
int velocityMax = 0;        // Berechnungsvariable für Kurvenfahrt in 8 Bit

//Motorreset
int reset1Switch = 0;
int reset2Switch = 0;
int reset3Switch = 0;
int reset4Switch = 0;
int reset5Switch = 0;
int reset6Switch = 0;

//Zeitüberwachung
unsigned long timer = 0;      // unsigned long gegen Variablen Overflow
unsigned long timeStamp = 0;
int timeOut = 2000;           // Zeit zwischen zwei Signalen

void setup()                  // Void Setup dient zur Initialisierung der Systemeinstellung. Die Schleife wird beim Systemstart durchlaufen. 
  {
  pinMode(dirR, OUTPUT);
  pinMode(dirL, OUTPUT);
  pinMode(pwmlR, OUTPUT);
  pinMode(pwmlL, OUTPUT);
  pinMode(pwmhR, OUTPUT);
  pinMode(pwmhL, OUTPUT);
  pinMode(reset1, OUTPUT);
  pinMode(reset2, OUTPUT);
  pinMode(reset3, OUTPUT);
  pinMode(reset4, OUTPUT);
  pinMode(reset5, OUTPUT);
  pinMode(reset6, OUTPUT);

  Serial.begin(baudRate);     // Serielle Kommunikation Laptop (Diagnose)
  Serial1.begin(baudRate1);   // Serielle Kommunikation BeagleBone
  }

void loop() 
  { 
  //Serielle Abfrage
  serialFlag = 0;
  zustand = 1;
  
  while(Serial1.available() > 0 && serialFlag == 0) // Die While Schleife dient der Kommunikationsabfrage.
    {
    Serial.println("Schleifenstart");               // Überwachungsausgabe
    incomingByte = Serial1.read();                  // Auslesen des übermittelten Byte
  
      switch(zustand)                               // Zuweisung der Byte in die Variablen startByte, movement, velocity, checkSumme
        {
          case 1:
            if(incomingByte == startByte)
              {
              zustand = zustand+1;
              Serial.println("StartByte erkannt");
              }
              break;      
          case 2:
              movement = incomingByte;
              Serial.print("Movement: ");
              Serial.println(movement);
              zustand = zustand+1;
              break;  
          case 3:
              velocity = incomingByte;
              Serial.print("Velocity: ");
              Serial.println(velocity);
              zustand = zustand+1; 
              break;
          case 4:
              checkSumme = incomingByte;
              Serial.print("Checksumme: ");
              Serial.println(checkSumme);
              // CheckSummeErrechnet errechnen
              checkSummeErrechnet = movement^velocity; // Checksumme über XOR Befehl
              
              if(checkSumme == checkSummeErrechnet)    // Abgleich der übermittelten Checksumme gegen der errechneten Checksumme um Datenverluste zu ermitteln
                {
                  serialFlag = 1;                      // Variable zum verlassen der While Schleife
                  Serial.println("Senden Erfolgreich");// Überwachungsausgabe
                  timer = millis();                    // Variable zur Zeitüberwachung, Zeitstempel Start
                }
              else  // Kommunikation hat einen Fehler, Leseschleife wird auf Anfang gesetzt, Crawler wird gestoppt.
                {
                  zustand = 1;
                  Serial.println("Senden Fehlgeschlagen");
                  myDriveFunction(stopp, stopp, forward, backward, left, forwardLeft, right, forwardRight, dirR, pwmlR, pwmhR, dirL, pwmlL, pwmhL, sourceClock, divider, cycle, velocityMax, radius, width, velocity);
                }
              break;
          default:
              break;
        }
    Serial.print("SerialFlag: "); //Überwachungsausgabe
    Serial.println(serialFlag);   // Überwachungsausgabe
    }
  Serial.println("Auszerhalb der Schleife"); // Steuervariablen zur Resetsteuerung der Motorcontroller (in der Hardware nicht implementiert)
  if(movement == reset) //Motor reset
    {
    switch(velocity) //Velocity = Motor Nummer
      {
      case 1:
          if (reset1Switch == 0)
            {
            digitalWrite(reset1, HIGH); // Ausschalten der Motorcontroller
            reset1Switch = 1;
            }
          else
            {
            digitalWrite(reset1, LOW);  // Einschalten der Motorcontroller
            reset1Switch = 0;
            }
          break;
      case 2:
          if (reset2Switch == 0)
            {
            digitalWrite(reset2, HIGH); 
            reset2Switch = 1;
            }
          else
            {
            digitalWrite(reset2, LOW);
            reset2Switch = 0;
            }
          break;
      case 3: 
          if (reset3Switch == 0)
            {
            digitalWrite(reset3, HIGH);
            reset3Switch = 1;
            }
          else
            {
            digitalWrite(reset3, LOW);
            reset3Switch = 0;
            }
          break;
      case 4:
          if (reset4Switch == 0)
            {
            digitalWrite(reset4, HIGH);
            reset4Switch = 1;
            }
          else
            {
            digitalWrite(reset4, LOW);
            reset4Switch = 0;
            }
          break;
      case 5: 
          if (reset5Switch == 0)
            {
            digitalWrite(reset5, HIGH);
            reset5Switch = 1;
            }
          else
            {
            digitalWrite(reset5, LOW);
            reset5Switch = 0;
            }
          break;
      case 6: 
          if (reset6Switch == 0)
            {
            digitalWrite(reset6, HIGH);
            reset6Switch = 1;
            }
          else
            {
            digitalWrite(reset6, LOW);
            reset6Switch = 0;
            }
          break;    
      }
    }
  else  // Motor Ansteuerung
    {  
    Serial.println("Motor wird angesteuert!");  // Überwachungsausgabe
    myDriveFunction(movement, stopp, forward, backward, left, forwardLeft, right, forwardRight, dirR, pwmlR, pwmhR, dirL, pwmlL, pwmhL, sourceClock, divider, cycle, velocityMax, radius, width, velocity); // Motorfunktion 
    }
    
  //Diagnose
  //myDiagnose(dirR, pwmlR, pwmhR, dirL, pwmlL, pwmhL, baudRate, baudRate1, checkSumme, checkSummeErrechnet, serialFlag, cycle, sourceClock, divider, reset1Switch, reset2Switch, reset3Switch, reset4Switch, reset5Switch, reset6Switch, timer, timeStamp, timeOut);

  //Zeitüberwachung
  timeStamp = millis(); // Zeitstempel nach Schleifendurchlauf
  if ((timeStamp-timer)>timeOut)  // Differenzermittlung und Vergleich mit maximaler Zeit
    {
    myDriveFunction(stopp, stopp, forward, backward, left, forwardLeft, right, forwardRight, dirR, pwmlR, pwmhR, dirL, pwmlL, pwmhL, sourceClock, divider, cycle, velocityMax, radius, width, velocity);  // Stopp des Crawlers  
    }
  
    
}

// Funktionen
void myDriveFunction(byte Movement, int Stopp, int Forward, int Backward, int Left, int ForwardLeft, int Right, int ForwardRight, int DirR, int PwmlR, int PwmhR, int DirL, int PwmlL, int PwmhL, int SourceClock, int Divider, int Cycle, int VelocityMax, int Radius, int Width, int Velocity)
{
   Serial.println("myDriveFunction start!");
   if(Movement == Stopp)        // Brake
    {
    Serial.print("Movement: ");
    Serial.println(Movement);
    digitalWrite(DirR, HIGH);
    digitalWrite(PwmlR, HIGH);
    analogWriteAdvance(PwmhR, SourceClock, Divider, Cycle, 0);
    digitalWrite(DirL, HIGH);
    digitalWrite(PwmlL, HIGH);
    analogWriteAdvance(PwmhL, SourceClock, Divider, Cycle, 0);
    }
  else if(Movement == Forward) // Forward
    {
    if(serialFlag == 1)
    {
      for (int x = 0; x <= Velocity; x++)
        {
        digitalWrite(DirR, LOW);
        digitalWrite(PwmlR, HIGH);
        analogWriteAdvance(PwmhR, SourceClock, Divider, Cycle, x);
        digitalWrite(DirL, LOW);
        digitalWrite(PwmlL, HIGH);
        analogWriteAdvance(PwmhL, SourceClock, Divider, Cycle, x);
        delay(10); //TEST ob ausreichend
        }
    }
    digitalWrite(DirR, LOW);
    digitalWrite(PwmlR, HIGH);
    analogWriteAdvance(PwmhR, SourceClock, Divider, Cycle, Velocity);
    digitalWrite(DirL, LOW);
    digitalWrite(PwmlL, HIGH);
    analogWriteAdvance(PwmhL, SourceClock, Divider, Cycle, Velocity);  
    }
  else if(Movement == Backward) // Backward
    {
    if(serialFlag == 1)
    {  
      for (int x = 0; x = Velocity; x++)
        {
        digitalWrite(DirR, HIGH);
        digitalWrite(PwmlR, HIGH);
        analogWriteAdvance(PwmhR, SourceClock, Divider, Cycle, x);
        digitalWrite(DirL, HIGH);
        digitalWrite(PwmlL, HIGH);
        analogWriteAdvance(PwmhL, SourceClock, Divider, Cycle, x);
        delay(10); //TEST ob ausreichend
        }
    }
    digitalWrite(DirR, HIGH);
    digitalWrite(PwmlR, HIGH);
    analogWriteAdvance(PwmhR, SourceClock, Divider, Cycle, Velocity);
    digitalWrite(DirL, HIGH);
    digitalWrite(PwmlL, HIGH);
    analogWriteAdvance(PwmhL, SourceClock, Divider, Cycle, Velocity);
    }
  else if(Movement == Left) //Left
    {                                                   
    if(serialFlag == 1)
    { 
      for (int x = 0; x <= Velocity; x++)
        {
        digitalWrite(DirR, LOW);
        digitalWrite(PwmlR, HIGH);
        analogWriteAdvance(PwmhR, SourceClock, Divider, Cycle, x);
        digitalWrite(DirL, HIGH);
        digitalWrite(PwmlL, HIGH);
        analogWriteAdvance(PwmhL, SourceClock, Divider, Cycle, x);
        }
    }
    digitalWrite(DirR, LOW);
    digitalWrite(PwmlR, HIGH);
    analogWriteAdvance(PwmhR, SourceClock, Divider, Cycle, Velocity);
    digitalWrite(DirL, HIGH);
    digitalWrite(PwmlL, HIGH);
    analogWriteAdvance(PwmhL, SourceClock, Divider, Cycle, Velocity);
    }
  else if(Movement == ForwardLeft)
    {
    if(serialFlag == 1)
    {
      for ( x = 0; x <= Velocity; x++)
        {
        x2 = ((x * Radius)/(Radius+(Width/2)));
        digitalWrite(DirR, LOW);
        digitalWrite(PwmlR, HIGH);
        analogWriteAdvance(PwmhR, SourceClock, Divider, Cycle, x);
        digitalWrite(DirL, LOW);
        digitalWrite(PwmlL, HIGH);
        analogWriteAdvance(PwmhL, SourceClock, Divider, Cycle, (((Radius-(Width/2))*x2)/Radius));
        }
    }
    Velocity2 = ((Velocity * Radius)/(Radius+(Width/2)));
    digitalWrite(DirR, LOW);
    digitalWrite(PwmlR, HIGH);
    analogWriteAdvance(PwmhR, SourceClock, Divider, Cycle, Velocity);
    digitalWrite(DirL, LOW);
    digitalWrite(PwmlL, HIGH);
    analogWriteAdvance(PwmhL, SourceClock, Divider, Cycle, (((Radius-(Width/2))*Velocity2)/Radius));    
    }
  else if(Movement == Right) //Right
    { 
    if(serialFlag == 1)
    {                                                    
      for ( x = 0; x <= Velocity; x++)
        {
        digitalWrite(DirR, HIGH);
        digitalWrite(PwmlR, HIGH);
        analogWriteAdvance(PwmhR, SourceClock, Divider, Cycle, x);
        digitalWrite(DirL, LOW);
        digitalWrite(PwmlL, HIGH);
        analogWriteAdvance(PwmhL, SourceClock, Divider, Cycle, x);
        }
    }
    digitalWrite(DirR, HIGH);
    digitalWrite(PwmlR, HIGH);
    analogWriteAdvance(PwmhR, SourceClock, Divider, Cycle, Velocity);
    digitalWrite(DirL, LOW);
    digitalWrite(PwmlL, HIGH);
    analogWriteAdvance(PwmhL, SourceClock, Divider, Cycle, Velocity);
    }
  else if(Movement == ForwardRight)
    {
    if(serialFlag == 1)
    {  
      for ( x = 0; x <= Velocity; x++)
        {
        x2 = ((x * Radius)/(Radius+(Width/2)));
        digitalWrite(DirR, LOW);
        digitalWrite(PwmlR, HIGH);
        analogWriteAdvance(PwmhR, SourceClock, Divider, Cycle, (((Radius-(Width/2))*x2)/Radius));
        digitalWrite(DirL, LOW);
        digitalWrite(PwmlL, HIGH);
        analogWriteAdvance(PwmhL, SourceClock, Divider, Cycle, x);  
        }
    }    
    Velocity2 = ((Velocity * Radius)/(Radius+(Width/2)));
    digitalWrite(DirR, LOW);
    digitalWrite(PwmlR, HIGH);
    analogWriteAdvance(PwmhR, SourceClock, Divider, Cycle, (((Radius-(Width/2))*Velocity)/Radius));
    digitalWrite(DirL, LOW);
    digitalWrite(PwmlL, HIGH);
    analogWriteAdvance(PwmhL, SourceClock, Divider, Cycle, Velocity);   
    }
  else
    {
    // Stopp
    Serial.print("Movement: ");
    Serial.println(Movement);
    digitalWrite(DirR, HIGH);
    digitalWrite(PwmlR, HIGH);
    analogWriteAdvance(PwmhR, SourceClock, Divider, Cycle, 0);
    digitalWrite(DirL, HIGH);
    digitalWrite(PwmlL, HIGH);
    analogWriteAdvance(PwmhL, SourceClock, Divider, Cycle, 0);  
    }  
    Serial.print("Velocity: ");
    Serial.println(Velocity);
    return;
}

void myDiagnose(int DirR, int PwmlR, int PwmhR, int DirL, int PwmlL, int PwmhL, int BaudRate, int BaudRate1, byte CheckSumme, byte CheckSummeErrechnet, int SerialFlag, int Cycle, int SourceClock, int Divider, int Reset1Switch, int Reset2Switch, int Reset3Switch, int Reset4Switch, int Reset5Switch, int Reset6Switch, unsigned long Timer, unsigned long TimeStamp, int TimeOut)
{
  Serial.println("Diagnose Programm");
  Serial.println("");
  Serial.println("Pinbelegung");
  Serial.print("DIR Right: ");
  Serial.println(DirR);
  Serial.print("PWML Right: ");
  Serial.println(PwmlR);
  Serial.print("PWMH Right: ");
  Serial.println(PwmhR);
  Serial.print("DIR Left: ");
  Serial.println(DirL);
  Serial.print("PWML Left: ");
  Serial.println(PwmlL);
  Serial.print("PWMH Left: ");
  Serial.println(PwmhL);
  Serial.println("");
  Serial.println("Serielle Schnittstellen");
  Serial.print("Baudrate: ");
  Serial.println(BaudRate);
  Serial.print("Baudrate1: ");
  Serial.println(BaudRate1);
  Serial.print("Übermittelte Checksumme: ");
  Serial.println(CheckSumme, BIN);
  Serial.print("Errechnete Checksumme: ");
  Serial.println(CheckSummeErrechnet, BIN);
  Serial.print("SerialFlag: ");
  Serial.println(SerialFlag);
  Serial.println("");
  Serial.println("PWM Signal");
  Serial.print("Cycle: ");
  Serial.println(Cycle);
  Serial.print("SourceClock: ");
  Serial.println(SourceClock);
  Serial.print("Divider: ");
  Serial.println(Divider);
  Serial.println("");
  Serial.println("Motor");
  Serial.print("Motor 1: ");
  if(Reset1Switch == 1){Serial.println("OFF");}
  else{Serial.println("ON");}
  Serial.print("Motor 2: ");
  if(Reset2Switch == 1){Serial.println("OFF");}
  else{Serial.println("ON");}
  Serial.print("Motor 3: ");
  if(Reset3Switch == 1){Serial.println("OFF");}
  else{Serial.println("ON");}
  Serial.print("Motor 4: ");
  if(Reset4Switch == 1){Serial.println("OFF");}
  else{Serial.println("ON");}
  Serial.print("Motor 5: ");
  if(Reset5Switch == 1){Serial.println("OFF");}
  else{Serial.println("ON");}
  Serial.print("Motor 6: ");
  if(Reset6Switch == 1){Serial.println("OFF");}
  else{Serial.println("ON");}
  Serial.println(" ");
  Serial.println("Zeitüberwachung");
  Serial.print("Timer: ");
  Serial.println(Timer);
  Serial.print("Time Stamp: ");
  Serial.println(TimeStamp);
  Serial.print("Delta Time: ");
  Serial.println((TimeStamp - Timer));
  Serial.print("Time OUT: ");
  Serial.println(TimeOut);
  Serial.println("Ende");
    
  return;
}

