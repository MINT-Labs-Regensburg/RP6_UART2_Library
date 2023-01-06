#ifndef __M128_INT0_LIB__
#define __M128_INT0_LIB__

void handle_ACS(void){}

void handle_Bumpers(void){}

void handle_BatLow(void){}

void handle_DriveSystemChange(void){}

void handle_RC5_reception(void){}

byte heartbeat;
void handle_WDT_RQ(void)
{
    if(heartbeat)
    {
        clearPosLCD(0, 14, 1);
        heartbeat = false;
    }
    else
    {
        setCursorPosLCD(0,14);
        printLCD("#");
        heartbeat = true;
    }
}

void thread_checkINT0(void)
{
        if(Port_ReadBit(PORT_PE6_INT))
        {
            RP6_readRegisters(RP6_BASE_ADR, 0, messageBuf, 3);

            byte compare;
            compare = messageBuf[0] ^ interrupt_status;

            interrupt_status = messageBuf[0];

            if(compare & MASK_ACS)
            {
                 // Die ACS Status Variablen setzen:
                 obstacleLeft = messageBuf[0] & 0x20;
                 obstacleRight = messageBuf[0] & 0x40;

                 // ACS in getrennter Funktion weiterbehandeln:
                 handle_ACS();
            }

            if(compare & MASK_BUMPER)
            {
                 bumperLeft = messageBuf[0] & 0x2;
                 bumperRight = messageBuf[0] & 0x4;
                 LeftBumper = bumperLeft;

                 handle_Bumpers();
            }

            /*if(interrupt_status & MASK_RC5)
            {
                RP6_readRegisters(RP6_BASE_ADR, I2C_REG_RC5_ADR, messageBuf, 2);
                RC5_toggle = messageBuf[0] >> 5;
                RC5_adr = messageBuf[0] & 0x1F;
                RC5_data = messageBuf[1];

                handle_RC5_reception();
            }  */

            /*if(compare & MASK_BATLOW)
            {
                RP6_readRegisters(RP6_BASE_ADR, I2C_REG_ADC_UBAT_L, messageBuf, 2);
                adcBat = (messageBuf[1] << 8) | (messageBuf[0]);

                handle_BatLow();
            }*/

            /*if(compare & MASK_DRIVE)
            {
                 movementComplete = messageBuf[2] & 1;
                 motorOvercurrent = messageBuf[2] & 4;
                 handle_DriveSystemChange();
            }*/

            if(messageBuf[1] & MASK_WDT_RQ)
            {
                handle_WDT_RQ();
            }
    }
}

#endif