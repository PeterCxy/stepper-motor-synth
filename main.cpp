#include <Arduino.h>
#include "motor_control.h"

#define NUM_MOTORS 4

MotorControl motors[NUM_MOTORS] = {
    MotorControl(5, 2),
    MotorControl(6, 3),
    MotorControl(7, 4),
    MotorControl(13, 12)
};

void handle_tick(unsigned long cur_micros) {
    // Randomize the order we process motor ticks every time
    // This helps reduce the minor frequency discrepancies between each motor
    unsigned long start_val = (cur_micros / 10) % NUM_MOTORS;
    for (unsigned long i = start_val; i < start_val + NUM_MOTORS; i++) {
        motors[i % NUM_MOTORS].Tick(cur_micros);
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

    // Test: play a C chord
    motors[0].TickAtPitch(60); // C4
    motors[1].TickAtPitch(64); // E4
    motors[2].TickAtPitch(67); // G4

    while (true) {
        unsigned long cur_micros = micros();

        handle_tick(cur_micros);
    }

    return 0;
}