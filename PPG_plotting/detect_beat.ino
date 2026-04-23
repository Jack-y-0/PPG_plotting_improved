// ---------- State ----------
bool wasAbove = false;
unsigned long lastBeatMs = 0;

void detect_beat(void) {
  unsigned long now = millis();
  // LED indicates "above threshold"
  // bool above = (signal_v > (signal_ema + threshold_v));   // EMA + offset
  above = (signal_v > (decayingMin + (0.8 * (decayingMax - decayingMin))));  // minimum + 80% of (max - min)
  digitalWrite(LED_PIN, above ? HIGH : LOW);

  // Beat detection: rising edge across threshold
  if (above && !wasAbove) {
    unsigned long ibi = now - lastBeatMs; // inter beat interval

    if (lastBeatMs != 0 && ibi >= MIN_IBI_MS && ibi <= MAX_IBI_MS) {
      int newBpm = (int)(60000UL / ibi); // BPM = 1 minute / milliseconds per beat

      // Smooth BPM (reduce jitter)
      if (bpm == 0) bpm = newBpm;
      else bpm = (bpm * 3 + newBpm) / 4;

      // Serial.print("Beat! BPM=");
      // Serial.println(bpm);
    }

    lastBeatMs = now;
  }

  wasAbove = above;

  // If no beats for ~2.5 s, consider "no signal"
  if (lastBeatMs != 0 && (now - lastBeatMs) > 2500) {
    bpm = 0;
  }
}