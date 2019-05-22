/*
This software is subject to the license described in the License.txt file
included with this software distribution. You may not use this file except
in compliance with this license.

Copyright (c) Dynastream Innovations Inc. 2016
All rights reserved.
*/

#include <fstream>
#include "ANT_SDK/types.h"
#include "ANT_SDK/dsi_framer_ant.hpp"
#include "ANT_SDK/dsi_thread.h"
#include "ANT_SDK/dsi_serial_generic.hpp"
//#include "dsi_debug.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <iostream>
#include <string>
#include "ANTSensor.h"
#include "ANTMaster.h"
#include <iostream>

#define ENABLE_EXTENDED_MESSAGES


using namespace std;
#define USER_BAUDRATE         (57600)  // For AT3/AP2, use 57600
#define USER_RADIOFREQ        (57)


#define USER_TRANSTYPE        (1)
#define USER_CHANNELTYPE	  (1)  // 1 for slave





#define MESSAGE_TIMEOUT       (1000)

// Indexes into message recieved from ANT
#define MESSAGE_BUFFER_DATA1_INDEX ((UCHAR) 0)
#define MESSAGE_BUFFER_DATA2_INDEX ((UCHAR) 1)
#define MESSAGE_BUFFER_DATA3_INDEX ((UCHAR) 2)
#define MESSAGE_BUFFER_DATA4_INDEX ((UCHAR) 3)
#define MESSAGE_BUFFER_DATA5_INDEX ((UCHAR) 4)
#define MESSAGE_BUFFER_DATA6_INDEX ((UCHAR) 5)
#define MESSAGE_BUFFER_DATA7_INDEX ((UCHAR) 6)
#define MESSAGE_BUFFER_DATA8_INDEX ((UCHAR) 7)
#define MESSAGE_BUFFER_DATA9_INDEX ((UCHAR) 8)
#define MESSAGE_BUFFER_DATA10_INDEX ((UCHAR) 9)
#define MESSAGE_BUFFER_DATA11_INDEX ((UCHAR) 10)
#define MESSAGE_BUFFER_DATA12_INDEX ((UCHAR) 11)
#define MESSAGE_BUFFER_DATA13_INDEX ((UCHAR) 12)
#define MESSAGE_BUFFER_DATA14_INDEX ((UCHAR) 13)

using namespace std;


ANT_MESSAGE stMessage;
#include "ANTMaster.h"

ANTMaster::ANTMaster()
{

	ucChannelType = CHANNEL_TYPE_INVALID;
	pclSerialObject = (DSISerialGeneric*)NULL;
	pclMessageObject = (DSIFramerANT*)NULL;
	uiDSIThread = (DSI_THREAD_ID)NULL;
	bMyDone = FALSE;
	bDone = FALSE;
	bDisplay = TRUE;
	bBroadcasting = FALSE;
}

ANTMaster::~ANTMaster()
{
	if (pclMessageObject)
		delete pclMessageObject;

	if (pclSerialObject)
		delete pclSerialObject;

	

	for (vector<ANTSensor*>::iterator it = antSensors_.begin(); it != antSensors_.end(); ++it) {
		delete(*it);
	}

	antSensors_.clear();

}

void ANTMaster::Close()
{
	//Wait for test to be done
	DSIThread_MutexLock(&mutexTestDone);
	bDone = TRUE;

	UCHAR ucWaitResult = DSIThread_CondTimedWait(&condTestDone, &mutexTestDone, DSI_THREAD_INFINITE);
	assert(ucWaitResult == DSI_THREAD_ENONE);

	DSIThread_MutexUnlock(&mutexTestDone);

	//Destroy mutex and condition var
	DSIThread_MutexDestroy(&mutexTestDone);
	DSIThread_CondDestroy(&condTestDone);

	//Close all stuff
	if (pclSerialObject)
		pclSerialObject->Close();

	popSensors();
	/*#if defined(DEBUG_FILE)
	   DSIDebug::Close();
	#endif
	*/
}

BOOL ANTMaster::InitMasterDevice(UCHAR ucDeviceNumber_, UCHAR ucChannelType_)
{
	BOOL bStatus;

	// Initialize condition var and mutex
	UCHAR ucCondInit = DSIThread_CondInit(&condTestDone);
	assert(ucCondInit == DSI_THREAD_ENONE);

	UCHAR ucMutexInit = DSIThread_MutexInit(&mutexTestDone);
	assert(ucMutexInit == DSI_THREAD_ENONE);


	// Create Serial object.
	pclSerialObject = new DSISerialGeneric();
	assert(pclSerialObject);

	// NOTE: Will fail if the module is not available.
	// If no device number was specified on the command line,
	// prompt the user for input.

	if (ucDeviceNumber_ == 0xFF)
	{
		printf("USB Device number?\n"); fflush(stdout);
		char st[1024];
		fgets(st, sizeof(st), stdin);
		sscanf(st, "%u", &ucDeviceNumber_);
	}

	ucChannelType = ucChannelType_;

	// Initialize Serial object.
	// The device number depends on how many USB sticks have been
	// plugged into the PC. The first USB stick plugged will be 0
	// the next 1 and so on.
	//
	// The Baud Rate depends on the ANT solution being used. AP1
	// is 50000, all others are 57600
	bStatus = pclSerialObject->Init(USER_BAUDRATE, ucDeviceNumber_);
	assert(bStatus);

	// Create Framer object.
	pclMessageObject = new DSIFramerANT(pclSerialObject);
	assert(pclMessageObject);

	//Create ONE MORE

	// Initialize Framer object.
	bStatus = pclMessageObject->Init();
	assert(bStatus);


	// Let Serial know about Framer.
	pclSerialObject->SetCallback(pclMessageObject);
	// Open Serial.
	bStatus = pclSerialObject->Open();



	// If the Open function failed, most likely the device
	// we are trying to access does not exist, or it is connected
	// to another program
	if (!bStatus)
	{
		printf("Failed to connect to device at USB port %d\n", ucDeviceNumber_);
		return FALSE;
	}

	// Create message thread.
	uiDSIThread = DSIThread_CreateThread(&ANTMaster::RunMessageThread, this);
	assert(uiDSIThread);

	printf("Initialization was successful!\n"); fflush(stdout);

	return TRUE;
}

bool ANTMaster::InitANTMasterChannels()
{  
	BOOL bStatus = false;
	// Reset system
	printf("Resetting module...\n");
	bStatus = pclMessageObject->ResetSystem();
	DSIThread_Sleep(1000);

	// Start the test by setting network key
	printf("Setting network key...\n");
	UCHAR ucNetKey[8] = USER_NETWORK_KEY;

	//Setting Networkkey
	bStatus = pclMessageObject->SetNetworkKey(USER_NETWORK_NUM, ucNetKey, MESSAGE_TIMEOUT);
	if (bStatus == false) {
		printf("Error setting networkkey for pclMessageObject\n");
		return false;
	}
	else
		printf("Networkkey set\n");



		//-----------------------------------------------------------------------------//
								//Assigning channel
		//----------------------------------------------------------------------------//
	for (int i = 0; i < antSensors_.size(); i++) {

		
		antSensors_[i]->setChannelNumber(i);
		cout << "CHANNEL NUMBER: " << antSensors_[i]->getChannelNumber() << endl;
		printf("Assigning channel...\n");
		bStatus = pclMessageObject->AssignChannel(antSensors_[i]->getChannelNumber(), USER_CHANNELTYPE, USER_NETWORK_NUM, MESSAGE_TIMEOUT);
		if (bStatus == false) {
			printf("Error Assigning channel\n");
			return false;
		}
		else
			printf("channel Assigned\n");
		//Setting channel ID
		printf("Setting channel ID...\n");
		bStatus = pclMessageObject->SetChannelID(antSensors_[i]->getChannelNumber(), antSensors_[i]->getDeviceNumber(), antSensors_[i]->getDeviceType(), USER_TRANSTYPE, MESSAGE_TIMEOUT);
		if (bStatus == false) {
			printf("Error Setting Channel ID\n");
			return false;
		}
		else
			printf("Channel ID set\n");

		//Setting Channel Period

		printf("Setting channel period... \n");

		bStatus = pclMessageObject->SetChannelPeriod(antSensors_[i]->getChannelNumber(), antSensors_[i]->getChannelPeriod(), MESSAGE_TIMEOUT);
		if (bStatus == false) {
			printf("Error Setting Channel Period\n");
			return false;
		}
		else
			printf("Channel period set\n");

		//Setting Radio Freq
		printf("Setting Radio Freq...\n");
		bStatus = pclMessageObject->SetChannelRFFrequency(antSensors_[i]->getChannelNumber(), USER_RADIOFREQ, MESSAGE_TIMEOUT);
		if (bStatus == false) {
			printf("Error Setting Radio Freq\n");
			return false;
		}
		else
			printf("Radio Freq set\n");

		//Opening Channel
		printf("Opening channel...\n");
		bStatus = pclMessageObject->OpenChannel(antSensors_[i]->getChannelNumber(), MESSAGE_TIMEOUT);
		if (bStatus == false) {
			printf("Error opening channel\n");
			return false;
		}
		else
			printf("Channel open\n");
		
		
		}
	pclMessageObject->RxExtMesgsEnable(TRUE);
	return true;
}

DSI_THREAD_RETURN ANTMaster::RunMessageThread(void * pvParameter_)
{
	((ANTMaster*)pvParameter_)->MessageThread();
	return NULL;
}

void ANTMaster::MessageThread()
{
	USHORT usSize;
	bDone = FALSE;

	while (!bDone)
	{
		
		if (pclMessageObject->WaitForMessage(1000))
		{
			//cout << "Start" << endl;
			usSize = pclMessageObject->GetMessage(&stMessage);

			if (bDone)
				break;

			if (usSize == DSI_FRAMER_ERROR)
			{
				// Get the message to clear the error
				usSize = pclMessageObject->GetMessage(&stMessage, MESG_MAX_SIZE_VALUE);
				continue;
			}
			//cout << "HAndle msg" << endl;
			if (usSize != DSI_FRAMER_ERROR && usSize != DSI_FRAMER_TIMEDOUT && usSize != 0)
			{
				ProcessMessage(stMessage, usSize);
			}
		}
	}

	DSIThread_MutexLock(&mutexTestDone);
	UCHAR ucCondResult = DSIThread_CondSignal(&condTestDone);
	assert(ucCondResult == DSI_THREAD_ENONE);
	DSIThread_MutexUnlock(&mutexTestDone);
}

void ANTMaster::ProcessMessage(ANT_MESSAGE stMessage, USHORT usSize_)
{
	BOOL bStatus;
	BOOL bPrintBuffer = FALSE;
	UCHAR ucDataOffset = MESSAGE_BUFFER_DATA2_INDEX;   // For most data messages


	switch (stMessage.ucMessageID)
	{
	case MESG_ACKNOWLEDGED_DATA_ID:
	case MESG_BURST_DATA_ID:
	case MESG_BROADCAST_DATA_ID:
	{
		// The flagged and unflagged data messages have the same
		// message ID. Therefore, we need to check the size to
		// verify of a flag is present at the end of a message.
		// To enable flagged messages, must call ANT_RxExtMesgsEnable first.
		if (usSize_ > MESG_DATA_SIZE)
		{
			UCHAR ucFlag = stMessage.aucData[MESSAGE_BUFFER_DATA10_INDEX];

			if (bDisplay && ucFlag & ANT_EXT_MESG_BITFIELD_DEVICE_ID)
			{
				// Channel ID of the device that we just recieved a message from.
				USHORT usDeviceNumber = stMessage.aucData[MESSAGE_BUFFER_DATA11_INDEX] | (stMessage.aucData[MESSAGE_BUFFER_DATA12_INDEX] << 8);
				UCHAR ucDeviceType = stMessage.aucData[MESSAGE_BUFFER_DATA13_INDEX];
				UCHAR ucTransmissionType = stMessage.aucData[MESSAGE_BUFFER_DATA14_INDEX];

				//printf("Chan ID(%d/%d/%d) - ", usDeviceNumber, ucDeviceType, ucTransmissionType);
				deviceNumberBuf_ = usDeviceNumber;
				deviceTypeBuf_ = ucDeviceType;
			}
		}

		// Display recieved message
		bPrintBuffer = TRUE;
		ucDataOffset = MESSAGE_BUFFER_DATA2_INDEX;   // For most data messages

		if (bDisplay)
		{
			/*if (stMessage.ucMessageID == MESG_ACKNOWLEDGED_DATA_ID)
				printf("Acked Rx:(%d): ", stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX]);
			else if (stMessage.ucMessageID == MESG_BURST_DATA_ID)
				printf("Burst(0x%02x) Rx:(%d): ", ((stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX] & 0xE0) >> 5), stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX] & 0x1F);
			else
				printf("Rx:(%d): ", stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX]);*/
		}
		break;
	}
	case MESG_EXT_BROADCAST_DATA_ID:
	case MESG_EXT_ACKNOWLEDGED_DATA_ID:
	case MESG_EXT_BURST_DATA_ID:
	{

		// The "extended" part of this message is the 4-byte channel
		// id of the device that we recieved this message from. This message
		// is only available on the AT3. The AP2 uses flagged versions of the
		// data messages as shown above.

		// Channel ID of the device that we just recieved a message from.
		USHORT usDeviceNumber = stMessage.aucData[MESSAGE_BUFFER_DATA2_INDEX] | (stMessage.aucData[MESSAGE_BUFFER_DATA3_INDEX] << 8);
		UCHAR ucDeviceType = stMessage.aucData[MESSAGE_BUFFER_DATA4_INDEX];
		UCHAR ucTransmissionType = stMessage.aucData[MESSAGE_BUFFER_DATA5_INDEX];
	


		bPrintBuffer = TRUE;
		ucDataOffset = MESSAGE_BUFFER_DATA6_INDEX;   // For most data messages

		if (bDisplay)
		{
			// Display the channel id
			/*
			printf("Chan ID(%d/%d/%d) ", usDeviceNumber, ucDeviceType, ucTransmissionType);

			if (stMessage.ucMessageID == MESG_EXT_ACKNOWLEDGED_DATA_ID)
				printf("- Acked Rx:(%d): ", stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX]);
			else if (stMessage.ucMessageID == MESG_EXT_BURST_DATA_ID)
				printf("- Burst(0x%02x) Rx:(%d): ", ((stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX] & 0xE0) >> 5), stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX] & 0x1F);
			else
				printf("- Rx:(%d): ", stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX]);*/
		}

		break;
	}

	default:
	{
		break;
	}
	}

	// If we recieved a data message, diplay its contents here.
	if (bPrintBuffer)
	{
		if (bDisplay)

		{

			char myData[8] = { 0 }; //Data buffer

			for (int i = 1; i < 8; i++) {	//Puts received ANT Data in buffer
				myData[i - 1] = stMessage.aucData[ucDataOffset + i];
			}


			
			//-----------------------------------------------------------------------------//
			//THIS IS WHERE THE DATA FUN HAPPENS!!!!!!!!!!!!
			//----------------------------------------------------------------------------//


			for (int i = 0; i < antSensors_.size(); i++) {
				if (deviceTypeBuf_ == 122) //If cadence sensor 
				{
					if (antSensors_[i]->getDeviceType() == deviceTypeBuf_ && antSensors_[i]->getDeviceNumber() == deviceNumberBuf_) {

						antSensors_[i]->setMeasTime(myData[3], myData[4]);
						antSensors_[i]->setRevCount(myData[5], myData[6]);
						antSensors_[i]->calcRPM();
						antSensors_[i]->setTimeStampMeas();
					}
				}
				else if (deviceTypeBuf_ == 120) //if heartrate monitor 
				{
					antSensors_[i]->setValue(myData[6]);
					antSensors_[i]->setTimeStampMeas();
				}

				//REMOVE THE COMMENT MART TO GET ANT DATA:
				//cout << "Device ID: " << antSensors_[i]->getDeviceNumber() << "Value: " << antSensors_[i]->getValue() << endl; 
			
			}
		}

			
		}

		else
		{

			
			
		}

	
	return;
}

void ANTMaster::printSensorInfo()
{
	cout << "ANT MASTER: " << getMasterNumber() << endl;
	cout << "--------------------------------------" << endl;
	for (int i = 0; i < antSensors_.size(); i++) {
		antSensors_[i]->printSensorInfo();
	}
	cout << "----------------------------------------" << endl;
}

void ANTMaster::popSensors()
{

	int count = antSensors_.size();
	for (int i = 0; i < count; i++) {
		antSensors_.pop_back();
	}

}

const int ANTMaster::getNumberOfSensors()
{
	return antSensors_.size();
}






void ANTMaster::pushSensor(ANTSensor  *antSensor)
{


	if (antSensors_.size() >= 8) {
		cout << "sensorArray is 8. ANTMaster can't listen on more than 8 channels" << endl;
		}
	else if (antSensors_.size() <= 8) {
		antSensors_.push_back(antSensor);
	}
}

BOOL ANTMaster::ResetSensorChannel(ANTSensor * antSensor)
{

	BOOL status = false;
	status = pclMessageObject->CloseChannel(antSensor->getChannelNumber(), MESSAGE_TIMEOUT);
	if (status) {
		cout << "Closing Channel: " << antSensor->getChannelNumber() << endl;
	}
	else {
		cout << "Error Closing Channel: " << antSensor->getChannelNumber() << endl;
		return false;
	}

	status = pclMessageObject->OpenChannel(antSensor->getChannelNumber(), MESSAGE_TIMEOUT);
	if (status) {
		cout << "Opening Channel: " << antSensor->getChannelNumber() << endl;
	}
	else {
		cout << "Error opening channel: " << antSensor->getChannelNumber() << endl;
		return false;
	}

	pclMessageObject->RxExtMesgsEnable(TRUE);
	return true;
}

const int ANTMaster::getANTArraySize()
{
	return antSensors_.size();
}


const int ANTMaster::getMasterNumber()
{
	return masterNumber_;
}

void ANTMaster::setMasterNumber(int masterNumber)
{
	masterNumber_ = masterNumber;
}

