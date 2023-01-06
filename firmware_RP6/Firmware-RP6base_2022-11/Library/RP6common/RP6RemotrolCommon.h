#define CMD_SET_SPEED               1
#define CMD_SET_SERVO               2
#define CMD_SET_LEDS                3
#define CMD_SET_BEEP                4
#define CMD_SET_START_MELODY        5
#define CMD_SET_FEATURE	 	        6
#define CMD_SET_STOP                7
#define CMD_SET_CONNECTION_SPEED    8
#define CMD_SET_MELODY              9
#define CMD_SET_ACSPOWER            10
#define CMD_SET_TEST                11

#define CMD_RESET_ID_COUNTER		99

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

#define LEDS_RP6          			0
#define LEDS_M32            		1

#define TEST_LCD                    0
#define TEST_BEEPER                 1
#define TEST_LED                    2
#define TEST_EXTERNAL_MEMORY        4
#define TEST_I2CLED                 5
#define TEST_I2CMOTOR               6
#define TEST_MIC                    7
#define TEST_MOTOR                  8
#define TEST_BATTERY                9
#define TEST_ACS                    10  
#define TEST_BUMPER					11
#define TEST_LIGHTSENSOR			12

#define ACS_POWER_OFF				0
#define ACS_POWER_LOW				1
#define ACS_POWER_MED				2
#define ACS_POWER_HIGH				3

#define ANSWER_OK               1
#define ANSWER_WRONG_ID         2
#define ANSWER_GENERAL_ERROR    3

#define MAX_PARAMS 6 


extern char text[50];
extern int params[MAX_PARAMS]; 
extern int counter;

extern uint8_t HasRP6;
extern uint8_t HasM32;
extern uint8_t HasM128;
extern uint8_t HasServoM32;
extern uint8_t HasServoM128;
extern uint8_t HasLCDM32;
extern uint8_t HasLCDM128;
extern uint8_t HasSRF02;
extern uint8_t HasSRF02_1;
extern uint8_t HasSRF02_2;
extern uint8_t HasSRF08;

void SerialHeartBeat(void);
void StartDataFrame(void);
void EndDataFrame(void);
void StartErrorFrame(void);
void EndErrorFrame(void);
void StartDebugFrame(void);
void EndDebugFrame(void);
void PrintDebug(char*debug);
void PrintError(char*error);
int getCommand(void);