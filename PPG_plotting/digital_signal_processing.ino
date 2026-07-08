//Use Exponential Moving Average (EMA) Low-Pass Filter for digital signal processing (DSP)
// -------------------------------------------------------------------
// Formula: EMA_new = (α * New_Value) + ((1 - α) * EMA_old)
//
// This acts as a recursive low-pass filter that smooths sensor noise 
// while retaining responsiveness to real signal changes.
//
// Parameters:
// - ALPHA (α): Smoothing factor (0.0 < α ≤ 1.0). 
//   Higher α = faster response, less smoothing (more noise).
//   Lower α = slower response, more smoothing (less noise).
//   Calculated as: α = 2.0 / (N + 1.0), where N is the desired period.
//
// State:
// - 'signal_ema' stores the previous output value. It MUST be declared 
//   as 'static' or 'global' to persist between loop iterations.
//
// Initialization:
// - On the first run, 'signal_ema' should be set equal to 'signal_v' 
//   to prevent a long warm-up ramp from zero.

float ALPHA = 0.01;
void digital_signal_processing(void) {
  // Update EMA, exponential moving average 
  signal_ema = ALPHA * signal_v + (1.0 - ALPHA) * signal_ema;

  // emVar = (1-alpha)*emVar + alpha * (signal - EMA)^2
  emVar = ((1.0-ALPHA) * emVar) + ALPHA * (signal_v - signal_ema) * (signal_v - signal_ema);
  // emVar = (1.0-ALPHA) * (emVar + ALPHA * (signal_v - signal_ema) * (signal_v - signal_ema));
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