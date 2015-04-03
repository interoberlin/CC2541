/***********************************************************************************
  @file     hal_types.h

  @brief    HAL type definitions

***********************************************************************************/

#ifndef HAL_TYPES_H
#define HAL_TYPES_H

#define CMD_TEST_TRANSMITER  0xA1
#define CMD_SET_FREQUENCY    0x64
#define CMD_RECEIVE          0xA3
#define CMD_SEND             0x63
#define CMD_GET_RSSI         0xA5
#define CMD_GET_LAST_RSSI    0xA6 
#define CMD_SET_GAIN         0xA8
#define CMD_SET_SYNC_LENGTH  0xA9

#ifndef NULL
#define NULL    ((void*)0)     /* changed from char* 93.01.21 ICLM */
#endif

#ifndef FLUSH
#define FLUSH  0x80;
#endif
#ifndef RE
#define RE     0x40;
#endif

#ifndef TRUE
#define TRUE 1
#else
#ifdef __IAR_SYSTEMS_ICC__
//#warning "Macro TRUE already defined"
#endif
#endif

#ifndef FALSE
#define FALSE 0
#else
#ifdef __IAR_SYSTEMS_ICC__
//#warning "Macro FALSE already defined"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** 
   Define Enums For System State and Packet Receive State
*/
/** Zurzeit ist nicht benutzt, ich wollte alles als Zustandsautomaten machen*/
 enum SYS_STATUS {
    DO_NOTHING,
    CONFIG,
    SEND,
    RECEIVE
};

/** Die Zustände für Packet verarbeitung, wie man sehen kann es gibt 4 zustände, warte auf ein kommand (IDLE), 
   speichere die länge des Pakets (LOAD_LENGTH), Speichere die Daten (LOAD_DATA) und Sende die Daten (SEND_DATA)
*/   

enum PACKET_RECEIVE_STATE {
   IDLE_ST,
   BEHAVIOUR_ST,
   FREQ_ST,
   GAIN_ST,
   SYNCLEN_ST,
   PREP_SEND_ST,
   LOAD_DATA_ST,
   SEND_DATA_SPI,
   END_TRANSMISSION
   };
 

/***********************************************************************************
 * TYPEDEFS
 */

typedef signed   char   int8;
typedef unsigned char   uint8;
typedef unsigned char   bool;

typedef signed   short  int16;
typedef unsigned short  uint16;

typedef signed   long   int32;
typedef unsigned long   uint32;

//-----------------------------------------------------//  
// definiere die enums als Datentypen
typedef enum SYS_STATUS SYSTEM_STATUS;   
typedef enum PACKET_RECEIVE_STATE PACKET_REC_STATE;

//-----------------------------------------------------//  

#define  EnableALLinterrupt(void) { EA = 1; }

/***********************************************************************/
// // Configure USART1 for Alternative 2 => Port P1 (PERCFG.U1CFG = 1)
// PERCFG = (PERCFG & ~PERCFG_U1CFG) | PERCFG_U1CFG_ALT2; //(PERCFG & ~PERCFG_U0CFG) | PERCFG_U0CFG_ALT1;
// // Give priority to USART 1 over USART 0 then Timer 1 for port 0 pins
// P2DIR &= P2DIR_PRIP0_USART1;  //P2DIR_PRIP0_USART0 
// // Set pins 4, 4, 5 and 7 as peripheral I/O.
// P1SEL = BIT7 | BIT6 | BIT5 | BIT4; //BIT5 | BIT4 | BIT3 | BIT2;
    

/***************************************************************************
* Setup interrupt
*/



// typedef void (*ISR_FUNC_PTR)(void);
// typedef void (*VFPTR)(void);

// BIT definitions.
   #define BIT7        0x80
   #define BIT6        0x40
   #define BIT5        0x20
   #define BIT4        0x10
   #define BIT3        0x08
   #define BIT2        0x04
   #define BIT1        0x02
   #define BIT0        0x01

   /*******************************************************************************
 * Interrupt Control Registers
 */

// IEN0 (0xA8) - Interrupt Enable 0 Register - bit accessible SFR register

// IEN1 (0xB8) - Interrupt Enable 1 Register - bit accessible SFR register
#define IEN1_P0IE                         0x20        // Port 0 interrupt enable
#define IEN1_T4IE                         0x10        // Timer 4 interrupt enable
#define IEN1_T3IE                         0x08        // Timer 3 interrupt enable
#define IEN1_T2IE                         0x04        // Timer 2 interrupt enable
#define IEN1_T1IE                         0x02        // Timer 1 interrupt enable
#define IEN1_DMAIE                        0x01        // DMA transfer interrupt enable

// IEN2 (0x9A) - Interrupt Enable 2 Register
#define IEN2_WDTIE                        0x20
#define IEN2_P1IE                         0x10      
#define IEN2_UTX1IE                       0x08     // USART 1 TX interrupt enable
#define IEN2_UTX0IE                       0x04
#define IEN2_P2IE                         0x02
#define IEN2_USBIE                        0x02
#define IEN2_RFIE                         0x01

// TCON (0x88) - CPU Interrupt Flag 1 - bit accessible SFR register

// S0CON (0x98) - CPU Interrupt Flag 2 - bit accessible SFR register

// S1CON (0x9B) - CPU Interrupt Flag 3
#define S1CON_RFIF_1                      0x02
#define S1CON_RFIF_0                      0x01

// IRCON (0xC0) - CPU Interrupt Flag 4 - bit accessible SFR register

// IRCON2 (0xE8) - CPU Interrupt Flag 5 - bit accessible SFR register

// IP1 (0xB9) - Interrupt Priority 1
#define IP1_IPG5                          0x20
#define IP1_IPG4                          0x10
#define IP1_IPG3                          0x08
#define IP1_IPG2                          0x04
#define IP1_IPG1                          0x02
#define IP1_IPG0                          0x01

// IP0 (0xA9) - Interrupt Priority 0
#define IP0_IPG5                          0x20
#define IP0_IPG4                          0x10
#define IP0_IPG3                          0x08
#define IP0_IPG2                          0x04
#define IP0_IPG1                          0x02
#define IP0_IPG0                          0x01

// U1CSR (0xF8) - USART 1 Control and Status
#define U1CSR_MODE                        0x80
#define U1CSR_RE                          0x40
#define U1CSR_SLAVE                       0x20
#define U1CSR_FE                          0x10
#define U1CSR_ERR                         0x08
#define U1CSR_RX_BYTE                     0x04
#define U1CSR_TX_BYTE                     0x02
#define U1CSR_TX_NBYTE                    0xFD
#define U1CSR_ACTIVE                      0x01
  
// U1UCR (0xFB) - USART 1 UART Control
#define U1UCR_FLUSH                       0x80
#define U1UCR_FLOW                        0x40
#define U1UCR_D9                          0x20
#define U1UCR_BIT9                        0x10
#define U1UCR_PARITY                      0x08
#define U1UCR_SPB                         0x04
#define U1UCR_STOP                        0x02
#define U1UCR_START                       0x01

// U0GCR (0xFC) - USART 1 Generic Control
#define U1GCR_CPOL                        0x80
#define U1GCR_CPHA                        0x40
#define U1GCR_ORDER                       0x20
#define U1GCR_BAUD_E                      0x1F
  #define U1GCR_BAUD_E0                     0x01
  #define U1GCR_BAUD_E1                     0x02
  #define U1GCR_BAUD_E2                     0x04
  #define U1GCR_BAUD_E3                     0x08
  #define U1GCR_BAUD_E4                     0x10  
  
/*******************************************************************************
 * Power Management and Clocks
 */

// SRCRC (0x6262) – Sleep Reset CRC (not available on the CC2544)
#define SRCRC_FORCE_RESET                 0x20          // Force watchdog reset
#define SRCRC_CRC_RESULT                  (0x03 << 2)   // CRC value, bit mask
  #define SRCRC_CRC_RESULT_PASS             (0x00 << 2)   // CRC of retained registers passed
  #define SRCRC_CRC_RESULT_LOW              (0x01 << 2)   // Low CRC value failed
  #define SRCRC_CRC_RESULT_HIGH             (0x02 << 2)   // High CRC value failed
  #define SRCRC_CRC_RESULT_BOTH             (0x03 << 2)   // Both CRC values failed
#define SRCRC_CRC_RESET_EN                0x01          // CRC != 00 reset enable, after wakeup from PM2/PM3.

// PCON (0x87) - Power Mode Control
//#define PCON_IDLE                         0x01

// SLEEPCMD (0xBE) - Sleep Mode Control
#define OSC32K_CALDIS                     0x80
#define SLEEPCMD_MODE                     (0x03)      // Power mode bit mask
  #define SLEEPCMD_MODE_IDLE                (0x00)
  #define SLEEPCMD_MODE_PM1                 (0x01)  
  #define SLEEPCMD_MODE_PM2                 (0x02)  
  #define SLEEPCMD_MODE_PM3                 (0x03)


// SLEEPSTA (0x9D) – Sleep-Mode Control Status
#define SLEEPSTA_CLK32K_CALDIS            0x80          // Calibration disable status
#define SLEEPSTA_RST                      (0x03 << 3)   // Cause of last reset, bit mask
  #define SLEEPSTA_RST_POR_BOD              (0x00 << 3)   // Power-on reset, or brownout detection
  #define SLEEPSTA_RST_EXT                  (0x01 << 3)   // External reset
  #define SLEEPSTA_RST_WDT                  (0x02 << 3)   // Watchdog Timer reset
  #define SLEEPSTA_RST_CLK_LOSS             (0x03 << 3)   // Clock loss reset
#define SLEEPSTA_CLK32K                   0x01          // 32 kHz clock signal

// STLOAD (0xAD) – Sleep Timer Load Status
#define STLOAD_LDRDY                      0x01

// CLKCONCMD (0xC6) – Clock Control Command
// and CLKCONSTA (0x9E) – Clock Control Status
#define CLKCON_OSC32K                     0x80          // 32 kHz clock source select/status
#define CLKCON_OSC                        0x40          // system clock source select/status
#define CLKCON_TICKSPD                    (0x07 << 3)   // bit mask, global timer tick speed divider
  #define CLKCON_TICKSPD_32M                (0x00 << 3)
  #define CLKCON_TICKSPD_16M                (0x01 << 3)
  #define CLKCON_TICKSPD_8M                 (0x02 << 3)
  #define CLKCON_TICKSPD_4M                 (0x03 << 3)
  #define CLKCON_TICKSPD_2M                 (0x04 << 3)
  #define CLKCON_TICKSPD_1M                 (0x05 << 3)
  #define CLKCON_TICKSPD_500K               (0x06 << 3)
  #define CLKCON_TICKSPD_250K               (0x07 << 3)
#define CLKCON_CLKSPD                     (0x07)        // bit mask for the clock speed division
  #define CLKCON_CLKSPD_32M                 (0x00)
  #define CLKCON_CLKSPD_16M                 (0x01)
  #define CLKCON_CLKSPD_8M                  (0x02)
  #define CLKCON_CLKSPD_4M                  (0x03)
  #define CLKCON_CLKSPD_2M                  (0x04)
  #define CLKCON_CLKSPD_1M                  (0x05)
  #define CLKCON_CLKSPD_500K                (0x06)
  #define CLKCON_CLKSPD_250K                (0x07)

// CLD (0x6290) – Clock-Loss Detection
#define CLD_EN                            0x01          // Clock-loss detector enable


/*******************************************************************************
 *  Flash Controller
 */

// FCTL (0x6270) - Flash Control
#define FCTL_BUSY                         0x80
#define FCTL_FULL                         0x40
#define FCTL_ABORT                        0x20
#define FCTL_CM                           (0x03 << 2)   // cache mode bit mask
  #define FCTL_CM_DIS                       (0x00 << 2)   // cache mode disabled
  #define FCTL_CM_EN                        (0x01 << 2)   // cache mode enabled
  #define FCTL_CM_PREFETCH                  (0x02 << 2)   // cache mode enabled, prefetch mode
  #define FCTL_CM_REALTIME                  (0x03 << 2)   // cache mode enabled, real-time mode
#define FCTL_WRITE                        0x02
#define FCTL_ERASE                        0x01


// FWDATA (0xAF) - Flash Write Data

// FADDRH (0xAD) - Flash Address High Byte

// FADDRL (0xAC) - Flash Address Low Byte
// PN9 and PN7 parameters
#define BSP_MODE_W_PN9_EN                   0x02    // Enable PN9 whitener.
#define BSP_MODE_W_PN7_EN                   0x01    // Enable PN7 whitener.


  // P0 (0x80) - Port 0 - bit accessible SFR register

  // P1 (0x90) - Port 1 - bit accessible SFR register 

  // P2 (0xA0) - Port 2 - bit accessible SFR register

  // PERCFG (0xF1) - Peripheral Control
  #define PERCFG_T1CFG                  0x40               // Timer 1 I/O location
  #define PERCFG_T3CFG                  0x20               // Timer 3 I/O location
  #define PERCFG_T4CFG                  0x10               // Timer 4 I/O location
  #define PERCFG_U1CFG                  0x02               // USART 1 I/O location
  #define PERCFG_U0CFG                  0x01               // USART 0 I/O location
    #define PERCFG_U0CFG_ALT1                 0x00          // Alternative 1 location
    #define PERCFG_U0CFG_ALT2                 0x01          // Alternative 2 location
    
   #define PERCFG_U1CFG_ALT1                 0x00            
   #define PERCFG_U1CFG_ALT2                 0x02 

  // APCFG (0xF2) - Analog Peripheral I/O Configuration
  #define APCFG_APCFG7                      0x80        // When set, analog I/O on P0_7 is enabled
  #define APCFG_APCFG6                      0x40
  #define APCFG_APCFG5                      0x20
  #define APCFG_APCFG4                      0x10
  #define APCFG_APCFG3                      0x08
  #define APCFG_APCFG2                      0x04
  #define APCFG_APCFG1                      0x02
  #define APCFG_APCFG0                      0x01

  // P0SEL (0xF3) – P0 Function Select
  #define P0SEL_SELP0_7                     0x80        // Pin function as peripheral I/O when set
  #define P0SEL_SELP0_6                     0x40
  #define P0SEL_SELP0_5                     0x20
  #define P0SEL_SELP0_4                     0x10
  #define P0SEL_SELP0_3                     0x08
  #define P0SEL_SELP0_2                     0x04
  #define P0SEL_SELP0_1                     0x02
  #define P0SEL_SELP0_0                     0x01

  // P1SEL (0xF4) – P1 Function Select
  #define P1SEL_SELP1_7                     0x80        // Pin function as peripheral I/O when set
  #define P1SEL_SELP1_6                     0x40
  #define P1SEL_SELP1_5                     0x20
  #define P1SEL_SELP1_4                     0x10
  #define P1SEL_SELP1_3                     0x08
  #define P1SEL_SELP1_2                     0x04
  #define P1SEL_SELP1_1                     0x02
  #define P1SEL_SELP1_0                     0x01

  // P2SEL (0xF5) – Port 2 Function Select and Port 1 Peripheral Priority Control
  #define P2SEL_PRI3P1                      0x40        //  When set USART 1 has priotity over USART 0.
  #define P2SEL_PRI2P1                      0x20        //  When set Timer 3 has priotity over USART 1.
  #define P2SEL_PRI1P1                      0x10        //  When set Timer 4 has priotity over Timer 1.
  #define P2SEL_PRI0P1                      0x08        //  When set Timer 1 has priotity over USART 0.
  #define P2SEL_SELP2_4                     0x04        //  P2.4 function select
  #define P2SEL_SELP2_3                     0x02        //  P2.3 function select
  #define P2SEL_SELP2_0                     0x01        //  P2.0 function select

  // P1INP (0xF6) – Port 1 Input Mode
  #define P1INP_MDP1                        0xFC      // P1.7 to P1.2 I/O input mode bit mask
  #define P1INP_MDP1_P1_7                   0x80      // When set P1.7 is 3-state, when not set pullup or pulldown (set in P2INP)
  #define P1INP_MDP1_P1_6                   0x40      // When set P1.6 is 3-state, when not set pullup or pulldown (set in P2INP)
  #define P1INP_MDP1_P1_5                   0x20      // When set P1.5 is 3-state, when not set pullup or pulldown (set in P2INP)
  #define P1INP_MDP1_P1_4                   0x10      // When set P1.4 is 3-state, when not set pullup or pulldown (set in P2INP)
  #define P1INP_MDP1_P1_3                   0x08      // When set P1.3 is 3-state, when not set pullup or pulldown (set in P2INP)
  #define P1INP_MDP1_P1_2                   0x04      // When set P1.2 is 3-state, when not set pullup or pulldown (set in P2INP)

  // P2INP (0xF7) – Port 1 Input Mode
  #define P2INP_PDUP2                       0x80      // Port 2 pullup/pulldown select.  1 - pulldown, 0- pullup. 
  #define P2INP_PDUP1                       0x40      // Port 1 pullup/pulldown select.  1 - pulldown, 0- pullup.
  #define P2INP_PDUP0                       0x20      // Port 0 pullup/pulldown select.  1 - pulldown, 0- pullup.
  #define P2INP_MDP2                        0x1F      //  P2.4 to P2.0 I/O input mode bit mask.
  #define P2INP_MDP2_P2_4                   0x10      //  P2.4 pullup/pulldown select.  1 - pulldown, 0- pullup. 
  #define P2INP_MDP2_P2_3                   0x10      //  P2.3 pullup/pulldown select.  1 - pulldown, 0- pullup. 
  #define P2INP_MDP2_P2_2                   0x10      //  P2.2 pullup/pulldown select.  1 - pulldown, 0- pullup. 
  #define P2INP_MDP2_P2_1                   0x10      //  P2.1 pullup/pulldown select.  1 - pulldown, 0- pullup. 
  #define P2INP_MDP2_P2_0                   0x10      //  P2.0 pullup/pulldown select.  1 - pulldown, 0- pullup. 


  // P0IFG (0x89) – Port 0 Interrupt Status Flag

  // P1IFG (0x8A) – Port 1 Interrupt Status Flag

  // P0DIR (0xFD) – Port 0 Direction Control ( P0.7 to P0.0 ).

  // P1DIR (0xFE) – Port 1 Direction Control ( P1.7 to P1.0 ).

  // P2DIR (0xFF) – Port 2 Direction and Port 0 Peripheral Priority Control
  #define P2DIR_PRIP0                       0xC0          // Port 0 peripheral priority control.
    #define P2DIR_PRIP0_USART0                (0x00 << 6) // USART 0 has priority, then USART 1, then Timer 1
    #define P2DIR_PRIP0_USART1                (0x01 << 6) // USART 1 has priority, then USART 0, then Timer 1
    #define P2DIR_PRIP0_T1_0_1                (0x02 << 6) // Timer 1 channels 0-1 has priority, then USART 1, then USART 0, then Timer 1 channels 2-3
    #define P2DIR_PRIP0_T1_2_3                (0x03 << 6) // Timer 1 channels 2-3 has priority, then USART 0, then USART 1, then Timer 1 channels 0-1
  #define P2DIR_DIRP2                       0x1F          // P2.4 to P2.0 I/O direction

  // P0INP (0x8F) – Port 0 Input Mode

  // P2INP (0xF7) – Port 2 Input Mode
  #define P2INP_PDUP2                       0x80        //  Port 2 pullup/pulldown select. When set high -> Pullup.
  #define P2INP_PDUP1                       0x40        //  Port 1 pullup/pulldown select. When set high -> Pullup.
  #define P2INP_PDUP0                       0x20        //  Port 0 pullup/pulldown select. When set high -> Pullup.
  #define P2INP_MDP2                        0x1F        //  P2.4 to P2.0 I/O input mode. When set high -> 3-state.

  // P0IFG (0x89) – Port 0 Interrupt Status Flag

  // P1IFG (0x8A) – Port 1 Interrupt Status Flag
  
  // P2IFG (0x8B) – Port 2 Interrupt Status Flag

  // PICTL (0x8C) - Port Interrupt Control
  #define PICTL_PADSC                       0x80          // Drive strength control for I/O pins in output mode.
  #define PICTL_P2ICON                      0x08          // Port 2, inputs 4 to 0 interrupt configuration. Interrupt on falling edge when set, rising edge when cleared.
  #define PICTL_P1ICONH                     0x04          // Port 1, inputs 7 to 4 interrupt configuration. Interrupt configuration. Interrupt on falling edge when set, rising edge when cleared.
  #define PICTL_P1ICONL                     0x02          // Port 1, inputs 3 to 0 interrupt configuration. Interrupt on falling edge when set, rising edge when cleared.
  #define PICTL_P0ICON                      0x01          // Port 0, inputs 7 to 0 interrupt configuration. Interrupt on falling edge when set, rising edge when cleared.

  // P0IEN (0xAB) – Port 0 Interrupt Mask

  // P1IEN (0x8D) – Port 1 Interrupt Mask

  // P2IEN (0xAC) – Port 2 Interrupt Mask

  // PMUX (0xAE) – Power-Down Signal Mux
  #define PMUX_CKOEN                        0x80            // Clock out enable, the 32 kHz clock.
  #define PMUX_CKOPIN                       (0x07 << 4)     // Selects which pin on Port 0, bit mask.
  #define PMUX_DREGSTA                      0x04            // Digital Regulator Status output enable.
  #define PMUX_DREGSTAPIN                   (0x07)          // Selects which pin on Port 1, bit mask.

  // OBSSEL0 (0x6243) – Observation output control register 0

  // OBSSEL1 (0x6244) – Observation output control register 1

  // OBSSEL2 (0x6245) – Observation output control register 2

  // OBSSEL3 (0x6246) – Observation output control register 3

  // OBSSEL4 (0x6247) – Observation output control register 4

  // OBSSEL5 (0x6248) – Observation output control register 5


// /***********************************************************************************
 // * Compiler abstraction
 // */

// /*****************************************************
 // * IAR MSP430
 // */
// #ifdef __IAR_SYSTEMS_ICC__

// #define _PRAGMA(x) _Pragma(#x)

// #if defined __ICC430__

// #ifndef CODE
// #define CODE
// #endif
// #ifndef XDATA
// #define XDATA
// #endif
// #define FAR
// #define NOP()  asm("NOP")

// #define HAL_ISR_FUNC_DECLARATION(f,v)   \
    // _PRAGMA(vector=v##_VECTOR) __interrupt void f(void)
// #define HAL_ISR_FUNC_PROTOTYPE(f,v)     \
    // _PRAGMA(vector=v##_VECTOR) __interrupt void f(void)
// #define HAL_ISR_FUNCTION(f,v)           \
    // HAL_ISR_FUNC_PROTOTYPE(f,v); HAL_ISR_FUNC_DECLARATION(f,v)


// /*****************************************************
 // * IAR 8051
 // */
// #elif defined __ICC8051__

// #ifndef BSP_H
// #define CODE   __code
// #define XDATA  __xdata
// #endif

// #define FAR
// #define NOP()  asm("NOP")

// #define HAL_MCU_LITTLE_ENDIAN()   __LITTLE_ENDIAN__
// #define HAL_ISR_FUNC_DECLARATION(f,v)   \
    // _PRAGMA(vector=v) __near_func __interrupt void f(void)
// #define HAL_ISR_FUNC_PROTOTYPE(f,v)     \
    // _PRAGMA(vector=v) __near_func __interrupt void f(void)
// #define HAL_ISR_FUNCTION(f,v)           \
    // HAL_ISR_FUNC_PROTOTYPE(f,v); HAL_ISR_FUNC_DECLARATION(f,v)

// /*****************************************************
 // * IAR ARM
 // */
// #elif defined __ICCARM__

// #ifndef CODE
// #define CODE
// #endif
// #ifndef XDATA
// #define XDATA
// #endif
// #define FAR
// #define NOP()  asm("NOP")

// #define HAL_ISR_FUNC_DECLARATION(f,v)
// #define HAL_ISR_FUNC_PROTOTYPE(f,v)
// #define HAL_ISR_FUNCTION(f,v)

// #else
// #error "Unsupported architecture"
// #endif


// /*****************************************************
 // * KEIL 8051
 // */
// #elif defined __KEIL__
// #include <intrins.h>
// #define BIG_ENDIAN

// #define CODE   code
// #define XDATA  xdata
// #define FAR
// #define NOP()  _nop_()

// #define HAL_ISR_FUNC_DECLARATION(f,v)   \
    // void f(void) interrupt v
// #define HAL_ISR_FUNC_PROTOTYPE(f,v)     \
    // void f(void)
// #define HAL_ISR_FUNCTION(f,v)           \
    // HAL_ISR_FUNC_PROTOTYPE(f,v); HAL_ISR_FUNC_DECLARATION(f,v)

// typedef unsigned short istate_t;

// // Keil workaround
// #define __code  code
// #define __xdata xdata


// /*****************************************************
 // * WIN32
 // */
// #elif defined WIN32

// #define DESKTOP

// #define CODE
// #define XDATA
// #include "windows.h"
// #ifndef FAR
// #define FAR far
// #endif
// #ifdef _MSC_VER
// #pragma warning (disable :4761)
// #pragma warning (disable :4100)
// #endif


// /*****************************************************
 // * LINUX
 // */
// #elif defined __linux

// #define DESKTOP
// #define LINUX

// #define CODE
// #define XDATA
// #ifndef FAR
// #define FAR far
// #endif


// /*****************************************************
 // * Code Composer Essential
 // */

// #elif __TI_COMPILER_VERSION__
// #define CODE
// #define XDATA
// #define FAR

// typedef unsigned short istate_t;


// /*****************************************************
 // * Other compilers
 // */
// #else
// #error "Unsupported compiler"
// #endif



#ifdef __cplusplus
}
#endif



/***********************************************************************************
  Copyright 2007-2008 Texas Instruments Incorporated. All rights reserved.

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

#endif
