#include <Arduino.h>
#include <midi_Defs.h>
#include <eRCaGuy_Timer2_Counter.h>
#include "motor_control.h"
#include "./pitch_table.h"

TickController::TickController() :
    midi_bend(8192), period_half_micros(0), period_with_bend(0) {

}

void TickController::SetPeriod(unsigned int half_micros) {
    period_half_micros = half_micros;
    CalculatePeriod();
}

void TickController::SetBend(unsigned int bend) {
    if (bend >= 16384) {
        return;
    }

    midi_bend = bend;
    CalculatePeriod();
}

void TickController::CalculatePeriod() {
    if (period_half_micros == 0) {
        period_with_bend = 0;
        return;
    }
    // Scale the MIDI bend value down to 0 - 2047
    unsigned int _bend = midi_bend / 8;
    period_with_bend = (unsigned int) (((float) period_half_micros) * pgm_read_float_near(midi_pitch_bend_scale + _bend));
}

unsigned int TickController::GetTruePeriod() {
    return period_with_bend;
}

MotorControl::MotorControl(int pin_dir, int pin_step) :
    pin_dir(pin_dir), pin_step(pin_step),
    last_tick_half_micros(0)
{
    // No actual constructor logic -- initialization is in Init()
}

void MotorControl::Init() {
    // Initialize the pin state
    pinMode(pin_dir, OUTPUT);
    pinMode(pin_step, OUTPUT);
    digitalWrite(pin_dir, LOW);
    digitalWrite(pin_step, LOW);
}

void MotorControl::TickOn(unsigned long period_half_micros) {
    tick_ctrl.SetPeriod(period_half_micros);
    // Force a tick
    last_tick_half_micros = 0;
}

void MotorControl::TickAtPitch(unsigned int midi_pitch) {
    // Bounds check
    if (midi_pitch < midi_pitch_offset || midi_pitch >= midi_pitch_max) {
        return;
    }
    
    TickOn(midi_pitch_period[midi_pitch - midi_pitch_offset]);
}

void MotorControl::TickPitchBend(int bend) {
    // Note: the Arduino MIDI library re-positions bend to [-8192, 8192) instead of the default [0, 16384)
    // we need to correct for this here
    bend = bend - MIDI_PITCHBEND_MIN;
    
    if (bend < 0 || bend >= 16384) return;
    
    tick_ctrl.SetBend(bend);
}

void MotorControl::TickOff() {
    tick_ctrl.SetPeriod(0);
}

void MotorControl::Tick(unsigned long cur_half_micros) {
    unsigned long period = tick_ctrl.GetTruePeriod();

    if (period == 0) {
        return;
    }

    if (last_tick_half_micros == 0 || cur_half_micros >= last_tick_half_micros + period) {
        DoTick();
        last_tick_half_micros = cur_half_micros;
    }
}

void MotorControl::DoTick() {
    // Pulse the step pin once
    digitalWrite(pin_step, HIGH);
    digitalWrite(pin_step, LOW);
}