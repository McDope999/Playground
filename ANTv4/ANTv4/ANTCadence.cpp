#include "ANTCadence.h"
#include <iostream>

using namespace std;





ANTCadence::ANTCadence()
{
	setDeviceNumber(0);
	setChannelPeriod(CHANNEL_PERIOD);
	setDeviceType(DEVICE_TYPE);
	setChannelNumber(0);
}

void ANTCadence::calcRPM()
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
	rpm_ = revDiff / timeMinutes;

	//return rpm;
}

void ANTCadence::setMeasTime(char ANTdataByte3, char ANTdataByte4)
{
	measTime_ = ANTdataByte3 | (ANTdataByte4 << 8); //Making uint16_t of two chars 
}

void ANTCadence::setRevCount(char ANTdataByte5, char ANTdataByte6)
{
	revCount_ = ANTdataByte5 | (ANTdataByte6 << 8); //Making uint16_t of two chars
}

void ANTCadence::setRPM(float rpm)
{
	rpm_ = rpm;
}

void ANTCadence::foo()
{
	cout << "HELLO cadence" << endl;
}

const float ANTCadence::getRPPM()
{
	return rpm_;
}

const uint16_t ANTCadence::getRevBuf()
{
	return revBuf_;
}

const uint16_t ANTCadence::getTimeBuf()
{
	return timeBuf_;
}

const uint16_t ANTCadence::getMeasTime()
{
	return measTime_;
}

const uint16_t ANTCadence::getRevCount()
{
	return revCount_;
}

float ANTCadence::getData()
{
	return this->getRPPM();
}

void ANTCadence::setTimeBuf(uint16_t measTime)
{
	timeBuf_ = measTime;
}

void ANTCadence::setRevBuf(uint16_t revCount)
{
	revBuf_ = revCount;
}

