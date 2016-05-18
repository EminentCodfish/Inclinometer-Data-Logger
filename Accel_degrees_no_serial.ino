#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

//Set the pins
#define cardSelect 4
#define VBATPIN A9

File logfile;
const float pi = 3.141593;

//Assign time variable
unsigned long time;

char filename[15];

//Sine wave amplitude
float xAmp = 9.89;
float yAmp = 9.89;

//Initialize the angle variables
float xAng;
float yAng;

void setup(void) 
{  
  /* Initialise the sensor */
  if(!accel.begin())
  {
    while(1);
  }
  
  // see if the card is present and can be initialized:
  if (!SD.begin(cardSelect)) {

  }
  strcpy(filename, "ACCLOG00.TXT");
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = '0' + i/10;
    filename[7] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }
  
  //Initialize the LED pins as an output
  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
}

uint8_t i=0;
void loop(void) 
{ 
  //Turn on LED
  digitalWrite(8,HIGH);
  
  //Check the time
  time = millis();
  
  //Measure the battery voltage
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  
  /* Get a new sensor event */ 
  sensors_event_t event; 
  accel.getEvent(&event);
  
  //read the values from the accelerometer
  float xRead = event.acceleration.x;
  float yRead = event.acceleration.y;
  float zRead = event.acceleration.z;
  
  //Convert acceleration to degrees
  if (((xRead/xAmp) <= 1.0) && ((xRead/xAmp)>= -1.0)){
    xAng = asin(xRead/xAmp)*180/pi;
  } else {
    xAng = 90.0;
  }

  if (((yRead/yAmp) <= 1.0) && ((yRead/yAmp)>= -1.0)){
    yAng = asin(yRead/yAmp)*180/pi;
  } else {
    yAng = 90.0;
  }

  logfile = SD.open(filename, FILE_WRITE);
 
  /* Log the results (acceleration is measured in m/s^2) */
  if (logfile) {
    logfile.print("Time: "); logfile.print(time); logfile.print("  ");
    logfile.print("X: "); logfile.print(xAng); logfile.print("  ");
    logfile.print("Y: "); logfile.print(yAng); logfile.print("  ");
    logfile.print("X: "); logfile.print(event.acceleration.x); logfile.print("  ");
    logfile.print("Y: "); logfile.print(event.acceleration.y); logfile.print("  ");
    logfile.print("VBat: " ); logfile.println(measuredvbat);
    logfile.close();
  }  

  delay(10);

  //Turn off LED
  digitalWrite(8, LOW);
  delay(965);
}
