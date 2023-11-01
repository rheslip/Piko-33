// sequencer related definitions and structures
#define NTRACKS 8   // we have 8 track sequences
#define MAX_SEQ_STEPS 16 // up to 16 step sequencer
#define DEFAULT_SEQ_STEPS 16 // up to 16 step sequencer
#define DEFAULT_VELOCITY_ON 100 // 
#define DEFAULT_DIVIDER 6 // clock divider for 16th notes

#define TEMPO    120 // default tempo
#define PPQN 24  // clocks per quarter note
#define NOTE_DURATION (PPQN/6) // sixteenth note duration
int16_t bpm = TEMPO;
int32_t lastMIDIclock; // timestamp of last MIDI clock
int16_t MIDIclocks=PPQN*2; // midi clock counter
int16_t MIDIsync = 16;  // number of clocks required to sync BPM
int16_t useMIDIclock = 0; // true if we are using MIDI clock
long clocktimer = 0; // clock rate in ms

// table of 24 ppqn clock dividers for 4/4 time 1/32,1/16,1/8,1/4,1/2,1 bar,2 bars,4 bars
int16_t divtable[] = {3,6,12,24,48,96,192,384};

// all of the sequencers use the same data structure even though the data may be different in each case
// this simplifies the code somewhat
// clocks are setup for 24ppqn MIDI clock
// note that there are two threads of execution running on the two Pico cores - UI and note handling
// must be careful about editing items that are used by the 2nd Pico core for note timing etc

struct sequencer {
  uint8_t note[MAX_SEQ_STEPS]; // MIDI note 64= nominal pitch, +- 1 octave range
  uint8_t velocity[MAX_SEQ_STEPS];  // MIDI velocity 0-127. velocity 0 = no note
  uint8_t probability[MAX_SEQ_STEPS];  // probability of playing 127=always play, 0 = don't play
  int16_t index;    // index of step we are on
  int16_t divider;   // clock rate divider - lookup via table
  int16_t clockticks;   //  clock counter
  bool enabled; // true when playing
};

// notes are stored as offsets from the root 
sequencer seq[NTRACKS] = {
  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60, // initial notes - if sample is C3 pitch will be correct
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // initial velocities
  127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, // initial probabilities - 100%
  DEFAULT_SEQ_STEPS-1,   // step index
  DEFAULT_DIVIDER,  // clock divider
  24,       // 24 ppqn clock
  true,   // track enabled


  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60, // initial notes - if sample is C3 pitch will be correct
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // initial velocities
  127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, // initial probabilities - 100%
  DEFAULT_SEQ_STEPS-1,   // step index
  DEFAULT_DIVIDER,  // clock divider
  24,       // 24 ppqn clock
  true,   // track enabled

   60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60, // initial notes - if sample is C3 pitch will be correct
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // initial velocities
  127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, // initial probabilities - 100%
  DEFAULT_SEQ_STEPS-1,   // step index
  DEFAULT_DIVIDER,  // clock divider
  24,       // 24 ppqn clock
  true,   // track enabled

  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60, // initial notes - if sample is C3 pitch will be correct
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // initial velocities
  127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, // initial probabilities - 100%
  DEFAULT_SEQ_STEPS-1,   // step index
  DEFAULT_DIVIDER,  // clock divider
  24,       // 24 ppqn clock
  true,   // track enabled

  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60, // initial notes - if sample is C3 pitch will be correct
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // initial velocities
  127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, // initial probabilities - 100%
  DEFAULT_SEQ_STEPS-1,   // step index
  DEFAULT_DIVIDER,  // clock divider
  24,       // 24 ppqn clock
  true,   // track enabled

  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60, // initial notes - if sample is C3 pitch will be correct
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // initial velocities
  127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, // initial probabilities - 100%
  DEFAULT_SEQ_STEPS-1,   // step index
  DEFAULT_DIVIDER,  // clock divider
  24,       // 24 ppqn clock
  true,   // track enabled

  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60, // initial notes - if sample is C3 pitch will be correct
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // initial velocities
  127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, // initial probabilities - 100%
  DEFAULT_SEQ_STEPS-1,   // step index
  DEFAULT_DIVIDER,  // clock divider
  24,       // 24 ppqn clock
  true,   // track enabled

  60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60, // initial notes - if sample is C3 pitch will be correct
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // initial velocities
  127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, // initial probabilities - 100%
  DEFAULT_SEQ_STEPS-1,   // step index
  DEFAULT_DIVIDER,  // clock divider
  24,       // 24 ppqn clock
  true,   // track enabled
};

// clock all the sequencers
// clockperiod is the period of the 24ppqn clock - used for calculating gate times etc - not used as yet
// it loops thru all tracks looking for events to process
void clocktick (long clockperiod) {

  for (uint8_t track=0; track<NTRACKS;++track) { // 
    --seq[track].clockticks;
    if (seq[track].clockticks <1) { // clock has counted down, do next step
      seq[track].clockticks = seq[track].divider; // reset the clock counter
      ++seq[track].index;
      if ((seq[track].index) >= DEFAULT_SEQ_STEPS) seq[track].index=0; // restart the sequence 
      if (seq[track].enabled && (seq[track].velocity[seq[track].index] > 0)) { // velocity > 0 is a note on
        if (random(0,122) < seq[track].probability[seq[track].index]) { // probability threshold for 100% is a little lower - allows for a bit of slop in the pot
          voice[track].level=seq[track].velocity[seq[track].index]; // set the volume level
      // *** should probably do some sanity checks here
          voice[track].sampleincrement=pitchtable[seq[track].note[seq[track].index]-60+12]; // look up sample increment to get the pitch for the sample
          rp2040.idleOtherCore(); // stop other core because it modifies sampleindex as well
          voice[track].sampleindex=0; // trigger sample for this track
          rp2040.resumeOtherCore();
        }
      }
    }
  }
}

// sync all the sequencers by resetting their clocks
void sync_sequencers(void) {
  for (uint8_t track=0; track<NTRACKS;++track) { // 
    seq[track].clockticks = 1; // set the clock counter so it will roll over 
  }
}

// must be called regularly for sequencer to run
// 
void do_clocks(void) {
  long clockperiod= (long)(((60.0/(float)bpm)/PPQN)*1000);
  //long clockperiod= (long)(((60.0/(float)bpm)/NOTE_DURATION)*1000);
  if ((millis() - clocktimer) > clockperiod) {
    clocktimer=millis(); 
    clocktick(clockperiod);
  }
}

