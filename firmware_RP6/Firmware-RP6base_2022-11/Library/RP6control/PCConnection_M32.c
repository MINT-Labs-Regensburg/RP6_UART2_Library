#include "RP6ControlLib.h" 		
#include "RP6Control_I2CMasterLib.h"
#include "RP6uart.h"
#include "RP6ControlServoLib.h"
#include "SRF02ControlLibrary.h"
#include "RP6Control_SelfTest.h"
#include "RP6RemotrolCommon.h"

//Verwendet StopWatch 1 und 2

int timeBetweenFrames = 200;
uint8_t sendData = 1;

int MelodyTimes[100];
int MelodyPitches[100];

void PlayMelody(int all)
{
    int current;
    current = 0;
    setStopwatch7(0);
    startStopwatch7();
    while(current <= all)
    {
        int currTime;
        currTime = MelodyTimes[current];
        int currPitch;
        currPitch = MelodyPitches[current];

        setBeeperPitch(currPitch);
        setStopwatch6(0);
        startStopwatch6();

        while(getStopwatch6()<currTime)
        {
            if(getStopwatch7() > 500)
            {
                setStopwatch7(0);
                startStopwatch7();
                SerialHeartBeat();
            }
        }
        current++;
    }
    setBeeperPitch(0);
    stopStopwatch6();
    stopStopwatch7();
}

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
		clearPosLCD(1,0,15);
		setCursorPosLCD(1,0);
		writeStringLCD(text);
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
			case CMD_SET_SERVO:
				servo_position[params[2]] = params[3];
				break;
			case CMD_SET_LEDS:
					switch(params[1])
					{
						case LEDS_RP6:
							setRP6LEDs(params[2]);
						break;
						case LEDS_M32:
							setLEDs(params[2]);
						break;
					}
				break;
			case CMD_SET_BEEP: 
				beep(params[2],params[3]);
			break;
			case CMD_SET_START_MELODY:
			{
				uint8_t ToneCount = params[1];
				uint8_t currCount = params[2];
				uint8_t currPitch = params[3];
				uint16_t currTime = params[4];
				SPI_EEPROM_writeByte(3,ToneCount);
				while(SPI_EEPROM_getStatus() & SPI_EEPROM_STAT_WIP);
				uint8_t buffer[3];
				buffer[0] = currPitch;
				buffer[1] = currTime & 0x00ff;
				buffer[2] = (currTime & 0xff00) >>8;
				SPI_EEPROM_writeBytes(4+((currCount-1)*3),buffer,3);
				while(SPI_EEPROM_getStatus() & SPI_EEPROM_STAT_WIP);
				if(ToneCount == currCount)
				{
					SPI_EEPROM_writeByte(0,8);	//Die Checkbytes schreiben
					SPI_EEPROM_writeByte(1,12);
					SPI_EEPROM_writeByte(2,91);
				}
			}
			break;
			case CMD_SET_FEATURE:
				switch(params[1])
				{
					case SET_FEATURE_RP6:
						HasRP6 = params[2];
					break;
					case SET_FEATURE_M32:
						HasM32 = params[2];
					break;
					case SET_FEATURE_SERVOM32:
						HasServoM32 = params[2];
					break;
					case SET_FEATURE_LCDM32:
						HasLCDM32 = params[2];
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
			case CMD_SET_STOP: stop();
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
			case CMD_SET_MELODY:
			{
				int current;
				int all;
				int currTime;
				int currPitch;
				current = params[1];
				all = params[2];
				currTime = params[3];
				currPitch = params[4];
				if((current >= 0 && current < 100) || current == -99)
				{
					if(current == -99)
					{
						PlayMelody(all);
						return;
					}
					MelodyTimes[current] = currTime;
					MelodyPitches[current] = currPitch;

				}
				else
				{
					StartDebugFrame();
					writeString_P("Ton-Counter ist falsch!Counter: ");
					writeInteger(current,DEC);
					EndDebugFrame();
				}
			}
			break;
			case CMD_SET_ACSPOWER:
				I2CTWI_transmit3Bytes(I2C_RP6_BASE_ADR, 0, CMD_SET_ACS_POWER, params[1]);
			break;
			case CMD_SET_TEST:
				switch(params[1])
				{
					case TEST_LCD:
						TestLCD();
					break;
					case TEST_BEEPER:
						TestBeeper();
					break;
					case TEST_LED:
						TestLEDs();
					break;
					case TEST_EXTERNAL_MEMORY:
						TestExternalMemory();
					break;
					case TEST_I2CLED:
						TestI2CLEDs();
					break;
					case TEST_I2CMOTOR:
						TestI2CMotor();
					break;
					case TEST_MIC:
						TestMic();
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
			writeInteger(mleft_speed, DEC);
			writeString_P("\n");
			
			writeString_P("SpeedR:");
			writeInteger(mright_speed, DEC);
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
			
			writeString_P("ADC0_Base:");
			writeInteger(adc0, DEC);
			writeString_P("\n");
			
			writeString_P("ADC1_Base:");
			writeInteger(adc1, DEC);
			writeString_P("\n");
			
			writeString_P("ADC2_M32:");
			writeInteger(readADC(2), DEC);
			writeString_P("\n");
			
			writeString_P("ADC3_M32:");
			writeInteger(ADC_3, DEC);
			writeString_P("\n");
			
			writeString_P("ADC4_M32:");
			writeInteger(ADC_4, DEC);
			writeString_P("\n");
			
			writeString_P("ADC5_M32:");
			writeInteger(ADC_5, DEC);
			writeString_P("\n");
			
			writeString_P("ADC6_M32:");
			writeInteger(ADC_6, DEC);
			writeString_P("\n");
			
			writeString_P("ADC7_M32:");
			writeInteger(ADC_7, DEC);
			writeString_P("\n");
			
		}
		
		if(HasSRF02)
		{
			writeString_P("Distance:");
			writeInteger(SRF02_Dist, DEC);
			writeString_P("\n");
		}

		if(HasM32)
		{
			writeString_P("Mic:");
			writeInteger(getMicrophonePeak(),DEC);
			writeString_P("\n");
		}	
		EndDataFrame();
	}	
}

void SendDebugData(void)
{
	if (getStopwatch1() >=timeBetweenFrames)
	{
		getAllSensors();
		getLightSensors();
		printSensors();				
		setStopwatch1(0);
	}	
}

void LCDHeartbeat(void)
{
	if(HasLCDM32)
	{
		static uint8_t heartbeat = false;
		if(heartbeat)
		{
			clearPosLCD(0, 15, 1);
			heartbeat = false;
		}
		else
		{
			setCursorPosLCD(0, 15);
			writeStringLCD_P("*"); 
			heartbeat = true;
			setCursorPosLCD(0,0);
			writeStringLCD_P("RP6 Remotrol");
		}
	setStopwatch3(0);
	}
}

void task_PCConnection(void)
{
	if(HasRP6)
		task_checkINT0();
	task_I2CTWI();
	
	task_Commands();
	if(HasSRF02)
		task_SRF02();
	if(HasServoM32)
		task_SERVO();
	SendDebugData();
	if(getStopwatch2() >= 500)
	{
		SerialHeartBeat();
		LCDHeartbeat();
		setStopwatch2(0);
	}	
}

void I2C_requestedDataReady(uint8_t dataRequestID)
{
	checkRP6Status(dataRequestID);
}

/**
 * Timed Watchdog display only - the other heartbeat function
 * does not work in this example as we use blocked moving functions here.
 */
void watchDogRequest(void)
{
	if(HasRP6 && HasLCDM32)
	{
		static uint8_t heartbeat2 = false;
		if(heartbeat2)
		{
			clearPosLCD(0, 14, 1);
			heartbeat2 = false;
		}
		else
		{
			setCursorPosLCD(0, 14);
			writeStringLCD_P("#"); 
			heartbeat2 = true;
		}
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
	I2CTWI_setRequestedDataReadyHandler(I2C_requestedDataReady);
	WDT_setRequestHandler(watchDogRequest); 
	// Enable Watchdog for Interrupt requests:
	I2CTWI_transmit3Bytes(I2C_RP6_BASE_ADR, 0, CMD_SET_WDT, true);
	// Enable timed watchdog requests:
	I2CTWI_transmit3Bytes(I2C_RP6_BASE_ADR, 0, CMD_SET_WDT_RQ, true);
	
	InitializeFeatures();
	
	initSERVO(SERVO1 | SERVO2 | SERVO3 | SERVO4 | SERVO5 | SERVO6 | SERVO7 | SERVO8); 
	dischargePeakDetector();
	//initSRF02();
	clearLCD();
	setCursorPosLCD(0,0);
	writeStringLCD_P("RP6 Remotrol");
	setStopwatch1(0);
	setStopwatch2(0);
	setStopwatch3(0);
	startStopwatch1();
	startStopwatch2();
	startStopwatch3();
}