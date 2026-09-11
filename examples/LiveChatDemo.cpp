/**
 * LiveChat UI Example - Demo cách sử dụng LiveChat interface
 * 
 * File này minh họa cách tích hợp LiveChat UI với Gemini Voice API
 * để tạo chatbot với giao diện đẹp mắt và tương tác tự nhiên
 */

#include <Arduino.h>
#include "LiveChatRenderer.h"
#include "UIManager.h"

// ==================== MOCK GEMINI API ====================
// Trong thực tế, thay bằng Gemini API thật

class MockGeminiAPI {
public:
    void startListening() {
        Serial.println("🎤 Gemini: Started listening...");
    }
    
    void sendAudio(const uint8_t* data, size_t len) {
        Serial.printf("📤 Gemini: Sent %d bytes audio\n", len);
    }
    
    bool isThinking() {
        return thinking;
    }
    
    bool hasResponse() {
        return hasResp;
    }
    
    const char* getResponse() {
        return "Xin chào! Tôi là AI assistant của bạn.";
    }
    
private:
    bool thinking = false;
    bool hasResp = false;
};

MockGeminiAPI gemini;

// ==================== DEMO FLOW ====================

void demoLiveChatFlow() {
    // Kiểm tra nếu đang ở LiveChat mode
    if (getUIMode() != MODE_LIVECHAT) {
        Serial.println("❌ Not in LiveChat mode!");
        return;
    }
    
    ChatState state = liveChatRenderer.getState();
    
    switch (state) {
        case CHAT_IDLE:
            Serial.println("💤 IDLE - Waiting for user tap to start...");
            // User tap -> serviceUI() sẽ tự động chuyển sang LISTENING
            break;
            
        case CHAT_LISTENING:
            Serial.println("🎤 LISTENING - Recording audio...");
            
            // Demo: Giả lập audio waveform
            static float phase = 0.0f;
            phase += 0.1f;
            float level = (sin(phase) + 1.0f) / 2.0f * 0.8f;
            liveChatRenderer.setAudioLevel(level);
            
            // Demo: Sau 3 giây tự động chuyển sang thinking
            static uint32_t listenStart = 0;
            if (listenStart == 0) listenStart = millis();
            if (millis() - listenStart > 3000) {
                liveChatRenderer.setState(CHAT_THINKING);
                listenStart = 0;
                gemini.sendAudio(nullptr, 0);
            }
            break;
            
        case CHAT_THINKING:
            Serial.println("🤔 THINKING - Processing with Gemini...");
            
            // Demo: Sau 2 giây có response
            static uint32_t thinkStart = 0;
            if (thinkStart == 0) thinkStart = millis();
            if (millis() - thinkStart > 2000) {
                liveChatRenderer.setState(CHAT_SPEAKING);
                liveChatRenderer.setSpeakingText(gemini.getResponse());
                thinkStart = 0;
            }
            break;
            
        case CHAT_SPEAKING:
            Serial.println("🗣️ SPEAKING - Bot is responding...");
            
            // Demo: Progress animation
            static uint32_t speakStart = 0;
            if (speakStart == 0) speakStart = millis();
            
            uint32_t elapsed = millis() - speakStart;
            float progress = min(1.0f, elapsed / 4000.0f);  // 4 giây
            liveChatRenderer.setSpeakingProgress(progress);
            
            if (progress >= 1.0f) {
                liveChatRenderer.setState(CHAT_IDLE);
                speakStart = 0;
                Serial.println("✅ Response complete!");
            }
            break;
            
        case CHAT_INTERRUPTED:
            Serial.println("⚠️ INTERRUPTED - User stopped bot");
            // Animation tự động, sau 300ms về IDLE
            break;
    }
}

// ==================== ENGINE DEMO ====================

void demoSwitchEngines() {
    static uint32_t lastSwitch = 0;
    static uint8_t currentEngine = 0;
    
    if (getUIMode() != MODE_LIVECHAT) return;
    
    // Đổi engine mỗi 10 giây
    if (millis() - lastSwitch > 10000) {
        currentEngine = (currentEngine + 1) % ENGINE_MAX;
        liveChatRenderer.setEngine((AnimEngine)currentEngine);
        
        const char* names[] = {"Grobot", "Kawaii", "BMO", "Mochi", "Eyes"};
        Serial.printf("🎨 Switched to engine: %s\n", names[currentEngine]);
        
        lastSwitch = millis();
    }
}

// ==================== COLOR DEMO ====================

void demoColorThemes() {
    static uint32_t lastChange = 0;
    static uint8_t theme = 0;
    
    if (getUIMode() != MODE_LIVECHAT) return;
    if (liveChatRenderer.getState() != CHAT_IDLE) return;
    
    // Đổi theme mỗi 15 giây (chỉ khi idle)
    if (millis() - lastChange > 15000) {
        theme = (theme + 1) % 4;
        
        switch (theme) {
            case 0:  // Classic
                liveChatRenderer.setColors(TFT_WHITE, TFT_BLACK, TFT_CYAN);
                Serial.println("🎨 Theme: Classic (White/Black/Cyan)");
                break;
            case 1:  // Neon
                liveChatRenderer.setColors(TFT_MAGENTA, 0x0010, TFT_YELLOW);
                Serial.println("🎨 Theme: Neon (Magenta/DarkBlue/Yellow)");
                break;
            case 2:  // Nature
                liveChatRenderer.setColors(TFT_GREEN, 0x1082, TFT_ORANGE);
                Serial.println("🎨 Theme: Nature (Green/Brown/Orange)");
                break;
            case 3:  // Cool
                liveChatRenderer.setColors(0x07FF, 0x0014, 0xF81F);
                Serial.println("🎨 Theme: Cool (Cyan/Navy/Pink)");
                break;
        }
        
        lastChange = millis();
    }
}

// ==================== EMOTION DEMO ====================

void demoEmotions() {
    static uint32_t lastChange = 0;
    static uint8_t emotion = 0;
    
    if (getUIMode() != MODE_LIVECHAT) return;
    
    ChatState state = liveChatRenderer.getState();
    
    // Thay đổi emotion dựa trên state
    if (state == CHAT_LISTENING) {
        liveChatRenderer.setEmotion("neutral");
    } else if (state == CHAT_THINKING) {
        // Alternate giữa neutral và wink
        if (millis() - lastChange > 500) {
            emotion = (emotion + 1) % 2;
            liveChatRenderer.setEmotion(emotion == 0 ? "neutral" : "wink");
            lastChange = millis();
        }
    } else if (state == CHAT_SPEAKING) {
        liveChatRenderer.setEmotion("happy");
    } else if (state == CHAT_INTERRUPTED) {
        liveChatRenderer.setEmotion("sad");
    }
}

// ==================== SERIAL COMMANDS ====================

void handleSerialCommands() {
    if (!Serial.available()) return;
    
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    
    if (cmd == "livechat") {
        setUIMode(MODE_LIVECHAT);
        Serial.println("✅ Entered LiveChat mode");
    }
    else if (cmd == "face") {
        setUIMode(MODE_FACE);
        Serial.println("✅ Returned to Face mode");
    }
    else if (cmd == "listen") {
        if (getUIMode() == MODE_LIVECHAT) {
            liveChatRenderer.setState(CHAT_LISTENING);
            Serial.println("✅ Started listening");
        }
    }
    else if (cmd == "think") {
        if (getUIMode() == MODE_LIVECHAT) {
            liveChatRenderer.setState(CHAT_THINKING);
            Serial.println("✅ Started thinking");
        }
    }
    else if (cmd == "speak") {
        if (getUIMode() == MODE_LIVECHAT) {
            liveChatRenderer.setState(CHAT_SPEAKING);
            Serial.println("✅ Started speaking");
        }
    }
    else if (cmd == "idle") {
        if (getUIMode() == MODE_LIVECHAT) {
            liveChatRenderer.setState(CHAT_IDLE);
            Serial.println("✅ Back to idle");
        }
    }
    else if (cmd == "interrupt") {
        if (getUIMode() == MODE_LIVECHAT) {
            liveChatRenderer.onInterrupt();
            Serial.println("✅ Interrupted");
        }
    }
    else if (cmd.startsWith("engine ")) {
        String eng = cmd.substring(7);
        if (eng == "grobot") liveChatRenderer.setEngine(ENGINE_GROBOT);
        else if (eng == "kawaii") liveChatRenderer.setEngine(ENGINE_KAWAII);
        else if (eng == "bmo") liveChatRenderer.setEngine(ENGINE_BMO);
        else if (eng == "mochi") liveChatRenderer.setEngine(ENGINE_MOCHI);
        else if (eng == "eyes") liveChatRenderer.setEngine(ENGINE_EYES);
        Serial.printf("✅ Switched to %s engine\n", eng.c_str());
    }
    else if (cmd == "help") {
        Serial.println("\n=== LiveChat Demo Commands ===");
        Serial.println("livechat     - Enter LiveChat mode");
        Serial.println("face         - Return to Face mode");
        Serial.println("listen       - Start listening");
        Serial.println("think        - Start thinking");
        Serial.println("speak        - Start speaking");
        Serial.println("idle         - Back to idle");
        Serial.println("interrupt    - Interrupt bot");
        Serial.println("engine [name]- Switch engine (grobot/kawaii/bmo/mochi/eyes)");
        Serial.println("help         - Show this help\n");
    }
}

// ==================== MAIN DEMO LOOP ====================
// Thêm vào loop() trong main.cpp

void liveChatDemo() {
    // Handle serial commands
    handleSerialCommands();
    
    // Run demo flow
    if (getUIMode() == MODE_LIVECHAT) {
        demoLiveChatFlow();
        // demoSwitchEngines();  // Uncomment để auto-switch engines
        // demoColorThemes();     // Uncomment để auto-change colors
        demoEmotions();           // Auto emotion changes
    }
}

// ==================== USAGE IN MAIN.CPP ====================
/*

// Trong main.cpp, thêm vào loop():

void loop() {
    serviceConfig();
    serviceNvs();
    if (getUIMode() == MODE_FACE) serviceFaceRenderer();
    serviceUI();
    serviceDebugOverlay();
    
    // THÊM DÒNG NÀY:
    liveChatDemo();  // Demo LiveChat features
}

// Test qua Serial Monitor (115200 baud):
// > livechat        → Vào LiveChat mode
// > listen          → Test listening state
// > think           → Test thinking animation
// > speak           → Test speaking animation
// > engine kawaii   → Đổi sang Kawaii style
// > help            → Xem tất cả commands

*/
