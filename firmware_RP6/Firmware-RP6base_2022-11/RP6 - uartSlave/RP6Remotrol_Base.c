#include "PCConnection_Base.h"
#include "RP6RobotBaseLib.h"

int main(void) 
{
	initRobotBase();
	powerON();
	setACSPwrHigh();
	initializeConnection();			

	while(true)  
	{ 	
		task_RP6System();	
		task_PCConnection();						
	} 
	return 0; 
} 

