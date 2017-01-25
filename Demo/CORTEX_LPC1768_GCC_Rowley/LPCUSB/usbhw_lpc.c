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


/** @file
	USB hardware layer
 */


#include "usbdebug.h"
#include "usbhw_lpc.h"
#include "usbapi.h"

/** Installed device interrupt handler */
static TFnDevIntHandler *_pfnDevIntHandler = NULL;
/** Installed endpoint interrupt handlers */
static TFnEPIntHandler	*_apfnEPIntHandlers[16];
/** Installed frame interrupt handlers */
static TFnFrameHandler	*_pfnFrameHandler = NULL;

/** convert from endpoint address to endpoint index */
#define EP2IDX(bEP)	((((bEP)&0xF)<<1)|(((bEP)&0x80)>>7))
/** convert from endpoint index to endpoint address */
#define IDX2EP(idx)	((((idx)<<7)&0x80)|(((idx)>>1)&0xF))



/**
	Local function to wait for a device interrupt (and clear it)

	@param [in]	dwIntr		Bitmask of interrupts to wait for
 */
static void Wait4DevInt(unsigned long dwIntr)
{
	// wait for specific interrupt
	while ((USB->USBDevIntSt & dwIntr) != dwIntr);
	// clear the interrupt bits
	USB->USBDevIntClr = dwIntr;
}


/**
	Local function to send a command to the USB protocol engine

	@param [in]	bCmd		Command to send
 */
static void USBHwCmd(unsigned char bCmd)
{
	// clear CDFULL/CCEMTY
	USB->USBDevIntClr = CDFULL | CCEMTY;
	// write command code
	USB->USBCmdCode = 0x00000500 | (bCmd << 16);
	Wait4DevInt(CCEMTY);
}


/**
	Local function to send a command + data to the USB protocol engine

	@param [in]	bCmd		Command to send
	@param [in]	bData		Data to send
 */
static void USBHwCmdWrite(unsigned char bCmd, unsigned short bData)
{
	// write command code
	USBHwCmd(bCmd);

	// write command data
	USB->USBCmdCode = 0x00000100 | (bData << 16);
	Wait4DevInt(CCEMTY);
}


/**
	Local function to send a command to the USB protocol engine and read data

	@param [in]	bCmd		Command to send

	@return the data
 */
static unsigned char USBHwCmdRead(unsigned char bCmd)
{
	// write command code
	USBHwCmd(bCmd);

	// get data
	USB->USBCmdCode = 0x00000200 | (bCmd << 16);
	Wait4DevInt(CDFULL);
	return USB->USBCmdData;
}


/**
	'Realizes' an endpoint, meaning that buffer space is reserved for
	it. An endpoint needs to be realised before it can be used.

	From experiments, it appears that a USB reset causes USBReEP to
	re-initialise to 3 (= just the control endpoints).
	However, a USB bus reset does not disturb the USBMaxPSize settings.

	@param [in]	idx			Endpoint index
	@param [in] wMaxPSize	Maximum packet size for this endpoint
 */
static void USBHwEPRealize(int idx, unsigned short wMaxPSize)
{
	USB->USBReEP |= (1 << idx);
	USB->USBEpInd = idx;
	USB->USBMaxPSize = wMaxPSize;
	Wait4DevInt(EP_RLZED);
}


/**
	Enables or disables an endpoint

	@param [in]	idx		Endpoint index
	@param [in]	fEnable	TRUE to enable, FALSE to disable
 */
static void USBHwEPEnable(int idx, BOOL fEnable)
{
	USBHwCmdWrite(CMD_EP_SET_STATUS | idx, fEnable ? 0 : EP_DA);
}


/**
	Configures an endpoint and enables it

	@param [in]	bEP				Endpoint number
	@param [in]	wMaxPacketSize	Maximum packet size for this EP
 */
void USBHwEPConfig(unsigned char bEP, unsigned short wMaxPacketSize)
{
	int idx;

	idx = EP2IDX(bEP);

	// realise EP
	USBHwEPRealize(idx, wMaxPacketSize);

	// enable EP
	USBHwEPEnable(idx, TRUE);
}


/**
	Registers an endpoint event callback

	@param [in]	bEP				Endpoint number
	@param [in]	pfnHandler		Callback function
 */
void USBHwRegisterEPIntHandler(unsigned char bEP, TFnEPIntHandler *pfnHandler)
{
	int idx;

	idx = EP2IDX(bEP);

	ASSERT(idx<32);

	/* add handler to list of EP handlers */
	_apfnEPIntHandlers[idx / 2] = pfnHandler;

	/* enable EP interrupt */
	USB->USBEpIntEn |= (1 << idx);
	USB->USBDevIntEn |= EP_SLOW;

	DBG("Registered handler for EP 0x%x\n", bEP);
}


/**
	Registers an device status callback

	@param [in]	pfnHandler	Callback function
 */
void USBHwRegisterDevIntHandler(TFnDevIntHandler *pfnHandler)
{
	_pfnDevIntHandler = pfnHandler;

	// enable device interrupt
	USB->USBDevIntEn |= DEV_STAT;

	DBG("Registered handler for device status\n");
}


/**
	Registers the frame callback

	@param [in]	pfnHandler	Callback function
 */
void USBHwRegisterFrameHandler(TFnFrameHandler *pfnHandler)
{
	_pfnFrameHandler = pfnHandler;

	// enable device interrupt
	USB->USBDevIntEn |= FRAME;

	DBG("Registered handler for frame\n");
}


/**
	Sets the USB address.

	@param [in]	bAddr		Device address to set
 */
void USBHwSetAddress(unsigned char bAddr)
{
	USBHwCmdWrite(CMD_DEV_SET_ADDRESS, DEV_EN | bAddr);
}


/**
	Connects or disconnects from the USB bus

	@param [in]	fConnect	If TRUE, connect, otherwise disconnect
 */
void USBHwConnect(BOOL fConnect)
{
	USBHwCmdWrite(CMD_DEV_STATUS, fConnect ? CON : 0);
}


/**
	Enables interrupt on NAK condition

	For IN endpoints a NAK is generated when the host wants to read data
	from the device, but none is available in the endpoint buffer.
	For OUT endpoints a NAK is generated when the host wants to write data
	to the device, but the endpoint buffer is still full.

	The endpoint interrupt handlers can distinguish regular (ACK) interrupts
	from NAK interrupt by checking the bits in their bEPStatus argument.

	@param [in]	bIntBits	Bitmap indicating which NAK interrupts to enable
 */
void USBHwNakIntEnable(unsigned char bIntBits)
{
	USBHwCmdWrite(CMD_DEV_SET_MODE, bIntBits);
}


/**
	Gets the status from a specific endpoint.

	@param [in]	bEP		Endpoint number
	@return Endpoint status byte (containing EP_STATUS_xxx bits)
 */
unsigned char	USBHwEPGetStatus(unsigned char bEP)
{
	int idx = EP2IDX(bEP);

	return USBHwCmdRead(CMD_EP_SELECT | idx);
}


/**
	Sets the stalled property of an endpoint

	@param [in]	bEP		Endpoint number
	@param [in]	fStall	TRUE to stall, FALSE to unstall
 */
void USBHwEPStall(unsigned char bEP, BOOL fStall)
{
	int idx = EP2IDX(bEP);

	USBHwCmdWrite(CMD_EP_SET_STATUS | idx, fStall ? EP_ST : 0);
}


/**
	Writes data to an endpoint buffer

	@param [in]	bEP		Endpoint number
	@param [in]	pbBuf	Endpoint data
	@param [in]	iLen	Number of bytes to write

	@return TRUE if the data was successfully written or <0 in case of error.
*/
int USBHwEPWrite(unsigned char bEP, unsigned char *pbBuf, int iLen)
{
	int idx;

	idx = EP2IDX(bEP);

	// set write enable for specific endpoint
	USB->USBCtrl = WR_EN | ((bEP & 0xF) << 2);

	// set packet length
	USB->USBTxPLen = iLen;

	// write data
	while (USB->USBCtrl & WR_EN) {
		USB->USBTxData = (pbBuf[3] << 24) | (pbBuf[2] << 16) | (pbBuf[1] << 8) | pbBuf[0];
		pbBuf += 4;
	}

	// select endpoint and validate buffer
	USBHwCmd(CMD_EP_SELECT | idx);
	USBHwCmd(CMD_EP_VALIDATE_BUFFER);

	return iLen;
}


/**
	Reads data from an endpoint buffer

	@param [in]	bEP		Endpoint number
	@param [in]	pbBuf	Endpoint data
	@param [in]	iMaxLen	Maximum number of bytes to read

	@return the number of bytes available in the EP (possibly more than iMaxLen),
	or <0 in case of error.
 */
int USBHwEPRead(unsigned char bEP, unsigned char *pbBuf, int iMaxLen)
{
	unsigned int i, idx;
	unsigned long	dwData, dwLen;

	idx = EP2IDX(bEP);

	// set read enable bit for specific endpoint
	USB->USBCtrl = RD_EN | ((bEP & 0xF) << 2);

	// wait for PKT_RDY
	do {
		dwLen = USB->USBRxPLen;
	} while ((dwLen & PKT_RDY) == 0);

	// packet valid?
	if ((dwLen & DV) == 0) {
		return -1;
	}

	// get length
	dwLen &= PKT_LNGTH_MASK;

	// get data
	dwData = 0;
	for (i = 0; i < dwLen; i++) {
		if ((i % 4) == 0) {
			dwData = USB->USBRxData;
		}
		if ((pbBuf != NULL) && ((int)i < iMaxLen)) {
			pbBuf[i] = dwData & 0xFF;
		}
		dwData >>= 8;
	}

	// make sure RD_EN is clear
	USB->USBCtrl = 0;

	// select endpoint and clear buffer
	USBHwCmd(CMD_EP_SELECT | idx);
	USBHwCmd(CMD_EP_CLEAR_BUFFER);

	return dwLen;
}


/**
	Sets the 'configured' state.

	All registered endpoints are 'realised' and enabled, and the
	'configured' bit is set in the device status register.

	@param [in]	fConfigured	If TRUE, configure device, else unconfigure
 */
void USBHwConfigDevice(BOOL fConfigured)
{
	// set configured bit
	USBHwCmdWrite(CMD_DEV_CONFIG, fConfigured ? CONF_DEVICE : 0);
}


/**
	USB interrupt handler

	@todo Get all 11 bits of frame number instead of just 8

	Endpoint interrupts are mapped to the slow interrupt
 */
void USBHwISR(void)
{
	unsigned long	dwStatus;
	unsigned long dwIntBit;
	unsigned char	bEPStat, bDevStat, bStat;
	int i;
	unsigned short	wFrame;

	// handle device interrupts
	dwStatus = USB->USBDevIntSt;

	// frame interrupt
	if (dwStatus & FRAME) {
		// clear int
		USB->USBDevIntClr = FRAME;
		// call handler
		if (_pfnFrameHandler != NULL) {
			wFrame = USBHwCmdRead(CMD_DEV_READ_CUR_FRAME_NR);
			_pfnFrameHandler(wFrame);
		}
	}

	// device status interrupt
	if (dwStatus & DEV_STAT) {
		/*	Clear DEV_STAT interrupt before reading DEV_STAT register.
			This prevents corrupted device status reads, see
			LPC2148 User manual revision 2, 25 july 2006.
		*/
		USB->USBDevIntClr = DEV_STAT;
		bDevStat = USBHwCmdRead(CMD_DEV_STATUS);
		if (bDevStat & (CON_CH | SUS_CH | RST)) {
			// convert device status into something HW independent
			bStat = ((bDevStat & CON) ? DEV_STATUS_CONNECT : 0) |
					((bDevStat & SUS) ? DEV_STATUS_SUSPEND : 0) |
					((bDevStat & RST) ? DEV_STATUS_RESET : 0);
			// call handler
			if (_pfnDevIntHandler != NULL) {
				_pfnDevIntHandler(bStat);
			}
		}
	}

	// endpoint interrupt
	if (dwStatus & EP_SLOW) {
		// clear EP_SLOW
		USB->USBDevIntClr = EP_SLOW;
		// check all endpoints
		for (i = 0; i < 32; i++) {
			dwIntBit = (1 << i);
			if (USB->USBEpIntSt & dwIntBit) {
				// clear int (and retrieve status)
				USB->USBEpIntClr = dwIntBit;
				Wait4DevInt(CDFULL);
				bEPStat = USB->USBCmdData;
				// convert EP pipe stat into something HW independent
				bStat = ((bEPStat & EPSTAT_FE) ? EP_STATUS_DATA : 0) |
						((bEPStat & EPSTAT_ST) ? EP_STATUS_STALLED : 0) |
						((bEPStat & EPSTAT_STP) ? EP_STATUS_SETUP : 0) |
						((bEPStat & EPSTAT_EPN) ? EP_STATUS_NACKED : 0) |
						((bEPStat & EPSTAT_PO) ? EP_STATUS_ERROR : 0);
				// call handler
				if (_apfnEPIntHandlers[i / 2] != NULL) {
					_apfnEPIntHandlers[i / 2](IDX2EP(i), bStat);
				}
			}
		}
	}
}



/**
	Initialises the USB hardware


	@return TRUE if the hardware was successfully initialised
 */
BOOL USBHwInit(void)
{
	// P2.9 -> USB_CONNECT
	PINCON->PINSEL4 &= ~0x000C0000;
	PINCON->PINSEL4 |= 0x00040000;

	// P1.18 -> USB_UP_LED
	// P1.30 -> VBUS
	PINCON->PINSEL3 &= ~0x30000030;
	PINCON->PINSEL3 |= 0x20000010;

	// P0.29 -> USB_D+
	// P0.30 -> USB_D-
	PINCON->PINSEL1 &= ~0x3C000000;
	PINCON->PINSEL1 |= 0x14000000;

	// enable PUSB
	SC->PCONP |= (1 << 31);

	USB->OTGClkCtrl = 0x12;	                  /* Dev clock, AHB clock enable  */
	while ((USB->OTGClkSt & 0x12) != 0x12);

	// disable/clear all interrupts for now
	USB->USBDevIntEn = 0;
	USB->USBDevIntClr = 0xFFFFFFFF;
	USB->USBDevIntPri = 0;

	USB->USBEpIntEn = 0;
	USB->USBEpIntClr = 0xFFFFFFFF;
	USB->USBEpIntPri = 0;

	// by default, only ACKs generate interrupts
	USBHwNakIntEnable(0);

	return TRUE;
}

