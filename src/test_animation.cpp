// Test Animation trên ESP32
// File này test riêng các animations mà không cần UI phức tạp

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "LiveChatRenderer.h"

TFT_eSPI tft = TFT_eSPI();

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=================================");
    Serial.println("🎨 TEST ANIMATION TRÊN ESP32");
    Serial.println("=================================\n");
    
    // Khởi tạo màn hình
    tft.init();
    tft.setRotation(1); // Landscape mode
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println("Animation Test");
    
    Serial.println("✅ Màn hình đã khởi tạo");
    
    // Khởi tạo LiveChat renderer
    initLiveChat(&tft);
    Serial.println("✅ LiveChat renderer đã sẵn sàng\n");
    
    // Hiển thị menu commands
    Serial.println("📝 CÁC LỆNH TEST:");
    Serial.println("  idle      - Test Idle animation");
    Serial.println("  listen    - Test Listening animation");
    Serial.println("  think     - Test Thinking animation");
    Serial.println("  speak     - Test Speaking animation");
    Serial.println("  error     - Test Error animation");
    Serial.println("  kawaii    - Chuyển sang Kawaii engine");
    Serial.println("  mochi     - Chuyển sang Mochi engine");
    Serial.println("  grobot    - Chuyển sang Grobot engine");
    Serial.println("  simple    - Chuyển sang Simple engine");
    Serial.println("  geo       - Chuyển sang Geometric engine");
    Serial.println("  clear     - Xóa màn hình");
    Serial.println("  help      - Hiển thị menu này");
    Serial.println("\n💡 Gõ lệnh và nhấn Enter để test!\n");
}

void loop() {
    static String inputBuffer = "";
    static unsigned long lastAnimTime = 0;
    static ChatState currentState = CHAT_IDLE;
    
    // Đọc serial commands
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
            if (inputBuffer.length() > 0) {
                String cmd = inputBuffer;
                cmd.trim();
                cmd.toLowerCase();
                
                Serial.println("\n>>> Command: " + cmd);
                
                // Xử lý commands
                if (cmd == "idle") {
                    Serial.println("🔵 Testing IDLE animation...");
                    currentState = CHAT_IDLE;
                    setLiveChatState(CHAT_IDLE);
                    
                } else if (cmd == "listen") {
                    Serial.println("🎤 Testing LISTENING animation...");
                    currentState = CHAT_LISTENING;
                    setLiveChatState(CHAT_LISTENING);
                    
                } else if (cmd == "think") {
                    Serial.println("🤔 Testing THINKING animation...");
                    currentState = CHAT_THINKING;
                    setLiveChatState(CHAT_THINKING);
                    
                } else if (cmd == "speak") {
                    Serial.println("💬 Testing SPEAKING animation...");
                    currentState = CHAT_SPEAKING;
                    setLiveChatState(CHAT_SPEAKING);
                    
                } else if (cmd == "error") {
                    Serial.println("❌ Testing ERROR animation...");
                    currentState = CHAT_ERROR;
                    setLiveChatState(CHAT_ERROR);
                    
                } else if (cmd == "kawaii") {
                    Serial.println("🌸 Switching to KAWAII engine...");
                    setEmotionEngine(ENGINE_KAWAII);
                    Serial.println("   Kawaii: Anime-style với emoji kawaii");
                    
                } else if (cmd == "mochi") {
                    Serial.println("🫧 Switching to MOCHI engine...");
                    setEmotionEngine(ENGINE_MOCHI);
                    Serial.println("   Mochi: Bouncy blob với physics");
                    
                } else if (cmd == "grobot") {
                    Serial.println("🤖 Switching to GROBOT engine...");
                    setEmotionEngine(ENGINE_GROBOT);
                    Serial.println("   Grobot: Animations từ Grobot library");
                    
                } else if (cmd == "simple") {
                    Serial.println("⚪ Switching to SIMPLE engine...");
                    setEmotionEngine(ENGINE_SIMPLE);
                    Serial.println("   Simple: Basic shapes, ít RAM");
                    
                } else if (cmd == "geo") {
                    Serial.println("📐 Switching to GEOMETRIC engine...");
                    setEmotionEngine(ENGINE_GEOMETRIC);
                    Serial.println("   Geometric: Abstract geometric patterns");
                    
                } else if (cmd == "clear") {
                    Serial.println("🧹 Clearing screen...");
                    tft.fillScreen(TFT_BLACK);
                    
                } else if (cmd == "help") {
                    Serial.println("\n📝 HELP - Available commands:");
                    Serial.println("  STATES: idle, listen, think, speak, error");
                    Serial.println("  ENGINES: kawaii, mochi, grobot, simple, geo");
                    Serial.println("  OTHER: clear, help");
                    
                } else {
                    Serial.println("❌ Unknown command: " + cmd);
                    Serial.println("   Type 'help' for available commands");
                }
                
                Serial.println();
                inputBuffer = "";
            }
        } else {
            inputBuffer += c;
        }
    }
    
    // Update animation mỗi 50ms
    if (millis() - lastAnimTime >= 50) {
        updateLiveChat();
        lastAnimTime = millis();
    }
    
    // Delay nhỏ để không lag serial
    delay(10);
}
