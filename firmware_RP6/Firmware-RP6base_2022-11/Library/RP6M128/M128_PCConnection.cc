#ifndef __M128_PC_CONNECTION__
#define __M128_PC_CONNECTION__


#include "RP6CCLib.cc"
#include "RP6MasterLib.cc"

/*
** Funktion zum "parsen" eines command strings.
** Wenn ein fehler auftritt, gibt die funktion (-1) zurück.
** Ansonsten gibt sie die anzahl der ermittelten Parameter zurück.
**
** ACHTUNG: Funktion arbeitet nur korrekt, wenn
**  a) nur EIN zeichen den kommandostring einleitet
**  b) der Trenner kein zeichen ist, das atoi konvertiert.
**  c) nur EIN Zeichen die Endesequenz darstellt (ACHTUNG: seperator + Endezeichen geht auch nicht!)
*/
#define MAX_PARAMS 6
int params[MAX_PARAMS];

int cmdsplit(char cmd_string[])
{
    int cursor;
    cursor=0;
    int param_nr;
    param_nr =0;

    // Parameterarray initialisieren
    int i;
    for (i=0;i<MAX_PARAMS;i++)
        params[i] = -1;

    /* Prüfen ob der command-string gültig beginnt */
    if ((cmd_string[cursor] != '#'))
        return -1;

    while (cmd_string[cursor] != '\0' && cmd_string[cursor] != '*') // prüfen ob das ende erreicht wurde
    {
        char tmp[10];
        Str_Fill(tmp,'x',10);
        cursor++;
        int j;
        j= 0;
        while(cmd_string[cursor] != ':' && cmd_string[cursor] != '*')
        {
            tmp[j] = cmd_string[cursor];
            cursor++;
            j++;
        }
        params[param_nr] = Str_ReadInt(tmp); // wert auslesen
        if (param_nr >= MAX_PARAMS)   // sind mehr parameter verfügbar als erwartet? Stimmt was mit dem Commando nicht...
            return -1;
        param_nr++;

        // Überspingen der ausgelesen Zeichen und ermitteln des nächsten "interessanten" Teils im string.
        while((cmd_string[cursor] != ':') && (cmd_string[cursor] != '*') && (cmd_string[cursor] != '\0'))
            cursor++;
    }

  return param_nr;
 }

char text[20];
int counter;
int lastID;

#define ANSWER_OK               1
#define ANSWER_WRONG_ID         2
#define ANSWER_GENERAL_ERROR    3

byte getCommand(void)
{
    byte len;
    len = StrLen(text);
    if(len >0)
    {
        if(text[len-1] == '*') // Falls das letzte Zeichen das Zeichen für das Befehlsende ist
        {
            int idIndex;
            int id;
            idIndex = cmdsplit(text);
            id = params[idIndex-1];

            int i;
            for(i = 0; i< StrLen(text);i++)
            {
                text[i] = 0;
            }
            counter = 0;

            if(params[0] == CMD_RESET_ID_COUNTER)
		    {
                lastID = -1;
                printOK(id,ANSWER_OK);
                InitOldSensorValues();
                PrintDebug("Resetting ID-Counter");
                return 0;
		    }

            if(id > lastID && (id - lastID < 5))
            {
                printOK(id,ANSWER_OK);
                lastID = id;
                if(id == 99)
                {
                    lastID = -1;
                    PrintDebug("Changing lastID from 99 to -1");
                }
                return params[0];//Und das Befehlskommando zurückgeben
            }
            else
            {
                printOK(id,ANSWER_WRONG_ID);
                StartDebugFrame();
                print("Wrong ID received! Details:\r\n");
                print("ID: ");
                printInteger(id);
                print(" LastID: ");
                printInteger(lastID);
                print("\r\n");
                EndDebugFrame();
            }
        }
    }
    return 0;
}

byte StrLen(char str[])
{
    byte len;
    len = 0;
    while(str[len] != 0)
    {
        len++;
    }
    return len;
}

void printOK(int id, int answer)
{
    print("\n<OK|");
    printInteger(id);
    print("|");
    printInteger(answer);
    print(">\n");
}

byte sendFirmwareData;
int timeBetweenFrames;

void task_Commands(void)
{
    byte count;
    count = Serial_IRQ_Info(0,RS232_FIFO_RECV);
    if(count > 0)
    {
        do
        {
            unsigned char tmp;
            tmp = Serial_ReadExt(0);
            if (tmp =='#')
            {
                counter = 0;
            }
            if(tmp == '*')
            {
                count = 0;
            }
            text[counter] = tmp;
            text[counter + 1] = 0;
            counter++;
            if(count > 0)
                count--;
        } while(count > 0);
        byte cmd;
        cmd = getCommand();
        if(cmd)
            performCommand(cmd,params);
        cmd= 0;
    }
}

void InitOldSensorValues(void)
{
    oldBat = -999;
    oldSpeedL = -999;
    oldSpeedR = -999;
    oldPowerL = -999;
    oldPowerR = -999;
    oldLightL = -999;
    oldLightR = -999;
    oldObsL = -999;
    oldObsR = -999;
    oldBumpL = -999;
    oldBumpR = -999;
    oldDist = -999;
    oldMic = -999;
    oldTemp = -999;
    oldADC0 = -999;
    oldLEDsBase = -999;
    oldRC5_Adr = -999;
    oldRC5_Toggle = -999;
    oldRC5_Code = -999;
    oldSnakeL = -999;         //ADC0_M128
    oldSnakeR = -999;         //ADC7_M128
    oldHydro = -999;          //ADC2_M128 -> Hygrometer
    oldBat2 = -999;           //ADC4_M128
    oldTasterSnake = -999;    //ADC1_M128
    oldButtonM32 = -999;      //ButtonM32
    oldmleft_dist_neu = -999;   //Alte "neu gefahrene" Distanz links
    oldmright_dist_neu = -999;  //Alte "neu gefahrene" Distanz rechts
    oldmleft_dist = -999;       //Alte gefahrene Distanz links
    oldmright_dist = -999;      //Alte gefahrene Distanz rechts
    oldSRF02_Dist = -999;       //Alter Abstande SRF02_M32

}

void StartDataFrame(void)
{
    print("[DATA]\n");
}

void EndDataFrame(void)
{
    print("[/DATA]\n");
}

void StartErrorFrame(void)
{
    print("[ERROR]\n");
}

void EndErrorFrame(void)
{
    print("[/ERROR]\n");
}

void PrintError(char error[])
{
    StartErrorFrame();
    Serial_WriteText(0, error);
    EndErrorFrame();
}

void StartDebugFrame(void)
{
    print("[DEBUG]\n");
}

void EndDebugFrame(void)
{
    print("[/DEBUG]\n");
}

void PrintDebug(char debug[])
{
    StartDebugFrame();
    Serial_WriteText(0, debug);
    EndDebugFrame();
}

void SerialHeartBeat(void)
{
    print("\n#*#*#\n");
}

int LeftBumper;

int oldBat;
int oldSpeedL;
int oldSpeedR;
int oldPowerL;
int oldPowerR;
int oldLightL;
int oldLightR;
int oldADC0;
int oldADC1;
int oldObsL;
int oldObsR;
int oldBumpL;
int oldBumpR;
int oldLEDsBase;
int oldBumpHL;
int oldBumpHR;
int oldRC5_Adr;
int oldRC5_Toggle;
int oldRC5_Code;
int oldDist;
int oldMic;
float oldTemp;
int oldSnakeL;      //ADC0_M128
int oldSnakeR;      //ADC7_M128
int oldHydro;       //ADC2_M128
int oldBat2;        //ADC4_M128
int oldTasterSnake; //ADC1_M128
int oldButtonM32;   //ButtonM32
int oldmleft_dist_neu;   //Alte "neu gefahrene" Distanz links
int oldmright_dist_neu;  //Alte "neu gefahrene" Distanz rechts
int oldmleft_dist;       //Alte gefahrene Distanz links
int oldmright_dist;      //Alte gefahrene Distanz rechts
int oldSRF02_Dist;       //Alter Abstande SRF02_M32

/*void umrechnungEnternung(void){
	if(oldmleft_dist_neu > mleft_dist){
		mleft_dist_neu = mleft_dist;
	}else{
		mleft_dist_neu = mleft_dist - oldmleft_dist_neu;
	}
	if(oldmright_dist_neu > mright_dist){
		mright_dist_neu = mright_dist;
	}else{
		mright_dist_neu = mright_dist - oldmright_dist_neu;
	}

	oldmleft_dist_neu = mleft_dist;
	oldmright_dist_neu = mright_dist;
}*/

byte sendData;

void printSensors(void)
{
    char buffer[20];

    if(sendData)
    {
        StartDataFrame();

        if(HasRP6)
        {
            //umrechnungEnternung();

            if(oldBat != adcBat)
            {
              print("Bat:");
              printInteger(adcBat);
              print("\n");
              oldBat = adcBat;
            }

            if(oldmleft_dist != mleft_dist)
            {
              print("DistL:");
              printInteger(mleft_dist);
              print("\n");
              oldmleft_dist = mleft_dist;
            }

            if(oldmright_dist != mright_dist)
            {
              print("DistR:");
              printInteger(mright_dist);
              print("\n");
              oldmright_dist = mright_dist;
            }

            /*if(oldmleft_dist_neu != mleft_dist_neu)
            {
              print("Dist_n_L:");
              printInteger(mleft_dist_neu);
              print("\n");
              oldmleft_dist_neu = mleft_dist_neu;
            }

            if(oldmright_dist_neu != mright_dist_neu)
            {
              print("Dist_n_R:");
              printInteger(mright_dist_neu);
              print("\n");
              oldmright_dist_neu = mright_dist_neu;
            }*/

            if(oldSpeedL != mleft_speed)
            {
                print("SpeedL:");
                printInteger(mleft_speed);
                print("\n");
                oldSpeedL = mleft_speed;
            }

            if(oldSpeedR != mright_speed)
            {
                print("SpeedR:");
                printInteger(mright_speed);
                print("\n");
                oldSpeedR = mright_speed;
            }

            if(oldPowerL != adcMotorCurrentLeft)
            {
                print("PowerL:");
                printInteger(adcMotorCurrentLeft);
                print("\n");
                oldPowerL = adcMotorCurrentLeft;
            }

            if(oldPowerR != adcMotorCurrentRight)
            {
                print("PowerR:");
                printInteger(adcMotorCurrentRight);
                print("\n");
                oldPowerR = adcMotorCurrentRight;
            }

            print("LightL:");
            printInteger(adcLSL);
            print("\n");

            print("LightR:");
            printInteger(adcLSR);
            print("\n");

            print("ADC0_Base:");
            printInteger(adc0);
            print("\n");

            print("ADC1_Base:");
            printInteger(adc1);
            print("\n");

            if(oldObsL != obstacleLeft)
            {
                print("ObsL:");
                printInteger(obstacleLeft);
                print("\n");
                oldObsL = obstacleLeft;
            }

            if(oldObsR != obstacleRight)
            {
                print("ObsR:");
                printInteger(obstacleRight);
                print("\n");
                oldObsR = obstacleRight;
            }

            if(oldBumpR != bumperRight)
            {
                print("BumpR:");
                printInteger(bumperRight);
                print("\n");
                oldBumpR = bumperRight;
                M32_SetBeep(100,100);
            }

            if(oldBumpL != LeftBumper)
            {
                print("BumpL:");
                //printInteger(bumperLeft);        //die Variable bumperLeft klappt nicht, sie beinhaltet immer 0..Keine Ahnung warum
                printInteger(LeftBumper);          //Beim Auslesen von bumperLeft weise ich diesen Wert direkt der Varaible LeftBumper zu. Damit klappt es dann...
                print("\n");                       //Strange, aber naja...
                oldBumpL = LeftBumper;
                M32_SetBeep(100,100);
            }

            if(oldLEDsBase != status_leds_base)
            {
                print("LEDsBase:");
                printInteger(status_leds_base);
                print("\n");
                oldLEDsBase = status_leds_base;
            }

            if(oldBumpHL != bumper_back_left)
            {
                print("BumpHL:");
                printInteger(bumper_back_left);
                print("\n");
                oldBumpHL = bumper_back_left;
                M32_SetBeep(200,100);
            }

            if(oldBumpHR != bumper_back_right)
            {
                print("BumpHR:");
                printInteger(bumper_back_right);
                print("\n");
                oldBumpHR = bumper_back_right;
                M32_SetBeep(200,100);
            }

            /*if(oldRC5_Adr != RC5_adr)        //RC5_Adr funktioniert irgendiwe nicht
            {                                  //Wenns drinnen ist, kommt das Programm
                print("RC5-Adr:");             //zum Stocken
                printInteger(RC5_adr);
                print("\n");
                oldRC5_Adr = RC5_adr;
            }*/

            if(oldRC5_Toggle != RC5_toggle)
            {
                print("RC5-Toggle:");
                printInteger(RC5_toggle);
                print("\n");
                oldRC5_Toggle = RC5_toggle;
            }

            if(oldRC5_Code != RC5_data)
            {
                print("RC5-Code:");
                printInteger(RC5_data);
                print("\n");
                oldRC5_Code = RC5_data;
            }
        }

        if(HasSRF02)
        {
            if(oldDist != SRF02_Dist)
            {
                print("Distance:");
                printInteger(SRF02_Dist);
                print("\n");
                oldDist = SRF02_Dist;
            }
        }

        if(HasM32)
        {
            int mic;
            mic = M32_GetMic();
            SRF02_Dist = M32_GetSRF02Dist();

            if(oldMic != mic)
            {
                print("Mic:");
                printInteger(mic);
                print("\n");
                oldMic = mic;
            }

            if(oldSRF02_Dist != SRF02_Dist)
            {
                print("SRF02_Dist:");
                printInteger(SRF02_Dist);
                print("\n");
                oldSRF02_Dist = SRF02_Dist;
            }


            int ButtonM32;
            ButtonM32 = M32_GetButtons();
            if(oldButtonM32 != ButtonM32)
            {
                print("ButtonM32:");
                printInteger(ButtonM32);
                print("\n");
                oldButtonM32 = ButtonM32;
            }
        }
        if(HasM128)
        {
            if (oldSnakeL != SnakeL)
            {
                print("SnakeL:");         // Messwert ausgeben
                printInteger(SnakeL);
                print("\n");
                oldSnakeL = SnakeL;
             }

             if (oldSnakeR != SnakeR)
            {
                print("SnakeR:");
                printInteger(SnakeR);
                print("\n");
                oldSnakeR = SnakeR;
             }

             if (oldTasterSnake != TasterSnake)
            {
                print("TasterSnake:");
                printInteger(TasterSnake);
                print("\n");
                oldTasterSnake = TasterSnake;
             }

             if (oldHydro != Hydro)
            {
                print("Hydro:");
                printInteger(Hydro);
                print("\n");
                oldHydro = Hydro;
             }

             if (oldBat2 != Bat2)
            {
                print("Bat2:");
                printInteger(Bat2);
                print("\n");
                oldBat2 = Bat2;
             }

            if(oldTemp != temperature)
            {
                print("Temp:");
                Str_WriteFloat(temperature*100,0, buffer, 0);
                Serial_WriteText(0,buffer);
                print("\n");
                oldTemp = temperature;
            }
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

void LCDHeartBeat(void)
{
    if(HasLCDM128)
    {
        static int heartbeat;
        if(heartbeat)
        {
            clearPosLCD(0, 15, 1);
            heartbeat = false;
        }
        else
        {
            setCursorPosLCD(0, 15);
            writeStringLCD("*");
            heartbeat = true;
            setCursorPosLCD(0,0);
            writeStringLCD("RP6 Remotrol");
        }
    }
}

void taskPCConnection(void)
{
    task_Commands();
    if(HasServoM128)
        task_Servos();
    if(HasSRF02)
        task_SRF02();
    if(HasRP6)
        thread_checkINT0();
    if(HasM128)
        read_temperature();
    SendDebugData();
    if(getStopwatch2() >= 500)
    {
        SerialHeartBeat();
        LCDHeartBeat();
        setStopwatch2(0);
    }
}


//Stopwatches:
//1:    SendDebugData
//2:    Heartbeat
//3,4:  Temperatur
//5:    SRF02

void initPCConnection(void)
{
    initServos();
    InitializeFeatures();
    counter = 0;
    timeBetweenFrames = 240;
    sendData = 1;

    lastID = -1;

    clearLCD();
    setCursorPosLCD(0,0);
    initSRF02();

    RP6_writeCMD_1param(RP6_BASE_ADR, CMD_SET_ACS_POWER, ACS_PWR_HIGH);
    RP6_writeCMD_1param(RP6_BASE_ADR, CMD_SET_WDT, true);
    RP6_writeCMD_1param(RP6_BASE_ADR, CMD_SET_WDT_RQ, true);

    initTempSensor();
    setStopwatch1(0);
    setStopwatch2(0);
    startStopwatch1();
    startStopwatch2();
}

#endif