

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(MANUAL);

#define SR04T_TRIG_PIN D0
#define SR04T_ECHO_PIN D1

#include "jsn_sr04t.h"

jsnSr04t rangeFinder;


void setup() {
  rangeFinder.setup(SR04T_TRIG_PIN,SR04T_ECHO_PIN);
  Serial.begin(115200);
}

void loop() {
  float range = rangeFinder.getRangeMeters();
  Serial.printlnf("range(m): %.3f",range);
  delay(20);  // Delay between readings
}
