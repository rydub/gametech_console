#include "Arduino.h"
#include "ultrasonic.h"



void ultraSetup() {
  pinMode(TRIG_PIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN, INPUT); // Sets the echoPin as an Input
}

int ultraDistance() {
  long duration;
  int distance;
  // Clears the trigPin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH, 50000);
  // Calculating the distance in cm
  distance= duration*0.034/2;
  // Prints the distance on the Serial Monitor
  //Serial.print("Distance: ");
  //Serial.println(distance);
  return distance;
}

bool ultraCheck() {
  int currentReading = ultraDistance();
  if ((currentReading >= 5) && (currentReading <= 60)) {
    return true;
  } else {
    return false;
  }
}

