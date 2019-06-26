/*
 * Project bed-controller
 * Description:
 * Author:
 * Date:
 */

constexpr size_t switch_pin_matrix_size[2] = {4, 3};
constexpr pin_t switch_pin_matrix[2][4] = {
  {2, 3, 4, 5}, // Row Select Pin
  {A0, A1, A2, 0}  // Column Select Pin
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

struct Switches {
  size_t row;
  size_t column;
};
/**
 * | 0  | 1  |
 * | 2  | 3  |
 * | 4  | 5  |
 * | 6  | 7  |
 * | 8  | 9 |
 *   | 10 |
 */
constexpr size_t switch_locations[(size_t)SwitchId::Max][2] = {
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

static Switches lookupSwitchLocation(SwitchId id) {
  size_t const *loc = switch_locations[(size_t)id];
  return {loc[0], loc[1]};
}

static void clearSwitches() {
  for (size_t i = 0; i < 2; i++) {
    for (size_t j = 0; j < switch_pin_matrix_size[i]; j++) {
      digitalWrite(switch_pin_matrix[i][j], LOW);
    }
  }
}

static void setupSwitches() {
  for (size_t i = 0; i < 2; i++) {
    for (size_t j = 0; j < switch_pin_matrix_size[i]; j++) {
      pinMode(switch_pin_matrix[i][j], OUTPUT);
    }
  }
  clearSwitches();
}

bool flashlight_state = LOW;
constexpr uint32_t kToggleTimeout_ms = 250;

static void pressSwitch(SwitchId id, PinState state) {
  static Timer timer(kToggleTimeout_ms, clearSwitches, true);
  Switches switches = lookupSwitchLocation(id);
  clearSwitches();
  digitalWrite(switch_pin_matrix[0][switches.row], state);
  digitalWrite(switch_pin_matrix[1][switches.column], state);
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
  setupSwitches();

  Particle.function("toggleSwitch", particleToggleSwitch);
}


// loop() runs over and over again, as quickly as it can execute.
void loop() {

}