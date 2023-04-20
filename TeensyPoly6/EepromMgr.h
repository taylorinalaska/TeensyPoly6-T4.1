#include <EEPROM.h>

#define EEPROM_MIDI_CH 0
#define EEPROM_PITCHBEND 1
#define EEPROM_MODWHEEL_DEPTH 2
#define EEPROM_ENCODER_DIR 3
#define EEPROM_PICKUP_ENABLE 4
#define EEPROM_AFTERTOUCH_DEPTH 5
#define EEPROM_SCOPE_ENABLE 6
#define EEPROM_MIDI_OUT_CH 7
#define EEPROM_VU_ENABLE 8
#define EEPROM_MIDI_THRU 9
#define EEPROM_AMP_ENV 10
#define EEPROM_FILT_ENV 11
#define EEPROM_GLIDE_SHAPE 12


int getMIDIChannel() {
  byte midiChannel = EEPROM.read(EEPROM_MIDI_CH);
  if (midiChannel < 0 || midiChannel > 16) midiChannel = MIDI_CHANNEL_OMNI;  //If EEPROM has no MIDI channel stored
  return midiChannel;
}

void storeMidiChannel(byte channel) {
  EEPROM.update(EEPROM_MIDI_CH, channel);
}

boolean getEncoderDir() {
  byte ed = EEPROM.read(EEPROM_ENCODER_DIR);
  if (ed < 0 || ed > 1) return true;  //If EEPROM has no encoder direction stored
  return ed == 1 ? true : false;
}

void storeEncoderDir(byte encoderDir) {
  EEPROM.update(EEPROM_ENCODER_DIR, encoderDir);
}

int getPitchBendRange() {
  byte pitchbend = EEPROM.read(EEPROM_PITCHBEND);
  if (pitchbend < 1 || pitchbend > 12) return pitchBendRange;  //If EEPROM has no pitchbend stored
  return pitchbend;
}

void storePitchBendRange(byte pitchbend) {
  EEPROM.update(EEPROM_PITCHBEND, pitchbend);
}

float getModWheelDepth() {
  byte mw = EEPROM.read(EEPROM_MODWHEEL_DEPTH);
  if (mw < 1 || mw > 10) return modWheelDepth;  //If EEPROM has no mod wheel depth stored
  return mw;
}

void storeModWheelDepth(byte mw) {
  EEPROM.update(EEPROM_MODWHEEL_DEPTH, mw);
}

float getafterTouchDepth() {
  byte atdepth = EEPROM.read(EEPROM_AFTERTOUCH_DEPTH);
  if (atdepth < 1 || atdepth > 10) return afterTouchDepth;  //If EEPROM has no mod wheel depth stored
  return atdepth;
}

void storeafterTouchDepth(byte atdepth) {
  EEPROM.update(EEPROM_AFTERTOUCH_DEPTH, atdepth);
}
