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

    // Test: play Twinkle Twinkle Little Star
    unsigned long note_time = 500ul * 1000ul; // 500 ms
    unsigned int seq_motor[4][15] = {
        { 60, 60, 67, 67, 69, 69, 67, 0, 65, 65, 64, 64, 62, 62, 60 },
        { 57, 57, 64, 64, 65, 65, 64, 0, 62, 62, 60, 60, 59, 59, 57},
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        { 48, 48, 53, 53, 48, 48, 0, 0, 53, 53, 48, 48, 55, 55, 48},
    };
    unsigned long last_note_ts = 0;
    int cur_note = -1;

    while (true) {
        unsigned long cur_micros = micros();
        handle_tick(cur_micros);

        if (cur_micros - last_note_ts >= note_time) {
            last_note_ts = cur_micros;
            cur_note += 1;

            for (int i = 0; i < 4; i++) {
                if (seq_motor[i][cur_note] != 0) {
                    motors[i].TickAtPitch(seq_motor[i][cur_note]);
                } else {
                    motors[i].TickOff();
                }

                // Prevent out of bounds access
                if (cur_note >= 15) {
                    motors[i].TickOff();
                    while (true) {}
                }
            }
        }
    }

    return 0;
}