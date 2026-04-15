#define PROGRAMME_NAME "PPG_plotting"

// ---------- Pins ----------
// const uint8_t SENSOR_PIN = 15;  // chip physical pin 21
const uint8_t SENSOR_PIN = 32;     // GPIO32 proof of concept, breadboard
const uint8_t LED_PIN    = 13;     // onboard LED
const uint8_t Threshold_PIN = 25;

// ---------- Settings ----------
// int THRESHOLD = 1500;            // tune this (see Serial Plotter)
// int THRESHOLD = 2200;            // for Jack's finger

// const unsigned long SAMPLE_DELAY_MS = 10;   // ~100 Hz sampling
// const unsigned long SAMPLE_DELAY_MS = 20;   // ~50 Hz sampling
const unsigned long SAMPLE_DELAY_MS = 40;      // ~25 Hz sampling

// Debounce / sanity limits (BPM range)
const unsigned long MIN_IBI_MS = 300;          // 200 BPM max
const unsigned long MAX_IBI_MS = 2000;         // 30 BPM min

// ---------- State ----------
bool wasAbove = false;
unsigned long lastBeatMs = 0;
int bpm = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  Serial.println("PPG BPM on ESP32");
  Serial.println("Tip: Use Tools -> Serial Plotter.");
  Serial.println("Plot: Signal Threshold EMA Band1 Band2 BPM");
}

void loop() {
  unsigned long now = millis();

  // Read sensor
  int rawSignal = analogRead(SENSOR_PIN);
  float signal_v = (3.3 / 4096.0) * rawSignal;                   // signal in volts
  float threshold_v = (3.3 / 4096.0) * analogRead(Threshold_PIN); // threshold in volts

  static float signal_ema = threshold_v;
  float ALPHA = 0.01;

  // LED indicates "above threshold"
  bool above = (signal_v > (signal_ema + threshold_v));   // EMA + offset
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

  // If no beats for ~2.5 s, consider "no signal"
  if (lastBeatMs != 0 && (now - lastBeatMs) > 2500) {
    bpm = 0;
  }

  // Signal splitting into two bands for plotting
  float band1 = 0.0;
  float band2 = 0.0;

  // Range 1500–1800 (raw ADC)
  if (rawSignal >= 1900 && rawSignal <= 2000) {
    band1 = signal_v;
  }

  // Range 2500–3000 (raw ADC)
  if (rawSignal >= 3000 && rawSignal <= 3400) {
    band2 = signal_v;
  }

  // Update EMA
  signal_ema = ALPHA * signal_v + (1.0 - ALPHA) * signal_ema;

  // Serial Plotter output
  Serial.print("BPM:");
  Serial.print(bpm);

  Serial.print(" Min:");
  Serial.print(1.0);

  Serial.print(" Max:");
  Serial.print(3.3);

  Serial.print(" Signal:");
  Serial.print(signal_v);

  Serial.print(" Threshold:");
  Serial.print(threshold_v);

  Serial.print(" EMA:");
  Serial.print(signal_ema);

  Serial.print(" Band1:");
  Serial.print(band1);

  Serial.print(" Band2:");
  Serial.print(band2);

  // Serial.print(" BPM:");
  // Serial.println(bpm);

  Serial.println();
  delay(SAMPLE_DELAY_MS);
}