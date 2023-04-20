#include "SettingsService.h"

void settingsMIDICh(int index, const char *value);
void settingsEncoderDir(char *value);
void settingsPitchBend(int index, const char *value);
void settingsModWheelDepth(int index, const char *value);
void settingsafterTouchDepth(int index, const char *value);

int currentIndexMIDICh();
int currentIndexEncoderDir();
int currentIndexPitchBend();
int currentIndexModWheelDepth();
int currentIndexafterTouchDepth();

void settingsMIDICh(int index, const char *value) {
  if (strcmp(value, "ALL") == 0) {
    midiChannel = MIDI_CHANNEL_OMNI;
  } else {
    midiChannel = atoi(value);
  }
  storeMidiChannel(midiChannel);
}

void settingsEncoderDir(int index, const char *value) {
  if (strcmp(value, "Type 1") == 0) {
    encCW = true;
  } else {
    encCW = false;
  }
  storeEncoderDir(encCW ? 1 : 0);
}

void settingsPitchBend(int index, const char *value) {
  pitchBendRange = atoi(value);
  storePitchBendRange(pitchBendRange);
}

void settingsModWheelDepth(int index, const char *value) {
  modWheelDepth = atoi(value);
  storeModWheelDepth(modWheelDepth);
}

void settingsafterTouchDepth(int index, const char *value) {
  afterTouchDepth = atoi(value);
  storeafterTouchDepth(afterTouchDepth);
}

int currentIndexMIDICh() {
  return getMIDIChannel();
}

int currentIndexEncoderDir() {
  return getEncoderDir() ? 0 : 1;
}

int currentIndexPitchBend() {
  return getPitchBendRange() - 1;
}

int currentIndexModWheelDepth() {
  return getModWheelDepth() - 1;
}

int currentIndexafterTouchDepth() {
  return getafterTouchDepth() - 1;
}

// add settings to the circular buffer
void setUpSettings() {
  settings::append(settings::SettingsOption{ "MIDI In Ch.", { "All", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "\0" }, settingsMIDICh, currentIndexMIDICh });
  settings::append(settings::SettingsOption{ "Encoder", { "Type 1", "Type 2", "\0" }, settingsEncoderDir, currentIndexEncoderDir });
  settings::append(settings::SettingsOption{ "Pitch Bend", { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "\0" }, settingsPitchBend, currentIndexPitchBend });
  settings::append(settings::SettingsOption{ "MW Depth", { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "\0" }, settingsModWheelDepth, currentIndexModWheelDepth });
  settings::append(settings::SettingsOption{ "AT Depth", { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "\0" }, settingsafterTouchDepth, currentIndexafterTouchDepth });
}
