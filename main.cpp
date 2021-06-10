#include <Arduino.h>
#include "motor_control.h"

MotorControl motors[4] = {
    MotorControl(5, 2),
    MotorControl(6, 3),
    MotorControl(7, 4),
    MotorControl(13, 12)
};

int main() {
    init();
    // Enable the motor drivers
    pinMode(8, OUTPUT);
    digitalWrite(8, LOW);

    // Initialize the controllers
    for (int i = 0; i < 4; i++) {
        motors[i].Init();
    }

    // Test: play A4 (440 Hz) on each successive motor every second
    int cur_motor = -1;

    while (true) {
        unsigned long cur_micros = micros();

        for (int i = 0; i < 4; i++) {
            motors[i].Tick(cur_micros);
        }

        int new_motor = (cur_micros / 1000 / 1000) % 4;
        
        if (new_motor != cur_motor) {
            motors[new_motor].TickOn(2272);

            if (cur_motor != -1) {
                motors[cur_motor].TickOff();
            }

            cur_motor = new_motor;
        }
    }

    return 0;
}