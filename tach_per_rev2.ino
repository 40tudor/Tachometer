/*
 * Tachometer
 *
 * Uses a Hall Effect sensor to implement a magnetic tachometer.
 * The sensor is connected to pin 2 (interrupt 0)
 * Updates RPM every cycle
 *
 */

volatile unsigned long rpm;
unsigned long max_rpm;
unsigned long rpm_scale;
volatile unsigned long timeold;
int needle_pos;
unsigned long max_pos;

void rpm_counter()
 {
   //Each rotation, this interrupt function is run once
   //Update time since last interrupt
      rpm = ((60000)/(millis()-timeold));
      timeold=millis();
      digitalWrite(13,HIGH);
      delayMicroseconds(30000);
      digitalWrite(13,LOW);
   }

void setup()
 {
   //Initialize variables
   rpm = 0;
   rpm_scale = 1;
   max_rpm = rpm_scale*3500;
   timeold = 0;
   needle_pos = 0;
   max_pos = 118;
   
   //Initialize digital IO
   //Serial.begin(9600);
   pinMode (2,INPUT);
   pinMode (9,OUTPUT);
   rpm=max_rpm/2;
   needle_pos = int(((float)rpm/(float)max_rpm)*(float)max_pos);
   rpm=0;
   analogWrite (9,needle_pos);
   needle_pos = 0;
   delay(1000);
   analogWrite (9,0);
   
   //Interrupt 0 is digital pin 2, so that is where the sensor is connected
   //Triggers on FALLING (change from HIGH to LOW)
   attachInterrupt(0, rpm_counter, RISING);   
 }

 void loop()
 {
//   Update needle position - scale so max_rpm covers full range (max_pos)
//   needle_pos = (int(rpm)/int(max_rpm))*int(max_pos);
     needle_pos = int(((float)rpm/(float)max_rpm)*(float)max_pos);

   if (needle_pos <=0) needle_pos=0;
   if (needle_pos >=max_pos) needle_pos=max_pos;
   analogWrite (9,needle_pos);
//   Serial.print(needle_pos,DEC);
//   Serial.print("\t");
//   Serial.print(rpm,DEC);
//   Serial.print("\t");
//   Serial.print(max_rpm,DEC);
//   Serial.print("\t");
//   Serial.println(max_pos);
 }
