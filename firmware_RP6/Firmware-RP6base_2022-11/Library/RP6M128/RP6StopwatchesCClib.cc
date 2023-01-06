 /* ****************************************************************************
 *                           _______________________
 *                           \| RP6  ROBOT SYSTEM |/
 *                            \_-_-_-_-_-_-_-_-_-_/              >>> CCPRO M128
 * ----------------------------------------------------------------------------
 * ----------------------------- [c]2010 - Dirk -------------------------------
 * ****************************************************************************
 * File: RP6StopwatchesCClib.cc
 * Version: 1.0 - CompactC
 * Target: RP6 CCPRO M128 - C-Control PRO M128 @14.7456MHz
 * Author(s): Dirk
 * ****************************************************************************
 * Beschreibung:
 * Dies ist meine Stopwatches Bibliothek für die RP6 CCPRO M128. Die acht
 * Stopwatches funktionieren genau wie die Stopwatches, die mit den
 * Bibliotheken der RP6Base und RP6 Control M32 benutzt werden können.
 * Siehe Abschnitt 4.6.3. der RP6 Robot System Bedienungsanleitung!
 *
 * ****************************************************************************
 * CHANGELOG FINDEN SIE AM ENDE DIESER DATEI!
 * ****************************************************************************
 */

 // Alles nur EINMAL einbinden!
#ifndef __RP6STOPWATCHESCCLIB__
#define __RP6STOPWATCHESCCLIB__


/******************************************************************************/
// Defines:

// Timer-Verwendung (nur EINE dieser drei Definitionen ist erlaubt!):

#define USE_TIMER0
//#define USE_TIMER1
//#define USE_TIMER3

// Stopwatches:

#define STOPWATCH1 1
#define STOPWATCH2 2
#define STOPWATCH3 4
#define STOPWATCH4 8
#define STOPWATCH5 16
#define STOPWATCH6 32
#define STOPWATCH7 64
#define STOPWATCH8 128

#define stopStopwatch1() watches = watches & ~STOPWATCH1
#define stopStopwatch2() watches = watches & ~STOPWATCH2
#define stopStopwatch3() watches = watches & ~STOPWATCH3
#define stopStopwatch4() watches = watches & ~STOPWATCH4
#define stopStopwatch5() watches = watches & ~STOPWATCH5
#define stopStopwatch6() watches = watches & ~STOPWATCH6
#define stopStopwatch7() watches = watches & ~STOPWATCH7
#define stopStopwatch8() watches = watches & ~STOPWATCH8

#define startStopwatch1() watches = watches | STOPWATCH1
#define startStopwatch2() watches = watches | STOPWATCH2
#define startStopwatch3() watches = watches | STOPWATCH3
#define startStopwatch4() watches = watches | STOPWATCH4
#define startStopwatch5() watches = watches | STOPWATCH5
#define startStopwatch6() watches = watches | STOPWATCH6
#define startStopwatch7() watches = watches | STOPWATCH7
#define startStopwatch8() watches = watches | STOPWATCH8

#define isStopwatch1Running() (watches & STOPWATCH1)
#define isStopwatch2Running() (watches & STOPWATCH2)
#define isStopwatch3Running() (watches & STOPWATCH3)
#define isStopwatch4Running() (watches & STOPWATCH4)
#define isStopwatch5Running() (watches & STOPWATCH5)
#define isStopwatch6Running() (watches & STOPWATCH6)
#define isStopwatch7Running() (watches & STOPWATCH7)
#define isStopwatch8Running() (watches & STOPWATCH8)

#define getStopwatch1() watch1
#define getStopwatch2() watch2
#define getStopwatch3() watch3
#define getStopwatch4() watch4
#define getStopwatch5() watch5
#define getStopwatch6() watch6
#define getStopwatch7() watch7
#define getStopwatch8() watch8

#define setStopwatch1(__VALUE__) watch1 = __VALUE__
#define setStopwatch2(__VALUE__) watch2 = __VALUE__
#define setStopwatch3(__VALUE__) watch3 = __VALUE__
#define setStopwatch4(__VALUE__) watch4 = __VALUE__
#define setStopwatch5(__VALUE__) watch5 = __VALUE__
#define setStopwatch6(__VALUE__) watch6 = __VALUE__
#define setStopwatch7(__VALUE__) watch7 = __VALUE__
#define setStopwatch8(__VALUE__) watch8 = __VALUE__

/******************************************************************************/
// Variablen:

// 16 Bit Stopwatches --> Maximalwert ist 65535 oder ~65,535 Sekunden!

byte watches;
unsigned int watch1;
unsigned int watch2;
unsigned int watch3;
unsigned int watch4;
unsigned int watch5;
unsigned int watch6;
unsigned int watch7;
unsigned int watch8;

/******************************************************************************/
// Funktionen:

/**
 * INIT Stopwatches
 *
 * Unbedingt einmal aufrufen, bevor die Stopwatches benutzt werden!
 *
 * ANMERKUNG: Die Zeilen mit dem Kommentar "!!! Bugfix TimerX !!!"
 *            müssen entfernt werden, sobald der Fehler der Funktion
 *            Timer_TXTime() [X = 1 oder 3] in einer neuen Version
 *            der IDE beseitigt wurde!
 */
void initStopwatches(void)
{
    watches = 0;                        // Alle Stopwatches aus
#ifdef USE_TIMER0
    Irq_SetVect(INT_TIM0COMP, INT_1ms); // Timer0-ISR definieren
    Timer_T0Time(230, PS0_64);          // Timer0: 1ms Interrupt
#endif
#ifdef USE_TIMER1
    Irq_SetVect(INT_TIM1CMPA, INT_1ms); // Timer1-ISR definieren
    Timer_T1Time(14746, PS_1);          // Timer1: 1ms Interrupt
    DirAcc_Write(0x4e,8|PS_1);          // !!! Bugfix Timer1 !!!
#endif
#ifdef USE_TIMER3
    Irq_SetVect(INT_TIM3CMPA, INT_1ms); // Timer3-ISR definieren
    Timer_T3Time(14746, PS_1);          // Timer3: 1ms Interrupt
    DirAcc_Write(0x8a,8|PS_1);          // !!! Bugfix Timer3 !!!
#endif
}

/**
 * Interrupt Routine
 *
 */
void INT_1ms(void)
{
    int irqcnt;
   if(watches & STOPWATCH1) watch1++;
   if(watches & STOPWATCH2) watch2++;
   if(watches & STOPWATCH3) watch3++;
   if(watches & STOPWATCH4) watch4++;
   if(watches & STOPWATCH5) watch5++;
   if(watches & STOPWATCH6) watch6++;
   if(watches & STOPWATCH7) watch7++;
   if(watches & STOPWATCH8) watch8++;
#ifdef USE_TIMER0
    irqcnt = Irq_GetCount(INT_TIM0COMP);// Interrupt Request Counter
#endif
#ifdef USE_TIMER1
    irqcnt = Irq_GetCount(INT_TIM1CMPA);
#endif
#ifdef USE_TIMER3
    irqcnt = Irq_GetCount(INT_TIM3CMPA);
#endif
}

#endif

/******************************************************************************
 * Info
 * ****************************************************************************
 * Changelog:
 * - v. 1.0 (initial release) 24.05.2010 by Dirk
 *
 * ****************************************************************************
 */

/*****************************************************************************/
// EOF
