/*
 * Project bed-controller
 * Description:
 * Author:
 * Date:
 */
#include "Particle.h"

#include "switch_matrix.h"
#include "ArduinoJson.h"

// Use primary serial over USB interface for logging output
// You can watch logging of the Particle device via CLI:
// $ particle serial monitor --follow
SerialLogHandler logHandler;

constexpr uint32_t kMinToggleTimeout_ms = 250;

enum SwitchId  {
  HeadUp, HeadDown,
  FeetUp, FeetDown,
  BedUp,  BedDown,
  MemoA,  MemoB,
  Zero,   LED,
  Flashlight,
  Max
};

static SwitchMatrix switch_pin_matrix = {
  {2, 3, 4, 5}, // Row Select Pin
  {A0, A1, A2}  // Column Select Pin
};

/**
 * | 0  | 1  |
 * | 2  | 3  |
 * | 4  | 5  |
 * | 6  | 7  |
 * | 8  | 9 |
 *   | 10 |
 */
constexpr SwitchMatrix::Location switch_locations[(size_t)SwitchId::Max]= {
  {0, 0}, // 0
  {1, 1}, // 1
  {2, 0}, // 2
  {3, 1}, // 3
  {1, 0}, // 4
  {3, 0}, // 5
  {0, 1}, // 6
  {0, 2}, // 7
  {2, 1}, // 8
  {2, 2}, // 9
  {3, 2}, // 10.  Flashlight
};

static bool lookupSwitchLocation(int id, SwitchMatrix::Location *dst) {
  if (0 <= id && id < (int) SwitchId::Max) {
     *dst = switch_locations[(size_t)id];
     return true;
  } else {
    return false;
  }
}



int cloudPress(String json) {
  // Allocate the JSON document
  //
  // Inside the brackets, 200 is the capacity of the memory pool in bytes.
  // Don't forget to change this value to match your JSON document.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<200> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, json);

    // Test if parsing succeeds.
  if (error) {
    // Serial.print(F("deserializeJson() failed: "));
    // Serial.println(error.c_str());
    return -1;
  }

  SwitchMatrix::Location loc;
  if (lookupSwitchLocation(doc["switch"], &loc)) {
    uint32_t delay_ms = doc["delay"] | 250;
    return switch_pin_matrix.press(loc, delay_ms) ? 1 : -1;
  } else {
    return -1;
  }
}

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  switch_pin_matrix.setup();

  Particle.function("press", cloudPress);
}


// loop() runs over and over again, as quickly as it can execute.
void loop() {

}