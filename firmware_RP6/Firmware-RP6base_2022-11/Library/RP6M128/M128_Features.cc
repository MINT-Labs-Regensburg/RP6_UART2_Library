#ifndef __M128_FEATURES__
#define __M128_FEATURES__

byte HasRP6;
byte HasM32;
byte HasM128;
byte HasServoM32;
byte HasServoM128;
byte HasLCDM32;
byte HasLCDM128;
byte HasSRF02;
byte HasSRF02_1;
byte HasSRF02_2;
byte HasSRF08;

void InitializeFeatures(void)
{
    HasRP6 = 1;
    HasM32 = 1;
    HasM128 = 1;
    HasServoM32 = 1;
    HasServoM128 = 1;
    HasLCDM32 = 1;
    HasLCDM128 = 1;
    HasSRF02 = 0;
    HasSRF02_1 = 0;
    HasSRF02_2 = 0;
    HasSRF08 = 0;
}
 #endif