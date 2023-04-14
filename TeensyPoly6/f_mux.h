void checkMux() {

  mux1Read = adc->adc0->analogRead(muxPots1);
  mux2Read = adc->adc0->analogRead(muxPots2);
  mux3Read = adc->adc0->analogRead(muxPots3);
  mux4Read = adc->adc1->analogRead(muxPots4);

  if (mux1Read > (mux1ValuesPrev[muxInput] + QUANTISE_FACTOR) || mux1Read < (mux1ValuesPrev[muxInput] - QUANTISE_FACTOR)) {
    mux1ValuesPrev[muxInput] = mux1Read;

    switch (muxInput) {
      case 0:
        mux0 = mux1Read;
        break;
      case 1:
        mux1 = mux1Read;
        break;
      case 2:
        mux2 = mux1Read;
        break;
      case 3:
        mux3 = mux1Read;
        break;
      case 4:
        mux4 = mux1Read;
        break;
      case 5:
        mux5 = mux1Read;
        break;
      case 6:
        mux6 = mux1Read;
        break;
      case 7:
        mux7 = mux1Read;
        break;
    }
  }

  if (mux2Read > (mux2ValuesPrev[muxInput] + QUANTISE_FACTOR) || mux2Read < (mux2ValuesPrev[muxInput] - QUANTISE_FACTOR)) {
    mux2ValuesPrev[muxInput] = mux2Read;

    switch (muxInput) {
      case 0:
        mux8 = mux2Read;
        break;
      case 1:
        mux9 = mux2Read;
        break;
      case 2:
        mux10 = mux2Read;
        break;
      case 3:
        mux11 = mux2Read;
        break;
      case 4:
        mux12 = mux2Read;
        break;
      case 5:
        mux13 = mux2Read;
        break;
      case 6:
        mux14 = mux2Read;
        break;
      case 7:
        mux15 = mux2Read;
        break;
    }
  }

  if (mux3Read > (mux3ValuesPrev[muxInput] + QUANTISE_FACTOR) || mux3Read < (mux3ValuesPrev[muxInput] - QUANTISE_FACTOR)) {
    mux3ValuesPrev[muxInput] = mux3Read;

    switch (muxInput) {
      case 0:
        mux16 = mux3Read;
        break;
      case 1:
        mux17 = mux3Read;
        break;
      case 2:
        mux18 = mux3Read;
        break;
      case 3:
        mux19 = mux3Read;
        break;
      case 4:
        mux20 = mux3Read;
        break;
      case 5:
        mux21 = mux3Read;
        break;
      case 6:
        mux22 = mux3Read;
        break;
      case 7:
        mux23 = mux3Read;
        break;
    }
  }

  if (mux4Read > (mux4ValuesPrev[muxInput] + QUANTISE_FACTOR) || mux4Read < (mux4ValuesPrev[muxInput] - QUANTISE_FACTOR)) {
    mux4ValuesPrev[muxInput] = mux4Read;

    switch (muxInput) {
      case 0:
        mux24 = mux4Read;
        break;
      case 1:
        mux25 = mux4Read;
        break;
      case 2:
        mux26 = mux4Read;
        break;
      case 3:
        mux27 = mux4Read;
        break;
      case 4:
        mux28 = mux4Read;
        break;
      case 5:
        mux29 = mux4Read;
        break;
      case 6:
        mux30 = mux4Read;
        break;
      case 7:
        mux31 = mux4Read;
        break;
    }
  }

  muxInput++;
  if (muxInput >= MUXCHANNELS)
    muxInput = 0;

  digitalWriteFast(MUX1, muxInput & B0001);
  digitalWriteFast(MUX2, muxInput & B0010);
  digitalWriteFast(MUX3, muxInput & B0100);
}
