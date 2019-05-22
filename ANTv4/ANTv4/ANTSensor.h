#pragma once
#include <string>
#include "CurrentTime.h"
using namespace std;

//Base Class For ANTSensors

class ANTSensor {

public:

	ANTSensor();
	~ANTSensor();
	void setDeviceNumber(int deviceNumber); //setting the uniqque device number of ANTSensor
	const int getDeviceNumber();			//Getting the unique device number of ANTSensor
	
	void setDeviceType(int deviceType);		//Setting which ANTSensor (ex. 123 for Cadence, 120 for Heartrate)
	const int getDeviceType();				//Getting the type of ANTSensor (123 for Cadence, 120 for Heartrate)

	void setChannelPeriod(int channelPeriod);
	const int getChannelPeriod();

	void setChannelNumber(int channelNumber);
	const int getChannelNumber();

	const float getValue();
	void setValue(float value);

	void setTimeStampMeas();
	const int getTimeStampMeas();

	int getTimeSpan();
	

	const void printSensorInfo();

	void calcRPM();
	void setMeasTime(char ANTdataByte3, char ANTdataByte4); //Databyte 3 and 4 is time 
	void setRevCount(char ANTdataByte5, char ANTdataByte6);
	void setRPM(float rpm);

	const float getRPM();
	const uint16_t getRevBuf();
	
	const uint16_t getTimeBuf();
	const uint16_t getMeasTime();
	const uint16_t getRevCount();


private:

	CurrentTime *currentTime_;
	float value_ = 0;
	int timeStampMeas_ = 0;
	int timeStampNow_ = 0;
	int timeStampBuf_ = 0;
	int timeResult_ = 0;
	
	int channelNumber_ = 0;		//ChannelNumber between 0 - 7 //ANT Master cant listen to more than 8 channels
	int deviceType_ = 0;		//deviceType ex. 123 for cadenceSensor 
	int deviceNumber_ = 0;		//deviceNumber ex. 2345 (every ANT sensor has their own unique deviceNumber/ID
	int channelPeriod_ = 0;		//channelperiod ex. 8118 for cadenceSensor -> transmitting at 4HZ
	uint16_t revBuf_;	// RevCount N-1 (p. 40 ANT+ Device Profile Bicycle and Cadence)
	uint16_t revCount_;	// RevCount N (p. 40 ANT+ Device Profile Bicycle and Cadence)
	uint16_t timeBuf_;	// MeasTime N-1 (p. 40 ANT+ Device Profile Bicycle and Cadence)
	uint16_t measTime_;	// MeasTime N-1 (p. 40 ANT+ Device Profile Bicycle and Cadence)
	void setTimeBuf(uint16_t measTime);
	void setRevBuf(uint16_t revCount);
	float rpm_ = 0;

};