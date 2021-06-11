#pragma once

class MotorControl {
    private:
        // Control pins of the stepper motor
        int pin_dir, pin_step;
        // Timestamp of the last motor tick
        unsigned long last_tick_half_micros;
        // Interval of motor ticking; 0 to disable the motor
        unsigned long tick_period_half_micros;

        void DoTick();
    public:
        // Constructor
        MotorControl(int pin_dir, int pin_step);
        // Must be called before using any other functionality (but after enabling the controllers)
        void Init();
        // Set the motor to tick at a given interval (inverse of frequency)
        void TickOn(unsigned long period_half_micros);
        // Set the motor to tick with a given MIDI pitch
        void TickAtPitch(unsigned int midi_pitch);
        // Turn off the motor
        void TickOff();
        // Perform a tick if necessary
        // This should be called from the main event loop
        void Tick(unsigned long cur_half_micros);
};