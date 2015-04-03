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
*                  Dmytro Krush
*                  dmytro.krush@hsu-hh.de
*             	   (+49)40-6541-2636
*                  
* Version:         $Revision$
*
* Erstellt am:     $Date$
*/

/*! \file          TestRec.c
 *  \brief         
 *
 *   History:   
 */


#include "MCUTimer2.h"


/*
*********************************************************************************************************
*                                               Lokale Variablen
*********************************************************************************************************
*/
__monitor uint8 GetLock(void);
__monitor void ReleaseLock(void);
MCUTimer2_Com_CBF MCUTimer2_Com0_CBF;
MCUTimer2_Com_CBF MCUTimer2_Com1_CBF;
uint8 Lock;
uint16 currenteventmoment = 0;
uint16 SettetTime = 0;
//********************************************************************//  
/** @fn uint16  MCUTimer2_Init()
*  @brief Initialise Timer 2
*  @return none
*/

void MCUTimer2_Init(void)
{
//********************************************************************//   
  EA = 0;
  T2CTRL = STOP;        //Stop Timer2
  T2EVTCFG =  0xFF;
  T2MSEL = T2OVF;
  T2MOVF0 = 0x00;       //Reset OverflowCount 0
  T2MOVF1 = 0x00;       //Reset OverflowCount 1
  T2MOVF2 = 0x00;       //Reset OverflowCount 2
//********************************************************************// 
  T2MSEL = T2_PER;      //Set MUX to Overflov Count and Timer Period (timer period)
  T2M0 = 0x20;          //Set Timer Period to 32 => Overflow after 1us
  T2M1 = 0x00;          //Set Timer Period to 32 => Overflow after 1us
//********************************************************************//  
  T2MSEL = T2TIM;       //Set MUX to Overflov Count and Timer Value (timer count value)
  T2M0   = 0x00;        //Reset Timer Value
  T2M1   = 0x00;        //Reset Timer Value
//********************************************************************// 
  T2MSEL = T2OVF_PER;   //Set MUX to Overflov Count and Timer Period (timer period)
  T2MOVF0 = 0xFF;       //Reset OverflowCount 0
  T2MOVF1 = 0xFF;       //Reset OverflowCount 1
  T2MOVF2 = 0x00;       //Reset OverflowCount 2
  T2IRQM = 0x00;        //Disable all Timer2 Interrupt Sources
  T2IE = 1; 
  T2IF = 0;
  EA = 1;               //Enable Global Interrupts  
  return;
}//MCUTimer2_Init


//********************************************************************//  
/** @fn uint16  GetGlobalTimeLow()
*  @brief gets the actual time in us.
*  @return actulal time
*/
uint16 GetGlobalTimeLow(void)
{
  T2MSEL = T2OVF;       //Setze Multiplexer für T2MOVF0...T2MOVF2 auf OverflowCounter;
  //return (T2MOVF0 + (((uint16)T2MOVF1)<<8));  Warning[Pa082]: undefined behavior: the order of volatile accesses is undefined in this statement D:\PROJECTS\WSAN\CC2541_SENSOR_ACTOR_OLD - Copy\Source\MCUTimer2.c 93 
  
  uint8 time1 = T2MOVF0;  // Without any Warning 
  uint8 time2 = T2MOVF1;
  return (time1 + (((uint16)time2)<<8));
}//GetGlobalTimeLow

//********************************************************************//  
/** @fn void  ResetTimer()
*  @brief resets the timer 
*  @return none
*/

void ResetTimer(void) {
  if (GetLock()) {
     //********************************************************************//   
      T2CTRL = STOP;      //Stop Timer2
      T2MSEL = T2OVF;
      T2MOVF0 = 0x00;     //Reset OverflowCount 0
      T2MOVF1 = 0x00;     //Reset OverflowCount 1
      T2MOVF2 = 0x00;     //Reset OverflowCount 2
    //********************************************************************// 
      T2MSEL = T2_PER;      //Set MUX to Overflov Count and Timer Period
      T2M0 = 0x20;     //Set Timer Period to 32 => Overflow after 1 us
      T2M1 = 0x00;     //Set Timer Period to 32 => Overflow after 1 us
    //********************************************************************//  
      T2MSEL = T2TIM;   //Set MUX to Overflov Count and Timer Value
      T2M0   = 0x00;   //Reset Timer Value
      T2M1   = 0x00;   //Reset Timer Value
    //********************************************************************//  
      T2IRQM = 0x00;   //Disable all Timer2 Interrupt Sources   
      T2IRQF = 0; // Clear All Timer2 Interrupt flags.
      T2IF   = 0;
      ReleaseLock();
    }
}//ResetTimer



//********************************************************************//  
/** @fn void  StartTimer()
*  @brief Starts timer, acivates timer interrupts, clears all interupt flags. 
*  @return none
*/

void StartTimer(void) {
  if (GetLock()) {
    T2IRQM = 0x00;  //Disable all Timer2 Interrupt Sources
    IRCON &= ~0x04;   //Clear Timer2 Interrupt Flag
    IEN1 |= 0x04;    //Enable Timer2 Interrupt
    T2CTRL = RUN;   //Start Timer2
    ReleaseLock();
  }
}//StartTimer


//********************************************************************//  
/** @fn void  MCUTimer2CompareCC0(MCUTimer2_Com_CBF com0_cbf, uint16 latch_time)
*  @brief resets the timer 
*  @param com0_cbf CallBack function, executed on compare. 
*  @param latch_time compare 
*  @return none
*/

void MCUTimer2CompareCC0(MCUTimer2_Com_CBF com0_cbf, uint16 latch_time)
{
  //EA = 0;
  if (GetLock()) { 
    //TimerReceiveTime     = latch_time; 
    T2IF    = 0;                                    //Clear Timer2 Interrupt Flag
    T2IRQF &= TIMER2_OVF_COMPARE1F;                 //Clear Timer2 OverflowCompare1 IRQ Flag
    T2MSEL  = T2OVF_CMP1;                           //Set MUX to Overflov Compare Value1 and Timer Value
    T2MOVF0 = (uint8)(latch_time& 0xFF);            //Set OverflowCount 0
    T2MOVF1 = (uint8)((latch_time >> 8)& 0xFF);     //Set OverflowCount 1
    T2MOVF2 = 0x00;//(uint8)(latch_time >> 16)                     
    T2IRQM |= TIMER2_OVF_COMPARE1M;         //Enable Timer2 OverflowCompare Interrupt 1
    //T2IE    = 1;
    MCUTimer2_Com0_CBF = com0_cbf;
    ReleaseLock();
  }
}//MCUTimer2CompareCC0


//********************************************************************//  
void MCUTimer2CompareCC1(MCUTimer2_Com_CBF com1_cbf, uint16 latch_time)
{
  if (GetLock()) { 
    T2IF    = 0;                             //Clear Timer2 Interrupt Flag
    T2IRQF  &= TIMER2_OVF_COMPARE2F;         //Clear Timer2 OverflowCompare1 IRQ Flag
    T2MSEL  = T2OVF_CMP2;                   //Set MUX to Overflov Compare Value2 and Timer Value
    T2MOVF0 = (uint8)(latch_time & 0xFF);            //Set OverflowCount 0
    T2MOVF1 = (uint8)((latch_time >> 8) & 0xFF);     //Set OverflowCount 1
    T2MOVF2 = 0x00;//(uint8)(latch_time >> 16);
    T2IRQM  |= TIMER2_OVF_COMPARE2M;         //Enable Timer2 OverflowCompare Interrupt 1
    //T2IE    = 1;
    MCUTimer2_Com1_CBF = com1_cbf;
    ReleaseLock();
  }
}//MCUTimer2CompareCC1

//********************************************************************//  
void TimerClearCallBack(void) {
   //GetLock();                           // Block timer interrupts 

   MCUTimer2_Com1_CBF = NULL;           // clear callback function
   MCUTimer2_Com0_CBF = NULL;           // clear callback function
   //T2IE     = 0;                        // disable timer cpu interrupts
   T2IRQM   = 0x00;                     // Disable all Timer2 Interrupt Sources 
   T2IRQF   = 0x00;                     // Clear Timer2 CPU Interrupt Flag                 
   T2IF    = 0;                         // clear cpu flag
  // ReleaseLock();
}//TimerClearCallBack

/***********************************************************************************
* @fn          Timer1_isr
*
* @brief       Interrupt service routine for Timer1 . 
*
* @param       void
*
* @return      void
*/
uint8 status = 0;
#pragma vector=T2_VECTOR
__interrupt void Timer2_isr(void) {
  if (GetLock()) {
    status = T2IRQF;
    T2IRQM  = 0x00;     // mask interrupts        
    T2IRQF  = 0x00;     // clear flags
    T2IF    = 0;
    if(status & TIMER2_OVF_COMPARE1F) {      // independent from counter overflow react ob the Overflow counter compare to start the first CALLBACK function
      ReleaseLock();
      if(MCUTimer2_Com0_CBF != NULL)MCUTimer2_Com0_CBF();                         /* NUR Wenn der Funktionzeiger kein leerer Zeiger ist*/
      //return;
    }
    if(status & TIMER2_OVF_COMPARE2F)   { // If we have received second overflow start the second CALLBACK function
      ReleaseLock();
      if(MCUTimer2_Com1_CBF != NULL)MCUTimer2_Com1_CBF();                         /* NUR Wenn der Funktionzeiger kein leerer Zeiger ist*/
      //return;
    }
   // ReleaseLock();                        // if some interrupt is reisd, and it is invalid timer has to clear Lock too, in another case the system will stick 
  }
  T2IRQF  = 0x00;       // clear IRQ CPU flags
  T2IF    = 0;          // clear CPU flags
}


// /*******************************************************************************
// * @fn           RF_GetLock
// *
// * @brief        takes the semaphore
// *
// * @param        none
// *
// * @return       void
// */
 __monitor uint8 GetLock (void)
 {
     if (Lock == 0)                                                     /* ? Ist Lock Semaphore besetzt ?*/
     {
         Lock = 1;                                                      /* Semaphore ist frei -> Semaphore besetzen */
         return TRUE;
     }
     else                                                                        /* Semaphore ist besetzt  */
     {
         return FALSE;
     }
 } // RF_GetLock

// /*******************************************************************************
// * @fn          	RF_ReleaseLock
// *
// * @brief       	release a semaphore
// *
// * @param        none
// *
// * @return       void
// */
// /**
 // *   Funktion zum Freigeben des Semaphores
 // *   Funktion ist atomic, d.h. sie kann nicht
 // *   durch Interrupt unterbrochen werden.
 // *
 // *   \param NONE
 // *   \return NONE
 // */
 __monitor void ReleaseLock (void)
 {
     Lock = 0;              /* Semaphore freigeben */
 } // RF_ReleaseLock
