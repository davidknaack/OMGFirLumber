#include <MsTimer2.h>
#include <Servo.h>
#include <EEPROM.h>
#include "EventFuse.h"
#include "Gate.h"
#include "LaneIndicators.h"

// D2: control blue LED1
// D3: Control the green LED2
// D4: Control the red LED3
// D5: Control the red LED4
// D6: control the buzzer;
// D7: Connect TM1636 clock pin SCLK;
// D8: Connect TM1636 data pins DIO;
// D9: control keys K1;
// D10: control keys K2;
// D11: control buttons K3;

#define GATE_SERVO_PIN 2
#define GO_BUTTON_PIN 9
#define SPEED_SENSE_1 10
#define SPEED_SENSE_2 10
#define FINISH_SENSE_A 10
#define FINISH_SENSE_B 11
#define LANEA_LIGHT_PIN 3
#define LANEB_LIGHT_PIN 4

//#define GATE_SERVO_PIN 2
//#define GO_BUTTON_PIN 3
//#define SPEED_SENSE_1 4
//#define SPEED_SENSE_2 5
//#define FINISH_SENSE_A 6
//#define FINISH_SENSE_B 7
//#define LANEA_LIGHT_PIN 8
//#define LANEB_LIGHT_PIN 9
//#define RF_TRANSMIT 10
//#define RF_RECEIVE 11

#define MAX_RACE_TIME 50
#define NOVALUE 0xFFFF

#define rsReady 0
#define rsRacing 1
#define rsTimeout 2
#define rsDone 3

unsigned long startTime;
word laneATime;
word laneBTime;
word speedSenseA;
word speedSenseB;
boolean speedDone;
byte curState;
FuseID timeout;

#define DEBUG

void log(char* s)
{
#ifdef DEBUG
	Serial.print(s);
#endif
}

void log(char* s, unsigned long l)
{
#ifdef DEBUG
	Serial.print(s);
	Serial.print(l);
	Serial.println();
#endif
}

void setup()
{
	Serial.begin(115200);
	
	pinMode(GO_BUTTON_PIN, INPUT);
	pinMode(FINISH_SENSE_A, INPUT);
	pinMode(FINISH_SENSE_B, INPUT);
	pinMode(SPEED_SENSE_1, INPUT);
	pinMode(SPEED_SENSE_2, INPUT);
	pinMode(LANEA_LIGHT_PIN, OUTPUT);
	pinMode(LANEB_LIGHT_PIN, OUTPUT);
	
	Gate::init( GATE_SERVO_PIN, 10);
	LaneIndicators::init( LANEA_LIGHT_PIN, LANEB_LIGHT_PIN );

	MsTimer2::set(100, EventFuse::burn);
	MsTimer2::start();
	
	setState(rsReady);
}

void fuseStateChange(FuseID fuse, int& newState)
{
	setState(newState);
}

void fuseInvertLaneIndicators(FuseID fuse, int& state)
{
	LaneIndicators::setCompare(state*=-1);
}

void checkTransition(boolean invariant, byte newState)
{
	if (!invariant)
	{
		Serial.print("invalid transition:");
		Serial.print(curState);
		Serial.print(" ");
		Serial.println(newState);
	}
}

void setState(byte newState)
{
	log("setState: ");
	
	switch (newState)
	{
		case rsReady:
			log("rsReady\n");
			checkTransition( (curState == rsReady) || (curState==rsTimeout) || (curState==rsDone), newState);
			Gate::close();
			LaneIndicators::setAll(LOW);
			laneATime =	laneBTime =	speedSenseA = speedSenseB = NOVALUE;
			speedDone = false;
			break;
			
		case rsRacing:
			log("rsRacing\n");
			checkTransition( curState == rsReady, newState);
			Gate::open();
			startTime = millis();
			timeout = EventFuse::newFuse( rsTimeout, MAX_RACE_TIME, 1, fuseStateChange );
			break;
			
		case rsTimeout:
			log("rsTimeout\n");
			checkTransition( curState == rsRacing, newState);
			EventFuse::newFuse( 1, 2, 7, fuseInvertLaneIndicators );
			EventFuse::newFuse( rsReady, 20, 1, fuseStateChange );
			printTimes(laneATime, laneBTime);
			break;
			
		case rsDone:
			log("rsDone\n");
			checkTransition( curState==rsRacing, newState );
			EventFuse::fuses[timeout].cancel();
			LaneIndicators::setCompare( laneATime-laneBTime );
			EventFuse::newFuse( rsReady, 20, 1, fuseStateChange );	
			printTimes(laneATime, laneBTime);
			break;			
	}
	curState = newState;
}

void checkAndUpdate(word &val, uint8_t pin)
{
	if((val==NOVALUE)&&(!digitalRead(pin)))
		val = millis() - startTime;
}

void printTimes(unsigned int a, unsigned int b)
{
	Serial.print("A:");
	Serial.println(a);
	Serial.print("B:");
	Serial.println(b);
}

void printSpeed(unsigned int s)
{
	Serial.print("S:");
	Serial.print(s);
}

void loop()
{
	switch (curState)
	{
		case rsReady:
			if (!digitalRead(GO_BUTTON_PIN))
				setState(rsRacing);
			break;
			
		case rsRacing:
			checkAndUpdate(laneATime, FINISH_SENSE_A); 
			checkAndUpdate(laneBTime, FINISH_SENSE_B); 
			checkAndUpdate(speedSenseA, SPEED_SENSE_1); 
			checkAndUpdate(speedSenseB, SPEED_SENSE_2); 

			if (!speedDone&&(speedSenseA<NOVALUE)&&(speedSenseB<NOVALUE))
			{
				speedDone = true;
				printSpeed(speedSenseB-speedSenseA);
			}

			if (((laneATime<NOVALUE)&&(laneBTime<NOVALUE)))
				setState(rsDone);
			break;
			
		case rsTimeout:
		case rsDone:
			break;
	}
}
