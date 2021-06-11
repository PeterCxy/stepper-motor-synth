#include <Arduino.h>
#include <MIDI.h>
#include <eRCaGuy_Timer2_Counter.h>
#include "motor_control.h"

#define NUM_MOTORS 4

// Minimal interval between reading two chunks of MIDI data
// This improves frequency accuracy by throttling non-synthesizing actions
#define MIDI_CMD_INTERVAL_MIN 1000 // 1ms

using namespace midi;

MotorControl motors[NUM_MOTORS] = {
    MotorControl(5, 2),
    MotorControl(6, 3),
    MotorControl(7, 4),
    MotorControl(13, 12)
};

// MIDI setup
struct MySerialMIDISettings {
    // ttymidi wants a baud rate of 115200
    static const long BaudRate = 115200;
};

typedef SerialMIDI<HardwareSerial, MySerialMIDISettings> MySerialMIDI;

MySerialMIDI serialMIDI(Serial);

MidiInterface<MySerialMIDI> MIDI(serialMIDI);

void handle_tick(unsigned long cur_micros) {
    // Randomize the order we process motor ticks every time
    // This helps reduce the minor frequency discrepancies between each motor
    unsigned long start_val = (cur_micros / 10) % NUM_MOTORS;
    for (unsigned long i = start_val; i < start_val + NUM_MOTORS; i++) {
        motors[i % NUM_MOTORS].Tick(cur_micros);
    }
}

#define ASSERT_CHANNEL() \
    if (channel <= 0) return; \
    if (channel - 1 > NUM_MOTORS) return;

void midi_note_on(uint8_t channel, uint8_t note, uint8_t velocity) {
    ASSERT_CHANNEL();
    motors[channel - 1].TickAtPitch(note);
}

void midi_note_off(uint8_t channel, uint8_t note, uint8_t velocity) {
    ASSERT_CHANNEL();
    motors[channel - 1].TickOff();
}

int main() {
    // Arduino library initialization
    // Needed for some functions to work (like micros)
    init();

    // Timer based on Timer 2 (this breaks PWM output)
    timer2.setup();

    // Enable the motor drivers
    pinMode(8, OUTPUT);
    digitalWrite(8, LOW);

    // Initialize the controllers
    for (int i = 0; i < NUM_MOTORS; i++) {
        motors[i].Init();
    }

    // Start serial MIDI interface
    MIDI.begin(MIDI_CHANNEL_OMNI);

    // Set up MIDI callbacks
    MIDI.setHandleNoteOn(midi_note_on);
    MIDI.setHandleNoteOff(midi_note_off);

    unsigned long cur_micros;
    unsigned long cur_half_micros;
    unsigned long last_read_micros = 0;

    while (true) {
        cur_half_micros = timer2.get_count(); // The unit of get_count is 0.5us
        cur_micros = cur_half_micros / 2ul;
        handle_tick(cur_half_micros);

        if (cur_micros - last_read_micros >= MIDI_CMD_INTERVAL_MIN) {
            last_read_micros = cur_micros;

            // Read all available MIDI data
            // Note that the host is expected to send MIDI data only when
            // it is supposed to be played; therefore we don't need
            // to worry about the host sending data prematurely
            while (Serial.available()) {
                MIDI.read();
            }
        }
    }

    return 0;
}