//ANT Project - Tobias bach Aunbøl - Playground Marketing

#include <cstdio>

#include "mySerialClass.h"		//For serialCommunication to RF
#include "myStringFunc.h"		//Helper for searching received strings
#include "ANTCadence.h"
#include "ANTMaster.h"
#include "ANTHeartrate.h"
#include <iostream>
#include <unistd.h>
#include <list>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>			//For signalling every second
#include <sys/signal.h>		//---------SAME-------


#define ANTMASTER_NUMBER 0 //Hardcoded master number
#define CHANNEL_PERIOD_CADENCE 8102
#define CHANNEL_PERIOD_HEARTRATE 8070


using namespace std;

int receiveConfData(); //funktion to call when configuration data is received from PC


ANTMaster myMaster; //ANT Master Class
mySerialClass RFSerial("/dev/ttyUSB0", 9600); //RF Serial Port
myStringFunc stringHelp; //Object to help searching and seperate string received
ANTSensor *sensor; //pointer to sensor object(s)



bool restart_ = false; //checks whether a channel is reset


bool sendFlag_ = false; //is set every second

void sendHandler(int signal) { // is called every second and signalling with sendFlag_ that 
	alarm(1);					//resets alarm
	sendFlag_ = true;	
}


int main()
{

	signal(SIGALRM, sendHandler);			//setting up signal and sendHandler
	alarm(1);								//starts the first signalalarm

	myMaster.setMasterNumber(ANTMASTER_NUMBER);	

	RFSerial.openComPort();


	while (1) { // LOOPING FOREVER

		if (myMaster.getNumberOfSensors() == 0) { //If no sensors on master, then just wait for configuration message on RFSerial
			if (RFSerial.isDataAvailable()) {
				if (receiveConfData() > 0) { //RECEIVED STRING OK

					if (myMaster.InitMasterDevice(0, 1) && myMaster.InitANTMasterChannels()) { //If init all the master channels is ok!
						myMaster.printSensorInfo(); //Print configuration info
						RFSerial.sendString(to_string(myMaster.getMasterNumber()) + ";OK#"); //send SETUP OK 
					}
					else RFSerial.sendString(to_string(myMaster.getMasterNumber()) + ";FAIL#"); //send SETUP FAILED
				}
				else RFSerial.sendString(to_string(myMaster.getMasterNumber()) + ";FAIL#"); //send RECEIVED STRING NOT OK
			}
		}

		if (myMaster.getNumberOfSensors() > 0) { //If master is configured
			if (RFSerial.isDataAvailable()) { //Checks if there is a new configuartion message
				if (receiveConfData() > 0) { //RECEIVED STRING OK
					//THEN RECONFIGURE
					if (myMaster.InitANTMasterChannels()) {
						myMaster.printSensorInfo();
						
						RFSerial.sendString(to_string(myMaster.getMasterNumber()) + ";OK#"); //SETUP OK
					}
					else {
						RFSerial.sendString(to_string(myMaster.getMasterNumber()) + ";FAIL;#");
						myMaster.popSensors();
						myMaster.Close();

					} //SETUP FAILED
				}
				else {
					RFSerial.sendString(to_string(myMaster.getMasterNumber()) + ";FAIL#");
					myMaster.popSensors();
					myMaster.Close();

				}//RECEIVED STRING NOK OK

			}

			else { //If master is configured and there is no new configuration message then listen to sensors
	
				if (sendFlag_) { //If it's to send
					string arrayToSend[myMaster.getNumberOfSensors()]; //makes a string for every sensor
					string stringToSend = to_string(myMaster.getMasterNumber()) + ";"; //First is master number
					for (int i = 0; i < myMaster.getNumberOfSensors(); i++) {
						arrayToSend[i] = to_string(sensor[i].getDeviceNumber()) + ";" + to_string(sensor[i].getValue()) + ";" + to_string(sensor[i].getTimeSpan()) + ";"; // makes a string for every sensor on the form("devNr";"SensorValue";"Time";) 
						stringToSend += arrayToSend[i]; //Concatenates to one string

						if (sensor[i].getTimeSpan() > 10000) { // if we haven't heard from a sensor in 10 seconds

							restart_ = myMaster.ResetSensorChannel(&sensor[i]); //Then reset the channel 
							if (restart_) {
								sensor[i].setTimeStampMeas(); //set new timestamp
								RFSerial.sendString(to_string(myMaster.getMasterNumber()) + ";RESET;" + to_string(sensor[i].getDeviceNumber()) + ";#"); //sends info that the channel has been reset

							}
						}
					}
					RFSerial.sendString(stringToSend + "#"); //else sends the data string
					sendFlag_ = false; 


				}

			}
		}
	}
	myMaster.Close();
	delete[] sensor;


	return 0;
}

int receiveConfData() {

	//if we get new data, then we need to reconfigure the master
	myMaster.popSensors(); //removes sensors from master
	string received;



	while (received.size() == 0) { //Checks for data on serial port and blocks for 3 seconds in receive string to receive
		received = RFSerial.receiveString();
	}
	RFSerial.flush();												//FLUSH SERIAL PORT

	if (stringHelp.stringSearch(received, ';') < 0) {				//Searching received string from PC and puts the subtracted strings in subtrackedStrings_ in myStringFunc class
		return -1; //fail in received string (stop character in received string != ';')
	}


	// Setting sensor device number and type with data received.
	int i = 0;
	list<string>::iterator it;

	for (it = stringHelp.subtrackedStrings_.begin(); it != stringHelp.subtrackedStrings_.end(); it++) { //searching list of subtracted strings: ex: 122;1231;3423 is now 122 1231 3423


		int dev = stoi(*it); //String to int
		if (i == 0 && dev != ANTMASTER_NUMBER) { // IF THE FIRST STRING RECEIVED ISN'T THE ANTMASTER_NUMBER THEN IS EITHER WRONG DATA OR THE DATA ISN'T MEANT FOR THIS MASTER
			stringHelp.removeSubtrackedStrings(); // resets the vector
			received = ""; //resets the buffer
			return -1; 
		}

		if (i == 1 && (dev == 122 || dev == 120)) { //THE Next string is the device type 122 for cadence sensor and 120 for heartrate

				sensor = new ANTSensor[stringHelp.getCount() - 2]; //makes as many sensors as device numbers in received list

				for (int j = 0; j < stringHelp.getCount() - 2; j++) {

					sensor[j].setChannelNumber(0); 
					sensor[j].setDeviceType(dev);

					if (dev == 122) { //If Cadence Sensor
						sensor[j].setChannelPeriod(CHANNEL_PERIOD_CADENCE);
					}
					else if (dev == 120) { //If Heartrate Sensor
						sensor[j].setChannelPeriod(CHANNEL_PERIOD_HEARTRATE);
					}


				}

				
			}
		if ((i == 1) && (dev != 122 && dev != 120)) { //if dev number isn't a cadence or heartrate then clean up
				stringHelp.removeSubtrackedStrings();
				received = "";
				return -1;

			}

		else if (i > 1)
		{
			sensor[i - 2].setDeviceNumber(dev);
			myMaster.pushSensor(&sensor[i - 2]); //pushing the sensors to ANTMaster
		}

			i++;


	}

		stringHelp.removeSubtrackedStrings(); // reset subtrackedStrings_ 

		received = ""; //Reset string buffer
		return 1;

}