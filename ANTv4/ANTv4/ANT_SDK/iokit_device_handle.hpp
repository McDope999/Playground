/*
This software is subject to the license described in the License.txt file
included with this software distribution. You may not use this file except
in compliance with this license.

Copyright (c) Dynastream Innovations Inc. 2016
All rights reserved.
*/
#ifndef IOKIT_DEVICE_HANDLE_HPP
#define IOKIT_DEVICE_HANDLE_HPP

#include <list>
#include "types.h"

#include "iokit_types.hpp"
#include "iokit_device.hpp"
#include "iokit_interface.hpp"
#include "iokit_transfer.hpp"


//////////////////////////////////////////////////////////////////////////////////
// Public Definitions
//////////////////////////////////////////////////////////////////////////////////

//typedef struct IOKitTransferElement_
//{
//   auto_ptr<IOKitTransfer> *transfer_object;
//   struct IOKitTransferElement_ *next_element;
//} IOKitTransferElement;                    //linked list elements

//////////////////////////////////////////////////////////////////////////////////
// Public Class Prototypes
//////////////////////////////////////////////////////////////////////////////////

class IOKitDeviceHandle
{

   //STATIC

   public:
      static IOKitError::Enum Open(const IOKitDevice& dev, IOKitDeviceHandle*& dev_handle);
      static void Close(IOKitDeviceHandle*& dev_handle);
      static IOKitError::Enum Reset(IOKitDeviceHandle*& dev_handle);

      static const int MAX_INTERFACES = 32;

   //NON-STATIC

   public:
      IOKitError::Enum ClaimInterface(int interface_number);
      IOKitError::Enum ReleaseInterface(int interface_number);

      //Retrieve Descriptors
      IOKitError::Enum GetDescriptor(uint8_t desc_type, uint8_t desc_index, unsigned char data[], int length, int& transferred);
      IOKitError::Enum GetStringDescriptor(uint8_t desc_index, uint16_t langid, string_descriptor& str_desc, int& transferred);
      IOKitError::Enum GetStringDescriptorAscii(uint8_t desc_index, unsigned char data[], int length, int& transferred);

      IOKitError::Enum Write(unsigned char endpoint, unsigned char data[], int length, int& actual_length, unsigned int timeout); //!!can we make data const?
      IOKitError::Enum Read(unsigned char endpoint, unsigned char data[], int length, int& actual_length, unsigned int timeout);

      IOKitError::Enum WriteControl(unsigned char requestType, unsigned char request, unsigned short value, unsigned short index, unsigned char* data, int length, int& actual_length, unsigned int timeout);

      void CancelQueuedTransfers(void);

      const IOKitDevice device;  //make sure this ISN'T a reference


   protected:
      IOKitDeviceHandle(const IOKitDevice& dev) throw (IOKitError::Enum);
      virtual ~IOKitDeviceHandle();

   private:
      void PClose();
      std::list<IOKitTransfer*> transfer_list;
      BOOL areTransfersClosed;  //TRUE indicates all transfers are cancelled and no new ones should be started
      pthread_mutex_t lock_transfer_list;            //protect transfer_list access
      pthread_mutex_t lock_ep_in_lock;
      pthread_mutex_t lock_ep_other_lock;

   private:
      IOKitError::Enum TryOpen();

      static kern_return_t GetDevice(uint32_t dev_location, IOKitDevice& clDevice_);

      static void DevicesDetached(void* param, io_iterator_t removed_devices);
      static void* EventThreadStart(void* param_);
      void EventThread();

      IOKitError::Enum InterruptTransfer(unsigned char endpoint, unsigned char data[], int length, unsigned int timeout, int& transferred);
      IOKitError::Enum BulkTransfer(unsigned char endpoint, unsigned char data[], int length, unsigned int timeout, int& transferred);

      IOKitError::Enum SendSyncControlTransfer(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, unsigned char data[], uint16_t wLength, unsigned int timeout, int& transferred);
      IOKitError::Enum SendSyncInterruptTransfer(unsigned char endpoint, unsigned char buffer[], int length, unsigned int timeout, int& transferred);
      IOKitError::Enum SendSyncBulkTransfer(unsigned char endpoint, unsigned char buffer[], int length, unsigned int timeout, int& transferred);

      IOKitError::Enum handleTransfer(IOKitTransfer* transfer, BOOL isRead, int& transferred, unsigned int timeout);

   public:  //!! Should be private, but directly accessed by iokittransfer right now

      mach_port_t mach_port;

      pthread_t thread;       // thread to handle notifications
      pthread_mutex_t lock_runloop;   // lock protects run loop
      pthread_cond_t cond_runloop;

      volatile BOOL thread_started;
      volatile CFRunLoopRef run_loop;

      usb_device_t** usb_device;  //libusb has this in libusb_device

      //os stuff
      CFRunLoopSourceRef   event_source;

      pthread_mutex_t lock;       //lock protects claimed_interfaces
      IOKitInterface interfaces[MAX_INTERFACES];
};

#endif // !defined(USB_DEVICE_HANDLE_HPP)

