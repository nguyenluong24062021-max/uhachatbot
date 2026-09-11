#include <Arduino.h>
#include "UIManager.h"
#include "TouchHandler.h"
#include "Config.h"
#include "FaceRenderer.h"
#include "MenuApp.h"
#include "LiveChatRenderer.h"
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

static UIMode _mode = MODE_FACE;
static uint32_t _lastActivityMs = 0;
extern void refreshStatsText();

static void enterMenu() {
    _mode = MODE_MENU;
    pauseFace();
    liveChatRenderer.pause();
    tft.fillScreen(TFT_BLACK);
    gslc_SetPageCur(&m_gui, MENU_PG_ROOT);
    gslc_PageRedrawSet(&m_gui, true);   // force full redraw on entry
    _lastActivityMs = millis();
    refreshStatsText();
}

static void exitMenu() {
    _mode = MODE_FACE;
    tft.fillScreen(config.bgColour);
    resetFaceState();
    resumeFace();
    liveChatRenderer.pause();
}

static void enterLiveChat() {
    _mode = MODE_LIVECHAT;
    pauseFace();
    tft.fillScreen(TFT_BLACK);
    liveChatRenderer.resume();
    liveChatRenderer.setState(CHAT_IDLE);
    Serial.println("OK: entered LIVECHAT mode");
}

static void exitLiveChat() {
    _mode = MODE_FACE;
    liveChatRenderer.pause();
    tft.fillScreen(config.bgColour);
    resetFaceState();
    resumeFace();
    Serial.println("OK: exited LIVECHAT mode");
}

void initUI() {
    _mode = MODE_FACE;
}

void serviceUI() {
    TouchEvent ev;
    bool gotEvent = pollTouchEvent(ev);

    // Enter menu on long-press from face (one-way)
    if (gotEvent && ev.kind == TOUCH_LONG_PRESS && _mode == MODE_FACE) {
        enterMenu();
        Serial.println("OK: entered MENU");
    }

    // Double-tap from face to enter LiveChat mode
    if (gotEvent && ev.kind == TOUCH_TAP && _mode == MODE_FACE) {
        static uint32_t lastTapMs = 0;
        uint32_t now = millis();
        if (now - lastTapMs < 500) {  // Double tap trong 500ms
            enterLiveChat();
        }
        lastTapMs = now;
    }

    if (_mode == MODE_MENU) {
        // Any finger contact resets activity timer (taps, drags, holds all count)
        uint16_t tx, ty, tz;
        if (peekLastTouch(tx, ty, tz)) {
            _lastActivityMs = millis();
        }

        // Non-blocking idle check
        if (!config.touchDebug && config.menuTimeoutSec > 0 && (millis() - _lastActivityMs) >= (uint32_t)config.menuTimeoutSec * 1000) {
            exitMenu();
            Serial.println("OK: returned to FACE (idle)");
            return;
        }

        // 1Hz stats refresh while in menu
        static uint32_t _lastStatsMs = 0;
        if (millis() - _lastStatsMs >= 1000) {
            refreshStatsText();
            _lastStatsMs = millis();
        }

        gslc_Update(&m_gui);
    }
    
    if (_mode == MODE_LIVECHAT) {
        // Tap để interrupt bot khi đang speaking
        if (gotEvent && ev.kind == TOUCH_TAP) {
            if (liveChatRenderer.getState() == CHAT_SPEAKING) {
                liveChatRenderer.onInterrupt();
                Serial.println("OK: interrupted bot");
            } else if (liveChatRenderer.getState() == CHAT_IDLE) {
                // Tap trong idle mode -> bắt đầu listening
                liveChatRenderer.setState(CHAT_LISTENING);
                Serial.println("OK: started listening");
            }
        }
        
        // Long press để exit LiveChat
        if (gotEvent && ev.kind == TOUCH_LONG_PRESS) {
            exitLiveChat();
        }
        
        // Update LiveChat renderer
        serviceLiveChat();
    }
}

UIMode getUIMode() { return _mode; }

void setUIMode(UIMode m) {
    if (m == MODE_MENU && _mode == MODE_FACE) enterMenu();
    else if (m == MODE_FACE && _mode == MODE_MENU) exitMenu();
    else if (m == MODE_LIVECHAT && _mode == MODE_FACE) enterLiveChat();
    else if (m == MODE_FACE && _mode == MODE_LIVECHAT) exitLiveChat();
    else if (m == MODE_MENU && _mode == MODE_LIVECHAT) {
        exitLiveChat();
        enterMenu();
    }
    else if (m == MODE_LIVECHAT && _mode == MODE_MENU) {
        exitMenu();
        enterLiveChat();
    }
}

void menuBack() {
    if (_mode == MODE_MENU) exitMenu();
    else if (_mode == MODE_LIVECHAT) exitLiveChat();
}

bool menuSelect(uint8_t idx) {
    (void)idx;
    Serial.println("ERR: menu_select deprecated — use touch UI");
    return false;
}

void cmdMenuBack() {
    menuBack();
    Serial.println("OK");
}

void cmdMenuState() {
    printMenuState();
}

void printMode() {
    const char* modeName = "UNKNOWN";
    if (_mode == MODE_FACE) modeName = "FACE";
    else if (_mode == MODE_MENU) modeName = "MENU";
    else if (_mode == MODE_LIVECHAT) modeName = "LIVECHAT";
    Serial.printf("mode:           %s\n", modeName);
}

void printMenuState() {
    const char* stateName = "UNKNOWN";
    if (_mode == MODE_FACE) stateName = "FACE";
    else if (_mode == MODE_MENU) stateName = "MENU (GUIslice-driven)";
    else if (_mode == MODE_LIVECHAT) stateName = "LIVECHAT";
    Serial.printf("menu:           %s\n", stateName);
}