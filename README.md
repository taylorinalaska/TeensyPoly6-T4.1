Teensy-Poly6

**changes, comments, etc

I am working on adapting to Teensy 4.1. I have 0 experience with Teensy, and bought one specifically for this project. I have very little electronics experience, but followed the original schematic, and made changes as i learned more. I am slowly making progress, ***but this code currently does not work.*** the menu comes up, settings can be configured, but I cannot get audio out of the UDA1334 board.

see here:
https://forum.pjrc.com/threads/58474-2x-PCM1808-ADCs-recording-LR-ch-each-(I2S-quad-input-)-on-T4-and-also-1-DAC-UDA1334?p=222004&viewfull=1#post222004

regarding the UDA1334 pins from T3.? to T4.1,

>*"Note, you would have to change the I2S pins from the Teensy 3.x pinout to the Teensy 4.0 pinout. I.e.

>Instead of pin 9 for BCLK use pin 21/A7;

>Instead of pin 11 for MCLK use pin 23/A9;

>Instead of pin 13 for RX use pin 8;

>Instead of pin 22/A8 for TX use pin 7;

>Instead of pin 23/A9 for LRCLK use pin 20/A6."*

In order to use the pins specified in this post for the UDA1334, I have reassigned pins from the original schematic.
* ENC_BUT is on 24 (this was a mistake from the orignal schematic, it was always on pin 24, just marked wrong)
* Multiplexer 3 (see diagram) moved to A12
* Multiplexer 4 (see diagram) moved to A13
* SDA from 21 to 18
* SCL from 20 to 19
* SAVE from 7 to 11

I tried to document this as best I could on my schematic, but good lord is it ugly.


**Front panel schematic has number indicators for potentiometer locations, see photos folder, "synthlabeled.jpg"**

I haven't confirmed these yet, but they aren't labeled well in the schematic and I used this layout as a reference.


**Below is original text from forked readme
------------------------------------------------------------------

This is a polyphonic synth forked from albnys/TeensyPoly6 converted into a 2U 19" rack.

![Synth](photos/synth.jpg)

I have tidied up the files so it now compiles properly

The analogue interface now uses 6 MUX chips to read the pots and switches so there is no need to connect top the top or underneath of the Teensy 3.6 for the extra analogue inputs required. It runs smoother now as well.

The schematics are upto date now with the latest mux changes.

The synth is built using a teensy 3.6 overclocked to 192Mhz and is programmed in Arduino IDE using the teensy audio library.

Easily upgrade to a T4.1 or even a T4.0 as very few connections are required now. Increase of polyphony is probably available with a T4.x.

How it sounds in the original keyboard form, this sound engine is unchanged.

https://www.youtube.com/watch?v=Exk_K2VwGu0

* Triple VCOs with octave +/- shift.
* Osc A with SAW, PULSE (PWM) and TRIANGLE waves, plus SUB and Crossmod
* Osc B with SAW, PULSE (PWM) and TRIANGLE waves, plus tuning, oct +/-
* Osc C with 28 waves, plus tuning, oct +/-
* LP/BP filter with resonance and env +/- for A/D/R
* LFO with SAW, TRIANGLE and SAMPLE & HOLD waves
* Modulation destinations for FM. TM and AM
* LFO attack, decay and sustain with depth control
* PWM LFO with rate and depth
* Digital reverb with size and mix amount
* Digital delay with time and mix amount
* Programmable ranges for Pitch, Modulation and Aftertouch
* Mono, Poly and Unison (two voices) modes, Unison detune ranges
* Note priorities for Mono and Unison modes, Top, Bottom, Last.
* MIDI modulation, aftertouch, CC messages for controls, channel change, pitchbend.
* 999 memories with storage of all front panel controls except volume.
* MIDI In, Out, Thru
