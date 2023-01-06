/* **************************************************************************** 
 *                           _______________________ 
 *                           \| RP6  ROBOT SYSTEM |/ 
 *                            \_-_-_-_-_-_-_-_-_-_/             >>> RP6 CONTROL 
 * ---------------------------------------------------------------------------- 
 * ------------------------ [c]2008 - Dirk ------------------------------------ 
 * **************************************************************************** 
 * File: RP6ControlServoLib.c 
 * Version: 1.0 
 * Target: RP6 CONTROL - ATMEGA32 @16.00MHz 
 * Author(s): Dirk 
 * **************************************************************************** 
 * Description: 
 * This is my simple RP6 Control Servo Library for up to 8 Servos. 
 * 
 * COMMENT: It is a good idea to use a separate power supply for the servos! 
 * 
 * Servo connections: 
 *         SERVO1 -> I/O Pin 7 (IO_PC2)      SERVO5 -> I/O Pin 4 (IO_PC6) 
 *         SERVO2 -> I/O Pin 5 (IO_PC3)      SERVO6 -> I/O Pin 1 (IO_PC7) 
 *         SERVO3 -> I/O Pin 6 (IO_PC4)      SERVO7 -> I/O Pin 9 (IO_PD5) 
 *         SERVO4 -> I/O Pin 3 (IO_PC5)      SERVO8 -> I/O Pin 8 (IO_PD6) 
 * 
 * **************************************************************************** 
 * ATTENTION: Stopwatch 7 is used for the servo task! Please do 
 *            not use this stopwatch elsewhere in your program! 
 * 
 * **************************************************************************** 
 * THE CHANGELOG CAN BE FOUND AT THE END OF THIS FILE! 
 * **************************************************************************** 
 */ 
  
/*****************************************************************************/ 
// Includes: 

#include "RP6ControlServoLib.h" 

/*****************************************************************************/ 
// Variables: 

uint8_t usedservos; 
uint8_t servo_on = FALSE; 

uint16_t impulselength1 = 0; 
uint16_t impulselength2 = 0; 
uint16_t impulselength3 = 0; 
uint16_t impulselength4 = 0; 
uint16_t impulselength5 = 0; 
uint16_t impulselength6 = 0; 
uint16_t impulselength7 = 0; 
uint16_t impulselength8 = 0; 

volatile uint16_t intcounter = 0; 

/*****************************************************************************/ 
// Functions: 

/** 
 * INIT SERVO 
 * 
 * Call this once before using the servo function. 
 * Timer 1 is configured to work in "Clear Timer On 
 * Compare Match Mode" (CTC). So no PWM is generated! 
 * The timer runs on a fixed frequency (100kHz). 
 * 
 * Input:  servos -> Used servos 
 *         Examples: 
 *         - initSERVO(SERVO1 | SERVO2) -> Use only servos 1 and 2 
 *         - initSERVO(SERVO1 | SERVO6) -> Use only servos 1 and 6 
 *         - initSERVO(SERVO1 | SERVO2 | SERVO8) -> Use servos 1, 2 and 8 
 * 
 */ 
void initSERVO(uint8_t servos) 
{ 
   usedservos = servos;            // Save used servos 
   impulselength1 = 0; 
   impulselength2 = 0; 
   impulselength3 = 0; 
   impulselength4 = 0; 
   impulselength5 = 0; 
   impulselength6 = 0; 
   impulselength7 = 0; 
   impulselength8 = 0; 
   if (servos & SERVO1) {DDRC |= IO_PC2; PORTC &= ~IO_PC2;} 
   if (servos & SERVO2) {DDRC |= IO_PC3; PORTC &= ~IO_PC3;} 
   if (servos & SERVO3) {DDRC |= IO_PC4; PORTC &= ~IO_PC4;} 
   if (servos & SERVO4) {DDRC |= IO_PC5; PORTC &= ~IO_PC5;} 
   if (servos & SERVO5) {DDRC |= IO_PC6; PORTC &= ~IO_PC6;} 
   if (servos & SERVO6) {DDRC |= IO_PC7; PORTC &= ~IO_PC7;} 
   if (servos & SERVO7) {DDRD |= IO_PD5; PORTD &= ~IO_PD5;} 
   if (servos & SERVO8) {DDRD |= IO_PD6; PORTD &= ~IO_PD6;} 
// ----------------------------------------------------------- 
// Other possible ports for connecting servos to RP6Control: 
//   if (servos & SERVOx) {DDRA |= ADC6; PORTA &= ~ADC6;} 
//   if (servos & SERVOx) {DDRA |= ADC7; PORTA &= ~ADC7;} 
// ----------------------------------------------------------- 
   cli(); 
   // Timer 1: Normal port operation, mode 4 (CTC), clk/8 
   TCCR1A =  (0 << COM1A1) 
         | (0 << COM1A0) 
         | (0 << COM1B1) 
         | (0 << COM1B0) 
         | (0 << FOC1A) 
         | (0 << FOC1B) 
         | (0 << WGM11) 
         | (0 << WGM10); 
   TCCR1B =  (0 << ICNC1) 
         | (0 << ICES1) 
         | (0 << WGM13) 
         | (1 << WGM12) 
         | (0 << CS12) 
         | (1 << CS11) 
         | (0 << CS10); 
   OCR1A = ((F_CPU/8/F_TIMER1)-1);   // 19 at 100kHz 
// ------------------------------------------------------ 
// Possible OCR1A values (F_CPU = 16000000): 
//   OCR1A = 2000000 / F_TIMER1 - 1   // F_TIMER1  (Steps) 
//   OCR1A = 18;                  // 105263Hz  (9.5us) 
//   OCR1A = 19;                  // 100000Hz   (10us) 
//   OCR1A = 24;                  //  80000Hz (12.5us) 
//   OCR1A = 29;                  //  66667Hz   (15us) 
//   OCR1A = 34;                  //  57143Hz (17.5us) 
//   OCR1A = 39;                  //  50000Hz   (20us) 
// ------------------------------------------------------ 
   // Enable output compare A match interrupts: 
   startSERVO(); 
   sei(); 
   startStopwatch7();               // Needed for 20ms pulse repetition 
} 

/** 
 * START SERVO 
 * 
 * If the servo function was stopped with the 
 * function stopSERVO() before, it can be 
 * started again with this function. 
 * 
 */ 
void startSERVO(void) 
{ 
   TIMSK |= (1 << OCIE1A); 
   servo_on = TRUE; 
} 

/** 
 * STOP SERVO 
 * 
 * The servo function uses a certain amount of the 
 * processor's calculating time. If the servos are 
 * not moving for a while, the Timer 1 interrupt 
 * can be stopped with this function. 
 * 
 */ 
void stopSERVO(void) 
{ 
   TIMSK &= ~(1 << OCIE1A); 
   servo_on = FALSE; 
} 

/** 
 * PULSE SERVO 
 * 
 * This is the servo pulse generation. This function 
 * must be called every 20ms (pulse repetition). 
 * 
 * position = 0               : Left touch 
 * position = RIGHT_TOUCH     : Right touch 
 * position = MIDDLE_POSITION : Middle position 
 * 
 * ! Please make sure in your main program, that the ! 
 * ! servo position values (servoX_position) don't   ! 
 * ! exceed RIGHT_TOUCH!!!                           ! 
 * 
 * COMMENT: The pulses are only started here! 
 *          The pulses end in the Timer 1 ISR! 
 * 
 */ 
void pulseSERVO(void) 
{ 
   if (servo_on) { 
      intcounter = RIGHT_TOUCH;      // Avoid interference of Timer 1 ISR! 
                        // (Only necessary, if pulseSERVO() is called 
                        //  from outside of this library!) 
      if (usedservos & SERVO1) { 
         SERVO1_PULSE_ON; impulselength1 = LEFT_TOUCH + servo_position[0];} 
      if (usedservos & SERVO2) { 
         SERVO2_PULSE_ON; impulselength2 = LEFT_TOUCH + servo_position[1];} 
      if (usedservos & SERVO3) { 
         SERVO3_PULSE_ON; impulselength3 = LEFT_TOUCH + servo_position[2];} 
      if (usedservos & SERVO4) { 
         SERVO4_PULSE_ON; impulselength4 = LEFT_TOUCH + servo_position[3];} 
      if (usedservos & SERVO5) { 
         SERVO5_PULSE_ON; impulselength5 = LEFT_TOUCH + servo_position[4];} 
      if (usedservos & SERVO6) { 
         SERVO6_PULSE_ON; impulselength6 = LEFT_TOUCH + servo_position[5];} 
      if (usedservos & SERVO7) { 
         SERVO7_PULSE_ON; impulselength7 = LEFT_TOUCH + servo_position[6];} 
      if (usedservos & SERVO8) { 
         SERVO8_PULSE_ON; impulselength8 = LEFT_TOUCH + servo_position[7];} 
      intcounter = 0; 
   } 
} 

/** 
 * TIMER1 ISR 
 * 
 * In this ISR the servo pulses are finished, if the 
 * correct pulse length of each servo is reached. 
 * 
 */ 
ISR (TIMER1_COMPA_vect) 
{ 
   intcounter++; 
   if (intcounter == impulselength1) {SERVO1_PULSE_OFF;} 
   if (intcounter == impulselength2) {SERVO2_PULSE_OFF;} 
   if (intcounter == impulselength3) {SERVO3_PULSE_OFF;} 
   if (intcounter == impulselength4) {SERVO4_PULSE_OFF;} 
   if (intcounter == impulselength5) {SERVO5_PULSE_OFF;} 
   if (intcounter == impulselength6) {SERVO6_PULSE_OFF;} 
   if (intcounter == impulselength7) {SERVO7_PULSE_OFF;} 
   if (intcounter == impulselength8) {SERVO8_PULSE_OFF;} 
} 

/** 
 * SERVO TASK 
 * 
 * This is the servo task. The task performes the pulse repetition 
 * with the help of a stopwatch. 
 * At the next call of the servo task (earliest about 3ms after the 
 * last servo pulse generation) the compare A match interrupt will 
 * be disabled to reduce the interrupt load. It will be enabled 
 * again after the next pulseSERVO() function call. 
 * 
 */ 
void task_SERVO(void) 
{ 
   if (getStopwatch7() > 2) {TIMSK &= ~(1 << OCIE1A);} 
   if (getStopwatch7() > PULSE_REPETITION) { // Pulse every ~20ms 
      pulseSERVO();               // Servo pulse generation 
      if (servo_on) {TIMSK |= (1 << OCIE1A);} 
      setStopwatch7(0); 
   } 
} 

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
