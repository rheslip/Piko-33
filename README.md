# Piko-33
 Piko-33 Sample Sequencer
 
 Sample Sequencer for the Pikocore

Demo and tutorial:
 
Alternate firmware for the Pikocore designed by Zack Scholl https://infinitedigits.co/wares/pikocore/
Please visit Zack's site to buy a Pikocore, a kit, or a PCB - its a really fun little sample glitcher.

This firmware is based on a couple of other projects - Pico Drums and Pico Sequencer. Its an 8 track sample player with a 16 step sequencer per track.

This firmware requires an RP2040 board compatible with the one on the Pikocore which has larger 16Mbyte flash and a USR button. The smaller images provided may run on a Raspberry Pi Pico board but you need the USR button to do anything useful.

Controls:

To select a track - while holding the USR button, press one of the 8 buttons to select the corresponding track. The LEDs will indicate the step the sequencer is on. If sequencer steps are active the LED will be on and blink off as that step is played.

To toggle between the first 8 steps and the second 8 steps of a track, tap the USR button for approximately 1/2 second. The second 8 steps of the sequencer are selected when the blue LED near the USB connector of the RP2040 board is on.

To turn a sequencer step on or off, tap the corresponding button for approximately 1/2 second. The LED for the step is on when the step is enabled.

To edit a sequencer step, press and hold the button for that step. While holding the button use:


leftmost pot to change the pitch of the sample. Pitch range is +- one octave in chromatic steps.


2nd from left pot to change the velocity (volume) for that step. Turn the pot clockwise to increase the velocity. If the step is enabled and you can't hear it be sure the velocity is turned up.


3rd from left pot to change the probability for that step. Pot fully clockwise is 100% (plays every time), fully counterclockwise is 0% (step will never play). If the step is enabled and you can't hear it be sure the probability is turned up.


While holding the USR button, use the left pot to change the sample for the currently selected track.


While holding the USR button, use the 2nd from left left pot to change the clock rate for the currently selected track. Clock rates are indicated on the LEDs:

 top left LED on - 1/32 notes - sequencer repeats every .5 bar
 
 2nd from top left LED on - 1/16 notes - default setting on power up. 16 steps of 1/16 notes = 1 bar.
 
 3rd from top left LED on - 1/8 notes. 2 bars for 16 steps
 
 2nd from top left LED on - 1/4 notes. 4 bars for 16 steps
 
 bottom left LED on - 1/2 notes. 8 bars for 16 steps
 
 2nd from bottom left LED on - whole notes or 1 bar per step
 
 3rd from bottom left LED on - 2 bars per step
 
 2nd from bottom left LED on - 4 bars per step
 
So, each track is always 16 steps but the clock rate determines how long each step lasts. The sample length (and pitch) determines how long the sound for each step actually plays. If a new active step happens before the entire sample is played the sample will be retriggered from the beginning.

Tip: long step durations are useful for chord sequencing. Load some sustained chords samples and trigger them every bar, 2 or 4 bars. Adjust the step pitch for different chords.

While holding the shift button, use the 3rd from left pot to set the BPM. BPM readout on the LEDs is the same as on the Pikocore.

Right pot is master volume - same as Pikocore.

Button combinations:

Hold the top left button and press the top right button to mute/unmute the current track


Written with Arduino 2.1 with Pico Arduino RP2040 board support package installed. See https://arduino-pico.readthedocs.io/en/latest/

See the tutorial video on Pikobeats (my other firmware for the Pikocore) and notes in the code for instructions on how to change the sample set. A couple of sample UF2 binaries are provided in the build directory which can be installed as per the Pikocore instructions.

First released version Nov 1 2023



