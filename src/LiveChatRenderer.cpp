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
    if (_grobotEyes) {
        // Map emotions
        if (strcmp(emotion, "happy") == 0) {
            _grobotEyes->setEmotion(MOOD_HAPPY);
        } else if (strcmp(emotion, "sad") == 0) {
            _grobotEyes->setEmotion(MOOD_SAD);
        } else if (strcmp(emotion, "angry") == 0) {
            _grobotEyes->setEmotion(MOOD_ANGRY);
        } else if (strcmp(emotion, "wink") == 0) {
            _grobotEyes->setEmotion(MOOD_WINK);
        } else {
            _grobotEyes->setEmotion(MOOD_NEUTRAL);
        }
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
    // Kawaii style: cute round face với blush
    int16_t cx = CANVAS_W / 2;
    int16_t cy = CANVAS_H / 2;
    
    // Face circle
    _canvas->fillCircle(cx, cy, 60, _fgColor);
    
    // Eyes - kawaii style (simple dots or ovals)
    int16_t eyeY = cy - 15;
    int16_t eyeSpacing = 35;
    
    // Left eye
    _canvas->fillEllipse(cx - eyeSpacing, eyeY, 8, 12, _bgColor);
    
    // Right eye
    _canvas->fillEllipse(cx + eyeSpacing, eyeY, 8, 12, _bgColor);
    
    // Blush marks
    _canvas->fillCircle(cx - 50, cy + 5, 8, _accentColor);
    _canvas->fillCircle(cx + 50, cy + 5, 8, _accentColor);
    
    // Mouth - smile arc
    int16_t mouthY = cy + 20;
    for (int i = -20; i <= 20; i++) {
        int16_t x = cx + i;
        int16_t y = mouthY + (i * i) / 40;  // Parabola
        _canvas->drawPixel(x, y, _bgColor);
        _canvas->drawPixel(x, y + 1, _bgColor);
    }
}

void LiveChatRenderer::renderBMO() {
    // BMO style: retro game console look
    int16_t cx = CANVAS_W / 2;
    int16_t cy = CANVAS_H / 2;
    
    // Screen bezel
    _canvas->fillRoundRect(cx - 80, cy - 70, 160, 120, 8, _fgColor);
    _canvas->fillRoundRect(cx - 75, cy - 65, 150, 110, 5, _bgColor);
    
    // Pixel eyes - retro 8-bit style
    int16_t eyeY = cy - 20;
    int16_t pixelSize = 6;
    
    // Left eye (2x3 pixels)
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            _canvas->fillRect(cx - 30 + i * pixelSize, eyeY + j * pixelSize, 
                            pixelSize - 1, pixelSize - 1, _fgColor);
        }
    }
    
    // Right eye (2x3 pixels)
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            _canvas->fillRect(cx + 20 + i * pixelSize, eyeY + j * pixelSize, 
                            pixelSize - 1, pixelSize - 1, _fgColor);
        }
    }
    
    // Mouth - simple line
    _canvas->drawFastHLine(cx - 20, cy + 20, 40, _fgColor);
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
