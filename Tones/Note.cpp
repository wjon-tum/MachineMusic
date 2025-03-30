#include "Note.h"
#include <sstream>

String Note::toString() {
  String s = "[";
  return s + channel + "," + frequency + "," + deltaTime + "," + velocity + "]";
}