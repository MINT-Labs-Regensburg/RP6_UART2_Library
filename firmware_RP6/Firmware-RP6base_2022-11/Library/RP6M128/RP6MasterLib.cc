/*******************************************************************************
 * RP6 C-Control PRO M128 Erweiterungsmodul
 * ----------------------------------------------------------------------------
 * RP6 Master Library
 * ----------------------------------------------------------------------------
 * In diese Datei werden immer wieder benötigte Funktionen ausgelagert um
 * das Hauptprogramm übersichtlicher zu halten.
 ******************************************************************************/

#ifndef __MASTER_LIB__     // Alles nur EINMAL einbinden!
#define __MASTER_LIB__

//#include "RP6CCLib.cc"


// I2C Bus Adresse des Controllers auf dem Mainboard:
#define RP6_BASE_ADR 10

/*****************************************************************************/

// LEDs auf dem RP6 Mainboard setzen:
#define setRP6LEDs(__RP6_LEDS__) \
    {RP6_writeCMD_1param(RP6_BASE_ADR, CMD_SETLEDS, (__RP6_LEDS__));}

/*****************************************************************************/
// Interrupts:

byte interrupt_status;
#define MASK_DRIVE 	0x80 // Drive System    (1 Bit)
#define MASK_ACS 	0x60 // ACS Status      (2 Bit)
#define MASK_BUMPER 0x6 // Bumper Status    (2 Bit)
#define MASK_RC5    0x8 // RC5 Reception    (1 Bit)
#define MASK_BATLOW 0x1 // Low Voltage      (1 Bit)

#define MASK_WDT 0x4 // Watchdog      (1 Bit)
#define MASK_WDT_RQ 0x8 // Watchdog Request     (1 Bit)


/*****************************************************************************/
// Sensoren/ADCs:

// Die einzelnen ADC Kanäle bekommen je eine eigene Variable:
word adcBat;                // Akkuspannung
word adcMotorCurrentLeft;   // Motorstrom Links (Messwert!)
word adcMotorCurrentRight;  // Motorstrom Rechts (Messwert!)
word adcLSL;                // Lichtsensor Links
word adcLSR;                // Lichtsensor Rechts
word adc0;                  // Analog/Digital Wandler Kanal 0
word adc1;                  // Analog/Digital Wandler Kanal 1
word status_leds_base;      //Status-LEDs der Base
word SnakeL;                //ADC0_M128
word SnakeR;                //ADC7_M128
word Hydro;                 //ADC2_M128
word Bat2;                  //ADC4_M128
word TasterSnake;           //ADC1_M128

word ButtonM32;             //Taster_M32


// Gemessene Geschwindigkeiten (Messwert! In Zählschritten pro 1/5 Sekunde):
word mleft_speed;  // Links
word mright_speed; // Rechts

// Zurückgelegte Distanz (Messwert!)
word mleft_dist;  // Links
word mright_dist; // Rechts
word mleft_dist_neu;        //Distanz seit letztem Aufruf
word mright_dist_neu;       //Distanz seit letztem Aufruf

// Gewünschte Geschwindigkeit (kein Messwert!):
word mleft_des_speed;   // Links
word mright_des_speed;  // Rechts

// Motor PWM Wert
word mleft_power;   // Links
word mright_power;  // Rechts

// ACS Status:
byte obstacleLeft;  // Hindernis Links
byte obstacleRight; // Hindernis Rechts

// Bumper Status:
byte bumperLeft;    // Links
byte bumperRight;   // Rechts

word bumper_back_left;      //Bumper hinten rechts
word bumper_back_right;     //Bumper hinten links


byte messageBuf[29];

/**
 * Die obigen Variablen aktualisieren sich leider nicht von allein.
 * Man muss dazu diese Funktion aufrufen. Sie liest alle aktuellen Sensorwerte
 * über den I2C Bus aus.
 *
 * Natürlich dauert das eine gewisse Zeit. Man sollte das also nur so oft
 * wie unbedingt notwendig tun!
 * Bei wichtigen Änderungen (Kollision, Hindernis erkannt, Akku leer) wird ausserdem
 * eine Interrupt Leitung gesetzt.
 */
void getAllSensors(void)
{
    ADC_Set(ADC_VREF_VCC,0);        //ADC0_M128
    SnakeL = ADC_Read();

    ADC_Set(ADC_VREF_VCC,7);        //ADC7_M128
    SnakeR = ADC_Read();

    ADC_Set(ADC_VREF_VCC,2);        //ADC2_M128
    Hydro = ADC_Read();

    ADC_Set(ADC_VREF_VCC,4);        //ADC4_M128
    Bat2 = ADC_Read();

    ADC_Set(ADC_VREF_VCC,1);        //ADC1_M128
    TasterSnake = ADC_Read();


    RP6_readRegisters(RP6_BASE_ADR, I2C_REG_POWER_LEFT, messageBuf, 29);
    mleft_power = messageBuf[0];
    mright_power = messageBuf[1];
    mleft_speed = messageBuf[2];
    mright_speed = messageBuf[3];
    mleft_des_speed = messageBuf[4];
    mright_des_speed = messageBuf[5];
    mleft_dist = messageBuf[6] + (messageBuf[7]<<8);
    mright_dist = messageBuf[8] + (messageBuf[9]<<8);
    adcLSL = messageBuf[10] + (messageBuf[11]<<8);
    adcLSR = messageBuf[12] + (messageBuf[13]<<8);
    adcMotorCurrentLeft = messageBuf[14] + (messageBuf[15]<<8);
    adcMotorCurrentRight = messageBuf[16] + (messageBuf[17]<<8);
    adcBat = messageBuf[18] + (messageBuf[19]<<8);
    adc0 = messageBuf[20] + (messageBuf[21]<<8);
    adc1 = messageBuf[22] + (messageBuf[23]<<8);
    status_leds_base = messageBuf[24];
    bumper_back_left = messageBuf[25];
    bumper_back_right = messageBuf[26];
    RC5_data = messageBuf[27];
    RC5_toggle = messageBuf[28];
    //RC5_adr = messageBuf[29];
}

/**
 * Im Gegensatz zur getAllSensors Funktion, liest getLightSensors nur die Werte der zwei
 * Lichtsensoren vom Roboter aus.
 */
void getLightSensors(void)
{
	RP6_readRegisters(RP6_BASE_ADR, I2C_REG_ADC_LSL_L, messageBuf, 4);
	adcLSL = messageBuf[0] + (messageBuf[1]<<8);
	adcLSR = messageBuf[2] + (messageBuf[3]<<8);
}



/*****************************************************************************/
// Rotations Funktion

byte transmit_buffer[10]; // temporärer Buffer für die I2C Kommunikation


 /**
 * Diese Funktion setzt die Motorgeschwindigkeit auf einen bestimmten Wert.
 * Dieser wird dann fortlaufend eingeregelt (d.h. der Roboter versucht
 * die Geschwindigkeit möglichst konstant zu halten.)
 * Die Geschwindigkeit wird in "Encoder Zählschritten" pro 0.2 Sekunden
 * eingestellt (s. a. RP6 Anleitung!).
 *
 * Die zurückgelegte Strecke pro Zählschritt variiert von Roboter zu Roboter
 * aufgrund der hohen Auflösung der Encoder spürbar. Man muss durch
 * abfahren von Teststrecken bestimmter Länge die tatsächliche Auflösung
 * ermitteln.
 * Meist liegt diese im Bereich von 0.235mm bis 0.25mm pro Zählschritt.
 * Die Encoder liefern 625 Zählschritte pro Radumdrehung.
 *
 * Der Controller auf dem Mainboard aktualisiert die gemessenen
 * Geschwindigkeitswerte standardmäßig alle 200ms = 0.2s also
 * mit einer Frequenz von 5Hz.
 *
 * Die tatsächliche Geschwindigkeit errechnet sich bei einer
 * Auflösung von 0.25mm daher wie folgt:
 * speed = encoder_value * 0.25mm * 5 Hz  =
 *       = encoder_value * 1.25mm *   1/s = encoder_value * 0.125cm/s.
 * "encoder_value" ist hier die Zahl der Impulse die von den Encodern
 * zurückgeliefert werden.
 * Um aus einer gemessenen Geschwindigkeit in Encoder Zählschritten
 * also eine Geschwindigkeit in cm/s abzuleiten, muss man einfach den Messwert
 * mit 0.125 multiplizieren (sofern die Encoder Auflösung 0.25mm ist)
 *
 *
 * Beispiel für moveAtSpeed: Sie möchten den Roboter mit 10cm/s
 * fahren lassen.
 * Also:
 * 10cm/s = encoder_value * 0.125cm/s
 * => encoder_value = 10 / 0.125 = 80
 *
 * Im Programm schreibt man dann also:
 * moveAtSpeed(80,80); // Mit etwa 10cm/s geradeaus fahren!
 *
 */
void moveAtSpeed(byte desired_speed_left, byte desired_speed_right)
{
	RP6_writeCMD_2params(RP6_BASE_ADR, CMD_MOVE_AT_SPEED, desired_speed_left, desired_speed_right);
}



// Status des Antriebssystems:
byte movementComplete;  // Bewegungsvorgang abgeschlossen
byte motorOvercurrent;  // Überstromabschaltung der Motoren


// Bewegungsrichtungen:
#define FWD 0
#define BWD 1
#define LEFT 2
#define RIGHT 3

#define NON_BLOCKING 0
#define BLOCKING 1

/**
 * Diese Funktion veranlasst den Roboter mit einer bestimmten Geschwindigkeit
 * (desired_speed) in eine vorgegebene Richtung (dir) um einen bestimmten
 * Winkel (angle) auf der Stelle zu rotieren.
 * Mit dem Parameter "blocking" kann bestimmt werden, ob die Funktion
 * blockieren soll bis die Bewegung vom Slave Controller als abgeschlossen
 * gemeldet wurde. Dies ist für einfache Bewegungssequenzen hilfreich!
 *
 * Beispiel:
 * RP6_rotate(80,LEFT,100, BLOCKING); // mit 10cm/s um 100° nach links rotieren
 * RP6_rotate(40,RIGHT,50, BLOCKING); // mit 5cm/s um 50° nach rechts rotieren
 *
 */
void RP6_rotate(byte desired_speed, byte dir, word angle, byte blocking)
{
    transmit_buffer[0] = desired_speed;
    transmit_buffer[1] = dir;
    transmit_buffer[2] = ((angle>>8) & 0xFF);
    transmit_buffer[3] = (angle & 0xFF);
    movementComplete = false;
    RP6_writeCommand_params(RP6_BASE_ADR, CMD_ROTATE,
                             transmit_buffer, 4);
    while(blocking && !movementComplete) {
        Thread_Delay(1);
    }
}

/**
 * Diese Funktion veranlasst den Roboter mit einer bestimmten Geschwindigkeit
 * (desired_speed) in eine vorgegebene Richtung (dir) eine Strecke (distance)
 * abzufahren. Der Parameter distance hat hier die Einheit
 * "Encoder Zählschritte". D.h. es ist von der Auflösung der Encoder abhängig
 * wie weit der Roboter genau fährt. Sind es genau 0.25mm pro Zählschritt,
 * würde er bei 4000 einen Meter weit fahren.
 * Mit dem Parameter "blocking" kann bestimmt werden, ob die Funktion
 * blockieren soll bis die Bewegung vom Slave Controller als abgeschlossen
 * gemeldet wurde. Dies ist für einfache Bewegungssequenzen hilfreich!
 *
 * Beispiel:
 * RP6_move(80,FWD,1000, BLOCKING); // mit 10cm/s  25cm geradeaus fahren
 * RP6_rotate(40,BWD,4000, BLOCKING); // mit 5cm/s einen Meter rückwärts fahren
 *
 */
void RP6_move(byte desired_speed, byte dir, word distance, byte blocking)
{
    transmit_buffer[0] = desired_speed;
    transmit_buffer[1] = dir;
    transmit_buffer[2] = ((distance>>8) & 0xFF);
    transmit_buffer[3] = (distance & 0xFF);
    movementComplete = false;
    RP6_writeCommand_params(RP6_BASE_ADR, CMD_MOVE,
                             transmit_buffer, 4);

    while(blocking && !movementComplete) {
        Thread_Delay(1);
    }
}

/**
 * Identisch zu RP6_move, allerdings wird hierbei die Distanz direkt
 * in mm angegeben.
 * Maximalwert: 15000mm = 1500cm = 15m.
 * Diese Begrenzung ist notwendig, da "word" nur bis max. 65535 reicht.
 * Durch die Umrechnung in mm wird der Wert je nach eingestellter
 * Auflösung sehr viel größer. 15000 / 65535 ~= 0.229  d.h. die minimal
 * verwendbare Auflösung beträgt ungefähr 0.23mm pro Zählschritt.
 *
 * Beispiel:
 * RP6_move_mm(80,FWD,1000, BLOCKING); // mit 10cm/s 1m geradeaus fahren
 */
void RP6_move_mm(byte desired_speed, byte dir, word distance_mm, byte blocking)
{
    if(distance_mm > 15000) distance_mm = 15000;
    word distance;
    distance = DIST_MM(distance_mm);
    RP6_move(desired_speed,dir,distance,blocking);
}


/**
 * Die (dauerhafte) Bewegungsrichtung des Roboters wird mit dieser Funktion geändert.
 *
 * Beispiel:
 * changeDirection(LEFT);   // nach links drehen
 * changeDirection(BWD);    // Rückwärts
 * changeDirection(RIGHT);  // Rechts
 * changeDirection(FWD);    // Vorwärts
 */
void changeDirection(byte dir)
{
    RP6_writeCMD_1param(RP6_BASE_ADR, CMD_CHANGE_DIR, dir);
}

/**
 * Die aktuelle Bewegung des Roboter kann man mit dieser Funktion sofort stoppen.
 */
void stop(void)
{
	RP6_writeCMD(RP6_BASE_ADR, CMD_STOP);
}





/*****************************************************************************/
// Variablen für empfangene RC5 Codes:

byte RC5_toggle;    // Toggle Bits
byte RC5_adr;       // Geräteadresse
byte RC5_data;      // Tastencode



#endif
