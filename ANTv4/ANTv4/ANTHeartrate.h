#pragma once
#include "ANTSensor.h"

#define CHANNEL_PERIOD 8070
#define DEVICE_TYPE 120


class ANTHeartrate : public ANTSensor
{

public:
	ANTHeartrate();
	ANTHeartrate(int deviceNumber);
	void setHeartrate(char ANTDatabyte6);
	const int getHeartrate();
	void foo();
	float getData();

private:
	int heartrate_ = 0;
	
};