#include "LiveChatRenderer.h"
#include "Config.h"
#include <Arduino.h>
#include <Grobot_Animations.h>

// Global instance
LiveChatRenderer liveChatRenderer;

// Canvas dimensions
static constexpr int16_t CANVAS_W = 320;
static constexpr int16_t CANVAS_H = 240;
static constexpr int16_t FACE_Y = 50;  // Y offset cho face

// Grobot eyes instance (reuse từ FaceRenderer pattern)
static GrobotEyes* _grobotEyes = nullptr;

LiveChatRenderer::LiveChatRenderer()
    : _tft(nullptr)
    , _canvas(nullptr)
    , _state(CHAT_IDLE)
    , _prevState(CHAT_IDLE)
    , _engine(ENGINE_GROBOT)
    , _paused(false)
    , _stateStartMs(0)
    , _lastUpdateMs(0)
    , _audioLevel(0.0f)
    , _spectrumBands(0)
    , _speakingProgress(0.0f)
    , _fgColor(TFT_WHITE)
    , _bgColor(TFT_BLACK)
    , _accentColor(0x07FF)  // Cyan
    , _animPhase(0.0f)
    , _blinkCounter(0)
    , _lookX(0)
    , _lookY(0)
    , _kawaiiEmotion(KAWAII_NEUTRAL)
    , _kawaiiTearLevel(0)
    , _kawaiiSparkleLevel(0)
    , _kawaiiJawTremble(0)
    , _bmoMood(BMO_NEUTRAL)
    , _bmoMouthOpen(0.0f)
    , _bmoMouthShape(0)
{
    _speakingText[0] = '\0';
    for (uint8_t i = 0; i < 16; i++) {
        _audioSpectrum[i] = 0.0f;
    }
}

LiveChatRenderer::~LiveChatRenderer() {
    if (_canvas) {
        _canvas->deleteSprite();
        delete _canvas;
    }
    if (_grobotEyes) {
        delete _grobotEyes;
    }
}

void LiveChatRenderer::init(TFT_eSPI* tft) {
    _tft = tft;
    
    // Tạo canvas
    _canvas = new TFT_eSprite(_tft);
    if (!_canvas->createSprite(CANVAS_W, CANVAS_H)) {
        Serial.println("LiveChat: Failed to create sprite");
        delete _canvas;
        _canvas = nullptr;
        return;
    }
    
    // Init Grobot engine
    _grobotEyes = new GrobotEyes(_fgColor, _bgColor);
    _grobotEyes->setEmotion(MOOD_NEUTRAL);
    
    _state = CHAT_IDLE;
    _stateStartMs = millis();
    _lastUpdateMs = millis();
    
    clearCanvas();
    Serial.println("LiveChat: Initialized");
}

void LiveChatRenderer::setState(ChatState state) {
    if (_state != state) {
        _prevState = _state;
        _state = state;
        _stateStartMs = millis();
        Serial.printf("LiveChat: State %d -> %d\n", _prevState, _state);
    }
}

void LiveChatRenderer::setEngine(AnimEngine engine) {
    if (_engine != engine) {
        _engine = engine;
        Serial.printf("LiveChat: Engine -> %d\n", _engine);
    }
}

void LiveChatRenderer::setAudioLevel(float level) {
    _audioLevel = constrain(level, 0.0f, 1.0f);
}

void LiveChatRenderer::setAudioSpectrum(const float* spectrum, uint8_t bands) {
    _spectrumBands = min(bands, (uint8_t)16);
    for (uint8_t i = 0; i < _spectrumBands; i++) {
        _audioSpectrum[i] = constrain(spectrum[i], 0.0f, 1.0f);
    }
}

void LiveChatRenderer::setSpeakingText(const char* text) {
    strncpy(_speakingText, text, sizeof(_speakingText) - 1);
    _speakingText[sizeof(_speakingText) - 1] = '\0';
}

void LiveChatRenderer::setSpeakingProgress(float progress) {
    _speakingProgress = constrain(progress, 0.0f, 1.0f);
}

void LiveChatRenderer::onInterrupt() {
    setState(CHAT_INTERRUPTED);
}

void LiveChatRenderer::clearInterrupt() {
    if (_state == CHAT_INTERRUPTED) {
        setState(CHAT_IDLE);
    }
}

void LiveChatRenderer::setEmotion(const char* emotion) {
    // Map emotion string to engine-specific emotions
    String emo = emotion;
    emo.toLowerCase();
    
    // ENGINE_GROBOT
    if (_engine == ENGINE_GROBOT && _grobotEyes) {
        if (emo == "happy") _grobotEyes->setEmotion(MOOD_HAPPY);
        else if (emo == "sad") _grobotEyes->setEmotion(MOOD_SAD);
        else if (emo == "angry") _grobotEyes->setEmotion(MOOD_ANGRY);
        else if (emo == "wink") _grobotEyes->setEmotion(MOOD_WINK);
        else _grobotEyes->setEmotion(MOOD_NEUTRAL);
    }
    
    // ENGINE_KAWAII (17 emotions)
    else if (_engine == ENGINE_KAWAII) {
        if (emo == "neutral") _kawaiiEmotion = KAWAII_NEUTRAL;
        else if (emo == "happy") _kawaiiEmotion = KAWAII_HAPPY;
        else if (emo == "sad") _kawaiiEmotion = KAWAII_SAD;
        else if (emo == "angry") _kawaiiEmotion = KAWAII_ANGRY;
        else if (emo == "surprised") _kawaiiEmotion = KAWAII_SURPRISED;
        else if (emo == "confused") _kawaiiEmotion = KAWAII_CONFUSED;
        else if (emo == "excited") _kawaiiEmotion = KAWAII_EXCITED;
        else if (emo == "tired") _kawaiiEmotion = KAWAII_TIRED;
        else if (emo == "crying") _kawaiiEmotion = KAWAII_CRYING;
        else if (emo == "laughing") _kawaiiEmotion = KAWAII_LAUGHING;
        else if (emo == "love") _kawaiiEmotion = KAWAII_LOVE;
        else if (emo == "worried") _kawaiiEmotion = KAWAII_WORRIED;
        else if (emo == "annoyed") _kawaiiEmotion = KAWAII_ANNOYED;
        else if (emo == "shocked") _kawaiiEmotion = KAWAII_SHOCKED;
        else if (emo == "shy") _kawaiiEmotion = KAWAII_SHY;
        else if (emo == "skeptical") _kawaiiEmotion = KAWAII_SKEPTICAL;
        else if (emo == "determined") _kawaiiEmotion = KAWAII_DETERMINED;
        else _kawaiiEmotion = KAWAII_NEUTRAL;
        
        // Set special effects based on emotion
        _kawaiiTearLevel = (emo == "crying" || emo == "sad") ? 8 : 0;
        _kawaiiSparkleLevel = (emo == "excited" || emo == "love" || emo == "happy") ? 6 : 0;
        _kawaiiJawTremble = (emo == "angry" || emo == "shocked" || emo == "worried") ? 5 : 0;
    }
    
    // ENGINE_BMO (25 moods)
    else if (_engine == ENGINE_BMO) {
        if (emo == "neutral") _bmoMood = BMO_NEUTRAL;
        else if (emo == "happy") _bmoMood = BMO_HAPPY;
        else if (emo == "sad") _bmoMood = BMO_SAD;
        else if (emo == "angry") _bmoMood = BMO_ANGRY;
        else if (emo == "surprised") _bmoMood = BMO_SURPRISED;
        else if (emo == "confused") _bmoMood = BMO_CONFUSED;
        else if (emo == "excited") _bmoMood = BMO_EXCITED;
        else if (emo == "tired") _bmoMood = BMO_TIRED;
        else if (emo == "worried") _bmoMood = BMO_WORRIED;
        else if (emo == "annoyed") _bmoMood = BMO_ANNOYED;
        else if (emo == "love") _bmoMood = BMO_LOVE;
        else if (emo == "scared") _bmoMood = BMO_SCARED;
        else if (emo == "proud") _bmoMood = BMO_PROUD;
        else if (emo == "silly") _bmoMood = BMO_SILLY;
        else if (emo == "sleepy") _bmoMood = BMO_SLEEPY;
        else if (emo == "thinking") _bmoMood = BMO_THINKING;
        else if (emo == "skeptical") _bmoMood = BMO_SKEPTICAL;
        else if (emo == "determined") _bmoMood = BMO_DETERMINED;
        else if (emo == "playful") _bmoMood = BMO_PLAYFUL;
        else if (emo == "bored") _bmoMood = BMO_BORED;
        else if (emo == "focused") _bmoMood = BMO_FOCUSED;
        else if (emo == "dizzy") _bmoMood = BMO_DIZZY;
        else if (emo == "wink") _bmoMood = BMO_WINK;
        else if (emo == "embarrassed") _bmoMood = BMO_EMBARRASSED;
        else if (emo == "curious") _bmoMood = BMO_CURIOUS;
        else _bmoMood = BMO_NEUTRAL;
    }
}

void LiveChatRenderer::setColors(uint16_t fg, uint16_t bg, uint16_t accent) {
    _fgColor = fg;
    _bgColor = bg;
    _accentColor = accent;
    
    // Rebuild Grobot eyes với màu mới
    if (_grobotEyes) {
        delete _grobotEyes;
        _grobotEyes = new GrobotEyes(_fgColor, _bgColor);
        _grobotEyes->setEmotion(MOOD_NEUTRAL);
    }
}

void LiveChatRenderer::pause() {
    _paused = true;
}

void LiveChatRenderer::resume() {
    _paused = false;
}

void LiveChatRenderer::update() {
    if (!_tft || !_canvas || _paused) return;
    
    uint32_t now = millis();
    float dt = (now - _lastUpdateMs) / 1000.0f;
    _lastUpdateMs = now;
    
    // Update animation phase
    _animPhase += dt * 2.0f;  // 2 rad/s
    if (_animPhase > TWO_PI) _animPhase -= TWO_PI;
    
    // Clear canvas
    clearCanvas();
    
    // Render based on current engine
    switch (_engine) {
        case ENGINE_GROBOT:
            renderGrobot();
            break;
        case ENGINE_KAWAII:
            renderKawaii();
            break;
        case ENGINE_BMO:
            renderBMO();
            break;
        case ENGINE_MOCHI:
            renderMochi();
            break;
        case ENGINE_EYES:
            renderEyes();
            break;
        default:
            renderGrobot();
            break;
    }
    
    // Render state-specific overlays
    switch (_state) {
        case CHAT_LISTENING:
            renderListeningOverlay();
            break;
        case CHAT_THINKING:
            renderThinkingOverlay();
            break;
        case CHAT_SPEAKING:
            renderSpeakingOverlay();
            break;
        case CHAT_INTERRUPTED:
            renderInterruptedOverlay();
            break;
        case CHAT_IDLE:
        default:
            // Idle - chỉ hiện face
            break;
    }
    
    // Push to screen
    pushCanvas();
}

// ==================== ENGINE RENDERERS ====================

void LiveChatRenderer::renderGrobot() {
    if (_grobotEyes) {
        _grobotEyes->renderEmotions(*_canvas);
    }
}

void LiveChatRenderer::renderKawaii() {
    // Kawaii style: 17 emotions với tears, sparkles, jaw tremble
    int16_t cx = CANVAS_W / 2;
    int16_t cy = CANVAS_H / 2;
    
    // Face circle
    _canvas->fillCircle(cx, cy, 60, _fgColor);
    
    // Draw eyes based on emotion
    drawKawaiiEyes(cx, cy, _kawaiiEmotion);
    
    // Draw mouth based on emotion
    drawKawaiiMouth(cx, cy, _kawaiiEmotion);
    
    // Draw blush based on emotion
    drawKawaiiBlush(cx, cy, _kawaiiEmotion);
    
    // Draw special effects
    if (_kawaiiTearLevel > 0) {
        drawKawaiiTears(cx, cy, _kawaiiTearLevel);
    }
    if (_kawaiiSparkleLevel > 0) {
        drawKawaiiSparkles(cx, cy, _kawaiiSparkleLevel);
    }
}

void LiveChatRenderer::renderBMO() {
    // BMO style: 25 moods với lip-sync, retro 8-bit pixel style
    int16_t cx = CANVAS_W / 2;
    int16_t cy = CANVAS_H / 2;
    
    // Screen bezel
    _canvas->fillRoundRect(cx - 80, cy - 70, 160, 120, 8, _fgColor);
    _canvas->fillRoundRect(cx - 75, cy - 65, 150, 110, 5, _bgColor);
    
    // Draw pixel eyes based on mood
    drawBMOEyes(cx, cy, _bmoMood);
    
    // Draw mouth with lip-sync
    drawBMOMouth(cx, cy, _bmoMood, _bmoMouthOpen);
    
    // Draw accessories (hearts, stars, etc.) based on mood
    drawBMOAccessory(cx, cy, _bmoMood);
}

void LiveChatRenderer::renderMochi() {
    // Mochi style: cute blob với bouncy animation
    int16_t cx = CANVAS_W / 2;
    int16_t cy = CANVAS_H / 2;
    
    // Bouncy effect
    float bounce = sin(_animPhase * 2.0f) * 5.0f;
    cy += (int16_t)bounce;
    
    // Blob body - soft rounded shape
    int16_t blobW = 100;
    int16_t blobH = 80;
    
    // Main blob
    _canvas->fillEllipse(cx, cy, blobW / 2, blobH / 2, _fgColor);
    
    // Squash effect (more width when at bottom)
    float squash = 1.0f + abs(bounce) / 20.0f;
    _canvas->fillEllipse(cx, cy + blobH / 3, 
                        (int16_t)(blobW / 2 * squash), 
                        (int16_t)(blobH / 4), _fgColor);
    
    // Simple dot eyes
    int16_t eyeY = cy - 10;
    _canvas->fillCircle(cx - 20, eyeY, 5, _bgColor);
    _canvas->fillCircle(cx + 20, eyeY, 5, _bgColor);
    
    // Tiny mouth
    _canvas->fillCircle(cx, cy + 10, 3, _bgColor);
}

void LiveChatRenderer::renderEyes() {
    // Realistic eyes style
    int16_t cx = CANVAS_W / 2;
    int16_t cy = CANVAS_H / 2;
    int16_t eyeSpacing = 60;
    int16_t eyeRadius = 30;
    
    // Left eye
    _canvas->fillCircle(cx - eyeSpacing, cy, eyeRadius, TFT_WHITE);
    _canvas->drawCircle(cx - eyeSpacing, cy, eyeRadius, _fgColor);
    
    // Left iris
    int16_t irisR = 15;
    _canvas->fillCircle(cx - eyeSpacing + _lookX / 10, cy + _lookY / 10, 
                       irisR, _fgColor);
    
    // Left pupil
    _canvas->fillCircle(cx - eyeSpacing + _lookX / 10, cy + _lookY / 10, 
                       irisR / 2, _bgColor);
    
    // Right eye
    _canvas->fillCircle(cx + eyeSpacing, cy, eyeRadius, TFT_WHITE);
    _canvas->drawCircle(cx + eyeSpacing, cy, eyeRadius, _fgColor);
    
    // Right iris
    _canvas->fillCircle(cx + eyeSpacing + _lookX / 10, cy + _lookY / 10, 
                       irisR, _fgColor);
    
    // Right pupil
    _canvas->fillCircle(cx + eyeSpacing + _lookX / 10, cy + _lookY / 10, 
                       irisR / 2, _bgColor);
}

// ==================== STATE OVERLAYS ====================

void LiveChatRenderer::renderListeningOverlay() {
    // Hiển thị waveform/spectrum khi đang nghe
    int16_t waveY = CANVAS_H - 60;
    
    // Title text
    _canvas->setTextColor(_accentColor, _bgColor);
    _canvas->setTextSize(1);
    _canvas->setCursor(10, waveY - 15);
    _canvas->print("Listening...");
    
    // Waveform hoặc spectrum
    if (_spectrumBands > 0) {
        drawSpectrum(10, waveY, CANVAS_W - 20, 40);
    } else {
        drawWaveform(10, waveY, CANVAS_W - 20, 40);
    }
    
    // Pulse ring around face
    int16_t cx = CANVAS_W / 2;
    int16_t cy = CANVAS_H / 2;
    drawPulseRing(cx, cy, _animPhase);
}

void LiveChatRenderer::renderThinkingOverlay() {
    // Animated dots "..."
    int16_t dotY = CANVAS_H - 40;
    int16_t cx = CANVAS_W / 2;
    
    _canvas->setTextColor(_accentColor, _bgColor);
    _canvas->setTextSize(1);
    _canvas->setCursor(cx - 30, dotY - 15);
    _canvas->print("Thinking");
    
    drawThinkingDots(cx, dotY);
    
    // Pulse effect
    drawPulseRing(cx, CANVAS_H / 2, _animPhase * 1.5f);
}

void LiveChatRenderer::renderSpeakingOverlay() {
    // Hiển thị mouth sync animation
    int16_t cx = CANVAS_W / 2;
    int16_t cy = CANVAS_H / 2 + 40;
    
    // Speaking indicator
    _canvas->setTextColor(_accentColor, _bgColor);
    _canvas->setTextSize(1);
    _canvas->setCursor(10, CANVAS_H - 50);
    _canvas->print("Speaking...");
    
    // Progress bar
    int16_t barW = CANVAS_W - 20;
    int16_t barH = 4;
    int16_t barY = CANVAS_H - 35;
    _canvas->drawRect(10, barY, barW, barH, _fgColor);
    _canvas->fillRect(10, barY, (int16_t)(barW * _speakingProgress), barH, _accentColor);
    
    // Mouth animation
    float openness = (sin(_animPhase * 8.0f) + 1.0f) / 2.0f * 0.8f;  // 0-0.8
    drawMouthShape(cx, cy, openness);
}

void LiveChatRenderer::renderInterruptedOverlay() {
    // Hiệu ứng interrupted - màn hình flash
    uint32_t elapsed = getStateTime();
    
    if (elapsed < 300) {  // Flash trong 300ms
        // Draw X or stop icon
        int16_t cx = CANVAS_W / 2;
        int16_t cy = CANVAS_H / 2;
        int16_t size = 40;
        
        _canvas->drawLine(cx - size, cy - size, cx + size, cy + size, TFT_RED);
        _canvas->drawLine(cx + size, cy - size, cx - size, cy + size, TFT_RED);
        _canvas->drawCircle(cx, cy, size + 5, TFT_RED);
        
        _canvas->setTextColor(TFT_RED, _bgColor);
        _canvas->setTextSize(2);
        _canvas->setCursor(cx - 50, cy + size + 20);
        _canvas->print("STOPPED");
    } else {
        // Sau flash, trở về idle
        setState(CHAT_IDLE);
    }
}

// ==================== HELPER EFFECTS ====================

void LiveChatRenderer::drawWaveform(int16_t x, int16_t y, int16_t w, int16_t h) {
    // Sine wave based on audio level
    int16_t midY = y + h / 2;
    int16_t amplitude = (int16_t)(h / 2 * _audioLevel);
    
    for (int16_t i = 0; i < w; i++) {
        float angle = (_animPhase + (float)i / w * TWO_PI * 3.0f);
        int16_t waveY = midY + (int16_t)(sin(angle) * amplitude);
        _canvas->drawPixel(x + i, waveY, _accentColor);
    }
}

void LiveChatRenderer::drawSpectrum(int16_t x, int16_t y, int16_t w, int16_t h) {
    // Spectrum bars
    if (_spectrumBands == 0) return;
    
    int16_t barW = w / _spectrumBands;
    for (uint8_t i = 0; i < _spectrumBands; i++) {
        int16_t barH = (int16_t)(h * _audioSpectrum[i]);
        int16_t barX = x + i * barW;
        int16_t barY = y + h - barH;
        _canvas->fillRect(barX + 1, barY, barW - 2, barH, _accentColor);
    }
}

void LiveChatRenderer::drawPulseRing(int16_t cx, int16_t cy, float phase) {
    // Animated ring pulse
    float scale = (sin(phase) + 1.0f) / 2.0f;  // 0-1
    int16_t radius = 80 + (int16_t)(20 * scale);
    uint8_t alpha = (uint8_t)(100 * (1.0f - scale));  // Fade out
    
    // Tạo màu với alpha (giả lập bằng blend)
    uint16_t color = _accentColor;
    if (alpha < 128) {
        color = _bgColor;  // Fade ra nền nếu quá mờ
    }
    
    _canvas->drawCircle(cx, cy, radius, color);
    _canvas->drawCircle(cx, cy, radius + 1, color);
}

void LiveChatRenderer::drawThinkingDots(int16_t cx, int16_t cy) {
    // Animated dots "..."
    int16_t spacing = 15;
    int16_t baseR = 4;
    
    for (int i = 0; i < 3; i++) {
        float dotPhase = _animPhase + i * (TWO_PI / 3.0f);
        int16_t r = baseR + (int16_t)(sin(dotPhase * 2.0f) * 2.0f);
        int16_t dotX = cx - spacing + i * spacing;
        _canvas->fillCircle(dotX, cy, r, _accentColor);
    }
}

void LiveChatRenderer::drawMouthShape(int16_t cx, int16_t cy, float openness) {
    // Mouth shape - oval that opens/closes
    int16_t mouthW = 40;
    int16_t mouthH = (int16_t)(20 * openness);
    
    if (mouthH > 2) {
        _canvas->fillEllipse(cx, cy, mouthW / 2, mouthH, TFT_RED);
    } else {
        // Closed mouth - line
        _canvas->drawFastHLine(cx - mouthW / 2, cy, mouthW, _fgColor);
    }
}

// ==================== KAWAII HELPER FUNCTIONS ====================

void LiveChatRenderer::drawKawaiiEyes(int16_t cx, int16_t cy, KawaiiEmotion emotion) {
    int16_t eyeY = cy - 15;
    int16_t eyeSpacing = 35;
    int16_t leftX = cx - eyeSpacing;
    int16_t rightX = cx + eyeSpacing;
    
    switch (emotion) {
        case KAWAII_HAPPY:
        case KAWAII_EXCITED:
        case KAWAII_LAUGHING:
            // Happy arcs
            for (int i = -8; i <= 8; i++) {
                int16_t y = eyeY + (i * i) / 16;
                _canvas->drawPixel(leftX + i, y, _bgColor);
                _canvas->drawPixel(leftX + i, y + 1, _bgColor);
                _canvas->drawPixel(rightX + i, y, _bgColor);
                _canvas->drawPixel(rightX + i, y + 1, _bgColor);
            }
            break;
            
        case KAWAII_SAD:
        case KAWAII_CRYING:
        case KAWAII_WORRIED:
            // Sad arcs
            for (int i = -8; i <= 8; i++) {
                int16_t y = eyeY - (i * i) / 16;
                _canvas->drawPixel(leftX + i, y, _bgColor);
                _canvas->drawPixel(leftX + i, y - 1, _bgColor);
                _canvas->drawPixel(rightX + i, y, _bgColor);
                _canvas->drawPixel(rightX + i, y - 1, _bgColor);
            }
            break;
            
        case KAWAII_ANGRY:
        case KAWAII_ANNOYED:
            // Angry slanted eyes
            _canvas->drawLine(leftX - 8, eyeY - 3, leftX + 8, eyeY + 3, _bgColor);
            _canvas->drawLine(leftX - 8, eyeY - 2, leftX + 8, eyeY + 4, _bgColor);
            _canvas->drawLine(rightX - 8, eyeY + 3, rightX + 8, eyeY - 3, _bgColor);
            _canvas->drawLine(rightX - 8, eyeY + 4, rightX + 8, eyeY - 2, _bgColor);
            break;
            
        case KAWAII_SURPRISED:
        case KAWAII_SHOCKED:
            // Wide open circles
            _canvas->fillCircle(leftX, eyeY, 10, _bgColor);
            _canvas->fillCircle(rightX, eyeY, 10, _bgColor);
            _canvas->fillCircle(leftX, eyeY, 4, _fgColor);  // Highlight
            _canvas->fillCircle(rightX, eyeY, 4, _fgColor);
            break;
            
        case KAWAII_LOVE:
            // Hearts
            _canvas->fillCircle(leftX - 3, eyeY - 2, 4, TFT_PINK);
            _canvas->fillCircle(leftX + 3, eyeY - 2, 4, TFT_PINK);
            _canvas->fillTriangle(leftX - 7, eyeY, leftX, eyeY + 6, leftX + 7, eyeY, TFT_PINK);
            _canvas->fillCircle(rightX - 3, eyeY - 2, 4, TFT_PINK);
            _canvas->fillCircle(rightX + 3, eyeY - 2, 4, TFT_PINK);
            _canvas->fillTriangle(rightX - 7, eyeY, rightX, eyeY + 6, rightX + 7, eyeY, TFT_PINK);
            break;
            
        case KAWAII_TIRED:
            // Half-closed eyes
            _canvas->drawFastHLine(leftX - 8, eyeY, 16, _bgColor);
            _canvas->drawFastHLine(leftX - 8, eyeY + 1, 16, _bgColor);
            _canvas->drawFastHLine(rightX - 8, eyeY, 16, _bgColor);
            _canvas->drawFastHLine(rightX - 8, eyeY + 1, 16, _bgColor);
            break;
            
        case KAWAII_SHY:
            // Looking down
            _canvas->fillCircle(leftX, eyeY + 5, 6, _bgColor);
            _canvas->fillCircle(rightX, eyeY + 5, 6, _bgColor);
            break;
            
        case KAWAII_SKEPTICAL:
        case KAWAII_CONFUSED:
            // One eye raised
            _canvas->fillEllipse(leftX, eyeY - 2, 8, 12, _bgColor);
            _canvas->fillEllipse(rightX, eyeY + 2, 8, 10, _bgColor);
            break;
            
        case KAWAII_DETERMINED:
            // Determined eyes
            _canvas->fillEllipse(leftX, eyeY, 6, 10, _bgColor);
            _canvas->fillEllipse(rightX, eyeY, 6, 10, _bgColor);
            _canvas->drawLine(leftX - 10, eyeY - 8, leftX + 10, eyeY - 6, _bgColor);
            _canvas->drawLine(rightX - 10, eyeY - 6, rightX + 10, eyeY - 8, _bgColor);
            break;
            
        default: // KAWAII_NEUTRAL
            // Normal oval eyes
            _canvas->fillEllipse(leftX, eyeY, 8, 12, _bgColor);
            _canvas->fillEllipse(rightX, eyeY, 8, 12, _bgColor);
            break;
    }
}

void LiveChatRenderer::drawKawaiiMouth(int16_t cx, int16_t cy, KawaiiEmotion emotion) {
    int16_t mouthY = cy + 20;
    
    switch (emotion) {
        case KAWAII_HAPPY:
        case KAWAII_EXCITED:
            // Big smile
            for (int i = -25; i <= 25; i++) {
                int16_t y = mouthY + (i * i) / 50;
                _canvas->drawPixel(cx + i, y, _bgColor);
                _canvas->drawPixel(cx + i, y + 1, _bgColor);
            }
            break;
            
        case KAWAII_LAUGHING:
            // Open mouth laughing
            _canvas->fillEllipse(cx, mouthY + 5, 15, 12, _bgColor);
            break;
            
        case KAWAII_SAD:
        case KAWAII_CRYING:
            // Sad frown
            for (int i = -20; i <= 20; i++) {
                int16_t y = mouthY - (i * i) / 50;
                _canvas->drawPixel(cx + i, y, _bgColor);
                _canvas->drawPixel(cx + i, y - 1, _bgColor);
            }
            break;
            
        case KAWAII_ANGRY:
        case KAWAII_ANNOYED:
            // Angry straight line
            _canvas->drawFastHLine(cx - 18, mouthY, 36, _bgColor);
            _canvas->drawFastHLine(cx - 18, mouthY + 1, 36, _bgColor);
            break;
            
        case KAWAII_SURPRISED:
        case KAWAII_SHOCKED:
            // Open O
            _canvas->fillCircle(cx, mouthY + 5, 10, _bgColor);
            _canvas->fillCircle(cx, mouthY + 5, 7, _fgColor);
            break;
            
        case KAWAII_LOVE:
            // Heart-shaped mouth
            _canvas->fillCircle(cx - 5, mouthY, 6, TFT_PINK);
            _canvas->fillCircle(cx + 5, mouthY, 6, TFT_PINK);
            _canvas->fillTriangle(cx - 10, mouthY + 2, cx, mouthY + 10, cx + 10, mouthY + 2, TFT_PINK);
            break;
            
        case KAWAII_WORRIED:
            // Wavy worried mouth
            for (int i = -15; i <= 15; i++) {
                int16_t y = mouthY + (int16_t)(sin((i / 5.0f) * TWO_PI) * 3);
                _canvas->drawPixel(cx + i, y, _bgColor);
            }
            break;
            
        case KAWAII_SHY:
            // Small smile
            for (int i = -12; i <= 12; i++) {
                int16_t y = mouthY + (i * i) / 40;
                _canvas->drawPixel(cx + i, y, _bgColor);
            }
            break;
            
        case KAWAII_TIRED:
            // Small open mouth
            _canvas->fillCircle(cx, mouthY + 3, 5, _bgColor);
            break;
            
        case KAWAII_SKEPTICAL:
            // Smirk
            _canvas->drawLine(cx - 15, mouthY, cx + 10, mouthY - 3, _bgColor);
            _canvas->drawLine(cx - 15, mouthY + 1, cx + 10, mouthY - 2, _bgColor);
            break;
            
        case KAWAII_CONFUSED:
            // Curved question
            for (int i = -10; i <= 10; i++) {
                int16_t y = mouthY - abs(i) / 3;
                _canvas->drawPixel(cx + i, y, _bgColor);
            }
            break;
            
        case KAWAII_DETERMINED:
            // Determined grin
            for (int i = -18; i <= 18; i++) {
                int16_t y = mouthY + abs(i) / 6;
                _canvas->drawPixel(cx + i, y, _bgColor);
            }
            break;
            
        default: // KAWAII_NEUTRAL
            // Simple smile
            for (int i = -20; i <= 20; i++) {
                int16_t y = mouthY + (i * i) / 40;
                _canvas->drawPixel(cx + i, y, _bgColor);
            }
            break;
    }
}

void LiveChatRenderer::drawKawaiiBlush(int16_t cx, int16_t cy, KawaiiEmotion emotion) {
    int16_t blushY = cy + 5;
    uint16_t blushColor = _accentColor;
    
    // Emotions with prominent blush
    if (emotion == KAWAII_SHY || emotion == KAWAII_LOVE) {
        blushColor = TFT_PINK;
        _canvas->fillCircle(cx - 50, blushY, 10, blushColor);
        _canvas->fillCircle(cx + 50, blushY, 10, blushColor);
    } else if (emotion == KAWAII_HAPPY || emotion == KAWAII_EXCITED) {
        _canvas->fillCircle(cx - 50, blushY, 8, blushColor);
        _canvas->fillCircle(cx + 50, blushY, 8, blushColor);
    } else if (emotion != KAWAII_ANGRY && emotion != KAWAII_ANNOYED) {
        // Subtle blush for most emotions
        _canvas->fillCircle(cx - 50, blushY, 6, blushColor);
        _canvas->fillCircle(cx + 50, blushY, 6, blushColor);
    }
}

void LiveChatRenderer::drawKawaiiTears(int16_t cx, int16_t cy, uint8_t level) {
    // Tears fall from eyes
    int16_t tearX1 = cx - 35;
    int16_t tearX2 = cx + 35;
    int16_t tearStartY = cy - 10;
    
    for (uint8_t i = 0; i < level && i < 3; i++) {
        int16_t dropY = tearStartY + 10 + i * 8 + (int16_t)(sin(_animPhase + i) * 3);
        // Teardrop shape
        _canvas->fillCircle(tearX1, dropY, 3, TFT_CYAN);
        _canvas->fillTriangle(tearX1 - 2, dropY, tearX1 + 2, dropY, tearX1, dropY + 5, TFT_CYAN);
        _canvas->fillCircle(tearX2, dropY, 3, TFT_CYAN);
        _canvas->fillTriangle(tearX2 - 2, dropY, tearX2 + 2, dropY, tearX2, dropY + 5, TFT_CYAN);
    }
}

void LiveChatRenderer::drawKawaiiSparkles(int16_t cx, int16_t cy, uint8_t level) {
    // Sparkles around face
    for (uint8_t i = 0; i < level && i < 6; i++) {
        float angle = _animPhase + i * (TWO_PI / 6);
        int16_t sparkX = cx + (int16_t)(cos(angle) * 70);
        int16_t sparkY = cy + (int16_t)(sin(angle) * 70);
        uint8_t size = 2 + (i % 2);
        
        // Star shape
        _canvas->drawLine(sparkX, sparkY - size, sparkX, sparkY + size, TFT_YELLOW);
        _canvas->drawLine(sparkX - size, sparkY, sparkX + size, sparkY, TFT_YELLOW);
        _canvas->drawPixel(sparkX - size + 1, sparkY - size + 1, TFT_YELLOW);
        _canvas->drawPixel(sparkX + size - 1, sparkY - size + 1, TFT_YELLOW);
        _canvas->drawPixel(sparkX - size + 1, sparkY + size - 1, TFT_YELLOW);
        _canvas->drawPixel(sparkX + size - 1, sparkY + size - 1, TFT_YELLOW);
    }
}

// ==================== BMO HELPER FUNCTIONS ====================

void LiveChatRenderer::drawBMOEyes(int16_t cx, int16_t cy, BMOMood mood) {
    int16_t eyeY = cy - 20;
    int16_t pixelSize = 6;
    int16_t leftX = cx - 30;
    int16_t rightX = cx + 20;
    
    // Helper lambda to draw pixel eye
    auto drawPixelBlock = [this, pixelSize](int16_t x, int16_t y, int cols, int rows) {
        for (int i = 0; i < cols; i++) {
            for (int j = 0; j < rows; j++) {
                _canvas->fillRect(x + i * pixelSize, y + j * pixelSize,
                                pixelSize - 1, pixelSize - 1, _fgColor);
            }
        }
    };
    
    switch (mood) {
        case BMO_HAPPY:
        case BMO_EXCITED:
        case BMO_PLAYFUL:
            // Happy eyes - arcs
            drawPixelBlock(leftX, eyeY + pixelSize, 2, 2);
            drawPixelBlock(rightX, eyeY + pixelSize, 2, 2);
            _canvas->fillRect(leftX - pixelSize, eyeY, pixelSize - 1, pixelSize - 1, _fgColor);
            _canvas->fillRect(leftX + 2 * pixelSize, eyeY, pixelSize - 1, pixelSize - 1, _fgColor);
            _canvas->fillRect(rightX - pixelSize, eyeY, pixelSize - 1, pixelSize - 1, _fgColor);
            _canvas->fillRect(rightX + 2 * pixelSize, eyeY, pixelSize - 1, pixelSize - 1, _fgColor);
            break;
            
        case BMO_SAD:
        case BMO_TIRED:
            // Sad eyes - inverted arcs
            drawPixelBlock(leftX, eyeY, 2, 2);
            drawPixelBlock(rightX, eyeY, 2, 2);
            _canvas->fillRect(leftX - pixelSize, eyeY + 2 * pixelSize, pixelSize - 1, pixelSize - 1, _fgColor);
            _canvas->fillRect(leftX + 2 * pixelSize, eyeY + 2 * pixelSize, pixelSize - 1, pixelSize - 1, _fgColor);
            _canvas->fillRect(rightX - pixelSize, eyeY + 2 * pixelSize, pixelSize - 1, pixelSize - 1, _fgColor);
            _canvas->fillRect(rightX + 2 * pixelSize, eyeY + 2 * pixelSize, pixelSize - 1, pixelSize - 1, _fgColor);
            break;
            
        case BMO_ANGRY:
        case BMO_ANNOYED:
            // Angry eyes - slanted
            drawPixelBlock(leftX - pixelSize, eyeY, 2, 1);
            drawPixelBlock(leftX, eyeY + pixelSize, 2, 2);
            drawPixelBlock(rightX, eyeY, 2, 1);
            drawPixelBlock(rightX - pixelSize, eyeY + pixelSize, 2, 2);
            break;
            
        case BMO_SURPRISED:
            // Wide open - 3x4 blocks
            drawPixelBlock(leftX - pixelSize, eyeY, 3, 4);
            drawPixelBlock(rightX - pixelSize, eyeY, 3, 4);
            break;
            
        case BMO_LOVE:
            // Hearts - pixel art hearts
            _canvas->fillRect(leftX - pixelSize, eyeY, pixelSize - 1, pixelSize - 1, TFT_PINK);
            _canvas->fillRect(leftX + pixelSize, eyeY, pixelSize - 1, pixelSize - 1, TFT_PINK);
            drawPixelBlock(leftX - pixelSize, eyeY + pixelSize, 3, 2);
            _canvas->fillRect(rightX - pixelSize, eyeY, pixelSize - 1, pixelSize - 1, TFT_PINK);
            _canvas->fillRect(rightX + pixelSize, eyeY, pixelSize - 1, pixelSize - 1, TFT_PINK);
            drawPixelBlock(rightX - pixelSize, eyeY + pixelSize, 3, 2);
            break;
            
        case BMO_SLEEPY:
            // Half closed
            drawPixelBlock(leftX, eyeY + pixelSize, 2, 1);
            drawPixelBlock(rightX, eyeY + pixelSize, 2, 1);
            break;
            
        case BMO_WINK:
            // One eye closed
            drawPixelBlock(leftX, eyeY + pixelSize, 2, 1);
            drawPixelBlock(rightX, eyeY, 2, 3);
            break;
            
        case BMO_DIZZY:
            // Spiral eyes
            _canvas->fillRect(leftX, eyeY, pixelSize - 1, pixelSize - 1, _fgColor);
            _canvas->fillRect(leftX + pixelSize, eyeY + pixelSize, pixelSize - 1, pixelSize - 1, _fgColor);
            _canvas->fillRect(rightX, eyeY, pixelSize - 1, pixelSize - 1, _fgColor);
            _canvas->fillRect(rightX + pixelSize, eyeY + pixelSize, pixelSize - 1, pixelSize - 1, _fgColor);
            break;
            
        default: // BMO_NEUTRAL and others
            // Normal 2x3 eyes
            drawPixelBlock(leftX, eyeY, 2, 3);
            drawPixelBlock(rightX, eyeY, 2, 3);
            break;
    }
}

void LiveChatRenderer::drawBMOMouth(int16_t cx, int16_t cy, BMOMood mood, float openness) {
    int16_t mouthY = cy + 20;
    int16_t pixelSize = 6;
    
    // Update mouth open for speaking (lip-sync)
    if (_state == CHAT_SPEAKING) {
        openness = (sin(_animPhase * 8.0f) + 1.0f) / 2.0f * 0.8f + 0.2f;
    }
    
    int16_t mouthWidth = 5 + (int16_t)(openness * 3);  // 5-8 pixels wide
    int16_t startX = cx - (mouthWidth * pixelSize) / 2;
    
    switch (mood) {
        case BMO_HAPPY:
        case BMO_EXCITED:
        case BMO_PLAYFUL:
            // Smile curve
            for (int i = 0; i < mouthWidth; i++) {
                int16_t offset = abs(i - mouthWidth / 2);
                _canvas->fillRect(startX + i * pixelSize, mouthY + offset * pixelSize / 2,
                                pixelSize - 1, pixelSize - 1, _fgColor);
            }
            break;
            
        case BMO_SAD:
        case BMO_TIRED:
            // Frown
            for (int i = 0; i < mouthWidth; i++) {
                int16_t offset = abs(i - mouthWidth / 2);
                _canvas->fillRect(startX + i * pixelSize, mouthY - offset * pixelSize / 2,
                                pixelSize - 1, pixelSize - 1, _fgColor);
            }
            break;
            
        case BMO_SURPRISED:
            // O shape
            for (int i = 0; i < 4; i++) {
                _canvas->fillRect(startX + i * pixelSize, mouthY, pixelSize - 1, pixelSize - 1, _fgColor);
                _canvas->fillRect(startX + i * pixelSize, mouthY + 2 * pixelSize, pixelSize - 1, pixelSize - 1, _fgColor);
            }
            _canvas->fillRect(startX, mouthY + pixelSize, pixelSize - 1, pixelSize - 1, _fgColor);
            _canvas->fillRect(startX + 3 * pixelSize, mouthY + pixelSize, pixelSize - 1, pixelSize - 1, _fgColor);
            break;
            
        default:
            // Simple line with lip-sync
            int16_t height = 1 + (int16_t)(openness * 2);
            for (int h = 0; h < height; h++) {
                for (int i = 0; i < mouthWidth; i++) {
                    _canvas->fillRect(startX + i * pixelSize, mouthY + h * pixelSize,
                                    pixelSize - 1, pixelSize - 1, _fgColor);
                }
            }
            break;
    }
}

void LiveChatRenderer::drawBMOAccessory(int16_t cx, int16_t cy, BMOMood mood) {
    int16_t pixelSize = 6;
    
    switch (mood) {
        case BMO_LOVE:
            // Floating hearts
            for (int i = 0; i < 2; i++) {
                int16_t heartX = cx - 60 + i * 120;
                int16_t heartY = cy - 50 + (int16_t)(sin(_animPhase + i * TWO_PI) * 10);
                _canvas->fillRect(heartX, heartY, pixelSize - 1, pixelSize - 1, TFT_PINK);
                _canvas->fillRect(heartX + pixelSize, heartY, pixelSize - 1, pixelSize - 1, TFT_PINK);
                _canvas->fillRect(heartX, heartY + pixelSize, pixelSize - 1, pixelSize - 1, TFT_PINK);
                _canvas->fillRect(heartX + pixelSize, heartY + pixelSize, pixelSize - 1, pixelSize - 1, TFT_PINK);
            }
            break;
            
        case BMO_CONFUSED:
        case BMO_CURIOUS:
            // Question mark
            _canvas->fillRect(cx - pixelSize, cy - 60, pixelSize - 1, pixelSize - 1, _fgColor);
            _canvas->fillRect(cx, cy - 60, pixelSize - 1, pixelSize - 1, _fgColor);
            _canvas->fillRect(cx + pixelSize, cy - 54, pixelSize - 1, pixelSize - 1, _fgColor);
            _canvas->fillRect(cx, cy - 48, pixelSize - 1, pixelSize - 1, _fgColor);
            _canvas->fillRect(cx, cy - 36, pixelSize - 1, pixelSize - 1, _fgColor);
            break;
            
        case BMO_ANGRY:
            // Anger marks
            _canvas->fillRect(cx - 70, cy - 30, pixelSize * 2, pixelSize - 1, TFT_RED);
            _canvas->fillRect(cx - 70 + pixelSize, cy - 30 - pixelSize, pixelSize - 1, pixelSize, TFT_RED);
            break;
            
        case BMO_EXCITED:
            // Stars
            for (int i = 0; i < 3; i++) {
                float angle = _animPhase + i * (TWO_PI / 3);
                int16_t starX = cx + (int16_t)(cos(angle) * 65);
                int16_t starY = cy + (int16_t)(sin(angle) * 50);
                _canvas->fillRect(starX, starY, pixelSize - 1, pixelSize - 1, TFT_YELLOW);
                _canvas->fillRect(starX - pixelSize, starY, pixelSize - 1, pixelSize - 1, TFT_YELLOW);
                _canvas->fillRect(starX + pixelSize, starY, pixelSize - 1, pixelSize - 1, TFT_YELLOW);
                _canvas->fillRect(starX, starY - pixelSize, pixelSize - 1, pixelSize - 1, TFT_YELLOW);
                _canvas->fillRect(starX, starY + pixelSize, pixelSize - 1, pixelSize - 1, TFT_YELLOW);
            }
            break;
            
        default:
            // No accessory
            break;
    }
}

// ==================== UTILITIES ====================

void LiveChatRenderer::clearCanvas() {
    if (_canvas) {
        _canvas->fillSprite(_bgColor);
    }
}

void LiveChatRenderer::pushCanvas() {
    if (_canvas && _tft) {
        _canvas->pushSprite(0, 0);
    }
}

uint32_t LiveChatRenderer::getStateTime() const {
    return millis() - _stateStartMs;
}

// ==================== GLOBAL FUNCTIONS ====================

void initLiveChat(TFT_eSPI* tft) {
    liveChatRenderer.init(tft);
}

void serviceLiveChat() {
    liveChatRenderer.update();
}

void setLiveChatState(ChatState state) {
    liveChatRenderer.setState(state);
}

ChatState getLiveChatState() {
    return liveChatRenderer.getState();
}

void setEmotionEngine(AnimEngine engine) {
    liveChatRenderer.setEngine(engine);
}
