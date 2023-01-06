#ifndef __M128TEMP_LIB__
#define __M128TEMP_LIB__

#define running 1
#define idle 0
float temperature;
int state;

void initTempSensor(void)
{
    state = idle;
    setStopwatch3(0);
    startStopwatch3();
    setStopwatch4(0);
    startStopwatch4();
}

void read_temperature(void)
{
    if(getStopwatch3() >= 750)
    {
        if(state == idle)
        {
            // Temperatur lesen:
            TCN75_run(TCN75_ADR, TCN75A_CONFIG_RES_12);
            state = running;
            startStopwatch4();
        }
        else if(getStopwatch4()>=250)
        {
            TCN75_shutdown(TCN75_ADR);
            TCN75_read(TCN75_ADR);
            // ---------------------------------
            char tmp[16],result[32];
            int temp_low;
            // Messwert in "float" Wert konvertieren (12 Bit Messung):
            temp_low = getTemperatureLow();
            if(temp_low & 128)
                temp_low = (temp_low & 63) - 127;
            else
                temp_low = temp_low & 63;
            temperature = temp_low + (0.0625 * (getTemperatureHigh()>>4));
            state = idle;
            setStopwatch3(0);
            stopStopwatch4();
            setStopwatch4(0);
        }
    }
}

#endif