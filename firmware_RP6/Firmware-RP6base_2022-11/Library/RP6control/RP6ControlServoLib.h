/* **************************************************************************** 
 *                           _______________________ 
 *                           \| RP6  ROBOT SYSTEM |/ 
 *                            \_-_-_-_-_-_-_-_-_-_/             >>> RP6 CONTROL 
 * ---------------------------------------------------------------------------- 
 * ------------------------ [c]2008 - Dirk ------------------------------------ 
 * **************************************************************************** 
 * File: RP6ControlServoLib.h 
 * Version: 1.0 
 * Target: RP6 CONTROL - ATMEGA32 @16.00MHz 
 * Author(s): Dirk 
 * **************************************************************************** 
 * Description: 
 * This is the RP6ControlServoLib header file. 
 * You have to include this file, if you want to use the library 
 * RP6ControlServoLib.c in your own projects. 
 * 
 * **************************************************************************** 
 * THE CHANGELOG CAN BE FOUND AT THE END OF THIS FILE! 
 * **************************************************************************** 
 */ 
  
/*****************************************************************************/ 
// Includes: 

#include "RP6ControlLib.h"       // The RP6 Control Library. 
                        // Always needs to be included! 

/*****************************************************************************/ 
// Defines: 

// Servo constants: 
#define SERVO1            0b00000001 
#define SERVO2            0b00000010 
#define SERVO3            0b00000100 
#define SERVO4            0b00001000 
#define SERVO5            0b00010000 
#define SERVO6            0b00100000 
#define SERVO7            0b01000000 
#define SERVO8            0b10000000 

// Servo movement limits (depending on servo type): 
// Standard servos need an impulse every 20ms (50Hz). This impulse must have 
// a length of 1ms (0.7 .. 1ms) to move the servo lever to the left touch 
// and a length of 2ms (2 .. 2.3ms) for moving it to the right touch. In the 
// middle position the servo needs an impulse length of 1.5ms (1.3 .. 1.6ms). 
// If you want to modify the following constants for a certain servo type, 
// you must adapt the LEFT_TOUCH constant first (values ~70 .. 100 = ~0.7 .. 
// 1ms at 100kHz) by using a servo position value (servoX_position) of zero. 
// After that you have two "screws" to adjust the servo movement limits: 
// First you may change the RIGHT_TOUCH constant. If you choose a higher 
// value than 255, you will use 16-bit values. Higher values mean a longer 
// impulse length, but longer impulses than 2.3ms do not make sense. 
// Second you may alter the Timer 1 frequency constant (F_TIMER1). 
// A higher frequency leads to smaller steps of the servo movement. This of 
// course reduces the impulse length and may be compensated again by a higher 
// RIGHT_TOUCH constant. As a possible range of Timer 1 frequency values you 
// may use 50kHz (20us) .. 105.263kHz (9.5us). 
// HINT: If you alter F_TIMER1, you'll have to adapt LEFT_TOUCH and 
//       RIGHT_TOUCH again as you can see in the following table! 
//     Steps ->      9.5      10      12.5   15      17.5   20    [us] 
//  ------------------------------------------------------------------ 
//  LEFT_TOUCH      74      71      57      47      41      35 
//  RIGHT_TOUCH      169      162      129      107      92      80 
//  F_TIMER1      105263   100000   80000   66667   57143   50000 [Hz] 
#define LEFT_TOUCH         61         // Left servo touch      (~0.7ms) 
#define RIGHT_TOUCH         185         // Right servo touch     (~2.3ms) 
//#define MIDDLE_POSITION      150 // Middle position (~1.5ms) 
#define PULSE_REPETITION   20         // Pulse repetition freq. (~50Hz) 
#define F_TIMER1         100000      // Timer 1 frequency     (100kHz) 

// Servo ports: 
#define SERVO1_PULSE_ON      (PORTC |= IO_PC2)   // PC2 
#define SERVO1_PULSE_OFF   (PORTC &= ~IO_PC2) 
#define SERVO2_PULSE_ON      (PORTC |= IO_PC3)   // PC3 
#define SERVO2_PULSE_OFF   (PORTC &= ~IO_PC3) 
#define SERVO3_PULSE_ON      (PORTC |= IO_PC4)   // PC4 
#define SERVO3_PULSE_OFF   (PORTC &= ~IO_PC4) 
#define SERVO4_PULSE_ON      (PORTC |= IO_PC5)   // PC5 
#define SERVO4_PULSE_OFF   (PORTC &= ~IO_PC5) 
#define SERVO5_PULSE_ON      (PORTC |= IO_PC6)   // PC6 
#define SERVO5_PULSE_OFF   (PORTC &= ~IO_PC6) 
#define SERVO6_PULSE_ON      (PORTC |= IO_PC7)   // PC7 
#define SERVO6_PULSE_OFF   (PORTC &= ~IO_PC7) 
#define SERVO7_PULSE_ON      (PORTD |= IO_PD5)   // PD5 
#define SERVO7_PULSE_OFF   (PORTD &= ~IO_PD5) 
#define SERVO8_PULSE_ON      (PORTD |= IO_PD6)   // PD6 
#define SERVO8_PULSE_OFF   (PORTD &= ~IO_PD6) 
// ----------------------------------------------------------- 
// Other possible ports for connecting servos to RP6Control: 
//#define SERVOx_PULSE_ON      (PORTA |= ADC6)      // PA6 
//#define SERVOx_PULSE_OFF   (PORTA &= ~ADC6) 
//#define SERVOx_PULSE_ON      (PORTA |= ADC7)      // PA7 
//#define SERVOx_PULSE_OFF   (PORTA &= ~ADC7) 
// ----------------------------------------------------------- 

/*****************************************************************************/ 
// Variables: 

/*uint16_t servo1_position;      // Servo 1 position [0..RIGHT_TOUCH] 
uint16_t servo2_position;      // Servo 2 position [0..RIGHT_TOUCH] 
uint16_t servo3_position;      // Servo 3 position [0..RIGHT_TOUCH] 
uint16_t servo4_position;      // Servo 4 position [0..RIGHT_TOUCH] 
uint16_t servo5_position;      // Servo 5 position [0..RIGHT_TOUCH] 
uint16_t servo6_position;      // Servo 6 position [0..RIGHT_TOUCH] 
uint16_t servo7_position;      // Servo 7 position [0..RIGHT_TOUCH] 
uint16_t servo8_position;      // Servo 8 position [0..RIGHT_TOUCH] */
uint16_t servo_position[8];

/*****************************************************************************/ 
// Functions: 

void initSERVO(uint8_t servos); 
void startSERVO(void); 
void stopSERVO(void); 
void pulseSERVO(void); 
void task_SERVO(void); 

/****************************************************************************** 
 * Additional info 
 * **************************************************************************** 
 * Changelog: 
 * - v. 1.0 (initial release) 31.12.2008 by Dirk 
 * 
 * **************************************************************************** 
 */ 

/*****************************************************************************/ 
// EOF 
