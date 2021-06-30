#pragma once

class TickController {
    private:
        // 0 to disable
        unsigned int period_half_micros;
        // 0 - 16384, 8192 = no bend
        unsigned int midi_bend;
        unsigned int period_with_bend;
        
        void CalculatePeriod();

    public:
        TickController();
        void SetPeriod(unsigned int half_micros);
        void SetBend(unsigned int bend);
        // In units of half microseconds
        // 0 = disabled
        unsigned long NextTick();
        unsigned long NextTickWithTs(unsigned long cur_half_micros);
};

class MotorControl {
    private:
        // Control pins of the stepper motor
        int pin_dir, pin_step;
        unsigned long next_tick_half_micros;
        TickController tick_ctrl;

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
        // Set the pitch bend
        void TickPitchBend(int bend);
        // Turn off the motor
        void TickOff();
        // Perform a tick if necessary
        // This should be called from the main event loop
        void Tick(unsigned long cur_half_micros);
};