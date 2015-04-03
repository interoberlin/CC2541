/* $Id*/

/*
*********************************************************************************************************
* Department of Electrical Measurement Engineering
* Helmut-Schmidt-University
* Holstenhofweg 85
* Germany - 22043 Hamburg
*
* (c) Helmut-Schmidt-University, Hamburg
*     Dpt.of Electrical Measurement Engineering
*     All rights reserved
*
* Dateiname:       TestRec.c
*
* Programmier:      
*                   Dmytro Krush
*                   dmytro.krush@hsu-hh.de
*             	   (+49)40-6541-2636
*                  
* Version:         $Revision$
*
* Erstellt am:     $Date$
*/

/*! \file          TestRec.c
 *  \brief         
 *
 *   History:      For simplification was used some parts of TI examples.
 */

#ifndef CC2541
#define CC2541
#endif

#include "ioCC2541.h"
#include "hal_types.h"
#include "MCUTimer2.h"
#include "RF.h"
#define TEST_MODE                 0x00 
#define SYNCLENGTH                0x18                                          // no Dual-RX, 24Bit Sync. 
#define CONFIGURATION             PKT_CONF_CC25_VARLEN_NOADDR                   // User Configuration interprete as binary : 00 00 0(PKT_CONF_PHASE_INV) 0(PKT_CONF_ADDR) 00(PKT_CONF_MODE)
#define ByteReceivedThreshold     0x03                                          // Data send Threshold. This Threshold is the number of bytes in the Buffer. If the system write more bytes 

#define RECEIVE_DURATION          500;
#define CONFIG_CHANNEL_1          1
#define CONFIG_CHANNEL_2          83

#define DSLOT_PAYLOAD_LENGTH      11
#define SSLOT_PAYLOAD_LENGTH      2
static uint8 Receiver= 0x00;                                                    

void TransmitMessage(void);

/**
Define Internal variables
*/

// RX/TX Sturkturen

RFSendStruct MACLayer_pss;
RFReceiveStruct MACLayer_prs;


volatile static PACKET_REC_STATE PACKET;     // Wariable für die zustände
//static SYSTEM_STATUS  S_STATUS;

static bool   attemp_completed= TRUE;

uint8 ReceivedByte[16];
uint8 RFStatus    = 0x00;
uint8 RFRxLen    = 0x00;
uint8 LLE_STATUS = 0x00;
uint8 COREStatus = 0x00;
uint8 PacketLength = 16;
uint8 rf_active_int0 = 0x00;
uint8 rf_active_int1 = 0x00;
uint8 CurrentChannel = 0x00;	

bool crc_table_computed = FALSE;

//static SPISendStruct SPISndStrc;
uint8 tmp_byte;

// /***********************************************************************************
// * @fn          rf_isr
// *
// * @brief       Interrupt service routine for RF core. Handles TASKDONE
// *
// * @param       void
// *
// * @return      void
// */

void tryToStart(void);
/*******************************************************************************
* @fn          	main()
*
* @brief       	Main function. 
*        
* @return         success 
*/
/** CRC 32 look up table*/

uint32 numberOfAttemps = 0;

 int main(void) {
   setHighSpeedClock();                                 // set 32 MHz clock frequency
   configureGPIOPin();                                  // Prepare control pins
   MCUTimer2_Init();                                    // Initialise RF front
   RFInit();                                            // Initialisation of the radio, Set frequency to 2450 
   RFConfig(GFSK_1Mbps_160khz, SYNCLENGTH, CONFIGURATION);    // Configuration of the Radio for needed task
   RFEnableRadio();                                     // Activate Radio
   StartTimer();
   CurrentChannel = 21;                                 // set to 2400 MHz
   tryToStart();                                        // activate function first time
   while(1) {
      if (attemp_completed){                            // each attemp increment the counter
         numberOfAttemps++;                       
         attemp_completed = FALSE;                      
         if (numberOfAttemps > 10) {                    // after 10 successfull receive/send deactive interrupts <=> deactivate the system
           EA = 0;
         }
      }
   }
 }

void tryToStart(void) {  
  attemp_completed = TRUE;    // opearation completed (donk care successfull or not) 
  if (Receiver == 0x01 && MACLayer_prs.ReceivedPacketStatus == RF_RX_PACKET_OK) {       // Last operation was successfull
     Receiver = 0x00;
    if(CurrentChannel == CONFIG_CHANNEL_1) CurrentChannel = CONFIG_CHANNEL_2;           // set Channel, channels 1 and 83 will be used here.
    else                                   CurrentChannel = CONFIG_CHANNEL_1;
    
    MACLayer_prs.ReceiveStartTime   = GetGlobalTimeLow() + 5*RECEIVE_DURATION;          // set the time of receive operation will start 
    MACLayer_prs.MaxReceiveDuration = MACLayer_prs.ReceiveStartTime + RECEIVE_DURATION; // set the time for timeout event
    RFSetFrequency(&CurrentChannel);                                                    // set receive channel
    MACLayer_prs.ReceiveCBF         = tryToStart;
    RFReceiveMsg(&MACLayer_prs);                                                        // activate Receiveoperation handler
  } 
  else if (MACLayer_prs.ReceivedPacketStatus == RF_RECEIVE_TIMEOUT){                    // Receive operation timeout
     Receiver = 0x01;                                                                   // try to receive ones more
     MACLayer_prs.ReceivedPacketStatus = RF_RX_PACKET_OK;                               // reset the flag
      attemp_completed = FALSE;                                                         // Timeout will not be used as attemp  
     tryToStart();                                                                      // restart function
  }
  else// Test Transmitter
  {
    Receiver = 0x01;                                                                    // next step system will work as a receiver
    MACLayer_pss.PSDU[0]      = 'h';
    MACLayer_pss.PSDU[1]      = 'e';
    MACLayer_pss.PSDU[2]      = 'l';
    MACLayer_pss.PSDU[3]      = 'l';  
    MACLayer_pss.PSDU[4]      = 'o';
    MACLayer_pss.PSDU[5]      = ' ';
    MACLayer_pss.PSDU[6]      = 'w';
    MACLayer_pss.PSDU[7]      = 'o';
    MACLayer_pss.PSDU[8]      = 'r';
    MACLayer_pss.PSDU[9]      = 'l';
    MACLayer_pss.PSDU[10]     = 'd';
    MACLayer_pss.PSDULength   = DSLOT_PAYLOAD_LENGTH;                                   // 15 bytes, 11 payload 4 crc
    CurrentChannel++;                                                                   // set current channel
    if (CurrentChannel > 82) {                                                          // if channel is out of range, reset to 2402
        CurrentChannel = 2;
    }
    RFSetDirectFrequency(&CurrentChannel);
    MACLayer_pss.SendTime     = GetGlobalTimeLow() + 5*RECEIVE_DURATION;                // set the time of receive operation will start 
    MACLayer_pss.SendCBF      = tryToStart;
    RFSendMsg(&MACLayer_pss);   
  }
}
