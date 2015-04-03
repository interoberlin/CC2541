#ifndef PHY_H
#define PHY_H

#include "ioCC2541.h"
#include "hal_defs.h"
#include "hal_types.h"
#include "prop_regs.h"
#include "MCUTimer2.h"

/*******************************************************************************
* TYPEDEFS
*/

/*******************************************************************************
* CONSTANTS AND DEFINES
*/
// #ifndef SUCCESS
// #define SUCCESS 0
// #else
// #warning "Macro SUCCESS already defined"
// #endif
#define LONG_UPLINK_SYNC                1
#define PKT   P2_1     
#define GPIO6 P2_0
#define FIFO  P2_2
#define SPI_SEND P1_3
#define PPDU_PREPARATION_TIME          40  /* 40us sind nötig bevor der Sendebefel ausgeführt wird */
#define CONFIG_CHANNEL_1               1
#define CONFIG_CHANNEL_2               83
#define PHYLAYER_RX_PSDU_MIN_LENGTH    7   // Anpassung an neuen MACLayer vorher 6 - 
#define PHYLAYER_RX_PSDU_MAX_LENGTH    11  // Anpassung an neuen MACLayer vorher 255
#define PHYLAYER_TX_PSDU_MIN_LENGTH    0   // Anpassung an neuen MACLayer vorher 3
#define PHYLAYER_TX_PSDU_MAX_LENGTH    16  // Anpassung an neuen MACLayer vorher 11
#define RXPREPARETIME                  180 //50 //180 + 50 forrune time
#define TXPREPARETIME                  180

#define FRAME_DURATION                 2448  // 2304 us = 16 * 144 Framedauer USE FOR TIMER OVERFLOW-COUNT OVERFLOW
#define DSLOT_DURATION                 144   // 144 us DSlot USE FOR TIMER PERIOD
#define SSLOT_DURATION                 72    // 72 us SSlot USE FOR TIMER COMPARE 2
#define DSLOT_EXT_DURATION             184   // 184 us Extendet DSLOT

#define RECEIVER_STARTUP_TIME          15  //15 us
#define WORST_CASE_INT_RUN_TIME        40 //40 us

#define RADIO_WAKEUP_TIME           (640 + WORST_CASE_INT_RUN_TIME)   /* Zeit vom EventTimerInterrupt bis der Idle-Zustand erreicht wird */
#define RADIO_MIN_SLEEP_TIME        (RADIO_WAKEUP_TIME +  80)         /* Summe der  Transition-Zeiten (IDLE -> Sleep und zurück) */
#ifndef CRC_POLYNOM
#define CRC_POLYNOM                 0x04C11DB7
#endif
#ifndef CRC_POLYNOM_MIRROW
#define CRC_POLYNOM_MIRROW          0xEDB88320
#endif
#define DIRECT_CONVERSION_OFFSET    1
#define LONG_UPLINK_SYNC            1

/* Define RSSI Offsets*/
#define RSSI_OFFSET_HIGH_GAIN       107
#define RSSI_OFFSET_LOW_GAIN        98
#define RSSI_OFFSET_AGC_HIGH_GAIN   99
#define RSSI_OFFSET_AGC_LOW_GAIN    79

// Available modulation formats.
#define GFSK_1Mbps_250khz     0x04
#define GFSK_2Mbps_500khz     0x06

#define GFSK_250kbps_160khz   0x08
#define GFSK_1Mbps_160khz     0x0C

#define GFSK_2Mbps_320khz     0x0E

#define MSK_250kbps           0x10
#define MSK_500kbps           0x12

#define RSSI_MEAN_5_3mu      0x00
#define RSSI_MEAN_2x5_3mu    0x01
#define RSSI_MEAN_4x5_3mu    0x03
#define RSSI_MEAN_21mu       0x04
#define RSSI_MEAN_2x21mu     0x05 
#define RSSI_MEAN_4x21mu     0x07 

#define RUN_SUCCESS 0
#define RUN_ERROR   9

#define TX 1
#define RX 0

// Define force parameter.
#define FORCE   1
#define ATTEMPT 0

#define GET_MODULATION() ((MDMCTRL0 & 0x1E))
#define IS_2MBPS(mod) ((mod) == GFSK_2Mbps_320khz || (mod) == GFSK_2Mbps_500khz)
// Definitions for packet configuration
#define PKT_CONF_MODE               0x03 // Bit mask for mode bits
#define PKT_CONF_MODE_BASIC_FIXLEN  0x00
#define PKT_CONF_MODE_BASIC_VARLEN  0x01
#define PKT_CONF_MODE_AUTO_9BIT     0x02
#define PKT_CONF_MODE_AUTO_10BIT    0x03
#define PKT_CONF_ADDR               0x04 // Bit mask for address byte
#define PKT_CONF_PHASE_INV          0x08 // Bit mask for phase invert
#define PKT_CONF_INC_CRC            0x10 // Bit mask for including SW in CRC

// Compatibility modes
#define PKT_CONF_NRF (PKT_CONF_MODE_AUTO_9BIT | PKT_CONF_ADDR | PKT_CONF_INC_CRC)
#define PKT_CONF_NRF_COMPAT (PKT_CONF_MODE_BASIC_FIXLEN | PKT_CONF_ADDR | PKT_CONF_INC_CRC)
#define PKT_CONF_CC25_FIXLEN_NOADDR (PKT_CONF_MODE_BASIC_FIXLEN | PKT_CONF_PHASE_INV)
#define PKT_CONF_CC25_FIXLEN_ADDR (PKT_CONF_MODE_BASIC_FIXLEN | PKT_CONF_ADDR | PKT_CONF_PHASE_INV)
#define PKT_CONF_CC25_VARLEN_NOADDR (PKT_CONF_MODE_BASIC_VARLEN | PKT_CONF_PHASE_INV)
#define PKT_CONF_CC25_VARLEN_ADDR (PKT_CONF_MODE_BASIC_VARLEN | PKT_CONF_ADDR | PKT_CONF_PHASE_INV)

// Definitions for address configurations
#define ADDR_CONF_RX_AUTO_SW1       0x19
#define ADDR_CONF_RX_AUTO_SW2       0x1A
#define ADDR_CONF_RX_BASIC_SW1      0x01
#define ADDR_CONF_RX_BASIC_SW2      0x02
#define ADDR_CONF_TX_AUTO           0x18
#define ADDR_CONF_TX_BASIC          0x00

// TXPOWER values

#define TXPOWER_0_DBM           0xE1
#define TXPOWER_MINUS_2_DBM     0xD1
#define TXPOWER_MINUS_4_DBM     0xC1
#define TXPOWER_MINUS_6_DBM     0xB1
#define TXPOWER_MINUS_8_DBM     0xA1
#define TXPOWER_MINUS_10_DBM    0x91
#define TXPOWER_MINUS_12_DBM    0x81
#define TXPOWER_MINUS_14_DBM    0x71
#define TXPOWER_MINUS_16_DBM    0x61
#define TXPOWER_MINUS_18_DBM    0x51
#define TXPOWER_MINUS_20_DBM    0x41


// Bitmask for radio register RFIRQF1.
#define RFIRQF1_PINGRSP             0x80
#define RFIRQF1_TASKDONE            0x40
#define RFIRQF1_TXDONE              0x20
#define RFIRQF1_RXEMPTY             0x10
#define RFIRQF1_RXIGNORED           0x08
#define RFIRQF1_RXNOK               0x04
#define RFIRQF1_TXFLUSHED           0x02
#define RFIRQF1_RXOK                0x01

// Bitmask for radio register RFSTAT.
#define RFSTAT_MOD_UNDERFLOW        0x80
#define RFSTAT_DEM_STATUS           0x60
#define RFSTAT_SFD                  0x10
#define RFSTAT_CAL_RUNNING          0x08
#define RFSTAT_LOCK_STATUS          0x04
#define RFSTAT_TX_ACTIVE            0x02
#define RFSTAT_RX_ACTIVE            0x01

// Bitmask for radio register LLESTAT.
#define LLESTAT_AGC_LOWGAIN         0x10
#define LLESTAT_WAIT_T2E1           0x08
#define LLESTAT_LLE_IDLE            0x04
#define LLESTAT_SYNC_SEARCH         0x02
#define LLESTAT_VCO_ON              0x01

// Bitmask for radio register SW_CONF
#define SW_CONF_DUAL_RX     0x80
#define SW_CONF_SW_RX       0x20
#define SW_CONF_SW_LEN      0x1F

// Bitmask for radio register MDMCTRL1
#define MDMCTRL1_FOC_MODE   0xC0
#define MDMCTRL1_CORR_THR   0x1F

// Bitmask for radio register RFFSTATUS
#define TXAVAIL             0x80
#define TXFEMPTY            0x40
#define TXDTHEX             0x20
#define TXFFULL             0x10
#define RXAVAIL             0x08
#define RXFEMPTY            0x04
#define RXDTHEX             0x02
#define RXFFULL             0x01

// // Bitmask for radio register RFERRM


// #define RXFIFOFULL 0x40  //0 R/W RX FIFO is full when trying to store received data.
                     // //0: Interrupt disabled
                     // //1: Interrupt enabled
// #define LLEERR    0x20        //0 R/W LLE command or parameter error
                     // //0: Interrupt disabled
                     // //1: Interrupt enabled
#define RXTXABO   0x10//0 R/W Receive or transmit operation aborted
                     // //0: Interrupt disabled
                     // //1: Interrupt enabled
// #define RXOVERF   0x08 //0 R/W Rx FIFO overflow
                     // //0: Interrupt disabled
                     // //1: Interrupt enabled
// #define TXOVERF   0x04 //0 R/W Tx FIFO overflow
                     // //0: Interrupt disabled
                     // //1: Interrupt enabled
// #define RXUNDERF  0x02 //0 R/W Rx FIFO underflow
                     // //0: Interrupt disabled
                     // //1: Interrupt enabled
// #define TXUNDERF  0x01 //0 R/W Tx FIFO underflow
                     // //0: Interrupt disabled
                     // //1: Interrupt enabled


// Bitmask for radio register RFIRQM0
#define RXTHSHUP 0x08   //0 R/W0 Rx FIFO goes above its upper threshold.
                        // 0: No interrupt pending
                        // 1: Interrupt pending
#define TXTHSHUP 0x04   //0 R/W0 Tx FIFO goes above its upper threshold.
                        // 0: No interrupt pending
                        // 1: Interrupt pending
#define RXTHSHDN 0x02   //0 R/W0 Rx FIFO goes below its lower threshold.
                        // 0: No interrupt pending
                        // 1: Interrupt pending
#define TXTHSHDN 0x01   //0 R/W0 Tx FIFO goes below its lower threshold.
                        // 0: No interrupt pending
                        // 1: Interrupt pending

// Bitmask for radio register MDMTEST0
#define MDMTEST0_RSSI_ACC        0xE0
#define MDMTEST0_DC_BLOCK_LENGTH 0x0C
#define MDMTEST0_DC_BLOCK_MODE   0x03

// IP0 (0xA9) – Interrupt Priority 0
#define IP0_ST_P0INT_WDT                0x20
#define IP0_ENC_T4_P1INT                0x10
#define IP0_T3_I2C                      0x08
#define IP0_URX0_T2_UTX0                0x04
#define IP0_ADC_T1_P2INT_USB            0x02
#define IP0_RFERR_RF_DMA                0x01

// IP1 (0xB9) – Interrupt Priority 0
#define IP1_ST_P0INT_WDT                0x20
#define IP1_ENC_T4_P1INT                0x10
#define IP1_T3_I2C                      0x08
#define IP1_URX0_T2_UTX0                0x04
#define IP1_ADC_T1_P2INT_USB            0x02
#define IP1_RFERR_RF_DMA                0x01

// LLECTRL (0x61B1) – LLE Control
#define LLECTRL_LLE_MODE_SEL            0x06
#define LLECTRL_LLE_EN                  0x01

// RFC_OBS_CTRL0 (0x61AE) – RF Observation Mux Control 0
#define RFC_OBS_POL0            0x40
#define RFC_OBS_MUX0            0x3F
    #define RFC_SNIFF_DATA          0x07
    #define RFC_SNIFF_CLK           0x08
    #define TX_ACTIVE               0x09
    #define RX_ACTIVE               0x0A
    #define VCO_ON                  0x0B
    #define SYNCH_SEARCH            0x0C
    #define LLE_IDLE                0x0D
    #define WAIT_T2E1               0x0E
    #define AGC_LOWGAIN             0x0F
    #define FSC_LOCK                0x13
    #define PA_PD                   0x1B
    #define LNAMIX_PD               0x2C
    #define DEMO_SYNC_FOUND         0x30
    #define MOD_SYNC_SENT           0x31


// RSSI accuracy setting definitions
#define ONE_5_33_US_AVERAGE      0x00
#define TWO_5_33_US_AVERAGE      0x01
#define FOUR_5_33_US_AVERAGE     0x03
#define ONE_21_3_US_AVERAGE      0x04
#define TWO_21_3_US_AVERAGE      0x05
#define FOUR_21_3_US_AVERAGE     0x07

// SFR Register Descriptions

                   

// Gain modes
#define HAL_RF_GAIN_LOW                     0
#define HAL_RF_GAIN_HIGH                    1
#define HAL_RF_GAIN_2MBPS                   2

// Default Packet format values. 
#define LENGTH                  15
#define CRCLEN                  2

#define SEND_LLE_CMD(cmd)      st(while(RFST != 0); RFST = (cmd); while(RFST != 0);)
#define SEND_FIFO_CMD(cmd)     st(RFST = (cmd);)
            
enum RF_States{
    RF_IDLE_STATE,
    RF_SLEEP_STATE,
    RF_TX_PACKET_STATE,
    RF_RX_PACKET_STATE
};

enum RF_ErrorTypes{
    RF_NO_ERROR,
    RF_BUSY,
    RF_TIMEOUT,
    RF_OUT_OF_RANGE,
    RF_FALSE_STATE
};

enum RF_ReceivedPacketStatus{
    RF_RX_PACKET_OK,
    RF_CRC_ERROR,
    RF_ZERO_PACKET_LENGTH,
    RF_RECEIVE_TIMEOUT
};

enum RF_ReceivedPacketTypes{
    REGISTRY_PACKET,
    STARTUP_PACKET,
    COMMAND_PACKET,
    DOWNLOAD_PACKET,
    DOWNLINK1_PACKET,
    DOWNLINK2_PACKET,
    DOWNLINK_SPECIAL_PACKET,
    UNDEFINED_PACKET
};

enum RF_SentPacketStatus{
    RF_TX_PACKET_OK,
    RF_TX_ERROR
};
/*
*********************************************************************************************************
*                                               TYPEDEFS
*********************************************************************************************************/
struct RFRet
{
    enum  RF_States RF_State;
    enum  RF_ErrorTypes RF_ErrorType;
};
typedef struct  RFRet RFReturn;
typedef  RFReturn *RFReturnPtr;

typedef void (*MACLayerCBF)(void);

struct RFReceiveStructur
{
    enum RF_ReceivedPacketStatus ReceivedPacketStatus;
    enum RF_ReceivedPacketTypes ReceivedPacketType;
    int8 Rssi;
    uint16 ReceiveTime;
    uint16 ReceiveStartTime;
    uint16 MaxReceiveDuration;
    MACLayerCBF ReceiveCBF;
    uint8 PSDU[PHYLAYER_RX_PSDU_MAX_LENGTH];   // das erste byte ist NICHT die Länge
};

typedef struct RFReceiveStructur RFReceiveStruct;
typedef RFReceiveStruct *RFReceiveStructPtr;

struct RFSendStructur
{
    enum RF_SentPacketStatus SentPacketStatus;
    uint16 SendTime;
    MACLayerCBF SendCBF;
    uint8 PSDULength;
    uint8 PSDU[PHYLAYER_TX_PSDU_MAX_LENGTH]; // das erste byte ist NICHT die Länge
};

typedef struct RFSendStructur RFSendStruct;
typedef RFSendStruct *RFSendStructPtr;

typedef enum {
    CRC_8_ATM,      // 8-bit CRC from nRF
    CRC_16,         // 16-bit CRC from CC2500/CC251x
    CRC_16_CCITT,   // 16-bit CRC from nRF
    CRC_24,
    CRC_32
} crc_conf_t;


/*
*******************************************************************************************************************
** Functions for Radio Configuration
*******************************************************************************************************************
*/
extern bool RecOk_TimeOut;
__monitor uint8 RF_GetLock(void);
__monitor void RF_ReleaseLock(void);
void RFInit();
void RFConfig(uint8 modulationSheme, uint8 SyncLength, uint8 usedConfiguration);//, uint8 tx);
void RFSetFrequency(uint8* frequency);
void RFSetDirectFrequency(uint8* frequency) ;
void RFSetGain(uint8 gainMode);
void RFEnableRadio(void);
void RFStartRx(void);
void RFStartRxTest(void);
void RFStartTx(void);
void RFStartBaseTx(void);
void RFForceShutdown(void);
uint8 RFDisableRadio(uint8 forceReset);
void RFCommand(uint8 command);
int8 RFGetLastRssi(void);
int8 RFGetRssi(uint8 rssia_ccuracy);

void RFSetSyncWord(uint8 sw_conf,uint8 sw0,uint8 sw1,uint8 sw2,uint8 sw3,uint8 sw4,uint8 sw5,uint8 sw6,uint8 sw7);
void RFDisableInterrupt(uint8 mask);
void RFEnableInterrupt(uint8 mask);
void RFSetWhitening(uint8 whitening);
void RFSetWhiteningRegister (uint8 whRegValue);
void MaskAllInterrupts(void);
void setQuadratureSkew(uint8 skew);
uint8  RFReadRxPacket(uint8 *pBuf, uint8 maxLen);
//extern uint32 RFCalculateCRC32(uint8 *Data_Ptr, uint8 Data_Length);
void RF_ReceivedPPDUHandler (void);
void RF_TransmittedPPDUHandler(void);
uint8 RFSendMsg (RFSendStructPtr ssp);
uint8 RFReceiveMsg (RFReceiveStructPtr rsp);
void RF_ReceiveStartedHandler (void);
void RF_RadioWakeupRXHandler(void);
void RF_ReceiveTimeoutHandler(void);
void RF_RadioWakeupTXHandler(void);
void RF_StartTransmitter(void);
void RFSetPacketLength(uint8* Length);
void StopRF(void);
uint8  RFSetSAMSFD (void);
uint8  RFSetBeaconSFD (void);
void make_crc_table(void);
uint32 crc32_1byte(uint8* data, uint8 length);
/**
 Inline Funktion, schltet 32 MHz clock oscillator*/
#pragma inline 
void setHighSpeedClock() {
   SLEEPCMD = OSC32K_CALDIS;
   CLKCONCMD &= ~(CLKCON_CLKSPD | CLKCON_TICKSPD | CLKCON_OSC);
   while (CLKCONSTA != 0x80); // warte bis die änderungen gespeichert sind
   return;
}
/** Inline Funktion bereitet GPIO PIN. Dieser Pin liefert die Information zu FPGA, dass die Daten bereit sind.*/
#pragma inline 
void configureGPIOPin() {
    P1SEL &= ~BIT3;           // GPIO PIN FROM FPGA!
    P2SEL &= ~BIT0;           // GPIO6 PIN FROM FPGA!
    P2SEL &= ~BIT1;           // PKT PIN FROM FPGA!
    P2SEL &= ~BIT2;           // FIFO PIN FROM FPGA!
    P2DIR |= BIT0; 
    P2DIR |= BIT1; 
    P2DIR |= BIT2;
    P1DIR |= BIT3;            // Set as Output GPIO PIN FROM FPGA!
    SPI_SEND  = 0;
    GPIO6 = 0;                // GPIO6 PIN FROM FPGA!
    PKT   = 0;                // PKT PIN FROM FPGA! 
    FIFO  = 0;                // FIFO owerflow flag!
    return;
 }


void setOk(void);
#endif