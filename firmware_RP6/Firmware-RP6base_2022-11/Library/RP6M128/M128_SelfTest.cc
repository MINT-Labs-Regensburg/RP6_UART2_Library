#ifndef __M128_SELFTEST_LIB__
#define __M128_SELFTEST_LIB__

void TestLCD(void)
{
    TestStart("LCD");
    AbsDelay(750);
    SerialHeartBeat();
    AbsDelay(750);
    showScreenLCD("################","################");
    SerialHeartBeat();
    AbsDelay(1000);
    SerialHeartBeat();
    AbsDelay(1000);
    showScreenLCD("****************","****************");
    SerialHeartBeat();
    AbsDelay(1000);
    SerialHeartBeat();
    AbsDelay(1000);
    showScreenLCD("################","################");
    SerialHeartBeat();
    AbsDelay(1000);
    SerialHeartBeat();
    AbsDelay(1000);
    showScreenLCD("Test bestanden! ","<<<<<<<-->>>>>>>");
    SerialHeartBeat();
    AbsDelay(1000);
    SerialHeartBeat();
    AbsDelay(1000);
    SerialHeartBeat();
    TestEnd(1);
}

void TestBeeper(void)
{
    TestStart("Beeper");
    beep(100,350,0);
    beep(200,200,0);
    SerialHeartBeat();
    beep(150,350,0);
    beep(100,400,0);
    SerialHeartBeat();
    beep(200,750,0);
    TestEnd(1);
}

void TestLEDs(void)
{
    TestStart("LED");
    setLEDs(0);
    byte leds;
    word i;
    leds = 1;
    for(i = 0; i < 26; i++)
    {
        setLEDs(leds);
        leds = leds << 1;
        if(leds > 17)
            leds = 1;
        if(i % 10 == 0)
        {
            SerialHeartBeat();
        }
        AbsDelay(100);
    }
    setLEDs(0);
    TestEnd(1);
}

int i;
int testResult;

void TestTempSensor(void)
{
    TestStart("TempSensor");
    int lastTemp;
    lastTemp = 0;
    testResult = 1;
    for(i = 0; i < 16; i++)
    {
        TCN75_run(TCN75_ADR, TCN75A_CONFIG_RES_12);
        AbsDelay(250);
        TCN75_shutdown(TCN75_ADR);
        TCN75_read(TCN75_ADR);
        float temperature;
        char result[32];
        int temp_low;

        temp_low = getTemperatureLow();
        if(!temp_low & 128)
            temp_low = (temp_low & 63) - 127;
        else
            temp_low = temp_low & 63;
        temperature = temp_low + (0.0625 * (getTemperatureHigh()>>4));

        print("Temperature: ");
        Str_WriteFloat(temperature,4,result,0);
        Serial_WriteText(0,result);
        newline();
        SerialHeartBeat();
        newline();

        if(i == 0)
        {
            lastTemp = temperature;
        }
        int diff;
        diff = lastTemp - temperature;
        lastTemp = temperature;
        print("Diff: ");
        printInteger(diff);
        newline();
        if(diff > 2 || diff < -2)
        {
            testResult = 0;
        }
    }
    TestEnd(testResult);
}

void TestExternalMemory(void)
{
    TestStart("ExternalMemory");
    byte pattern;
    char result[64];
	char tmp[101];
    char tmp2[101];
    byte memtest[62000];
    word j,k,l;
    testResult = 1;
    pattern = 0x11;
    for(i = 0; i < 8; i++) {
        tmp = "MEM PATTERN: ";
        Str_Copy(result,tmp,0);
        Str_WriteInt(pattern,result,STR_APPEND);
        Serial_WriteText(0,result);
        SerialHeartBeat();

        println(" - Writing...");

        Str_Fill(tmp, pattern, 100);
        for(j = 0; j < 620; j++) {
              Str_Copy(memtest, tmp, j*100);
        }
        println("Reading...");
        for(j = 0; j < 62000; j = j + 5)
        {
            if(memtest[j] != pattern)
            {
                tmp = "MEMTEST FAIL: ";
                Str_Copy(result,tmp,0);
                Str_WriteInt(j,result,STR_APPEND);
                tmp = " !!!";
                Str_Copy(result,tmp,STR_APPEND);
                Serial_WriteText(0,result);
                testResult = 0;
            }
        }
        println("OK");
        pattern = pattern << 1;
    }
    TestEnd(testResult);
}

void TestI2CLEDs(void)
{
    TestStart("I2C LED");
    byte led_test;
    byte leds;
    leds = 1;
    testResult = 1;
    for(i = 0; i < 25; i++)
    {
        setRP6LEDs(leds);
        AbsDelay(200);
        led_test = RP6_readRegister(RP6_BASE_ADR, I2C_REG_LEDS);
        if(led_test != leds)
        {
            println("I2C COMMUNICATION ERROR!");
            print("Should be: ");
            printInteger(leds);
            println("");
            print("Is: ");
            printInteger(led_test);
            println("");
            testResult = 0;
        }
     	leds = leds << 1;
        if(leds > 63)
            leds = 1;
        SerialHeartBeat();
        AbsDelay(100);
    }
    setRP6LEDs(0);

    TestEnd(testResult);
}

void TestI2CMotor(void)
{
    TestStart("I2C Motor");

    int result;
    result = 1;

	int i;
	int j;
	for(j = 0; j < 2;j++)
	{
		changeDirection(j);
		for(i = 5; i <= 14-j; i++)
		{
			SerialHeartBeat();
			int speed;
            speed = i*10;
			moveAtSpeed(speed,speed);
			AbsDelay(850);
			SerialHeartBeat();
			AbsDelay(850);
			getAllSensors();

			Serial_WriteText(0, "Sollgeschwindigkeit:       ");
			printInteger(speed);
			Serial_WriteText(0, "\n");

			Serial_WriteText(0, "Geschwindigkeit links:    ");
			printInteger(mleft_speed);
			if(mleft_speed > speed - 15 && mleft_speed < speed + 15)
			{
				Serial_WriteText(0, " --> OK");
			}
			else
			{
				Serial_WriteText(0, " --> FEHLER");
				result = 0;
			}
			Serial_WriteText(0, "\n");

			Serial_WriteText(0, "Geschwindigkeit rechts: ");
			printInteger(mright_speed);
			if(mright_speed > speed - 15 && mright_speed < speed + 15)
			{
				Serial_WriteText(0, " --> OK");
			}
			else
			{
				Serial_WriteText(0, " --> FEHLER");
				result = 0;
			}
			Serial_WriteText(0, "\n");
		}
		moveAtSpeed(0,0);
		SerialHeartBeat();
		AbsDelay(500);
        SerialHeartBeat();
		AbsDelay(500);
	}
	moveAtSpeed(0,0);

    if(!result)
    {
        Serial_WriteText(0, "Während des Tests sind Fehler aufgetreten!!!");
    }

    TestEnd(result);
}

char lastTest[30];

void TestEnd(byte success)
{
    Serial_WriteText(0, "*##");
    Serial_WriteText(0, lastTest);
    Serial_WriteText(0, "|");
    printInteger(success);
    Serial_WriteText(0, "##*");
    newline();
    clearLCD();
}

void TestStart(char name[])
{
    Str_Fill(lastTest,0,30);
    Str_Copy(lastTest,name,0);
    Serial_WriteText(0, "#**");
    Serial_WriteText(0, name);
    Serial_WriteText(0, "**#");
    newline();
    clearLCD();
    writeLineLCD("      Test",0,0);
    writeLineLCD(name,1,0);
    AbsDelay(100);
}

#endif





