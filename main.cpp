#include <Arduino.h>
#include "motor_control.h"

MotorControl motors[4] = {
    MotorControl(5, 2),
    MotorControl(6, 3),
    MotorControl(7, 4),
    MotorControl(13, 12)
};

void handle_tick(unsigned long cur_micros) {
    // Randomize the order we process motor ticks every time
    // This helps reduce the minor frequency discrepancies between each motor
    unsigned long start_val = (cur_micros / 10) % 4;
    for (unsigned long i = start_val; i < start_val + 4; i++) {
        motors[i % 4].Tick(cur_micros);
    }
}

int main() {
    // Arduino library initialization
    // Needed for some functions to work (like micros)
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

        handle_tick(cur_micros);

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