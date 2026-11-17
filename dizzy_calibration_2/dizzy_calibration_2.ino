// Pin definitions
const byte pwmPin = 9;      // Connected to RC filter driving galvanometer

// Configuration for your V8, 10,000 RPM tachometer
const float maxRPM = 10000.0; 

void setup() {
  pinMode(pwmPin, OUTPUT);
  Serial.begin(9600);
  
  while (!Serial); // Wait for Serial Monitor to open on Arduino Micro
  
  printMenu();
}

void loop() {
  if (Serial.available() > 0) {
    char choice = Serial.read();
    
    // Ignore newline and carriage return characters
    if (choice == '\n' || choice == '\r') return;

    float targetRPM = 0;
    
    switch (choice) {
      case '0':
        targetRPM = 0.0;
        break;
      case '1':
        targetRPM = 2500.0; // 25% scale
        break;
      case '2':
        targetRPM = 5000.0; // 50% scale (Mid-scale check)
        break;
      case '3':
        targetRPM = 7500.0; // 75% scale
        break;
      case '4':
        targetRPM = 10000.0; // 100% full-scale deflection
        break;
      case 'r':
      case 'R':
        runSweep();
        printMenu();
        return;
      default:
        Serial.println(F("Invalid selection. Choose 0-4 or 'r'."));
        return;
    }

    setRPM(targetRPM);
  }
}

// Sets the PWM to hit a specific RPM target
void setRPM(float rpm) {
  float constrainedRPM = constrain(rpm, 0, maxRPM);
  int pwmValue = map(constrainedRPM, 0, maxRPM, 0, 255);
  
  analogWrite(pwmPin, pwmValue);
  
  Serial.print(F("Commanding Target: "));
  Serial.print(constrainedRPM);
  Serial.print(F(" RPM (PWM Value: "));
  Serial.print(pwmValue);
  Serial.println(F(")/255"));
}

// Automatically sweeps the needle up and down for visual inspection
void runSweep() {
  Serial.println(F("\nRunning automatic sweep test..."));
  
  // Sweep up
  for (int rpm = 0; rpm <= 10000; rpm += 500) {
    setRPM(rpm);
    delay(300);
  }
  
  // Sweep down
  for (int rpm = 10000; rpm >= 0; rpm -= 500) {
    setRPM(rpm);
    delay(300);
  }
  
  Serial.println(F("Sweep complete.\n"));
}

void printMenu() {
  Serial.println(F("\n================================"));
  Serial.println(F(" 1962 Sun Tach Calibration Tool "));
  Serial.println(F("================================"));
  Serial.println(F(" [0] Set Needle to 0 RPM"));
  Serial.println(F(" [1] Set Needle to 2,500 RPM (25%)"));
  Serial.println(F(" [2] Set Needle to 5,000 RPM (50%)"));
  Serial.println(F(" [3] Set Needle to 7,500 RPM (75%)"));
  Serial.println(F(" [4] Set Needle to 10,000 RPM (100%)"));
  Serial.println(F(" [r] Run Automatic Sweep Test"));
  Serial.println(F("--------------------------------"));
  Serial.print(F("Enter command: "));
}