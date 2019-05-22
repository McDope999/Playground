#include "mySerialClass.h"
#include <iostream>
#include <unistd.h>

using namespace std;


mySerialClass::mySerialClass(string comPort, int baudRate)
{
	setBaudrate(baudRate);
	setComport(comPort);
}

mySerialClass::mySerialClass()
{
	serialFlush(fileDescriptor_);
}

string mySerialClass::getComport()
{
	return comPort_;
}

void mySerialClass::setBaudrate(int baudrate)
{
	baudrate = (baudrate > 0 ? baudrate_ = baudrate : baudrate_ = 0);
}
const int mySerialClass::getBaudrate()
{
	return baudrate_;
}

void mySerialClass::setComport(string comPort)
{
	comPort_ = comPort;
	
}

int mySerialClass::openComPort()
{
	char comPortbuf[comPort_.size() + 1];
	strcpy(comPortbuf, comPort_.c_str());


	fileDescriptor_ = serialOpen(comPortbuf, baudrate_);
	
	if (fileDescriptor_ < 0) {
		std::cout << "error opening serial port: " << fileDescriptor_ << std::endl;
		fileDescriptor_ = -1;
		return fileDescriptor_;
	}
	else {
		std::cout << "serialPort Open" << std::endl;
		return fileDescriptor_;
	}
	
}

const int mySerialClass::getFileDescriptor()
{
	return fileDescriptor_;
}

void mySerialClass::closeComPort()
{
	cout << "serialPort Closed" << endl;
	serialClose(fileDescriptor_);
}

void mySerialClass::sendChar(unsigned char charToSend)
{
	serialPutchar(fileDescriptor_, charToSend);
}

void mySerialClass::sendString(string stringToSend)
{
	const char *buf = stringToSend.c_str();
	serialPuts(getFileDescriptor(), buf);
	
}

bool mySerialClass::isDataAvailable()
{
	if (serialDataAvail(getFileDescriptor()) > 1) {
		return true;
	}
	else {
		return false;
	}
}


string mySerialClass::receiveString()
{
	
	int bytesToRead = serialDataAvail(getFileDescriptor());
	
	if (bytesToRead < 0) {
		cout << "failed to read/receive from serial" << endl;
	}

	else if (bytesToRead > 0) {
		usleep(3000000);
		bytesToRead = serialDataAvail(getFileDescriptor());
		
		for (int i = 0; i < bytesToRead; i++) {
			
			receiveBuf_[i] = serialGetchar(getFileDescriptor());
			
		}
		serialFlush(fileDescriptor_);
	}
	
	string buf = receiveBuf_;
	
	buf = buf.substr(0, bytesToRead);
	

	return buf;
		
}

void mySerialClass::flush()
{
	serialFlush(getFileDescriptor());
}

