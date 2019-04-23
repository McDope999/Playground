/*
This software is subject to the license described in the License.txt file
included with this software distribution. You may not use this file except
in compliance with this license.

Copyright (c) Dynastream Innovations Inc. 2016
All rights reserved.
*/
#include "demo.h"
#include <fstream>
#include "types.h"
#include "dsi_framer_ant.hpp"
#include "dsi_thread.h"
#include "dsi_serial_generic.hpp"
//#include "dsi_debug.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <iostream>
#include <string>

#define ENABLE_EXTENDED_MESSAGES

#define USER_BAUDRATE         (57600)  // For AT3/AP2, use 57600
#define USER_RADIOFREQ        (57)

#define USER_ANTCHANNEL       (0)
#define USER_DEVICENUM_SEARCH (0)

#define USER_DEVICENUM_C2	  (2393)
#define USER_DEVICENUM_C3	  (2297)

#define USER_DEVICETYPE       (123)
#define USER_TRANSTYPE        (1)
#define USER_CHANNELTYPE	  (1)  // 1 for slave

#define USER_NETWORK_KEY      {0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45,}
#define USER_NETWORK_NUM      (0)      // The network key is assigned to this network number

#define USER_CHANNELPERIOD	  (8118)  //ANT+ caddence sensor is sending at 4 Hz

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

//MY GLOBALS:
static uint16_t timeBuf = NULL;
static uint16_t revBuf = NULL;
static float RPM = 0;

using namespace std;

//


////////////////////////////////////////////////////////////////////////////////
// main
//
// Usage:
//
// c:\DEMO_LIB.exe [device_no] [channel_type]
//
// ... where
//
// device_no:     USB Device port, starting at 0
// channel_type:  Master = 0, Slave = 1
//
// ... example
//
// c:\Demo_LIB.exe 0 0
//
// Comment to USB port 0 and open a Master channel
//
// If optional arguements are not supplied, user will
// be prompted to enter these after the program starts.
//
////////////////////////////////////////////////////////////////////////////////
ANT_MESSAGE stMessage;


int main()
{
	//DSIDebug::Close();
   Demo* device1 = new Demo();
   Demo* device2 = new Demo();
   device1->Init(USER_DEVICENUM_C2, USER_CHANNELTYPE);
   device1->InitMyANT(USER_DEVICENUM_C2);
   device2->Init(USER_DEVICENUM_C3, USER_CHANNELTYPE);
   device2->InitMyANT(USER_DEVICENUM_C3);

	   cout << "Press any key to exit" << endl;
	   cin >> testDemo;
		   pclDemo->Close();

      delete pclDemo;
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Demo
//
// Constructor, intializes Demo class
//
////////////////////////////////////////////////////////////////////////////////
Demo::Demo()
{
   ucChannelType = CHANNEL_TYPE_INVALID;
   pclSerialObject = (DSISerialGeneric*)NULL;
   pclMessageObject = (DSIFramerANT*)NULL;
   uiDSIThread = (DSI_THREAD_ID)NULL;
   bMyDone = FALSE;
   bDone = FALSE;
   bDisplay = TRUE;
   bBroadcasting = FALSE;

   //memset(aucTransmitBuffer,0,ANT_STANDARD_DATA_PAYLOAD_SIZE);
}

////////////////////////////////////////////////////////////////////////////////
// ~Demo
//
// Destructor, clean up and loose memory
//
////////////////////////////////////////////////////////////////////////////////
Demo::~Demo()
{
   if(pclMessageObject)
      delete pclMessageObject;

   if(pclSerialObject)
      delete pclSerialObject;
}
void Demo::InitMyANT(UCHAR ucDeviceNumber_)
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
		printf("Error setting networkkey\n");
	}
	else
		printf("Networkkey set\n");

	//Assigning channel
	printf("Assigning channel...\n");
	bStatus = pclMessageObject->AssignChannel(USER_ANTCHANNEL, 0, 0, MESSAGE_TIMEOUT);
	if (bStatus == false) {
		printf("Error Assigning channel\n");
	}
	else
		printf("channel Assigned\n");
	//Setting channel ID
	printf("Setting channel ID...\n");
	bStatus = pclMessageObject->SetChannelID(USER_ANTCHANNEL, ucDeviceNumber_ , USER_DEVICETYPE, USER_TRANSTYPE, MESSAGE_TIMEOUT);
	if (bStatus == false) {
		printf("Error Setting Channel ID\n");
	}
	else
		printf("Channel ID set\n");
	
	//Setting Channel Period

	printf("Setting channel period... \n");

	bStatus = pclMessageObject->SetChannelPeriod(USER_ANTCHANNEL, USER_CHANNELPERIOD, MESSAGE_TIMEOUT);
	if (bStatus == false) {
		printf("Error Setting Channel Period\n");
	}
	else
		printf("Channel period set\n");

	//Setting Radio Freq
	printf("Setting Radio Freq...\n");
	bStatus = pclMessageObject->SetChannelRFFrequency(USER_ANTCHANNEL, USER_RADIOFREQ, MESSAGE_TIMEOUT);
	if (bStatus == false) {
		printf("Error Setting Radio Freq\n");
	}
	else
		printf("Radio Freq set\n");

	//Opening Channel
	printf("Opening channel...\n");
	bStatus = pclMessageObject->OpenChannel(USER_ANTCHANNEL, MESSAGE_TIMEOUT);
	if (bStatus == false) {
		printf("Error opening channel\n");
	}
	else
		printf("Channel open\n");



	//Enabling extended messages

	pclMessageObject->RxExtMesgsEnable(TRUE);

}
////////////////////////////////////////////////////////////////////////////////
// Init
//
// Initize the Demo and ANT Library.
//
// ucDeviceNumber_: USB Device Number (0 for first USB stick plugged and so on)
//                  If not specified on command line, 0xFF is passed in as invalid.
// ucChannelType_:  ANT Channel Type. 0 = Master, 1 = Slave
//                  If not specified, 2 is passed in as invalid.
//
////////////////////////////////////////////////////////////////////////////////
BOOL Demo::Init(UCHAR ucDeviceNumber_, UCHAR ucChannelType_)
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

   if(ucDeviceNumber_ == 0xFF)
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
   if(!bStatus)
   {
      printf("Failed to connect to device at USB port %d\n", ucDeviceNumber_);
      return FALSE;
   }

   // Create message thread.
   uiDSIThread = DSIThread_CreateThread(&Demo::RunMessageThread, this);
   assert(uiDSIThread);

   printf("Initialization was successful!\n"); fflush(stdout);

   return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
// Close
//
// Close connection to USB stick.
//
////////////////////////////////////////////////////////////////////////////////
void Demo::Close()
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
   if(pclSerialObject)
      pclSerialObject->Close();

/*#if defined(DEBUG_FILE)
   DSIDebug::Close();
#endif
*/
}

////////////////////////////////////////////////////////////////////////////////
// RunMessageThread
//
// Callback function that is used to create the thread. This is a static
// function.
//
////////////////////////////////////////////////////////////////////////////////
DSI_THREAD_RETURN Demo::RunMessageThread(void *pvParameter_)
{
   ((Demo*) pvParameter_)->MessageThread();
   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
// MessageThread
//
// Run message thread
////////////////////////////////////////////////////////////////////////////////
void Demo::MessageThread()
{
   
   USHORT usSize;
   bDone = FALSE;

   while(!bDone)
   {
      if(pclMessageObject->WaitForMessage(1000))
      {
         usSize = pclMessageObject->GetMessage(&stMessage);

         if(bDone)
            break;

         if(usSize == DSI_FRAMER_ERROR)
         {
            // Get the message to clear the error
            usSize = pclMessageObject->GetMessage(&stMessage, MESG_MAX_SIZE_VALUE);
            continue;
         }

         if(usSize != DSI_FRAMER_ERROR && usSize != DSI_FRAMER_TIMEDOUT && usSize != 0)
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


////////////////////////////////////////////////////////////////////////////////
// ProcessMessage
//
// Process ALL messages that come from ANT, including event messages.
//
// stMessage: Message struct containing message recieved from ANT
// usSize_:
////////////////////////////////////////////////////////////////////////////////
void Demo::ProcessMessage(ANT_MESSAGE stMessage, USHORT usSize_)
{
   BOOL bStatus;
   BOOL bPrintBuffer = FALSE;
   UCHAR ucDataOffset = MESSAGE_BUFFER_DATA2_INDEX;   // For most data messages


   switch(stMessage.ucMessageID)
   {
      case MESG_ACKNOWLEDGED_DATA_ID:
      case MESG_BURST_DATA_ID:
      case MESG_BROADCAST_DATA_ID:
      {
         // The flagged and unflagged data messages have the same
         // message ID. Therefore, we need to check the size to
         // verify of a flag is present at the end of a message.
         // To enable flagged messages, must call ANT_RxExtMesgsEnable first.
         if(usSize_ > MESG_DATA_SIZE)
         {
            UCHAR ucFlag = stMessage.aucData[MESSAGE_BUFFER_DATA10_INDEX];

            if(bDisplay && ucFlag & ANT_EXT_MESG_BITFIELD_DEVICE_ID)
            {
               // Channel ID of the device that we just recieved a message from.
               USHORT usDeviceNumber = stMessage.aucData[MESSAGE_BUFFER_DATA11_INDEX] | (stMessage.aucData[MESSAGE_BUFFER_DATA12_INDEX] << 8);
               UCHAR ucDeviceType =  stMessage.aucData[MESSAGE_BUFFER_DATA13_INDEX];
               UCHAR ucTransmissionType = stMessage.aucData[MESSAGE_BUFFER_DATA14_INDEX];

               printf("Chan ID(%d/%d/%d) - ", usDeviceNumber, ucDeviceType, ucTransmissionType);
            }
         }

         // Display recieved message
         bPrintBuffer = TRUE;
         ucDataOffset = MESSAGE_BUFFER_DATA2_INDEX;   // For most data messages

         if(bDisplay)
         {
            if(stMessage.ucMessageID == MESG_ACKNOWLEDGED_DATA_ID )
               printf("Acked Rx:(%d): ", stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX]);
            else if(stMessage.ucMessageID == MESG_BURST_DATA_ID)
               printf("Burst(0x%02x) Rx:(%d): ", ((stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX] & 0xE0) >> 5), stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX] & 0x1F );
            else
               printf("Rx:(%d): ", stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX]);
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
         UCHAR ucDeviceType =  stMessage.aucData[MESSAGE_BUFFER_DATA4_INDEX];
         UCHAR ucTransmissionType = stMessage.aucData[MESSAGE_BUFFER_DATA5_INDEX];

         bPrintBuffer = TRUE;
         ucDataOffset = MESSAGE_BUFFER_DATA6_INDEX;   // For most data messages

         if(bDisplay)
         {
            // Display the channel id
            printf("Chan ID(%d/%d/%d) ", usDeviceNumber, ucDeviceType, ucTransmissionType );

            if(stMessage.ucMessageID == MESG_EXT_ACKNOWLEDGED_DATA_ID)
               printf("- Acked Rx:(%d): ", stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX]);
            else if(stMessage.ucMessageID == MESG_EXT_BURST_DATA_ID)
               printf("- Burst(0x%02x) Rx:(%d): ", ((stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX] & 0xE0) >> 5), stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX] & 0x1F );
            else
               printf("- Rx:(%d): ", stMessage.aucData[MESSAGE_BUFFER_DATA1_INDEX]);
         }

         break;
      }

      default:
      {
         break;
      }
   }

   // If we recieved a data message, diplay its contents here.
   if(bPrintBuffer)
   {
      if(bDisplay)
      {
         printf("[%02x],[%02x],[%02x],[%02x],[%02x],[%02x],[%02x],[%02x]\n",
            stMessage.aucData[ucDataOffset + 1],
            stMessage.aucData[ucDataOffset + 2],
            stMessage.aucData[ucDataOffset + 3],
            stMessage.aucData[ucDataOffset + 4],
            stMessage.aucData[ucDataOffset + 5],
            stMessage.aucData[ucDataOffset + 6],
		    stMessage.aucData[ucDataOffset + 7]);
		

		 /*OLD STUFF*/
		 /*
		 char myData[8] = { 0 };

		 for (int i = 1; i < 8; i++) {
			 myData[i - 1] = stMessage.aucData[ucDataOffset + i];
		}
		 printf("MYDATA: ");
		 for (int i = 0; i < 8; i++) {

			 printf("%d - ", myData[i]);

		 }


		 printf("\n");

		
		 uint16_t measTime = myData[3] | (myData[4] << 8);
		 uint16_t revCount = myData[5] | (myData[6] << 8);
		 cout << "measTime = " << measTime << " | revCount = " << revCount << endl;

		 if (revBuf == NULL) {

			 // no basis for calc
		 }

		 else if (revBuf == revCount) {
			
			 // no need to calc
		 }

		 else
		 {
			 float rpm = calcRPM(measTime, revCount, timeBuf, revBuf);
			 // ALL GOOD, update RPM in model
			 RPM = rpm;
		 }

		 cout << "RPM: " << RPM << endl;
		 //save in buffer
		 revBuf = revCount;
		 timeBuf = measTime;

		 //LOGFILE STUFF
		 /*LOGFILE STUFF
			//std::ofstream logfile;
			//logfile.open("log.txt", std::ios_base::app);


			/*for (int i = 0; i < 7; i++) {
				myData[i] = stMessage.aucData[ucDataOffset + i+1];
				logfile << "%d" << /*static_cast<uint16_t>*//*myData[i] << " - "; //stMessage.aucData[ucDataOffset+i] << " HEJ ";
					//cout << static_cast<int16_t>1myData[i]) << " HEJ ";
			}

			logfile << "\n";*/
      }
      else
      {
         static int iIndex = 0;
         static char ac[] = {'|','/','-','\\'};
         printf("Rx: %c\r",ac[iIndex++]); fflush(stdout);
         iIndex &= 3;

      }
   }

   return;
}

float Demo::calcRPM(uint16_t measTime, uint16_t revCount, uint16_t timeBuf, uint16_t revBuf)
{
	float revDiff = revCount - revBuf;
	if (revDiff < 0)
	{
		// the 16bit int has overflown
		revDiff += UINT16_MAX;
	}

	if (revDiff == 0)
	{
		cout << "Unable to calculate RPM. no difference between revCount and revBuf." << endl; //Maybe exception here
	}

	float timeDiff = measTime - timeBuf;

	if (timeDiff < 0)
	{
		// the 16bit int has overflown
		timeDiff += UINT16_MAX;
	}

	if (timeDiff == 0)
	{
		cout << "Unable to calculate RPM. no difference between measTime and timeBuf." << endl; //Maybe exception here
	}

	float ones = 1;
	float onezerotwofour = 1024;
	float sixzero = 60;
	float valueToSeconds = ones / onezerotwofour; // value in 1/1024 seconds
	float secondsToMinutes = ones / sixzero ; // 60 seconds pr minute

	float timeMinutes = timeDiff * valueToSeconds * secondsToMinutes;

	float rpm = (60 * (revDiff * 1024)) / timeDiff;
	//float rpm = revDiff / timeMinutes;

	return rpm;
}



