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
* Dateiname:       MCUTimer1.h
*
* Programmierer:   Dirk Krüger
*                  <dkrueger@hsu-hh.de>
*             	   (+49)40-6541-3815
*
*
* Version:         $Revision$
*
* Erstellt am:     $Date$
*/

/*! \file          MCUTimer1.h
 *  \brief         Datei enthält die Timer Funktionen
 *
 *  \see           MCUTimer1.c
 */

/*
*********************************************************************************************************
*                                               TIMERHEADER Datei
*********************************************************************************************************
*/

#ifndef   MCUTIMER2_H
#define   MCUTIMER2_H

/*
*********************************************************************************************************
*                                               INCLUDES
*********************************************************************************************************
*/
#include    "hal_types.h"
#include    "prop_regs.h"

/******************************************* TIMER DEFINES  ********************************************/
// ************************* REGISTER : T2IRQF
#define TIMER2_LONG_COMPARE2F 0x80 //0 R/W0 Set when the Timer 2 overflow counter is equal to t2ovf_cmp2 and the timer
                                   //counts to the value set at t2_cmp2
#define TIMER2_LONG_COMPARE1F 0x40 //0 R/W0 Set when the Timer 2 overflow counter is equal to t2ovf_cmp1 and the timer
                                   //counts to the value set at t2_cmp1
#define TIMER2_OVF_COMPARE2F  0x20 //0 R/W0 Set when the Timer 2 overflow counter counts to the value set at t2ovf_cmp2
#define TIMER2_OVF_COMPARE1F  0x10 //0 R/W0 Set when the Timer 2 overflow counter counts to the value set at Timer 2
                                   //t2ovf_cmp1
#define TIMER2_OVF_PERF       0x08 //R/W0 Set when the Timer 2 overflow counter would have counted to a value equal to
                                   //t2ovf_per, but instead wraps to 0
#define TIMER2_COMPARE2F      0x04 //0 R/W0 Set when the Timer 2 counter counts to the value set at t2_cmp2
#define TIMER2_COMPARE1F      0x02 //0 R/W0 Set when the Timer 2 counter counts to the value set at t2_cmp1
#define TIMER2_PERF           0x01 //0 R/W0 Set when the Timer 2 counter would have counted to a value equal to t2_per,
                                   //but instead wraps to 0.
// ************************* REGISTER : T2IRQM
#define TIMER2_LONG_COMARE2M  0x80 //default: 0; R/W Enables the TIMER2_LONG_COMPARE2F interrupt
#define TIMER2_LONG_COMARE1M  0x40 //default: 0; R/W Enables the TIMER2_LONG_COMPARE1F interrupt
#define TIMER2_OVF_COMPARE2M  0x20 //default: 0; R/W Enables the TIMER2_OVF_COMPARE2 interrupt
#define TIMER2_OVF_COMPARE1M  0x10 //default: 0; R/W Enables the TIMER2_OVF_COMPARE1 interrupt
#define TIMER2_OVF_PERM       0x08 //default: 0; R/W Enables the TIMER2_OVF_PER interrupt
#define TIMER2_COMPARE2M      0x04 //default: 0; R/W Enables the TIMER2_COMPARE2 interrupt
#define TIMER2_COMPARE1M      0x02 //default: 0; R/W Enables the TIMER2_COMPARE1 interrupt
#define TIMER2_PERM           0x01 //default: 0; R/W Enables the TIMER2_PER interrupt
// *************************   T2MSEL
#define T2OVF                 0x00// BITS(6:4) 000: t2ovf (overflow counter)
#define T2OVF_CAP             0x10// BITS(6:4) 001: t2ovf_cap (overflow capture)
#define T2OVF_PER             0x20// BITS(6:4) 010: t2ovf_per (overflow period)
#define T2OVF_CMP1            0x30// BITS(6:4) 011: t2ovf_cmp1 (overflow compare 1)
#define T2OVF_CMP2            0x40// BITS(6:4) 100: t2ovf_cmp2 (overflow compare 2)

#define T2TIM                 0x00// BITS(2:0) 000: t2tim (timer count value)
#define T2_CAP                0x01// BITS(2:0) 001: t2_cap (timer capture)
#define T2_PER                0x02// BITS(2:0) 010: t2_per (timer period)
#define T2_CMP1               0x03// BITS(2:0) 011: t2_cmp1 (timer compare 1)
#define T2_CMP2               0x04// BITS(2:0) 100: t2_cmp2 (timer compare 2)
// ************************ T2CTRL
#define RUN                   0x01  // Write 1 to start timer
#define STOP                  0x00  // Write 0 to stop timer
#define STATE                 0x04  // 0: Timer idle

// Timer2 events
#define NO_TIMER_EVENT           0x77
                                 // 1: Timer running
//#define LATCH_AT_ONCE         0x08
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/
//#define GlobalTimeLow       (uint16)T1CNTL + (((uint16)T1CNTH) << 8)
/*
*********************************************************************************************************
*                                               TYPEDEFS
*********************************************************************************************************
*/

/**
 * Typdefinition eines Funktionszeigers mit einem 16-Bit Parameter und keinem Rückgabewert.\n
 * Wird für die Übergabe und den Aufruf der Callback-Funktion des  TimerB Capture-Moduls
 * eingesetzt.
 */
typedef void (*MCUTimer2_Cap_CBF)(uint16);

/**
 * Typdefinition eines Funktionszeigers ohne Parameter und Rückgabewert.\n
 * Wird für die Übergabe und den Aufruf der Callback-Funktion des  TimerB Compare-Moduls
 * eingesetzt.
 */
typedef void (*MCUTimer2_Com_CBF)(void);

/*
*********************************************************************************************************
*                                               GLOBALE VARIABLEN
*********************************************************************************************************
*/

/**
 * Globale Systemzeit mit  Auflösung von ca. 65 ms (entspricht 2^16 * 1us)
 */
extern uint32 GlobalTimeHigh;

/*
*********************************************************************************************************
*                                               GLOBALE FUNKTIONENSDEKLARATIONEN
*********************************************************************************************************
*/

void MCUTimer2_Init(void);
extern uint16 GetGlobalTimeLow(void);
void MCUTimer2CompareCC0(MCUTimer2_Com_CBF com0_cbf, uint16 event_time);
void MCUTimer2CompareCC1(MCUTimer2_Com_CBF com1_cbf, uint16 event_time);
void MCUTimer2CaptureCC3(MCUTimer2_Cap_CBF cap3_cbf, uint8 edge);
void TimerClearCallBack(void);
void ResetTimer(void);
void StartTimer(void);


#endif