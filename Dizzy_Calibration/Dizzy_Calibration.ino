const int gaugePin = 9;    // PWM output to moving-coil gauge
const int maxRPM = 10000;  // Maximum reading on your gauge face

// Test points in RPM to check across your dial
int testPoints[] = {0, 2000, 4000, 5000, 6000, 8000, 10000};
int numPoints = 7;

void setup() {
  pinMode(gaugePin, OUTPUT);
  Serial.begin(9600);

  // STEP 1: Full-scale hold for 15 seconds
  // Turn your 10k potentiometer now until the needle hits 10,000 RPM
  Serial.println("HOLDING AT 10,000 RPM - Adjust potentiometer now...");
  analogWrite(gaugePin, 255);
  delay(15000); 
}

void loop() {
  // STEP 2: Step up through the dial (3 seconds per mark)
  for (int i = 0; i < numPoints; i++) {
    int targetRPM = testPoints[i];
    int pwmValue = map(targetRPM, 0, maxRPM, 0, 255);
    
    analogWrite(gaugePin, pwmValue);
    
    Serial.print("Target RPM: ");
    Serial.print(targetRPM);
    Serial.print(" | PWM: ");
    Serial.println(pwmValue);
    
    delay(3000);
  }

  // STEP 3: Smooth sweep back down to 0 RPM
  Serial.println("Sweeping down to 0 RPM...");
  for (int pwm = 255; pwm >= 0; pwm--) {
    analogWrite(gaugePin, pwm);
    delay(15);
  }
  
  delay(2000);
}
