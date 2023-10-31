// Convert a set of WAV audio files to C data arrays for the Teensy3 Audio Library
// Copyright 2014, Paul Stoffregen (paul@pjrc.com)
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

// compile with:  gcc -O2 -Wall -S -o wav2header wav2header.c
//  win32 cygwin:       i686-w64-mingw32-gcc -s -O2 -Wall wav2header.c -o wav2header.exe
// windows 10 cygwin64: x86_64-w64-mingw32-gcc -s -O2 -Wall wav2header.c -o wav2header.exe

// R Heslip Feb 6/2019
// modded to generate header files from 22khz 16bit PCM wav files 
// header files are formatted specifically for my Motivation Radio drum machine/sample player sketch
// March 28 2023 - modded compile instructions above for windows 10 which only supports 64 bits
// changed to 44khz output by default

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

uint8_t ulaw_encode(int16_t audio);
void print_byte(FILE *out, uint8_t b);
void filename2samplename(void);
uint32_t padding(uint32_t length, uint32_t block);
uint8_t read_uint8(FILE *in);
int16_t read_int16(FILE *in);
uint32_t read_uint32(FILE *in);
void die(const char *format, ...) __attribute__ ((format (printf, 1, 2)));

// WAV file format:
// http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html

const char *filename="";
char samplename[64];
unsigned int bcount, wcount;
unsigned int total_length=0;
int pcm_mode=1;

void wav2c(FILE *in, FILE *out, FILE *outh)
{
	uint32_t header[4];
	int16_t format, channels, bits,bytespersample;
	uint32_t rate;
	uint32_t i, length, padlength=0, arraylen;
	uint32_t chunkSize;
	int32_t audio=0;
	uint32_t skip=1; // 1 is no downsampling

	// read the WAV file's header
	for (i=0; i < 4; i++) {
		header[i] = read_uint32(in);
	}
	while (header[3] != 0x20746D66) {
		// skip past unknown sections until "fmt "
		chunkSize = read_uint32(in);
		for (i=0; i < chunkSize; i++) {
			read_uint8(in);
		}
		header[3] = read_uint32(in);
	}
	chunkSize = read_uint32(in);

	// read the audio format parameters
	format = read_int16(in);
	channels = read_int16(in);
	rate = read_uint32(in);
	read_uint32(in); // ignore byterate
	read_int16(in);  // ignore blockalign
	bits = read_int16(in);
	bytespersample = bits/8;
	printf("format: %d, channels: %d, rate: %d, bits %d\n", format, channels, rate, bits);
	if (format != 1)
		die("file %s is compressed, only uncompressed supported", filename);
//	if (rate != 22050)
//		die("sample rate %d in %s is unsupported\n"
//		  "sample rate must be 22050", rate, filename);
	if (channels != 1 && channels != 2)
		die("file %s has %d channels, but only 1 & 2 are supported", filename, channels);
	if (!((bits == 16) || (bits == 24)))
		die("file %s has %d bit format, but only 16 is supported", filename, bits);
/*	if (rate == 44100) {
		printf("Resampling 44khz file to 22khz\n");
		skip=2; // write every 2nd sample
	}
*/

	// skip past any extra data on the WAVE header (hopefully it doesn't matter?)
	for (chunkSize -= 16; chunkSize > 0; chunkSize--) {
		read_uint8(in);
	}

	// read the data header, skip non-audio data
	while (1) {
		header[0] = read_uint32(in);
		length = read_uint32(in);
		if (header[0] == 0x61746164) break; // beginning of actual audio data
		// skip over non-audio data
		for (i=0; i < length; i++) {
			read_uint8(in);
		}
	}

//	printf("data size in bytes %d\n",length);
	
	// the length must be a multiple of the data size
	if (channels == 2) {
		if (length % (bytespersample*2)) die("file %s data length is not a multiple of %d", filename, bytespersample);
		length = length / (bytespersample*2);
	}
	if (channels == 1) {
		if (length % (bits/8)) die("file %s data length is not a multiple of %d", filename, bytespersample);
		length = length / bytespersample;
	}

//	printf("data size in samples %d\n",length);
	
	if (length > 0xFFFFFF) die("file %s data length is too long", filename);
	bcount = 0;
/*
	// AudioPlayMemory requires padding to 2.9 ms boundary (128 samples @ 44100)
	if (rate == 44100) {
		padlength = padding(length, 128);
		format = 1;
	} else if (rate == 22050) {
		padlength = padding(length, 64);
		format = 2;
	} else if (rate == 11025) {
		padlength = padding(length, 32);
		format = 3;
	}
*/
	
	if (pcm_mode) {
		arraylen = ((length + padlength) * 2 + 3) / bytespersample + 1;
		arraylen = length/skip; // RH skip=2 for downsampling from 44 to 22khz
		format |= 0x80;
	} else {
		arraylen = (length + padlength + 3) / bytespersample + 1;
	}
	total_length += arraylen;

	// output a minimal header, just the length, #bits and sample rate
	//fprintf(outh, "extern const unsigned int AudioSample%s[%d];\n", samplename, arraylen);
	fprintf(out, "// Converted from %s, using %d Hz, %s encoding\n", filename, rate,
	  (pcm_mode ? "PCM" : "u-law"));
	fprintf(out, "#define %s_SIZE %d\n\n", samplename, arraylen);	  
	//fprintf(out, "const uint16_t %s[%d] = {\n", samplename, arraylen);
	fprintf(out, "const int16_t %s[] = {\n", samplename);
	//fprintf(out, "0x%08X,", length | (format << 24));
	wcount = 0;

	// finally, read the audio data
	while (length > 0) {
		if (channels == 1) {
			if (bits == 24) read_uint8(in);  // throw away low byte
			audio = read_int16(in);
			
		} else {
			if (bits == 24) read_uint8(in);  
			audio = read_int16(in);
			if (bits == 24) read_uint8(in);  
			audio += read_int16(in);
			audio /= 2;
		}
		if (pcm_mode) {
			if ((length % skip)==0) {// downsampling if skip >1
				print_byte(out, audio);
				print_byte(out, audio >> 8);
			}
		} else {
			print_byte(out, ulaw_encode(audio));
		}
//		printf("length = %d\n",length);
		length--;
	}
	while (padlength > 0) {
		print_byte(out, 0);
		padlength--;
	}
	while (bcount > 0) {
		print_byte(out, 0);
	}
	if (wcount > 0) fprintf(out, "\n");
	fprintf(out, "};\n");
}


uint8_t ulaw_encode(int16_t audio)
{
	uint32_t mag, neg;

	// http://en.wikipedia.org/wiki/G.711
	if (audio >= 0) {
		mag = audio;
		neg = 0;
	} else {
		mag = audio * -1;
		neg = 0x80;
	}
	mag += 128;
	if (mag > 0x7FFF) mag = 0x7FFF;
	if (mag >= 0x4000) return neg | 0x70 | ((mag >> 10) & 0x0F);  // 01wx yz00 0000 0000
	if (mag >= 0x2000) return neg | 0x60 | ((mag >> 9) & 0x0F);   // 001w xyz0 0000 0000
	if (mag >= 0x1000) return neg | 0x50 | ((mag >> 8) & 0x0F);   // 0001 wxyz 0000 0000
	if (mag >= 0x0800) return neg | 0x40 | ((mag >> 7) & 0x0F);   // 0000 1wxy z000 0000
	if (mag >= 0x0400) return neg | 0x30 | ((mag >> 6) & 0x0F);   // 0000 01wx yz00 0000
	if (mag >= 0x0200) return neg | 0x20 | ((mag >> 5) & 0x0F);   // 0000 001w xyz0 0000
	if (mag >= 0x0100) return neg | 0x10 | ((mag >> 4) & 0x0F);   // 0000 0001 wxyz 0000
	return neg | 0x00 | ((mag >> 3) & 0x0F);   // 0000 0000 1wxy z000
}



// compute the extra padding needed
uint32_t padding(uint32_t length, uint32_t block)
{
	uint32_t extra;

	extra = length % block;
	if (extra == 0) return 0;
	return block - extra;
}

// pack the output bytes into 32 bit words, lsb first, and
// format the data nicely with commas and newlines
void print_byte(FILE *out, uint8_t b)
{
	static uint32_t buf32=0;

	buf32 |= (b << (8 * bcount++));
	if (bcount >= 2) {
		fprintf(out, "%6d,", (int16_t)buf32);
		buf32 = 0;
		bcount = 0;
		if (++wcount >= 8) {
			fprintf(out, "\n");
			wcount = 0;
		}
	}
}

// convert the WAV filename into a C-compatible name
void filename2samplename(void)
{
	unsigned int len, i, n;
	unsigned char c;

	len = strlen(filename) - 4;
	if (len >= sizeof(samplename)-1) len = sizeof(samplename)-1; // in case of huge filename
	for (i=0, n=0; n < len; i++) {
		c = filename[i];
		if (c=='.') break;  // rh I added this to get rid of the "wav" 
		if (isalpha(c) || c == '_' || (isdigit(c) && n > 0)) {  // removes special chars
			samplename[n] = (n == 0) ? toupper(c) : tolower(c); // capitalizes first letter
			n++;
		}
	}
	samplename[n] = 0;
}


const char *title = "// Audio data converted from WAV file by wav2header based on wav2sketch by Paul Stoffregen\n\n";

int main(int argc, char **argv)
{
	DIR *dir;
	struct dirent *f;
	struct stat s;
	FILE *fp, *outc=NULL, *outh=NULL,*outinc=NULL;
	char buf[128];
	int midinote=35, len;


	dir = opendir(".");
	if (!dir) die("unable to open directory");
	
	outh = fopen("sampledefs.h", "w");  // sample structure definitions
	if (outh == NULL) die("unable to write %s\n", buf);
	fprintf(outh, "// sample structure built by wav2header based on wav2sketch by Paul Stoffregen\n\n");

	outinc = fopen("samples.h", "w");   // include file for all the samples
	if (outh == NULL) die("unable to write %s\n", buf);
	
	fprintf(outh, "struct sample_t {\n");
	fprintf(outh, "  const int16_t * samplearray; // pointer to sample array\n");
	fprintf(outh, "  uint32_t samplesize; // size of the sample array\n");	
	fprintf(outh, "  uint32_t sampleindex; // current sample array index when playing. index at last sample= not playing\n");	
	fprintf(outh, "  uint8_t MIDINOTE;  // MIDI note on that plays this sample\n");	
	fprintf(outh, "  uint8_t play_volume; // play volume 0-127\n");	
	fprintf(outh, "  char sname[20];        // sample name\n");	
	fprintf(outh, "} sample[] = {\n\n");	

	
	while (1) {
		f = readdir(dir);
		if (!f) break;
		//if ((f->d_type & DT_DIR)) continue; // skip directories
		//if (!(f->d_type & DT_REG)) continue; // skip special files
		if (stat(f->d_name, &s) < 0) continue; // skip if unable to stat
		if (S_ISDIR(s.st_mode)) continue;  // skip directories
		if (!S_ISREG(s.st_mode)) continue; // skip special files
		filename = f->d_name;
		len = strlen(filename);
		if (len < 5) continue;
		if (strcasecmp(filename + len - 4, ".wav") != 0) continue;
		fp = fopen(filename, "rb");
		if (!fp) die("unable to read file %s", filename);
		filename2samplename();
		printf("converting: %s  -->  %s\n", filename, samplename);
		snprintf(buf, sizeof(buf), "%s.h", samplename);
		outc = fopen(buf, "w");
		if (outc == NULL) die("unable to write %s", buf);
		snprintf(buf, sizeof(buf), "%s.h", samplename);
		//outh = fopen(buf, "w");
		//if (outh == NULL) die("unable to write %s\n", buf);
		//fprintf(outh, "%s", title);
		fprintf(outc, "%s", title);
		//fprintf(outc, "#include \"%s\"\n\n", buf);
		wav2c(fp, outc, outh);
		
		fprintf(outinc, "#include \"%s.h\"\n", samplename); // build the sample include file
		
		// build the sampledef structure initializers
		fprintf(outh, "\t%s,\t// pointer to sample array\n", samplename); //
		fprintf(outh, "\t%s_SIZE,\t// size of the sample array\n", samplename); // 
		fprintf(outh, "\t%s_SIZE,\t//sampleindex. if at end of sample array sound is not playing\n", samplename); // 
		fprintf(outh, "\t%d,\t// MIDI note on that plays this sample\n", midinote++); // 
		fprintf(outh, "\t%d,\t// play volume 0-127\n", 127); // 
		fprintf(outh, "\t\"%s\",\t// sample name\n\n", samplename); // 
		
		//wav2c(fp, stdout, stdout);
		fclose(outc);
		//fclose(outh);
		fclose(fp);
	}
	fprintf(outh, "};\n"); // end sampledefs.h initializer section
	fclose(outh);
	fprintf(outinc, "#include \"sampledefs.h\"\n"); // add sampledefs.h to the include filelist
	fclose(outinc);
	printf("Total data size %d bytes\n", total_length * 2);
	return 0;
}

uint8_t read_uint8(FILE *in)
{
	int c1;

	c1 = fgetc(in);
	if (c1 == EOF) die("byte read error, end of data while reading from %s\n", filename);
	c1 &= 255;
	return c1;
}

int16_t read_int16(FILE *in)
{
	int c1, c2;

	c1 = fgetc(in);
	if (c1 == EOF) die("int16 read error, end of data while reading from %s\n", filename);
	c2 = fgetc(in);
	if (c2 == EOF) die("int16 read error, end of data while reading from %s\n", filename);
	c1 &= 255;
	c2 &= 255;
	return (c2 << 8) | c1;
}


uint32_t read_uint32(FILE *in)
{
	int c1, c2, c3, c4;

	c1 = fgetc(in);
	if (c1 == EOF) die("int32 read error, end of data while reading from %s\n", filename);
	c2 = fgetc(in);
	if (c2 == EOF) die("int32 read error, end of data while reading from %s\n", filename);
	c3 = fgetc(in);
	if (c3 == EOF) die("int32 read error, end of data while reading from %s\n", filename);
	c4 = fgetc(in);
	if (c4 == EOF) die("int32 read error, end of data while reading from %s\n", filename);
	c1 &= 255;
	c2 &= 255;
	c3 &= 255;
	c4 &= 255;
	return (c4 << 24) | (c3 << 16) | (c2 << 8) | c1;
}

void die(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	fprintf(stderr, "wav2sketch: ");
	vfprintf(stderr, format, args);
	fprintf(stderr, "\n");
	exit(1);
}

