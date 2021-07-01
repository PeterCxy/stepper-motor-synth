#!/usr/bin/env python3

start_pitch = 36 # C2
end_pitch = 84 # C6

pitch_A0 = 21
pitch_C1 = 24
pitch_A4 = 69
freq_A4 = 440

pitch_D5 = 74
pitch_F5 = 77

def pitch_to_freq(pitch):
    dist_A4 = pitch - pitch_A4
    factor = pow(2, abs(dist_A4) / 12)
    
    if dist_A4 > 0:
        # Notes after A4 are about 20 cents flat
        factor = factor * pow(2, 20 / 12 / 100)

        if pitch >= pitch_D5:
            # After D5 it's about another 10 cents flat
            factor = factor * pow(2, 10 / 12 / 100)

        if pitch >= pitch_F5:
            # After F5 it's about another 10 cents flat
            factor = factor * pow(2, 10 / 12 / 100)

        return freq_A4 * factor
    else:
        return freq_A4 / factor

def pitch_to_period_micros(pitch):
    return 1 / pitch_to_freq(pitch) * 1000 * 1000

pitch_names = ["A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"]
def pitch_to_name(pitch):
    return pitch_names[(pitch - pitch_A0) % 12] + str(int((pitch - pitch_C1) / 12) + 1)

print("#pragma once")
print("#include <avr/pgmspace.h>")
print("")
print("constexpr unsigned int midi_pitch_offset = " + str(start_pitch) + ";")
print("constexpr unsigned int midi_pitch_max = " + str(end_pitch) + ";")
print("// period is in units of 0.5 us, instead of 1 us")
print("constexpr PROGMEM unsigned long midi_pitch_period[" + str(end_pitch - start_pitch) + "] = {")

for i in range(start_pitch, end_pitch):
    period = int(2.0 * pitch_to_period_micros(i))
    name = pitch_to_name(i)
    print("    " + str(period) + "ul,\t// " + name)

print("};")

print("")
print("// scale factors for bend values from 0 to 2047")
print("// We don't support the full 16385 levels of MIDI bend")
print("// MIDI bend values have to be converted first to the nearest supported one")
print("constexpr PROGMEM float midi_pitch_bend_scale[2048] = {")

for i in range(0, 2048):
    factor = pow(2, (8192 - i * 8) / 49152)
    print("    " + str(factor) + "f,\t// " + str(i))

print("};")