#include <Arduino.h>
#include "motor_control.h"

MotorControl::MotorControl(int pin_dir, int pin_step) :
    pin_dir(pin_dir), pin_step(pin_step),
    last_tick_micros(0), tick_period_micros(0)
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

void MotorControl::TickOn(unsigned long period_micros) {
    tick_period_micros = period_micros;
    // Force the next tick to happen
    last_tick_micros = 0;
}

void MotorControl::TickOff() {
    tick_period_micros = 0;
}

void MotorControl::Tick(unsigned long cur_micros) {
    if (cur_micros == 0) {
        return;
    }

    if (tick_period_micros == 0) {
        return;
    }

    if (last_tick_micros == 0 || (cur_micros - last_tick_micros) >= tick_period_micros) {
        DoTick();
        last_tick_micros = cur_micros;
    }
}

void MotorControl::DoTick() {
    // Pulse the step pin once
    digitalWrite(pin_step, HIGH);
    digitalWrite(pin_step, LOW);
}