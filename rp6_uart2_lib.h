/*!
 * @file rp6_uart2_lib.h
 *
 * Designed specifically to work with Arexx RP6 Robot Base System
 * ----> https://www.arexx.com/rp6/html/de/
 *
 * RP6-Base Firmware mostly from FabianE. Remotroll V1.3
 * This software runs on an ESP32 and communicates with the RP6-Base 
 * via UART. The RP6 has to run the Remotrol-v1.3-Slave Demo from FabianE.!
 * 
 * See Wiki: 
 * https://wiki.mint-labs.de/books/der-rp6-robot-mit-einem-esp-als-wifi-remote
 *
 *
 * Written by Fabian "fabqu" Queck for MINT-Labs Regensburg e.V.
 *
 * License CC BY-SA 4.0
 *
 */
 
/* ****************************************************************************
 *                                  ___   ___          
 *                                 /   \ /   \         
 *                                (  -  x  +  )             >>> ESP32-CAM
 *                                 \___/ \___/         
 *                                   ARDUINO           
 *                           _______________________
 *                           \| RP6  ROBOT SYSTEM |/
 *                            \_-_-_-_-_-_-_-_-_-_/         >>> BASE CONTROLLER
 *                            
 * ****************************************************************************
 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 /*
   Firmware changes on the RP6-Base Slave:
   [+] added Bumpers left/right at the back to SL4 & SL1, respectively
   [+] added Bumpers back and ADC0/ADC1 to UART-output
   [-] automatic indicators when driving left/right and warning when turning
   [-] switching ACS/Sharps on/off when going forward/backward

   Hardware:
   Connect ESP UART2 Pins via level shifter to UART from RP6 (RxESP->LevelShifter->TxRP6, TxESP->LevelShifter->RxRP6)
   Connect ST1 from RP6 via level shifter to ESP startPin (high on ST1 starts the RP6-Base)
   Connect ST2 from RP6 via level shifter to ESP resetPin (low on ST2 resets the RP6-Base)
   Connect GND from RP6 to GND from ESP
   !done!
   
   This library:
   [+] Sends commands for Speed and lights to the RP6
   [+] can start and reset the RP6-Base
   [+] reads sensor data via UART from RP6, which comes periodically in the following format:
        [DATA]
        Bat:955
        SpeedL:0
        SpeedR:0
        PowerL:0
        PowerR:2
        LightL:592
        LightR:865
        BumpL:0
        BumpR:0
        ObsL:1
        ObsR:1
        ADC0:860
        ADC1:855
        BumpHL:0
        BumpHR:0
        [/DATA]
*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef RP6_UART2_LIB_H
#define RP6_UART2_LIB_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// RP6 standard components:
// - RP6base or RP6base_v2
// - Firmware Remotroll V1.3 from FabianE.

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// Includes:
#include <Arduino.h>
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// Command Defines
#define CMD_SET_SPEED               1 //#1:100:50:0:id* -> #set speed (1):left (100):right (50):forward (0):id*
#define CMD_SET_SERVO               2 
#define CMD_SET_LEDS                3 //#3:1:2:id* switches on the LED "2" on RP6Base ("1"), all others off ; #3:1:0:id* 
#define CMD_SET_BEEP                4
#define CMD_SET_START_MELODY        5
#define CMD_SET_FEATURE             6
#define CMD_SET_STOP                7
#define CMD_SET_CONNECTION_SPEED    8
#define CMD_SET_MELODY              9
#define CMD_SET_ACSPOWER           10
#define CMD_SET_TEST               11
#define CMD_SET_CMD_SET_IRCOM      12
#define CMD_RESET_ID_COUNTER       99
#define CMD_DIR_FWD                 0
#define CMD_DIR_BWD                 1
#define CMD_DIR_TURN_L              2
#define CMD_DIR_TURN_R              3
#define CMD_FLAG_STP                0
#define CMD_FLAG_MOVE               1
#define CMD_RP6_BASE                1 // For LEDs, Servos, ...
#define CMD_RP6_M32                 2 // For LEDs, Servos, ...
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// struct for Data from the RP6
struct DataArray{
  int Bat;
  int SpeedL;
  int SpeedR;
  int PowerL;
  int PowerR;
  int LightL;
  int LightR;
  int BumpL;
  int BumpR;
  int ObsL;
  int ObsR;
  int ADC0;
  int ADC1;
  int BumpHL;
  int BumpHR;
};
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~ Class for the RP6 functions
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
class RP6 {
  public:  // public members of the class, globally available
    RP6(); //constructor
    ~RP6(); //destructor, when the class is destructed
    void begin(int startPin, int resetPin, int RXD2, int TXD2);  //init RP6
    void start();   // start RP6
    void reset();   // reset RP6
    DataArray getData(); //pointer to Array
    void stop();    // stop engines
    void drive(int speedL, int speedR, int dir); // drive with speed left/right in direction "dir"
    void setLEDs(bool sl1, bool sl2, bool sl3, bool sl4, bool sl5, bool sl6); 
    void setLED(int sl, bool onoff); 
  
  private:  // private members of the class, only inside the class available
    int getIDcounter();	  // get the correct ID counter
	void parseData();     // parse the data from getData() public function
    int _startPin;        // connect to ST1 to start the RP6
    int _resetPin;        // connect to ST2 to reset the RP6
    int _RXD2;            // for second UART
    int _TXD2;            // for second UART
	int _id_counter;
    String _dataReceived;
    DataArray _dataRP6;
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#endif //RP6_UART2_LIB_H

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~ Additional info
 ~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~ Changelog:
 ~ 
 ~ 
 ~ 
 ~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
