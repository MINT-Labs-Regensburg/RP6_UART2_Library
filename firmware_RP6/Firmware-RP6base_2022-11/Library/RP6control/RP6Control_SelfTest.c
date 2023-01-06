#include "RP6uart.h"
#include "RP6ControlLib.h" 	
#include "RP6Control_I2CMasterLib.h"
#include "PCConnection_M32.h"

char lastTest[30];

void TestEnd(int success)
{
    writeString_P("*##");
    writeString(lastTest);
    writeString_P("|");
    writeInteger(success, DEC);
    writeString_P("##*");
    writeString_P("\r\n");
    clearLCD();
}

void TestStart(char name[])
{
    int i;
	for(i = 0; i< strlen(lastTest);i++)
	{
		if(i < strlen(name))
		{
			lastTest[i] = name[i];
		}
		else
		{
			lastTest[i] = '\0';
		}		
	}
    writeString_P("#**");
    writeString(name);
    writeString_P("**#");
    writeString_P("\r\n");
	clearLCD();
	setCursorPosLCD(0,0);
    writeStringLCD("      Test");
	setCursorPosLCD(1,0);
    writeStringLCD(name);
}

void TestLCD(void)
{
	TestStart("LCD Test");
	
	uint8_t keynumber = 0;
	uint8_t cnt = 0;
	SerialHeartBeat();
	while(keynumber < 6)
	{		
		uint8_t key = checkReleasedKeyEvent(); 
		if(key == keynumber)
		{
			keynumber++;
			showScreenLCD("PRESS BUTTON", "NUMBER ");
			writeIntegerLCD(keynumber,DEC);
			writeString_P("### PRESS BUTTON NUMBER ");
			writeInteger(keynumber,DEC);
			writeString_P("!\n");
		}
		mSleep(10);
		cnt++;
		if(cnt % 50 == 0)
		{
			SerialHeartBeat();
		}
	}
	
	TestEnd(1);
}

void TestBeeper(void)
{
	TestStart("Beeper Test");
	
	sound(50,50,100); 
	sound(80,50,100);
	sound(100,50,100);
	sound(120,50,100);
	SerialHeartBeat();
	sound(140,50,100);
	sound(160,50,100);
	sound(180,50,100);
	sound(200,50,100);
	
	TestEnd(1);
}

void TestLEDs(void)
{
	TestStart("LED Test");
	
	setLEDs(0b0000);
	mSleep(100);
	setLEDs(0b0001);
	mSleep(100);
	setLEDs(0b0010);
	mSleep(100);
	setLEDs(0b0100);
	mSleep(100);
	setLEDs(0b1000);
	mSleep(100);
	setLEDs(0b1001);
	mSleep(100);
	setLEDs(0b1011);
	mSleep(100);
	setLEDs(0b1111);
	SerialHeartBeat();
	mSleep(400);
	setLEDs(0b0000);
	mSleep(400);
	setLEDs(0b1111);
	SerialHeartBeat();
	mSleep(400);
	setLEDs(0b0000);
	
	TestEnd(1);
}

void TestExternalMemory(void)
{
	TestStart("External Memory Test");
	uint8_t testResult = 1;
	
	writeString_P("\nLoesche 250 Bytes...\n");	
	uint8_t cnt;
	for(cnt = 0; cnt < 250; cnt++)
	{
		SPI_EEPROM_writeByte(cnt, 0);
		while(SPI_EEPROM_getStatus() & SPI_EEPROM_STAT_WIP);
	}
	SerialHeartBeat();

	writeString_P("...Done!\nSchreibe 250 Bytes auf EEPROM:\n");
	for(cnt = 0; cnt < 250; cnt++)
	{
		SPI_EEPROM_writeByte(cnt, cnt);
		while(SPI_EEPROM_getStatus() & SPI_EEPROM_STAT_WIP);
	}
	
	mSleep(400);
	setLEDs(0b1111);
	SerialHeartBeat();
	writeString_P("\nLesen und Vergleichen:\n");
	
	for(cnt = 0; cnt < 250; cnt++)
	{
		uint8_t result = SPI_EEPROM_readByte(cnt);
		if(result != cnt)
		{
			writeString_P("\nEEPROM FEHLER!!!! EEPROM BESCHAEDIGT!!!\n");
			writeString_P("Gelesen: "); writeInteger(result,DEC);
			writeString_P(", Soll: "); writeInteger(cnt,DEC); writeChar('\n');
			testResult = 0;
		}
	}
	SerialHeartBeat();
	writeString_P("\nLoesche 250 Bytes...\n");
	for(cnt = 0; cnt < 250; cnt++)
	{
		SPI_EEPROM_writeByte(cnt, 0x0);
		while(SPI_EEPROM_getStatus() & SPI_EEPROM_STAT_WIP);
	}	
	mSleep(400);
	setLEDs(0b0000);
	
	TestEnd(testResult);
}

void TestI2CLEDs(void)
{
	TestStart("I2CLED Test");
	uint8_t testResult = 1;
	
	uint8_t runningLight = 1;
	uint8_t cnt;
	for(cnt = 0; cnt < 24; cnt++)
	{
		writeIntegerLength(cnt,DEC,3);
		writeChar(':');
		writeIntegerLength(runningLight,DEC,3);
		writeChar(',');
		writeChar(' ');

		setRP6LEDs(runningLight);
		mSleep(50);
		I2CTWI_transmitByte(I2C_RP6_BASE_ADR, 29);
		uint8_t result = I2CTWI_readByte(I2C_RP6_BASE_ADR);
		if(result != runningLight) 
		{
			writeString_P("\nTWI TEST FEHLER!\n");
			testResult = 0;
		}
		runningLight <<= 1; 
		if(runningLight > 32) 
			runningLight = 1;
	
		if((cnt+1) % 6 == 0) writeChar('\n');
		
		if(cnt % 4 == 0)
		{
			SerialHeartBeat();
		}
		
		mSleep(100);
	}
	setRP6LEDs(0);
	
	TestEnd(testResult);
}

void TestI2CMotor(void)
{
	TestStart("I2CMotor Test");	
	
	uint8_t result = 1;
	
	uint8_t i;
	uint8_t j;
	for(j = 0; j < 2;j++)
	{
		changeDirection(j);
		for(i = 5; i <= 14-j; i++)
		{
			SerialHeartBeat();
			uint8_t speed = i*10;
			moveAtSpeed(speed,speed);	
			mSleep(750);
			SerialHeartBeat();
			mSleep(750);
			getAllSensors();
			
			writeString_P("Sollgeschwindigkeit:       ");
			writeInteger(speed, DEC);
			writeString_P("\n");
			
			writeString_P("Geschwindigkeit links:    ");
			writeInteger(mleft_speed, DEC);
			if(mleft_speed > speed - 15 && mleft_speed < speed + 15)
			{
				writeString_P(" --> OK");
			}
			else
			{
				writeString_P(" --> FEHLER");
				result = 0;
			}			
			writeString_P("\n");
			
			writeString_P("Geschwindigkeit rechts: ");
			writeInteger(mright_speed, DEC);
			if(mright_speed > speed - 15 && mright_speed < speed + 15)
			{
				writeString_P(" --> OK");
			}
			else
			{
				writeString_P(" --> FEHLER");
				result = 0;
			}			
			writeString_P("\n");			
		}
		moveAtSpeed(0,0);
		SerialHeartBeat();
		mSleep(500);
	}
	moveAtSpeed(0,0);	
	
	if(!result)
	{
		writeString_P("Während des Tests sind Fehler aufgetreten!!!");
	}
	
	TestEnd(result);
}

void TestMic(void)
{
	TestStart("Mic Test");
	
	uint8_t result = 0;
	
	dischargePeakDetector();
	uint8_t i;
	for(i = 0; i < 100; i++)
	{
		uint16_t tmp = getMicrophonePeak();
		if(tmp > 10)
		{			
			writeInteger(tmp, DEC);
			writeString_P("| ");
			result++;
		}	
		
		if(i % 10 == 0)
		{
			SerialHeartBeat();
		}		
		mSleep(100);
	}
	
	if(result < 5)
	{
		result = 0;
		writeString_P("Es konnten keine Geraeusche erkannt werden!\nMikrofon ist eventuell kaputt!");
	}
	
	TestEnd(result);
}