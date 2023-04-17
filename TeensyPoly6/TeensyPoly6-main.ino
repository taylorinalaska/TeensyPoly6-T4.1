/*
  poly6 programmer- Firmware Rev 1.2

  Includes code by:
    Dave Benn - Handling MUXs, a few other bits and original inspiration  https://www.notesandvolts.com/2019/01/teensy-synth-part-10-hardware.html

  Arduino IDE
  Tools Settings:
  Board: "Teensy3.5"
  USB Type: "Serial + MIDI + Audio"
  CPU Speed: "180"
  Optimize: "Fastest"

  Additional libraries:
    Agileware CircularBuffer available in Arduino libraries manager
    Replacement files are in the Modified Libraries folder and need to be placed in the teensy Audio folder.
*/
#include <Audio.h>
#include <SD.h>
#include <Wire.h>
#include <SerialFlash.h>
#include <MIDI.h>

#include "a_globals.h"
#include "Hardware.h"
#include "f_mux.h"
#include "g_params.h"
#include "PatchMgr.h"
#include "EepromMgr.h"
#include "Settings.h"
#include "i_noteOn.h"

#define PARAMETER 0      //The main page for displaying the current patch and control (parameter) changes
#define RECALL 1         //Patches list
#define SAVE 2           //Save patch page
#define REINITIALISE 3   // Reinitialise message
#define PATCH 4          // Show current patch bypassing PARAMETER
#define PATCHNAMING 5    // Patch naming page
#define DELETE 6         //Delete patch page
#define DELETEMSG 7      //Delete patch message page
#define SETTINGS 8       //Settings page
#define SETTINGSVALUE 9  //Settings page

unsigned int state = PARAMETER;

#include "ST7735Display.h"

boolean cardStatus = false;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);


void setup() {
  AudioMemory(470);
  SPI.begin();
  setupDisplay();


  //Read MIDI Channel from EEPROM
  midiChannel = getMIDIChannel();
  Serial.println("MIDI Ch:" + String(midiChannel) + " (0 is Omni On)");

  //Midi setup
  // usbMIDI.begin();
  // usbMIDI.setHandleNoteOn(myNoteOn);
  // usbMIDI.setHandleNoteOff(myNoteOff);
  // usbMIDI.setHandlePitchChange(myPitchBend);
  // usbMIDI.setHandleProgramChange(myProgramChange);

  MIDI.setHandleNoteOn(myNoteOn);
  MIDI.setHandleNoteOff(myNoteOff);
  MIDI.setHandlePitchBend(myPitchBend);
  MIDI.setHandleProgramChange(myProgramChange);
  MIDI.setHandleControlChange(myControlChange);
  MIDI.setHandleAfterTouchChannel(myAfterTouch);
  MIDI.begin(midiChannel);

  setupHardware();

  cardStatus = SD.begin(BUILTIN_SDCARD);
  if (cardStatus) {
    Serial.println("SD card is connected");
    //Get patch numbers and names from SD card
    loadPatches();
    if (patches.size() == 0) {
      //save an initialised patch to SD card
      savePatch("1", INITPATCH);
      loadPatches();
    }
  } else {
    Serial.println("SD card is not connected or unusable");
    reinitialiseToPanel();
    showPatchPage("No SD", "conn'd / usable");
  }

  //Read Encoder Direction from EEPROM
  encCW = getEncoderDir();
  recallPatch(patchNo);  //Load first patch
  //reinitialiseToPanel();
  updateScreen();

  //vco setup
  vcoA1.begin(vcoVol, 150, WAVEFORM_SAWTOOTH);
  vcoB1.begin(vcoVol, 150, WAVEFORM_SQUARE);
  vcoC1.begin(vcoVol * 1.5, 150, WAVEFORM_ARBITRARY);
  sub1.begin(vcoVol * 1.5, 150, WAVEFORM_TRIANGLE);

  vcoA2.begin(vcoVol, 150, WAVEFORM_SAWTOOTH);
  vcoB2.begin(vcoVol, 150, WAVEFORM_SQUARE);
  vcoC2.begin(vcoVol * 1.5, 150, WAVEFORM_ARBITRARY);
  sub2.begin(vcoVol * 1.5, 150, WAVEFORM_TRIANGLE);

  vcoA3.begin(vcoVol, 150, WAVEFORM_SAWTOOTH);
  vcoB3.begin(vcoVol, 150, WAVEFORM_SQUARE);
  vcoC3.begin(vcoVol * 1.5, 150, WAVEFORM_ARBITRARY);
  sub3.begin(vcoVol * 1.5, 150, WAVEFORM_TRIANGLE);

  vcoA4.begin(vcoVol, 150, WAVEFORM_SAWTOOTH);
  vcoB4.begin(vcoVol, 150, WAVEFORM_SQUARE);
  vcoC4.begin(vcoVol * 1.5, 150, WAVEFORM_ARBITRARY);
  sub4.begin(vcoVol * 1.5, 150, WAVEFORM_TRIANGLE);

  vcoA5.begin(vcoVol, 150, WAVEFORM_SAWTOOTH);
  vcoB5.begin(vcoVol, 150, WAVEFORM_SQUARE);
  vcoC5.begin(vcoVol * 1.5, 150, WAVEFORM_ARBITRARY);
  sub5.begin(vcoVol * 1.5, 150, WAVEFORM_TRIANGLE);

  vcoA6.begin(vcoVol, 150, WAVEFORM_SAWTOOTH);
  vcoB6.begin(vcoVol, 150, WAVEFORM_SQUARE);
  vcoC6.begin(vcoVol * 1.5, 150, WAVEFORM_ARBITRARY);
  sub6.begin(vcoVol * 1.5, 150, WAVEFORM_TRIANGLE);


  //filter
  filter1.octaveControl(7);
  filterEnv1.sustain(0);

  filter2.octaveControl(7);
  filterEnv2.sustain(0);

  filter3.octaveControl(7);
  filterEnv3.sustain(0);

  filter4.octaveControl(7);
  filterEnv4.sustain(0);

  filter5.octaveControl(7);
  filterEnv5.sustain(0);

  filter6.octaveControl(7);
  filterEnv6.sustain(0);


  //lfo A
  lfoA1.begin(WAVEFORM_SINE);
  lfoA2.begin(WAVEFORM_SINE);
  lfoA3.begin(WAVEFORM_SINE);
  lfoA4.begin(WAVEFORM_SINE);
  lfoA5.begin(WAVEFORM_SINE);
  lfoA6.begin(WAVEFORM_SINE);

  //lfo B
  lfoB1.begin(0.5, 1, WAVEFORM_TRIANGLE);
  lfoB2.begin(0.5, 1, WAVEFORM_TRIANGLE);
  lfoB3.begin(0.5, 1, WAVEFORM_TRIANGLE);
  lfoB4.begin(0.5, 1, WAVEFORM_TRIANGLE);
  lfoB5.begin(0.5, 1, WAVEFORM_TRIANGLE);
  lfoB6.begin(0.5, 1, WAVEFORM_TRIANGLE);




  //dly
  dlyFiltL.frequency(4000);
  dlyFiltR.frequency(3000);

  dlyMixL.gain(0, 0.75);
  dlyMixL.gain(0, 0.75);


  dlyL.disable(1);
  dlyL.disable(2);
  dlyL.disable(3);
  dlyL.disable(4);
  dlyL.disable(5);
  dlyL.disable(6);
  dlyL.disable(7);

  dlyR.disable(1);
  dlyR.disable(2);
  dlyR.disable(3);
  dlyR.disable(4);
  dlyR.disable(5);
  dlyR.disable(6);
  dlyR.disable(7);


  //reverb
  reverb.roomsize(0.9);
  reverb.damping(0.8);




  //LFO DESTINATION DISCONNECT
  patchCord7.disconnect();   //vcoA
  patchCord8.disconnect();   //vcoB
  patchCord9.disconnect();   //vcoC
  patchCord10.disconnect();  //sub
  patchCord15.disconnect();  //vcoA
  patchCord16.disconnect();  //vcoB
  patchCord17.disconnect();  //vcoC
  patchCord18.disconnect();  //sub
  patchCord20.disconnect();  //vcoA
  patchCord21.disconnect();  //vcoB
  patchCord22.disconnect();  //vcoC
  patchCord23.disconnect();  //sub
  patchCord25.disconnect();  //vcoA
  patchCord26.disconnect();  //vcoB
  patchCord27.disconnect();  //vcoC
  patchCord28.disconnect();  //sub
  patchCord36.disconnect();  //vcoA
  patchCord37.disconnect();  //vcoB
  patchCord38.disconnect();  //vcoC
  patchCord39.disconnect();  //sub
  patchCord41.disconnect();  //vcoA
  patchCord42.disconnect();  //vcoB
  patchCord43.disconnect();  //vcoC
  patchCord44.disconnect();  //sub

  patchCord11.disconnect();  //filter
  patchCord19.disconnect();  //filter
  patchCord24.disconnect();  //filter
  patchCord29.disconnect();  //filter
  patchCord40.disconnect();  //filter
  patchCord45.disconnect();  //filter
}

void myPitchBend(byte channel, int pitch) {
  newpitchbend = (pitch + 8192) / 16;
}

void myControlChange(byte channel, byte control, int value) {
  switch (control) {

    case CCmodwheel:
      midiMod = (value << 3);
      //Serial.println(lfoAmppot);
      break;
  }
}

void myAfterTouch(byte channel, byte value) {
  midiMod = (value << 3);
}

FLASHMEM void myProgramChange(byte channel, byte program) {
  state = PATCH;
  patchNo = program + 1;
  recallPatch(patchNo);
  state = PARAMETER;
}

FLASHMEM void recallPatch(int patchNo) {
  //allNotesOff();
  File patchFile = SD.open(String(patchNo).c_str());
  if (!patchFile) {
    Serial.println("File not found");
  } else {
    String data[NO_OF_PARAMS];  //Array of data read in
    recallPatchData(patchFile, data);
    setCurrentPatchData(data);
    patchFile.close();
  }
  //  renderCurrentPatchPage();
  //  updateScreen();
}

FLASHMEM void reinitialiseToPanel() {
  //This sets the current patch to be the same as the current hardware panel state - all the pots
  //The four button controls stay the same state
  //This reinialises the previous hardware values to force a re-read
  muxInput = 0;
  for (int i = 0; i < MUXCHANNELS; i++) {
    mux1ValuesPrev[i] = RE_READ;
    mux2ValuesPrev[i] = RE_READ;
    mux3ValuesPrev[i] = RE_READ;
    mux4ValuesPrev[i] = RE_READ;
  }
  patchName = INITPATCHNAME;
  showPatchPage("Initial", "Panel Settings");
}

FLASHMEM void setCurrentPatchData(String data[]) {
  patchName = data[0];
  octave = data[1].toFloat();
  octaveB = data[2].toFloat();
  octaveC = data[3].toFloat();
  shapeA = data[4].toInt();
  shapeB = data[5].toInt();
  shapeC = data[6].toInt();
  tuneB = data[7].toFloat();
  tuneC = data[8].toFloat();
  crossMod = data[9].toFloat();
  vcoAvol = data[10].toFloat();
  vcoBvol = data[11].toFloat();
  vcoCvol = data[12].toFloat();
  Subvol = data[13].toFloat();
  cut = data[14].toInt();
  res = data[15].toFloat();
  filtAtt = data[16].toInt();
  filtDec = data[17].toInt();
  filtAmt = data[18].toFloat();
  filterMode = data[19].toInt();
  envAtt = data[20].toInt();
  envDec = data[21].toInt();
  envRel = data[22].toInt();
  envSus = data[23].toFloat();
  lfoAamp = data[24].toFloat();
  lfoAfreq = data[25].toFloat();
  lfoAdel = data[26].toInt();
  lfoAatt = data[27].toInt();
  lfoAdec = data[28].toInt();
  lfoArel = data[29].toInt();
  lfoAsus = data[30].toFloat();
  lfoBamp = data[31].toFloat();
  lfoBfreq = data[32].toFloat();
  dlyAmt = data[33].toFloat();
  dlyTimeL = data[34].toFloat();
  dlyTimeR = data[35].toFloat();
  revMix = data[36].toFloat();
  revSize = data[37].toFloat();
  lfoAdest = data[38].toFloat();
  lfoAshape = data[39].toFloat();

  //Patchname
  updatePatchname();

  Serial.print("Set Patch: ");
  Serial.println(patchName);
}

FLASHMEM String getCurrentPatchData() {
  return patchName + "," + String(octave) + "," + String(octaveB) + "," + String(octaveC) + "," + String(shapeA) + "," + String(shapeB) + "," + String(shapeC) + "," + String(tuneB) + "," + String(tuneC) + "," + String(crossMod) + "," + String(vcoAvol) + "," + String(vcoBvol) + "," + String(vcoCvol) + "," + String(Subvol) + "," + String(cut) + "," + String(res) + "," + String(filtAtt) + "," + String(filtDec) + "," + String(filtAmt) + "," + String(filterMode) + "," + String(envAtt) + "," + String(envDec) + "," + String(envRel) + "," + String(envSus) + "," + String(lfoAamp) + "," + String(lfoAfreq) + "," + String(lfoAdel) + "," + String(lfoAatt) + "," + String(lfoAdec) + "," + String(lfoArel) + "," + String(lfoAsus) + "," + String(lfoBamp) + "," + String(lfoBfreq) + "," + String(dlyAmt) + "," + String(dlyTimeL) + "," + String(dlyTimeR) + "," + String(revMix) + "," + String(revSize) + "," + String(lfoAdest) + "," + String(lfoAshape);
}

void updatePatchname() {
  showPatchPage(String(patchNo), patchName);
}

void allNotesOff() {
}

void checkSwitches() {

  saveButton.update();
  if (saveButton.read() == LOW && saveButton.duration() > HOLD_DURATION) {
    switch (state) {
      case PARAMETER:
      case PATCH:
        state = DELETE;
        saveButton.write(HIGH);  //Come out of this state
        del = true;              //Hack
        updateScreen();
        break;
    }
  } else if (saveButton.risingEdge()) {
    if (!del) {
      switch (state) {
        case PARAMETER:
          if (patches.size() < PATCHES_LIMIT) {
            resetPatchesOrdering();  //Reset order of patches from first patch
            patches.push({ patches.size() + 1, INITPATCHNAME });
            state = SAVE;
            updateScreen();
          }
          break;
        case SAVE:
          //Save as new patch with INITIALPATCH name or overwrite existing keeping name - bypassing patch renaming
          patchName = patches.last().patchName;
          state = PATCH;
          updateScreen();
          savePatch(String(patches.last().patchNo).c_str(), getCurrentPatchData());
          showPatchPage(patches.last().patchNo, patches.last().patchName);
          patchNo = patches.last().patchNo;
          loadPatches();  //Get rid of pushed patch if it wasn't saved
          setPatchesOrdering(patchNo);
          renamedPatch = "";
          state = PARAMETER;
          updateScreen();

          break;
        case PATCHNAMING:
          if (renamedPatch.length() > 0) patchName = renamedPatch;  //Prevent empty strings
          state = PATCH;
          updateScreen();
          savePatch(String(patches.last().patchNo).c_str(), getCurrentPatchData());
          showPatchPage(patches.last().patchNo, patchName);
          patchNo = patches.last().patchNo;
          loadPatches();  //Get rid of pushed patch if it wasn't saved
          setPatchesOrdering(patchNo);
          renamedPatch = "";
          state = PARAMETER;
          updateScreen();
          break;
      }
    } else {
      del = false;
    }
  }

  settingsButton.update();
  if (settingsButton.read() == LOW && settingsButton.duration() > HOLD_DURATION) {
    //If recall held, set current patch to match current hardware state
    //Reinitialise all hardware values to force them to be re-read if different
    state = REINITIALISE;
    reinitialiseToPanel();
    settingsButton.write(HIGH);  //Come out of this state
    reini = true;
    updateScreen();                          //Hack
  } else if (settingsButton.risingEdge()) {  //cannot be fallingEdge because holding button won't work
    if (!reini) {
      switch (state) {
        case PARAMETER:
          settingsValueIndex = getCurrentIndex(settingsOptions.first().currentIndex);
          showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[settingsValueIndex], SETTINGS);
          state = SETTINGS;
          updateScreen();
          break;
        case SETTINGS:
          settingsOptions.push(settingsOptions.shift());
          settingsValueIndex = getCurrentIndex(settingsOptions.first().currentIndex);
          showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[settingsValueIndex], SETTINGS);
        case SETTINGSVALUE:
          //Same as pushing Recall - store current settings item and go back to options
          settingsHandler(settingsOptions.first().value[settingsValueIndex], settingsOptions.first().handler);
          showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[settingsValueIndex], SETTINGS);
          state = SETTINGS;
          updateScreen();
          break;
      }
    } else {
      reini = false;
    }
  }

  backButton.update();
  if (backButton.read() == LOW && backButton.duration() > HOLD_DURATION) {
    //If Back button held, Panic - all notes off
    allNotesOff();
    backButton.write(HIGH);              //Come out of this state
    panic = true;                        //Hack
  } else if (backButton.risingEdge()) {  //cannot be fallingEdge because holding button won't work
    if (!panic) {
      switch (state) {
        case RECALL:
          setPatchesOrdering(patchNo);
          state = PARAMETER;
          updateScreen();
          break;
        case SAVE:
          renamedPatch = "";
          state = PARAMETER;
          loadPatches();  //Remove patch that was to be saved
          setPatchesOrdering(patchNo);
          updateScreen();
          break;
        case PATCHNAMING:
          charIndex = 0;
          renamedPatch = "";
          state = SAVE;
          updateScreen();
          break;
        case DELETE:
          setPatchesOrdering(patchNo);
          state = PARAMETER;
          break;
        case SETTINGS:
          state = PARAMETER;
          updateScreen();
          break;
        case SETTINGSVALUE:
          settingsValueIndex = getCurrentIndex(settingsOptions.first().currentIndex);
          showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[settingsValueIndex], SETTINGS);
          state = SETTINGS;
          updateScreen();
          break;
      }
    } else {
      panic = false;
    }
  }

  //Encoder switch
  recallButton.update();
  if (recallButton.read() == LOW && recallButton.duration() > HOLD_DURATION) {
    //If Recall button held, return to current patch setting
    //which clears any changes made
    state = PATCH;
    //Recall the current patch
    patchNo = patches.first().patchNo;
    recallPatch(patchNo);
    updateScreen();
    state = PARAMETER;
    recallButton.write(HIGH);  //Come out of this state
    recall = true;             //Hack
  } else if (recallButton.risingEdge()) {
    if (!recall) {
      switch (state) {
        case PARAMETER:
          state = RECALL;  //show patch list
          updateScreen();
          break;
        case RECALL:
          state = PATCH;
          //Recall the current patch
          patchNo = patches.first().patchNo;
          recallPatch(patchNo);
          state = PARAMETER;
          updateScreen();
          break;
        case SAVE:
          showRenamingPage(patches.last().patchName);
          patchName = patches.last().patchName;
          state = PATCHNAMING;
          updateScreen();
          break;
        case PATCHNAMING:
          if (renamedPatch.length() < 13) {
            renamedPatch.concat(String(currentCharacter));
            charIndex = 0;
            currentCharacter = CHARACTERS[charIndex];
            showRenamingPage(renamedPatch);
            updateScreen();
          }
          break;
        case DELETE:
          //Don't delete final patch
          if (patches.size() > 1) {
            state = DELETEMSG;
            patchNo = patches.first().patchNo;     //PatchNo to delete from SD card
            patches.shift();                       //Remove patch from circular buffer
            deletePatch(String(patchNo).c_str());  //Delete from SD card
            loadPatches();                         //Repopulate circular buffer to start from lowest Patch No
            renumberPatchesOnSD();
            loadPatches();                      //Repopulate circular buffer again after delete
            patchNo = patches.first().patchNo;  //Go back to 1
            recallPatch(patchNo);               //Load first patch
            updateScreen();
          }
          state = PARAMETER;
          updateScreen();
          break;
        case SETTINGS:
          //Choose this option and allow value choice
          settingsValueIndex = getCurrentIndex(settingsOptions.first().currentIndex);
          showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[settingsValueIndex], SETTINGSVALUE);
          state = SETTINGSVALUE;
          updateScreen();
          break;
        case SETTINGSVALUE:
          //Store current settings item and go back to options
          settingsHandler(settingsOptions.first().value[settingsValueIndex], settingsOptions.first().handler);
          showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[settingsValueIndex], SETTINGS);
          state = SETTINGS;
          updateScreen();
          break;
      }
    } else {
      recall = false;
    }
  }
}

void checkEncoder() {
  //Encoder works with relative inc and dec values
  //Detent encoder goes up in 4 steps, hence +/-3

  long encRead = encoder.read();
  if ((encCW && encRead > encPrevious + 3) || (!encCW && encRead < encPrevious - 3)) {
    switch (state) {
      case PARAMETER:
        state = PATCH;
        patches.push(patches.shift());
        patchNo = patches.first().patchNo;
        recallPatch(patchNo);
        state = PARAMETER;
        updateScreen();
        break;
      case RECALL:
        patches.push(patches.shift());
        updateScreen();
        break;
      case SAVE:
        patches.push(patches.shift());
        updateScreen();
        break;
      case PATCHNAMING:
        if (charIndex == TOTALCHARS) charIndex = 0;  //Wrap around
        currentCharacter = CHARACTERS[charIndex++];
        showRenamingPage(renamedPatch + currentCharacter);
        updateScreen();
        break;
      case DELETE:
        patches.push(patches.shift());
        updateScreen();
        break;
      case SETTINGS:
        settingsOptions.push(settingsOptions.shift());
        settingsValueIndex = getCurrentIndex(settingsOptions.first().currentIndex);
        showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[settingsValueIndex], SETTINGS);
        updateScreen();
        break;
      case SETTINGSVALUE:
        if (settingsOptions.first().value[settingsValueIndex + 1] != '\0')
          showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[++settingsValueIndex], SETTINGSVALUE);
        updateScreen();
        break;
    }
    encPrevious = encRead;
  } else if ((encCW && encRead < encPrevious - 3) || (!encCW && encRead > encPrevious + 3)) {
    switch (state) {
      case PARAMETER:
        state = PATCH;
        patches.unshift(patches.pop());
        patchNo = patches.first().patchNo;
        recallPatch(patchNo);
        state = PARAMETER;
        updateScreen();
        break;
      case RECALL:
        patches.unshift(patches.pop());
        updateScreen();
        break;
      case SAVE:
        patches.unshift(patches.pop());
        updateScreen();
        break;
      case PATCHNAMING:
        if (charIndex == -1)
          charIndex = TOTALCHARS - 1;
        currentCharacter = CHARACTERS[charIndex--];
        showRenamingPage(renamedPatch + currentCharacter);
        updateScreen();
        break;
      case DELETE:
        patches.unshift(patches.pop());
        updateScreen();
        break;
      case SETTINGS:
        settingsOptions.unshift(settingsOptions.pop());
        settingsValueIndex = getCurrentIndex(settingsOptions.first().currentIndex);
        showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[settingsValueIndex], SETTINGS);
        updateScreen();
        break;
      case SETTINGSVALUE:
        if (settingsValueIndex > 0)
          showSettingsPage(settingsOptions.first().option, settingsOptions.first().value[--settingsValueIndex], SETTINGSVALUE);
        updateScreen();
        break;
    }
    encPrevious = encRead;
  }
}

void loop() {
  checkMux();
  updateParams();

  checkSwitches();
  checkEncoder();

  //usbMIDI.read();
  MIDI.read();

  //cross mod
  modMix1.gain(0, crossMod);
  modMix2.gain(0, crossMod);
  modMix3.gain(0, crossMod);
  modMix4.gain(0, crossMod);
  modMix5.gain(0, crossMod);
  modMix6.gain(0, crossMod);

  //voice 1 frequencies
  vcoA1.frequency(noteFreqs[note1freq] * octave * bend);
  vcoB1.frequency(noteFreqs[note1freq] * octave * octaveB * tuneB * bend);
  vcoC1.frequency(noteFreqs[note1freq] * octave * octaveC * tuneC * bend);
  sub1.frequency(noteFreqs[note1freq] / 2 * octave * bend);

  vcoA2.frequency(noteFreqs[note2freq] * octave * bend);
  vcoB2.frequency(noteFreqs[note2freq] * octave * octaveB * tuneB * bend);
  vcoC2.frequency(noteFreqs[note2freq] * octave * octaveC * tuneC * bend);
  sub2.frequency(noteFreqs[note2freq] / 2 * octave * bend);

  vcoA3.frequency(noteFreqs[note3freq] * octave * bend);
  vcoB3.frequency(noteFreqs[note3freq] * octave * octaveB * tuneB * bend);
  vcoC3.frequency(noteFreqs[note3freq] * octave * octaveC * tuneC * bend);
  sub3.frequency(noteFreqs[note3freq] / 2 * octave * bend);

  vcoA4.frequency(noteFreqs[note4freq] * octave * bend);
  vcoB4.frequency(noteFreqs[note4freq] * octave * octaveB * tuneB * bend);
  vcoC4.frequency(noteFreqs[note4freq] * octave * octaveC * tuneC * bend);
  sub4.frequency(noteFreqs[note4freq] / 2 * octave * bend);

  vcoA5.frequency(noteFreqs[note5freq] * octave * bend);
  vcoB5.frequency(noteFreqs[note5freq] * octave * octaveB * tuneB * bend);
  vcoC5.frequency(noteFreqs[note5freq] * octave * octaveC * tuneC * bend);
  sub5.frequency(noteFreqs[note5freq] / 2 * octave * bend);

  vcoA6.frequency(noteFreqs[note6freq] * octave * bend);
  vcoB6.frequency(noteFreqs[note6freq] * octave * octaveB * tuneB * bend);
  vcoC6.frequency(noteFreqs[note6freq] * octave * octaveC * tuneC * bend);
  sub6.frequency(noteFreqs[note6freq] / 2 * octave * bend);



  //vco Mixer
  voiceMix1.gain(0, vcoAvol * mainVol);
  voiceMix1.gain(1, vcoBvol * mainVol);
  voiceMix1.gain(2, vcoCvol * mainVol);
  voiceMix1.gain(3, Subvol * mainVol);

  voiceMix2.gain(0, vcoAvol * mainVol);
  voiceMix2.gain(1, vcoBvol * mainVol);
  voiceMix2.gain(2, vcoCvol * mainVol);
  voiceMix2.gain(3, Subvol * mainVol);

  voiceMix3.gain(0, vcoAvol * mainVol);
  voiceMix3.gain(1, vcoBvol * mainVol);
  voiceMix3.gain(2, vcoCvol * mainVol);
  voiceMix3.gain(3, Subvol * mainVol);

  voiceMix4.gain(0, vcoAvol * mainVol);
  voiceMix4.gain(1, vcoBvol * mainVol);
  voiceMix4.gain(2, vcoCvol * mainVol);
  voiceMix4.gain(3, Subvol * mainVol);

  voiceMix5.gain(0, vcoAvol * mainVol);
  voiceMix5.gain(1, vcoBvol * mainVol);
  voiceMix5.gain(2, vcoCvol * mainVol);
  voiceMix5.gain(3, Subvol * mainVol);

  voiceMix6.gain(0, vcoAvol * mainVol);
  voiceMix6.gain(1, vcoBvol * mainVol);
  voiceMix6.gain(2, vcoCvol * mainVol);
  voiceMix6.gain(3, Subvol * mainVol);

  //vco A shape
  if (shapeA == 0) {
    vcoA1.begin(WAVEFORM_PULSE);
    vcoA2.begin(WAVEFORM_PULSE);
    vcoA3.begin(WAVEFORM_PULSE);
    vcoA4.begin(WAVEFORM_PULSE);
    vcoA5.begin(WAVEFORM_PULSE);
    vcoA6.begin(WAVEFORM_PULSE);
  } else if (shapeA == 1) {
    vcoA1.begin(WAVEFORM_SAWTOOTH);
    vcoA1.amplitude(vcoVol);
    vcoA2.begin(WAVEFORM_SAWTOOTH);
    vcoA2.amplitude(vcoVol);
    vcoA3.begin(WAVEFORM_SAWTOOTH);
    vcoA3.amplitude(vcoVol);
    vcoA4.begin(WAVEFORM_SAWTOOTH);
    vcoA4.amplitude(vcoVol);
    vcoA5.begin(WAVEFORM_SAWTOOTH);
    vcoA5.amplitude(vcoVol);
    vcoA6.begin(WAVEFORM_SAWTOOTH);
    vcoA6.amplitude(vcoVol);
  } else if (shapeA == 2) {
    vcoA1.begin(WAVEFORM_TRIANGLE_VARIABLE);
    vcoA1.amplitude(vcoVol * 1.5);
    vcoA2.begin(WAVEFORM_TRIANGLE_VARIABLE);
    vcoA2.amplitude(vcoVol * 1.5);
    vcoA3.begin(WAVEFORM_TRIANGLE_VARIABLE);
    vcoA3.amplitude(vcoVol * 1.5);
    vcoA4.begin(WAVEFORM_TRIANGLE_VARIABLE);
    vcoA4.amplitude(vcoVol * 1.5);
    vcoA5.begin(WAVEFORM_TRIANGLE_VARIABLE);
    vcoA5.amplitude(vcoVol * 1.5);
    vcoA6.begin(WAVEFORM_TRIANGLE_VARIABLE);
    vcoA6.amplitude(vcoVol * 1.5);
  }

  //vco B shape
  if (shapeB == 0) {
    vcoB1.begin(WAVEFORM_PULSE);
    vcoB2.begin(WAVEFORM_PULSE);
    vcoB3.begin(WAVEFORM_PULSE);
    vcoB4.begin(WAVEFORM_PULSE);
    vcoB5.begin(WAVEFORM_PULSE);
    vcoB6.begin(WAVEFORM_PULSE);
  } else if (shapeB == 1) {
    vcoB1.begin(WAVEFORM_SAWTOOTH);
    vcoB1.amplitude(vcoVol);
    vcoB2.begin(WAVEFORM_SAWTOOTH);
    vcoB2.amplitude(vcoVol);
    vcoB3.begin(WAVEFORM_SAWTOOTH);
    vcoB3.amplitude(vcoVol);
    vcoB4.begin(WAVEFORM_SAWTOOTH);
    vcoB4.amplitude(vcoVol);
    vcoB5.begin(WAVEFORM_SAWTOOTH);
    vcoB5.amplitude(vcoVol);
    vcoB6.begin(WAVEFORM_SAWTOOTH);
    vcoB6.amplitude(vcoVol);
  } else if (shapeB == 2) {
    vcoB1.begin(WAVEFORM_TRIANGLE_VARIABLE);
    vcoB1.amplitude(vcoVol * 1.5);
    vcoB2.begin(WAVEFORM_TRIANGLE_VARIABLE);
    vcoB2.amplitude(vcoVol * 1.5);
    vcoB3.begin(WAVEFORM_TRIANGLE_VARIABLE);
    vcoB3.amplitude(vcoVol * 1.5);
    vcoB4.begin(WAVEFORM_TRIANGLE_VARIABLE);
    vcoB4.amplitude(vcoVol * 1.5);
    vcoB5.begin(WAVEFORM_TRIANGLE_VARIABLE);
    vcoB5.amplitude(vcoVol * 1.5);
    vcoB6.begin(WAVEFORM_TRIANGLE_VARIABLE);
    vcoB6.amplitude(vcoVol * 1.5);
  }

  //Vco C shapes
  switch (shapeC) {
    case 1 ... 32:
      vcoC1.arbitraryWaveform(wave1, 2000);
      vcoC2.arbitraryWaveform(wave1, 2000);
      vcoC3.arbitraryWaveform(wave1, 2000);
      vcoC4.arbitraryWaveform(wave1, 2000);
      vcoC5.arbitraryWaveform(wave1, 2000);
      vcoC6.arbitraryWaveform(wave1, 2000);
      break;
    case 37 ... 69:
      vcoC1.arbitraryWaveform(wave2, 2000);
      vcoC2.arbitraryWaveform(wave2, 2000);
      vcoC3.arbitraryWaveform(wave2, 2000);
      vcoC4.arbitraryWaveform(wave2, 2000);
      vcoC5.arbitraryWaveform(wave2, 2000);
      vcoC6.arbitraryWaveform(wave2, 2000);
      break;
    case 73 ... 105:
      vcoC1.arbitraryWaveform(wave3, 2000);
      vcoC2.arbitraryWaveform(wave3, 2000);
      vcoC3.arbitraryWaveform(wave3, 2000);
      vcoC4.arbitraryWaveform(wave3, 2000);
      vcoC5.arbitraryWaveform(wave3, 2000);
      vcoC6.arbitraryWaveform(wave3, 2000);
      break;
    case 109 ... 141:
      vcoC1.arbitraryWaveform(wave4, 2000);
      vcoC2.arbitraryWaveform(wave4, 2000);
      vcoC3.arbitraryWaveform(wave4, 2000);
      vcoC4.arbitraryWaveform(wave4, 2000);
      vcoC5.arbitraryWaveform(wave4, 2000);
      vcoC6.arbitraryWaveform(wave4, 2000);
      break;
    case 145 ... 177:
      vcoC1.arbitraryWaveform(wave5, 2000);
      vcoC2.arbitraryWaveform(wave5, 2000);
      vcoC3.arbitraryWaveform(wave5, 2000);
      vcoC4.arbitraryWaveform(wave5, 2000);
      vcoC5.arbitraryWaveform(wave5, 2000);
      vcoC6.arbitraryWaveform(wave5, 2000);
      break;
    case 181 ... 212:
      vcoC1.arbitraryWaveform(wave6, 2000);
      vcoC2.arbitraryWaveform(wave6, 2000);
      vcoC3.arbitraryWaveform(wave6, 2000);
      vcoC4.arbitraryWaveform(wave6, 2000);
      vcoC5.arbitraryWaveform(wave6, 2000);
      vcoC6.arbitraryWaveform(wave6, 2000);
      break;
    case 217 ... 248:
      vcoC1.arbitraryWaveform(wave7, 2000);
      vcoC2.arbitraryWaveform(wave7, 2000);
      vcoC3.arbitraryWaveform(wave7, 2000);
      vcoC4.arbitraryWaveform(wave7, 2000);
      vcoC5.arbitraryWaveform(wave7, 2000);
      vcoC6.arbitraryWaveform(wave7, 2000);
      break;
    case 253 ... 285:
      vcoC1.arbitraryWaveform(wave8, 2000);
      vcoC2.arbitraryWaveform(wave8, 2000);
      vcoC3.arbitraryWaveform(wave8, 2000);
      vcoC4.arbitraryWaveform(wave8, 2000);
      vcoC5.arbitraryWaveform(wave8, 2000);
      vcoC6.arbitraryWaveform(wave8, 2000);
      break;
    case 289 ... 320:
      vcoC1.arbitraryWaveform(wave9, 2000);
      vcoC2.arbitraryWaveform(wave9, 2000);
      vcoC3.arbitraryWaveform(wave9, 2000);
      vcoC4.arbitraryWaveform(wave9, 2000);
      vcoC5.arbitraryWaveform(wave9, 2000);
      vcoC6.arbitraryWaveform(wave9, 2000);
      break;
    case 325 ... 357:
      vcoC1.arbitraryWaveform(wave10, 2000);
      vcoC2.arbitraryWaveform(wave10, 2000);
      vcoC3.arbitraryWaveform(wave10, 2000);
      vcoC4.arbitraryWaveform(wave10, 2000);
      vcoC5.arbitraryWaveform(wave10, 2000);
      vcoC6.arbitraryWaveform(wave10, 2000);
      break;
    case 361 ... 393:
      vcoC1.arbitraryWaveform(wave11, 2000);
      vcoC2.arbitraryWaveform(wave11, 2000);
      vcoC3.arbitraryWaveform(wave11, 2000);
      vcoC4.arbitraryWaveform(wave11, 2000);
      vcoC5.arbitraryWaveform(wave11, 2000);
      vcoC6.arbitraryWaveform(wave11, 2000);
      break;
    case 397 ... 429:
      vcoC1.arbitraryWaveform(wave12, 2000);
      vcoC2.arbitraryWaveform(wave12, 2000);
      vcoC3.arbitraryWaveform(wave12, 2000);
      vcoC4.arbitraryWaveform(wave12, 2000);
      vcoC5.arbitraryWaveform(wave12, 2000);
      vcoC6.arbitraryWaveform(wave12, 2000);
      break;
    case 433 ... 465:
      vcoC1.arbitraryWaveform(wave13, 2000);
      vcoC2.arbitraryWaveform(wave13, 2000);
      vcoC3.arbitraryWaveform(wave13, 2000);
      vcoC4.arbitraryWaveform(wave13, 2000);
      vcoC5.arbitraryWaveform(wave13, 2000);
      vcoC6.arbitraryWaveform(wave13, 2000);
      break;
    case 469 ... 500:
      vcoC1.arbitraryWaveform(wave14, 2000);
      vcoC2.arbitraryWaveform(wave14, 2000);
      vcoC3.arbitraryWaveform(wave14, 2000);
      vcoC4.arbitraryWaveform(wave14, 2000);
      vcoC5.arbitraryWaveform(wave14, 2000);
      vcoC6.arbitraryWaveform(wave14, 2000);
      break;
    case 505 ... 537:
      vcoC1.arbitraryWaveform(wave15, 2000);
      vcoC2.arbitraryWaveform(wave15, 2000);
      vcoC3.arbitraryWaveform(wave15, 2000);
      vcoC4.arbitraryWaveform(wave15, 2000);
      vcoC5.arbitraryWaveform(wave15, 2000);
      vcoC6.arbitraryWaveform(wave15, 2000);
      break;
    case 541 ... 573:
      vcoC1.arbitraryWaveform(wave16, 2000);
      vcoC2.arbitraryWaveform(wave16, 2000);
      vcoC3.arbitraryWaveform(wave16, 2000);
      vcoC4.arbitraryWaveform(wave16, 2000);
      vcoC5.arbitraryWaveform(wave16, 2000);
      vcoC6.arbitraryWaveform(wave16, 2000);
      break;
    case 577 ... 609:
      vcoC1.arbitraryWaveform(wave17, 2000);
      vcoC2.arbitraryWaveform(wave17, 2000);
      vcoC3.arbitraryWaveform(wave17, 2000);
      vcoC4.arbitraryWaveform(wave17, 2000);
      vcoC5.arbitraryWaveform(wave17, 2000);
      vcoC6.arbitraryWaveform(wave17, 2000);
      break;
    case 613 ... 645:
      vcoC1.arbitraryWaveform(wave18, 2000);
      vcoC2.arbitraryWaveform(wave18, 2000);
      vcoC3.arbitraryWaveform(wave18, 2000);
      vcoC4.arbitraryWaveform(wave18, 2000);
      vcoC5.arbitraryWaveform(wave18, 2000);
      vcoC6.arbitraryWaveform(wave18, 2000);
      break;
    case 649 ... 680:
      vcoC1.arbitraryWaveform(wave19, 2000);
      vcoC2.arbitraryWaveform(wave19, 2000);
      vcoC3.arbitraryWaveform(wave19, 2000);
      vcoC4.arbitraryWaveform(wave19, 2000);
      vcoC5.arbitraryWaveform(wave19, 2000);
      vcoC6.arbitraryWaveform(wave19, 2000);
      break;
    case 685 ... 717:
      vcoC1.arbitraryWaveform(wave20, 2000);
      vcoC2.arbitraryWaveform(wave20, 2000);
      vcoC3.arbitraryWaveform(wave20, 2000);
      vcoC4.arbitraryWaveform(wave20, 2000);
      vcoC5.arbitraryWaveform(wave20, 2000);
      vcoC6.arbitraryWaveform(wave20, 2000);
      break;
    case 721 ... 752:
      vcoC1.arbitraryWaveform(wave21, 2000);
      vcoC2.arbitraryWaveform(wave21, 2000);
      vcoC3.arbitraryWaveform(wave21, 2000);
      vcoC4.arbitraryWaveform(wave21, 2000);
      vcoC5.arbitraryWaveform(wave21, 2000);
      vcoC6.arbitraryWaveform(wave21, 2000);
      break;
    case 757 ... 789:
      vcoC1.arbitraryWaveform(wave22, 2000);
      vcoC2.arbitraryWaveform(wave22, 2000);
      vcoC3.arbitraryWaveform(wave22, 2000);
      vcoC4.arbitraryWaveform(wave22, 2000);
      vcoC5.arbitraryWaveform(wave22, 2000);
      vcoC6.arbitraryWaveform(wave22, 2000);
      break;
    case 793 ... 825:
      vcoC1.arbitraryWaveform(wave23, 2000);
      vcoC2.arbitraryWaveform(wave23, 2000);
      vcoC3.arbitraryWaveform(wave23, 2000);
      vcoC4.arbitraryWaveform(wave23, 2000);
      vcoC5.arbitraryWaveform(wave23, 2000);
      vcoC6.arbitraryWaveform(wave23, 2000);
      break;
    case 829 ... 860:
      vcoC1.arbitraryWaveform(wave24, 2000);
      vcoC2.arbitraryWaveform(wave24, 2000);
      vcoC3.arbitraryWaveform(wave24, 2000);
      vcoC4.arbitraryWaveform(wave24, 2000);
      vcoC5.arbitraryWaveform(wave24, 2000);
      vcoC6.arbitraryWaveform(wave24, 2000);
      break;
    case 865 ... 896:
      vcoC1.arbitraryWaveform(wave25, 2000);
      vcoC2.arbitraryWaveform(wave25, 2000);
      vcoC3.arbitraryWaveform(wave25, 2000);
      vcoC4.arbitraryWaveform(wave25, 2000);
      vcoC5.arbitraryWaveform(wave25, 2000);
      vcoC6.arbitraryWaveform(wave25, 2000);
      break;
    case 901 ... 933:
      vcoC1.arbitraryWaveform(wave26, 2000);
      vcoC2.arbitraryWaveform(wave26, 2000);
      vcoC3.arbitraryWaveform(wave26, 2000);
      vcoC4.arbitraryWaveform(wave26, 2000);
      vcoC5.arbitraryWaveform(wave26, 2000);
      vcoC6.arbitraryWaveform(wave26, 2000);
      break;
    case 937 ... 966:
      vcoC1.arbitraryWaveform(wave27, 2000);
      vcoC2.arbitraryWaveform(wave27, 2000);
      vcoC3.arbitraryWaveform(wave27, 2000);
      vcoC4.arbitraryWaveform(wave27, 2000);
      vcoC5.arbitraryWaveform(wave27, 2000);
      vcoC6.arbitraryWaveform(wave27, 2000);
      break;
    case 970 ... 1024:
      vcoC1.arbitraryWaveform(wave28, 2000);
      vcoC2.arbitraryWaveform(wave28, 2000);
      vcoC3.arbitraryWaveform(wave28, 2000);
      vcoC4.arbitraryWaveform(wave28, 2000);
      vcoC5.arbitraryWaveform(wave28, 2000);
      vcoC6.arbitraryWaveform(wave28, 2000);
      break;
  }

  // d_filter_lfos starts here

  //filter
  filter1.frequency(cut);
  filter1.resonance(res);

  filter2.frequency(cut);
  filter2.resonance(res);

  filter3.frequency(cut);
  filter3.resonance(res);

  filter4.frequency(cut);
  filter4.resonance(res);

  filter5.frequency(cut);
  filter5.resonance(res);

  filter6.frequency(cut);
  filter6.resonance(res);


  //filter env
  filterEnv1.attack(filtAtt);
  filterEnv1.decay(filtDec);
  filterEnv1.release(filtDec);
  dc1.amplitude(filtAmt);

  filterEnv2.attack(filtAtt);
  filterEnv2.decay(filtDec);
  filterEnv2.release(filtDec);
  dc2.amplitude(filtAmt);

  filterEnv3.attack(filtAtt);
  filterEnv3.decay(filtDec);
  filterEnv3.release(filtDec);
  dc3.amplitude(filtAmt);

  filterEnv4.attack(filtAtt);
  filterEnv4.decay(filtDec);
  filterEnv4.release(filtDec);
  dc4.amplitude(filtAmt);

  filterEnv5.attack(filtAtt);
  filterEnv5.decay(filtDec);
  filterEnv5.release(filtDec);
  dc5.amplitude(filtAmt);

  filterEnv6.attack(filtAtt);
  filterEnv6.decay(filtDec);
  filterEnv6.release(filtDec);
  dc6.amplitude(filtAmt);


  //filter mode
  if (filterMode == 1) {
    filterMode1.gain(0, 1);
    filterMode1.gain(1, 0);
    filterMode2.gain(0, 1);
    filterMode2.gain(1, 0);
    filterMode3.gain(0, 1);
    filterMode3.gain(1, 0);
    filterMode4.gain(0, 1);
    filterMode4.gain(1, 0);
    filterMode5.gain(0, 1);
    filterMode5.gain(1, 0);
    filterMode6.gain(0, 1);
    filterMode6.gain(1, 0);
  } else if (filterMode == 0) {
    filterMode1.gain(0, 0);
    filterMode1.gain(1, 1);
    filterMode2.gain(0, 0);
    filterMode2.gain(1, 1);
    filterMode3.gain(0, 0);
    filterMode3.gain(1, 1);
    filterMode4.gain(0, 0);
    filterMode4.gain(1, 1);
    filterMode5.gain(0, 0);
    filterMode5.gain(1, 1);
    filterMode6.gain(0, 0);
    filterMode6.gain(1, 1);
  }

  //lfo A params
  lfoA1.amplitude(lfoAamp);
  lfoA1.frequency(lfoAfreq);
  lfoAenv1.delay(lfoAdel);
  lfoAenv1.attack(lfoAatt);
  lfoAenv1.decay(lfoAdec);
  lfoAenv1.release(lfoArel);
  lfoAenv1.sustain(lfoAsus);

  lfoA2.amplitude(lfoAamp);
  lfoA2.frequency(lfoAfreq);
  lfoAenv2.delay(lfoAdel);
  lfoAenv2.attack(lfoAatt);
  lfoAenv2.decay(lfoAdec);
  lfoAenv2.release(lfoArel);
  lfoAenv2.sustain(lfoAsus);

  lfoA3.amplitude(lfoAamp);
  lfoA3.frequency(lfoAfreq);
  lfoAenv3.delay(lfoAdel);
  lfoAenv3.attack(lfoAatt);
  lfoAenv3.decay(lfoAdec);
  lfoAenv3.release(lfoArel);
  lfoAenv3.sustain(lfoAsus);

  lfoA4.amplitude(lfoAamp);
  lfoA4.frequency(lfoAfreq);
  lfoAenv4.delay(lfoAdel);
  lfoAenv4.attack(lfoAatt);
  lfoAenv4.decay(lfoAdec);
  lfoAenv4.release(lfoArel);
  lfoAenv4.sustain(lfoAsus);

  lfoA5.amplitude(lfoAamp);
  lfoA5.frequency(lfoAfreq);
  lfoAenv5.delay(lfoAdel);
  lfoAenv5.attack(lfoAatt);
  lfoAenv5.decay(lfoAdec);
  lfoAenv5.release(lfoArel);
  lfoAenv5.sustain(lfoAsus);

  lfoA6.amplitude(lfoAamp);
  lfoA6.frequency(lfoAfreq);
  lfoAenv6.delay(lfoAdel);
  lfoAenv6.attack(lfoAatt);
  lfoAenv6.decay(lfoAdec);
  lfoAenv6.release(lfoArel);
  lfoAenv6.sustain(lfoAsus);

  //lfo shape switch
  if (lfoAshape == 0) {
    lfoA1.begin(WAVEFORM_SINE);
    lfoA2.begin(WAVEFORM_SINE);
    lfoA3.begin(WAVEFORM_SINE);
    lfoA4.begin(WAVEFORM_SINE);
    lfoA5.begin(WAVEFORM_SINE);
    lfoA6.begin(WAVEFORM_SINE);
  } else if (lfoAshape == 1) {
    lfoA1.begin(WAVEFORM_SAWTOOTH_REVERSE);
    lfoA2.begin(WAVEFORM_SAWTOOTH_REVERSE);
    lfoA3.begin(WAVEFORM_SAWTOOTH_REVERSE);
    lfoA4.begin(WAVEFORM_SAWTOOTH_REVERSE);
    lfoA5.begin(WAVEFORM_SAWTOOTH_REVERSE);
    lfoA6.begin(WAVEFORM_SAWTOOTH_REVERSE);
  } else if (lfoAshape == 2) {
    lfoA1.begin(WAVEFORM_SAMPLE_HOLD);
    lfoA2.begin(WAVEFORM_SAMPLE_HOLD);
    lfoA3.begin(WAVEFORM_SAMPLE_HOLD);
    lfoA4.begin(WAVEFORM_SAMPLE_HOLD);
    lfoA5.begin(WAVEFORM_SAMPLE_HOLD);
    lfoA6.begin(WAVEFORM_SAMPLE_HOLD);
  }


  //lfo A read
  unsigned long currTime = millis();
  if (currTime - prevTime >= readInt) {
    ampMod = lfoAread1.read();
    prevTime = currTime;
  }
  finalMix.gain(0, (1 - (ampMod * 3.2)));
  finalMix.gain(1, (1 - (ampMod * 3.2)));





  //lfo B params
  lfoB1.amplitude(lfoBamp);
  lfoB1.frequency(lfoBfreq);

  lfoB2.amplitude(lfoBamp);
  lfoB2.frequency(lfoBfreq);

  lfoB3.amplitude(lfoBamp);
  lfoB3.frequency(lfoBfreq);

  lfoB4.amplitude(lfoBamp);
  lfoB4.frequency(lfoBfreq);

  lfoB5.amplitude(lfoBamp);
  lfoB5.frequency(lfoBfreq);

  lfoB6.amplitude(lfoBamp);
  lfoB6.frequency(lfoBfreq);



  //LFO A DESTINATION
  if (lfoAdest == 0) {      //lfo - pitch
    patchCord7.connect();   //vcoA
    patchCord8.connect();   //vcoB
    patchCord9.connect();   //vcoC
    patchCord10.connect();  //sub
    patchCord15.connect();  //vcoA
    patchCord16.connect();  //vcoB
    patchCord17.connect();  //vcoC
    patchCord18.connect();  //sub
    patchCord20.connect();  //vcoA
    patchCord21.connect();  //vcoB
    patchCord22.connect();  //vcoC
    patchCord23.connect();  //sub
    patchCord25.connect();  //vcoA
    patchCord26.connect();  //vcoB
    patchCord27.connect();  //vcoC
    patchCord28.connect();  //sub
    patchCord36.connect();  //vcoA
    patchCord37.connect();  //vcoB
    patchCord38.connect();  //vcoC
    patchCord39.connect();  //sub
    patchCord41.connect();  //vcoA
    patchCord42.connect();  //vcoB
    patchCord43.connect();  //vcoC
    patchCord44.connect();  //sub

    patchCord11.disconnect();  //filter
    patchCord19.disconnect();  //filter
    patchCord24.disconnect();  //filter
    patchCord29.disconnect();  //filter
    patchCord40.disconnect();  //filter
    patchCord45.disconnect();  //filter

    patchCord12.disconnect();  //lfoAread.
  }
  if (lfoAdest == 1) {  //lfo - filter

    patchCord7.disconnect();   //vcoA
    patchCord8.disconnect();   //vcoB
    patchCord9.disconnect();   //vcoC
    patchCord10.disconnect();  //sub
    patchCord15.disconnect();  //vcoA
    patchCord16.disconnect();  //vcoB
    patchCord17.disconnect();  //vcoC
    patchCord18.disconnect();  //sub
    patchCord20.disconnect();  //vcoA
    patchCord21.disconnect();  //vcoB
    patchCord22.disconnect();  //vcoC
    patchCord23.disconnect();  //sub
    patchCord25.disconnect();  //vcoA
    patchCord26.disconnect();  //vcoB
    patchCord27.disconnect();  //vcoC
    patchCord28.disconnect();  //sub
    patchCord36.disconnect();  //vcoA
    patchCord37.disconnect();  //vcoB
    patchCord38.disconnect();  //vcoC
    patchCord39.disconnect();  //sub
    patchCord41.disconnect();  //vcoA
    patchCord42.disconnect();  //vcoB
    patchCord43.disconnect();  //vcoC
    patchCord44.disconnect();  //sub

    patchCord11.connect();  //filter
    patchCord19.connect();  //filter
    patchCord24.connect();  //filter
    patchCord29.connect();  //filter
    patchCord40.connect();  //filter
    patchCord45.connect();  //filter

    patchCord12.disconnect();  //lfoAread.
  }
  if (lfoAdest == 2) {  //lfo - amp

    patchCord7.disconnect();   //vcoA
    patchCord8.disconnect();   //vcoB
    patchCord9.disconnect();   //vcoC
    patchCord10.disconnect();  //sub
    patchCord15.disconnect();  //vcoA
    patchCord16.disconnect();  //vcoB
    patchCord17.disconnect();  //vcoC
    patchCord18.disconnect();  //sub
    patchCord20.disconnect();  //vcoA
    patchCord21.disconnect();  //vcoB
    patchCord22.disconnect();  //vcoC
    patchCord23.disconnect();  //sub
    patchCord25.disconnect();  //vcoA
    patchCord26.disconnect();  //vcoB
    patchCord27.disconnect();  //vcoC
    patchCord28.disconnect();  //sub
    patchCord36.disconnect();  //vcoA
    patchCord37.disconnect();  //vcoB
    patchCord38.disconnect();  //vcoC
    patchCord39.disconnect();  //sub
    patchCord41.disconnect();  //vcoA
    patchCord42.disconnect();  //vcoB
    patchCord43.disconnect();  //vcoC
    patchCord44.disconnect();  //sub

    patchCord11.disconnect();  //filter
    patchCord19.disconnect();  //filter
    patchCord24.disconnect();  //filter
    patchCord29.disconnect();  //filter
    patchCord40.disconnect();  //filter
    patchCord45.disconnect();  //filter

    patchCord12.connect();  //lfoAread.
  }

  // e_dly_reverb start here

  //Main ENVELOPE
  env1.attack(envAtt);
  env2.attack(envAtt);
  env3.attack(envAtt);
  env4.attack(envAtt);
  env5.attack(envAtt);
  env6.attack(envAtt);

  env1.decay(envDec);
  env2.decay(envDec);
  env3.decay(envDec);
  env4.decay(envDec);
  env5.decay(envDec);
  env6.decay(envDec);

  env1.sustain(envSus);
  env2.sustain(envSus);
  env3.sustain(envSus);
  env4.sustain(envSus);
  env5.sustain(envSus);
  env6.sustain(envSus);

  env1.release(envRel);
  env2.release(envRel);
  env3.release(envRel);
  env4.release(envRel);
  env5.release(envRel);
  env6.release(envRel);



  //delay
  dlyL.delay(0, dlyTimeL);
  dlyMixL.gain(1, dlyAmt * 0.9);

  dlyR.delay(0, dlyTimeR);
  dlyMixR.gain(1, (dlyAmt / 1.4) * 0.9);


  //reverb
  fxL.gain(1, revMix);
  fxR.gain(1, revMix);

  reverb.roomsize(revSize);


  //output gain reduction
  fxL.gain(0, outGain - revMix / 1.6);
  fxL.gain(2, outGain - revMix / 1.6);

  fxR.gain(0, outGain - revMix / 1.6);
  fxR.gain(2, outGain - revMix / 1.6);
}
