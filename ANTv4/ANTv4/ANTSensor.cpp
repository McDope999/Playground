#include "ANTSensor.h"
#include <iostream>
#include <chrono>
#include <cstdint>


using namespace std;





ANTSensor::ANTSensor()
{
	channelNumber_ = 0;
	currentTime_ = new CurrentTime();
}

ANTSensor::~ANTSensor()
{
	delete currentTime_;
}

const int ANTSensor::getDeviceNumber()
{
	return deviceNumber_;
}

void ANTSensor::setDeviceType(int deviceType)
{
	deviceType = (deviceType >= 0 ? deviceType_ = deviceType : deviceType_ = 0);
}

const int ANTSensor::getDeviceType()
{
	return deviceType_;
}

void ANTSensor::setChannelPeriod(int channelPeriod)
{
	channelPeriod = (channelPeriod > 0 ? channelPeriod_ = channelPeriod : channelPeriod_ = 0);
}

const int ANTSensor::getChannelPeriod()
{
	return channelPeriod_;
}

void ANTSensor::setChannelNumber(int channelNumber)
{
	channelNumber = ((channelNumber >= 0) && (channelNumber <= 7) ? channelNumber_ = channelNumber : channelNumber_ = -1);
	if (channelNumber_ < 0) {
		cout << "Error setting channelNumber(" << channelNumber << ")" << endl;
	}
}

const int ANTSensor::getChannelNumber()
{
	return channelNumber_;
}

const float ANTSensor::getValue()
{
	return value_;
}

void ANTSensor::setValue(float value)
{
	if (value > 0) {
		value_ = value;
	}
}

void ANTSensor::setTimeStampMeas()
{
	timeStampMeas_ = currentTime_->getMilliSeconds();
}

const int ANTSensor::getTimeStampMeas()
{
	return timeStampMeas_;
}

int ANTSensor::getTimeSpan(){

	return(currentTime_->getMilliSeconds() - getTimeStampMeas());
}





const void ANTSensor::printSensorInfo()
{
	//cout << "OK printSensor" << endl;
	cout << "------------ANTSensorInfo--------------" << endl;
	cout << "DeviceNumber: " << this->getDeviceNumber() << endl;
	cout << "ChannelNumber: " << this->getChannelNumber() << endl;
	cout << "DeviceType: " << this->getDeviceType() << endl;
	cout << "---------------------------------------" << endl;
}


void ANTSensor::setDeviceNumber(int deviceNumber)
{
	deviceNumber = (((deviceNumber >= 0) && deviceNumber <= 65535) ? deviceNumber_ = deviceNumber : deviceNumber_ = 0);
}

void ANTSensor::calcRPM()
{
	float revDiff = revCount_ - revBuf_;

	if (revDiff < 0)
	{
		// the 16bit int has overflown
		revDiff += UINT16_MAX;
	}

	if (revDiff == 0)
	{
		//cout << "Unable to calculate RPM. no difference between revCount and revBuf." << endl; //Maybe exception here
		return;
	}

	float timeDiff = measTime_ - timeBuf_;

	if (timeDiff < 0)
	{
		// the 16bit int has overflown
		timeDiff += UINT16_MAX;
	}

	if (timeDiff == 0)
	{
		//cout << "Unable to calculate RPM. no difference between measTime and timeBuf." << endl; //Maybe exception here
		return;
	}

	float ones = 1;
	float onezerotwofour = 1024;
	float sixzero = 60;
	float valueToSeconds = ones / onezerotwofour; // value in 1/1024 seconds
	float secondsToMinutes = ones / sixzero; // 60 seconds pr minute

	float timeMinutes = timeDiff * valueToSeconds * secondsToMinutes;
	revBuf_ = revCount_;
	timeBuf_ = measTime_;
	setValue( revDiff / timeMinutes);

	//return rpm;
}

void ANTSensor::setMeasTime(char ANTdataByte3, char ANTdataByte4)
{
	measTime_ = ANTdataByte3 | (ANTdataByte4 << 8); //Making uint16_t of two chars 
}

void ANTSensor::setRevCount(char ANTdataByte5, char ANTdataByte6)
{
	revCount_ = ANTdataByte5 | (ANTdataByte6 << 8); //Making uint16_t of two chars
}

void ANTSensor::setRPM(float rpm)
{
	rpm_ = rpm;
}

const float ANTSensor::getRPM()
{
	return rpm_;
}

const uint16_t ANTSensor::getRevBuf()
{
	return revBuf_;
}

const uint16_t ANTSensor::getTimeBuf()
{
	return timeBuf_;
}

const uint16_t ANTSensor::getMeasTime()
{
	return measTime_;
}

const uint16_t ANTSensor::getRevCount()
{
	return revCount_;
}



void ANTSensor::setTimeBuf(uint16_t measTime)
{
	timeBuf_ = measTime;
}

void ANTSensor::setRevBuf(uint16_t revCount)
{
	revBuf_ = revCount;
}
