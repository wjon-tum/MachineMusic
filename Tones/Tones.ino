#include "Note.h"

#define LED_PIN 4
#define BUZ_PIN 6

void serialToNotes(String);
void playNotes(int, int);

std::vector<Note> notes;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZ_PIN, OUTPUT);

  while (true) {
    while (Serial.available() == 0) {}
    String s = Serial.readStringUntil(' ');
    if (s.startsWith("END")) {
      break;
    }
    serialToNotes(s);
  }

  tone(BUZ_PIN, 443);
  delay(1000);
  noTone(BUZ_PIN);

  delay(2000);
  for (Note n : notes) {
    Serial.println(n.toString());
  }
  Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
}

void loop() {
  playNotes(0, BUZ_PIN);
  delay(60000);
}

void serialToNotes(String s) {
  char delimiter = ',';
  int startIndex = 0;
  int endIndex;

  endIndex = s.indexOf(delimiter, startIndex);
  int channel = s.substring(startIndex, endIndex).toInt();
  startIndex = endIndex + 1;
  endIndex = s.indexOf(delimiter, startIndex);
  double frequency = s.substring(startIndex, endIndex).toFloat();
  startIndex = endIndex + 1;
  endIndex = s.indexOf(delimiter, startIndex);
  int deltaTime = s.substring(startIndex, endIndex).toInt();
  startIndex = endIndex + 1;
  endIndex = s.indexOf(delimiter, startIndex);
  int velocity = s.substring(startIndex, endIndex).toInt();

  notes.push_back(Note((byte) channel, frequency, deltaTime, (byte) velocity));
}

void playNotes(int channel, int pin) {
  for (Note n : notes) {
    if (n.channel != channel) {
      continue;
    }
    delay(n.deltaTime);
    if (n.velocity > 0) {
      tone(pin, n.frequency);
    } else {
      noTone(pin);
    }
  }

  noTone(pin);
}
