#include "RP6I2CmasterTWI.h"
#include "RP6ControlLib.h"




/*****************************************************************************/
// The I2C slave address of the compass module CMPS03 is 0xC0:
#define CMPS03_ADR		0xC0


// CMPS03 I2C-ID
#define MEASURE_id		222				// 16-bit compass value

/*****************************************************************************/
// I2C Event handlers:




// wird aus PCConnection aufgerunfen!!
uint16_t CMPS03_result = 0xffff;
void checkCMPS03Status(uint8_t dataRequestID)
{
	uint8_t results[3];
	I2CTWI_transmitByte(CMPS03_ADR, 2);
	
	
	if (dataRequestID == MEASURE_id) {
		I2CTWI_readBytes(CMPS03_ADR, results, 2);
		CMPS03_result = (results[0] << 8) + results[1];
	}

}

void task_CMPS03(void)
{
	I2CTWI_requestDataFromDevice(CMPS03_ADR, MEASURE_id, 1); // receive it
}

