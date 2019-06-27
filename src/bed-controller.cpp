/*
 * Project bed-controller
 * Description:
 * Author:
 * Date:
 */
// Particle Libraries
#include "Particle.h"

// Private Libraries
#include "ArduinoJson.h"

// additional libraries
#include "switch_matrix.h"
#include "version.h"

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

static SwitchMatrix switches = {
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

constexpr uint32_t kPressWatchdogTime_ms = 2000;
Timer press_watchdog(kPressWatchdogTime_ms, &SwitchMatrix::clear, switches, true);
bool timer_disabled = false;

int cloudPressFor(String json) {
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
    return -1;
  }

  SwitchMatrix::Location loc;
  if (lookupSwitchLocation(doc["switch"], &loc)) {
    uint32_t delay_ms = doc["delay"] | 250;
    return switches.press(loc, delay_ms) ? 1 : -1;
  } else {
    return -1;
  }
}

int cloudPress(String json) {
  StaticJsonDocument<200> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, json);

  // Test if parsing succeeds.
  if (error) {
    return -1;
  }

  if (!(doc["force"] | false) && switches.isPressing()) {
    if (!timer_disabled) {
      press_watchdog.reset();    
    }
    return 2;
  }

 
  JsonVariant id = doc["switch"];
  
  if (id.isNull()) {
    return -1;
  }

  SwitchMatrix::Location loc;
  if (lookupSwitchLocation(id, &loc)) {
    if (!timer_disabled) {
      press_watchdog.reset();
    }
    switches.set(loc, HIGH);
    return 1;
  } else {
    return -1;
  }
}

int cloudSetPressTimeout(String json) {
    StaticJsonDocument<200> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, json);

  // Test if parsing succeeds.
  if (error) {
    return -1;
  }

  uint32_t timeout = doc["timeout"] | kPressWatchdogTime_ms;
  timer_disabled = timeout == 0;
  press_watchdog.changePeriod(timeout);

  return 1;
}

int cloudRelease(String _) {
  press_watchdog.stop();
  switches.clear();
  return 1;
}

bool is_pressing;
// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  switches.setup();

  Particle.function("pressFor", cloudPressFor);
  Particle.function("press", cloudPress);
  Particle.function("release", cloudRelease);
  Particle.function("pressTimeout", cloudSetPressTimeout);
  Particle.variable("isPressing", is_pressing);
  Particle.variable("version", kVersion);
}


// loop() runs over and over again, as quickly as it can execute.
void loop() {
  is_pressing = switches.isPressing();
}