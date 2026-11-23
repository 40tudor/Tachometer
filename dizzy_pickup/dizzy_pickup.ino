const int ignPin = 2;      // Optocoupler input (Interrupt 0)
const int gaugePin = 9;    // PWM output to moving-coil gauge

// V8 Distributor & 10k Gauge Settings
const int pulsesPerRev = 4;    // 8-cylinder 4-stroke = 4 sparks per rev
const int maxRPM = 10000;      // 10,000 RPM gauge face

volatile unsigned long lastPulseTime = 0;
volatile unsigned long pulseInterval = 0;
unsigned long lastUpdate = 0;

void countPulse() {
  unsigned long now = micros();
  unsigned long interval = now - lastPulseTime;
  
  // Debounce: 1000us allows up to 15,000 RPM on a V8 while blocking coil ringing
  if (interval > 1000) { 
    pulseInterval = interval;
    lastPulseTime = now;
  }
}

void setup() {
  pinMode(ignPin, INPUT_PULLUP);
  pinMode(gaugePin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ignPin), countPulse, FALLING);
}

void loop() {
  // Update needle every 40ms (25Hz) for smooth response
  if (millis() - lastUpdate >= 40) {
    lastUpdate = millis();

    noInterrupts();
    unsigned long interval = pulseInterval;
    unsigned long timeSinceLast = micros() - lastPulseTime;
    interrupts();

    int rpm = 0;
    // Engine running check (pulse received within last 0.5 seconds)
    if (interval > 0 && timeSinceLast < 500000) {
      rpm = (60000000UL / interval) / pulsesPerRev;
    }

    // Constrain and map RPM to 0-255 PWM duty cycle
    rpm = constrain(rpm, 0, maxRPM);
    int pwmValue = map(rpm, 0, maxRPM, 0, 255);
    
    analogWrite(gaugePin, pwmValue);
  }
}

