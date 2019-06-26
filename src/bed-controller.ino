/*
 * Project bed-controller
 * Description:
 * Author:
 * Date:
 */

#include <array>

struct SwitchMatrix {
  struct Location {
    size_t row;
    size_t column;
  };
  
  std::array<pin_t, 4> rows;
  std::array<pin_t, 3> columns;

  void for_each (void (*fn)(pin_t)) {
    for (pin_t pin : columns) {
      fn(pin);
    }

    for (pin_t pin : rows) {
      fn(pin);
    }
  }

  void setup() {
    for_each([](pin_t pin) { pinMode(pin, OUTPUT); });
    
    clear();
  }

  void clear() {
    for_each([](pin_t pin) { digitalWrite(pin, LOW); });
  }

  void set(Location loc, PinState state) {
    clear();
    digitalWrite(rows[loc.row], state);
    digitalWrite(columns[loc.column], state);
  }
};

static SwitchMatrix switch_pin_matrix = {
  {2, 3, 4, 5}, // Row Select Pin
  {A0, A1, A2}  // Column Select Pin
};


enum SwitchId  {
  HeadUp, HeadDown,
  FeetUp, FeetDown,
  BedUp,  BedDown,
  MemoA,  MemoB,
  Zero,   LED,
  Flashlight,
  Max
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

static SwitchMatrix::Location const &lookupSwitchLocation(SwitchId id) {
  return switch_locations[(size_t)id];
}

bool flashlight_state = LOW;
constexpr uint32_t kToggleTimeout_ms = 250;

static void pressSwitch(SwitchId id, PinState state) {
  static Timer timer(kToggleTimeout_ms, &SwitchMatrix::clear, switch_pin_matrix, true);
  SwitchMatrix::Location loc = lookupSwitchLocation(id);
  switch_pin_matrix.set(loc, state);
  timer.start();
}

int particleToggleSwitch(String switch_str) {
  int switch_idx = switch_str.toInt();
  if (0 <= switch_idx && switch_idx < (int) SwitchId::Max) {
    pressSwitch((SwitchId) switch_idx, HIGH);
    return 1;
  } else {
    return -1;
  }
}

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  switch_pin_matrix.setup();

  Particle.function("toggleSwitch", particleToggleSwitch);
}


// loop() runs over and over again, as quickly as it can execute.
void loop() {

}