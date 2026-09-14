# 📖 HƯỚNG DẪN SỬ DỤNG - UHA CHATBOT

> **Project:** ESP32 Chatbot với LCD Touch Screen và LiveChat Animations  
> **Version:** c369513-dirty  
> **Last Updated:** 2026-09-14

---

## 📑 MỤC LỤC

1. [Tổng quan Project](#1-tổng-quan-project)
2. [Yêu cầu Phần cứng](#2-yêu-cầu-phần-cứng)
3. [Cài đặt Phần mềm](#3-cài-đặt-phần-mềm)
4. [Build và Upload](#4-build-và-upload)
5. [Test trên Wokwi Simulator](#5-test-trên-wokwi-simulator)
6. [Test trên Hardware](#6-test-trên-hardware)
7. [Sử dụng LiveChat Animations](#7-sử-dụng-livechat-animations)
8. [Các Engine Animations](#8-các-engine-animations)
9. [Serial Commands](#9-serial-commands)
10. [Xử lý Lỗi Thường gặp](#10-xử-lý-lỗi-thường-gặp)

---

## 1. TỔNG QUAN PROJECT

### Mô tả
ESP32 Chatbot với màn hình cảm ứng ST7789 240x320, hỗ trợ:
- ✅ UI/Menu system với GUIslice
- ✅ LiveChat animations với 5 engines khác nhau
- ✅ Touch calibration và handler
- ✅ Brightness control (manual + auto LDR)
- ✅ Serial commands để test
- ✅ Wokwi simulator support

### Cấu trúc Project
```
uhachatbot/
├── platformio.ini          # Build config
├── wokwi.toml             # Wokwi simulator config
├── diagram.json           # Hardware diagram
├── PINOUT.md              # Pin connections
├── include/               # Header files
│   ├── LiveChatRenderer.h # Animation engine
│   ├── UIManager.h        # UI/Menu manager
│   └── Config.h           # Configuration
├── src/                   # Source code
│   ├── main.cpp           # Main app với UI
│   ├── test_animation.cpp # Test app (chỉ animations)
│   └── LiveChatRenderer.cpp # Animation implementation
└── docs/                  # Documentation
    ├── SETUP_GUIDE.md
    ├── ANIMATION_TEST_GUIDE.md
    └── QUICK_TEST_COMMANDS.md
```

---

## 2. YÊU CẦU PHẦN CỨNG

### Hardware cần thiết:
- **ESP32 DevKit V1** hoặc tương tự
- **LCD ST7789 240x320** với SPI interface
- **Touch Screen XPT2046** (resistive)
- **Cáp USB** để nạp code
- (Optional) **LDR sensor** để auto brightness

### Pinout (xem chi tiết trong [`PINOUT.md`](PINOUT.md)):

| Chức năng | GPIO | Ghi chú |
|-----------|------|---------|
| LCD MOSI  | 26   | SPI shared |
| LCD SCLK  | 25   | SPI shared |
| LCD MISO  | 34   | SPI shared |
| LCD CS    | 13   | Chip select |
| LCD DC    | 27   | Data/Command |
| LCD RST   | 14   | Reset |
| LCD BL    | 12   | Backlight (PWM) |
| Touch CS  | 23   | Touch chip select |
| Touch IRQ | 35   | Touch interrupt |
| Touch MOSI| 4    | Touch data in |
| Touch MISO| 32   | Touch data out |
| Touch CLK | 33   | Touch clock |

---

## 3. CÀI ĐẶT PHẦN MỀM

### Bước 1: Cài VSCode
- Download: https://code.visualstudio.com/
- Install và khởi động

### Bước 2: Cài PlatformIO Extension
1. Trong VSCode nhấn `Ctrl+Shift+X`
2. Tìm: **"PlatformIO IDE"**
3. Click **Install**
4. Đợi 5-10 phút để cài đặt
5. Reload VSCode

### Bước 3: Cài Wokwi Extension (Optional - để simulator)
1. Trong VSCode nhấn `Ctrl+Shift+X`
2. Tìm: **"Wokwi Simulator"**
3. Click **Install**

### Bước 4: Mở Project
1. `File → Open Folder → d:/uhachatbot`
2. PlatformIO tự động detect project
3. Chờ download libraries (TFT_eSPI, Grobot_Animations, GUIslice, XPT2046)

---

## 4. BUILD VÀ UPLOAD

### Build Main App (với UI đầy đủ)

#### Cấu hình trong `platformio.ini`:
```ini
build_src_filter = +<*> -<test_animation.cpp>
```

#### Build:
```bash
# Dùng Terminal
pio run

# Hoặc trong VSCode PlatformIO sidebar:
# Project Tasks → Esp32dev → General → Build
```

#### Upload lên ESP32:
```bash
# Dùng Terminal
pio run -t upload

# Hoặc trong VSCode:
# Project Tasks → Esp32dev → General → Upload
```

### Build Test Animation App (chỉ test animations)

#### Sửa `platformio.ini`:
```ini
build_src_filter = +<test_animation.cpp> +<LiveChatRenderer.cpp> -<main.cpp> -<Config.cpp> -<UIManager.cpp> -<MenuCallbacks.cpp> -<TouchHandler.cpp> -<TouchTest.cpp> -<TouchCalib.cpp> -<DebugOverlay.cpp> -<LdrSensor.cpp> -<LED_Solution.cpp> -<FaceRenderer.cpp> -<DisplayInit.cpp> -<icons/>
```

#### Build và Upload:
```bash
pio run -t upload
```

---

## 5. TEST TRÊN WOKWI SIMULATOR

### Chuẩn bị:

#### File `wokwi.toml`:
```toml
[wokwi]
version = 1
firmware = '.pio/build/Esp32dev/firmware.bin'
elf = '.pio/build/Esp32dev/firmware.elf'
gdbServerPort = 3333

[serial]
port = 115200
```

#### File `diagram.json`:
- ESP32 DevKit V1
- ILI9341 LCD 240x320 (tương thích ST7789)
- Serial Monitor connection

### Chạy Simulator:

1. **Build firmware trước:**
   ```bash
   pio run
   ```

2. **Start Wokwi:**
   - Click chuột phải vào `diagram.json`
   - Chọn "Start Wokwi Simulator"
   
   Hoặc:
   - `F1` → gõ "Wokwi: Start Simulator" → Enter

3. **Xem Serial Monitor:**
   - Output hiển thị trong panel Wokwi
   - Có thể gõ lệnh test (xem phần 9)

### Lưu ý với Wokwi:
- ⚠️ **Sprite allocation failed**: Wokwi RAM giới hạn, không đủ cho sprite 320x240. Đây là bình thường.
- ⚠️ **NVS errors**: Wokwi không có NVS storage, code dùng fallback.
- ⚠️ **Touch không hoạt động**: Wokwi không hỗ trợ touch screen thật.
- ✅ **Firmware vẫn chạy OK**, UI hiển thị, Serial commands hoạt động.

---

## 6. TEST TRÊN HARDWARE

### Kết nối Hardware:
1. Đấu nối theo pinout trong [`PINOUT.md`](PINOUT.md)
2. Kiểm tra nguồn 3.3V và GND
3. Cắm USB vào ESP32

### Upload Firmware:
```bash
pio run -t upload
```

### Mở Serial Monitor:
```bash
pio device monitor

# Hoặc trong VSCode:
# Project Tasks → Esp32dev → General → Monitor
```

### First Boot:
1. **Touch Calibration:**
   - Chạm 4 góc màn hình theo hướng dẫn
   - Calibration data lưu vào NVS

2. **Menu chính:**
   - Logo RSC hiển thị
   - Menu icons: Settings, Volume, Mic, Help
   - Touch để navigate

### Chi tiết xem [`TEST_ON_HARDWARE.md`](TEST_ON_HARDWARE.md)

---

## 7. SỬ DỤNG LIVECHAT ANIMATIONS

### Khái niệm:

**LiveChat Animations** là hệ thống vẽ mặt/biểu cảm animated cho chatbot, gồm:

- **5 Animation Engines**: Grobot, Kawaii, BMO, Mochi, Eyes
- **4 Chat States**: IDLE, LISTENING, THINKING, SPEAKING
- **Emotion System**: Mỗi engine hỗ trợ nhiều emotions khác nhau

### Flow hoạt động:

```
[User speaks] → LISTENING state → [Process] → THINKING state 
              → [Bot responds] → SPEAKING state → [Done] → IDLE
```

### API trong code:

```cpp
#include "LiveChatRenderer.h"

// Init
initLiveChat(&tft);

// Change state
setLiveChatState(CHAT_LISTENING);  // Đang nghe
setLiveChatState(CHAT_THINKING);   // Đang xử lý
setLiveChatState(CHAT_SPEAKING);   // Đang nói

// Change engine
liveChatRenderer.setEngine(ENGINE_KAWAII);
liveChatRenderer.setEngine(ENGINE_BMO);

// Update animation (gọi trong loop)
serviceLiveChat();
```

---

## 8. CÁC ENGINE ANIMATIONS

### 1. ENGINE_GROBOT
- **Nguồn**: Grobot_Animations library
- **Style**: Round eyes với emotions
- **Moods**: NEUTRAL, HAPPY, SAD, ANGRY, WINK
- **Đặc điểm**: Đơn giản, ít RAM

### 2. ENGINE_KAWAII  
- **Nguồn**: Inspired by lvgl_kawaii_face
- **Style**: Anime/manga cute style
- **Emotions**: 17 emotions khác nhau
  - NEUTRAL, HAPPY, SAD, ANGRY, SURPRISED
  - CONFUSED, EXCITED, TIRED, CRYING
  - LAUGHING, LOVE, WORRIED, ANNOYED
  - SHOCKED, SHY, SKEPTICAL, DETERMINED
- **Effects**: 
  - Tears (crying)
  - Sparkles (excited/love)
  - Blush (shy/happy)
  - Jaw tremble (angry/scared)

### 3. ENGINE_BMO
- **Nguồn**: BMO-ESP32 retro style
- **Style**: 8-bit pixel art (retro game console)
- **Moods**: 25 moods
  - NEUTRAL, HAPPY, SAD, ANGRY, SURPRISED
  - CONFUSED, EXCITED, TIRED, WORRIED, ANNOYED
  - LOVE, SCARED, PROUD, SILLY, SLEEPY
  - THINKING, SKEPTICAL, DETERMINED, PLAYFUL
  - BORED, FOCUSED, DIZZY, WINK, EMBARRASSED, CURIOUS
- **Features**:
  - Pixel art eyes/mouth (6x6 blocks)
  - Lip-sync khi SPEAKING
  - Accessories (hearts, stars, question marks)
  - Retro bezel (màn hình vuông)

### 4. ENGINE_MOCHI
- **Style**: Cute bouncy blob
- **Đặc điểm**:
  - Blob hình ellipse mềm mại
  - Bouncing animation (nhảy nhẹ)
  - Squash/stretch effect
  - Đơn giản: 2 mắt dot + miệng nhỏ

### 5. ENGINE_EYES
- **Nguồn**: esp32-eyes realistic
- **Style**: Realistic eyeballs
- **Đặc điểm**:
  - 2 mắt hình tròn như thật
  - Iris và pupil di chuyển
  - Look direction tracking
  - White sclera + colored iris

---

## 9. SERIAL COMMANDS

### Test Animation Commands (trong `test_animation.cpp`):

#### States:
```
idle      - Chuyển sang IDLE state
listen    - Chuyển sang LISTENING state  
think     - Chuyển sang THINKING state
speak     - Chuyển sang SPEAKING state
error     - Chuyển sang INTERRUPTED state
```

#### Engines:
```
kawaii    - Chuyển sang Kawaii engine
mochi     - Chuyển sang Mochi engine
grobot    - Chuyển sang Grobot engine
bmo       - Chuyển sang BMO engine
eyes      - Chuyển sang Eyes engine
```

#### Khác:
```
clear     - Xóa màn hình
help      - Hiển thị menu commands
```

### Ví dụ Test Workflow:

```bash
# Serial Monitor - Gõ từng lệnh và Enter

# Test Kawaii engine
kawaii
idle
listen
think
speak

# Test BMO engine
bmo
idle
listen
think
speak

# Test Mochi
mochi
idle
speak

# Clear screen
clear
```

### Config Commands (trong main app):

Xem chi tiết trong [`Config.cpp`](src/Config.cpp):

```
# Brightness
bright 50         - Set brightness 50%
bright auto       - Enable auto brightness

# Engine
engine kawaii     - Switch to Kawaii
engine bmo        - Switch to BMO

# Volume
volume 80         - Set volume 80%
volume mute       - Mute audio

# Emotion (debug)
emotion happy     - Set emotion
emotion sad       - Set emotion
```

---

## 10. XỬ LÝ LỖI THƯỜNG GẶP

### Build Errors:

#### Lỗi: `multiple definition of setup()`
**Nguyên nhân**: Build cả `main.cpp` và `test_animation.cpp` cùng lúc

**Giải pháp**: Sửa `platformio.ini`:
```ini
# Để chạy main app:
build_src_filter = +<*> -<test_animation.cpp>

# Để chạy test app:
build_src_filter = +<test_animation.cpp> +<LiveChatRenderer.cpp> -<main.cpp> -<Config.cpp> -<UIManager.cpp> -<MenuCallbacks.cpp> -<TouchHandler.cpp> -<TouchTest.cpp> -<TouchCalib.cpp> -<DebugOverlay.cpp> -<LdrSensor.cpp> -<LED_Solution.cpp> -<FaceRenderer.cpp> -<DisplayInit.cpp> -<icons/>
```

#### Lỗi: `undefined reference to initLiveChat`
**Nguyên nhân**: Thiếu `LiveChatRenderer.cpp` trong build

**Giải pháp**: Thêm vào build_src_filter:
```ini
build_src_filter = +<test_animation.cpp> +<LiveChatRenderer.cpp> -<main.cpp>
```

### Runtime Errors:

#### Wokwi: `LiveChat: Failed to create sprite`
**Nguyên nhân**: Wokwi RAM giới hạn, không đủ cho sprite 320x240

**Giải pháp**: 
- ✅ Bình thường, không ảnh hưởng
- ✅ Trên hardware thật sẽ OK (ESP32 có 320KB RAM)

#### `nvs_open failed: NOT_FOUND`
**Nguyên nhân**: 
- Wokwi: Không hỗ trợ NVS
- Hardware: Lần đầu boot, chưa có data

**Giải pháp**:
- ✅ Code có fallback, vẫn chạy OK
- ✅ Sau lần calibrate đầu tiên, lỗi biến mất

#### Touch không chính xác
**Nguyên nhân**: Chưa calibrate hoặc calibrate sai

**Giải pháp**:
1. Menu → Settings → Touch Calibration
2. Chạm chính xác 4 góc theo hướng dẫn
3. Test lại touch

#### Màn hình trắng/đen
**Nguyên nhân**: 
- Pinout sai
- Màn hình không khởi tạo
- Backlight OFF

**Giải pháp**:
1. Kiểm tra lại pinout theo `PINOUT.md`
2. Kiểm tra nguồn 3.3V
3. Check backlight pin (GPIO 12)
4. Thử reset ESP32

---

## 11. TÍCH HỢP ANIMATIONS VÀO LOGIC API

> **Quan trọng**: Infrastructure animations đã sẵn sàng, nhưng cần thêm logic để auto-trigger theo flow conversation.

### Hiện trạng:

#### ✅ Đã có sẵn:
- LiveChat renderer init trong [`main.cpp:43`](src/main.cpp:43)
- Animation update loop trong [`UIManager.cpp:120`](src/UIManager.cpp:120)
- Manual serial commands trong [`Config.cpp:380-398`](src/Config.cpp:380)
- API đầy đủ trong [`LiveChatRenderer.h`](include/LiveChatRenderer.h)

#### ❌ Chưa có (cần code):
- Auto state transitions khi user nói/bot trả lời
- Emotion mapping từ Gemini/API response
- Audio level visualization cho LISTENING state
- Engine selection persistence trong Settings

---

### Bước 1: Tích hợp State Transitions

Thêm callbacks vào nơi xử lý voice/API:

```cpp
// File: src/VoiceHandler.cpp (hoặc nơi xử lý voice)

#include "LiveChatRenderer.h"

// Khi bắt đầu nghe user
void onStartListening() {
    setLiveChatState(CHAT_LISTENING);
    Serial.println("[Animation] → LISTENING");
}

// Khi gửi request lên API (Gemini/ChatGPT)
void onSendToAPI(const char* userText) {
    setLiveChatState(CHAT_THINKING);
    Serial.println("[Animation] → THINKING");
    
    // Send API request here...
}

// Khi nhận response và bắt đầu TTS (Text-to-Speech)
void onStartSpeaking(const char* responseText) {
    setLiveChatState(CHAT_SPEAKING);
    liveChatRenderer.setSpeakingText(responseText);
    Serial.println("[Animation] → SPEAKING");
    
    // Start TTS playback...
}

// Khi hoàn thành conversation
void onConversationComplete() {
    setLiveChatState(CHAT_IDLE);
    Serial.println("[Animation] → IDLE");
}

// Khi user ngắt lời bot
void onInterrupted() {
    setLiveChatState(CHAT_INTERRUPTED);
    liveChatRenderer.onInterrupt();
    Serial.println("[Animation] → INTERRUPTED");
}
```

#### Example Flow Integration:

```cpp
void handleUserVoiceInput() {
    // 1. Bắt đầu listening
    onStartListening();
    
    // 2. Record audio
    String userText = recordAndTranscribe();
    
    // 3. Send to API
    onSendToAPI(userText.c_str());
    String apiResponse = sendToGemini(userText);
    
    // 4. Speak response
    onStartSpeaking(apiResponse.c_str());
    playTTS(apiResponse);
    
    // 5. Complete
    onConversationComplete();
}
```

---

### Bước 2: Parse Emotion từ API Response

#### Option A: Gemini trả về metadata emotion

```cpp
// File: src/GeminiParser.cpp

#include <ArduinoJson.h>
#include "LiveChatRenderer.h"

void parseGeminiResponse(const String& jsonResponse) {
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, jsonResponse);
    
    // Parse response text
    const char* text = doc["candidates"][0]["content"]["parts"][0]["text"];
    
    // Parse emotion metadata (nếu Gemini trả về)
    if (doc.containsKey("emotion")) {
        const char* emotion = doc["emotion"];
        liveChatRenderer.setEmotion(emotion);
        Serial.printf("[Emotion] Set to: %s\n", emotion);
    }
    
    // Parse colors (nếu có)
    if (doc.containsKey("colors")) {
        uint16_t fg = doc["colors"]["foreground"];
        uint16_t bg = doc["colors"]["background"];
        uint16_t accent = doc["colors"]["accent"];
        liveChatRenderer.setColors(fg, bg, accent);
    }
}
```

#### Option B: Auto-detect emotion từ text response

```cpp
void autoDetectEmotion(const String& responseText) {
    String lower = responseText;
    lower.toLowerCase();
    
    // Happy indicators
    if (lower.indexOf("haha") >= 0 ||
        lower.indexOf("😂") >= 0 ||
        lower.indexOf("😊") >= 0 ||
        lower.indexOf("vui") >= 0) {
        liveChatRenderer.setEmotion("happy");
    }
    // Sad indicators
    else if (lower.indexOf("buồn") >= 0 ||
             lower.indexOf("😢") >= 0 ||
             lower.indexOf("tiếc") >= 0) {
        liveChatRenderer.setEmotion("sad");
    }
    // Excited indicators
    else if (lower.indexOf("wow") >= 0 ||
             lower.indexOf("amazing") >= 0 ||
             lower.indexOf("tuyệt") >= 0) {
        liveChatRenderer.setEmotion("excited");
    }
    // Angry indicators
    else if (lower.indexOf("tức") >= 0 ||
             lower.indexOf("giận") >= 0) {
        liveChatRenderer.setEmotion("angry");
    }
    // Default neutral
    else {
        liveChatRenderer.setEmotion("neutral");
    }
}
```

#### Sử dụng:

```cpp
void onAPIResponse(const String& response) {
    // Parse và detect emotion
    autoDetectEmotion(response);
    
    // Hoặc parse từ JSON
    parseGeminiResponse(response);
    
    // Sau đó speak
    onStartSpeaking(response.c_str());
}
```

---

### Bước 3: Audio Level Visualization

Nếu có microphone input để hiển thị waveform/spectrum:

```cpp
// File: src/AudioInput.cpp

#include "LiveChatRenderer.h"

// Đọc mic level (giả sử dùng ADC hoặc I2S)
float getMicrophoneLevel() {
    // Read from ADC/I2S
    int rawValue = analogRead(MIC_PIN);
    return (float)rawValue / 4095.0f;  // Normalize 0.0-1.0
}

// Nếu có FFT để analyze spectrum
void getAudioSpectrum(float* spectrum, uint8_t bands) {
    // Perform FFT on audio buffer
    // Fill spectrum array with band levels (0.0-1.0)
    // Example pseudo-code:
    for (uint8_t i = 0; i < bands; i++) {
        spectrum[i] = fft_band_magnitude(i) / MAX_MAGNITUDE;
    }
}

// Update trong loop
void updateAudioVisualization() {
    if (getLiveChatState() == CHAT_LISTENING) {
        // Simple level meter
        float level = getMicrophoneLevel();
        liveChatRenderer.setAudioLevel(level);
        
        // Optional: Full spectrum (16 bands)
        float spectrum[16];
        getAudioSpectrum(spectrum, 16);
        liveChatRenderer.setAudioSpectrum(spectrum, 16);
    }
}
```

#### Thêm vào main loop:

```cpp
void loop() {
    serviceConfig();
    serviceNvs();
    
    // Update audio visualization
    updateAudioVisualization();
    
    if (getUIMode() == MODE_FACE) serviceFaceRenderer();
    serviceUI();
    serviceDebugOverlay();
}
```

---

### Bước 4: Speaking Progress Tracking

Hiển thị progress khi bot đang nói:

```cpp
// File: src/TTSHandler.cpp

#include "LiveChatRenderer.h"

class TTSHandler {
private:
    String currentText;
    uint32_t startTime;
    uint32_t duration;
    
public:
    void speak(const String& text) {
        currentText = text;
        startTime = millis();
        
        // Estimate duration (giả sử 150 words/min)
        int wordCount = countWords(text);
        duration = (wordCount * 60000) / 150;  // ms
        
        // Start speaking
        onStartSpeaking(text.c_str());
        playAudio(text);
    }
    
    void updateProgress() {
        if (getLiveChatState() == CHAT_SPEAKING) {
            uint32_t elapsed = millis() - startTime;
            float progress = (float)elapsed / (float)duration;
            progress = constrain(progress, 0.0f, 1.0f);
            
            liveChatRenderer.setSpeakingProgress(progress);
        }
    }
};

// Update trong loop
TTSHandler ttsHandler;

void loop() {
    // ... existing code
    ttsHandler.updateProgress();
    serviceUI();
}
```

---

### Bước 5: Engine Selection trong Settings UI

Thêm menu chọn animation engine:

```cpp
// File: src/MenuCallbacks.cpp

#include "LiveChatRenderer.h"
#include <Preferences.h>

// Global preferences
extern Preferences prefs;

// Callback khi user chọn engine
void onEngineSelected(int engineIndex) {
    AnimEngine engine;
    const char* engineName;
    
    switch(engineIndex) {
        case 0:
            engine = ENGINE_GROBOT;
            engineName = "Grobot";
            break;
        case 1:
            engine = ENGINE_KAWAII;
            engineName = "Kawaii";
            break;
        case 2:
            engine = ENGINE_BMO;
            engineName = "BMO";
            break;
        case 3:
            engine = ENGINE_MOCHI;
            engineName = "Mochi";
            break;
        case 4:
            engine = ENGINE_EYES;
            engineName = "Eyes";
            break;
        default:
            return;
    }
    
    // Set engine
    setEmotionEngine(engine);
    Serial.printf("Engine changed to: %s\n", engineName);
    
    // Save to NVS
    prefs.begin("livechat", false);
    prefs.putUChar("engine", (uint8_t)engine);
    prefs.end();
}

// Load engine từ NVS khi boot
void loadEnginePreference() {
    prefs.begin("livechat", true);
    uint8_t engine = prefs.getUChar("engine", ENGINE_GROBOT);
    prefs.end();
    
    setEmotionEngine((AnimEngine)engine);
    Serial.printf("Loaded engine: %d\n", engine);
}

// Gọi trong setup()
void setup() {
    // ... existing setup
    loadEnginePreference();
}
```

---

### Bước 6: Complete Integration Example

File mới: `src/ConversationManager.cpp`

```cpp
#include "ConversationManager.h"
#include "LiveChatRenderer.h"
#include "VoiceInput.h"
#include "GeminiAPI.h"
#include "TTSOutput.h"

class ConversationManager {
private:
    enum State {
        IDLE,
        LISTENING,
        PROCESSING,
        SPEAKING
    } state;
    
    VoiceInput voiceIn;
    GeminiAPI gemini;
    TTSOutput tts;
    
public:
    void begin() {
        state = IDLE;
        setLiveChatState(CHAT_IDLE);
    }
    
    void startConversation() {
        if (state != IDLE) return;
        
        // Transition to listening
        state = LISTENING;
        setLiveChatState(CHAT_LISTENING);
        voiceIn.startRecording();
    }
    
    void update() {
        switch(state) {
            case LISTENING:
                updateListening();
                break;
                
            case PROCESSING:
                updateProcessing();
                break;
                
            case SPEAKING:
                updateSpeaking();
                break;
                
            default:
                break;
        }
    }
    
private:
    void updateListening() {
        // Update audio visualization
        float micLevel = voiceIn.getLevel();
        liveChatRenderer.setAudioLevel(micLevel);
        
        // Check if done recording
        if (voiceIn.isRecordingComplete()) {
            String userText = voiceIn.getTranscription();
            Serial.printf("User: %s\n", userText.c_str());
            
            // Transition to processing
            state = PROCESSING;
            setLiveChatState(CHAT_THINKING);
            gemini.sendRequest(userText);
        }
    }
    
    void updateProcessing() {
        // Check if API response ready
        if (gemini.isResponseReady()) {
            String response = gemini.getResponse();
            Serial.printf("Bot: %s\n", response.c_str());
            
            // Parse emotion
            String emotion = gemini.getEmotion();
            if (emotion.length() > 0) {
                liveChatRenderer.setEmotion(emotion.c_str());
            }
            
            // Transition to speaking
            state = SPEAKING;
            setLiveChatState(CHAT_SPEAKING);
            liveChatRenderer.setSpeakingText(response.c_str());
            tts.speak(response);
        }
    }
    
    void updateSpeaking() {
        // Update speaking progress
        float progress = tts.getProgress();
        liveChatRenderer.setSpeakingProgress(progress);
        
        // Check if done speaking
        if (tts.isDone()) {
            // Back to idle
            state = IDLE;
            setLiveChatState(CHAT_IDLE);
        }
    }
};

// Global instance
ConversationManager conversation;

// Sử dụng trong main.cpp
void setup() {
    // ... existing setup
    conversation.begin();
}

void loop() {
    // ... existing services
    conversation.update();
    serviceUI();
}
```

---

### Tóm tắt - Checklist Tích hợp:

#### Infrastructure (✅ Đã có):
- [x] LiveChat renderer initialized
- [x] Animation update loop
- [x] API functions (setState, setEngine, setEmotion)
- [x] 5 animation engines implemented

#### Logic cần thêm (❌ TODO):
- [ ] State transition callbacks trong voice handler
- [ ] Emotion parser từ API response
- [ ] Audio level reading và visualization
- [ ] Speaking progress tracking
- [ ] Engine selection UI + NVS persistence
- [ ] Conversation manager orchestration

#### Files cần tạo/sửa:
1. `src/VoiceHandler.cpp` - Voice input + callbacks
2. `src/GeminiParser.cpp` - Parse API response
3. `src/AudioInput.cpp` - Mic level/spectrum
4. `src/TTSHandler.cpp` - TTS progress tracking
5. `src/MenuCallbacks.cpp` - Engine selection UI
6. `src/ConversationManager.cpp` - Orchestrate toàn bộ flow

---

## 📚 TÀI LIỆU THAM KHẢO

### Documentation:
- [`PINOUT.md`](PINOUT.md) - Pin connections chi tiết
- [`SETUP_GUIDE.md`](SETUP_GUIDE.md) - Setup từng bước
- [`ANIMATION_TEST_GUIDE.md`](ANIMATION_TEST_GUIDE.md) - Test animations
- [`QUICK_TEST_COMMANDS.md`](QUICK_TEST_COMMANDS.md) - Cheat sheet commands
- [`TEST_ON_HARDWARE.md`](TEST_ON_HARDWARE.md) - Test trên hardware
- [`ARCHITECTURE.md`](ARCHITECTURE.md) - Kiến trúc code
- [`EMOTIONS_IMPLEMENTATION.md`](EMOTIONS_IMPLEMENTATION.md) - Emotion system

### Source Code:
- [`src/main.cpp`](src/main.cpp) - Main app entry point
- [`src/test_animation.cpp`](src/test_animation.cpp) - Test animation app
- [`src/LiveChatRenderer.cpp`](src/LiveChatRenderer.cpp) - Animation engine
- [`src/UIManager.cpp`](src/UIManager.cpp) - UI/Menu system
- [`src/Config.cpp`](src/Config.cpp) - Config & commands

### Headers:
- [`include/LiveChatRenderer.h`](include/LiveChatRenderer.h) - Animation API
- [`include/UIManager.h`](include/UIManager.h) - UI API
- [`include/Config.h`](include/Config.h) - Config API

---

## 🎯 QUICK START

### Để test nhanh trên Wokwi:

```bash
# 1. Build
pio run

# 2. Start Wokwi (trong VSCode)
# Click chuột phải diagram.json → Start Wokwi Simulator

# 3. Xem Serial output và gõ lệnh test:
kawaii
idle
listen
think
speak
```

### Để chạy trên Hardware:

```bash
# 1. Đấu nối theo PINOUT.md

# 2. Upload
pio run -t upload

# 3. Monitor
pio device monitor

# 4. Touch calibration (lần đầu)
# Follow instructions on screen

# 5. Use UI
# Touch menu icons to navigate
```

---

## ❓ HỖ TRỢ

- **Issues**: Tạo issue trên GitHub repo
- **Documentation**: Đọc files .md trong project
- **Serial Debug**: Monitor serial output để debug

**Build Info:**
- Platform: ESP32 (espressif32)
- Framework: Arduino
- RAM: 320KB
- Flash: 4MB
- Display: ST7789 240x320 SPI
- Touch: XPT2046 Resistive

---

**© 2026 UHA Chatbot Project**
