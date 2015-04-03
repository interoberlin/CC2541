
/***********************************************************************************
  Filename:     prop_regs.h

  Description:  Header that defines the RAM-based registers on CC2540 (proprietary mode)

  Comments:    There are two kinds of definitions: Standard definitions as for 
               the hardware registers and a struct based definition. The latter
               can be used in the debugger (open PRF in the watrch window)

                TI Confidential - NDA Restrictions

***********************************************************************************/

#ifndef __PROP_REG_H
#define __PROP_REG_H

#include <ioCC2541.h>
// Definition below works only with little-endian registers, and only with a little-endian compiler (such as IAR)
#define DXREG(addr)       (((unsigned short volatile __xdata *) addr)[0])

typedef union {
    unsigned char value;
    struct {
        unsigned char FREQ:7;
        unsigned char SYNTH_ON:1;
    };
} chan_t;

typedef union {
    unsigned char value;
    struct {
        unsigned char MODE:2;
        unsigned char REPEAT:1;
        unsigned char START_CONF:1;
        unsigned char STOP_CONF:2;
        unsigned char TX_ON_CC_CONF:1;
        unsigned char REPEAT_CONF:1;
    };
} task_conf_t;

typedef union {
    unsigned char value;
    struct {
        unsigned char AUTOFLUSH_IGN:1;
        unsigned char AUTOFLUSH_CRC:1;
        unsigned char AUTOFLUSH_EMPTY:1;
        unsigned char RX_STATUS_CONF:1;
        unsigned char RX_ADDR_CONF:2;
        unsigned char TX_ADDR_CONF:2;
    };
} fifo_conf_t;

// typedef union {
    // unsigned char value;
    // struct {
        // unsigned char ADDR_LEN:4;
        // unsigned char QUICK_TX:4;
    // };
// } pkt_conf_t;
typedef union {
    unsigned char value;
    struct {
      unsigned char ADDR_LEN:1;
      unsigned char AGC_EN:1;
      unsigned char START_TONE:1;
      unsigned char :1;
      unsigned char QUICK_TX:4;
    };
} pkt_conf_t;

typedef union {
    unsigned char value;
    struct {
        unsigned char RXCAP:2;
        unsigned char TXCAP:2;
        unsigned char TXIF:2;
    };
} radio_conf_t;

typedef union {
    unsigned char value;
    struct {
        unsigned char ENA0:1;
        unsigned char ENA1:1;
        unsigned char REUSE:1;
        unsigned char AA:1;
        unsigned char VARLEN:1;
        unsigned char FIXEDSEQ:1;
    };
} conf_t;

typedef union {
    unsigned char value;
    struct {
        unsigned char VALID:1;
        unsigned char SEQ:2;
        unsigned char ACKSEQ:2;
        unsigned char ACK_PAYLOAD_SENT:1;
        unsigned char NEXTACK:1;
    };
} seqstat_t;

typedef union {
    unsigned char value[12];
    struct {
        conf_t CONF;
        unsigned char RXLENGTH;
        union {
            unsigned char P_ADDRESS[8];
            struct {
                unsigned char ADDRESS;
                seqstat_t SEQSTAT;
                unsigned char ACKLENGTH[2];
                unsigned short CRCVAL;
                unsigned char N_TXDONE;
                unsigned char N_RXIGNORED;
            };
        };
        unsigned char N_RXOK;
        unsigned char N_RXNOK;
    };
} address_entry_t;
    

typedef enum {
    TASK_ENDOK = 0,
    TASK_RXTIMEOUT = 1,
    TASK_NOSYNC = 2,
    TASK_NOCC = 3,
    TASK_MAXRT = 4,
    TASK_STOP = 5,
    TASK_ABORT = 6,
    TASK_UNDEF = 15,
    TASKERR_INTERNAL = 255,
    TASKERR_CMD = 254,
    TASKERR_SEM = 253,
    TASKERR_PAR = 252,
    TASKERR_TXFIFO = 251,
    TASKERR_RXFIFO = 250,
    TASKERR_MODUNF = 249
} end_cause_t;


typedef struct {
    chan_t CHAN;
    task_conf_t TASK_CONF;
    fifo_conf_t FIFO_CONF;
    pkt_conf_t PKT_CONF;
    unsigned char CRC_LEN;
    unsigned char RSSI_LIMIT;
    unsigned short RSSI_COUNT;
    unsigned char CRC_INIT[4];
    unsigned char W_INIT;
    unsigned char RETRANS_CNT;
    unsigned short TX_DELAY;
    unsigned short RETRANS_DELAY;
    unsigned short SEARCH_TIME;
    unsigned short RX_TX_TIME;
    unsigned short TX_RX_TIME;
    address_entry_t ADDR_ENTRY[8];
    unsigned char N_TX;
    unsigned char LAST_RSSI;
    unsigned char LAST_DCOFF[4];
    radio_conf_t RADIO_CONF;
    end_cause_t ENDCAUSE;
} prf_reg_t;

__xdata __no_init volatile prf_reg_t PRF @ 0x6000;


#define PRF_CHAN                XREG(0x6000)
#define PRF_TASK_CONF           XREG(0x6001)
#define PRF_FIFO_CONF           XREG(0x6002)
#define PRF_PKT_CONF            XREG(0x6003)
#define PRF_CRC_LEN             XREG(0x6004)
#define PRF_RSSI_LIMIT          XREG(0x6005)
#define PRF_RSSI_COUNT          DXREG(0x6006) // 2 bytes
#define PRF_CRC_INIT            PXREG(0x6008) // 4 bytes
#define PRF_W_INIT              XREG(0x600C)
#define PRF_RETRANS_CNT         XREG(0x600D)
#define PRF_TX_DELAY            DXREG(0x600E) // 2 bytes
#define PRF_RETRANS_DELAY       DXREG(0x6010) // 2 bytes
#define PRF_SEARCH_TIME         DXREG(0x6012) // 2 bytes
#define PRF_RX_TX_TIME          DXREG(0x6014) // 2 bytes
#define PRF_TX_RX_TIME          DXREG(0x6016) // 2 bytes
#define PRF_ADDR_ENTRY0         PXREG(0x6018) // 12 bytes
#define PRF_ADDR_ENTRY1         PXREG(0x6024) // 12 bytes
#define PRF_ADDR_ENTRY2         PXREG(0x6030) // 12 bytes
#define PRF_ADDR_ENTRY3         PXREG(0x603C) // 12 bytes
#define PRF_ADDR_ENTRY4         PXREG(0x6048) // 12 bytes
#define PRF_ADDR_ENTRY5         PXREG(0x6054) // 12 bytes
#define PRF_ADDR_ENTRY6         PXREG(0x6060) // 12 bytes
#define PRF_ADDR_ENTRY7         PXREG(0x606C) // 12 bytes
#define PRF_N_TX                XREG(0x6078)
#define PRF_LAST_RSSI           XREG(0x6079)
#define PRF_LAST_DCOFF          PXREG(0x607A) // 4 bytes
#define PRF_RADIO_CONF          XREG(0x607E)
#define PRF_ENDCAUSE            XREG(0x607F)
            
#define CONF_OFFSET             0
#define RXLENGTH_OFFSET         1
#define ADDRESS_OFFSET          2
#define SEQSTAT_OFFSET          3
#define ACKLENGTH0_OFFSET       4
#define ACKLENGTH1_OFFSET       5
#define CRCVAL_OFFSET           6
#define N_TXDONE_OFFSET         8
#define N_RXIGNORED_OFFSET      9
#define N_RXOK_OFFSET           10
#define N_RXNOK_OFFSET          11

// LLE commands
#define CMD_SHUTDOWN            0x01
#define CMD_DEMOD_TEST          0x02
#define CMD_RX_TEST             0x03
#define CMD_TX_TEST             0x04
#define CMD_TX_FIFO_TEST        0x05
#define CMD_PING                0x06
#define CMD_RX                  0x08
#define CMD_TX                  0x09
#define CMD_TX_ON_CC            0x0A
#define CMD_STOP                0x0B
#define CMD_SEND_EVENT1         0x21
#define CMD_SEND_EVENT2         0x22
#define CMD_FLUSH_ACK0          0x30
#define CMD_FLUSH_ACK1          0x31
#define CMD_FLUSH_ACK2          0x32
#define CMD_FLUSH_ACK3          0x33
#define CMD_FLUSH_ACK4          0x34
#define CMD_FLUSH_ACK5          0x35
#define CMD_FLUSH_ACK6          0x36
#define CMD_FLUSH_ACK7          0x37

// FIFO commands Load this commands to the Register RFST to execute a FIFO command.
#define CMD_RXFIFO_RESET        0x81
#define CMD_RXFIFO_DEALLOC      0x82
#define CMD_RXFIFO_RETRY        0x83
#define CMD_RXFIFO_DISCARD      0x84
#define CMD_RXFIFO_COMMIT       0x85
#define CMD_TXFIFO_RESET        0x91
#define CMD_TXFIFO_DEALLOC      0x92
#define CMD_TXFIFO_RETRY        0x93
#define CMD_TXFIFO_DISCARD      0x94
#define CMD_TXFIFO_COMMIT       0x95
#define CMD_FIFO_RESET          0xF1
#define CMD_FIFO_DEALLOC        0xF2
#define CMD_FIFO_RETRY          0xF3
#define CMD_FIFO_DISCARD        0xF4
#define CMD_FIFO_COMMIT         0xF5

//RFIRQF0 Interrupt Flags (Added Dirk Krueger, 21.06.2012)
#define RXTHSUP_FLAG            0x08
#define TXTHSUP_FLAG            0x04
#define RXTHSDN_FLAG            0x02
#define TXTHSDN_FLAG            0x01

//RFIRQF1 Interrupt Flags (Added Dirk Krueger, 21.06.2012)
#define PINGRSP_FLAG            0x80
#define TASKDONE_FLAG           0x40
#define TXDONE_FLAG             0x20
#define RXEMPTY_FLAG            0x10
#define RXIGNORED_FLAG          0x08
#define RXNOK_FLAG              0x04
#define TXFLUSHED_FLAG          0x02
#define RXOK_FLAG               0x01

//RFERRM Interrupt Enable (Added Dirk Krueger, 21.06.2012)
#define STROBEERR                0x40
#define TXUNDERF                 0x20
#define TXOVERF                  0x10
#define RXUNDERF                 0x08
#define RXOVERF                  0x04
#define RXABO                    0x02
#define NLOCK                    0x01
// RFERRM Interrupt Flags (Added Dmytro Krush, 27.05.2013)
#define RXFIFOFULL_FLAG         0x40
#define LLEERR_FLAG             0x20
#define RXTXABO_FLAG            0x10
#define RXOVERF_FLAG            0x08
#define TXOVERF_FLAG            0x04
#define RXUNDERF_FLAG           0x02
#define TXUNDERF_FLAG           0x01







#endif

/***********************************************************************************
  Copyright 2010 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
***********************************************************************************/
