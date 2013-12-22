#include <EEPROM.h>
#include "Gate.h"

namespace Gate
{
	Servo _servo;
	uint8_t _storageBase; // EEPROM start location for settings storage
	uint8_t _servoPin;
	uint8_t _valOpen;
	uint8_t _valClose;
	
	void servoInit();
	void loadOpenClose();
	uint8_t validateStorage();

	/****************************************************************/
	void init(uint8_t servoPin, uint8_t storageBase)
	{
		_servoPin = servoPin;
		_storageBase = storageBase;
		if (validateStorage())
			setOpenClose(1000, 2000);
		else
			loadOpenClose();
	}
	
	/****************************************************************/
	void loadOpenClose()
	{
		_valOpen = EEPROM.read(_storageBase);
		_valClose = EEPROM.read(_storageBase+1);
		servoInit();
	}
	
	/****************************************************************/
	void setOpenClose(uint8_t open, uint8_t close)
	{
		_valOpen = open;
		_valClose = close;
		EEPROM.write(_storageBase,   _valOpen);
		EEPROM.write(_storageBase+1, _valClose);
		servoInit();
	}

	/****************************************************************/
	void servoInit(){
		if (_valOpen < _valClose)
			_servo.attach(_servoPin, _valOpen, _valClose);
		else
			_servo.attach(_servoPin, _valClose, _valOpen);	
	}
	
	/****************************************************************/
	uint8_t validateStorage()
	{
		unsigned char mark[] = "TRACK";
		
		uint8_t doInit = 0;
		for(uint8_t i = 0; i < 5; i ++)
			if(doInit || (doInit = (EEPROM.read(i+_storageBase) != mark[i])))
				EEPROM.write(i+_storageBase, mark[i]);
				
		return doInit;
	}
	
	/****************************************************************/
	void open(){
		_servo.writeMicroseconds(_valOpen);
	}
	
	/****************************************************************/
	void close(){
		_servo.writeMicroseconds(_valClose);
	}
}