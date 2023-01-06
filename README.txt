/* ****************************************************************************
 *                                  ___   ___          
 *                                 /   \ /   \         
 *                                (  -  x  +  )             >>> ESP32-CAM
 *                                 \___/ \___/         
 *                                   ARDUINO           
 *                           _______________________
 *                           \| RP6  ROBOT SYSTEM |/
 *                            \_-_-_-_-_-_-_-_-_-_/         >>> RP6-BASE
 *                            
 * ****************************************************************************
 */

   README of Arduino library for the Arexx RP6 Robot base plattform:
=========================================================================

Version:       1.0
Date:          06.01.2023
Author:        Fabian "fabqu" Queck, MINT-Labs Regensburg e.V.
Lizenz:        CC BY-SA 4.0 (see LICENSE.txt)

--------------------------------------------------------------------------

Designed specifically to work with Arexx RP6 Robot Base System
 * ----> https://www.arexx.com/rp6/html/de/
 *
 * RP6-Base Firmware mostly from FabianE. Remotroll V1.3
 * This software runs on an ESP32 and communicates with the RP6-Base 
 * via UART. The RP6 has to run the Remotrol-v1.3-Slave Demo from FabianE.!
 * 
 * For more information on how it works see our Wiki: 
 * https://wiki.mint-labs.de/books/der-rp6-robot-mit-einem-esp-als-wifi-remote
 *

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 /*
   Firmware changes on the RP6-Base Slave:
   [+] added Bumpers left/right at the back to SL4 & SL1, respectively
   [+] added Bumpers back and ADC0/ADC1 to UART-output
   [-] automatic indicators when driving left/right and warning when turning
   [-] switching ACS/Sharps on/off when going forward/backward

   Hardware:
   Connect ESP UART2 Pins via level shifter to UART from RP6
     (RxESP->LevelShifter->TxRP6, TxESP->LevelShifter->RxRP6)
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

// Command Defines
#define CMD_SET_SPEED               1 //#1:100:50:0:id* -> #set speed (1):left (100):right (50):forward (0):id*
#define CMD_SET_SERVO               2 
#define CMD_SET_LEDS                3 //#3:1:2:id* switches LED "2" on RP6Base ("1") ; #3:1:0:id* 
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
*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

--------------------------------------------------------------------------
