FLASHMEM void updateParams() {

  mainVol = (float)mux23 / 1024;

  bend = 1 + (newpitchbend / 1023 / 4.3) - 0.12;


  //main octave
  if (digitalRead(MAIN_OCT_1) == 1) {
    octave = 0.5;
  } else if (digitalRead(MAIN_OCT_1) == 0 && digitalRead(MAIN_OCT_2) == 0) {
    octave = 1;
  } else if (digitalRead(MAIN_OCT_2) == 1) {
    octave = 2;
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  // PRESET MODE ///////// PRESET MODE //////// PRESET MODE //////////// PRESET MODE /////// PRESET MODE ////
  // PRESET MODE ///////// PRESET MODE //////// PRESET MODE //////////// PRESET MODE /////// PRESET MODE ////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  ///////////////  OCTAVES OCTAVES /////////////////////////////////////////////////////////////7////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////


  //octave vco B
  if (digitalRead(B_OCTAVE_1) == 1) {
    octBsw = 0;
  } else if (digitalRead(B_OCTAVE_1) == 0 && digitalRead(B_OCTAVE_2) == 0) {
    octBsw = 1;
  } else if (digitalRead(B_OCTAVE_2) == 1) {
    octBsw = 2;
  }
  if (oldOctBsw < octBsw || oldOctBsw > octBsw) {
    if (digitalRead(B_OCTAVE_1) == 1) {
      octaveB = 0.5;
    } else if (digitalRead(B_OCTAVE_1) == 0 && digitalRead(B_OCTAVE_2) == 0) {
      octaveB = 1;
    } else if (digitalRead(B_OCTAVE_2) == 1) {
      octaveB = 2;
    }
    oldOctBsw = octBsw;
    //Serial.println("octave B switch");
  }



  //octave vco C
  if (digitalRead(C_OCTAVE_1) == 1) {
    octCsw = 0;
  } else if (digitalRead(C_OCTAVE_1) == 0 && digitalRead(C_OCTAVE_2) == 0) {
    octCsw = 1;
  } else if (digitalRead(C_OCTAVE_2) == 1) {
    octCsw = 2;
  }
  if (oldOctCsw < octCsw || oldOctCsw > octCsw) {
    if (digitalRead(C_OCTAVE_1) == 1) {
      octaveC = 0.5;
    } else if (digitalRead(C_OCTAVE_1) == 0 && digitalRead(C_OCTAVE_2) == 0) {
      octaveC = 1;
    } else if (digitalRead(C_OCTAVE_2) == 1) {
      octaveC = 2;
    }
    oldOctCsw = octCsw;
    //Serial.println("octave C switch");
  }


  //////////// SHAPES SHAPES ////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  //Shape A
  if (digitalRead(A_SHAPE_1) == 1) {
    shapeAsw = 0;
  } else if (digitalRead(A_SHAPE_1) == 0 && digitalRead(A_SHAPE_2) == 0) {
    shapeAsw = 1;
  } else if (digitalRead(A_SHAPE_2) == 1) {
    shapeAsw = 2;
  }
  if (oldShapeAsw < shapeAsw || oldShapeAsw > shapeAsw) {
    if (digitalRead(A_SHAPE_1) == 1) {
      shapeA = 0;
    } else if (digitalRead(A_SHAPE_1) == 0 && digitalRead(A_SHAPE_2) == 0) {
      shapeA = 1;
    } else if (digitalRead(A_SHAPE_2) == 1) {
      shapeA = 2;
    }
    oldShapeAsw = shapeAsw;
    //Serial.println("shape A switch");
  }



  //Shape B
  if (digitalRead(B_SHAPE_1) == 1) {
    shapeBsw = 0;
  } else if (digitalRead(B_SHAPE_1) == 0 && digitalRead(B_SHAPE_2) == 0) {
    shapeBsw = 1;
  } else if (digitalRead(B_SHAPE_2) == 1) {
    shapeBsw = 2;
  }
  if (oldShapeBsw < shapeBsw || oldShapeBsw > shapeBsw) {
    if (digitalRead(B_SHAPE_1) == 1) {
      shapeB = 0;
    } else if (digitalRead(B_SHAPE_1) == 0 && digitalRead(B_SHAPE_2) == 0) {
      shapeB = 1;
    } else if (digitalRead(B_SHAPE_2) == 1) {
      shapeB = 2;
    }
    oldShapeBsw = shapeBsw;
    //Serial.println("shape B switch");
  }




  //Vco C shape
  shapeCpot = mux11;
  if (oldShapeCpot + tresh2 < shapeCpot || oldShapeCpot - tresh2 > shapeCpot) {
    shapeC = mux11;
    oldShapeCpot = shapeCpot + tresh2 / 2;
    //Serial.println("shape C turn");
  }




  /////////// TUNINGS TUNINGS //////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  //tuneB
  tuneBpot = mux13;
  if (oldTuneBpot + tresh < tuneBpot || oldTuneBpot - tresh > tuneBpot) {
    if (mux13 < 512) {
      tuneB = ((float)mux13 / 1023) + 0.5;
    } else {
      tuneB = ((float)mux13 / 510);
    }
    oldTuneBpot = tuneBpot + tresh / 2;
    //Serial.println("tuneB turn");
  }

  //tuneC
  tuneCpot = mux12;
  if (oldTuneCpot + tresh < tuneCpot || oldTuneCpot - tresh > tuneCpot) {
    if (mux12 < 512) {
      tuneC = ((float)mux12 / 1023) + 0.5;
    } else {
      tuneC = ((float)mux12 / 510);
    }
    oldTuneCpot = tuneCpot + tresh / 2;
    //Serial.println("tuneC turn");
  }

  //Cross mod
  crossModpot = mux14;
  if (oldCrossModpot + tresh < crossModpot || oldCrossModpot - tresh > crossModpot) {
    crossMod = (float)mux14 / 512;
    oldCrossModpot = crossModpot + tresh / 2;
    //Serial.println("crossmod turn");
  }




  ///////////// VOLUMES VOLUMES /////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  volApot = mux10;
  if (oldVolApot + tresh2 < volApot || oldVolApot - tresh2 > volApot) {
    vcoAvol = (float)mux10 / 1023;
    oldVolApot = volApot + tresh2 / 2;
    //Serial.println("volA turn");
  }


  volBpot = mux9;
  if (oldVolBpot + tresh2 < volBpot || oldVolBpot - tresh2 > volBpot) {
    vcoBvol = (float)mux9 / 1023;
    oldVolBpot = volBpot + tresh2 / 2;
    //Serial.println("volB turn");
  }

  volCpot = mux8;
  if (oldVolCpot + tresh2 < volCpot || oldVolCpot - tresh2 > volCpot) {
    vcoCvol = (float)mux8 / 1023;
    oldVolCpot = volCpot + tresh2 / 2;
    //Serial.println("volC turn");
  }

  volSubpot = mux17;
  if (oldVolSubpot + tresh2 < volSubpot || oldVolSubpot - tresh2 > volSubpot) {
    Subvol = (float)mux17 / 1023;
    oldVolSubpot = volSubpot + tresh2 / 2;
    //Serial.println("vol sub turn");
  }

  //////////// FILTER FILTER ////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  //Filter
  cutpot = mux25;
  if (oldCutpot + tresh < cutpot || oldCutpot - tresh > cutpot) {
    cut = 15000 * (float)mux25 / 1023 + 15;  /////cut
    oldCutpot = cutpot + tresh / 2;
    //Serial.println("cut turn");
  }

  respot = mux24;
  if (oldRespot + tresh2 < respot || oldRespot - tresh2 > respot) {
    res = 4.5 * (float)mux24 / 1023 + 1.1;
    oldRespot = respot + tresh2 / 2;
    //Serial.println("res turn");
  }

  //Filter Env

  fAttpot = mux0;
  if (oldFAttpot + tresh2 < fAttpot || oldFAttpot - tresh2 > fAttpot) {
    filtAtt = (3000 * (float)mux0 / 1023);
    oldFAttpot = fAttpot + tresh2 / 2;
    //Serial.println("filter attack turn");
  }

  fDecpot = mux1;
  if (oldFDecpot + tresh2 < fDecpot || oldFDecpot - tresh2 > fDecpot) {
    filtDec = (3000 * (float)mux1 / 1023);
    oldFDecpot = fDecpot + tresh2 / 2;
    //Serial.println("filter decay turn");
  }

  fAmtpot = mux2;
  if (oldFAmtpot + tresh2 < fAmtpot || oldFAmtpot - tresh2 > fAmtpot) {
    filtAmt = (float)mux2 / 512 - 1;
    oldFAmtpot = fAmtpot + tresh2 / 2;
    //Serial.println("filter Amt turn");
  }



  //FilterMode
  if (digitalRead(FILTER_MODE) == 1) {
    filtModesw = 1;
  } else if (digitalRead(FILTER_MODE) == 0) {
    filtModesw = 0;
  }
  if (oldFiltModesw < filtModesw || oldFiltModesw > filtModesw) {
    if (digitalRead(FILTER_MODE) == 1) {
      filterMode = 1;
    } else if (digitalRead(FILTER_MODE) == 0) {
      filterMode = 0;
    }
    oldFiltModesw = filtModesw;
    //Serial.println("filter mode switch");
  }



  /////////////// MAIN ENVELOPE ENVELOPE ////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  attpot = mux27;
  if (oldAttpot + tresh2 < attpot || oldAttpot - tresh2 > attpot) {
    envAtt = 3000 * (float)mux27 / 1023;
    oldAttpot = attpot + tresh2 / 2;
    //Serial.println("Attack turn");
  }

  decpot = mux26;
  if (oldDecpot + tresh2 < decpot || oldDecpot - tresh2 > decpot) {
    envDec = 5000 * (float)mux26 / 1023;
    envRel = 5000 * (float)mux26 / 1023;
    oldDecpot = decpot + tresh2 / 2;
    //Serial.println("Decay turn");
  }

  suspot = mux22;
  if (oldSuspot + tresh2 < suspot || oldSuspot - tresh2 > suspot) {
    envSus = (float)mux22 / 100;
    oldSuspot = suspot + tresh2 / 2;
    //Serial.println("Sustain turn");
  }





  ////////////// LFO A   LFO A  /////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  lfoAmppot = mux3 + midiMod;
  //Serial.println(lfoAmppot);
  if (oldLfoAmppot + tresh2 < lfoAmppot || oldLfoAmppot - tresh2 > lfoAmppot) {
    if (lfoAdest == 0 && lfoAshape != 2) {
      lfoAamp = ((float)mux3 + midiMod) / 1024 / 10;
    } else {
      lfoAamp = ((float)mux3 + midiMod) / 1024 / 3;
    }
    oldLfoAmppot = lfoAmppot + tresh2 / 2;
    //Serial.println("Lfo A amp turn");
  }

  lfoFreqpot = mux4;
  if (oldLfoFreqpot + tresh2 < lfoFreqpot || oldLfoFreqpot - tresh2 > lfoFreqpot) {
    lfoAfreq = 20 * (float)mux4 / 1024 + 0.1;
    oldLfoFreqpot = lfoFreqpot + tresh2 / 2;
    //Serial.println("Lfo A Freq turn");
  }

  lfoAttpot = mux5;
  if (oldLfoAttpot + tresh2 < lfoAttpot || oldLfoAttpot - tresh2 > lfoAttpot) {
    lfoAdel = 2000 * (float)mux5 / 1024;
    lfoAatt = 3000 * (float)mux5 / 1024;
    oldLfoAttpot = lfoAttpot + tresh2 / 2;
    Serial.println("Lfo A Att turn");
  }

  lfoDecpot = mux6;
  if (oldLfoDecpot + tresh2 < lfoDecpot || oldLfoDecpot - tresh2 > lfoDecpot) {
    lfoAdec = 4000 * (float)mux6 / 1024;
    lfoArel = 4000 * (float)mux6 / 1024;
    oldLfoDecpot = lfoDecpot + (tresh2 / 2);
    //Serial.println("Lfo A Dec turn");
  }

  lfoSuspot = mux7;
  if (oldLfoSuspot + tresh2 * 2 < lfoSuspot || oldLfoSuspot - tresh2 * 2 > lfoSuspot) {
    lfoAsus = (float)mux7 / 1024;
    oldLfoSuspot = lfoSuspot + ((tresh2 * 2) / 2);
    //Serial.println("Lfo A Sus turn");
  }


  //lfoA destination
  if (digitalRead(LFOA_DEST_1) == 1) {  //lfo - pitch
    lfoDestsw = 0;
  } else if (digitalRead(LFOA_DEST_1) == 0 && digitalRead(LFOA_DEST_2) == 0) {  //lfo - filter
    lfoDestsw = 1;
  } else if (digitalRead(LFOA_DEST_2) == 1) {  //lfo - amp
    lfoDestsw = 2;
  }
  if (oldLfoDestsw < lfoDestsw || oldLfoDestsw > lfoDestsw) {
    if (digitalRead(LFOA_DEST_1) == 1) {  //lfo - pitch
      lfoAdest = 0;
    } else if (digitalRead(LFOA_DEST_1) == 0 && digitalRead(LFOA_DEST_2) == 0) {  //lfo - filter
      lfoAdest = 1;
    } else if (digitalRead(LFOA_DEST_2) == 1) {  //lfo - amp
      lfoAdest = 2;
    }
    oldLfoDestsw = lfoDestsw;
    //Serial.println("Lfo dest switch");
  }



  //lfoA shape
  if (digitalRead(LFOA_SHAPE_1) == 1) {
    lfoShapesw = 0;
  } else if (digitalRead(LFOA_SHAPE_1) == 0 && digitalRead(LFOA_SHAPE_2) == 0) {
    lfoShapesw = 1;
  } else if (digitalRead(LFOA_SHAPE_2) == 1) {
    lfoShapesw = 2;
  }

  if (oldLfoShapesw < lfoShapesw || oldLfoShapesw > lfoShapesw) {
    if (digitalRead(LFOA_SHAPE_1) == 1) {
      lfoAshape = 0;
    } else if (digitalRead(LFOA_SHAPE_1) == 0 && digitalRead(LFOA_SHAPE_2) == 0) {
      lfoAshape = 1;
    } else if (digitalRead(LFOA_SHAPE_2) == 1) {
      lfoAshape = 2;
    }
    oldLfoShapesw = lfoShapesw;
    //Serial.println("Lfo shape switch");
  }



  ///////////// LFO B    LFO B //////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  lfoBAmppot = mux15;
  if (oldLfoBAmppot + tresh2 < lfoBAmppot || oldLfoBAmppot - tresh2 > lfoBAmppot) {
    lfoBamp = (float)mux15 / 1023;
    oldLfoBAmppot = lfoBAmppot + tresh2 / 2;
    //Serial.println("Lfo B amp turn");
  }

  lfoBFreqpot = mux16;
  if (oldLfoBFreqpot + tresh2 < lfoBFreqpot || oldLfoBFreqpot - tresh2 > lfoBFreqpot) {
    lfoBfreq = 5 * (float)mux16 / 1023 + 0.1;
    oldLfoBFreqpot = lfoBFreqpot + tresh2 / 2;
    Serial.println("Lfo B freq turn");
  }






  //////////////// FX FX FX FX //////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  //Delay
  dlyAmtpot = mux21;
  if (oldDlyAmtpot + tresh2 < dlyAmtpot || oldDlyAmtpot - tresh2 > dlyAmtpot) {
    dlyAmt = (float)mux21 / 1100 - 0.1;
    if (dlyAmt < 0) {
      dlyAmt = 0;
    }
    oldDlyAmtpot = dlyAmtpot + tresh2 / 2;
    //Serial.println("Dly amt turn");
  }


  dlyTimepot = mux20;
  if (oldDlyTimepot + tresh2 < dlyTimepot || oldDlyTimepot - tresh2 > dlyTimepot) {
    dlyTimeL = mux20 / 2.5;
    dlyTimeR = mux20 / 1.25;
    oldDlyTimepot = dlyTimepot + tresh2 / 2;
    //Serial.println("Dly time turn");
  }



  //Reverb
  revMixpot = mux18;
  if (oldRevMixpot + tresh2 < revMixpot || oldRevMixpot - tresh2 > revMixpot) {
    revMix = ((float)mux18 / 1024 / 1.2);
    oldRevMixpot = revMixpot + tresh2 / 2;
    //Serial.println("Rev mix turn");
  }


  revSizepot = mux19;
  if (oldRevSizepot + tresh2 < revSizepot || oldRevSizepot - tresh2 > revSizepot) {
    revSize = ((float)mux19 / 1024 - 0.01);
    oldRevSizepot = revSizepot + tresh2 / 2;
    //Serial.println("Rev size turn");
  }
}
