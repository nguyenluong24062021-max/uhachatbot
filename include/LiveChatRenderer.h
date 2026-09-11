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

// Kawaii emotions (17 total)
enum KawaiiEmotion : uint8_t {
    KAWAII_NEUTRAL = 0,
    KAWAII_HAPPY,
    KAWAII_SAD,
    KAWAII_ANGRY,
    KAWAII_SURPRISED,
    KAWAII_CONFUSED,
    KAWAII_EXCITED,
    KAWAII_TIRED,
    KAWAII_CRYING,
    KAWAII_LAUGHING,
    KAWAII_LOVE,
    KAWAII_WORRIED,
    KAWAII_ANNOYED,
    KAWAII_SHOCKED,
    KAWAII_SHY,
    KAWAII_SKEPTICAL,
    KAWAII_DETERMINED
};

// BMO moods (25 total)
enum BMOMood : uint8_t {
    BMO_NEUTRAL = 0,
    BMO_HAPPY,
    BMO_SAD,
    BMO_ANGRY,
    BMO_SURPRISED,
    BMO_CONFUSED,
    BMO_EXCITED,
    BMO_TIRED,
    BMO_WORRIED,
    BMO_ANNOYED,
    BMO_LOVE,
    BMO_SCARED,
    BMO_PROUD,
    BMO_SILLY,
    BMO_SLEEPY,
    BMO_THINKING,
    BMO_SKEPTICAL,
    BMO_DETERMINED,
    BMO_PLAYFUL,
    BMO_BORED,
    BMO_FOCUSED,
    BMO_DIZZY,
    BMO_WINK,
    BMO_EMBARRASSED,
    BMO_CURIOUS
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
    
    // Kawaii engine state
    KawaiiEmotion _kawaiiEmotion;
    uint8_t _kawaiiTearLevel;      // 0-10 for crying
    uint8_t _kawaiiSparkleLevel;   // 0-10 for excitement
    uint8_t _kawaiiJawTremble;     // 0-10 for fear/anger
    
    // BMO engine state
    BMOMood _bmoMood;
    float _bmoMouthOpen;           // 0.0-1.0 for lip sync
    uint8_t _bmoMouthShape;        // 0-4 different mouth shapes
    
    // Engine-specific rendering
    void renderGrobot();
    void renderKawaii();
    void renderBMO();
    void renderMochi();
    void renderEyes();
    
    // Kawaii helper functions
    void drawKawaiiEyes(int16_t cx, int16_t cy, KawaiiEmotion emotion);
    void drawKawaiiMouth(int16_t cx, int16_t cy, KawaiiEmotion emotion);
    void drawKawaiiBlush(int16_t cx, int16_t cy, KawaiiEmotion emotion);
    void drawKawaiiTears(int16_t cx, int16_t cy, uint8_t level);
    void drawKawaiiSparkles(int16_t cx, int16_t cy, uint8_t level);
    
    // BMO helper functions
    void drawBMOEyes(int16_t cx, int16_t cy, BMOMood mood);
    void drawBMOMouth(int16_t cx, int16_t cy, BMOMood mood, float openness);
    void drawBMOAccessory(int16_t cx, int16_t cy, BMOMood mood);
    
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
