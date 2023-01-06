////////////////////////////////////////////////////////
//            RP6 Master on ESP via UART2             //
////////////////////////////////////////////////////////
/*
   RP6-Base Firmware mostly from FabianE. Remotroll V1.3
   This software runs on an ESP32 and communicates with
   the RP6-Base via UART. The RP6 has to run the
   Remotrol-v1.3-Slave Demo from FabianE.!

   
   For more information see the Wiki:
   https://wiki.mint-labs.de/books/der-rp6-robot-mit-einem-esp-als-wifi-remote

   Hardware:
   Connect ESP UART2 Pins via level shifter to UART from RP6
     (RxESP->LevelShifter->TxRP6, TxESP->LevelShifter->RxRP6)
   Connect ST1 from RP6 via level shifter to ESP startPin (high on ST1 starts the RP6-Base)
   Connect ST2 from RP6 via level shifter to ESP resetPin (low on ST2 resets the RP6-Base)
   Connect GND from RP6 to GND from ESP
   !done!
*/

//////////////////////////////////////////////
//        RP6-UART include library          //
//////////////////////////////////////////////

#include <rp6_uart2_lib.h>

// UART2 defines; default ist RXD2=16, TXD2=17 but you can map it to any pin
#define RXD2 16                   // for second uart; connected via level shifter
#define TXD2 17                   // for second uart; connected via level shifter
// RP6 start and reset defines;
#define startPin 19               // high on ST1 starts the RP6-Base; level shifter recommended
#define resetPin 21               // low on ST2 resets the RP6-Base; no need for a level shifter

//Status LED defines
#define LED_SL1                     1 // Status LED 1 (IO1)  on RP6Base, Bumper back right   & blinker right (yellow)
#define LED_SL2                     2 // Status LED 2 (IO2)  on RP6Base, nothing as input    & light low (purple)
#define LED_SL3                     3 // Status LED 3 (LIO1) on RP6Base, Bumper front right  & IR Back on/off
#define LED_SL4                     4 // Status LED 4 (IO5)  on RP6Base, Bumper back left    & blinker left (orange)
#define LED_SL5                     5 // Status LED 5 (IO4)  on RP6Base, nothing as input    & light high (green)
#define LED_SL6                     6 // Status LED 6 (LIO2) on RP6Base, Bumper front left   & nothing as output

// make a new objekt of the RP6 class
RP6 rp6;

/////////////////////////////////////////////
//           END RP6-UART include          //
/////////////////////////////////////////////


void setup()
{
  Serial.begin(115200);   //serial for PC-Com
  
  rp6.begin(startPin, resetPin, RXD2, TXD2);
  
}

void loop()
{
  rp6.reset();              // reset the RP6
  Serial.println("RP6 was resettet");
  delay(2000);
  rp6.start();              // start the RP6
  Serial.println("RP6 was freshly started");
  delay(2000);
                             
  Serial.println("ATTENTION!!!");
  Serial.println("The RP6 will drive in three seconds!");
  delay(1000);
  Serial.println("The RP6 will drive in two seconds!");
  delay(1000);
  Serial.println("The RP6 will drive in one second!");
  delay(1000);
  rp6.drive(50,75,CMD_DIR_FWD);       // drive forward with speed 50 right, 75 left (from 200)
  delay(1000);
  Serial.println("The RP6 will stop in three seconds.");
  delay(1000);
  Serial.println("The RP6 will stop in two seconds.");
  delay(1000);
  Serial.println("The RP6 will stop in one seconds.");
  delay(1000);
  rp6.stop();               // stop the RP6
  delay(1000);
  
  // set all six status LEDs on, then off
  Serial.println("All status LEDs will be switched on, then off.");
  delay(1000);
  rp6.setLED(1,true);
  delay(500);
  rp6.setLED(2,true);
  delay(500);
  rp6.setLED(3,true);
  delay(500);
  rp6.setLED(4,true);
  delay(500);
  rp6.setLED(5,true);
  delay(500);
  rp6.setLED(6,true);
  delay(2000);
  rp6.setLED(1,false);
  delay(500);
  rp6.setLED(2,false);
  delay(500);
  rp6.setLED(3,false);
  delay(500);
  rp6.setLED(4,false);
  delay(500);
  rp6.setLED(5,false);
  delay(500);
  rp6.setLED(6,false);
  delay(2000);
  Serial.println("All status LEDs can be switched on or off together, too.");
  rp6.setLEDs(0,0,0,0,0,0);
  delay(500);
  rp6.setLEDs(1,0,1,0,1,0);
  delay(500);
  rp6.setLEDs(0,0,0,0,0,0);
  delay(500);
  rp6.setLEDs(0,1,0,1,0,1);
  delay(500);
  rp6.setLEDs(0,0,0,0,0,0);
  delay(500);
  

  // print all available sensor values from the RP6
  Serial.println("All available sensor data from the RP6 will be displayed:");
  DataArray dataRP6 = rp6.getData();
  Serial.print("Battery: ");
  Serial.println(dataRP6.Bat);
  Serial.print("SpeedL: ");
  Serial.println(dataRP6.SpeedL);
  Serial.print("SpeedR: ");
  Serial.println(dataRP6.SpeedR);
  Serial.print("PowerL: ");
  Serial.println(dataRP6.PowerL);
  Serial.print("PowerR: ");
  Serial.println(dataRP6.PowerR);
  Serial.print("LightL: ");
  Serial.println(dataRP6.LightL);
  Serial.print("LightR: ");
  Serial.println(dataRP6.LightR);
  Serial.print("BumpL: ");
  Serial.println(dataRP6.BumpL);
  Serial.print("BumpR: ");
  Serial.println(dataRP6.BumpR);
  Serial.print("ObsL: ");
  Serial.println(dataRP6.ObsL);
  Serial.print("ObsR: ");
  Serial.println(dataRP6.ObsR);
  Serial.print("ADC0: ");
  Serial.println(dataRP6.ADC0);
  Serial.print("ADC1: ");
  Serial.println(dataRP6.ADC1);
  Serial.print("BumpHL: ");
  Serial.println(dataRP6.BumpHL);
  Serial.print("BumpHR: ");
  Serial.println(dataRP6.BumpHR);
}
