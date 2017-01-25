/*
	LPCUSB, an USB device driver for LPC microcontrollers	
	Copyright (C) 2006 Bertrik Sikken (bertrik@sikken.nl)

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright
	   notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.
	3. The name of the author may not be used to endorse or promote products
	   derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
	IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, 
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
	THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
	@file
*/

#include "usbstruct.h"		// for TSetupPacket

/*************************************************************************
	USB configuration
**************************************************************************/

#define MAX_PACKET_SIZE0	64		/**< maximum packet size for EP 0 */

/*************************************************************************
	USB hardware interface
**************************************************************************/

// endpoint status sent through callback
#define EP_STATUS_DATA		(1<<0)		/**< EP has data */
#define EP_STATUS_STALLED	(1<<1)		/**< EP is stalled */
#define EP_STATUS_SETUP		(1<<2)		/**< EP received setup packet */
#define EP_STATUS_ERROR		(1<<3)		/**< EP data was overwritten by setup packet */
#define EP_STATUS_NACKED	(1<<4)		/**< EP sent NAK */

// device status sent through callback
#define DEV_STATUS_CONNECT		(1<<0)	/**< device just got connected */
#define DEV_STATUS_SUSPEND		(1<<2)	/**< device entered suspend state */
#define DEV_STATUS_RESET		(1<<4)	/**< device just got reset */

// interrupt bits for NACK events in USBHwNakIntEnable
// (these bits conveniently coincide with the LPC176x USB controller bit)
#define INACK_CI		(1<<1)			/**< interrupt on NACK for control in */
#define INACK_CO		(1<<2)			/**< interrupt on NACK for control out */
#define INACK_II		(1<<3)			/**< interrupt on NACK for interrupt in */
#define INACK_IO		(1<<4)			/**< interrupt on NACK for interrupt out */
#define INACK_BI		(1<<5)			/**< interrupt on NACK for bulk in */
#define INACK_BO		(1<<6)			/**< interrupt on NACK for bulk out */

BOOL USBHwInit			(void);
void USBHwISR			(void);

void USBHwNakIntEnable	(unsigned char bIntBits);

void USBHwConnect		(BOOL fConnect);

void USBHwSetAddress	(unsigned char bAddr);
void USBHwConfigDevice	(BOOL fConfigured);

// endpoint operations
void USBHwEPConfig		(unsigned char bEP, unsigned short wMaxPacketSize);
int  USBHwEPRead		(unsigned char bEP, unsigned char *pbBuf, int iMaxLen);
int	 USBHwEPWrite		(unsigned char bEP, unsigned char *pbBuf, int iLen);
void USBHwEPStall		(unsigned char bEP, BOOL fStall);
unsigned char   USBHwEPGetStatus	(unsigned char bEP);

/** Endpoint interrupt handler callback */
typedef void (TFnEPIntHandler)	(unsigned char bEP, unsigned char bEPStatus);
void USBHwRegisterEPIntHandler	(unsigned char bEP, TFnEPIntHandler *pfnHandler);

/** Device status handler callback */
typedef void (TFnDevIntHandler)	(unsigned char bDevStatus);
void USBHwRegisterDevIntHandler	(TFnDevIntHandler *pfnHandler);

/** Frame event handler callback */
typedef void (TFnFrameHandler)(unsigned short wFrame);
void USBHwRegisterFrameHandler(TFnFrameHandler *pfnHandler);


/*************************************************************************
	USB application interface
**************************************************************************/

// initialise the complete stack, including HW
BOOL USBInit(void);

/** Request handler callback (standard, vendor, class) */
typedef BOOL (TFnHandleRequest)(TSetupPacket *pSetup, int *piLen, unsigned char **ppbData);
void USBRegisterRequestHandler(int iType, TFnHandleRequest *pfnHandler, unsigned char *pbDataStore);
void USBRegisterCustomReqHandler(TFnHandleRequest *pfnHandler);

/** Descriptor handler callback */
typedef BOOL (TFnGetDescriptor)(unsigned short wTypeIndex, unsigned short wLangID, int *piLen, unsigned char **ppbData);

/** Default standard request handler */
BOOL USBHandleStandardRequest(TSetupPacket *pSetup, int *piLen, unsigned char **ppbData);

/** Default EP0 handler */
void USBHandleControlTransfer(unsigned char bEP, unsigned char bEPStat);

/** Descriptor handling */
void USBRegisterDescriptors(const unsigned char *pabDescriptors);
BOOL USBGetDescriptor(unsigned short wTypeIndex, unsigned short wLangID, int *piLen, unsigned char **ppbData);
