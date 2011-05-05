#!/usr/bin/python

import math
import struct
import ossaudiodev

dsp = ossaudiodev.open('w')

SAMPLE_RATE = 44100 # Hz
# AFMT = Audio ForMaT ?
# S16 = 16 bits per sample
# LE = (L)ittle (E)ndian
dsp.setparameters(ossaudiodev.AFMT_S16_LE,
                  1, # 1 channel, mono
                  SAMPLE_RATE)

FREQUENCY = 440.0 # Hz
sin_wave_encoded = ""
phase = 0.0
for s in range(SAMPLE_RATE * 2):
    value = 32767 * math.sin(2*math.pi*FREQUENCY * s / SAMPLE_RATE + phase)
    sin_wave_encoded += struct.pack("<h", int(value))
dsp.write(sin_wave_encoded)
dsp.close()
