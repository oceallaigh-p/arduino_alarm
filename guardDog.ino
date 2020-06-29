/*
 *      Name:   Paidraig O'Ceallaigh
 *      
 *      Date:   26 June 2020
 *      
 *      Title:  Arduino Project - "Guard Dog"
 *      
 *      Purpose: Use the Arduino to create an intruder detection system.
 *               It uses a touch sensor to arm and disarm the system and
 *               an ultrasonic sensor (HC-SR04) to detect possible intruders. 
 *               If an intruder is detected, the buzzer will sound and a 
 *               message will be written to Serial Monitor
 *      
 */


// Constants
const int TRIG_PIN   = 12;    // Attach Arduino pin D3 to Trigger Pin of HC-SR04
const int ECHO_PIN   = 13;    // Attach Arduino pin D2 to Echo Pin of HC-SR04
const int BUZZER_PIN = 8;     // Attach Arduino pin D 11 to Positive Pim of Buzzer
const int TOUCH_PIN  = 2;     // Attach Arduino pin D 2 to Signal Pin of Touch Senor
const int FREQUENCY_FAR    = 2500;  // Frequency of tone in hertz
const int DURATION_FAR     = 500;   // Duration of tone in milliseconds
const int FREQUENCY_CLOSE  = 2500;  // Frequency of tone in hertz
const int DURATION_CLOSE   = 100;   // Duration of tone in milliseconds
const int DISTANCE_FAR     = 36;    // Distance alarm begins to sound
const int DISTANCE_CLOSE   = 12;    // Distance alarm frequency increases
const int TONE_DELAY_FAR   = DURATION_FAR * 2.00;     // Time between tones 
const int TONE_DELAY_CLOSE = DURATION_CLOSE * 1.25;   // Time between tones
const unsigned long TOUCH_DELAY = 500;    // Delay for touch sensor in milliseconds


// Variables
bool system_armed = false;    // Flag for status of system (armed or disarmed)
byte previous_state;          // Previous state of touch sensor
byte current_state;           // Current state of touch sensor
unsigned long start_time;     // Initial value of time (for sensor timing) in milliseconds
unsigned long current_time;   // Cuurent value of time (for sensor timing) in milliseconds


// Function that sets initial values
void setup() {
  // Begin serial communication at 9600 baud
  Serial.begin(9600);

  // Set the start time 
  start_time = millis();

  // Configure the pins as input/output 
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TOUCH_PIN, INPUT);

  // Print startup message
  Serial.println();
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  Serial.println("    Welcome to Guard Dog     ");
  Serial.println();
  Serial.println("    Gaurd Dog is DISARMED    ");
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  Serial.println();
}


// Function that gets the distance of the intruder
int getDistance() {
  
  // Clears the Trigger Pin condition
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Reads the Echo Pin and returns the sound wave travel time (microseconds)
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate and return the distance in inches
  return (duration * 0.0133/2);
}


// Function that arms and disarms the system
void armDisarm() {
  current_time = millis();
  previous_state = current_state;

  // Using millis() instead of delay to control timing of
  //   touch sensor (from: https://forum.arduino.cc/index.php?topic=503368.0)
  if (current_time - start_time > TOUCH_DELAY) {
    current_state = digitalRead(TOUCH_PIN);

    if (current_state == LOW && previous_state == HIGH) {
      Serial.println();
      Serial.println("Sensor is touched");

      system_armed = !system_armed;

      if (system_armed) {
        Serial.println("Guard Dog has been ARMED");
        Serial.println();
      }
      else {
        Serial.println("Guard Dog has been DISARMED");
        Serial.println();
      }
    }
  }
}


// Function that activates the buzzer
void activateBuzzer(int freq, int dur, int t_delay) {
  tone(BUZZER_PIN, freq, dur);
  delay(t_delay);
}


// Main Function
void loop() {

  // Arm and disarm the system
  armDisarm();

  // If system is armed, check for motion and activate
  //   buzzer if movement is detected within defined range
  if (system_armed) {

    // Store intruder distance
    int distance = getDistance();
    
    if (distance > DISTANCE_CLOSE && distance < DISTANCE_FAR ) {
      activateBuzzer(FREQUENCY_FAR, DURATION_FAR, TONE_DELAY_FAR);
      Serial.print("Intruder detected: ");
      Serial.print(distance);
      Serial.println(" inches away.");
    }
    if (distance < DISTANCE_CLOSE) {
      activateBuzzer(FREQUENCY_CLOSE, DURATION_CLOSE, TONE_DELAY_CLOSE);
      Serial.print("Intruder detected: ");
      Serial.print(distance);
      Serial.println(" inches away.");
    }
  }

}
