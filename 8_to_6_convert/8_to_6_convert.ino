// ============================================================================
// 8-CYLINDER TO 6-CYLINDER (3/4 FREQUENCY) TACHOMETER CONVERTER
// Hardware: Arduino Micro (ATmega32U4)
// Input:    Pin 3 (INT0) <- 4N35 Optocoupler Collector (4 pulses/rev)
// Output:   Pin 9        -> 2N3904 Base Resistor (3 pulses/rev to Sun FZ106N)
// ============================================================================

const uint8_t TACH_IN_PIN  = 3;
const uint8_t TACH_OUT_PIN = 9;

// Spark-ring debounce & timeout settings
// At 10,000 RPM on an 8-cyl (666.7 Hz), pulse period is 1,500 us.
// 900 us blocks ignition coil ringing while safely reading past 12,000 RPM.
const unsigned long MIN_PULSE_INTERVAL_US = 900;
const unsigned long ENGINE_STOP_TIMEOUT_US = 250000; // 0.25s (~60 RPM cutoff)

volatile unsigned long lastSparkMicros = 0;
volatile unsigned long measuredPeriodUs = 0;
volatile bool newPulseFlag = false;

unsigned long lastToggleMicros = 0;
bool outPinState = LOW;

void onSparkISR() {
  unsigned long now = micros();
  unsigned long dt = now - lastSparkMicros;

  if (dt >= MIN_PULSE_INTERVAL_US) {
    measuredPeriodUs = dt;
    lastSparkMicros = now;
    newPulseFlag = true;
  }
}

void setup() {
  pinMode(TACH_IN_PIN, INPUT); // External 10k pull-up (R3) installed
  pinMode(TACH_OUT_PIN, OUTPUT);
  digitalWrite(TACH_OUT_PIN, LOW);

  // 4N35 pulls Pin 3 LOW during spark and releases HIGH (RISING) when spark ends
  attachInterrupt(digitalPinToInterrupt(TACH_IN_PIN), onSparkISR, RISING);
}

void loop() {
  unsigned long now = micros();
  unsigned long currentPeriod;
  unsigned long lastSpark;

  // Safely copy volatile ISR variables
  noInterrupts();
  currentPeriod = measuredPeriodUs;
  lastSpark = lastSparkMicros;
  interrupts();

  // Check if engine is cranking/running
  if (currentPeriod == 0 || (now - lastSpark) > ENGINE_STOP_TIMEOUT_US) {
    // Engine stopped: hold output LOW (transistor Q1 OFF -> tach sees steady 12V, 0 RPM)
    outPinState = LOW;
    digitalWrite(TACH_OUT_PIN, LOW);
    return;
  }

  // Frequency_out = (3/4) * Frequency_in  ==>  Period_out = (4/3) * Period_in
  // Half-period for 50% duty cycle toggle = (Period_out / 2) = (2/3) * Period_in
  unsigned long halfPeriodOutUs = (currentPeriod * 2UL) / 3UL;

  if ((now - lastToggleMicros) >= halfPeriodOutUs) {
    lastToggleMicros = now;
    outPinState = !outPinState;
    digitalWrite(TACH_OUT_PIN, outPinState);
  }
}