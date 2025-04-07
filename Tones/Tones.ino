#include "Note.h"


#define LED_PIN 4
#define BUZ_PIN 6
#define SPK_PIN 8

void serialToNotes(String);
void playNotes(std::vector<int>);

std::vector<Note> notes;

void setup() {
  Serial.begin(115200);

  while (true) {
    while (Serial.available() == 0) {}
    String s = Serial.readStringUntil(' ');
    if (s.startsWith("END")) {
      break;
    }
    serialToNotes(s);
  }

  ledcAttachChannel(SPK_PIN, 440, 8, 1);
  ledcAttachChannel(BUZ_PIN, 440, 8, 2);

  delay(2000);
  for (Note n : notes) {
    Serial.println(n.toString());
  }
  Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
}

void loop() {
  playNotes({ BUZ_PIN, SPK_PIN });
  delay(6000);
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

  notes.push_back(Note((byte)channel, frequency, deltaTime, (byte)velocity));
}

void playNotes(std::vector<int> pins) {
  for (Note n : notes) {
    if (n.channel >= pins.size()) {
      continue;
    }
    delay(n.deltaTime);
    if (n.velocity > 0) {
      ledcWriteTone(pins[n.channel], n.frequency);
    } else {
      ledcWriteTone(pins[n.channel], 0);
    }
  }
}
