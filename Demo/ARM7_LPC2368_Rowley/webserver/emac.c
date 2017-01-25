/******************************************************************
 *****                                                        *****
 *****  Name: cs8900.c                                        *****
 *****  Ver.: 1.0                                             *****
 *****  Date: 07/05/2001                                      *****
 *****  Auth: Andreas Dannenberg                              *****
 *****        HTWK Leipzig                                    *****
 *****        university of applied sciences                  *****
 *****        Germany                                         *****
 *****  Func: ethernet packet-driver for use with LAN-        *****
 *****        controller CS8900 from Crystal/Cirrus Logic     *****
 *****                                                        *****
 *****  Keil: Module modified for use with Philips            *****
 *****        LPC2378 EMAC Ethernet controller                *****
 *****                                                        *****
 ******************************************************************/

/* Adapted from file originally written by Andreas Dannenberg.  Supplied with permission. */

#include "FreeRTOS.h"
#include "semphr.h" 
#include "task.h"
#include "emac.h"

/* The semaphore used to wake the uIP task when data arives. */
SemaphoreHandle_t xEMACSemaphore = NULL;

static unsigned short *rptr;
static unsigned short *tptr;

// easyWEB internal function
// help function to swap the byte order of a WORD

static unsigned short SwapBytes(unsigned short Data)
{
  return (Data >> 8) | (Data << 8);
}

// Keil: function added to write PHY
void write_PHY (int PhyReg, int Value)
{
  unsigned int tout;

  MADR = DP83848C_DEF_ADR | PhyReg;
  MWTD = Value;

  /* Wait utill operation completed */
  tout = 0;
  for (tout = 0; tout < MII_WR_TOUT; tout++) {
    if ((MIND & MIND_BUSY) == 0) {
      break;
    }
  }
}


// Keil: function added to read PHY
unsigned short read_PHY (unsigned char PhyReg) 
{
  unsigned int tout;

  MADR = DP83848C_DEF_ADR | PhyReg;
  MCMD = MCMD_READ;

  /* Wait until operation completed */
  tout = 0;
  for (tout = 0; tout < MII_RD_TOUT; tout++) {
    if ((MIND & MIND_BUSY) == 0) {
      break;
    }
  }
  MCMD = 0;
  return (MRDD);
}


// Keil: function added to initialize Rx Descriptors
void rx_descr_init (void)
{
  unsigned int i;

  for (i = 0; i < NUM_RX_FRAG; i++) {
    RX_DESC_PACKET(i)  = RX_BUF(i);
    RX_DESC_CTRL(i)    = RCTRL_INT | (ETH_FRAG_SIZE-1);
    RX_STAT_INFO(i)    = 0;
    RX_STAT_HASHCRC(i) = 0;
  }

  /* Set EMAC Receive Descriptor Registers. */
  RxDescriptor    = RX_DESC_BASE;
  RxStatus        = RX_STAT_BASE;
  RxDescriptorNumber = NUM_RX_FRAG-1;

  /* Rx Descriptors Point to 0 */
  RxConsumeIndex  = 0;
}


// Keil: function added to initialize Tx Descriptors
void tx_descr_init (void) {
  unsigned int i;

  for (i = 0; i < NUM_TX_FRAG; i++) {
    TX_DESC_PACKET(i) = TX_BUF(i);
    TX_DESC_CTRL(i)   = 0;
    TX_STAT_INFO(i)   = 0;
  }

  /* Set EMAC Transmit Descriptor Registers. */
  TxDescriptor    = TX_DESC_BASE;
  TxStatus        = TX_STAT_BASE;
  TxDescriptorNumber = NUM_TX_FRAG-1;

  /* Tx Descriptors Point to 0 */
  TxProduceIndex  = 0;
}


// configure port-pins for use with LAN-controller,
// reset it and send the configuration-sequence

portBASE_TYPE Init_EMAC(void)
{
portBASE_TYPE xReturn = pdPASS;
static portBASE_TYPE xAttempt = 0;
// Keil: function modified to access the EMAC
// Initializes the EMAC ethernet controller
  volatile unsigned int regv,tout,id1,id2;

  /* Enable P1 Ethernet Pins. */
  PINSEL2 = configPINSEL2_VALUE;
  PINSEL3 = (PINSEL3 & ~0x0000000F) | 0x00000005;

  /* Power Up the EMAC controller. */
  PCONP |= 0x40000000;
  vTaskDelay( 10 );

  /* Reset all EMAC internal modules. */
  MAC1 = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | MAC1_RES_MCS_RX |
             MAC1_SIM_RES | MAC1_SOFT_RES;
  Command = CR_REG_RES | CR_TX_RES | CR_RX_RES;

  /* A short delay after reset. */
  vTaskDelay( 10 );

  /* Initialize MAC control registers. */
  MAC1 = MAC1_PASS_ALL;
  MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
  MAXF = ETH_MAX_FLEN;
  CLRT = CLRT_DEF;
  IPGR = IPGR_DEF;

  /* Enable Reduced MII interface. */
  Command = CR_RMII | CR_PASS_RUNT_FRM;

  /* Reset Reduced MII Logic. */
  SUPP = SUPP_RES_RMII;
  SUPP = 0;

  /* Put the DP83848C in reset mode */
  write_PHY (PHY_REG_BMCR, 0x8000);
  write_PHY (PHY_REG_BMCR, 0x8000);

  /* Wait for hardware reset to end. */
  for (tout = 0; tout < 100; tout++) {
    vTaskDelay( 200 );
    regv = read_PHY (PHY_REG_BMCR);
    if (!(regv & 0x8000)) {
      /* Reset complete */
      break;
    }
  }

  /* Check if this is a DP83848C PHY. */
  id1 = read_PHY (PHY_REG_IDR1);
  id2 = read_PHY (PHY_REG_IDR2);
  if (((id1 << 16) | (id2 & 0xFFF0)) == DP83848C_ID) {
    /* Configure the PHY device */

    /* Use autonegotiation about the link speed. */
    write_PHY (PHY_REG_BMCR, PHY_AUTO_NEG);
    /* Wait to complete Auto_Negotiation. */
    for (tout = 0; tout < 10; tout++) {
      vTaskDelay( 200 );
      regv = read_PHY (PHY_REG_BMSR);
      if (regv & 0x0020) {
        /* Autonegotiation Complete. */
        break;
      }
    }
  }
  else
  {
    xReturn = pdFAIL;
  }

  /* Check the link status. */
  if( xReturn == pdPASS )
  {
    xReturn = pdFAIL;
    for (tout = 0; tout < 10; tout++) {
      vTaskDelay( 200 );
      regv = read_PHY (PHY_REG_STS);
      if (regv & 0x0001) {
        /* Link is on. */
        xReturn = pdPASS;
        break;
      }
    }
  }

  if( xReturn == pdPASS )
  {
    /* Configure Full/Half Duplex mode. */
    if (regv & 0x0004) {
      /* Full duplex is enabled. */
      MAC2    |= MAC2_FULL_DUP;
      Command |= CR_FULL_DUP;
      IPGT     = IPGT_FULL_DUP;
    }
    else {
      /* Half duplex mode. */
      IPGT = IPGT_HALF_DUP;
    }

    /* Configure 100MBit/10MBit mode. */
    if (regv & 0x0002) {
      /* 10MBit mode. */
      SUPP = 0;
    }
    else {
      /* 100MBit mode. */
      SUPP = SUPP_SPEED;
    }

    /* Set the Ethernet MAC Address registers */
    SA0 = (emacETHADDR0 << 8) | emacETHADDR1;
    SA1 = (emacETHADDR2 << 8) | emacETHADDR3;
    SA2 = (emacETHADDR4 << 8) | emacETHADDR5;

    /* Initialize Tx and Rx DMA Descriptors */
    rx_descr_init ();
    tx_descr_init ();

    /* Receive Broadcast and Perfect Match Packets */
    RxFilterCtrl = RFC_UCAST_EN | RFC_BCAST_EN | RFC_PERFECT_EN;

    /* Create the semaphore used ot wake the uIP task. */
    vSemaphoreCreateBinary( xEMACSemaphore );

    /* Reset all interrupts */
    IntClear  = 0xFFFF;

    /* Enable receive and transmit mode of MAC Ethernet core */
    Command  |= (CR_RX_EN | CR_TX_EN);
    MAC1     |= MAC1_REC_EN;
  }

  return xReturn;
}


// reads a word in little-endian byte order from RX_BUFFER

unsigned short ReadFrame_EMAC(void)
{
  return (*rptr++);
}

// reads a word in big-endian byte order from RX_FRAME_PORT
// (useful to avoid permanent byte-swapping while reading
// TCP/IP-data)

unsigned short ReadFrameBE_EMAC(void)
{
  unsigned short ReturnValue;

  ReturnValue = SwapBytes (*rptr++);
  return (ReturnValue);
}


// copies bytes from frame port to MCU-memory
// NOTES: * an odd number of byte may only be transfered
//          if the frame is read to the end!
//        * MCU-memory MUST start at word-boundary

void CopyFromFrame_EMAC(void *Dest, unsigned short Size)
{
  unsigned short * piDest;                       // Keil: Pointer added to correct expression

  piDest = Dest;                                 // Keil: Line added
  while (Size > 1) {
    *piDest++ = ReadFrame_EMAC();
    Size -= 2;
  }
  
  if (Size) {                                         // check for leftover byte...
    *(unsigned char *)piDest = (char)ReadFrame_EMAC();// the LAN-Controller will return 0
  }                                                   // for the highbyte
}

// does a dummy read on frame-I/O-port
// NOTE: only an even number of bytes is read!

void DummyReadFrame_EMAC(unsigned short Size)    // discards an EVEN number of bytes
{                                                // from RX-fifo
  while (Size > 1) {
    ReadFrame_EMAC();
    Size -= 2;
  }
}

// Reads the length of the received ethernet frame and checks if the 
// destination address is a broadcast message or not
// returns the frame length
unsigned short StartReadFrame(void) {
  unsigned short RxLen;
  unsigned int idx;

  idx = RxConsumeIndex;
  RxLen = (RX_STAT_INFO(idx) & RINFO_SIZE) - 3;
  rptr = (unsigned short *)RX_DESC_PACKET(idx);
  return(RxLen);
}

void EndReadFrame(void) {
  unsigned int idx;

  /* DMA free packet. */
  idx = RxConsumeIndex;

  if (++idx == NUM_RX_FRAG)
    idx = 0;

  RxConsumeIndex = idx;
}

unsigned int CheckFrameReceived(void) {             // Packet received ?

  if (RxProduceIndex != RxConsumeIndex)     // more packets received ?
    return(1);
  else 
    return(0);
}

unsigned int uiGetEMACRxData( unsigned char *ucBuffer )
{
unsigned int uiLen = 0;

    if( RxProduceIndex != RxConsumeIndex )
    {
        uiLen = StartReadFrame();
        CopyFromFrame_EMAC( ucBuffer, uiLen );
        EndReadFrame();
    }

    return uiLen;
}

// requests space in EMAC memory for storing an outgoing frame

void RequestSend(void)
{
  unsigned int idx;

  idx  = TxProduceIndex;
  tptr = (unsigned short *)TX_DESC_PACKET(idx);
}

// check if ethernet controller is ready to accept the
// frame we want to send

unsigned int Rdy4Tx(void)
{
  return (1);   // the ethernet controller transmits much faster
}               // than the CPU can load its buffers


// writes a word in little-endian byte order to TX_BUFFER
void WriteFrame_EMAC(unsigned short Data)
{
  *tptr++ = Data;
}

// copies bytes from MCU-memory to frame port
// NOTES: * an odd number of byte may only be transfered
//          if the frame is written to the end!
//        * MCU-memory MUST start at word-boundary

void CopyToFrame_EMAC(void *Source, unsigned int Size)
{
  unsigned short * piSource;

  piSource = Source;
  Size = (Size + 1) & 0xFFFE;    // round Size up to next even number
  while (Size > 0) {
    WriteFrame_EMAC(*piSource++);
    Size -= 2;
  }
}

void DoSend_EMAC(unsigned short FrameSize)
{
  unsigned int idx;

  idx = TxProduceIndex;
  TX_DESC_CTRL(idx) = FrameSize | TCTRL_LAST;
  if (++idx == NUM_TX_FRAG) idx = 0;
  TxProduceIndex = idx;
}

