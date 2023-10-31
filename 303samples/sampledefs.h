// sample structure built by wav2header based on wav2sketch by Paul Stoffregen

struct sample_t {
  const int16_t * samplearray; // pointer to sample array
  uint32_t samplesize; // size of the sample array
  uint32_t sampleindex; // current sample array index when playing. index at last sample= not playing
  uint8_t MIDINOTE;  // MIDI note on that plays this sample
  uint8_t play_volume; // play volume 0-127
  char sname[20];        // sample name
} sample[] = {

	Am_bastata_120a,	// pointer to sample array
	Am_bastata_120a_SIZE,	// size of the sample array
	Am_bastata_120a_SIZE,	//sampleindex. if at end of sample array sound is not playing
	35,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Am_bastata_120a",	// sample name

	Am_bastata_120f,	// pointer to sample array
	Am_bastata_120f_SIZE,	// size of the sample array
	Am_bastata_120f_SIZE,	//sampleindex. if at end of sample array sound is not playing
	36,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Am_bastata_120f",	// sample name

	Am_bastatb_120a,	// pointer to sample array
	Am_bastatb_120a_SIZE,	// size of the sample array
	Am_bastatb_120a_SIZE,	//sampleindex. if at end of sample array sound is not playing
	37,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Am_bastatb_120a",	// sample name

	Am_bastatb_120c,	// pointer to sample array
	Am_bastatb_120c_SIZE,	// size of the sample array
	Am_bastatb_120c_SIZE,	//sampleindex. if at end of sample array sound is not playing
	38,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Am_bastatb_120c",	// sample name

	Arp120,	// pointer to sample array
	Arp120_SIZE,	// size of the sample array
	Arp120_SIZE,	//sampleindex. if at end of sample array sound is not playing
	39,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Arp120",	// sample name

	Ws05,	// pointer to sample array
	Ws05_SIZE,	// size of the sample array
	Ws05_SIZE,	//sampleindex. if at end of sample array sound is not playing
	40,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws05",	// sample name

	Ws06,	// pointer to sample array
	Ws06_SIZE,	// size of the sample array
	Ws06_SIZE,	//sampleindex. if at end of sample array sound is not playing
	41,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws06",	// sample name

	Ws09,	// pointer to sample array
	Ws09_SIZE,	// size of the sample array
	Ws09_SIZE,	//sampleindex. if at end of sample array sound is not playing
	42,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws09",	// sample name

	Ws10,	// pointer to sample array
	Ws10_SIZE,	// size of the sample array
	Ws10_SIZE,	//sampleindex. if at end of sample array sound is not playing
	43,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws10",	// sample name

	Ws13,	// pointer to sample array
	Ws13_SIZE,	// size of the sample array
	Ws13_SIZE,	//sampleindex. if at end of sample array sound is not playing
	44,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws13",	// sample name

	Ws14,	// pointer to sample array
	Ws14_SIZE,	// size of the sample array
	Ws14_SIZE,	//sampleindex. if at end of sample array sound is not playing
	45,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws14",	// sample name

	Ws16,	// pointer to sample array
	Ws16_SIZE,	// size of the sample array
	Ws16_SIZE,	//sampleindex. if at end of sample array sound is not playing
	46,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws16",	// sample name

	Ws17,	// pointer to sample array
	Ws17_SIZE,	// size of the sample array
	Ws17_SIZE,	//sampleindex. if at end of sample array sound is not playing
	47,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws17",	// sample name

	Ws18,	// pointer to sample array
	Ws18_SIZE,	// size of the sample array
	Ws18_SIZE,	//sampleindex. if at end of sample array sound is not playing
	48,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws18",	// sample name

	Ws19,	// pointer to sample array
	Ws19_SIZE,	// size of the sample array
	Ws19_SIZE,	//sampleindex. if at end of sample array sound is not playing
	49,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws19",	// sample name

	Ws20,	// pointer to sample array
	Ws20_SIZE,	// size of the sample array
	Ws20_SIZE,	//sampleindex. if at end of sample array sound is not playing
	50,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws20",	// sample name

	Ws21,	// pointer to sample array
	Ws21_SIZE,	// size of the sample array
	Ws21_SIZE,	//sampleindex. if at end of sample array sound is not playing
	51,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws21",	// sample name

	Ws22,	// pointer to sample array
	Ws22_SIZE,	// size of the sample array
	Ws22_SIZE,	//sampleindex. if at end of sample array sound is not playing
	52,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws22",	// sample name

	Ws23,	// pointer to sample array
	Ws23_SIZE,	// size of the sample array
	Ws23_SIZE,	//sampleindex. if at end of sample array sound is not playing
	53,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws23",	// sample name

	Ws24,	// pointer to sample array
	Ws24_SIZE,	// size of the sample array
	Ws24_SIZE,	//sampleindex. if at end of sample array sound is not playing
	54,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws24",	// sample name

	Ws25,	// pointer to sample array
	Ws25_SIZE,	// size of the sample array
	Ws25_SIZE,	//sampleindex. if at end of sample array sound is not playing
	55,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws25",	// sample name

	Ws30,	// pointer to sample array
	Ws30_SIZE,	// size of the sample array
	Ws30_SIZE,	//sampleindex. if at end of sample array sound is not playing
	56,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws30",	// sample name

	Ws31,	// pointer to sample array
	Ws31_SIZE,	// size of the sample array
	Ws31_SIZE,	//sampleindex. if at end of sample array sound is not playing
	57,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws31",	// sample name

	Ws32,	// pointer to sample array
	Ws32_SIZE,	// size of the sample array
	Ws32_SIZE,	//sampleindex. if at end of sample array sound is not playing
	58,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws32",	// sample name

	Ws34,	// pointer to sample array
	Ws34_SIZE,	// size of the sample array
	Ws34_SIZE,	//sampleindex. if at end of sample array sound is not playing
	59,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws34",	// sample name

	Ws36,	// pointer to sample array
	Ws36_SIZE,	// size of the sample array
	Ws36_SIZE,	//sampleindex. if at end of sample array sound is not playing
	60,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws36",	// sample name

	Ws37,	// pointer to sample array
	Ws37_SIZE,	// size of the sample array
	Ws37_SIZE,	//sampleindex. if at end of sample array sound is not playing
	61,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws37",	// sample name

	Ws38,	// pointer to sample array
	Ws38_SIZE,	// size of the sample array
	Ws38_SIZE,	//sampleindex. if at end of sample array sound is not playing
	62,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws38",	// sample name

	Ws39,	// pointer to sample array
	Ws39_SIZE,	// size of the sample array
	Ws39_SIZE,	//sampleindex. if at end of sample array sound is not playing
	63,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws39",	// sample name

	Ws42,	// pointer to sample array
	Ws42_SIZE,	// size of the sample array
	Ws42_SIZE,	//sampleindex. if at end of sample array sound is not playing
	64,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws42",	// sample name

	Ws44,	// pointer to sample array
	Ws44_SIZE,	// size of the sample array
	Ws44_SIZE,	//sampleindex. if at end of sample array sound is not playing
	65,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws44",	// sample name

	Ws45,	// pointer to sample array
	Ws45_SIZE,	// size of the sample array
	Ws45_SIZE,	//sampleindex. if at end of sample array sound is not playing
	66,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws45",	// sample name

	Ws51,	// pointer to sample array
	Ws51_SIZE,	// size of the sample array
	Ws51_SIZE,	//sampleindex. if at end of sample array sound is not playing
	67,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws51",	// sample name

	Ws56,	// pointer to sample array
	Ws56_SIZE,	// size of the sample array
	Ws56_SIZE,	//sampleindex. if at end of sample array sound is not playing
	68,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws56",	// sample name

	Ws57,	// pointer to sample array
	Ws57_SIZE,	// size of the sample array
	Ws57_SIZE,	//sampleindex. if at end of sample array sound is not playing
	69,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws57",	// sample name

	Ws59,	// pointer to sample array
	Ws59_SIZE,	// size of the sample array
	Ws59_SIZE,	//sampleindex. if at end of sample array sound is not playing
	70,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws59",	// sample name

	Ws60,	// pointer to sample array
	Ws60_SIZE,	// size of the sample array
	Ws60_SIZE,	//sampleindex. if at end of sample array sound is not playing
	71,	// MIDI note on that plays this sample
	127,	// play volume 0-127
	"Ws60",	// sample name

};
