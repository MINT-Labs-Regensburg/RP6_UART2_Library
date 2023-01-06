#include "RP6uart.h"
#include "RP6RemotrolCommon.h"

void StartDataFrame(void)
{
	writeString_P("[DATA]\n");
}

void EndDataFrame(void)
{
	writeString_P("[/DATA]\n");
}

void StartErrorFrame(void)
{
	writeString_P("[ERROR]\n");
}

void EndErrorFrame(void)
{
	writeString_P("[/ERROR]\n");
}

void PrintError(char*error)
{
	StartErrorFrame();
	writeString(error);
	EndErrorFrame();
}

void StartDebugFrame(void)
{
	writeString_P("[DEBUG]\n");
}

void EndDebugFrame(void)
{
	writeString_P("[/DEBUG]\n");
}

void PrintDebug(char*debug)
{
	StartDebugFrame();
	writeString(debug);
	EndDebugFrame();
}

void SerialHeartBeat(void)
{
    writeString_P("\n#*#*#\n");
}

uint8_t HasRP6;
uint8_t HasM32;
uint8_t HasM128;
uint8_t HasServoM32;
uint8_t HasServoM128;
uint8_t HasLCDM32;
uint8_t HasLCDM128;
uint8_t HasSRF02;
uint8_t HasSRF02_1;
uint8_t HasSRF02_2;
uint8_t HasSRF08;

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

int params[MAX_PARAMS]; 

int cmdsplit(char*cmd_string) 
{    
	int cursor = 0;  
	int param_nr = 0; 

	// Parameterarray initialisieren 
	for (int i=0;i<MAX_PARAMS;i++) 
		params[i] = -1; 

	/* Prüfen ob der command-string gültig beginnt */ 
	if ((cmd_string == NULL) || (cmd_string[cursor] != '#')) 
		return -1; 

	while (cmd_string[cursor] != '\0' && cmd_string[cursor] != '*') // prüfen ob das ende erreicht wurde 
	{
		char *tmp; 

		cursor++; 
		tmp = (cmd_string + cursor);  // tmp wird auf den anfang des für atoi auszulesenden werts "gesetzt" 
		params[param_nr] = atoi(tmp); // wert auslesen 
		if (param_nr >= MAX_PARAMS)   // sind mehr parameter verfügbar als erwartet? Stimmt was mit dem Commando nicht... 
			return -1; 
		param_nr++; 

		// Überspingen der ausgelesen Zeichen und ermitteln des nächsten "interessanten" Teils im string. 
		while((cmd_string[cursor] != ':') && (cmd_string[cursor] != '*') && (cmd_string[cursor] != '\0')) 
			cursor++; 
	} 

  return param_nr; 
 } 
 
 void printOK(int id, int answer)
{
    writeString_P("\n<OK|");
    writeInteger(id,DEC);
    writeString_P("|");
    writeInteger(answer,DEC);
    writeString_P(">\n");
}


char text[50];
int counter = 0;
int lastID;

int getCommand(void)
{
	if(text[strlen(text)-1] == '*') // Falls das letzte Zeichen das Zeichen für das Befehlsende ist
	{
		int idIndex;
        int id;
		idIndex = cmdsplit(text);
		id = params[idIndex-1];
		
		int i;
		for(i = 0; i< strlen(text);i++)
		{
			text[i] = '\0';
		}
		counter = 0;
		
		if(params[0] == CMD_RESET_ID_COUNTER)
		{
			lastID = -1;
			printOK(id,ANSWER_OK);
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
			writeString_P("Wrong ID received! Details:\r\n");
			writeString_P("ID: ");
			writeInteger(id,DEC);
			writeString_P(" LastID: ");
			writeInteger(lastID,DEC);
			writeString_P("\r\n");
			EndDebugFrame();
		}
	}	
	return 0;
}