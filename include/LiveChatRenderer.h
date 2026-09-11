#pragma once
#include <TFT_eSPI.h>
#include <stdint.h>

// Trạng thái live chat
enum ChatState : uint8_t {
    CHAT_IDLE = 0,      // Chờ đợi, hiện face bình thường
    CHAT_LISTENING,     // Đang nghe người dùng nói
    CHAT_THINKING,      // Đang xử lý, gửi lên Gemini
    CHAT_SPEAKING,      // Bot đang trả lời
    CHAT_INTERRUPTED    // Người dùng ngắt lời bot
};

// Loại animation engine để mix
enum AnimEngine : uint8_t {
    ENGINE_GROBOT = 0,  // Grobot_Animations (đã có)
    ENGINE_KAWAII,      // lvgl_kawaii_face style
    ENGINE_BMO,         // BMO-ESP32 retro style
    ENGINE_MOCHI,       // Mochi cute blob style
    ENGINE_EYES,        // esp32-eyes realistic
    ENGINE_MAX
};

// Cấu hình emotion cho từng trạng thái
struct EmotionConfig {
    AnimEngine engine;
    const char* emotionName;
    uint16_t fgColor;      // Màu chính (eye/face)
    uint16_t bgColor;      // Màu nền
    uint16_t accentColor;  // Màu nhấn (blush, effects)
};

// Live chat renderer class
class LiveChatRenderer {
public:
    LiveChatRenderer();
    ~LiveChatRenderer();
    
    void init(TFT_eSPI* tft);
    void update();  // Gọi mỗi frame trong loop()
    
    // Điều khiển trạng thái
    void setState(ChatState state);
    ChatState getState() const { return _state; }
    
    // Điều khiển engine
    void setEngine(AnimEngine engine);
    AnimEngine getEngine() const { return _engine; }
    
    // Voice visualization cho listening state
    void setAudioLevel(float level);  // 0.0 - 1.0
    void setAudioSpectrum(const float* spectrum, uint8_t bands);
    
    // Speaking animation
    void setSpeakingText(const char* text);  // Text đang được nói
    void setSpeakingProgress(float progress); // 0.0 - 1.0
    
    // Interrupt handling
    void onInterrupt();  // User ngắt lời
    void clearInterrupt();
    
    // Effects
    void showPulse();      // Pulse effect khi thinking
    void showWaveform();   // Waveform khi listening
    void showMouthSync();  // Mouth sync khi speaking
    
    // Emotion control
    void setEmotion(const char* emotion);
    void setColors(uint16_t fg, uint16_t bg, uint16_t accent);
    
    // Pause/Resume
    void pause();
    void resume();
    bool isPaused() const { return _paused; }

private:
    TFT_eSPI* _tft;
    TFT_eSprite* _canvas;
    
    ChatState _state;
    ChatState _prevState;
    AnimEngine _engine;
    
    bool _paused;
    uint32_t _stateStartMs;
    uint32_t _lastUpdateMs;
    
    // Audio visualization data
    float _audioLevel;
    float _audioSpectrum[16];
    uint8_t _spectrumBands;
    
    // Speaking data
    char _speakingText[256];
    float _speakingProgress;
    
    // Colors
    uint16_t _fgColor;
    uint16_t _bgColor;
    uint16_t _accentColor;
    
    // Animation state
    float _animPhase;
    uint8_t _blinkCounter;
    int16_t _lookX, _lookY;
    
    // Engine-specific rendering
    void renderGrobot();
    void renderKawaii();
    void renderBMO();
    void renderMochi();
    void renderEyes();
    
    // State-specific overlays
    void renderListeningOverlay();
    void renderThinkingOverlay();
    void renderSpeakingOverlay();
    void renderInterruptedOverlay();
    
    // Helper effects
    void drawWaveform(int16_t x, int16_t y, int16_t w, int16_t h);
    void drawSpectrum(int16_t x, int16_t y, int16_t w, int16_t h);
    void drawPulseRing(int16_t cx, int16_t cy, float phase);
    void drawThinkingDots(int16_t cx, int16_t cy);
    void drawMouthShape(int16_t cx, int16_t cy, float openness);
    
    // Utilities
    void clearCanvas();
    void pushCanvas();
    uint32_t getStateTime() const;
};

// Global instance
extern LiveChatRenderer liveChatRenderer;

// Convenience functions
void initLiveChat(TFT_eSPI* tft);
void serviceLiveChat();
void setLiveChatState(ChatState state);
ChatState getLiveChatState();
