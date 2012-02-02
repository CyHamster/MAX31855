/*******************************************************************************
* MAX31855 Library
* Version: 1.01
* Date: 1-2-2012
* Contributor: Mark Griffiths
* Company: None
* Website: justforrealmen.com
*
* This is a MAX31855 library for Arduino. It is based on a library from
* Rocket Scream Electronics (www.rocketscream.com). 
* 
* Adjustments made include:
* Correct handling of negative temperatures
* Passing of chip 'thermocouple type' to library for use in calculating thermocouple voltages and 'adjusted' temperatures
* Use of thermocouple coefficients for more accurate calculation of actual temperature (only N type currently)
* Returning of the actual bit string from chip for debugging purposes
* Single capture and use of data from chip rather than seperate for thermocouple reading and CJ reading
*
*
* Left to do:
* Include coefficients for other thermocouple types (only use N and R personally so all the rest are up to you! ;) )
* 
*
* This library is licensed under Creative Commons Attribution-ShareAlike 3.0 
* Unported License.
*
* Revision  Description
* ========  ===========
* 1.00      Initial public release.
*
*******************************************************************************/
#include	"MAX31855.h"


/******************************************************************************
*	MAX31855
*
* create an instance of the library passing in the pins used and chip thermocouple type
*
*/

MAX31855::MAX31855(byte SO, byte SCK, byte CS, int tcType)
  {
    _sdo = SO;
    _sck = SCK;
    _cs = CS;
    _tc_type = tcType;
    

    // set correct pin Modes and initial states
    pinMode(_sdo, INPUT);
    pinMode(_sck, OUTPUT);
    digitalWrite(_sck, LOW);
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
  }
  

/****************************************************************************
*	readData
* 
* read in the data from the chip and store it internally for use in subsequent temperature readings
*
* returns bool: 'true' if the bit data indicates no 'fault', otherwise returns 'false'
*
*/

bool MAX31855::readData()
{
	int bitCount;
	bool dataValid;
	
	digitalWrite(_sck, LOW);
	digitalWrite(_cs, LOW);
        bitData = 0;
        delayMicroseconds(1);
	
	// Shift in 32-bit of data
	for (bitCount = 31; bitCount >= 0; bitCount--)
	{
		digitalWrite(_sck, HIGH);
		
		// If data bit is high
		if (digitalRead(_sdo))
		{
			bitData |= ((long)1 << bitCount);
		}	
		
		digitalWrite(_sck, LOW);
	}
	
	// Deselect MAX31855 chip
	digitalWrite(_cs, HIGH);
        if(bitData & 0x00010000 == 0x00010000)
        {
	  // set dataValid to 'false' if the 'FAULT' bit (D16) is set
	dataValid = false;
        }
        else
	{
	// set dataValid to 'true' if no 'FAULT' condition present
        dataValid = true;
	}
	return dataValid;
}


/****************************************************************************
*	faultCondition
*
* used to check what faults the chip has triggered 
*
* returns string: list of fault conditions triggered
*
*/

String MAX31855::faultCondition()
{

String faults = "";

	if(bitData & 0x0001)
	{
	faults = "Open Circuit ";
	}
	if(bitData & 0x0002)
	{
	faults += "Short to GND ";
	}
	if(bitData & 0x0004)
	{
	faults += "Short to Vcc";
	}
return faults;
}

/****************************************************************************
*	temperature
*
* calculates the temperature from the bit data and converts it to the 'unit' requested
*
* is passed enum: CELSIUS, FAHRENHEIT, VOLTAGE, ADJUSTEDCELSIUS
* indicating the requested temperature unit
*
* returns float: temperature in the requested unit
*
*/

float MAX31855::temperature(scale unit)
{
  long data;
  float temperature;
  
  data = bitData >> 18;
  
  
  switch(unit)
  {
    case CELSIUS:
      temperature = data * 0.25;
      break;
    case FAHRENHEIT:
      temperature = data * 0.45 + 32;
      break;
    case VOLTAGE:
      //for N types only at this time
      temperature = data * 9.064;
      break;
    case ADJUSTEDCELSIUS:
	switch(_tc_type)
		{
		case type_N:
			temperature = adjustTemp(data);
			break;
		}

  }
  return temperature;
}


/****************************************************************************
*	temperatureCJC
*
* calculates the cold junction temperature from the bit data and converts it to the 'unit' requested
*
* is passed enum: CELSIUS, FAHRENHEIT
*
* returns float: temperature in the requested unit
*
*/

float MAX31855::temperatureCJC(scale unit)
{
  int data;
  float temperature;
  
  data = bitData;
  data = data >> 4;
  
  
  switch(unit)
  {
    case CELSIUS:
      temperature = data * 0.0625;
      break;
    case FAHRENHEIT:
      temperature = data * 0.1125 + 32;
      break;
  }
  return temperature;
}


/****************************************************************************
*	adjustTemp	internal only function
*
* adjusts the measured temperature using the ITS90 coefficients
*
* is passed long: the bit data for the thermocouple
*
* returns float: the compensated temperature
*
*/

float MAX31855::adjustTemp(long inTemp)
{
float CJCtemp;
float CJCvoltage;
float adjustedtemp;
float adjustedvoltage;

 CJCtemp = temperatureCJC(CELSIUS);
 CJCvoltage = CJCtemp * 2.5929394601e01;
 CJCvoltage = CJCvoltage + pow(CJCtemp,2) * 1.5710141880e-02;
 CJCvoltage = CJCvoltage + pow(CJCtemp,3) * 4.3825627237e-05;
 CJCvoltage = CJCvoltage + pow(CJCtemp,4) * -2.5261169794e-07;
 CJCvoltage = CJCvoltage + pow(CJCtemp,5) * 6.4311819339e-10;
 CJCvoltage = CJCvoltage + pow(CJCtemp,6) * -1.0063471519e-12;
 CJCvoltage = CJCvoltage + pow(CJCtemp,7) * 9.9745338992e-16;
 CJCvoltage = CJCvoltage + pow(CJCtemp,8) * -6.0863245607e-19;
 CJCvoltage = CJCvoltage + pow(CJCtemp,9) * 2.0849229339e-22;
 CJCvoltage = CJCvoltage + pow(CJCtemp,10) * -3.0682196151e-26;

 adjustedvoltage = inTemp * 9.064;
 adjustedvoltage -= CJCvoltage;

 adjustedtemp = adjustedvoltage * 3.8783277e-02;
 adjustedtemp = adjustedtemp + pow(adjustedvoltage,2) * -1.1612344e-06;
 adjustedtemp = adjustedtemp + pow(adjustedvoltage,3) * 6.9525655e-11;
 adjustedtemp = adjustedtemp + pow(adjustedvoltage,4) * -3.0090077e-15;
 adjustedtemp = adjustedtemp + pow(adjustedvoltage,5) * 8.8311584e-20;
 adjustedtemp = adjustedtemp + pow(adjustedvoltage,6) * -1.6213839e-24;
 adjustedtemp = adjustedtemp + pow(adjustedvoltage,7) * 1.6693362e-29;
 adjustedtemp = adjustedtemp + pow(adjustedvoltage,8) * -7.3117540e-35;
 adjustedtemp += CJCtemp;

 return adjustedtemp;
}


/****************************************************************************
*	bitString
*
* used for debugging purposes, comparing the temperatures outputted by the library against the bit string 'BY HAND!'
*
* returns long: bit data from chip
*
*
*/

long MAX31855::bitString()
{
return bitData;
}