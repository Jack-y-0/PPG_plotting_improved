// ---------- State ----------
bool wasAbove = false;
unsigned long lastBeatMs = 0;
const unsigned long millsPerSec = 1000;
const unsigned long secPerMin = 60;
float noSignalT = 2.5; // signal time(s) for flag: no beat detected
float percentMinMaxThreshold = 80.0/100.0; // empirically chosen

void detect_beat(void) {
  unsigned long now = millis();
  // LED indicates "above threshold"
  // bool above = (signal_v > (signal_ema + threshold_v));   // EMA + offset
  above = (signal_v > (decayingMin + (percentMinMaxThreshold * (decayingMax - decayingMin))));  // minimum + 80% of (max - min)
  digitalWrite(LED_PIN, above ? HIGH : LOW);

  // Beat detection: rising edge across threshold
  if (above && !wasAbove) {
    unsigned long ibi = now - lastBeatMs; // inter beat interval

    if (lastBeatMs != 0 && ibi >= MIN_IBI_MS && ibi <= MAX_IBI_MS) {
      int newBpm = (int)(secPerMin * millsPerSec / ibi); // BPM = 1 minute / milliseconds per beat
      //newBPM units: beats per minute = (60,000 (milliseconds/minute)) / (inner beat interval (milliseconds/beat))

      // Smooth BPM (reduce jitter)
      if (bpm == 0) bpm = newBpm;
      else bpm = (bpm * 3 + newBpm) / 4; // four point exponential moving average?

      // Serial.print("Beat! BPM=");
      // Serial.println(bpm);
    }

    lastBeatMs = now;
  }

  wasAbove = above;

  // If no beats for noSignalT (s), consider "no signal"
  if (lastBeatMs != 0 && (now - lastBeatMs) > noSignalT * millsPerSec) {
    bpm = NAN;
  }
}