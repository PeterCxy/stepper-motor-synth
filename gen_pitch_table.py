#!/usr/bin/env python3

start_pitch = 36 # C2
end_pitch = 84 # C6

pitch_A0 = 21
pitch_C1 = 24
pitch_A4 = 69
freq_A4 = 440

def pitch_to_freq(pitch):
    dist_A4 = pitch - pitch_A4
    factor = pow(2, abs(dist_A4) / 12)
    
    if dist_A4 > 0:
        return freq_A4 * factor
    else:
        return freq_A4 / factor

def pitch_to_period_micros(pitch):
    return 1 / pitch_to_freq(pitch) * 1000 * 1000

pitch_names = ["A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"]
def pitch_to_name(pitch):
    return pitch_names[(pitch - pitch_A0) % 12] + str(int((pitch - pitch_C1) / 12) + 1)

print("#pragma once")
print("")
print("constexpr unsigned int midi_pitch_offset = " + str(start_pitch) + ";")
print("constexpr unsigned int midi_pitch_max = " + str(end_pitch) + ";")
print("constexpr unsigned long midi_pitch_period[" + str(end_pitch - start_pitch) + "] = {")

for i in range(start_pitch, end_pitch):
    period = int(pitch_to_period_micros(i))
    name = pitch_to_name(i)
    if name == "D3" or name == "D#3" or name == "E3":
        period = int(period / 2)
    print("    " + str(period) + "ul,\t// " + name)

print("};")