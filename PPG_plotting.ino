#include <Arduino.h>

// ---------- Pins ----------
const uint8_t SENSOR_PIN = 15;  // chip physical pin 21
const uint8_t LED_PIN    = 13;  // onboard LED (change if you want)

// ---------- Settings ----------
int THRESHOLD = 1500;            // tune this (see Serial Plotter)
// const unsigned long SAMPLE_DELAY_MS = 10;   // ~100 Hz sampling
// const unsigned long SAMPLE_DELAY_MS = 20;   // ~50 Hz sampling
const unsigned long SAMPLE_DELAY_MS = 40;   // ~25 Hz sampling

// Debounce / sanity limits (BPM range)
const unsigned long MIN_IBI_MS = 300;   // 200 BPM max
const unsigned long MAX_IBI_MS = 2000;  // 30 BPM min

// ---------- State ----------
bool wasAbove = false;
unsigned long lastBeatMs = 0;
int bpm = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  Serial.println("PPG BPM on Arduino Nano (A7 = chip pin 21)");
  Serial.println("Tip: Use Tools -> Serial Plotter.");
  Serial.println("Plot: signal threshold bpm");
}

void loop() {
  unsigned long now = millis();

  // Read sensor
  int signal = analogRead(SENSOR_PIN);

  // LED indicates "above threshold"
  bool above = (signal > THRESHOLD);
  digitalWrite(LED_PIN, above ? HIGH : LOW);

  // Beat detection: rising edge across threshold
  if (above && !wasAbove) {
    unsigned long ibi = now - lastBeatMs;

    if (lastBeatMs != 0 && ibi >= MIN_IBI_MS && ibi <= MAX_IBI_MS) {
      int newBpm = (int)(60000UL / ibi);

      // Smooth BPM (reduce jitter)
      if (bpm == 0) bpm = newBpm;
      else bpm = (bpm * 3 + newBpm) / 4;

      Serial.print("Beat! BPM=");
      Serial.println(bpm);
    }

    lastBeatMs = now;
  }

  wasAbove = above;

  // If no beats for ~2.5s, consider "no signal"
  if (lastBeatMs != 0 && (now - lastBeatMs) > 2500) {
    bpm = 0;
  }
  Serial.print(0); // plot baseline
  Serial.print(' ');
  Serial.print(4000);// plot a maximum to prevent auto scaling
  Serial.print(' ');
  Serial.print(signal);
  Serial.print(' ');
  Serial.print(THRESHOLD);
  Serial.print(' ');
  Serial.println(bpm);

  delay(SAMPLE_DELAY_MS);
}
