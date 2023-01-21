/* ****************************************************************************
 *                                  ___   ___          
 *                                 /   \ /   \         
 *                                (  -  x  +  )             >>> ESP32
 *                                 \___/ \___/         
 *                                   ARDUINO           
 *                           _______________________
 *                           \| RP6  ROBOT SYSTEM |/
 *                            \_-_-_-_-_-_-_-_-_-_/         >>> BASE CONTROLLER
 *                            
 * ****************************************************************************
 */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// include the library
#include "rp6_uart2_lib.h"

bool sLEDs[6] = {false,false,false,false,false,false};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// constructor
RP6::RP6(){
  _id_counter = 0;
  _dataReceived = "";
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// destructor
RP6::~RP6(){
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// initialization
void RP6::begin(int startPin, int resetPin, int RXD2, int TXD2){
  _startPin = startPin;   // high on ST1 starts the RP6-Base; level shifter recommended
  _resetPin = resetPin;   // low on ST2 resets the RP6-Base; no need for a level shifter
  _RXD2 = RXD2;           // for second uart; connected via level shifter
  _TXD2 = TXD2;           // for second uart; connected via level shifter
  pinMode(_startPin, OUTPUT);
  pinMode(_resetPin, OUTPUT);
  digitalWrite(_resetPin, HIGH);
  digitalWrite(_startPin, LOW);
  Serial2.begin(38400, SERIAL_8N1, _RXD2, _TXD2); //second Serial on pins 16/17 for RP6-Com
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// get the correct ID counter
int RP6::getIDcounter(){
	_id_counter++;
	if (_id_counter >= 99) {
		_id_counter = 0;
		Serial2.println("#99:99*");
		return _id_counter;
	}
	else return _id_counter;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// start the RP6
void RP6::start(){
  _id_counter = 0; // id_counter zurücksetzen!
  digitalWrite(_startPin, HIGH);
  delay(200);
  digitalWrite(_startPin, LOW);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// reset the RP6
void RP6::reset(){
  digitalWrite(_resetPin, LOW);
  delay(200);
  digitalWrite(_resetPin, HIGH);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// stop the drive system
void RP6::stop(){
  Serial2.println("#" + String(CMD_SET_STOP) + ":" + String(getIDcounter()) + "*");
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// drive any direction
void RP6::drive(int speedL, int speedR, int dir){
  Serial2.println("#" + String(CMD_SET_SPEED) + ":" + String(speedL) + ":" + String(speedR) + ":" + String(dir) + ":" + String(getIDcounter()) + "*");
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// set the six status LEDs, called SL1 to SL6
void RP6::setLEDs(bool sl1, bool sl2, bool sl3, bool sl4, bool sl5, bool sl6){  
  Serial2.println("#" + String(CMD_SET_LEDS) + ":" + String(CMD_RP6_BASE) + ":" + String(sl1|sl2<<1|sl3<<2|sl4<<3|sl5<<4|sl6<<5) + ":" + String(getIDcounter()) + "*");
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// set one of the six status LEDs, called SL1 to SL6
void RP6::setLED(int sl, bool onoff){  
  sLEDs[sl-1] = onoff;
  Serial2.println("#" + String(CMD_SET_LEDS) + ":" + String(CMD_RP6_BASE) + ":" + String(sLEDs[0]|sLEDs[1]<<1|sLEDs[2]<<2|sLEDs[3]<<3|sLEDs[4]<<4|sLEDs[5]<<5) + ":" + String(getIDcounter()) + "*");
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// get data from the RP6
DataArray RP6::getData(){ 
    if(Serial2.available()){
    char c = Serial2.read();

    if(c == '\n'){
      parseData();
      _dataReceived = "";
    }
    else {
      _dataReceived += c;
    }
  }
  return _dataRP6;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// parse the data that was read
void RP6::parseData(){
  String part1;
  String part2;
  DataArray dataRP6 = _dataRP6;
  if (_dataReceived == "[DATA]" || _dataReceived == "[/DATA]"){
    //nothing
  }
  else {
    part1 = _dataReceived.substring(0, _dataReceived.indexOf(":"));
    part2 = _dataReceived.substring(_dataReceived.indexOf(":")+1);
    

    if (part1 == "Bat"){
      dataRP6.Bat = part2.toInt();
    } else if (part1 == "SpeedL"){
      dataRP6.SpeedL = part2.toInt();
    } else if (part1 == "SpeedR"){
      dataRP6.SpeedR = part2.toInt();
    } else if (part1 == "PowerL"){
      dataRP6.PowerL = part2.toInt();
    } else if (part1 == "PowerR"){
      dataRP6.PowerR = part2.toInt();
    } else if (part1 == "LightL"){
      dataRP6.LightL = part2.toInt();
    } else if (part1 == "LightR"){
      dataRP6.LightR = part2.toInt();
    } else if (part1 == "BumpL"){
      dataRP6.BumpL = part2.toInt();
    } else if (part1 == "BumpR"){
      dataRP6.BumpR = part2.toInt();
    } else if (part1 == "ObsL"){
      dataRP6.ObsL = part2.toInt();
    } else if (part1 == "ObsR"){
      dataRP6.ObsR = part2.toInt();
    } else if (part1 == "ADC0"){
      dataRP6.ADC0 = part2.toInt();
    } else if (part1 == "ADC1"){
      dataRP6.ADC1 = part2.toInt();
    } else if (part1 == "BumpHL"){
      dataRP6.BumpHL = part2.toInt();
    } else if (part1 == "BumpHR"){
      dataRP6.BumpHR = part2.toInt();
    }
    _dataRP6 = dataRP6;
  }
}
