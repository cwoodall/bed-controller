#pragma once

#include <array>

class SwitchMatrix {
public:
  struct Location {
    size_t row;
    size_t column;
  };
  
private:
  std::array<pin_t, 4> rows;
  std::array<pin_t, 3> columns;
  Timer on_timer;
  bool is_pressing;

public:
  SwitchMatrix(std::array<pin_t, 4> r, std::array<pin_t, 3> c) : 
    rows{r}, 
    columns{c}, 
    on_timer(250, &SwitchMatrix::clear, *this, true),
    is_pressing(false)
  {}

  bool isPressing() const { return is_pressing; }

  void for_each (void (*fn)(pin_t)) {
    for (pin_t pin : columns) fn(pin);
    for (pin_t pin : rows) fn(pin);
  }

  void setup() {
    for_each([](pin_t pin) { pinMode(pin, OUTPUT); });
    
    clear();
  }

  void clear() {
    for_each([](pin_t pin) { digitalWrite(pin, LOW); });
    is_pressing = false;
  }

  void set(Location loc, PinState state) {
    clear();
    digitalWrite(rows[loc.row], state);
    digitalWrite(columns[loc.column], state);
    is_pressing = true;
  }

  bool press(Location loc, uint32_t duration_ms = 250) {
    if (is_pressing) {
      return false;
    } 

    on_timer.changePeriod(duration_ms);
    set(loc, HIGH);
    on_timer.start();

    return true;
  }
};