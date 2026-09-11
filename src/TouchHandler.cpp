// TouchHandler.cpp
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include "TouchHandler.h"
#include "Config.h"
#include <Preferences.h>

#define T_IRQ  35
#define T_DO   32   // MISO của bus touch riêng
#define T_DIN  4    // MOSI của bus touch riêng
#define T_CLK  33
#define T_CS   23

// Calibration tuyến tính — đã xác nhận chuẩn trên board thật, giữ nguyên
#define TS_MINX 200
#define TS_MAXX 3900
#define TS_MINY 200
#define TS_MAXY 3900

// Lọc nhiễu: lấy trung bình vài mẫu, bỏ mẫu áp lực yếu
#define TOUCH_SAMPLES     3
#define TOUCH_Z_THRESHOLD 300

static const uint32_t LONG_PRESS_MS   = 1000;
static const uint16_t TAP_MOVE_THRESH = 15;

SPIClass touchSPI(HSPI);   // bus SPI phần cứng thứ hai, độc lập với LCD
XPT2046_Touchscreen touchscreen(T_CS, T_IRQ);

static enum { TS_IDLE, TS_DOWN, TS_LONG_FIRED } _ts = TS_IDLE;
static uint32_t _pressStartMs = 0;
static uint16_t _startX = 0, _startY = 0, _lastX = 0, _lastY = 0;

static uint16_t _lastX_cached = 0, _lastY_cached = 0, _lastZ_cached = 0;
static bool     _lastTouched_cached = false;

struct TouchCalibData { float ax,bx,dx, ay,by,dy; bool valid; };
static TouchCalibData _cal = {0,0,0, 0,0,0, false};

void initTouch()
{
    touchSPI.begin(T_CLK, T_DO, T_DIN, T_CS);
    touchscreen.begin(touchSPI);
    touchscreen.setRotation(1);

    // Nạp calibration đã đo bằng calib:start (đã xác nhận sai số 1-5px)
    Preferences p;
    p.begin("touchcal", true);
    _cal.valid = p.getBool("ok", false);
    if (_cal.valid) {
        _cal.ax = p.getFloat("ax", 0); _cal.bx = p.getFloat("bx", 0); _cal.dx = p.getFloat("dx", 0);
        _cal.ay = p.getFloat("ay", 0); _cal.by = p.getFloat("by", 0); _cal.dy = p.getFloat("dy", 0);
    }
    p.end();
    Serial.printf("[Touch] Calib NVS: %s\n", _cal.valid ? "loaded" : "khong co, dung fallback map()");
}

bool getTouchPoint(uint16_t &x, uint16_t &y, uint16_t &z)
{
    if (!touchscreen.touched()) {
        if (config.touchDebug && _lastTouched_cached) Serial.println("released");
        _lastTouched_cached = false;
        return false;
    }

    long sumX = 0, sumY = 0, sumZ = 0;
    int valid = 0;
    for (int i = 0; i < TOUCH_SAMPLES; i++) {
        TS_Point p = touchscreen.getPoint();
        if (p.z > TOUCH_Z_THRESHOLD) { sumX += p.x; sumY += p.y; sumZ += p.z; valid++; }
    }
    if (valid == 0) {
        if (config.touchDebug && _lastTouched_cached) Serial.println("released");
        _lastTouched_cached = false;
        return false;
    }

    int16_t rx = sumX / valid, ry = sumY / valid;
    if (_cal.valid) {
        x = (uint16_t)(_cal.ax*rx + _cal.bx*ry + _cal.dx);
        y = (uint16_t)(_cal.ay*rx + _cal.by*ry + _cal.dy);
    } else {
        x = map(rx, TS_MINX, TS_MAXX, 0, 320);
        y = map(ry, TS_MINY, TS_MAXY, 0, 240);
    }
    z = sumZ / valid;

    if (config.touchDebug) {
        static uint32_t _lastRawPrintMs = 0;
        if (!_lastTouched_cached) {
            Serial.printf("touch DOWN at (%d,%d) z=%d\n", x, y, z);
            _lastRawPrintMs = millis();
        } else if (millis() - _lastRawPrintMs >= 200) {
            Serial.printf("RAW (%d,%d) z=%d\n", x, y, z);
            _lastRawPrintMs = millis();
        }
    }

    _lastX_cached = x; _lastY_cached = y; _lastZ_cached = z;
    _lastTouched_cached = true;
    return true;
}

bool pollTouchEvent(TouchEvent& out) {
    out.kind = TOUCH_NONE;
    uint16_t x, y, z;
    bool touched = getTouchPoint(x, y, z);
    uint32_t now = millis();

    switch (_ts) {
        case TS_IDLE:
            if (touched) {
                _ts = TS_DOWN;
                _pressStartMs = now;
                _startX = _lastX = x;
                _startY = _lastY = y;
            }
            break;

        case TS_DOWN:
            if (touched) {
                _lastX = x; _lastY = y;
                if (now - _pressStartMs >= LONG_PRESS_MS &&
                    abs((int)x - (int)_startX) <= TAP_MOVE_THRESH &&
                    abs((int)y - (int)_startY) <= TAP_MOVE_THRESH) {
                    out = { TOUCH_LONG_PRESS, _startX, _startY };
                    _ts = TS_LONG_FIRED;
                    return true;
                }
            } else {
                if (abs((int)_lastX - (int)_startX) <= TAP_MOVE_THRESH &&
                    abs((int)_lastY - (int)_startY) <= TAP_MOVE_THRESH) {
                    out = { TOUCH_TAP, _startX, _startY };
                    _ts = TS_IDLE;
                    return true;
                }
                _ts = TS_IDLE;
            }
            break;

        case TS_LONG_FIRED:
            if (!touched) _ts = TS_IDLE;
            break;
    }
    return false;
}

bool peekLastTouch(uint16_t& x, uint16_t& y, uint16_t& z) {
    x = _lastX_cached; y = _lastY_cached; z = _lastZ_cached;
    return _lastTouched_cached;
}