#!/usr/bin/python

import math
import struct
import sys
import ossaudiodev

dsp = ossaudiodev.open('r')

SAMPLE_RATE = 44100 # Hz
# AFMT = Audio ForMaT ?
# S16 = 16 bits per sample
# LE = (L)ittle (E)ndian
dsp.setparameters(ossaudiodev.AFMT_S16_LE,
                  1, # 1 channel, mono
                  SAMPLE_RATE)

to_be_written = ""  # store the captured audio to write as a file

try:
    while True:
        CHUNK_SIZE = 1000  # number of samples per iteration
        byte_string = dsp.read(2 * CHUNK_SIZE)  # 2 = sizeof(short)
        to_be_written += byte_string

        # Calculate the volume in dB
        max_v = 1
        for s in struct.unpack("<%dh" % CHUNK_SIZE, byte_string):
            max_v = max(max_v, abs(s))
        volume = 6.0 * math.log(max_v / 32768.0, 2.0)
        # ... and print it out
        sys.stdout.write("%6.1fdB  \r" % volume)
        sys.stdout.flush()
except KeyboardInterrupt:
    # Stop on Ctrl+C
    pass

dsp.close()

import wave

out = wave.open('output.wav', 'w')
out.setnchannels(1)
out.setsampwidth(2)
out.setframerate(SAMPLE_RATE)
out.writeframes(to_be_written)
out.close()
