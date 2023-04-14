int getVoiceNo(int note) {
  voiceToReturn = -1;       //Initialise to 'null'
  earliestTime = millis();  //Initialise to now
  if (note == -1) {
    //NoteOn() - Get the oldest free voice (recent voices may be still on release stage)
    for (int i = 0; i < NO_OF_VOICES; i++) {
      if (voices[i].note == -1) {
        if (voices[i].timeOn < earliestTime) {
          earliestTime = voices[i].timeOn;
          voiceToReturn = i;
        }
      }
    }
    if (voiceToReturn == -1) {
      //No free voices, need to steal oldest sounding voice
      earliestTime = millis();  //Reinitialise
      for (int i = 0; i < NO_OF_VOICES; i++) {
        if (voices[i].timeOn < earliestTime) {
          earliestTime = voices[i].timeOn;
          voiceToReturn = i;
        }
      }
    }
    return voiceToReturn + 1;
  } else {
    //NoteOff() - Get voice number from note
    for (int i = 0; i < NO_OF_VOICES; i++) {
      if (voices[i].note == note) {
        return i + 1;
      }
    }
  }
  //Shouldn't get here, return voice 1
  return 1;
}

void myNoteOn(byte channel, byte note, byte velocity) {

  if (digitalRead(1) == MONO_POLY1) {  //POLYPHONIC mode
    if (note < 0 || note > 127) return;
    switch (getVoiceNo(-1)) {
      case 1:
        voices[0].note = note;
        note1freq = note;
        env1.noteOn();
        filterEnv1.noteOn();
        lfoAenv1.noteOn();
        env1on = true;
        voiceOn[0] = true;
        break;
      case 2:
        voices[1].note = note;
        note2freq = note;
        env2.noteOn();
        filterEnv2.noteOn();
        lfoAenv2.noteOn();
        env2on = true;
        voiceOn[1] = true;
        break;
      case 3:
        voices[2].note = note;
        note3freq = note;
        env3.noteOn();
        filterEnv3.noteOn();
        lfoAenv3.noteOn();
        env3on = true;
        voiceOn[2] = true;
        break;
      case 4:
        voices[3].note = note;
        note4freq = note;
        env4.noteOn();
        filterEnv4.noteOn();
        lfoAenv4.noteOn();
        env4on = true;
        voiceOn[3] = true;
        break;
      case 5:
        voices[4].note = note;
        note5freq = note;
        env5.noteOn();
        filterEnv5.noteOn();
        lfoAenv5.noteOn();
        env5on = true;
        voiceOn[4] = true;
        break;
      case 6:
        voices[5].note = note;
        note6freq = note;
        env6.noteOn();
        filterEnv6.noteOn();
        lfoAenv6.noteOn();
        env6on = true;
        voiceOn[5] = true;
        break;
    }
  } else {
    voices[0].note = note;
    note1freq = note;
    env1.noteOn();
    filterEnv1.noteOn();
    lfoAenv1.noteOn();
    env1on = true;
    voiceOn[0] = true;
  }
}

void myNoteOff(byte channel, byte note, byte velocity) {

  if (digitalRead(MONO_POLY1) == 1) {  //POLYPHONIC mode
    switch (getVoiceNo(note)) {
      case 1:
        env1.noteOff();
        filterEnv1.noteOff();
        lfoAenv1.noteOff();
        env1on = false;
        voices[0].note = -1;
        voiceOn[0] = false;
        break;
      case 2:
        env2.noteOff();
        filterEnv2.noteOff();
        lfoAenv2.noteOff();
        env2on = false;
        voices[1].note = -1;
        voiceOn[1] = false;
        break;
      case 3:
        env3.noteOff();
        filterEnv3.noteOff();
        lfoAenv3.noteOff();
        env3on = false;
        voices[2].note = -1;
        voiceOn[2] = false;
        break;
      case 4:
        env4.noteOff();
        filterEnv4.noteOff();
        lfoAenv4.noteOff();
        env4on = false;
        voices[3].note = -1;
        voiceOn[3] = false;
        break;
      case 5:
        env5.noteOff();
        filterEnv5.noteOff();
        lfoAenv5.noteOff();
        env5on = false;
        voices[4].note = -1;
        voiceOn[4] = false;
        break;
      case 6:
        env6.noteOff();
        filterEnv6.noteOff();
        lfoAenv6.noteOff();
        env6on = false;
        voices[5].note = -1;
        voiceOn[5] = false;
        break;
    }
  } else {
    env1.noteOff();
    filterEnv1.noteOff();
    lfoAenv1.noteOff();
    env1on = false;
    voices[0].note = -1;
    voiceOn[0] = false;
  }
}

