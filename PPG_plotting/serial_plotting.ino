void plot_the_data(void) {
  // Serial Plotter output
  Serial.print("BPM:");
  Serial.print(bpm);

  Serial.print(" Min:");
  Serial.print(1.0);

  Serial.print(" Max:");
  Serial.print(3.3);

  Serial.print(" Signal:");
  Serial.print(signal_v);

  // Serial.print(" Threshold:");
  // Serial.print(threshold_v);

  // Serial.print(" EMA:");
  // Serial.print(signal_ema);

  Serial.print(" EMA+Offset:");
  Serial.print(signal_ema + threshold_v);

  Serial.print(" emVar:");
  Serial.print(emVar);

  Serial.print(" decayingMax:");
  Serial.print(decayingMax);

  Serial.print(" decayingMin:");
  Serial.print(decayingMin);

  // Serial.print(" Band1:");
  // Serial.print(band1);

  // Serial.print(" Band2:");
  // Serial.print(band2);

  // Serial.print(" BPM:");
  // Serial.println(bpm);

  Serial.println();
}