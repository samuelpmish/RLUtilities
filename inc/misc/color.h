#pragma once

struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;

  static Color red(){ return Color{255, 0, 0, 255}; }
  static Color green(){ return Color{0, 255, 0, 255}; }
  static Color blue(){ return Color{0, 0, 255, 255}; }
  static Color purple(){ return Color{255, 0, 255, 255}; }
  static Color white(){ return Color{255, 255, 255, 255}; }
  static Color black(){ return Color{0, 0, 0, 255}; }
};
