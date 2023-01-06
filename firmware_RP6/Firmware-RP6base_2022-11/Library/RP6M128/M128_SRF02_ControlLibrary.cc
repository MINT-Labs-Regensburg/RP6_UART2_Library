#ifndef __M128_SRF02_LIB__
#define __M128_SRF02_LIB__

#define I2C_SRF02_ADR_1 0xE0
#define I2C_SRF02_ADR_2 0xE2

#define SRF02_STATUS_MEASUREMENT 0
#define SRF02_STATUS_IDLE 1

#define SRF02_1 0
#define SRF02_2 1

byte SRF02_1_Status;
byte SRF02_2_Status;
byte SRF02_Last_Sensor;
int SRF02_1_dist;
int SRF02_2_dist;

int SRF02_Dist;

byte SRF02IsReady(byte adr)
{
    //I2CTWI_transmitByte(adr, 0);
    //uint8_t firmware = I2CTWI_readByte(adr);
    //return (firmware != 0);
    return getStopwatch5() >= 70;
}

void SRF02StartMeasurement(byte adr)
{
    RP6_writeCMD(adr, 81);
}

int SRF02GetDistance(byte adr)
{
    byte lowByte;
    lowByte = RP6_readRegister(adr, 3);
    byte highByte;
    highByte = RP6_readRegister(adr, 2);
    return (highByte * 256) + lowByte;
}



void calculateAverageDist(void)
{
    SRF02_Dist = (SRF02_1_dist+SRF02_2_dist) / 2;
}

void initSRF02(void)
{
    SRF02_1_Status = SRF02_STATUS_IDLE;
    SRF02_2_Status = SRF02_STATUS_IDLE;
    SRF02_Last_Sensor = SRF02_2;
    SRF02_1_dist = 0;
    SRF02_2_dist = 0;
    SRF02_Dist =0;
    setStopwatch5(0);
    startStopwatch5();
}

void task_SRF02(void)
{
    int dist;
    if(SRF02IsReady(I2C_SRF02_ADR_1) && (SRF02_1_Status == SRF02_STATUS_MEASUREMENT))
    {
        dist = SRF02GetDistance(I2C_SRF02_ADR_1);
        SRF02_1_dist = dist;
        SRF02_1_Status = SRF02_STATUS_IDLE;
        calculateAverageDist();
    }
    if(SRF02IsReady(I2C_SRF02_ADR_2) && (SRF02_2_Status == SRF02_STATUS_MEASUREMENT))
    {
        dist = SRF02GetDistance(I2C_SRF02_ADR_2);
        SRF02_2_dist = dist;
        SRF02_2_Status = SRF02_STATUS_IDLE;
        calculateAverageDist();
    }

    if((SRF02_1_Status == SRF02_STATUS_IDLE) && (SRF02_Last_Sensor == SRF02_2) && (SRF02_2_Status == SRF02_STATUS_IDLE))
    {
        SRF02StartMeasurement(I2C_SRF02_ADR_1);
        SRF02_1_Status = SRF02_STATUS_MEASUREMENT;
        SRF02_Last_Sensor = SRF02_1;
        setStopwatch5(0);
    }
    if((SRF02_2_Status == SRF02_STATUS_IDLE)  && (SRF02_Last_Sensor == SRF02_1) && (SRF02_1_Status == SRF02_STATUS_IDLE))
    {
        SRF02StartMeasurement(I2C_SRF02_ADR_2);
        SRF02_2_Status = SRF02_STATUS_MEASUREMENT;
        SRF02_Last_Sensor = SRF02_2;
        setStopwatch5(0);
    }
}

#endif