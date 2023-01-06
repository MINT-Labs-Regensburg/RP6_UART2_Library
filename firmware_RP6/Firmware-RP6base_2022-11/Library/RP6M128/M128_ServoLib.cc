#ifndef __M128_SERVO_LIB__
#define __M128_SERVO_LIB__

//------------------------------------------------------------------------------
// Defines:

// Servo Konstanten:
#define SERVO1 13       // Servo an PB5
#define SERVO2 14       // Servo an PB6
#define SERVO3 15       // Servo an PB7

// Servo Bewegungsgrenzen 0,7..2,3ms (abhängig vom Servotyp):
#define LINKER_ANSCHLAG      540                      // Linker Anschlag  (0,7ms)
#define RECHTER_ANSCHLAG   (1860 - LINKER_ANSCHLAG) // Rechter Anschlag (2,3ms)
#define MITTE_POSITION      (RECHTER_ANSCHLAG / 2)   // Mitte-Position   (1,5ms)
// Wenn man die Bewegungsgrenzen für ein bestimmtes Servo einstellen will,
// muss erst der linke Anschlag so eingestellt werden, dass das Servo bei
// servo_pos = 0 am linken Anschlag steht und nicht "brummt", d.h. gegen
// den Anschlag arbeitet. Die Werte für LINKER_ANSCHLAG werden zwischen
// 700 und 1000 (us) liegen.
// Danach muss mit servo_pos = RECHTER_ANSCHLAG die Stellung am rechten
// Anschlag gesucht werden, bei der das Servo ebenfalls nicht "brummt",
// indem der Wert "2300" in der Definition von RECHTER_ANSCHLAG geändert
// wird. Die Werte werden zwischen 2000 und 2300 (us) liegen.
// Nach dieser Einstellung kann man das Servo mit Werten (servo_pos) von
// 0 (= linker Anschlag) bis RECHTER_ANSCHLAG bewegen. Natürlich könnte
// man auch direkt die Werte für die Impulsdauer (us) benutzen. Der Vor-
// teil dieser Umrechnung auf Positionswerte besteht darin, dass man die
// Servogrenzen vorab gut einstellen kann.

//------------------------------------------------------------------------------
// Variablen:

byte servo_var[9];                          // globale Variablendeklaration
unsigned int servo_pos[3], impulsdauer[3];

void initServos(void)
{
    // Initialisierung für 3 Servos, Periodendauer 20ms, Timer 3:
    Servo_Init(3, 1, servo_var, 1);
    servo_pos[0] = 600;             //Servo auf PB6
    servo_pos[1] = 800;             //Servo auf PB5
    servo_pos[2] = 600;             //Servo auf PB7

    impulsdauer[0] = LINKER_ANSCHLAG;
    impulsdauer[1] = LINKER_ANSCHLAG;
    impulsdauer[2] = LINKER_ANSCHLAG;
}

void SetServoPos(byte servo, int pos)
{
    servo_pos[servo-1] = pos;
}

void task_Servos(void)
{
    impulsdauer[0] = LINKER_ANSCHLAG + servo_pos[0];
    impulsdauer[1] = LINKER_ANSCHLAG + servo_pos[1];
    impulsdauer[2] = LINKER_ANSCHLAG + servo_pos[2];

    Servo_Set(SERVO1, impulsdauer[0]);
    Servo_Set(SERVO2, impulsdauer[1]);
    Servo_Set(SERVO3, impulsdauer[2]);
}

void Servo_Off(void)
{
    Servo_Set(SERVO1, 0);
    Servo_Set(SERVO2, 0);
    Servo_Set(SERVO3, 0);
}

#endif

