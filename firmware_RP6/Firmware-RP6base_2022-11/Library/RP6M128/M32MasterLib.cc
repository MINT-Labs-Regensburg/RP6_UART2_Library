#ifndef __M32_MASTER_LIB__
#define __M32_MASTER_LIB__

#define RP6Control_I2C_SLAVE_ADR    20

#define I2C_MICROPHONEPEAKLOW	    0
#define I2C_MICROPHONEPEAKHIGH	    1
#define I2C_BUTTON		 		    2
#define I2C_ADC_2		 		    3
#define I2C_ADC_3		 		    4
#define I2C_ADC_4		 		    5
#define I2C_ADC_5		 		    6
#define I2C_ADC_6		 		    7
#define I2C_ADC_7		 		    8
#define I2C_RF02_DIST			    9

void M32_SetServoPosition(byte servo, byte pos)
{
    byte params[2];
    params[0] = servo;
    params[1] = pos;
    M32_SendCommand(CMD_SET_SERVO,params);
}

void M32_SetLEDs(byte leds)
{
    byte params[1];
    params[0] = leds;
    M32_SendCommand(CMD_SETLEDS,params);
}

void M32_SetBeep(byte pitch, int time)
{
    byte params[3];
    params[0] = pitch;
    params [1] = (time & 0x00ff);
    params[2] = (time & 0xff00) >> 8;
    M32_SendCommand(CMD_SET_BEEP,params);
}

void M32_SetStartMelody(byte totalCount, byte currentCount, byte pitch, int time)
{
    byte params[5];
    params[0] = totalCount;
    params[1] = currentCount;
    params[2] = pitch;
    params[3] = (time & 0x00ff);
    params[4] = (time & 0xff00) >> 8;
    M32_SendCommand(CMD_SET_START_MELODY,params);
}

int M32_GetMic(void)
{
    byte low,high;
    low = RP6_readRegister(RP6Control_I2C_SLAVE_ADR, I2C_MICROPHONEPEAKLOW);
    high = RP6_readRegister(RP6Control_I2C_SLAVE_ADR, I2C_MICROPHONEPEAKHIGH);

    return (high*256)+low;
}

int M32_GetSRF02Dist(void)
{
    SRF02_Dist = RP6_readRegister(RP6Control_I2C_SLAVE_ADR, I2C_RF02_DIST);
    return SRF02_Dist;
}

int M32_GetButtons(void)
{
    ButtonM32 = RP6_readRegister(RP6Control_I2C_SLAVE_ADR, I2C_BUTTON);
    return ButtonM32;
}

void M32_SendCommand(byte cmd, byte params[])
{
    RP6_writeCommand_params(RP6Control_I2C_SLAVE_ADR,cmd,params,6);
}

#endif