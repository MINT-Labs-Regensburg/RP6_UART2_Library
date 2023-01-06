#include "RP6RobotBaseLib.h" 		
#include "RP6uart.h"
#include "SRF02ControlLibrary.h"
#include "RP6I2CmasterTWI.h"
#include "RP6RobotBase_SelfTest.h"
#include "RP6RemotrolCommon.h"

int timeBetweenFrames = 200;
uint8_t sendData = 1;

void task_Commands(void)
{
	if(getBufferLength()) 
	{
		char tmp = readChar(); 
		if (tmp =='#') 
		{
			mSleep(10); 
			counter = 0; 
		}
		text[counter] = tmp; 
		text[counter + 1] = '\0';
		counter++;
	}
	int cmd = getCommand(); 	
	if(cmd)
	{				
		switch(cmd)
		{
			case CMD_SET_SPEED:
				moveAtSpeed(params[1],params[2]);
				changeDirection(params[3]); 
			break;
			case CMD_SET_LEDS:
				setLEDs(params[2]);
			break;
			case CMD_SET_FEATURE:
				switch(params[1])
				{
					case SET_FEATURE_RP6:
						HasRP6 = params[2];
					break;
					case SET_FEATURE_SRF02:
						HasSRF02 = params[2];
					break;
					case SET_FEATURE_SRF02_1:
						HasSRF02_1 = params[2];
					break;
					case SET_FEATURE_SRF02_2:
						HasSRF02_2 = params[2];
					break;
					case SET_FEATURE_SRF08:
						HasSRF08 = params[2];
					break;
				}		
				break;
			case CMD_SET_STOP: 
				stop();
			break;				
			case CMD_SET_CONNECTION_SPEED:
				if(params[1] < 0)
				{
					sendData = 0;
					timeBetweenFrames = 1000;
				}
				else
				{
					sendData = 1;
					timeBetweenFrames = params[1];
				}
            break;
			case CMD_SET_ACSPOWER:
				switch(params[1])
				{
					case ACS_POWER_OFF: 
						setACSPwrOff();
					break;
					case ACS_POWER_LOW:
						setACSPwrLow();
					break;
					case ACS_POWER_MED:
						setACSPwrMed();
					break;
					case ACS_POWER_HIGH:
						setACSPwrHigh();
					break;
				}
			break;
			case CMD_SET_TEST:
				switch(params[1])
				{
					case TEST_LED:
						TestLEDs();
					break;
					case TEST_MOTOR:
						TestMotor();
					break;
					case TEST_BATTERY:
						TestBattery();
					break;
					case TEST_ACS:
						TestACS();
					break;
					case TEST_BUMPER:
						TestBumper();
					break;
					case TEST_LIGHTSENSOR:
						TestLightSensor();
					break;
				}
			break;
		}
		cmd= 0;
	}
}

void printSensors(void)
{
	if(sendData)
    {
		StartDataFrame();
		if(HasRP6)
		{			
			writeString_P("Bat:");
			writeInteger(adcBat, DEC);
			writeString_P("\n");
			
			writeString_P("SpeedL:");
			writeInteger(getLeftSpeed(), DEC);
			writeString_P("\n");
			
			writeString_P("SpeedR:");
			writeInteger(getRightSpeed(), DEC);
			writeString_P("\n");
			
			writeString_P("PowerL:");
			writeInteger(adcMotorCurrentLeft, DEC);
			writeString_P("\n");
			
			writeString_P("PowerR:");
			writeInteger(adcMotorCurrentRight, DEC);
			writeString_P("\n");

			writeString_P("LightL:");
			writeInteger(adcLSL, DEC);
			writeString_P("\n");
			
			writeString_P("LightR:");
			writeInteger(adcLSR, DEC);
			writeString_P("\n");			

			writeString_P("BumpL:");
			writeInteger(bumper_left, DEC);
			writeString_P("\n");
			
			writeString_P("BumpR:");
			writeInteger(bumper_right, DEC);
			writeString_P("\n");
			
			writeString_P("ObsL:");
			writeInteger(obstacle_left, DEC);
			writeString_P("\n");
			
			writeString_P("ObsR:");
			writeInteger(obstacle_right, DEC);
			writeString_P("\n");
			
			writeString_P("ADC0:");
			writeInteger(adc0, DEC);
			writeString_P("\n");
			
			writeString_P("ADC1:");
			writeInteger(adc1, DEC);
			writeString_P("\n");
			
			writeString_P("BumpHL:");
			writeInteger(bumper_back_left, DEC);
			writeString_P("\n");
			
			writeString_P("BumpHR:");
			writeInteger(bumper_back_right, DEC);
			writeString_P("\n");
			
		}
		
		if(HasSRF02)
		{
			writeString_P("Distance:");
			writeInteger(SRF02_Dist, DEC);
			writeString_P("\n");
		}
		EndDataFrame();
	}	
}

void SendDebugData(void)
{
	if (getStopwatch1() > timeBetweenFrames)
	{
		printSensors();
		setStopwatch1(0);
	}
}

void task_PCConnection(void)
{
	task_Commands();
	if(HasSRF02)
		task_SRF02();
	SendDebugData();

	if(getStopwatch2() >= 500)
	{
		SerialHeartBeat();
		setStopwatch2(0);
	}	
}

void InitializeFeatures(void)
{
	HasRP6 = 1;
    HasM32 = 0;
    HasM128 = 0;
    HasServoM32 = 0;
    HasServoM128 = 0;
    HasLCDM32 = 0;
    HasLCDM128 = 0;
    HasSRF02 = 0;
    HasSRF02_1 = 0;
    HasSRF02_2 = 0;
    HasSRF08 = 0;
}

void initializeConnection(void)
{	
	I2CTWI_initMaster(100);
	InitializeFeatures();
	//initSRF02();
	setStopwatch1(0);
	setStopwatch2(0);
	startStopwatch1();
	startStopwatch2();
}