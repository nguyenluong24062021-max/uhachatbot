// TouchTest.cpp — man hinh debug cam ung doc lap
// Ve con tro hien tai + duong di (trail), hien toa do/raw/z tren HUD.
// Dung getTouchPoint() that (khong doc raw truc tiep) de phan anh dung
// pipeline UI dang dung (affine calib + loc nhieu trong TouchHandler.cpp).
#include <TFT_eSPI.h>
#include "TouchTest.h"
#include "TouchHandler.h"
#include "UIManager.h"

extern TFT_eSPI tft;

// Nut Back o goc tren-trai, giong quy uoc cua Test Suite cu
#define BACK_BTN_W 60
#define BACK_BTN_H 24

#define TRAIL_MAX 40
#define CURSOR_R  8

static int16_t trailX[TRAIL_MAX], trailY[TRAIL_MAX];
static int     trailLen = 0;
static int16_t cursorX = -1, cursorY = -1;

static void drawBackBtn() {
    tft.fillRoundRect(2, 2, BACK_BTN_W, BACK_BTN_H, 3, TFT_RED);
    tft.drawRoundRect(2, 2, BACK_BTN_W, BACK_BTN_H, 3, TFT_WHITE);
    tft.setTextColor(TFT_WHITE, TFT_RED);
    tft.setTextSize(1);
    tft.setCursor(10, 9);
    tft.print("< Thoat");
}

static bool isBackBtn(int tx, int ty) {
    return tx >= 2 && tx < 2 + BACK_BTN_W && ty >= 2 && ty < 2 + BACK_BTN_H;
}

static void trailClear() {
    for (int i = 0; i < trailLen; i++) {
        tft.drawPixel(trailX[i], trailY[i], TFT_BLACK);
        if (i > 0) tft.drawLine(trailX[i-1], trailY[i-1], trailX[i], trailY[i], TFT_BLACK);
    }
    trailLen = 0;
}

static void trailAdd(int16_t x, int16_t y) {
    if (trailLen > 0) {
        tft.drawLine(trailX[trailLen-1], trailY[trailLen-1], x, y, 0x4208); // xam nhat
    }
    if (trailLen < TRAIL_MAX) {
        trailX[trailLen] = x; trailY[trailLen] = y; trailLen++;
    } else {
        memmove(trailX, trailX+1, (TRAIL_MAX-1)*sizeof(int16_t));
        memmove(trailY, trailY+1, (TRAIL_MAX-1)*sizeof(int16_t));
        trailX[TRAIL_MAX-1] = x; trailY[TRAIL_MAX-1] = y;
    }
}

// Ve crosshair + vong tron tai vi tri cham, xoa vi tri cu truoc do
static void cursorDraw(int16_t x, int16_t y) {
    if (cursorX >= 0) {
        tft.drawFastHLine(cursorX - CURSOR_R, cursorY, CURSOR_R*2+1, TFT_BLACK);
        tft.drawFastVLine(cursorX, cursorY - CURSOR_R, CURSOR_R*2+1, TFT_BLACK);
        tft.drawCircle(cursorX, cursorY, CURSOR_R, TFT_BLACK);
    }
    cursorX = x; cursorY = y;
    tft.drawFastHLine(x - CURSOR_R, y, CURSOR_R*2+1, TFT_YELLOW);
    tft.drawFastVLine(x, y - CURSOR_R, CURSOR_R*2+1, TFT_YELLOW);
    tft.drawCircle(x, y, CURSOR_R, TFT_GREEN);
    tft.fillCircle(x, y, 2, TFT_WHITE);
}

#define HUD_H 34
static void drawHud(bool touched, uint16_t x, uint16_t y, uint16_t z, uint32_t touchCount) {
    int W = tft.width();
    tft.fillRect(0, tft.height() - HUD_H, W, HUD_H, TFT_BLACK);
    tft.drawFastHLine(0, tft.height() - HUD_H, W, TFT_DARKGREY);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    char buf[64];
    int ly = tft.height() - HUD_H + 4;
    snprintf(buf, sizeof(buf), "Trang thai: %s", touched ? "DANG CHAM" : "nha tay");
    tft.setCursor(4, ly); tft.print(buf);
    snprintf(buf, sizeof(buf), "x=%3d  y=%3d  z=%4d", x, y, z);
    tft.setCursor(4, ly + 11); tft.print(buf);
    snprintf(buf, sizeof(buf), "So lan cham: %lu", (unsigned long)touchCount);
    tft.setCursor(4, ly + 22); tft.print(buf);
}

static bool wantExitSerial() {
    if (!Serial.available()) return false;
    String l = Serial.readStringUntil('\n');
    l.trim();
    return l.equalsIgnoreCase("touchtest:exit");
}

void startTouchTest() {
    Serial.println("[TouchTest] Bat dau. Cham/keo tren man de xem con tro + duong di.");
    Serial.println("[TouchTest] Cham goc tren-trai hoac go touchtest:exit de thoat.");
    Serial.println("[TouchTest] Moi lan cham, toa do se duoc in ra Serial (x,y,z).");

    tft.fillScreen(TFT_BLACK);
    drawBackBtn();
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(70, 9);
    tft.print("Touch Test");

    trailClear();
    cursorX = cursorY = -1;
    uint32_t touchCount = 0;
    bool wasTouched = false;

    drawHud(false, 0, 0, 0, touchCount);

    while (true) {
        if (wantExitSerial()) { Serial.println("[TouchTest] Da thoat (serial)"); break; }

        uint16_t x, y, z;
        // getTouchPoint() = pipeline that: raw -> loc nhieu -> affine calib
        // (dung cua TouchHandler.cpp, chua qua remap/rotate rieng cua GUIslice)
        bool touched = getTouchPoint(x, y, z);

        if (touched) {
            if (isBackBtn(x, y)) {
                Serial.println("[TouchTest] Da thoat (cham goc)");
                break;
            }
            if (!wasTouched) {
                touchCount++;
                Serial.printf("[TouchTest] DOWN #%lu: x=%d y=%d z=%d\n",
                              (unsigned long)touchCount, x, y, z);
            } else {
                Serial.printf("[TouchTest] MOVE: x=%d y=%d z=%d\n", x, y, z);
            }
            trailAdd(x, y);
            cursorDraw(x, y);
            drawHud(true, x, y, z, touchCount);
        } else {
            if (wasTouched) {
                Serial.println("[TouchTest] UP (nha tay)");
                drawHud(false, cursorX < 0 ? 0 : cursorX, cursorY < 0 ? 0 : cursorY, 0, touchCount);
            }
        }
        wasTouched = touched;
        delay(15);
    }

    setUIMode(MODE_FACE);
}
