#include "ANTSensor.h"
#pragma once

#define CHANNEL_PERIOD 8102
#define DEVICE_TYPE 122


class ANTCadence : public ANTSensor 
{
public:
	//ANTCadence(int deviceNumber);
	ANTCadence();
	void calcRPM();
	void setMeasTime(char ANTdataByte3, char ANTdataByte4); //Databyte 3 and 4 is time 
	void setRevCount(char ANTdataByte5, char ANTdataByte6);
	void setRPM(float rpm);
	void foo(); // this is pure virtual stuff //nothing else
	
	const float getRPPM();
	const uint16_t getRevBuf();
	const uint16_t getTimeBuf();
	const uint16_t getMeasTime();
	const uint16_t getRevCount();
	float getData();
	
private:

	void setTimeBuf(uint16_t measTime);
	void setRevBuf(uint16_t revCount);

	uint16_t revBuf_ = 0;	// RevCount N-1 (p. 40 ANT+ Device Profile Bicycle and Cadence)
	uint16_t revCount_ = 0;	// RevCount N (p. 40 ANT+ Device Profile Bicycle and Cadence)
	uint16_t timeBuf_ = 0;	// MeasTime N-1 (p. 40 ANT+ Device Profile Bicycle and Cadence)
	uint16_t measTime_ = 0;	// MeasTime N-1 (p. 40 ANT+ Device Profile Bicycle and Cadence)
	float rpm_ = 0;			//RPM calculated from calcRPM
};