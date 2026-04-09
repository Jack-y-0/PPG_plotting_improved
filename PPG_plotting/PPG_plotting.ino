#define PROGRAMME_NAME "PPG_plotting"




// ---------- Pins ----------
// const uint8_t SENSOR_PIN = 15;  // chip physical pin 21
// const uint8_t SENSOR_PIN = 32;  // GPIO32 proof of concept, breadboard 
const uint8_t LED_PIN    = 13;  // onboard LED (change if you want)

// Replacing the current read with this (so you have raw + voltage):
int rawSignal = analogRead(SENSOR_PIN);
float signal = (3.3 / 4096.0) * rawSignal;
// ---------- Settings ----------
// int THRESHOLD = 1500;            // tune this (see Serial Plotter)
// int THRESHOLD = 2200;            // for Jack's finger
const uint8_t Threshold_PIN = 25;

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
  float signal = (3.3 / 4096.0) * analogRead(SENSOR_PIN); // voltage
  float THRESHOLD = (3.3 / 4096.0) * analogRead(Threshold_PIN); // threshold V
  static float signal_ema = THRESHOLD;
  float ALPHA = 0.01;

  // LED indicates "above threshold"
  bool above = (signal > signal_ema); //threshold is replaced with ema
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


  // signal splitting into two bands for plotting
float band1 = 0.0;
float band2 = 0.0;

// Range 1500–1800 (raw ADC)
if (rawSignal >= 1500 && rawSignal <= 1800) {
  band1 = signal;
}

// Range 2500–3000 (raw ADC)
if (rawSignal >= 2500 && rawSignal <= 3000) {
  band2 = signal;
}



  Serial.print("Min:1"); // plot baseline
  Serial.print(" ");
  Serial.print("Max:3.3");// plot a maximum to prevent auto scaling
  Serial.print(" Signal:");
  Serial.print(signal);
  Serial.print(" Threshold:");
  Serial.print(THRESHOLD);

  Serial.print(" EMA:");
  // return last = alpha * input + (1 - alpha) * last;
  signal_ema = (ALPHA) * signal + (1 - ALPHA) * signal_ema;
  Serial.print(signal_ema);

  Serial.print(" BPM:");
  Serial.println(bpm);

// For plotting two bands separately (e.g., for a stacked plot):
Serial.print(" Band1:");
Serial.print(band1);

Serial.print(" Band2:");
Serial.print(band2);


  delay(SAMPLE_DELAY_MS);
}
