#include "RP6I2CmasterTWI.h"
#include "RP6RobotBaseLib.h" 

#define I2C_SRF02_ADR_1 0xE0
#define I2C_SRF02_ADR_2 0xE2

#define SRF02_STATUS_MEASUREMENT 0
#define SRF02_STATUS_IDLE 1

#define SRF02_1 0
#define SRF02_2 1

#define SRF02_ENABLED 1
#define SRF02_DISABLED 0

uint8_t SRF02_1_Status = SRF02_STATUS_IDLE;
uint8_t SRF02_2_Status = SRF02_STATUS_IDLE;
uint8_t SRF02_Last_Sensor = SRF02_2;
uint16_t SRF02_1_dist = 0;
uint16_t SRF02_2_dist = 0;

uint8_t SRF02_Is_Enabled = SRF02_DISABLED;

uint16_t SRF02_Dist =0;

uint8_t SRF02IsReady(uint8_t adr)
{
	//I2CTWI_transmitByte(adr, 0);
	//uint8_t firmware = I2CTWI_readByte(adr);
	//return (firmware != 0);
	return getStopwatch7() >= 70;
}

void SRF02StartMeasurement(uint8_t adr)
{
	I2CTWI_transmit2Bytes(adr,0,81);
}

uint16_t SRF02GetDistance(uint8_t adr)
{
	I2CTWI_transmitByte(adr, 3);
	uint16_t lowByte = I2CTWI_readByte(adr);	
	I2CTWI_transmitByte(adr, 2);
	uint16_t highByte = I2CTWI_readByte(adr);
	return (highByte * 256) + lowByte;
}

void calculateAverageDist(void)
{
	SRF02_Dist = (SRF02_1_dist+SRF02_2_dist) / 2;
}

void initSRF02(void)
{
	SRF02_Is_Enabled = SRF02_ENABLED;
	startStopwatch7();
}

void task_SRF02(void)
{
	if(SRF02_Is_Enabled)
	{
		if(SRF02IsReady(I2C_SRF02_ADR_1) && (SRF02_1_Status == SRF02_STATUS_MEASUREMENT))
		{
			uint16_t dist = SRF02GetDistance(I2C_SRF02_ADR_1);
			SRF02_1_dist = dist;
			SRF02_1_Status = SRF02_STATUS_IDLE;
			calculateAverageDist();
		}			
		if(SRF02IsReady(I2C_SRF02_ADR_2) && (SRF02_2_Status == SRF02_STATUS_MEASUREMENT))
		{
			uint16_t dist = SRF02GetDistance(I2C_SRF02_ADR_2);
			SRF02_2_dist = dist;
			SRF02_2_Status = SRF02_STATUS_IDLE;
			calculateAverageDist();
		}
					
		if((SRF02_1_Status == SRF02_STATUS_IDLE) && (SRF02_Last_Sensor == SRF02_2) && (SRF02_2_Status == SRF02_STATUS_IDLE))
		{
			SRF02StartMeasurement(I2C_SRF02_ADR_1);
			SRF02_1_Status = SRF02_STATUS_MEASUREMENT;
			SRF02_Last_Sensor = SRF02_1;
			setStopwatch7(0);
		}	
		if((SRF02_2_Status == SRF02_STATUS_IDLE)  && (SRF02_Last_Sensor == SRF02_1) && (SRF02_1_Status == SRF02_STATUS_IDLE))
		{
			SRF02StartMeasurement(I2C_SRF02_ADR_2);
			SRF02_2_Status = SRF02_STATUS_MEASUREMENT;
			SRF02_Last_Sensor = SRF02_2;
			setStopwatch7(0);
		}	
	}
	else
	{
		if(SRF02_Dist != -1)
		{
			SRF02_Dist = -1;
		}
	}	
}