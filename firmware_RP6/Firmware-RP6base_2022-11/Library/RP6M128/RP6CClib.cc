/* ****************************************************************************
 *                           _______________________
 *                           \| RP6  ROBOT SYSTEM |/
 *                            \_-_-_-_-_-_-_-_-_-_/              >>> CCPRO M128
 * ----------------------------------------------------------------------------
 * ----------------------- [c]2008 - AREXX ENGINEERING ------------------------
 * -------------------------- http://www.arexx.com/ ---------------------------
 * ------------------------- http://www.conrad.com/ ---------------------------
 * ****************************************************************************
 * File: RP6CClib.cc
 * Version: 1.0 - CompactC
 * Target: RP6 CCPRO M128 - C-Control PRO M128 @14.7456MHz
 * Author(s): Dominik S. Herwald
 * ****************************************************************************
 * Beschreibung:
 * Diese Funktionsbibliothek enthält einige nützliche Funktionen die es
 * erleichtern den RP6 mit dem C-Control PRO Modul zu steuern.
 * Diese Bibliothek ist nur eine Ergänzung zu den ohnehin schon durch die
 * C-Control PRO bereitgestellten Bibliotheken.
 *
 * Sie müssen UNBEDINGT das RP6Base_I2CSlave Programm in den Controller auf
 * dem RP6 Mainboard laden. Andernfalls können Sie (offensichtlich) nicht
 * auf die Funktionen des Roboters über den I2C Bus zugreifen.
 *
 * BITTE LESEN SIE DIE RP6 CCPRO M128 ANLEITUNG! DORT FINDEN SIE AUCH
 * EINIGE BEISPIELPROGRAMME UND WEITERE ERLÄUTERUNGEN ZU DIESER BIBLIOTHEK.
 *
 * -----
 * Hinweis: Sie sollten besser nichts an dieser Bibliothek ändern wenn
 * Sie noch keine Programmier Erfahrung haben.
 * Allerdings ist es eine sehr gute Idee die Kommentare zu lesen und die
 * Funktionsweise des Codes nachzuvollziehen um zu verstehen wie alles
 * funktioniert und wie es verwendet werden kann.
 * -----
 *
 * Für Erfahrene Anwender:
 * Der Code dieser Bibliothek funktioniert, ist aber nicht immer optimal.
 * Es gibt einige Verbesserungsmöglichkeiten und man könnte noch zahlreiche
 * weitere Hilfsfunktionen hinzufügen.
 *
 * Natürlich dürfen Sie gerne Ihre Modifikationen mit anderen Anwendern über
 * das Internet austauschen, z.B. im CCPRO oder AREXX Forum!
 *
 * Dieser Code steht unter der Open Source Lizenz GPL - beachten Sie dazu
 * auch den Lizenzhinweis am Ende dieser Datei!
 *
 * ****************************************************************************
 * CHANGELOG UND LIZENZINFORMATIONEN FINDEN SIE AM ENDE DIESER DATEI!
 * ****************************************************************************
 */

#include "RP6StopwatchesCClib.cc"

 // Alles nur EINMAL einbinden!
#ifndef __RP6CCLIB__
#define __RP6CCLIB__


/******************************************************************************/
// C-Control Standard Port Konfiguration auf dem RP6 CCPRO M128 Modul:

#define PORT_LED1 51
#define PORT_LED2 52

#define PORT_SCL 24
#define PORT_SDA 25

#define PORT_START 36

#define PORT_SCK    9
#define PORT_MOSI   10
#define PORT_MISO   11
#define PORT_SS     8
#define PORT_STR    34
#define PORT_LCD_EN 29

#define PORT_TXD0   33
#define PORT_RXD0   32
#define PORT_TXD1   27
#define PORT_RXD1   26

#define PORT_SND    35


#define PORT_PE5_INT 37
#define PORT_PE6_INT 38


// Ein paar Definitionen um den Code lesbarer gestalten zu können:
#define PORT_ON    0
#define PORT_OFF   1
#define PORT_OUT   1
#define PORT_IN    0

#define LF 0x0A
#define CR 0x0D
#define SPACE 0x20

#define LED1 1
#define LED2 2
#define LED3 4
#define LED4 8
#define LED5 16
#define LED6 32


/******************************************************************************/
// Encoder / Drehgeber Auflösung:

/**
 * Hier kann die Encoder Auflösung eingestellt werden, die von den
 * Routinen in dieser Bibliothek und einigen der Beispielprogramme verwendet
 * wird um Encoder Zählschritte in Millimeter umzurechnen.
 *
 * Wie das genau funktioniert, ist in der RP6 Anleitung beschrieben!
 * Normalerweise ist 0.24 bereits ein ausreichend guter Wert, kommt aber
 * auf die Beschaffenheit des Untergrunds an.
 */
//#define ENCODER_RESOLUTION 0.23
//#define ENCODER_RESOLUTION 0.235
#define ENCODER_RESOLUTION 0.24
//#define ENCODER_RESOLUTION 0.25


#define DIST_MM(__DST__) ((__DST__)/(ENCODER_RESOLUTION))
#define DIST_CM(__DST__) ((__DST__)/(ENCODER_RESOLUTION/10))
#define DIST_M(__DST__) ((__DST__)/(ENCODER_RESOLUTION/1000))


/******************************************************************************/
// Textausgabe Makros:

char __tmp_txt__[64];

/**
 * Dieses Makro sendet einen Text über UART0:
 */
#define print(__STRING__) {__tmp_txt__ = __STRING__;\
Serial_WriteText(0,__tmp_txt__);}

/**
 * Dieses Makro sendet eine Zeile Text über UART0 (text + CR+LF).
 */
#define println(__STRING__) {__tmp_txt__ = __STRING__;\
Serial_WriteText(0,__tmp_txt__);Serial_Write(0,CR);Serial_Write(0,LF);}

/**
 * Sendet einen Integer-Wert über UART0.
 */
#define printInteger(__INT__) {Str_WriteInt((__INT__),__tmp_txt__,0);\
Serial_WriteText(0,__tmp_txt__);}

/**
 * Sendet CR+LF über UART0.
 */
#define newline() {Serial_Write(0,CR);Serial_Write(0,LF);}


/******************************************************************************/
//  Externes Schieberegister für LCD und LEDs des Erweiterungsmoduls:
byte external_port;
byte lcd_rs;

#define LCD_RS_ON 8;
#define LCD_RS_OFF 0;

/**
 * Hilfsfunktion für einfache kurze Verzögerungen.
 */
void delayCycles(unsigned int dly)
{
    while(dly--);
}

/**
 * Sendet den Inhalt der "external_port" Variable an das 8-Bit
 * Schieberegister auf dem Erweiterungsmodul. Damit sind die drei roten
 * LEDs sowie die Datenleitungen und das RS Signal des LC-Displays verbunden.
 */
void outputExt(void)
{
    Thread_Lock(1);
    SPI_Write(external_port);
    Port_WriteBit(PORT_STR, 1);
    delayCycles(5);
    Port_WriteBit(PORT_STR, 0);
    Thread_Lock(0);
}

/******************************************************************************/
// LEDs:

/**
 * Diese Funktion setzt die LEDs auf dem Erweiterungsmodul.
 * LED1 und LED2 können auch direkt mit normalen Portbefehlen gesetzt werden,
 * aber LED3, 4 und 5 sind am externen Schieberegister angeschlossen.
 * Daher muss deren Wert zunächst über den SPI Bus übertragen werden.
 */
void setLEDs(byte leds)
{
    external_port = (external_port & 0xF8) | ((leds >> 2) & 0x07);
	outputExt();
	Port_WriteBit(PORT_LED1, leds & 0x01);
	delayCycles(5);
    Port_WriteBit(PORT_LED2, leds & 0x02);
}

/******************************************************************************/
// LCD:

// ----------------------------------------------------------------------------
// Makros zur Ausgabe auf dem LCD:

char __tmp_txt_lcd__[32];

/**
 * Sendet einen Text an das LCD.
 */
#define printLCD(__STRING__) {__tmp_txt_lcd__ = __STRING__; writeStringLCD(__tmp_txt_lcd__);}

/**
 * Beschreibt eine bestimmte Zeile des LCDs.
 */
#define printlnLCD(__STRING__, __LINE__, __POS__) {__tmp_txt_lcd__ = __STRING__;\
writeLineLCD(__tmp_txt_lcd__, __LINE__, __POS__);}

/**
 * Konvertiert eine Zahl in ASCII Zeichen und schreibt diese auf das LCD.
 */
#define printIntegerLCD(__INT__) {Str_WriteInt((__INT__),__tmp_txt_lcd__,0);writeStringLCD(__tmp_txt_lcd__);}

/**
 * Schreibt zwei komplette Zeilen Text auf das LCD.
 */
#define showScreenLCD(__STRING1__, __STRING2__) {clearLCD(); printLCD(__STRING1__); setCursorPosLCD(1,0); printLCD(__STRING2__); }


/**
 * Sendet ein 4 Bit Nibble an das LCD.
 */
void setLCDD(byte lcdd)
{
    Thread_Lock(1);
	external_port = ((external_port & 0x07) | ((lcdd<<4) & 0xF0)) | lcd_rs;
	outputExt();
	delayCycles(6);
	Port_WriteBit(PORT_LCD_EN, 1);
	delayCycles(6);
	Port_WriteBit(PORT_LCD_EN, 0);
	delayCycles(6);
	Thread_Lock(0);
}

/**
 * Sendet ein Kommando an das LCD.
 */
void writeLCDCommand(byte cmd)
{
    Thread_Lock(1);
    lcd_rs = LCD_RS_OFF;
	setLCDD(cmd >> 4);
	setLCDD(cmd);
	delayCycles(1);
	Thread_Lock(0);
}

/**
 * Sendet ein Zeichen an das LCD.
 */
void writeCharLCD(char ch)
{
    Thread_Lock(1);
	lcd_rs = LCD_RS_ON;
	setLCDD(ch >> 4);
	setLCDD(ch);
	delayCycles(1);
	Thread_Lock(0);
}

/**
 * Initialisiert das LCD - muss immer aufgerufen werden bevor
 * man das LCD verwenden kann!
 * (wird schon in RP6_CCPRO_Init gemacht)
 */
void RP6_initLCD(void)
{
    Thread_Lock(1);
    delayCycles(2500);
	setLCDD(0x03);
	delayCycles(2500);
	setLCDD(0x03);
	delayCycles(2500);
	setLCDD(0x03);
	delayCycles(2500);
	setLCDD(0x02);
	delayCycles(2500);
	writeLCDCommand(0x0028);
	delayCycles(2500);
	writeLCDCommand(0x0008);
	delayCycles(2500);
	writeLCDCommand(0x0001);
	delayCycles(2500);
	writeLCDCommand(0x0002);
	delayCycles(2500);
	writeLCDCommand(0x000C);
	delayCycles(2500);
	Thread_Lock(0);
}

/**
 * Sendet einen Text-String an das LCD.
 */
void writeStringLCD(char text[])
{
    Thread_Lock(1);
    word i, size;
    size = Str_Len(text);
    for(i = 0; i < size; i++)
        writeCharLCD(text[i]);
    Thread_Lock(0);
}

/**
 * Beschreibt eine bestimmte Zeile des LCDs mit einem Text.
 */
void writeLineLCD(char text[], byte text_line, byte pos)
{
    Thread_Lock(1);
	setCursorPosLCD(text_line, pos);
	word i, size;
    size = Str_Len(text);
    for(i = 0; i < size; i++)
        writeCharLCD(text[i]);
	Thread_Lock(0);
}

/**
 * Setzt die Cursor Position auf dem LCD
 */
void setCursorPosLCD(byte text_line, byte pos)
{
	pos = pos | 128;
	if(text_line==1) pos = pos + 0x40;
	writeLCDCommand(pos);
}


/**
 * Löscht den gesamten Inhalt des LCDs.
 */
void clearLCD(void)
{
    Thread_Lock(1);
	writeLCDCommand(0x01);
	delayCycles(25);
	Thread_Lock(0);
}

/**
 * Löscht eine bestimmte Anzahl von Zeichen beginnend an einer bestimmten Stelle.
 */
void clearPosLCD(byte text_line, byte pos, byte length)
{
    Thread_Lock(1);
	setCursorPosLCD(text_line,pos);
	while(length--)
		writeCharLCD(' ');
	Thread_Lock(0);
}



/******************************************************************************/
// Piezo Tongeber:

/**
 * Dieses Makro erzeugt einen Ton mit dem Piezo Tongeber der an PB5
 * angeschlossen ist.  Die Tonhöhe kann über den Parameter PITCH vorgegeben
 * werden (Hinweis: niedrige Werte ergeben hohe Frequenzen und umgekehrt!).
 * z.B. sound(440) => 261 Hz, sound(330) => 349 Hz, sound(262) => 440 Hz
 *
 * Es wird nicht blockiert und keine weitere Pause hinzugefügt.
 * Es wird nur die Frequenz gesetzt.
 */
#define sound(__PITCH__) { Timer_T3FRQ(__PITCH__, PS_64); }

/**
 * Tonerzeugung wird abgeschaltet.
 */
#define sound_off() { Timer_T3Stop(); Timer_Disable(3); }

/**
 * Dieses Makro macht dasselbe wie sound(), aber es wird zusätzlich noch
 * eine kurze Pause hinzugefügt. Dieses Makro kann NICHT verwendet werden,
 * wenn Sie Threads verwenden, da AbsDelay verwendet wird.
 */
#define tone(__PITCH__, __TIME__) {sound(__PITCH__); AbsDelay(__TIME__);}

/**
 * Diese Funktion erzeugt einen Ton mit dem Piezo Tongeber an PB5.
 * Der Parameter pitch legt die Tonhöhe fest. "time" legt die Tondauer
 * und "pause" die Pause nach dem Ton fest.
 * Die Pause wird nach dem Abschalten des Tongebers eingefügt.
 * Nützlich für Melodien.
 *
 * Diese Funktion blockiert den Programmfluss für time+pause Millisekunden.
 * Sie ist NICHT für die Verwendung mit Threads geeignet, da AbsDelay
 * verwendet wird.
 *
 */
void beep(word pitch, word time, word pause)
{
    Timer_T3FRQ(pitch, PS_64);
    AbsDelay(time);
    Timer_T3Stop();
    Timer_Disable(3);
    AbsDelay(pause);
}

/**
 * Identisch zu beep, aber es wird Thread_Delay verwendet und somit kann
 * diese Funktion auch mit Threads verwendet werden.
 * (daher das _t wie _thread)
 * Allerdings verwendet Thread_Delay eine 10ms Zeitbasis.
 * beep(100,100,200) würde einen 100ms langen Ton mit 200ms Pause erzeugen.
 * beep_t(100,100,200) hingegen würde einen 1 Sekunde langen Ton gefolgt von
 * 2 Sekunden Pause erzeugen.
 *
 */
void beep_t(word pitch, word time, word pause)
{
    Timer_T3FRQ(pitch, PS_64);
    Thread_Delay(time);
    Timer_T3Stop();
    Timer_Disable(3);
    Thread_Delay(pause);
}

/******************************************************************************/
// I2C Read Register des I2CSlave Programms:

#define I2C_REG_STATUS1 		 0
#define I2C_REG_STATUS2 		 1
#define I2C_REG_MOTION_STATUS 	 2
#define I2C_REG_POWER_LEFT 		 3
#define I2C_REG_POWER_RIGHT 	 4
#define I2C_REG_SPEED_LEFT 		 5
#define I2C_REG_SPEED_RIGHT 	 6
#define I2C_REG_DES_SPEED_LEFT 	 7
#define I2C_REG_DES_SPEED_RIGHT  8
#define I2C_REG_DIST_LEFT_L 	 9
#define I2C_REG_DIST_LEFT_H 	 10
#define I2C_REG_DIST_RIGHT_L     11
#define I2C_REG_DIST_RIGHT_H 	 12
#define I2C_REG_ADC_LSL_L 		 13
#define I2C_REG_ADC_LSL_H 		 14
#define I2C_REG_ADC_LSR_L 		 15
#define I2C_REG_ADC_LSR_H 		 16
#define I2C_REG_ADC_MOTOR_CURL_L 17
#define I2C_REG_ADC_MOTOR_CURL_H 18
#define I2C_REG_ADC_MOTOR_CURR_L 19
#define I2C_REG_ADC_MOTOR_CURR_H 20
#define I2C_REG_ADC_UBAT_L 		 21
#define I2C_REG_ADC_UBAT_H 		 22
#define I2C_REG_ADC_ADC0_L 		 23
#define I2C_REG_ADC_ADC0_H 		 24
#define I2C_REG_ADC_ADC1_L 		 25
#define I2C_REG_ADC_ADC1_H 		 26
#define I2C_REG_RC5_ADR	 		 27
#define I2C_REG_RC5_DATA	 	 28
#define I2C_REG_LEDS	 		 29

// ----------------------------------------------------------------------------
// Kommandos des RP6 I2CSlave Programms:

#define CMD_POWER_OFF 		0
#define CMD_POWER_ON 		1
#define CMD_CONFIG 			2
#define CMD_SETLEDS 		3
#define CMD_STOP   			4
#define CMD_MOVE_AT_SPEED   5
#define CMD_CHANGE_DIR	    6
#define CMD_MOVE 			7
#define CMD_ROTATE 			8
#define CMD_SET_ACS_POWER	9
#define CMD_SEND_RC5		10
#define CMD_SET_WDT			11
#define CMD_SET_WDT_RQ		12

// ACS Power Level für CMD_SET_ACS_POWER:
#define ACS_PWR_OFF  0
#define ACS_PWR_LOW  1
#define ACS_PWR_MED  2
#define ACS_PWR_HIGH 3


// ----------------------------------------------------------------------------
// I2C Register Schreib-Routinen:

/**
 * Sendet ein einzelnes Kommando Byte über den I2C Bus an den Controller
 * auf dem Mainboard (es wird in das Kommandoregister 0 geschrieben!).
 * Es werden keine weiteren Parameter übergeben.
 */
void RP6_writeCMD(byte adr, byte cmd)
{
   Thread_Lock(1);
   I2C_Start();
   I2C_Write(adr);
   I2C_Write(0);
   I2C_Write(cmd);
   I2C_Stop();
   Thread_Lock(0);
}

/**
 * Diese Funktion sendet ein Kommando Byte mit einem Parameter
 * Byte über den I2C Bus.
 */
void RP6_writeCMD_1param(byte adr, byte cmd, byte param)
{
   Thread_Lock(1);
   I2C_Start();
   I2C_Write(adr);
   I2C_Write(0);
   I2C_Write(cmd);
   I2C_Write(param);
   I2C_Stop();
   Thread_Lock(0);
}

/**
 * Sendet ein Kommandobyte mit zwei Parametern über den I2C Bus.
 */
void RP6_writeCMD_2params(byte adr, byte cmd, byte param1, byte param2)
{
   Thread_Lock(1);
   I2C_Start();
   I2C_Write(adr);
   I2C_Write(0);
   I2C_Write(cmd);
   I2C_Write(param1);
   I2C_Write(param2);
   I2C_Stop();
   Thread_Lock(0);
}

/**
 * Sendet ein Kommandobyte mit drei Parametern über den I2C Bus.
 */
void RP6_writeCMD_3params(byte adr, byte cmd, byte param1, byte param2, byte param3)
{
   Thread_Lock(1);
   I2C_Start();
   I2C_Write(adr);
   I2C_Write(0);
   I2C_Write(cmd);
   I2C_Write(param1);
   I2C_Write(param2);
   I2C_Write(param3);
   I2C_Stop();
   Thread_Lock(0);
}

/**
 * Sendet ein Kommandobyte mit "param_count" Parametern über den
 * I2C Bus. Diese Funktion ist nützlich für größere Datentransfers
 * mit bis zu 255 Bytes. Die Daten müssen in einem Array übergeben werden!
 */
void RP6_writeCommand_params(byte adr, byte cmd,
                             byte params[], byte param_count)
{
    Thread_Lock(1);
    I2C_Start();
    I2C_Write(adr);
    I2C_Write(0);
    I2C_Write(cmd);
    byte i;
    for(i=0;i<param_count;i++)
        I2C_Write(params[i]);
    I2C_Stop();
    Thread_Lock(0);
}

// ----------------------------------------------------------------------------
// I2C Register Lese-Routinen:

/**
 * Liest ein einzelnes Register Byte vom Controller auf dem RP6 Mainboard.
 */
byte RP6_readRegister(byte adr, byte reg)
{
    byte I2C_DATA;
    Thread_Lock(1);
    I2C_Start();
    I2C_Write(adr);
    I2C_Write(reg);
    I2C_Start();
    I2C_Write(adr+1);
    I2C_DATA=(I2C_Read_NACK());
    I2C_Stop();
    Thread_Lock(0);
    return I2C_DATA;
}

/**
 * Liest "reg_count" Register des Controllers auf dem RP6 Mainboard.
 * Der Parameter "readBuffer" ist ein byte Array und muss groß genug
 * für die Daten sein (mindestens reg_count+1).
 */
void RP6_readRegisters(byte adr, byte start_reg, byte readBuffer[], byte reg_count)
{
    Thread_Lock(1);
    I2C_Start();
    I2C_Write(adr);
    I2C_Write(start_reg);
    I2C_Start();
    I2C_Write(adr+1);
    byte i;
    for(i=0;i<reg_count;i++)
        readBuffer[i] = (I2C_Read_ACK());
    readBuffer[i+1] = (I2C_Read_NACK());
    I2C_Stop();
    Thread_Lock(0);
}


/******************************************************************************/
// Temperatur Sensor:

/**
 * Hinweis (falls zusätzliche Temperatursensoren angesteuert werden müssen aber
 * kein TCN75_A_ besorgt werden kann):
 * Der 12 Bit Temperatursensor mit I2C Interface auf dem RP6 CCPRO Modul ist
 * ein Microchip TCN75A. Er ist kompatibel mit National LM75 und Analog ADT75.
 * Der TCN75A unterstüzt allerdings höhere Auflösungen von bis zu 12 Bits
 * während der normale TCN75/LM75 nur 9 Bit unterstützt und keinen single shot
 * Modus hat. Ansonsten ist die Registerstruktur allerdings kompatibel.
 *
 */

#define TCN75_ADR 0x90

// Register:
#define TCN75_TEMP 0
#define TCN75_CONFIG 1
#define TCN75_HYST 2
#define TCN75_LIMIT 3

// Konfigurations Register Bits:
// TCN75A:
// OneShot[1] - RESOLUTION[2] - FAULT[2] - ALERT[1] - COMP/INT[1] - SHUTDOWN[1]
// ----
// TCN75, LM75 oder kompatibel
// (9 Bit Auflösung und kein Single Shot Modus)
// Config register bits:
// RESERVED[3]        -         FAULT[2] - ALERT[1] - COMP/INT[1] - SHUTDOWN[1]

// Konfigurationsregister Bitmasken:
#define TCN75_CONFIG_RUN 0 // default
#define TCN75_CONFIG_SHUTDOWN 1
#define TCN75_CONFIG_COMP 0 // default
#define TCN75_CONFIG_INT 2
#define TCN75_CONFIG_ALERT_LOW 0 // default
#define TCN75_CONFIG_ALERT_HIGH 4
#define TCN75_CONFIG_FAULT_1 0 // default
#define TCN75_CONFIG_FAULT_2 8
#define TCN75_CONFIG_FAULT_4 16
#define TCN75_CONFIG_FAULT_6 24

// Nur für den TCN75A - high resolution und OneShot Modus:
#define TCN75A_CONFIG_RES_9 0 // default
#define TCN75A_CONFIG_RES_10 32 // 0b00100000
#define TCN75A_CONFIG_RES_11 64 // 0b01000000
#define TCN75A_CONFIG_RES_12 96 // 0b01100000
#define TCN75A_CONFIG_ONESHOT_DISABLED 0 // default
#define TCN75A_CONFIG_ONESHOT 128 //0b10000000


/**
 * Sendet das Konfigurationsbyte an einen TCN75.
 */
void TCN75_write_cfg(byte adr, byte config)
{
    I2C_Start();
    I2C_Write(adr);
    I2C_Write(TCN75_CONFIG);
    I2C_Write(config);
    I2C_Stop();
}

/**
 * Dieses Makro ist eigentlich nur ein Aufruf von TCN75_write_cfg mit dem
 * Parameter TCN75_CONFIG_SHUTDOWN um eine Messung zu stoppen bzw. den Sensor
 * in den Standby zu versetzen.
 * Der Code wird so lesbarer.
 */
#define TCN75_shutdown(__ADR__) {TCN75_write_cfg(__ADR__, TCN75_CONFIG_SHUTDOWN);}

/**
 * Dieses Makro ist eigentlich nur ein Aufruf von TCN75_write_cfg mit dem
 * Parameter TCN75_CONFIG_RUN um eine Messung zu starten.
 */
#define TCN75_run(__ADR__,__CONFIG__) {TCN75_write_cfg(__ADR__, __CONFIG__);}

// Die vom Sensor gelesenen Datenbytes werden in diesen beiden Variablen
// gespeichert:
byte temperature_low;
byte temperature_high;
// Achtung: Das muss noch konvertiert werden

/**
 * Mit den beiden Makros können die Temperatursensor Variablen ausgelesen
 * werden (damit ist es offensichtlicher, dass es Library Variablen sind und
 * diese nicht im eigenen Programm definiert wurden)
 */
#define getTemperatureHigh() (temperature_high)
#define getTemperatureLow() (temperature_low)


/**
 * Liest die beiden Datenregister des Temperatursensors aus.
 * Diese werden in den beiden Variablen  temperature_low und high abgelegt.
 * Hinweis: Je nach Konfiguration des Sensors liegen die Daten anders in den
 * beiden Werten - bitte Datenblatt beachten!
 */
void TCN75_read(byte adr)
{
    Thread_Lock(1);
    I2C_Start();
    I2C_Write(adr);
    I2C_Write(TCN75_TEMP);
    I2C_Start();
    I2C_Write(adr+1);
    temperature_low = I2C_Read_ACK();
    temperature_high = I2C_Read_NACK();
    I2C_Stop();
    Thread_Lock(0);
}


/******************************************************************************/
// Initialisierung und Synchronisation mit anderen Controllern:

/**
 * Diese Funktion ist für den Einsatz der CCPRO Unit auf dem RP6 essentiell und
 * sehr sehr wichtig!
 * Sie wird benötigt um alle Controller des RP6 synchronisiert zu starten!
 * Dazu wird zunächst gewartet bis entweder der Starttaster auf dem RP6 CCPRO
 * Modul gedrückt wurde oder aber ein Startsignal über den I2C Bus gesendet
 * wird (Low Pegel auf SDA). Sofern der Starttaster gedrückt worden ist, wird
 * selbst ein Start + Stop Signal auf dem I2C Bus erzeugt um die anderen
 * Controller davon zu informieren.
 * Danach wird mit dem Anwenderprogramm fortgefahren.
 *
 * Diese Funktion muss nicht explizit zu Programmbeginn aufgerufen werden, da
 * dies bereits von RP6_CCPRO_Init erledigt wird.
 */
void RP6_waitForStart(void)
{
	int i;
	Port_DataDirBit(PORT_LED1, PORT_OUT);
	Port_DataDirBit(PORT_LED2, PORT_OUT);
    Port_DataDirBit(PORT_SCL, PORT_IN);
	Port_DataDirBit(PORT_SDA, PORT_IN);
	Port_WriteBit(PORT_SDA, 0);
	Port_DataDirBit(PORT_START, PORT_IN);
    Port_WriteBit(PORT_LED2, 0);
	Port_WriteBit(PORT_LED1, 0);
    i = 0;
    while(Port_ReadBit(PORT_SDA)) {
         if(i == 5000)
    	 	Port_WriteBit(PORT_LED1, 1);
    	 else if(i > 6000) {
		 	Port_WriteBit(PORT_LED1, 0);
		 	i = 0;
		 }
		 if(!Port_ReadBit(PORT_START)) {
		 	while(!Port_ReadBit(PORT_START))
				Port_WriteBit(PORT_LED2, 1);
			I2C_Start();
			I2C_Write(0);
            I2C_Write(0);
			I2C_Stop();
		 	break;
		 }
		 i++;
	}
    Port_WriteBit(PORT_LED2, 0);
	Port_WriteBit(PORT_LED1, 0);
 	AbsDelay(250);
}

// 140 byte Sende + 108 Empfungspuffer + 6 byte interne FIFO Verwaltung
byte buffer[255];

/**
 * Sie müssen diese Funktion auf jeden fall ALS ALLERERSTES IN ALLEN CCPRO
 * PROGRAMMEN FÜR DAS RP6 CCPRO Erweiterungsmodul aufrufen.
 * Es initialisiert nicht nur alle Komponenten, sondern wartet auch auf
 * Startsignale (s. RP6_waitForStart).
 */
void RP6_CCPRO_Init(void)
{
    // Portrichtungen konfigurieren:
    Port_DataDirBit(PORT_SCL, PORT_IN);
	Port_DataDirBit(PORT_SDA, PORT_IN);
	Port_DataDirBit(PORT_START, PORT_IN);
	Port_DataDirBit(PORT_PE5_INT, PORT_IN);
	Port_DataDirBit(PORT_PE6_INT, PORT_IN);
	Port_DataDirBit(PORT_MISO, PORT_IN);
	Port_DataDirBit(PORT_SS, PORT_IN);
	Port_DataDirBit(PORT_SCK, PORT_OUT);
	Port_DataDirBit(PORT_MOSI, PORT_OUT);
	Port_DataDirBit(PORT_LCD_EN, PORT_OUT);
	Port_DataDirBit(PORT_LED1, PORT_OUT);
	Port_DataDirBit(PORT_LED2, PORT_OUT);
	Port_DataDirBit(PORT_STR, PORT_OUT);
	Port_DataDirBit(PORT_SND, PORT_OUT);

	Port_WriteBit(PORT_STR, 0);
	Port_WriteBit(PORT_SCK, 1);
	Port_WriteBit(PORT_MOSI, 1);
    Port_WriteBit(PORT_LCD_EN, 0);
	Port_WriteBit(PORT_LED2, 0);
	Port_WriteBit(PORT_LED1, 0);
    Port_WriteBit(PORT_SND, 0);

    // SPI Modul initialisieren:
    SPI_Enable(0x50);  // (SPE + MSTR Bits setzen)

    // Externes Schieberegister zurücksetzen:
	external_port = 0;
	outputExt();

    // Serielle Schnittstelle initialisieren:
	#ifndef DEBUG
    Serial_Init_IRQ(0,buffer,108,140,SR_8BIT|SR_1STOP|SR_NO_PAR,SR_BD38400);
    #endif

	// I2C Modul initialisieren:
	I2C_Init(I2C_100kHz);

	// LCD Initialisieren:
	RP6_initLCD();

    // Startbildschirm anzeigen:
    showScreenLCD("RP6 CCPRO M128", "Press Start!");

    // Timer3 wird für den Beeper verwendet.
    Timer_T3Stop();
    Port_WriteBit(PORT_SND, 0);

	// Auf Startsignal warten...
    RP6_waitForStart();

    // LCD Inhalt löschen:
    clearLCD();

	initStopwatches();
}

#endif

/******************************************************************************
 * Info
 * ****************************************************************************
 * Changelog:
 * - v. 1.0 (initial release) 16.10.2008 by Dominik S. Herwald
 *
 * ****************************************************************************
 * Bugs, feedback, questions and modifications can be posted on the AREXX Forum
 * on http://www.arexx.com/forum/ !
 * Of course you can also write us an e-mail to: info@arexx.nl
 * AREXX Engineering may publish updates from time to time on AREXX.com!
 * ****************************************************************************
 * - LICENSE -
 * GNU GPL v2 (http://www.gnu.org/licenses/gpl.txt, a local copy can be found
 * on the RP6 CD in the RP6 sorce code folders!)
 * This program is free software. You can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 * ****************************************************************************
 */

/*****************************************************************************/
// EOF




