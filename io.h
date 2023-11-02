
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
// I/O pin definitions for Pikocore - I'm using VCC-GND YD-RP2040 board from AliExpress


#ifndef IO_H_
#define IO_H_

#define CPU_USE 0
#define PWMOUT 20

// analog inputs for voltage control - range approx 0-5v
// 
#define AIN0 	26
#define AIN1 	27
#define AIN2 	28
#define AIN3 	29 // not available on standard Pico board



// button inputs
#define BUTTON0  4 // key1 input on schematic
#define BUTTON1	5
#define BUTTON2	6
#define BUTTON3	7
#define BUTTON4   8
#define BUTTON5	9
#define BUTTON6	10
#define BUTTON7 11  // for the Pico board
#define SHIFTBUTTON 24 // USR button on VCC-GND boards

#define LED0 12 // LED1 output on schematic
#define LED1 13
#define LED2 14
#define LED3 15
#define LED4 16
#define LED5 17
#define LED6 18
#define LED7 19

#define FNLED 25 // indicates 2nd function mode

// MIDI serial port pins - not really MIDI but the serial port is exposed on the first two jacks
#define MIDIRX 1
#define MIDITX 0

#endif // IO_H_

