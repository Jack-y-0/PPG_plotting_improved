void digital_signal_processing(void) {
  // Update EMA
  signal_ema = ALPHA * signal_v + (1.0 - ALPHA) * signal_ema;
  
  // emVar = (1-alpha)*emVar + alpha * (signal - EMA)^2
  emVar = ((1.0-ALPHA) * emVar) + ALPHA * (signal_v - signal_ema) * (signal_v - signal_ema);

  // Update Decaying Max
  if (signal_v > decayingMax) {
    decayingMax = signal_v; // Jump to new peak
  } else {
    decayingMax -= ALPHA * (decayingMax - signal_v); // Slowly decay
  }

  // Update Decaying Min
  if (signal_v < decayingMin) {
    decayingMin = signal_v; // Jump to new valley
  } else {
    decayingMin += ALPHA * (signal_v - decayingMin); // Slowly decay
  }

}