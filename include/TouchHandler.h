#ifndef TOUCHHANDLER_H
#define TOUCHHANDLER_H

#include <Arduino.h>
#include <GUIslice_th.h>

void initTouch();
bool getTouchPoint(uint16_t &x, uint16_t &y, uint16_t &z);

enum TouchEventKind { TOUCH_NONE = 0, TOUCH_TAP, TOUCH_LONG_PRESS };
struct TouchEvent {
    TouchEventKind kind;
    uint16_t x, y;
};

bool pollTouchEvent(TouchEvent& out);
bool peekLastTouch(uint16_t& x, uint16_t& y, uint16_t& z);

// GUIslice adapter — bridges this touch layer to GUIslice's plugin interface.
// Instantiate in main.cpp, register via gslc_InitTouchHandler().
//
// GHI CHU QUAN TRONG — da do thuc te qua Serial (guirot/touchremap):
//   guirot=1 (dung, khop hien thi mat) + touchremap=false -> lech vuong goc (X/Y trao)
//   guirot=1 + touchremap=true  -> dung chieu tong the NHUNG truc Y bi dao nguoc
//   guirot=0/2 -> hong hien thi emotion, loai
// GUIslice khong co cong tac rieng de lat 1 truc, nen lat Y thu cong ngay tai day,
// truoc khi tra ve cho GUIslice. Panel o setRotation(1) la landscape 320x240,
// va getTouchPoint()/peekLastTouch() da tra toa do y trong khoang 0..240
// (xem map(ry, TS_MINY, TS_MAXY, 0, 240) trong TouchHandler.cpp).
#define TOUCH_PANEL_H 240
class CydTouchHandler : public TouchHandler {
public:
  void begin() override {}   // no-op; hardware init done in initTouch()

  THPoint getPoint() override {
    uint16_t x, y, z;
    bool touched = peekLastTouch(x, y, z);
    if (touched) {
      int16_t yFlipped = TOUCH_PANEL_H - (int16_t)y;   // lat truc Y — xem ghi chu tren
      if (!inTouch_) { inTouch_ = true; x_ = x; y_ = yFlipped; }
      else { x_ = (x_ * 7 + x) / 8; y_ = (y_ * 7 + yFlipped) / 8; }
      releaseHoldoff_ = 3;
      return THPoint(x_, y_, z ? z : 100);
    }
    if (releaseHoldoff_ > 0) {
      releaseHoldoff_--;
      return THPoint(x_, y_, 100);
    }
    inTouch_ = false;
    return THPoint(0, 0, 0);
  }

private:
  int16_t x_ = 0, y_ = 0;
  bool    inTouch_ = false;
  uint8_t releaseHoldoff_ = 0;
};

#endif