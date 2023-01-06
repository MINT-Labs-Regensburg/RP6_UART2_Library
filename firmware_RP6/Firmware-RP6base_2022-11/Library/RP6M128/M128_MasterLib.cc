#ifndef __M128_MASTER_LIB__
#define __M128_MASTER_LIB__

#define CMD_SET_SPEED               1
#define CMD_SET_SERVO               2
#define CMD_SET_LEDS                3
#define CMD_SET_BEEP                4
#define CMD_SET_START_MELODY        5
#define CMD_SET_FEATURE             6
#define CMD_SET_STOP                7
#define CMD_SET_CONNECTION_SPEED    8
#define CMD_SET_MELODY              9
#define CMD_SET_NEW_ACS_POWER       10
#define CMD_SET_TEST                11
#define CMD_SET_IRCOM               12
#define CMD_RESET_ID_COUNTER        99

#define SET_FEATURE_RP6             0
#define SET_FEATURE_M32             1
#define SET_FEATURE_M128            2
#define SET_FEATURE_SERVOM32        3
#define SET_FEATURE_SERVOM128       4
#define SET_FEATURE_LCDM32          5
#define SET_FEATURE_LCDM128         6
#define SET_FEATURE_SRF02           7
#define SET_FEATURE_SRF02_1         8
#define SET_FEATURE_SRF02_2         9
#define SET_FEATURE_SRF08           10


#define SERVO_M32                   0
#define SERVO_M128                  1

#define LEDS_RP6                    0
#define LEDS_M32                    1
#define LEDS_M128                   2

#define BEEP_M32                    0
#define BEEP_M128                   1

#define TEST_LCD                    0
#define TEST_BEEPER                 1
#define TEST_LED                    2
#define TEST_TEMP                   3
#define TEST_EXTERNAL_MEMORY        4
#define TEST_I2CLED                 5
#define TEST_I2CMOTOR               6

int MelodyTimes[100];
int MelodyPitches[100];

void performCommand(byte cmd, int params[])
{
    switch(cmd)
    {
        case CMD_SET_SPEED:
            moveAtSpeed(params[1],params[2]);
            changeDirection(params[3]);
        break;
        case CMD_SET_SERVO:
            switch(params[1])
            {
                case SERVO_M32:
                    M32_SetServoPosition(params[2],params[3]);
                break;
                case SERVO_M128:
                    SetServoPos(params[2],params[3]*10);    //Klappt noch nicht!!!
                break;
            }
        break;
        case CMD_SET_LEDS:
            switch(params[1])
            {
                case LEDS_RP6:
                    setRP6LEDs(params[2]);
                break;
                case LEDS_M32:
                    M32_SetLEDs(params[2]);
                break;
                case LEDS_M128:
                    setLEDs(params[2]);
                break;
            }
        break;
        case CMD_SET_BEEP:
            switch(params[1])
            {
                case BEEP_M32:
                    M32_SetBeep(params[2],params[3]);
                break;
                case BEEP_M128:
                    beep(params[2],params[3],0);
                break;
            }
        break;
        case CMD_SET_START_MELODY:
            M32_SetStartMelody(params[1],params[2],params[3],params[4]);
        break;
        case CMD_SET_FEATURE:
        {
            switch(params[1])
            {
                case SET_FEATURE_RP6:
                    HasRP6 = params[2];
                break;
                case SET_FEATURE_M32:
                    HasM32 = params[2];
                break;
                case SET_FEATURE_M128:
                    HasM128 = params[2];
                break;
                case SET_FEATURE_SERVOM32:
                    HasServoM32 = params[2];
                break;
                case SET_FEATURE_SERVOM128:
                    HasServoM128 = params[2];
                break;
                case SET_FEATURE_LCDM32:
                    HasLCDM32 = params[2];
                break;
                case SET_FEATURE_LCDM128:
                    HasLCDM128 = params[2];
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
                print("Ton-Counter ist falsch!Counter: ");
                printInteger(current);
                EndDebugFrame();
            }
        }
        break;
        case CMD_SET_NEW_ACS_POWER:
            RP6_writeCMD_1param(RP6_BASE_ADR, CMD_SET_ACS_POWER, params[1]);
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
                case TEST_TEMP:
                    TestTempSensor();
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
            }
        break;
        //case CMD_SET_IRCOM:
        //    RP6_writeCMD_params(RP6_BASE_ADR, CMD_SEND_RC5, params[1], params[2]);
        //break;
    }
}

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

        sound(currPitch);
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
    sound_off();
    stopStopwatch6();
    stopStopwatch7();
}

#endif