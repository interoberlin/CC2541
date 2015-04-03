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
* Dateiname:       RF.c
*
* Programmier:      
*                  Dirk Krüger , Dmytro Krush
*                  <dkrueger@hsu-hh.de>
*             	   (+49)40-6541-2636
*                  Modified:
*                  Dmytro Krush
*                  dmytro.krush@hsu-hh.de
* Version:         $Revision$
*
* Erstellt am:     $Date$
*/

/*! \file          RF.c
 *  \brief         
 *
 *   History:      For simplification was used some parts of TI examples.
 */


#include "RF.h"
#include <string.h>
static uint8 _gain;

/*
*********************************************************************************************************
*                                               LOKALE VARIABLEN
*********************************************************************************************************
*/
/** Pointer auf internen Buffer für Sendedaten */
uint8 *RF_TxBuffer_ptr;

/** Berechneter CRC beim Empfang */
uint32 RF_CalcRecCRC;

/** Empfangener CRC beim Empfang */
uint32 RF_RecCRC;

/** Berechneter CRC beim Senden */
uint32 RF_CalcSendCRC;

/** RF Zustandsvariable */
enum RF_States RF_State;

/** Lokaler Zeiger auf die aktuelle PSDU */
uint8 *RF_PSDUPtr;

/** aktuelle PSDU-Länge*/
uint8 RF_PSDULength;

/** aktueller Ausführzeitpunkt */
uint16 RF_ExecTime;

uint16 RF_MaxReceiveDuration;

/** aktuelle Ausgangsleistung */
uint8 RF_RadioPower;

/** aktuelle Frequenz */
uint8 RF_RadioChannel;

/** aktuelle Datenrate */
uint8 RF_RadioDataRate;

/** aktuelle Präambellänge */
uint8 RF_RadioPreambleLength;

/** aktueller Start-of-Frame Delimiter */
uint16 RF_RadioSFD[2];

/** Länge des aktuellen Start-of-Frame Delimiters */
uint8 RF_RadioSFDLength;

/** Semaphore des Physical Layers */
static volatile unsigned int RF_Lock;

/** Pointer auf Structur zur Übergabe von Rückgabeinformationen an den MAC-Layer */
RFReturnPtr RF_ReturnPtr;

RFSendStructPtr RF_SendStructPtr;
RFReceiveStructPtr RF_ReceiveStructPtr;
uint8 SyncAttempFailed     = TRUE;

uint32 crc32Lookup[256] ;     // CRC look up table, BIT CRC takes too much timr and cannot be used. Standart CRC cannot be used because of different polynom and different position of the CRC Checksum in a packet
static uint8 RXPacketLength = 11;
//
#pragma vector=RFERR_VECTOR
__interrupt void RF_ErrorInterrupt(void)
{
    
   if (RFERRF & TXUNDERF_FLAG) {
      RFForceShutdown();              // flush all commands immidiately
      SEND_LLE_CMD(CMD_TXFIFO_RESET);  // Reset TX FiFo
      GPIO6     = 0;                       // claer trigger pin
      RF_State  = RF_IDLE_STATE;
      RF_SendStructPtr->SendCBF();     // Send opearation completed, activate callback
   } 
   if (RFERRF & RXTXABO_FLAG) {
      SEND_LLE_CMD(CMD_TXFIFO_RESET);  // Reset TX FiFo
      SEND_LLE_CMD(CMD_RXFIFO_RESET);  // Reset RX FiFo
      RF_ReceiveTimeoutHandler();
   } 
   if (RFERRF & RXOVERF_FLAG) {
      SEND_LLE_CMD(CMD_RXFIFO_RESET);  // Reset RX FiFo
      RF_ReceiveTimeoutHandler();
   }  
                          // Deactive Transmitter Imidiately
   RFERRF = 0x00;
}

#pragma vector=RF_VECTOR
__interrupt void RF_Interrupt(void) {
 
   if (RFIRQF1 & RFIRQF1_RXOK) {
      RF_ReceiveStructPtr->ReceiveTime = GetGlobalTimeLow();
      GPIO6     = 0;
      TimerClearCallBack();
      RFForceShutdown();
      RF_ReceivedPPDUHandler();
   } 

}
/*******************************************************************************
* @fn          	setQuadratureSkew
*
* @brief       	sets a quadrature skew setting
*
* @param          Skew value
*
* @return         none
*/

void setQuadratureSkew(uint8 skew)  
{
   ACOMPQS    = skew; 
}//setQuadratureSkew


/*******************************************************************************
* @fn          	RFInit
*
* @brief       	Initialise the RF front
                  set RAM Registers!
*                 clears RX and TX FIFO Buffer
*
* @return         none
*/
void RFInit(){
   /* Clear radio memory.
   *  The RAM registers don't have a default value set after power on */
   memset((void*)RFCORE_RAM_PAGE, 0, RFCORE_RAM_PAGE_SZ);
   //RF_ReturnPtr = return_ptr;
   // Reset RXFIFO and TXFIFO.
   SEND_LLE_CMD(CMD_RXFIFO_RESET);
   SEND_LLE_CMD(CMD_TXFIFO_RESET);
   RFFCFG    = 0x33;               // Auto Commit TX FIFO and RX FIFO 
   RFTXFTHRS = 0x10;               //TX FIFO Threshold at 16 Bytes
   RFRXFTHRS = 0x10;               //TX FIFO Threshold at 16 Bytes
   TXPOWER   = TXPOWER_0_DBM; 
   return;
   }//RFInit

/*******************************************************************************
* @fn          	RFConfig
*
* @brief       	Used to send a command to RF Front
*
* @param          uint8 modulationSheme Modilation Shema which should be used. Available modes : 
         GFSK_250kbps_160khz
         GFSK_1Mbps_160khz
         GFSK_1Mbps_250khz 
         GFSK_2Mbps_320khz
         GFSK_2Mbps_500khz
         MSK_250kbps
         MSK_500kbps 
        
* @param          uint8 SyncLength Length of the syncword
* @param          uint8 usedConfiguration Task Configuration,
* @param          uint8 tx transmitter or receiver 
* @return         none
*/
void RFConfig(uint8 modulationSheme, uint8 SyncLength, uint8 usedConfiguration){//, uint8 tx) {
   //Eigene Konfiguration
  // Change this parameters if needed!
   /* RAM based registers. These are cleared in RFInit(). */ 
   //while (!SEMAPHORE0);                        // wait on semaphore 0 is set (take)
   // all protected with SEMAPHORE1
   // Packet config
   EA = 0;
   LLECTRL = 0x00;
   MaskAllInterrupts();
   PRF.PKT_CONF.ADDR_LEN           = 0x00;//!!(usedConfiguration & PKT_CONF_ADDR);
   PRF.PKT_CONF.AGC_EN             = 0;
   PRF.PKT_CONF.START_TONE         = 0;
   // Task Config
   PRF.TASK_CONF.MODE              = 0x00;
   PRF.TASK_CONF.REPEAT            = 0;        // Repeat. // was 1
   PRF.TASK_CONF.START_CONF        = 0;        // Start immediately after command.
   PRF.TASK_CONF.STOP_CONF         = 0;        // Don't stop on timer 2 event 2.
   PRF.TASK_CONF.TX_ON_CC_CONF     = 1;
   PRF.TASK_CONF.REPEAT_CONF       = 0;
   //PRF_PKT_CONF                    = 0x02;     //  for USE AGC set to 0x02 (Automatic Gain Controll) NO ADDRESS Length, Ordinary Transmion                 // wait on semaphore 1 is set (take)
   // all protected with SEMAPHORE1
   PRF.FIFO_CONF.AUTOFLUSH_IGN     = 0;        // No Flush duplicate packets.
   PRF.FIFO_CONF.AUTOFLUSH_CRC     = 0;        // do not Flush packets with CRC error.
   PRF.FIFO_CONF.AUTOFLUSH_EMPTY   = 0;        // do not Flush empty packets.
   PRF.FIFO_CONF.RX_STATUS_CONF    = 0;        // append status information in FIFO.
   PRF.FIFO_CONF.RX_ADDR_CONF      = 0;        // address byte in Rx FIFO.
   PRF.FIFO_CONF.TX_ADDR_CONF      = 0;        // Read address from PRF.ADDR_ENTRY[0].ADDRESS.
   PRF.CRC_LEN                     = 0;        // Number of CRC bytes, permitted values are 0..4
   PRF.CRC_INIT[0]                 = 0xFF;     // CRC init Byte 0
   PRF.CRC_INIT[1]                 = 0xFF;     // CRC init Byte 1
   PRF.CRC_INIT[2]                 = 0xFF;     // CRC init Byte 2
   PRF.CRC_INIT[3]                 = 0xFF;     // CRC init Byte 3

   PRF_CHAN                        = 21;        // Channel 21 = 2400 MHz
   PRF_RSSI_LIMIT                  = 0x7F;      //0x00;
   PRF_RSSI_COUNT                  = 0x0000;    // 2 bytes

   PRF_W_INIT                      = 0xFF;
   PRF_RETRANS_CNT                 = 0x00;
   PRF_TX_DELAY                    = 0x0000;    // 2 bytes
   PRF_RETRANS_DELAY               = 0x0000;    // 2 bytes
   PRF_SEARCH_TIME                 = 0x0000;    // 2 bytes
   PRF_RX_TX_TIME                  = 0x0000;    // 2 bytes
   PRF_TX_RX_TIME                  = 0x0000;    // 2 bytes
   PRF.ADDR_ENTRY[0].CONF.ENA0     = 1;
   PRF_N_TX                        = 0x00;
   PRF_LAST_RSSI                   = 0x00;
   //PRF_LAST_DCOFF                  = 0x00000000; // 4 bytes
   PRF_RADIO_CONF                  = 0x00;
   PRF_ENDCAUSE                    = 0x00;    

   //SEMAPHORE1                      = 1;        // set Semaphore 1 (release)

   TXCTRL                          = 0x19;       // TX Settings.
   TXFILTCFG                       = 0x03;       // Set Tx Filter bandwidth.
   IVCTRL                          = 0x1B;       // Analog Control. 
   FRMCTRL0                        = 0x43;//0x41| (!!(usedConfiguration & PKT_CONF_ADDR)); // Amplitude weight in frequency offset compensation (assuming MSB first).
   //MDMCTRL0                        = (modulationSheme) | (!!(usedConfiguration & PKT_CONF_PHASE_INV));   // Set Modulation and data rate.
   MDMCTRL0                        = modulationSheme;// | PKT_CONF_PHASE_INV;   // Set Modilation
   MDMCTRL1                        = 0x48;      // Correlation threshold.
   MDMCTRL2                        = 0xA3;      //Synch: MSB first, Preamble for Freq.-Offset estimation, Preamble is set to 0101 0101  4 Byte Preamble // Pre inv.to first SyncBit, 

   // Set RSSI mode to peak detect after sync.
   MDMCTRL3                        = 0x63;      //no ErrorBits in Sync, Ramping of DAC, no Sniff, peak detect after sync.
   ADCTEST0                        = 0x10;      // was 0x66 
   
   MDMTEST0                        = 0xA2;      //RSSI: two 21us Windows Mean, DC-Block 16 Samples, DC-Cancellation enabled (Last config was A2!!!)
   MDMTEST1                        = 0x0A;      //DC-Delay = 5, IF = 1MHz, Baseband Freq = 0MHz (should be 0A)
   // Set gain  (will set correct gain even for 2 MBPS).
   RFSetGain(HAL_RF_GAIN_HIGH);

   // Program sync word length and configure single sync word.    
   // Set Default Sync Word lengths and the sync word itself
   RFSetSyncWord(SyncLength, 0x00, 0x3E, 0x94, 0x59, 0x00, 0x3E, 0x94, 0x59);
   
   RFSetWhitening(0x00);
   RFSetWhiteningRegister (0x65);
   //Quadrature Skew Setting (see User Guide Table 23-24)
   setQuadratureSkew(0x16);
   FREQTUNE                       = 0x0F;         //no Freq. Tune   
   RFERRIE                        = 1;            // activate perefirie interrupts 
   LLECTRL                        |= 0x01;
   RFIRQM1                        |= RFIRQF1_RXOK;// Activate "Packet Received Correctly" Interrupt
   S1CON                          = 0;            // clear cpu flag
   RFIRQF1                        = 0x00;         // Clear all RF core interrupts Flags
   RFIRQF0                        = 0x00;         // Clear all RF core interrupts Flags
   IEN2                           |= IEN2_RFIE;   // activate Radio Interrupts
   EA                             = 1;            // Enable all interrupts
   return;
}//RFConfig


/**
 *   
 *
 *   \param ptr_ch Zeiger auf eine Variable, in der die Frequenz
 *                 geschrieben werden soll
 *   \return SUCCESS, d.h. TRUE oder FALSE
 */
uint8  RFSetSAMSFD (void)
{
  if(RF_State == RF_IDLE_STATE)                               /* ? IDLE ?*/
        {
            MDMCTRL2 = 0xA3;         //Synch: MSB first, Preamble for Freq.-Offset estimation, Pre inv.to first SyncBit, 4 Byte Preamble 3 byte sync
            RF_ReturnPtr->RF_ErrorType = RF_NO_ERROR;
            return TRUE;
        }
        else                                                                    /* Radio ist nicht Idle -> Cmd nicht annehmen und State zurückgeben */
        {
            RF_ReturnPtr->RF_ErrorType = RF_FALSE_STATE;
            return FALSE;
        }
} //  RFSetSAMSFD



/**
 *  
 *
 *   \param ptr_ch Zeiger auf eine Variable, in der die Frequenz
 *                 geschrieben werden soll
 *   \return SUCCESS, d.h. TRUE oder FALSE
 */
uint8  RFSetBeaconSFD (void)
{
   if(RF_State == RF_IDLE_STATE)                               /* ? IDLE ?*/
   {
    MDMCTRL2 = 0xA3;          //Synch: MSB first, Preamble for Freq.-Offset estimation, Pre inv.to first SyncBit, 1 Byte Preamble 3 byte sync (not by me)
    RF_ReturnPtr->RF_ErrorType = RF_NO_ERROR;
    return TRUE;
   }
   else                                                                    /* Radio ist nicht Idle -> Cmd nicht annehmen und State zurückgeben */
   {
    RF_ReturnPtr->RF_ErrorType = RF_FALSE_STATE;
    return FALSE;
   }

} //  RFIetBeaconSFD

/**
 * @fn RF_TransmittedPPDUHandler(void) Load data to be transmitted
 *  
 * @param NONE
 * @return NONE
 */
void RF_TransmittedPPDUHandler(void) {
   /**
    Transmitter starts, now we need to wait on activate an Underflow interrupt!!!
    At first clear interrupt flags.
   */   
   GPIO6 = 1;                 // pin as trigger for Real Time Spectrum Analyser
   RFERRF = 0x00;             // Clear all interrupts
   RFERRM = TXUNDERF_FLAG;             // TX underflow Flag, inform that all data was send successfully
   SEND_LLE_CMD(CMD_TX_FIFO_TEST);
   return;
}



/**
 *   Globale Funktion zum Einschalten des Empfängers zu einem definierten Zeitpunkt
 *   Sowohl der Funktionsaufruf als auch eine vorhergehende Paramteraktualisierung
 *   muss auf den höheren Layern innerhalb eines Atomic-Blocks stattfinden, um
 *   eine Korruption der Übergabeparameter durch Interrupts zu verhindern.
 *
 *   \param *psdu_ptr Zeiger auf zu sendende MAC Protocol Data Unit (PSDU)
 *   \param psdu_length Länge der PSDU
 *   \param ppdu_send_time Sendezeitpunkt der Physical Layer Protocol Data Unit (PPDU)
 *   \return SUCCESS, d.h. TRUE oder FALSE
 */

uint8 RFReceiveMsg (RFReceiveStructPtr rsp)
{
    //uint32 tmp1;
        if(RF_State == RF_IDLE_STATE)                               /* ? Idle ?*/
        {
            //tmp1 = rsp->ReceiveStartTime - GetGlobalTimeLow();
            //if((tmp1 > RECEIVER_STARTUP_TIME) && (tmp1 < 0xF000))               /* ? Reicht die Zeit zum Anschalten des Empfängers ?*/
            //{
               RF_PSDUPtr    = rsp->PSDU;                                /* Parameterübernahme in die lokalen Variablen */
               if((RF_RadioChannel == CONFIG_CHANNEL_1) || (RF_RadioChannel == CONFIG_CHANNEL_2))
               {
                  RXPacketLength = 11;    // 88 BITS  Config Telegram DSLOT (normal)
               }
               else
               {
                  RXPacketLength = 15;    // 120 Bits, 80 BitPayload + 24 Bits SyncWord + 2Bit UL group + 6 Bit Cell ID (Downlink Telegramm DSLOT Extended)
               }   
               RF_ExecTime             = rsp->ReceiveStartTime;
               RF_MaxReceiveDuration   = rsp->MaxReceiveDuration;
               RF_ReceiveStructPtr     = rsp;
               PRF.ADDR_ENTRY[0].RXLENGTH = RXPacketLength;   
               RF_ReturnPtr->RF_ErrorType = RF_NO_ERROR;
               RF_State = RF_RX_PACKET_STATE;
               RF_ReturnPtr->RF_State = RF_RX_PACKET_STATE;
               MCUTimer2CompareCC1(RF_ReceiveStartedHandler, (RF_ExecTime - (uint16)RXPREPARETIME)); // start receive operation after on difined time 
               RFIRQM1 |= RFIRQF1_RXOK;                                                              // Activate "Packet Received Correctly" Interrupt
               S1CON    = 0;
               RFIRQF1  = 0x00;                                                                       // Clear all RF core interrupts Flags
               RFIRQF0  = 0x00;                                                                       // Clear all RF core interrupts Flags
               return TRUE;
        }
        else          
        {
            RF_ReturnPtr->RF_ErrorType = RF_FALSE_STATE;
            return FALSE;
        }
} // RFReceiveMsg


/**
 *   Callback-Funktion, welche durch den Timer-Interrupt-Handler
 *   aufgerufen wird.
 *
 *   \param NONE
 *   \return NONE
 */

 //   PRF_CHAN = (*frequency) + 21;
 //   FREQCTRL = (*frequency) + 21;
//#define TRIGGER GPIO6
void RF_ReceiveStartedHandler (void)
{   
    GPIO6     = 1;
    SEND_LLE_CMD(CMD_RX);
    while (!(LLESTAT& 0x03));
    MCUTimer2CompareCC0(RF_ReceiveTimeoutHandler, RF_MaxReceiveDuration + DSLOT_DURATION);     // now + 142 = 180 mu nach dem start, System starts to count time! Means from 
    return;
  
  
} // RF_ReceiveStartedHandler


//+ DSLOT_EXT_DURATION 

/*******************************************************************************
* @fn           RF_ReceivedPPDUHandler
*
* @brief        Read the first available packet in the RXFIFO.
*
* @param        uint16_t event_time: 
*
* @return       void
*/

void RF_ReceivedPPDUHandler (void) {

    if (RFFSTATUS & RXAVAIL) {                  // Something is saved in the RX buffer
        RF_PSDULength = RFD;
        RF_ReceiveStructPtr->ReceivedPacketStatus = RF_RX_PACKET_OK;
        RF_ReceiveStructPtr->Rssi = RFGetLastRssi(); // Get RSSI from RF front
           if( (RF_PSDULength < PHYLAYER_RX_PSDU_MIN_LENGTH) || (RF_PSDULength > (PHYLAYER_RX_PSDU_MAX_LENGTH+4)))
           {
              RF_ReceiveStructPtr->ReceivedPacketStatus = RF_ZERO_PACKET_LENGTH;
           }else{
               RF_PSDULength-=4;                       // Received Packet always has the length of the packet! It should be loaded from the FIFO first.
           //Empfangenen CRC aus FIFO auslesen
               ((uint8*)(&RF_RecCRC))[3] = RFD;
               ((uint8*)(&RF_RecCRC))[2] = RFD;
               ((uint8*)(&RF_RecCRC))[1] = RFD;
               ((uint8*)(&RF_RecCRC))[0] = RFD;
               //Empangene Daten aus FIFO auslesen
               while(RF_PSDULength--)
               {
                 
                   *RF_PSDUPtr = RFD;
                   RF_PSDUPtr++;
               }
               //CRC-Berechnung
               if((RF_RadioChannel == CONFIG_CHANNEL_1) || (RF_RadioChannel == CONFIG_CHANNEL_2))
               {
                 RF_CalcRecCRC = crc32_1byte((RF_PSDUPtr - 7), 7);
               }
               else
               {
                 RF_CalcRecCRC = crc32_1byte((RF_PSDUPtr - 11), 10);            // PNO spezification is not match here. CRC should be computed for whole Packet of 11 bytes.   
               }                                                                // here only first 10 bytes are used. In old version it has been done probably because timing problems (CRC took to much time)
               if(RF_RecCRC != RF_CalcRecCRC)
                 RF_ReceiveStructPtr->ReceivedPacketStatus = RF_CRC_ERROR;
           }
               RF_State = RF_IDLE_STATE;
               RF_ReturnPtr->RF_State = RF_IDLE_STATE;
               SEND_LLE_CMD(CMD_RXFIFO_RESET);               // Clear Receiver Buffer.
               RF_ReceiveStructPtr->ReceiveCBF();
           //}
        }
} //RF_ReceivedPPDUHandler

/**
 *   Globale Funktion zum Senden eines PHY-Pakets zu einer definierten Zeit.
 *   Sowohl der Funktionsaufruf als auch eine vorhergehende Paramteraktualisierung
 *   muss auf den höheren Layern innerhalb eines Atomic-Blocks stattfinden, um
 *   eine Korruption der Übergabeparameter durch Interrupts zu verhindern.
 *
 *   \param *psdu_ptr Zeiger auf zu sendende PHY Service Data Unit (PSDU)
 *   \param psdu_length Länge der PSDU
 *   \param ppdu_send_time Sendezeitpunkt der Physical Layer Protocol Data Unit (PPDU)
 *   \return SUCCESS, d.h. TRUE oder FALSE
 */

uint8 RFSendMsg (RFSendStructPtr ssp)
{
    if(RF_State == RF_IDLE_STATE)                               /* ? Idle ?*/
        {
            RF_PSDUPtr    = ssp->PSDU;                                    /* Parameterübernahme in die lokalen Variablen */
            RF_PSDULength = ssp->PSDULength;
            RF_ExecTime   = ssp->SendTime;
            RF_SendStructPtr = ssp;
            if((RF_PSDULength > PHYLAYER_TX_PSDU_MIN_LENGTH) && (RF_PSDULength < PHYLAYER_TX_PSDU_MAX_LENGTH))  {        /* ? Reicht die Zeit zum Anschalten des Senders ? und ist die Paketlänge ok?*/
                  RF_CalcSendCRC = crc32_1byte(RF_PSDUPtr, RF_PSDULength);
                  if(LONG_UPLINK_SYNC)
                  {
                     RFD = (((uint8*)(&RF_CalcSendCRC))[3]);
                     RFD = (((uint8*)(&RF_CalcSendCRC))[2]);
                     RFD = (((uint8*)(&RF_CalcSendCRC))[1]);
                     RFD = (((uint8*)(&RF_CalcSendCRC))[0]);
                  for(uint8 SendDataIdx = 0; SendDataIdx < (RF_PSDULength); SendDataIdx++)
                  {
                     RFD = (*(RF_PSDUPtr + SendDataIdx));
                  }
                  /*while(RF_PSDULength--) // Optimisation
                  {
                     RFD = (*(RF_PSDUPtr++));
                  }*/
                     RFD = 0x55;                                                                     // Guard Byte needed, transmitter generate an Underflow interrupt 8 mu earlier as all data were send. (1 Byte == 8 mu)
                     MCUTimer2CompareCC0(RF_TransmittedPPDUHandler, (RF_ExecTime - TXPREPARETIME));  // Set Timer-Interrupt on Transmitter Event 
                  }
                  else
                  {
                   //packetlen = (RF_PSDULength + 4 + 2) * 8;
                   RFD = 0xB0 + (((uint8*)(&RF_CalcSendCRC))[3]>>4);
                   RFD = (((uint8*)(&RF_CalcSendCRC))[3]<<4) + (((uint8*)(&RF_CalcSendCRC))[2]>>4);
                   RFD = (((uint8*)(&RF_CalcSendCRC))[2]<<4) + (((uint8*)(&RF_CalcSendCRC))[1]>>4);
                   RFD = (((uint8*)(&RF_CalcSendCRC))[1]<<4) + (((uint8*)(&RF_CalcSendCRC))[0]>>4);
                   RFD = (((uint8*)(&RF_CalcSendCRC))[0]<<4) + (*RF_PSDUPtr >> 4);
                   for(uint8 SendDataIdx = 0; SendDataIdx < (RF_PSDULength - 1); SendDataIdx++)
                   {
                     RFD = (*(RF_PSDUPtr + SendDataIdx) << 4) + (*(RF_PSDUPtr + SendDataIdx + 1) >> 4);
                   }
                   /*while(--RF_PSDULength) // Optimisation
                   {
                   
                     RFD = ((*(RF_PSDUPtr)) << 4) + ((*(RF_PSDUPtr + 1)) >> 4);
                     RF_PSDUPtr++
                   }*/
                   RFD = 0x55;                                                                     // Guard Byte needed, transmitter generate an Underflow interrupt 8 mu earlier as all data were send. (1 Byte == 8 mu)
                   MCUTimer2CompareCC0(RF_TransmittedPPDUHandler, (RF_ExecTime - TXPREPARETIME));  // Set Timer-Interrupt on Transmitter Event 
                  }
                  RF_ReturnPtr->RF_ErrorType = RF_NO_ERROR;
                  RF_State = RF_TX_PACKET_STATE;
                  RF_ReturnPtr->RF_State = RF_TX_PACKET_STATE;
                  return TRUE;
            }
            else                                                                /* Zeit reicht nicht -> Timeout*/
            {
                RF_ReturnPtr->RF_ErrorType = RF_TIMEOUT;
                return FALSE;
            }
        }
        else                                                                    /* Radio ist nicht Idle -> Cmd nicht annehmen und State zurückgeben */
        {
            RF_ReturnPtr->RF_ErrorType = RF_FALSE_STATE;
            return FALSE;
        }
} // RFSendMsg

/*******************************************************************************
* @fn           halRfEnableInterrupt
*
* @brief        Enable RF interrupt source.
*
* @param        uint8 mask: The rf flag-mask to be enabled.
*
* @return       void
*/
void RFEnableInterrupt(uint8 mask) {
    // Enable selected flags in mask.
    RFIRQM1 |= mask;
    IEN2    |= IEN2_RFIE;
    EA       = 1;            // Global Interrupts Enable
    return;
} // RFEnableInterrupt


// /*******************************************************************************
// * @fn          	UnmaskInterrupts
// *
// * @brief       	Unmask all interrupt sources
// *
// * @param          none
// *
// * @return         none
// */

 void MaskAllInterrupts() {
    // // Interrupt Mask Properties
   RFIRQM0    = 0x00;               //ALL Interrupt masked
   RFIRQM1    = 0x00;               //ALL Interrupt masked
   RFERRM     = 0x00;               //ALL Error Interrupt masked
   IEN2       &= ~IEN2_RFIE;
 }//UnmaskInterrupts

/*******************************************************************************
* @fn          	RFSetFrequency
*
* @brief       	Used to set the RF Frequency
*
* @param          uint8 frequency frequency to be set
*
* @return         none
*/
void RFSetFrequency(uint8* frequency) {
    RFDisableRadio(FORCE);
    PRF_CHAN = (*frequency) + 21;
    RF_RadioChannel = (*frequency);
    RFEnableRadio();           // Activate Radio
    return;
}//RFSetFrequency
/*******************************************************************************
* @fn          	RFSetFrequency
*
* @brief       	Used for setting the maximal Packet length to be received
*
* @param          uint8 Length of the packet
*
* @return         none
*/
void RFSetPacketLength(uint8* Length) {
   RFRXFTHRS = (*Length);
   return;
}//

/*******************************************************************************
* @fn          	RFSetDirectFrequency
*
* @brief       	Used to set the RF Frequency (for Rx and TX test Commands (our case))
*
* @param          uint16 frequency frequency to be set
*
* @return         none
*/
void RFSetDirectFrequency(uint8* frequency) {
    RFDisableRadio(FORCE);
    FREQCTRL = (*frequency) + 21;
    RF_RadioChannel = (*frequency);
    RFEnableRadio();           // Activate Radio
    return;
}//RFSetDirectFrequency

/*******************************************************************************
* @fn          	RFSetGain
*
* @brief       	Used to set the RF Gain
*
* @param          uint8 gainMode the gain cannot be set exlicitly, user should select one of the gived modes
*                 Possible mode : HAL_RF_GAIN_HIGH
*                               : HAL_RF_GAIN_LOW
*
* @return         none
*/
void RFSetGain(uint8 gainMode) {
    uint8 modulation = GET_MODULATION();
    _gain = gainMode;
    
    if (IS_2MBPS(modulation)) {
        // Always use this gain for 2 MBPS
        LNAGAIN   = 0x7F;     // LNA gain.
        RXCTRL    = 0x29;     // Receiver currents.
        FSCTRL    = 0x5A;     // Prescaler and mixer currents.
        _gain = HAL_RF_GAIN_2MBPS;
    }
    else if (gainMode == HAL_RF_GAIN_HIGH) {
        LNAGAIN   = 0x7F;     // LNA gain.
        RXCTRL    = 0x3F;     // Receiver currents.
        FSCTRL    = 0x5A;     // Prescaler and mixer currents.
    }
    else if (gainMode == HAL_RF_GAIN_LOW) {
        LNAGAIN   = 0x3A;     // LNA gain.
        RXCTRL    = 0x33;     // Receiver currents.
        FSCTRL    = 0x55;     // Prescaler and mixer currents.
    }
    return;
}//RFSetGain
/*******************************************************************************
* @fn          	RFEnableRadio
*
* @brief       	Used to senable RF Front
*
*
* @return         none
*/
#pragma inline
void RFEnableRadio(void) {
    LLECTRL |= 0x01;
}//RFEnableRadio

/*******************************************************************************
* @fn          	RFForceShutdown
*
* @brief       	Force shutdown of the RF Front
*
* @param        void
*
* @return       void
*/
#pragma inline
void RFForceShutdown(void) {
    RFIRQM1  = 0x00; // Disable RX interrupts
    S1CON    = 0;    // Clear RF general interrupt flag
    RFIRQF1  = 0x00; // Clear all RF core interrupts Flags
    RFIRQF0  = 0x00; // Clear all RF core interrupts Flags
    RFERRF   = 0x00; // Clear all RF core error interrupts Flags
    SEND_LLE_CMD(CMD_SHUTDOWN); 
    return;
}//RFStartRx


/*******************************************************************************
* @fn          	RFDisableRadio
*
* @brief       	Used to deactivate the radio
*
* @param          uint8 forceReset
*
* @return         none
*/
uint8 RFDisableRadio(uint8 forceReset) {
   uint8 lleStat = LLESTAT;
   // Check if radio is idle.
   if((LLECTRL != 0) && !(lleStat & LLESTAT_LLE_IDLE)) {
     if(forceReset) {
         // End active task.
         SEND_LLE_CMD(CMD_STOP);
            /* 
            *  A CMD_SHUTDOWN will stop any active tasks immediately, 
            *  even aborting ongoing transmission or reception. But CMD_STOP
            *  is used here as it ends the operation more graceully, waiting 
            *  for ongoing TX/RX operations to complete. This might increase 
            *  the duration of the function to set the LLE in reset.
            */
         
         while(!(RFIRQF1 & RFIRQF1_TASKDONE));
     }
     else {
         // Radio is active, return the read LLE status (LLESTAT value might have changed).
         return lleStat;
     }
   }
   // LLE is idle or in reset -> Disable link layer engine.
   LLECTRL &= ~0x01;
   // Clear RF interrupts.
   RFIRQF1  = 0x00; // Clear all RF core interrupts Flags
   RFIRQF0  = 0x00; // Clear all RF core interrupts Flags
   RFERRF   = 0x00; // Clear all RF core error interrupts Flags
   S1CON = 0;       // Clear RF general interrupt flag
   // Set to a undefined value in enum type end cause.
   PRF.ENDCAUSE = TASK_UNDEF;
   return SUCCESS;
}//RFDisableRadio


/*******************************************************************************
* @fn          	RFGetLastRssi (TI original function)
*
* @brief       	used to get Last RSSI
*
* @return         int8 RSSI Value
*/
int8 RFGetLastRssi(void) {
  // Variable to store RSSI value.
    int8 rssi_val;
    // Read RSSI value for last received packet.
    rssi_val=PRF_LAST_RSSI;
    if(rssi_val == -128) {
        return -128;
    }

    if(_gain==HAL_RF_GAIN_2MBPS) {
        rssi_val -= RSSI_OFFSET_HIGH_GAIN;
    }
    else if (_gain==HAL_RF_GAIN_HIGH) {
        rssi_val -= RSSI_OFFSET_HIGH_GAIN;   
    }
    else {
       rssi_val -= RSSI_OFFSET_LOW_GAIN;
    }
    return rssi_val;
}//RFGetLastRssi

/*******************************************************************************
* @fn          	RFGetRssi  (TI original function)
*
* @brief       	used to get RSSI Value with given Accuracy
* @param          uint8 rssi_accuracy
*
* @return         int8 RSSI Value
*/
int8 RFGetRssi(uint8 rssi_accuracy) {

 // Check if receiver is running.
    if(!(RFSTAT&RFSTAT_RX_ACTIVE)) {
        // The receiver is NOT running as required. 
        return RUN_ERROR;
    }
  
    // Set RSSI accuracy.
    MDMTEST0 &= ~MDMTEST0_RSSI_ACC;
    MDMTEST0 |= (rssi_accuracy << 5);
  
    // Variable to store RSSI value.
    signed char rssi_val;

    // Read RSSI value (signed 2's complement)
    rssi_val = RSSI;
  
    // If measurement is unvalid, -128 (0x80) is returned.
    if(rssi_val == -128) {
        return -128;
    }
  
    // The reference level depends on the LNA gain setting. 
    // Subtract the correct RSSI OFFSET.
    if(_gain==HAL_RF_GAIN_2MBPS) {
        if((LLESTAT & 0x10) == 0x10) {
            // AGC algorithm has reduced the front-end gain
            rssi_val -= RSSI_OFFSET_LOW_GAIN;
        }
        else {
            rssi_val -= RSSI_OFFSET_HIGH_GAIN;
        }
    }
    else if (_gain==HAL_RF_GAIN_HIGH) {
        rssi_val -= RSSI_OFFSET_HIGH_GAIN;   
    }
    else {
        rssi_val -= RSSI_OFFSET_LOW_GAIN;
    }

    return rssi_val;
}//RFGetRssi


/*******************************************************************************
* @fn          	RFSetSyncWord
*
* @brief       	Set sync word.
*
* @param        uint8 sw_conf: Configuration of the Sync, how many bytes is the sync word, etc.

* @param        uint8 sw0: Primary sync word part 1.
* @param        uint8 sw1: Primary sync word part 2.
* @param        uint8 sw2: Primary sync word part 3.
* @param        uint8 sw3: Primary sync word part 4.

* @param        uint8 sw4: Secondary sync word part 1.
* @param        uint8 sw5: Secondary sync word part 2.
* @param        uint8 sw6: Secondary sync word part 3.
* @param        uint8 sw7: Secondary sync word part 4.
*
* @return       void
*/
void RFSetSyncWord(uint8 sw_conf, uint8 sw0, uint8 sw1, uint8 sw2, uint8 sw3, uint8 sw4, uint8 sw5, uint8 sw6, uint8 sw7) {
   SW_CONF = sw_conf;
   SW0 = sw0;
   SW1 = sw1;
   SW2 = sw2;
   SW3 = sw3;
   SW4 = sw4;
   SW5 = sw5;
   SW6 = sw6;
   SW7 = sw7;
}//RFSetSyncWord


/*******************************************************************************
* @fn          	RFSetWhiteningRegister
*
* @brief       	Sets PN7 whitening register. Must be made by MCU by testing operation
*
* @param        uint8 whRegValue: set whitener generator polynom register
*
* @return       void
*/
void RFSetWhiteningRegister (uint8 Value) {
    BSP_W = Value;
}//RFSetWhiteningRegister

/*******************************************************************************
* @fn          	halRfSetWhitening  (TI original function)
*
* @brief       	Enable or disable whitening
*
* @param        uint8 whitening: Chosen whitening 
* @param        uint8 whRegValue: set whitener generator polynom register
*
* @return       void
*/
void RFSetWhitening(uint8 whitening) {

    if (whitening & BSP_MODE_W_PN9_EN) {
        BSP_MODE = 0x02;
        PRF.W_INIT = 0x80;
    }else if (whitening & BSP_MODE_W_PN7_EN) {
        BSP_MODE = 0x01;
        PRF.W_INIT = 0x80;
    }
    else {
        BSP_MODE = 0x00;
    }
    return;
}//RFSetWhitening
/*******************************************************************************
* @fn          	make_crc_table
*
* @brief       	Calculate a LUT Table for CRC32 computation
* @return       256 x 32 bit Table
*/
void make_crc_table(void)
   {
	uint32 bit, crc;
	for (uint16 i=0; i<256; i++) {

		crc=(uint32)i;
		crc<<= 24;
		for (uint8 j=0; j<8; j++) {
			bit = crc & 0x80000000;
			crc<<= 1;
			if (bit) crc^= CRC_POLYNOM;
		}
		crc32Lookup[i]= crc;
	}
   }
/*******************************************************************************
* @fn          	crc32_1byte
*
* @brief       	Calculate a CRC32 for a packet using a LOOK UP Table
*
* @param        uint8* data: data for CRC computation
* @param        uint8 length: number of bytes
*
* @return       uint32 CRC32 result
*/

uint32 crc32_1byte(uint8* Data_Ptr, uint8 Data_Length)
{
  uint32 crc32 = 0xffffffff; // same as previousCrc32 ^ 0xFFFFFFFF
  while (Data_Length--) {
    crc32 = (crc32Lookup[((((uint8*)(&crc32))[3]) ^ (*Data_Ptr++)) & 0xff]) ^(crc32 << 8);
  }

  return ~crc32; // same as crc ^ 0xFFFFFFFF
}


/*******************************************************************************
* @fn           RfReadRxPacket
*
* @brief        Read the first available packet in the RXFIFO.
*
* @param        uint8 *pBuf: The byte array to store the packet in.
* @param        uint8 maxLen: The maximum packet size allowed in pBuf.
*
* @return       void
*/

uint8 RFReadRxPacket(uint8 *pBuf, uint8 maxLen) {
    uint8 length;
    uint8 i;
    length = RFD;
    if (length > maxLen) return 0;
    for (i = 0; i < length; i++) {
      uint8 byte = RFD;
       if (i < maxLen) {
                pBuf[i] = byte;
      }
   }
   return length;
}
/*******************************************************************************
* @fn           RF_ReceiveTimeoutHandler
*
* @brief        Clears all all timers, Set packet status calls callback function
*
* @param        none
* @param        none
*
* @return       void
*/
void RF_ReceiveTimeoutHandler(void)
{
    
    if(RF_State == RF_RX_PACKET_STATE)
    {
      RFForceShutdown();                                                        // deactivate immidiately
      TimerClearCallBack();                                                     // clear all timers  
      RF_ReceiveStructPtr->ReceivedPacketStatus = RF_RECEIVE_TIMEOUT;           // Set status to time out
      RF_State = RF_IDLE_STATE;                                                 // Set RF state to idle  
      RF_ReturnPtr->RF_State = RF_IDLE_STATE;                                   // Set RF state to idle (inform upper layer)
      RF_ReceiveStructPtr->ReceiveCBF();                                        // Callback 
    }                                                                         
}