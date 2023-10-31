// sample structure built by wav2header based on wav2sketch by Paul Stoffregen

struct sample_t {
  const int16_t * samplearray; // pointer to sample array
  uint32_t samplesize; // size of the sample array
  uint32_t sampleindex; // current sample array index when playing. index at last sample= not playing
  uint8_t MIDINOTE;  // MIDI note on that plays this sample
  uint8_t play_volume; // play volume 0-127
  char sname[20];        // sample name
} sample[] = {

	Cl,	// pointer to sample array
	Cl_SIZE,	// size of the sample array
	Cl_SIZE,	//sampleindex. if at end of sample array sound is not playing
	35,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Cl",	// sample name

	Clap01,	// pointer to sample array
	Clap01_SIZE,	// size of the sample array
	Clap01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	36,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Clap01",	// sample name

	Clhat01,	// pointer to sample array
	Clhat01_SIZE,	// size of the sample array
	Clhat01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	37,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Clhat01",	// sample name

	Kick01,	// pointer to sample array
	Kick01_SIZE,	// size of the sample array
	Kick01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	38,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Kick01",	// sample name

	Oh50,	// pointer to sample array
	Oh50_SIZE,	// size of the sample array
	Oh50_SIZE,	//sampleindex. if at end of sample array sound is not playing
	39,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Oh50",	// sample name

	Pdhat01,	// pointer to sample array
	Pdhat01_SIZE,	// size of the sample array
	Pdhat01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	40,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Pdhat01",	// sample name

	Pdhat02,	// pointer to sample array
	Pdhat02_SIZE,	// size of the sample array
	Pdhat02_SIZE,	//sampleindex. if at end of sample array sound is not playing
	41,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Pdhat02",	// sample name

	Rim03,	// pointer to sample array
	Rim03_SIZE,	// size of the sample array
	Rim03_SIZE,	//sampleindex. if at end of sample array sound is not playing
	42,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Rim03",	// sample name

	Sd1025,	// pointer to sample array
	Sd1025_SIZE,	// size of the sample array
	Sd1025_SIZE,	//sampleindex. if at end of sample array sound is not playing
	43,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sd1025",	// sample name

	Sd5010,	// pointer to sample array
	Sd5010_SIZE,	// size of the sample array
	Sd5010_SIZE,	//sampleindex. if at end of sample array sound is not playing
	44,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Sd5010",	// sample name

	Snroff08,	// pointer to sample array
	Snroff08_SIZE,	// size of the sample array
	Snroff08_SIZE,	//sampleindex. if at end of sample array sound is not playing
	45,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Snroff08",	// sample name

	Tom01,	// pointer to sample array
	Tom01_SIZE,	// size of the sample array
	Tom01_SIZE,	//sampleindex. if at end of sample array sound is not playing
	46,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Tom01",	// sample name

};
