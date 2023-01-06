int mic;

void read_mic(void)
{
    while(true)
    {
        // Temperatur lesen:
        Thread_Lock(1);  // Es können nicht mehrere Threads gleichzeitig
                        // auf den I2C Bus zugreifen.
            // Die Übertragung würde sonst gestört.
            // Mit Thread_Lock(1) schaltet man also zunächst
            // das Multithreading aus ...
         // ... führt den kritischen Bereich aus...
        mic = M32_GetMic();
        Thread_Lock(0);    // ... und schaltet es wieder an!
        Thread_Delay(500);
    }
}