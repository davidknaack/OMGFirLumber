#include <Arduino.h>
#include <LaneIndicators.h>

namespace LaneIndicators
{
	uint8_t _laneAPin;
	uint8_t _laneBPin;

	/****************************************************************/
	void setAll(uint8_t state)
	{
		digitalWrite( _laneAPin, state);
		digitalWrite( _laneBPin, state);
	}
	
	/****************************************************************/
	void init(uint8_t laneAPin, uint8_t laneBPin)
	{
		_laneAPin = laneAPin;
		_laneBPin = laneBPin;
		pinMode( _laneAPin, OUTPUT );
		pinMode( _laneBPin, OUTPUT );
		setAll(0);
	}
	
	/****************************************************************/
	void setCompare(int compare)
	{
		if( compare < 0)
		{
			digitalWrite( _laneAPin, HIGH);
			digitalWrite( _laneBPin, LOW);
		}
		if( compare > 0)
		{		
			digitalWrite( _laneAPin, LOW);
			digitalWrite( _laneBPin, HIGH);
		}
		if( compare == 0)
		{
			for(int i = 0; i<8; i++)
			{
				setAll(i%2);
				delay(100);
			}
		}
	}
}
