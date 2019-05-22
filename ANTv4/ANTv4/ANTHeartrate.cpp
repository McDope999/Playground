#include "ANTHeartrate.h"
#include <iostream>

using namespace std;

ANTHeartrate::ANTHeartrate()
{
	setDeviceType(0);
	setChannelPeriod(CHANNEL_PERIOD);
	setDeviceType(DEVICE_TYPE);
	setChannelNumber(0);
}

ANTHeartrate::ANTHeartrate(int deviceNumber)
{
	setDeviceNumber(deviceNumber);
	setChannelPeriod(CHANNEL_PERIOD);
	setDeviceType(DEVICE_TYPE);
}

void ANTHeartrate::setHeartrate(char ANTDatabyte6)
{
	if (ANTDatabyte6 > 0) {
		heartrate_ = ANTDatabyte6;
	}
}

const int ANTHeartrate::getHeartrate()
{
	return heartrate_;
}

void ANTHeartrate::foo()
{
}

float ANTHeartrate::getData()
{
	return float(heartrate_);
}
