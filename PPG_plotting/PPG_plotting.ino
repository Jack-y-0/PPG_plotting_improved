#define PROGRAMME_NAME "PPG_plotting"
#define VERSION " V1 "  // Adding multiple splash OLED Screens and fixing IP displayed
//#define MODEL_NAME "Model: BUTTOM"
#define MODEL_NAME "Model: PPG_PMD"
#define DEVICE_UNDER_TEST "PPG Breadboard"
#define LICENSE "GNU Affero General Public License, version 3 "
#define ORIGIN "UK"

// ---------- Status LED ----------
#define LED_BUILTIN 2
#define TIME_HIGH 1000
#define TIME_LOW 500

// ---------- Pins ----------
// const uint8_t SENSOR_PIN = 15;  // chip physical pin 21
const uint8_t SENSOR_PIN = 32;  // GPIO32 proof of concept, breadboard
const uint8_t LED_PIN = 13;     // onboard LED
const uint8_t WIPER_PIN = 25;

// ---------- Settings ----------
const unsigned long SAMPLE_DELAY_MS = 10;   // ~100 Hz sampling
// const unsigned long SAMPLE_DELAY_MS = 20;   // ~50 Hz sampling
// const unsigned long SAMPLE_DELAY_MS = 40;  // ~25 Hz sampling

// Debounce / sanity limits (BPM range)
const unsigned long MIN_IBI_MS = 300;   // 200 BPM max
const unsigned long MAX_IBI_MS = 2000;  // 30 BPM min

// ---------- State ----------
int bpm = 0; // this will be calculated from a inter beat interval 
bool above = 0; // filter signal is above threshold

float signal_v = 0;
float threshold_v = 0;
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
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  while (!Serial){ // wait for UART0 to initialise
    ;
  }
  splashserial();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // Read sensor
  int rawSignal = analogRead(SENSOR_PIN);
  signal_v = (3.3 / 4096.0) * rawSignal;                       // signal in volts
  float threshold_v = (3.3 / 4096.0) * analogRead(WIPER_PIN);  // threshold in volts

  detect_beat();

  digital_signal_processing();  // EMA decaying max. and min.

  plot_the_data();

  delay(SAMPLE_DELAY_MS);
}