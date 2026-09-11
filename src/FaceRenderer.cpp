#include "FaceRenderer.h"
#include <Grobot_Animations.h>
#include "Config.h"
#include <esp_system.h>   // for esp_random()
#include <string.h>

static uint32_t _nextIdleSwitch = 0;
// ----- Sprite + Grobot state -----
static TFT_eSPI    *_tft     = nullptr;
static TFT_eSprite *_canvas  = nullptr;
static GrobotEyes  *_eyes    = nullptr;
static int16_t _lookX = 0, _lookY = 0;
static bool _hudOn = false;
static FaceShape _customL = {0, 0, 0, 30, 45};   // default = MOOD_NEUTRAL
static FaceShape _customR = {0, 0, 0, 30, 45};
static uint32_t _splashUntil = 0;             // 0 = inactive; else millis() at which to clear
static constexpr uint32_t SPLASH_DURATION_MS = 800;
static bool _paused = false;

// Maps a mood name string (as stored in config.mood) to a MoodData preset.
// The library only ships 5 presets (NEUTRAL/HAPPY/ANGRY/SAD/WINK); the
// IDLE1-3 names used for idle animation are mapped onto that same set so
// existing calls to setMood("IDLE1") etc. keep working.
static const MoodData& moodFromName(const char* name)
{
    if (strcmp(name, "HAPPY") == 0)  return MOOD_HAPPY;
    if (strcmp(name, "ANGRY") == 0)  return MOOD_ANGRY;
    if (strcmp(name, "SAD") == 0)    return MOOD_SAD;
    if (strcmp(name, "WINK") == 0)   return MOOD_WINK;
    if (strcmp(name, "IDLE1") == 0)  return MOOD_HAPPY;
    if (strcmp(name, "IDLE2") == 0)  return MOOD_WINK;
    if (strcmp(name, "IDLE3") == 0)  return MOOD_SAD;
    return MOOD_NEUTRAL; // "NEUTRAL" and any unrecognised name
}

// Internal helper — (re)builds the eyes with current colour state.
static void rebuildEyes()
{
    if (_eyes) { delete _eyes; _eyes = nullptr; }
    _eyes = new GrobotEyes(config.eyeColour, config.bgColour);
    _eyes->setEmotion(moodFromName(config.mood));
}

void initFaceRenderer(TFT_eSPI *tft)
{
    _tft = tft;

    _canvas = new TFT_eSprite(_tft);
    if (!_canvas->createSprite(320, 140)) {     // was (320, 120)
        Serial.println("FaceRenderer: sprite alloc failed (heap fragmented?)");
        delete _canvas;
        _canvas = nullptr;
        return;
    }
    _canvas->fillSprite(config.bgColour);

    rebuildEyes();
    _hudOn = config.hudOn;
}

void showSplash()
{
    if (!_tft) return;
    _tft->setRotation(3); // re-orientation
    _tft->fillScreen(config.bgColour);
    _tft->setTextColor(config.eyeColour);
    _tft->setTextSize(3);
    int cx = _tft->width() / 2;
    int cy = _tft->height() / 2;
    _tft->setCursor(cx - 65, cy - 12);
    _tft->print("RSC-CYD");
    _splashUntil = millis() + SPLASH_DURATION_MS;
}

void serviceFaceRenderer()
{
    if (!_tft || !_canvas || !_eyes) return;
    if (_splashUntil != 0) {
        if (millis() < _splashUntil) return;     // still showing splash, skip render
        _tft->fillScreen(config.bgColour);       // splash done, clear before face takes over
        _splashUntil = 0;
    }
    if (config.idleAnim && !config.moodAutoCycle) {
        uint32_t now = millis();
        if (_nextIdleSwitch == 0) {
            _nextIdleSwitch = now + 5000 + (esp_random() % 10000);
        } else if (now >= _nextIdleSwitch) {
            static const char* idles[] = {"IDLE1", "IDLE2", "IDLE3"};
            setMood(idles[esp_random() % 3]);
            _nextIdleSwitch = now + 5000 + (esp_random() % 10000);
        }
    } else {
        _nextIdleSwitch = 0;
    }
    if (_paused) return;
    if (config.moodAutoCycle) {
        // Library no longer exposes moodSwitch(); cycle through the same
        // preset list as the idle-animation branch above, on its own timer.
        static uint32_t nextAutoCycle = 0;
        uint32_t now = millis();
        if (nextAutoCycle == 0 || now >= nextAutoCycle) {
            static const char* cycle[] = {"NEUTRAL", "HAPPY", "IDLE1", "IDLE2", "IDLE3"};
            static uint8_t idx = 0;
            setMood(cycle[idx]);
            idx = (idx + 1) % (sizeof(cycle) / sizeof(cycle[0]));
            nextAutoCycle = now + 4000;
        }
    }
    _eyes->renderEmotions(*_canvas);
    if (_hudOn) _eyes->HUD(*_tft);
}

void setEyeColour(uint16_t rgb565)
{
    if (config.eyeColour == rgb565) return;
    config.eyeColour = rgb565;
    if (_eyes) rebuildEyes();
}

void setBackgroundColour(uint16_t rgb565)
{
    if (config.bgColour == rgb565) return;
    config.bgColour = rgb565;
    if (_canvas) _canvas->fillSprite(config.bgColour);
    if (_eyes) rebuildEyes();
}

void drawTouchMarker(int x, int y)
{
    if (!_tft) return;
    _tft->fillCircle(x, y, 5, TFT_GREEN);
}

void setMood(const char* moodName)
{
    strncpy(config.mood, moodName, sizeof(config.mood) - 1);
    config.mood[sizeof(config.mood) - 1] = '\0';
    if (_eyes) _eyes->setEmotion(moodFromName(moodName));
}

void setLookAt(int16_t x, int16_t y) { if (!_eyes) return; _lookX = x; _lookY = y; _eyes->lookAt(x, y); }
void getLookAt(int16_t &x, int16_t &y) { x = _lookX; y = _lookY; }
void triggerBlink() { if (_eyes) _eyes->blink(); }
void setHud(bool on) { _hudOn = on; }

static MoodData toMood(const FaceShape &s) {
    return MoodData{s.topH, s.botH, s.tilt, s.pR, s.radius};
}

void setFaceCustom(const FaceShape &shape) {
    _customL = shape;
    _customR = shape;
    if (_eyes) _eyes->setEmotion(toMood(shape));
}
void setFaceLeft(const FaceShape &shape) {
    _customL = shape;
    if (_eyes) _eyes->setEmotion(toMood(_customL), toMood(_customR));
}
void setFaceRight(const FaceShape &shape) {
    _customR = shape;
    if (_eyes) _eyes->setEmotion(toMood(_customL), toMood(_customR));
}
void getFaceLeft (FaceShape &out) { out = _customL; }
void getFaceRight(FaceShape &out) { out = _customR; }
void pauseFace()    { _paused = true; }
void resumeFace()   { _paused = false; }
bool isFacePaused() { return _paused; }
void resetFaceState() { rebuildEyes(); }