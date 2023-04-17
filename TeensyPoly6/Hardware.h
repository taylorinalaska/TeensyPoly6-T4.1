//#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#include <Bounce.h>
#include <ADC.h>
#include <ADC_util.h>
ADC *adc = new ADC();

#define MONO_POLY1 17
#define MONO_POLY2 38
#define A_SHAPE_1 31
#define A_SHAPE_2 15
#define B_SHAPE_1 4
#define B_SHAPE_2 5
#define MAIN_OCT_1 6
#define MAIN_OCT_2 7
#define B_OCTAVE_1 8
#define C_OCTAVE_1 32
#define C_OCTAVE_2 39
#define B_OCTAVE_2 16
#define LFOA_SHAPE_1 24
#define LFOA_SHAPE_2 25
#define LFOA_DEST_1 26
#define LFOA_DEST_2 27
#define FILTER_MODE 37

#define MUX1 28
#define MUX2 29
#define MUX3 30

#define DEBOUNCE 30
#define RECALL_SW 18
#define SAVE_SW 19
#define SETTINGS_SW 21
#define BACK_SW 20

#define ENCODER_PINA 36
#define ENCODER_PINB 35
static long encPrevious = 0;

#define MUXCHANNELS 8
#define QUANTISE_FACTOR 7
static byte muxInput = 0;

static int mux1ValuesPrev[MUXCHANNELS] = {};
static int mux2ValuesPrev[MUXCHANNELS] = {};
static int mux3ValuesPrev[MUXCHANNELS] = {};
static int mux4ValuesPrev[MUXCHANNELS] = {};

static int mux1Read = 0;
static int mux2Read = 0;
static int mux3Read = 0;
static int mux4Read = 0;

Bounce recallButton = Bounce(RECALL_SW, DEBOUNCE);  //On encoder
boolean recall = true;                              //Hack for recall button
Bounce saveButton = Bounce(SAVE_SW, DEBOUNCE);
boolean del = true;  //Hack for save button
Bounce settingsButton = Bounce(SETTINGS_SW, DEBOUNCE);
boolean reini = true;  //Hack for settings button
Bounce backButton = Bounce(BACK_SW, DEBOUNCE);
boolean panic = true;                         //Hack for back button
Encoder encoder(ENCODER_PINB, ENCODER_PINA);  //This often needs the pins swapping depending on the encoder

int mux0;
int mux1;
int mux2;
int mux3;
int mux4;
int mux5;
int mux6;
int mux7;
int mux8;
int mux9;
int mux10;
int mux11;
int mux12;
int mux13;
int mux14;
int mux15;
int mux16;
int mux17;
int mux18;
int mux19;
int mux20;
int mux21;
int mux22;
int mux23;
int mux24;
int mux25;
int mux26;
int mux27;
int mux28;
int mux29;
int mux30;
int mux31;

void setupHardware() {
  //Switches
  pinMode(MONO_POLY1, INPUT_PULLDOWN);   //poly
  pinMode(MONO_POLY2, INPUT_PULLDOWN);   //poly
  pinMode(A_SHAPE_1, INPUT_PULLDOWN);    //shape A
  pinMode(A_SHAPE_2, INPUT_PULLDOWN);    //shape A
  pinMode(B_SHAPE_1, INPUT_PULLDOWN);    //shabe B
  pinMode(B_SHAPE_2, INPUT_PULLDOWN);    //shape B
  pinMode(MAIN_OCT_1, INPUT_PULLDOWN);   //main oct
  pinMode(MAIN_OCT_2, INPUT_PULLDOWN);   //main oct
  pinMode(B_OCTAVE_1, INPUT_PULLDOWN);   //oct B
  pinMode(B_OCTAVE_2, INPUT_PULLDOWN);   //oct C
  pinMode(C_OCTAVE_1, INPUT_PULLDOWN);   //oct C
  pinMode(C_OCTAVE_2, INPUT_PULLDOWN);   //oct B
  pinMode(FILTER_MODE, INPUT_PULLDOWN);  //filt Mode

  pinMode(LFOA_DEST_1, INPUT_PULLDOWN);   //lfo dest
  pinMode(LFOA_DEST_2, INPUT_PULLDOWN);   //lfo dest
  pinMode(LFOA_SHAPE_1, INPUT_PULLDOWN);  //lfo shape
  pinMode(LFOA_SHAPE_2, INPUT_PULLDOWN);  //lfo shape

  pinMode(RECALL_SW, INPUT_PULLUP);  //On encoder
  pinMode(SAVE_SW, INPUT_PULLUP);
  pinMode(SETTINGS_SW, INPUT_PULLUP);
  pinMode(BACK_SW, INPUT_PULLUP);

  //Mux setup
  pinMode(MUX1, OUTPUT);
  pinMode(MUX2, OUTPUT);
  pinMode(MUX3, OUTPUT);

  digitalWrite(MUX1, 0);
  digitalWrite(MUX2, 0);
  digitalWrite(MUX3, 0);

  adc->adc0->setAveraging(16);                                          // set number of averages 0, 4, 8, 16 or 32.
  adc->adc0->setResolution(10);                                         // set bits of resolution  8, 10, 12 or 16 bits.
  adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED);  // change the conversion speed
  adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED);           // change the sampling speed

  //MUXs on ADC1
  adc->adc1->setAveraging(16);                                          // set number of averages 0, 4, 8, 16 or 32.
  adc->adc1->setResolution(10);                                         // set bits of resolution  8, 10, 12 or 16 bits.
  adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED);  // change the conversion speed
  adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED);           // change the sampling speed
}