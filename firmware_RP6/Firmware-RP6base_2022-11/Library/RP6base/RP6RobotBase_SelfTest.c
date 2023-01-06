#include "RP6uart.h"
#include "RP6RobotBaseLib.h" 	
#include "PCConnection_Base.h"
#include "RP6RemotrolCommon.h"

char lastTest[30];

void TestEnd(int success)
{
    writeString_P("*##");
    writeString(lastTest);
    writeString_P("|");
    writeInteger(success, DEC);
    writeString_P("##*");
    writeString_P("\r\n");
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
}

void printUBat(uint16_t uBat)
{
	writeIntegerLength((((uBat/102.4f)+0.1f)), DEC, 2);
	writeChar('.');
	writeIntegerLength((((uBat/1.024f)+10)), DEC, 2);
	writeChar('V');
}

void TestLEDs(void)
{
	TestStart("LED Test");
	
	setLEDs(0b111111);
	mSleep(500);
	setLEDs(0b000000);
	mSleep(500);
	SerialHeartBeat();
	setLEDs(0b111111);
	mSleep(500);
	setLEDs(0b000000);
	mSleep(250);
	SerialHeartBeat();
	uint8_t testLEDs = 1;
	uint8_t i;
	for(i = 0; i < 12; i++){
		setLEDs(testLEDs); 
		testLEDs <<= 1; 
		if(testLEDs > 63)
			testLEDs = 1; 

		if(i % 2 == 0)
		{
			SerialHeartBeat();
		}
			
		mSleep(400);
	}
	setLEDs(0b111111);
	mSleep(400);
	setLEDs(0b000000);
	SerialHeartBeat();
	
	TestEnd(1);
}

void TestMotor(void)
{
	TestStart("Motor Test");

	task_RP6System();

	powerON();
	setMotorDir(FWD,FWD);
	setMotorPower(0,0);
	moveAtSpeed(0,0);
	setStopwatch1(0);
	setStopwatch2(0);
	startStopwatch1();
	startStopwatch2();
	uint8_t state = 0;
	uint8_t cnt = 0;
	uint8_t speed_test = 0;
	uint8_t errors = 0;
	
	uint8_t counter = 0;
	
	task_RP6System();
	while(true)
	{
		if(getStopwatch2() > 200)
		{	
			if(counter %2 == 0)
			{
				SerialHeartBeat();
			}
			counter++;
			if (counter > 4)
				counter  = 0;
			writeString_P("T: ");
			writeIntegerLength(speed_test,DEC,3);
			writeString_P(" |VL: ");
			writeIntegerLength(getLeftSpeed(),DEC,3);
			writeString_P(" |VR: ");
			writeIntegerLength(getRightSpeed(),DEC,3);
			writeString_P(" |PL: ");
			writeIntegerLength(getMotorLeft(),DEC,3);
			writeString_P(" |PR: ");
			writeIntegerLength(getMotorRight(),DEC,3);
			writeString_P(" |IL: ");
			writeIntegerLength(adcMotorCurrentLeft,DEC,3);
			writeString_P(" |IR: ");
			writeIntegerLength(adcMotorCurrentRight,DEC,3);
			writeString_P(" |UB: ");
			printUBat(adcBat);
			writeChar('\n');
			if((getLeftSpeed() < 130 && adcMotorCurrentLeft > 120)
			   || (getLeftSpeed() >= 130 && adcMotorCurrentLeft > 150))
			{
				writeString_P("####### Links: STROMAUFNAHME IST ZU HOCH! ########\n");
				errors++;
			}
			if((getRightSpeed() < 130 && adcMotorCurrentRight > 120)
			   || (getRightSpeed() >= 130 && adcMotorCurrentRight > 150))
			{
				writeString_P("####### Rechts: STROMAUFNAHME IST ZU HOCH! ########\n");
				errors++;
			}
			
			if(getLeftSpeed() > 25 && getMotorLeft() >= 30 && adcMotorCurrentLeft < 7)
			{
				writeString_P("####### Links: Stromsensor ist defekt!!! Gemessene Stromaufnahme ist zu gering! ########\n");
				errors++;
			}
			if(getRightSpeed() > 25 && getMotorRight() >= 30 && adcMotorCurrentRight < 7)
			{
					writeString_P("####### Rechts: Stromsensor ist defekt!!! Gemessene Stromaufnahme ist zu gering!! ########\n");
				errors++;
			}
			if(adcBat < 550)
			{
				writeString_P("####### FEHLER: Versorgungspannung ist zu gering (<5.50V)! Bitte Spannungsversorgung überprüfen! #######\n");
				errors++;
			}
			if(errors)
				break;

			setStopwatch2(0);
		}
		if(getStopwatch1() > 1000)
		{
			moveAtSpeed(speed_test,speed_test);
			if(cnt > 2)
			{
				if((getLeftSpeed() >= ((speed_test > 5) ? (speed_test - 10) : speed_test)) 
					&& (getLeftSpeed() <= (speed_test+10)))
				{
					writeString_P("Geschwindigkeit Links: OK\n");
				}
				else
				{
					writeString_P("####### FEHLER Links ########\n");
					errors++;
				}

				if((getRightSpeed() >= ((speed_test > 5) ? (speed_test - 10) : speed_test)) 
					&& (getRightSpeed() <= (speed_test+10)))
				{
					writeString_P("Geschwindigkeit Rechts: OK\n");
				}
				else
				{
					writeString_P("####### FEHLER Rechts #######\n");
					errors++;
				}

				if(errors) break;

				if(state == 1)
				{
					if(speed_test <= 120)
					{
						if(speed_test >= 80)
							changeDirection(BWD);
						else if(speed_test >= 60)
							changeDirection(FWD);
						else if(speed_test >= 40)
							changeDirection(BWD);
						else if(speed_test < 40)
							state = 2;
					}
					speed_test -= 20;
				}

				if(state == 0)
				{
					if(speed_test >= 80)
						state = 1;
					speed_test += 20;
				}
				cnt = 0;
			}
			else
				cnt++;
			setStopwatch1(0);
		}
		
		task_RP6System();

		if(state==2)
		{
			moveAtSpeed(0,0);
			setMotorPower(0,0);
			break;
		}
		task_RP6System();
	}

	stopStopwatch1();
	setMotorPower(0,0);
	moveAtSpeed(0,0);
	setMotorDir(FWD,FWD);
	if(errors)
	{
		setLEDs(0b110110);
		writeString_P("####### !!!  WARNUNG WARNUNG WARNUNG  !!! #######\n");
		writeString_P("##### !!! ES WURDEN FEHLER FESTGESTELLT !!! #####\n");
		writeString_P("##### Bitte Motor und Encoder ueberpruefen! #####\n");
		powerOFF();
	}
	else
	{						   
		writeString_P("\n***** MOTOREN UND SENSOREN OKAY! *****\n");
		mSleep(1000);
	}
	setLEDs(0b000000);
	
	TestEnd(!errors);
}

void TestBattery(void)
{
	TestStart("Battery Test");
	
	uint8_t result = 1;
	
	writeString_P("Starte 20 Messungen:\n");
	uint16_t ubat;
	uint8_t i;
	for(i = 1; i <= 20; i++)
	{
		writeString_P("Messung #"); 
		writeInteger(i, DEC);
		writeString_P(": ");
		ubat = readADC(ADC_BAT);
		printUBat(ubat);

		if(ubat >= 570 && ubat <= 970)
		{
			writeString_P(" --> OK!\n");
		}
		else 
		{
			writeString_P("\nWARNUNG: SPANNUNG IST ZU ");
			if(ubat < 570) 
			{
				writeString_P("NIEDRIG");
			}
			else if(ubat > 970)
			{
				writeString_P("HOCH");
			}
			writeString_P(" (min. 5.50V bis max. 9.50V)!\n");
			result = 0;
		}
		
		if(i % 5 == 0)
		{		
			SerialHeartBeat();
		}
		
		mSleep(200);
	}
	
	TestEnd(result);
}

void TestACS(void)
{
	TestStart("ACS Test");
		
	setACSPwrHigh();

	uint8_t i;
	for(i = 1; i <= 100; i++)
	{			
		task_RP6System();

		static uint8_t info_left = false;
		static uint8_t info_right = false;
	
		if(isObstacleLeft()) 
		{
			if(!info_left) 
			{			 
				writeString_P("HINDERNIS: LINKS!\n");
				info_left = true;
			}
		}
		else if(info_left) 
		{
			writeString_P("FREI: LINKS!\n");
			info_left = false;
		}
		if(isObstacleRight()) 
		{
			if(!info_right) 
			{
				writeString_P("HINDERNIS: RECHTS!\n");
				info_right = true;
			}
		}
		else if(info_right) 
		{
			writeString_P("FREI: RECHTS!\n");
			info_right = false;
		}

		statusLEDs.byte = 0b000000;
		if(isObstacleLeft() && isObstacleRight())
			statusLEDs.byte = 0b100100;
		statusLEDs.LED5 = isObstacleLeft();
		statusLEDs.LED2 = isObstacleRight();
		statusLEDs.LED4 = (!isObstacleLeft());
		statusLEDs.LED1 = (!isObstacleRight());
		updateStatusLEDs();
		
		if(i % 10 == 0)
		{		
			SerialHeartBeat();
		}
		
		uint8_t j;
		for(j = 0; j <10;j++)
		{
			task_RP6System();
			mSleep(10);
		}
	}
	
	setLEDs(0b000000);	
		
	TestEnd(1);
}

void TestBumper(void)
{
	TestStart("Bumper Test");
	
	uint8_t i;
	for(i = 0; i < 100; i++)
	{		
		static uint8_t info_left = false;
		static uint8_t info_right = false;
		uint8_t bumper_left = getBumperLeft();
		uint8_t bumper_right = getBumperRight();
		
		if(bumper_left) 
		{
			if(!info_left) 
			{			 
				writeString_P("BUMPER: LINKS!\n");
				info_left = true;
			}
		}
		else if(info_left) 
		{
			writeString_P("FREI: LINKS!\n");
			info_left = false;
		}
		if(bumper_right) 
		{
			if(!info_right) 
			{
				writeString_P("BUMPER: RECHTS!\n");
				info_right = true;
			}
		}
		else if(info_right) 
		{
			writeString_P("FREI: RECHTS!\n");
			info_right = false;
		}
	    statusLEDs.LED4 = (!bumper_left);
		statusLEDs.LED1 = (!bumper_right);
		updateStatusLEDs();
		
		if(i % 10 == 0)
		{		
			SerialHeartBeat();
		}
		
		mSleep(100);
	}
	setLEDs(0b000000);
	
	TestEnd(1);
}

void TestLightSensor(void)
{
	TestStart("LightSensor Test");
	
	setStopwatch1(0);
	setStopwatch2(0);
	setStopwatch3(0);
	startStopwatch1();
	startStopwatch2();	
	startStopwatch3();
	writeString_P("Starte Messungen...:\n");
	uint16_t sens_l;
	uint16_t sens_r;

	while(true)
	{		
		if(getStopwatch1() > 50) 
		{		
			sens_l = readADC(ADC_LS_L);
			sens_r = readADC(ADC_LS_R);
			
			int16_t dif = ((int16_t)(sens_l - sens_r));
			if(dif > 160) dif = 160;
			if(dif < -160) dif = -160;
	
			// Set LEDs - a small bargraph display that shows which sensor has detected
			// brighter light - and how much brighter it is:
			if(dif > 140)
				setLEDs(0b111000);
			else if(dif > 75)
				setLEDs(0b011000);
			else if(dif > 35)
				setLEDs(0b001000);
			else if(dif > -35)
				setLEDs(0b001001);
			else if(dif > -75)
				setLEDs(0b000001);
			else if(dif > -140)
				setLEDs(0b000011);
			else 
				setLEDs(0b000111);
			setStopwatch1(0);
		}
		
		if(getStopwatch2() > 250) 
		{	
			sens_l = readADC(ADC_LS_L);
			sens_r = readADC(ADC_LS_R);

			writeString_P("Links: ");
			writeIntegerLength(sens_l, DEC, 4);
			writeString_P(",  Rechts: ");
			writeIntegerLength(sens_r, DEC, 4);
			writeChar('\n');
			SerialHeartBeat();
			setStopwatch2(0);
		}
		
		if(getStopwatch3() > 10000)
		{
			stopStopwatch3();
			setStopwatch3(0);
			break;
		}
	}
	//stopStopwatch1();
	//stopStopwatch2();	
	//stopStopwatch3();
	setLEDs(0);
		
	TestEnd(1);
}