#ifndef	MAX31855_H
#define MAX31855_H

#if	ARDUINO >= 100
	#include "Arduino.h"
#else  
	#include "WProgram.h"
#endif


#define	FAULT_SHORT_GND	10001
#define	FAULT_SHORT_VCC	10002	
#define	FAULT_OPEN	10003
#define type_K 	20001
#define type_J 	20002
#define type_N 	20003
#define type_S 	20004
#define type_T 	20005
#define type_E 	20006
#define type_R 	20007

enum	scale
{
	FAHRENHEIT,
	CELSIUS,	
	VOLTAGE,
	ADJUSTEDCELSIUS
};

class	MAX31855
{
	public:
		MAX31855(byte SO, byte SCK, byte CS, int tcType);
		
		bool readData();
		float temperature(scale unit);
		float temperatureCJC(scale unit);
		String faultCondition();
		long bitString();
		
	private:
		byte _sdo;
		byte _cs;
		byte _sck;
		int _tc_type;
		long bitData;
		
		float adjustTemp(long inTemp);

};
#endif