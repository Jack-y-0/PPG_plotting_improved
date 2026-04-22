#define PROGRAMME_NAME "PPG_plotting"
#define VERSION " V1 "  /// Adding multiple splash OLED Screens and fixing IP displayed
//#define MODEL_NAME "Model: BUTTOM"
#define MODEL_NAME "Model: PPG_PMD"
#define DEVICE_UNDER_TEST "PPG Breadboard"
#define LICENSE "GNU Affero General Public License, version 3 "
#define ORIGIN "UK"
#define LED_BUILTIN 2
#define TIME_HIGH 1000
#define TIME_LOW 500

// ---------- Pins ----------
// const uint8_t SENSOR_PIN = 15;  // chip physical pin 21
const uint8_t SENSOR_PIN = 32;  // GPIO32 proof of concept, breadboard
const uint8_t LED_PIN = 13;     // onboard LED
const uint8_t WIPER_PIN = 25;

// ---------- Settings ----------
// int THRESHOLD = 1500;            // tune this (see Serial Plotter)
// int THRESHOLD = 2200;            // for Jack's finger

// const unsigned long SAMPLE_DELAY_MS = 10;   // ~100 Hz sampling
// const unsigned long SAMPLE_DELAY_MS = 20;   // ~50 Hz sampling
const unsigned long SAMPLE_DELAY_MS = 40;  // ~25 Hz sampling

// Debounce / sanity limits (BPM range)
const unsigned long MIN_IBI_MS = 300;   // 200 BPM max
const unsigned long MAX_IBI_MS = 2000;  // 30 BPM min

// ---------- State ----------
bool wasAbove = false;
unsigned long lastBeatMs = 0;
int bpm = 0;

float signal_v = 0;
float threshold_v =0;
static float signal_ema = threshold_v;
static float emVar = 0;
static float decayingMax = 0;
static float decayingMin = 3.3;  // Max for 10-bit ADC
float ALPHA = 0.01;

void splashserial() {
  Serial.println(F("==================================="));
  Serial.print(PROGRAMME_NAME);
  Serial.println(VERSION);
  Serial.println(MODEL_NAME);
  Serial.println(DEVICE_UNDER_TEST);
  Serial.print(F("Compiled at: "));
  Serial.println(F(__DATE__ " " __TIME__));
  Serial.println(LICENSE);
  Serial.println(F("==================================="));
  Serial.println();
}

void setup() {
  splashserial();
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  Serial.println("PPG BPM on ESP32");
  Serial.println("Tip: Use Tools -> Serial Plotter.");
  Serial.println("Plot: Signal Threshold EMA Band1 Band2 BPM");

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  unsigned long now = millis();

  // Read sensor
  int rawSignal = analogRead(SENSOR_PIN);
  signal_v = (3.3 / 4096.0) * rawSignal;                 // signal in volts
  float threshold_v = (3.3 / 4096.0) * analogRead(WIPER_PIN);  // threshold in volts

  // LED indicates "above threshold"
  // bool above = (signal_v > (signal_ema + threshold_v));   // EMA + offset
  bool above = (signal_v > (decayingMin + (0.8 * (decayingMax - decayingMin))));  // minimum + 80% of (max - min)
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


  digital_signal_processing(); // EMA decaying max. and min.

  plot_the_data(); 

  delay(SAMPLE_DELAY_MS);
}