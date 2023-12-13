#include <ESP32Servo.h>
#include "BluetoothSerial.h"
#include "pitches.h"

const char *pin = "1234";
String device_name = "Pescador-ESP32";

BluetoothSerial SerialBT;
Servo servo;
Servo servo2;
int pinServo = 5;
int pinServo2 = 2;
int pinLed1 = 4;
int pinLed2 = 19;
const int buzzerPin = 18;
const int trigPin = 26;
const int echoPin = 27;
#define SOUND_SPEED 0.034

const int breathDuration = 2000;

long duration;
float distanceCm;

int jingleBells[] = { NOTE_E5, NOTE_E5, NOTE_E5,
                      NOTE_E5, NOTE_E5, NOTE_E5,
                      NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5,
                      NOTE_E5,
                      NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5,
                      NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5,
                      NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5,
                      NOTE_D5, NOTE_G5 };
int noteDurations[] = { 8, 8, 4,
                        8, 8, 4,
                        8, 8, 8, 8,
                        2,
                        8, 8, 8, 8,
                        8, 8, 8, 16, 16,
                        8, 8, 8, 8,
                        4, 4 };

int santaMelody[] = {
  NOTE_G4,
  NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_G4,
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, NOTE_C5,
  NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_G4,
  NOTE_A4, NOTE_G4, NOTE_F4, NOTE_F4,
  NOTE_E4, NOTE_G4, NOTE_C4, NOTE_E4,
  NOTE_D4, NOTE_F4, NOTE_B3,
  NOTE_C4
};

int noteDurations2[] = {
  8,
  8, 8, 4, 4, 4,
  8, 8, 4, 4, 4,
  8, 8, 4, 4, 4,
  8, 8, 4, 2,
  4, 4, 4, 4,
  4, 2, 4,
  1
};

void playMelody() {
  int size = sizeof(noteDurations) / sizeof(int);

  for (int thisNote = 0; thisNote < size; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(buzzerPin, jingleBells[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzerPin);
  }
}

void playMelodySanta() {
  for (int i = 0; i < sizeof(santaMelody) / sizeof(santaMelody[0]); i++) {
    int noteDuration = 2000 / noteDurations2[i];
    digitalWrite(led, HIGH);
    tone(buzzer, santaMelody[i], noteDurations2);
    delay(noteDurations2 * 1.1);
    noTone(buzzer);
    delay(50);
  }
}

void breatheEffect(int pinLed1) {
  int originalState = digitalRead(pinLed1);

  for (int repeat = 0; repeat < 4; repeat++) {
    for (int i = 0; i <= 255; i++) {
      analogWrite(pinLed1, i);
      delay(breathDuration / 255);
    }

    for (int i = 255; i >= 0; i--) {
      analogWrite(pinLed1, i);
      delay(breathDuration / 255);
    }
  }

  digitalWrite(pinLed1, originalState);
}

void setup() {
  Serial.begin(9600);
  SerialBT.begin(device_name);
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
#ifdef USE_PIN
  SerialBT.setPin(pin);
  Serial.println("Using PIN");
#endif

  playMelody();
  playMelodySanta();

  servo.attach(pinServo, 500, 2500);
  servo2.attach(pinServo2, 500, 2500);
  pinMode(pinLed1, OUTPUT);
  pinMode(pinLed2, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

int pos = 0;
int pos2 = 90;
int ppos2 = 240;

void loop() {
  updateBluetooth();

  measureDistance();

  Serial.print("Distancia (cm): ");
  Serial.println(distanceCm);

  if (distanceCm < 50) {
    digitalWrite(pinLed1, HIGH);
  } else {
    digitalWrite(pinLed1, LOW);
  }
}

void updateBluetooth() {
  if (SerialBT.available()) {
    char data = SerialBT.read();

    if (data == '1') {
      performServoActions();
    }

    if (data == '2') {
      breatheEffect(pinLed1);
    }
  }
}

void measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED / 2;
}

void performServoActions() {
  digitalWrite(pinLed2, HIGH);

  for (pos = 90; pos <= 130; pos += 5) {
    servo.write(pos);
    servo2.write(pos2);
    delay(30);
  }

  delay(5000);

  for (pos = 130; pos >= 90; pos -= 4) {
    servo.write(pos);
    servo2.write(ppos2);
    delay(120);
  }

  delay(400);
  digitalWrite(pinLed2, LOW);

  servo.write(130);
  servo2.write(ppos2);
}
