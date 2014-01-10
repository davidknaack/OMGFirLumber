#include <MsTimer2.h>
#include <Servo.h>
#include <EEPROM.h>
#include "EventFuse.h"
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
#define GO_BUTTON_PIN 8
#define SPEED_SENSE_1 9
#define SPEED_SENSE_2 10
#define FINISH_SENSE_A 11
#define FINISH_SENSE_B 12
#define LANEA_LIGHT_PIN 6
#define LANEB_LIGHT_PIN 7

//#define GATE_SERVO_PIN 2
//#define GO_BUTTON_PIN 9
//#define SPEED_SENSE_1 10
//#define SPEED_SENSE_2 10
//#define FINISH_SENSE_A 10
//#define FINISH_SENSE_B 11
//#define LANEA_LIGHT_PIN 3
//#define LANEB_LIGHT_PIN 4

#define MAX_RACE_TIME 50
#define NOVALUE 0xFFFF

int GATE_OPEN = 1100;
int GATE_CLOSE = 1900;

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
Servo gate;

#undef DEBUG

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
	
	pinMode(GO_BUTTON_PIN, INPUT_PULLUP);
	pinMode(FINISH_SENSE_A, INPUT_PULLUP);
	pinMode(FINISH_SENSE_B, INPUT_PULLUP);
	pinMode(SPEED_SENSE_1, INPUT_PULLUP);
	pinMode(SPEED_SENSE_2, INPUT_PULLUP);
	pinMode(LANEA_LIGHT_PIN, OUTPUT);
	pinMode(LANEB_LIGHT_PIN, OUTPUT);
	
	digitalWrite(LANEA_LIGHT_PIN,1);
	delay(500);
	digitalWrite(LANEA_LIGHT_PIN,0);
		
        gate.attach(GATE_SERVO_PIN, GATE_OPEN, GATE_CLOSE);

        changeGate(0, GATE_OPEN);
        delay(1000);
        changeGate(0, GATE_CLOSE);

	digitalWrite(LANEB_LIGHT_PIN,1);
	delay(500);
	digitalWrite(LANEB_LIGHT_PIN,0);

        LaneIndicators::init( LANEA_LIGHT_PIN, LANEB_LIGHT_PIN );

	MsTimer2::set(100, EventFuse::burn);
	MsTimer2::start();
	
	setState(rsReady);
}

void changeGate( FuseID fuse, int& userVal)
{
        gate.writeMicroseconds(userVal);
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
			changeGate(0, GATE_CLOSE);
			LaneIndicators::setAll(LOW);
			laneATime =	laneBTime =	speedSenseA = speedSenseB = NOVALUE;
			speedDone = false;
			break;
			
		case rsRacing:
			log("rsRacing\n");
			checkTransition( curState == rsReady, newState);
			changeGate(0, GATE_OPEN);
                        printStart();
                        EventFuse::newFuse( GATE_CLOSE, 20, 1, changeGate ); 
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

void printStart()
{
  Serial.println("s");
}

void printTimes(unsigned int a, unsigned int b)
{
	Serial.print("a:");
	Serial.println(a);
	Serial.print("b:");
	Serial.println(b);
        Serial.println("e");
}

void printSpeed(unsigned int s)
{
	Serial.print("t:");
	Serial.println(s);
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
