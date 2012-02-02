/*
  read_MAX31855.ino
 
 TODO: 
   Clean up code and comment!!  
 	Also make use of all library functions and make more robust.
 
 This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported License.
 http://creativecommons.org/licenses/by-sa/3.0/
 */

#include <MAX31855.h>

// Arduino MEGA ports

int SCLK = 51;
int SDI = 53;
int CS = 50;

float tempTC, tempCJC, realTemp;
bool dataValid;
long bitData;
String errorCodes = "";

MAX31855 thermo1(SDI, SCLK, CS, type_N);

void setup() {
  Serial.begin(57600);
}

void loop() {
  int cntr1;
  tempTC=0.0;
  realTemp=0.0;
  tempCJC=0.0;

  dataValid = thermo1.readData();
  if(dataValid==false)
  {
    errorCodes =  thermo1.faultCondition();
    Serial.print("Thermocouple error found: ");
    Serial.println(errorCodes);
  }
  else
  {
    tempCJC = tempCJC + thermo1.temperatureCJC(CELSIUS);
    tempTC = tempTC + thermo1.temperature(CELSIUS);
    realTemp = realTemp + thermo1.temperature(ADJUSTEDCELSIUS);
    Serial.print(realTemp);
    Serial.print("\t");
    Serial.print(tempTC);
    Serial.print("\t");
    Serial.println(tempCJC);
  }


  delay(2000);
}

