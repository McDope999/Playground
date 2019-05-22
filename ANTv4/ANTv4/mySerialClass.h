//Tobias Bach Aunbøl - Playground Marketing

#pragma once
#include <wiringSerial.h>
#include <string.h>
#include <string>
#include <list>

//This class is a wrapper around the C serial library wiringSerial.h documention for wiringSerial.h can be found at http://wiringpi.com/reference/serial-library/
//In ANT project this class is used to write received ANT data to the RF antenna on serialPort, and receive configuration details from PC over RF communication 
using namespace std;


class mySerialClass {

public:

	mySerialClass(string comPort, int baudRate); 
	mySerialClass();
	string getComport();
	
	void setBaudrate(int baudrate);
	const int getBaudrate();
	void setComport(string comPort);
	int openComPort();
	const int getFileDescriptor();
	void closeComPort();
	void sendChar(unsigned char charToSend); //Sends one char to serialPort
	void sendString(string stringToSend); //Sends stringToSend to SerialPort
	bool isDataAvailable();
	string receiveString(); //This function is blocking for 10 seconds if no data are available or until something is received
	void flush();



private:
	int baudrate_ = 0;
	int fileDescriptor_ = 0;
	string comPort_;
	char receiveBuf_[1000];
	
};