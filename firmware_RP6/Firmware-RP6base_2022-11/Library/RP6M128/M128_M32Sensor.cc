int mic;

void read_mic(void)
{
    while(true)
    {
        // Temperatur lesen:
        Thread_Lock(1);  // Es k�nnen nicht mehrere Threads gleichzeitig
                        // auf den I2C Bus zugreifen.
            // Die �bertragung w�rde sonst gest�rt.
            // Mit Thread_Lock(1) schaltet man also zun�chst
            // das Multithreading aus ...
         // ... f�hrt den kritischen Bereich aus...
        mic = M32_GetMic();
        Thread_Lock(0);    // ... und schaltet es wieder an!
        Thread_Delay(500);
    }
}