// Copyright 2023 Rich Heslip
//
// Author: Rich Heslip 
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// 
// sample player inspired by Jan Ostman's ESP8266 drum machine http://janostman.wordpress.com
// completely rewritten for the Motivation Radio Eurorack module
// plays samples in response to gate/trigger inputs and MIDI note on messages
// will play any 22khz sample file converted to a header file in the appropriate format
// Feb 3/19 - initial version
// Feb 11/19 - sped up encoder/trigger ISR so it will catch 1ms pulses from Grids
// Jan 2023 - ported code to Pi Pico so I can use it on a 16mb flash version
// oct 2023 - ported to Pikocore Hardware and converted to a simple rhythm box
// oct 2023 - turned it into a sample sequencer for the Pikocore hardware

#include <Arduino.h> 
#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/sio.h"
//#include "MIDI.h"
#include <PWMAudio.h>
#include "io.h"

//#define MONITOR_CPU  // define to monitor Core 2 CPU usage on pin CPU_USE

#define SAMPLERATE 22050
//#define SAMPLERATE 44100 // VCC-GND 16mb flash boards won't overclock fast enough for 44khz ?

PWMAudio DAC(PWMOUT);  // 16 bit PWM audio

/* no MIDI for now
// MIDI stuff
uint8_t MIDI_Channel=10;  // default MIDI channel for percussion
struct SerialMIDISettings : public midi::DefaultSettings
{
  static const long BaudRate = 31250;
};


// must use HardwareSerial for extra UARTs
HardwareSerial MIDISerial(2);

// instantiate the serial MIDI library
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, MIDISerial, MIDI, SerialMIDISettings);
*/

#define NPOTS 3 // number of pots
uint16_t potvalue[NPOTS]; // pot readings
uint16_t lastpotvalue[NPOTS]; // old pot readings
bool potlock[NPOTS]; // when pots are locked it means they must change by MIN_POT_CHANGE to register
uint32_t pot_timer; // reading pots too often causes noise
#define POT_SAMPLE_TIME 30 // delay time between pot reads
#define MIN_POT_CHANGE 30 // locked pot reading must change by this in order to register
#define MIN_COUNTS 8  // unlocked pot must change by this in order to register
#define POT_AVERAGING 20 // analog sample averaging count 
#define POT_MIN 4   // A/D may not read min value of 0 so use a bit larger value for map() function
#define POT_MAX 1019 // A/D may not read max value of 1023 so use a bit smaller value for map() function

// flag all pot values as locked ie they have to change more than MIN_POT_CHANGE to register
void lockpots(void) {
  for (int i=0; i<NPOTS;++i) potlock[i]=1;
}

// sample analog pot input and do filtering. 
// if pots are locked, change value only if movement is greater than MIN_POT_CHANGE
uint16_t readpot(uint8_t potnum) {
    int val=0;
    int input;
    switch (potnum) { // map potnum to RP2040 pin
      case 0:
        input=AIN0;
        break;
      case 1:
        input=AIN1;
        break;
      case 2:
      default:   // shouldn't happen
        input=AIN2;
        break;     
    }
    // note that Pikocore pots are wired "backwards" - max voltage is full ccw
    for (int j=0; j<POT_AVERAGING;++j) val+=(1024-analogRead(input)); // read the A/D a few times and average for a more stable value
    val=val/POT_AVERAGING;
    if (potlock[potnum]) {
      int delta=lastpotvalue[potnum]-val;  // this needs to be done outside of the abs() function - see arduino abs() docs
      if (abs(delta) > MIN_POT_CHANGE) {
        potlock[potnum]=0;   // flag pot no longer locked
        potvalue[potnum]=lastpotvalue[potnum]=val; // save the new reading
      }
      else val=lastpotvalue[potnum];
    }
    else {
      if (abs(lastpotvalue[potnum]-val) > MIN_COUNTS) lastpotvalue[potnum]=val; // even if pot is unlocked, make sure pot has moved at least MIN_COUNT counts so values don't jump around
      else val=lastpotvalue[potnum];
      potvalue[potnum]=val; // pot is unlocked so save the reading
    }
    return val;
}

int current_track=0; // track we are working on

// we have 8 voices that can play any sample when triggered
// this structure holds the settings for each voice

#define NUM_VOICES 8
struct voice_t {
  int16_t sample;   // index of the sample structure in sampledefs.h
  int16_t level;   // 0-1000 for legacy reasons
  uint32_t sampleindex; // 20:12 fixed point index into the sample array
  uint16_t sampleincrement; // 1:12 fixed point sample step for pitch changes 
} voice[NUM_VOICES] = {
  0,      // default voice 0 assignment - typically a kick but you can map them any way you want
  250,  // initial level
  0,    // sampleindex
  4096, // initial pitch step - normal pitch
  
  1,      // default voice 1 assignment 
  250,
  0,    // sampleindex
  4096, // initial pitch step - normal pitch

  2,    // default voice 2 assignment 
  250, // level
  0,    // sampleindex
  4096, // initial pitch step - normal pitch

  3,    // default voice 3 assignment 
  250, // level
  0,    // sampleindex
  4096, // initial pitch step - normal pitch

  4,    // default voice 4 assignment 
  250,  // level
  0,    // sampleindex
  4096, // initial pitch step - normal pitch

  5,    // default voice 5 assignment 
  250,  // level
  0,    // sampleindex
  4096, // initial pitch step - normal pitch

  6,    // default voice 6 assignment 
  250,  // level
  0,    // sampleindex
  4096, // initial pitch step - normal pitch

  10,    // default voice 7 assignment 
  250,   // level
  0,    // sampleindex
  4096, // initial pitch step - normal pitch 
};  

// two octave chromatic pitch table - maps a number to a 12 bit pitch step per above
uint16_t pitchtable[25]= {
2048, // down one octave
2170,
2299,
2435,
2580,
2734,
2896,
3069,
3251,
3444,
3649,
3866,
4096,  // nominal pitch
4340,
4598,
4871,
5161,
5468,
5793,
6137,
6502,
6889,
7298,
7732,
8192, // up one octave
};

// we can have an arbitrary number of samples but you will run out of memory at some point
// sound sample files are 22khz 16 bit signed PCM format - see the sample include files for examples
// you can change the sample rate to whatever you want but most testing was done at 22khz. 44khz probably works but not much testing was done

// use the wave2header22khz.exe utility to automagically batch convert all the .wav files in a directory into the required header files
// put your 22khz or 44khz PCM wav files in a sample subdirectory with a copy of the utility, run the utility and it will generate all the required header files
// wave2header creates a header file containing the signed PCM values for each sample - note that it may change the name of the file if required to make it "c friendly"
// wave2header also creates sampledefs.h which is an array of structures containing information about each sample file 
// the samples are arranged in alphabetical order to facilitate grouping samples by name - you can manually edit this file to change the order of the samples as needed
// sampledefs.h contains other information not used by this program e.g. the name of the sample file - I wrote it for another project
// wave2header also creates "samples.h" which #includes all the generated header files


//#include "808samples/samples.h" // 808 sounds
//#include "Angular_Jungle_Set/samples.h"   // Jungle soundfont set - great!
//#include "Angular_Techno_Set/samples.h"   // Techno
//#include "Acoustic3/samples.h"   // acoustic drums
//#include "Pico_kit/samples.h"   // assorted samples
//#include "testkit/samples.h"   // small kit for testing
//#include "Trashrez/samples.h"
//#include "world/samples.h"
//#include "testchords/samples.h"
#include "303samples/samples.h"

#define NUM_SAMPLES (sizeof(sample)/sizeof(sample_t)) 

// sample and debounce the keys

#define NUM_BUTTONS 9 // 8 buttons plus USR button on VCC-GND board
#define CLICK_TIME 350 // anything shorter than this is a button click, anything longer than this is button held

#define BUT0 1 // bit masks for each button
#define BUT1 2
#define BUT2 4
#define BUT3 8
#define BUT4 16
#define BUT5 32
#define BUT6 64
#define BUT7 128
#define BUTSHIFT 256
// macro definitions of button states 
#define BUTTONPRESSED(button) (button)  // last state was off, current state is on
#define BUTTONDOWN(button) (button | (button << NUM_BUTTONS)) //last state was on, current state is on
#define BUTTONRELEASED(button) (button << NUM_BUTTONS) //last state was on, current state is off

uint8_t debouncecnt[NUM_BUTTONS] = {0,0,0,0,0,0,0,0,0}; // debounce counters
bool button[NUM_BUTTONS]={0,0,0,0,0,0,0,0,0};  // key active flags
#define SHIFT 8 // index of "shift" USR button 
uint8_t FN2nd=0;  // for toggling between first and second 8 steps of the sequencer and 1st and 2nd set of parameters

 // scan buttons
bool scanbuttons(void)
{
  bool pressed;
  for (int i=0; i< NUM_BUTTONS;++i) {
    switch (i) { // sample gate inputs
      case 0:
        pressed=!digitalRead(BUTTON0); // active low key inputs
        break;
      case 1:
        pressed=!digitalRead(BUTTON1);
        break; 
      case 2:
        pressed=!digitalRead(BUTTON2);
        break;
      case 3:
        pressed=!digitalRead(BUTTON3);
        break;
      case 4:
        pressed=!digitalRead(BUTTON4);
        break;
      case 5:
        pressed=!digitalRead(BUTTON5);
        break;
       case 6:
        pressed=!digitalRead(BUTTON6);
        break;
      case 7:
        pressed=!digitalRead(BUTTON7);
        break;
      case 8:
        pressed=!digitalRead(SHIFTBUTTON);
        break;
    }
    
    if (pressed) { 
      if (debouncecnt[i]<=3) ++debouncecnt[i];
      if (debouncecnt[i]==2) { // trigger on second sample of key active
        button[i]=1;
      }
    }
    else {
      debouncecnt[i]=0;
      button[i]=0;
    }
  }
  if (pressed) return true;
  else return false;
}

// returns a bit vector of the current button states 
// button 0 = bit 0 etc

uint16_t buttonbits(void) {
  uint16_t bits=0;
  for (int16_t i=0;i< NUM_BUTTONS;++i) {
    if (button[i]) bits=bits | (1<<i);
  }
  return bits;
}

/* no MIDI for now
// serial MIDI handler

void HandleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  if (channel==MIDI_Channel) {
      for (int i=0; i< NUM_SAMPLES;++i) {  // 
        if (sample[i].MIDINOTE == note) {
          sample[i].play_volume=velocity;  // use MIDI volume
          sample[i].sampleindex=0;  // if note matches sample MIDI note number, start it playing 
        }
      }
  }
}
*/

// include here to avoid forward references - I'm lazy :)
#include "seq.h"

// edit a step on a track
void edit_step(int track,int step) {
  if(!potlock[0]) seq[track].note[step]=60+(int8_t)(map(potvalue[0],POT_MIN,POT_MAX,-12,+12)); // change note if pot has moved enough - relative to C3
  if(!potlock[1]) seq[track].velocity[step]=(uint8_t)(map(potvalue[1],POT_MIN,POT_MAX,0,127)); // change sample volume level if pot has moved enough 
  if(!potlock[2]) seq[track].probability[step]=(uint8_t)(map(potvalue[2],POT_MIN,POT_MAX,0,127)); // change sample probability if pot has moved enough 
}

// toggle step on a track on/off - velocity=0 for off
void toggle_step(int track,int step) {
  if (seq[track].velocity[step]!=0) seq[track].velocity[step]=0;
  else seq[track].velocity[step]=DEFAULT_VELOCITY_ON;
}

#define DISPLAY_TIME 2000 // time in ms to display numbers on LEDS
#define DISPLAY_TIME_SHORT 500
int32_t display_timer;

// show a number in binary on the LEDs 
void display_value(int16_t value, int32_t time){
  for (int i=LED7; i>=LED0;--i) { // can loop this way because port assignments are sequential
    digitalWrite(i,value&1);
    value=value>>1;
  }
  display_timer=millis()+time;
}

// show steps that are playing on track
void display_active_steps(int track) {
  int16_t volmap=0;
  for (int i=0;i<7;++i){
    if (seq[track].velocity[i + FN2nd*8] >0) volmap |=1;
    volmap<<=1;
  }
  display_value(volmap,DISPLAY_TIME);
}

// main core setup
void setup() {      

 Serial.begin(115200);

// Serial.print("Number of Samples ");
// Serial.println(NUM_SAMPLES);      

#ifdef MONITOR_CPU
  pinMode(CPU_USE, OUTPUT); // for monitoring CPU usage
#endif

  pinMode(BUTTON0, INPUT_PULLUP);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);
  pinMode(BUTTON5, INPUT_PULLUP);
  pinMode(BUTTON6, INPUT_PULLUP);
  pinMode(BUTTON7, INPUT_PULLUP);
  pinMode(SHIFTBUTTON, INPUT_PULLUP);

  pinMode(AIN0, INPUT);
  pinMode(AIN1, INPUT);
  pinMode(AIN2, INPUT);

  pinMode(LED0,OUTPUT);
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);
  pinMode(LED5,OUTPUT);
  pinMode(LED6,OUTPUT);
  pinMode(LED7,OUTPUT);
  pinMode(FNLED,OUTPUT);

// set up Pico PWM audio output
	DAC.setBuffers(4, 128); // DMA buffers 
	DAC.begin(SAMPLERATE);

/* no MIDI for now	
//  Set up serial MIDI port
  MIDISerial.begin(31250, SERIAL_8N1, MIDIRX,MIDITX ); // midi port

  // set up serial MIDI library callbacks
  MIDI.setHandleNoteOn(HandleNoteOn);  // 

  // Initiate serial MIDI communications, listen to all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);
  */
  for (int i=0; i< NUM_VOICES; ++i) { // silence all voices by setting sampleindex to last sample
    voice[i].sampleindex=sample[voice[i].sample].samplesize<<12; // sampleindex is a 20:12 fixed point number
  } 
  display_value(NUM_SAMPLES,DISPLAY_TIME); // show number of samples on the display

}



// main core handles UI
void loop() {
  bool anybuttonpressed,held;

  static uint32_t lastbuttonstate, buttonvector;
  static int32_t buttontimer;

// buttonvector holds the last button state and the current button state
// makes it easy to detect state and change of state for any button or combination of buttons

  buttonvector=(lastbuttonstate << (NUM_BUTTONS)) | buttonbits();
  if (buttonvector !=0) anybuttonpressed=true;
  else buttontimer=millis(); // reset timer if no buttons are pressed

  if (buttonvector==0) buttontimer=millis(); // reset timer if no buttons are pressed

  if ((millis()-buttontimer) > CLICK_TIME) held=true;
  else held=false;

// UI is driven by button clicks and presses (holds) which we handle here
  switch (buttonvector) {
    case (BUTTONPRESSED(BUTSHIFT)): // a button was pressed so start timing it to see if its a click or a hold
    case (BUTTONPRESSED(BUT0)):
    case (BUTTONPRESSED(BUT1)):
    case (BUTTONPRESSED(BUT2)):
    case (BUTTONPRESSED(BUT3)):
    case (BUTTONPRESSED(BUT4)):
    case (BUTTONPRESSED(BUT5)):
    case (BUTTONPRESSED(BUT6)):
    case (BUTTONPRESSED(BUT7)):
      lockpots();
      buttontimer=millis();
      break;
    case (BUTTONDOWN(BUTSHIFT)): // shift button is down
      if (held) {
          // normal functions on pots
          int newbpm=map(potvalue[2],POT_MIN,POT_MAX,50,305); // precalculate possible new BPM
          if(!potlock[2] && (bpm !=newbpm)) {
            bpm=newbpm; // set BPM
            display_value(bpm-50,DISPLAY_TIME); // show BPM Pikocore style
          }
          if(!potlock[0]) { // change sample if pot has moved enough
            int16_t newsample=(int16_t) map(potvalue[0],POT_MIN,POT_MAX,0,NUM_SAMPLES-1); // precompute new sample
            if (voice[current_track].sample != newsample) { 
              rp2040.idleOtherCore(); // don't mess with sample parameters while other core is running
              voice[current_track].sample = newsample;
              rp2040.resumeOtherCore(); // enable sample processing again
            }
          }
          if(!potlock[1]) { // change clock divider if pot has moved enough
            int8_t divindex = (int16_t) map(potvalue[1],POT_MIN,POT_MAX,0,7);
            int16_t newdivider=divtable[divindex]; // precompute new clock divider
            if (seq[current_track].divider != newdivider) { 
              seq[current_track].divider = newdivider; 
              sync_sequencers();  // sync all sequencers to maintain phase relationship
              display_value(1<<(7-divindex),DISPLAY_TIME); // show divider
            }
          }

      }
      break;
    case (BUTTONRELEASED(BUTSHIFT)):  // shift button was released
      if (!held) {  // !held means button was clicked
        if (FN2nd) {
          FN2nd=false; // UI has two modes toggled by the shift button
          digitalWrite(FNLED,0); // show mode
        }
        else {
          FN2nd=true;
          digitalWrite(FNLED,1);
        }
      }
      break;  

    case (0b100000001100000000): // shift + button to select track  
      current_track=0;
      break; 
    case (0b100000010100000000): // shift + button to select track  
      current_track=1;
      break;
    case (0b100000100100000000): // shift + button to select track  
      current_track=2;
      break;
    case (0b100001000100000000): // shift + button to select track  
      current_track=3;
      break;
    case (0b100010000100000000): // shift + button to select track  
      current_track=4;
      break;  
    case (0b100100000100000000): // shift + button to select track  
      current_track=5;
      break;  
    case (0b101000000100000000): // shift + button to select track  
      current_track=6;
      break;  
    case (0b110000000100000000): // shift + button to select track  
      current_track=7;
      break; 
    case (BUTTONDOWN(BUT0)): // button held, edit step
      if (held) {
        edit_step(current_track,0+FN2nd*8);
      }
      break; 
    case (BUTTONRELEASED(BUT0)):  // button was released
      if (!held) toggle_step(current_track,0+FN2nd*8);  // !held means button was clicked   
      break; 
    case (BUTTONDOWN(BUT1)): // button held, edit step
      if (held) {
        edit_step(current_track,1+FN2nd*8);
      }
      break;
    case (BUTTONRELEASED(BUT1)):  // button was released
      if (!held) toggle_step(current_track,1+FN2nd*8);  // !held means button was clicked   
      break;  
    case (BUTTONDOWN(BUT2)): // button held, edit step
      if (held) {
        edit_step(current_track,2+FN2nd*8);
      }
      break; 
    case (BUTTONRELEASED(BUT2)):  // button was released
      if (!held) toggle_step(current_track,2+FN2nd*8);  // !held means button was clicked   
      break; 
    case (BUTTONDOWN(BUT3)): // button held, edit step
      if (held) {
        edit_step(current_track,3+FN2nd*8);
      }
      break;
    case (BUTTONRELEASED(BUT3)):  // button was released
      if (!held) toggle_step(current_track,3+FN2nd*8);  // !held means button was clicked   
      break;  
    case (BUTTONDOWN(BUT4)): // button held, edit step
      if (held) {
        edit_step(current_track,4+FN2nd*8);
      }
      break;
    case (BUTTONRELEASED(BUT4)):  // button was released
      if (!held) toggle_step(current_track,4+FN2nd*8);  // !held means button was clicked   
      break;  
    case (BUTTONDOWN(BUT5)): // button held, edit step
      if (held) {
        edit_step(current_track,5+FN2nd*8);
      }
      break;
    case (BUTTONRELEASED(BUT5)):  // button was released
      if (!held) toggle_step(current_track,5+FN2nd*8);  // !held means button was clicked   
      break;   
    case (BUTTONDOWN(BUT6)): // button held, edit step
      if (held) {
        edit_step(current_track,6+FN2nd*8);
      }
      break; 
    case (BUTTONRELEASED(BUT6)):  // button was released
      if (!held) toggle_step(current_track,6+FN2nd*8);  // !held means button was clicked   
      break; 
    case (BUTTONDOWN(BUT7)): // button held, edit step
      if (held) {
        edit_step(current_track,7+FN2nd*8);
      }
      break;
    case (BUTTONRELEASED(BUT7)):  // button was released
      if (!held) toggle_step(current_track,7+FN2nd*8);  // !held means button was clicked   
      break;    
  }
  lastbuttonstate=buttonvector & 0x1ff; // save last state


// MIDI.read();  // do serial MIDI

  do_clocks();  // process sequencer clocks
  scanbuttons();

// reading A/D seems to cause noise in the audio so don't do it too often
  if ((millis()-pot_timer) > POT_SAMPLE_TIME) {
    readpot(0); readpot(1); readpot(2); // sample pots.
    pot_timer=millis();
  }

// default display show steps for current track and sequencer step
  if ((millis() > display_timer)) {
    bool state;
    int8_t ledindex= seq[current_track].index - FN2nd*8; // show first 8 or 2nd 8 steps based on FN2nd state
    int8_t step=0;
    for (int i=LED0;i<=LED7;++i) { // LED port numbers are sequential on the Pikocore
      if (seq[current_track].velocity[step+FN2nd*8] !=0) state=1;
      else state=0;
      if (step == ledindex) state=!state; // show steps - invert led on triggered steps
      digitalWrite(i,state);
      ++step;
    }
  }

}

// second core setup
// second core dedicated to sample processing
void setup1() {
  delay (1000); // wait for main core to start up perhipherals
}

// second core calculates samples and sends to DAC
void loop1(){
  int32_t newsample,samplesum=0;
  uint32_t index;
  int16_t samp0,samp1,delta,tracksample;

 // oct 22 2023 resampling code
// to change pitch we step through the sample by .5 rate for half pitch up to 2 for double pitch
// sample.sampleindex is a fixed point 20:12 fractional number
// we step through the sample array by sampleincrement - sampleincrement is treated as a 1 bit integer and a 12 bit fraction
// for sample lookup sample.sampleindex is converted to a 20 bit integer which limits the max sample size to 2**20 or about 1 million samples, about 45 seconds @22khz
  // oct 24/2023 - scan through voices instead of sample array
  // faster because there are only 8 voices vs typically 45 or more samples

  for (int track=0; track< NTRACKS;++track) {  // look for samples that are playing, scale their volume, and add them up
    tracksample=voice[track].sample; // precompute for a little more speed below
    index=voice[track].sampleindex>>12; // get the integer part of the sample increment
    if (index <= sample[tracksample].samplesize) { // if sample is still playing, do interpolation   
      samp0=sample[tracksample].samplearray[index]; // get the first sample to interpolate
      samp1=sample[tracksample].samplearray[index+1];// get the second sample
      delta=samp1-samp0;
      newsample=(int32_t)samp0+((int32_t)delta*((int32_t)voice[track].sampleindex & 0x0fff))/4096; // interpolate between the two samples
      //samplesum+=((int32_t)samp0+(int32_t)delta*(sample[i].sampleindex & 0x0fff)/4096)*sample[i].play_volume;
      // samplesum+=(newsample*(127*voice[track].level))/1000;
      samplesum+=(newsample*voice[track].level); // changed to MIDI velocity levels 0-127
      voice[track].sampleindex+=voice[track].sampleincrement; // add step increment
    }
  }

  samplesum=samplesum>>7;  // adjust for volume multiply above
  if  (samplesum>32767) samplesum=32767; // clip if sample sum is too large
  if  (samplesum<-32767) samplesum=-32767;



#ifdef MONITOR_CPU  
  digitalWrite(CPU_USE,0); // low - CPU not busy
#endif
 // write samples to DMA buffer - this is a blocking call so it stalls when buffer is full
	DAC.write(int16_t(samplesum)); // left

#ifdef MONITOR_CPU
  digitalWrite(CPU_USE,1); // hi = CPU busy
#endif
}


