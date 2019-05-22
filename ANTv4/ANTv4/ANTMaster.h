
#ifndef _ANTMASTER_H_
#define _ANTMASTER_H_

#include "ANT_SDK/types.h"
#include "ANT_SDK/dsi_framer_ant.hpp"
#include "ANT_SDK/dsi_thread.h"
#include "ANT_SDK/dsi_serial_generic.hpp"
#include <string>
#include "ANTSensor.h"

#include <list>


#define CHANNEL_TYPE_MASTER   (0)
#define CHANNEL_TYPE_SLAVE    (1)
#define CHANNEL_TYPE_INVALID  (2)
#define USER_NETWORK_KEY      {0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45,}
#define USER_NETWORK_NUM      (0)      // The network key is assigned to this network number
#define MAX_ANT_CHANNELS	  (8) //ANTMaster cant listen on more than 8 ANTchannels

using namespace std;

class ANTMaster {

public:

	ANTMaster();
	virtual ~ANTMaster();

	void Close();
	
	BOOL InitMasterDevice(UCHAR ucDeviceNumber_, UCHAR ucChannelType_);
	bool InitANTMasterChannels(); //Init channelse to ANTSensors.... NB: cadanceSensors_ Needs to be pushed first
	void pushSensor(ANTSensor *antSensor); //Pushes sensor in the right vector
	BOOL ResetSensorChannel(ANTSensor *antSensor);
	const int getANTArraySize();
	//const void print();
	const int getMasterNumber();
	void setMasterNumber(int masterNumber);
	void printSensorInfo();
	void popSensors();
	const int getNumberOfSensors();

private:
//------------------------------------------------------//
	//ANT SDK STUFF:
//-----------------------------------------------------//
	//Starts the Message thread.
	static DSI_THREAD_RETURN RunMessageThread(void *pvParameter_);
	
	//Listens for a response from the module
	void MessageThread();
	
	//Decodes the received message
	void ProcessMessage(ANT_MESSAGE stMessage, USHORT usSize_);

	
	BOOL bBursting; //holds whether the bursting phase of the test has started
	BOOL bBroadcasting;
	BOOL bMyDone;
	BOOL bDone;
	UCHAR ucChannelType;

	DSISerialGeneric* pclSerialObject; //Pointer to serial object
	DSIFramerANT* pclMessageObject; //Pointer to ANTFramer
	
	DSI_THREAD_ID uiDSIThread;
	DSI_CONDITION_VAR condTestDone;
	DSI_MUTEX mutexTestDone;
	BOOL bDisplay;
	UCHAR aucTransmitBuffer[ANT_STANDARD_DATA_PAYLOAD_SIZE];
//--------------------------------------------------------//
	//My ANT Stuff:
//-------------------------------------------------------//
   
	//vector<ANTCadence*> cadanceSensors_;
	//vector<ANTHeartrate*> heartRateSensors_;
	vector<ANTSensor*> antSensors_;
	int deviceNumberBuf_ = NULL;
	int deviceTypeBuf_ = NULL;
	int masterNumber_ = 0; //Masternumber;
	




	// UCHAR pucData[USER_DATA_LENGTH];  //data to transfer in a burst




};

#endif //TEST_H
