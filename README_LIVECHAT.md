# 🤖 LiveChat UI - ESP32 Chatbot Giao Diện

Hệ thống giao diện live chat đẹp mắt cho ESP32 Chatbot với Gemini Voice API, tích hợp 5 animation engines khác nhau và hỗ trợ đầy đủ voice interaction flow.

## ✨ Tính năng nổi bật

### 🎨 5 Animation Engines
- **Grobot**: Animated eyes từ thư viện Grobot_Animations
- **Kawaii**: Phong cách cute Nhật Bản với blush marks  
- **BMO**: Retro 8-bit pixel art (Adventure Time inspired)
- **Mochi**: Cute bouncy blob với squash & stretch
- **Eyes**: Realistic eye tracking

### 🎭 Smart State Machine
- **IDLE**: Chờ đợi với face animation
- **LISTENING**: Voice input với waveform/spectrum visualization
- **THINKING**: Processing với pulse rings và animated dots
- **SPEAKING**: Voice output với mouth sync và progress bar
- **INTERRUPTED**: User có thể ngắt lời bot bất cứ lúc nào

### 👆 Touch Interactions
- Double-tap → Enter LiveChat mode
- Tap (idle) → Start listening
- Tap (speaking) → Interrupt bot
- Long-press → Exit LiveChat mode

### 🎨 Customization
- Switchable animation engines
- Color themes (Classic/Neon/Nature/Cool)
- Emotion control (happy/sad/angry/wink)
- Real-time audio visualization

## 📁 Cấu trúc project

```
uhachatbot/
├── include/
│   ├── LiveChatRenderer.h          ⭐ NEW - LiveChat UI header
│   ├── UIManager.h                 📝 Modified - Thêm MODE_LIVECHAT
│   └── ...
├── src/
│   ├── LiveChatRenderer.cpp        ⭐ NEW - Implementation (550 lines)
│   ├── UIManager.cpp               📝 Modified - LiveChat integration
│   ├── main.cpp                    📝 Modified - Init LiveChat
│   └── ...
├── examples/
│   └── LiveChatDemo.cpp            ⭐ NEW - Demo code & examples
├── LIVECHAT_DEMO.md                ⭐ NEW - Hướng dẫn chi tiết
├── IMPLEMENTATION_SUMMARY.md       ⭐ NEW - Tổng kết implementation
├── ARCHITECTURE.md                 ⭐ NEW - Kiến trúc hệ thống
└── README_LIVECHAT.md              ⭐ NEW - File này
```

## 🚀 Quick Start

### 1. Clone project
```bash
git clone https://github.com/Pt284/uhachatbot
cd uhachatbot
```

### 2. Build & Upload
```bash
# Sử dụng PlatformIO
pio run -t upload

# Monitor serial output
pio device monitor
```

### 3. Test LiveChat UI
```
Serial Monitor (115200 baud):

> livechat      # Enter LiveChat mode
> listen        # Test listening animation
> think         # Test thinking animation
> speak         # Test speaking animation
> engine mochi  # Switch to Mochi engine
> help          # Show all commands
```

### 4. Touch Interactions
- **Double-tap** màn hình từ FACE mode → Vào LiveChat
- **Tap** một lần → Bắt đầu listening (nếu đang idle)
- **Tap** khi bot đang nói → Ngắt lời
- **Long-press** → Thoát về FACE mode

## 💻 Code Usage

### Basic Setup
```cpp
#include "LiveChatRenderer.h"

// Trong setup()
extern TFT_eSPI tft;
initLiveChat(&tft);

// Trong loop()
serviceLiveChat();  // Tự động update nếu ở MODE_LIVECHAT
```

### Control States
```cpp
// Start listening
liveChatRenderer.setState(CHAT_LISTENING);

// Show audio level
liveChatRenderer.setAudioLevel(0.8f);  // 0.0 - 1.0

// Processing
liveChatRenderer.setState(CHAT_THINKING);

// Speaking
liveChatRenderer.setState(CHAT_SPEAKING);
liveChatRenderer.setSpeakingProgress(0.5f);  // 50%

// Back to idle
liveChatRenderer.setState(CHAT_IDLE);
```

### Switch Engines
```cpp
liveChatRenderer.setEngine(ENGINE_KAWAII);  // Cute style
liveChatRenderer.setEngine(ENGINE_BMO);     // Retro style
liveChatRenderer.setEngine(ENGINE_MOCHI);   // Bouncy blob
```

### Customize Colors
```cpp
liveChatRenderer.setColors(
    TFT_CYAN,      // Foreground
    TFT_BLACK,     // Background
    TFT_MAGENTA    // Accent
);
```

## 🎯 Integration với Gemini Voice API

### Full Flow Example
```cpp
void handleVoiceChat() {
    // 1. User tap → Start listening
    liveChatRenderer.setState(CHAT_LISTENING);
    
    // 2. Record audio
    while (recording) {
        float level = micGetLevel();
        liveChatRenderer.setAudioLevel(level);
        serviceLiveChat();
    }
    
    // 3. Send to Gemini
    liveChatRenderer.setState(CHAT_THINKING);
    geminiAPI.sendAudio(audioBuffer, bufferSize);
    
    // 4. Wait for response
    while (geminiAPI.isProcessing()) {
        serviceLiveChat();  // Animate thinking
    }
    
    // 5. Play response
    liveChatRenderer.setState(CHAT_SPEAKING);
    AudioStream* stream = geminiAPI.getResponse();
    
    while (stream->available()) {
        speakerPlay(stream->read());
        float progress = stream->position() / stream->length();
        liveChatRenderer.setSpeakingProgress(progress);
        serviceLiveChat();
    }
    
    // 6. Done
    liveChatRenderer.setState(CHAT_IDLE);
}
```

## 📚 Documentation

- **[LIVECHAT_DEMO.md](LIVECHAT_DEMO.md)** - Hướng dẫn sử dụng chi tiết, API reference
- **[IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md)** - Tổng kết implementation, code statistics
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - Kiến trúc hệ thống, data flow diagrams
- **[examples/LiveChatDemo.cpp](examples/LiveChatDemo.cpp)** - Demo code đầy đủ với examples

## 🎬 Demo States

### IDLE State
```
┌─────────────────┐
│                 │
│    (◕‿◕)       │  ← Cute face waiting
│                 │
│   Tap to talk   │
└─────────────────┘
```

### LISTENING State
```
┌─────────────────┐
│                 │
│    (◕ω◕)       │  ← Listening face
│                 │
│  ▁▂▃▅▇▅▃▂▁    │  ← Waveform
│  Listening...   │
└─────────────────┘
```

### THINKING State
```
┌─────────────────┐
│                 │
│    (◕.◕)       │  ← Thinking face
│      ◌         │  ← Pulse ring
│  Thinking...   │
└─────────────────┘
```

### SPEAKING State
```
┌─────────────────┐
│                 │
│    (◕▽◕)       │  ← Happy speaking
│                 │
│  ████████░░░   │  ← Progress: 80%
│  Speaking...    │
└─────────────────┘
```

## 🔧 Technical Specs

### Hardware Requirements
- **MCU**: ESP32 (4MB RAM recommended)
- **Display**: ST7789 240x320 TFT
- **Touch**: XPT2046 Resistive
- **Audio In**: I2S Microphone
- **Audio Out**: I2S Speaker/DAC

### Memory Usage
- Sprite buffer: ~150KB
- Grobot eyes: ~50KB
- Total: ~200KB RAM

### Performance
- Target: 30 FPS
- Frame time: ~20-40ms (tùy engine)

## 🎨 Animation Engines Comparison

| Engine | Style | Complexity | Performance | Best For |
|--------|-------|------------|-------------|----------|
| Grobot | Library eyes | Medium | Good | General purpose |
| Kawaii | Cute Japanese | Low | Excellent | Friendly assistant |
| BMO | 8-bit retro | Low | Excellent | Retro/gaming theme |
| Mochi | Bouncy blob | Low | Excellent | Playful personality |
| Eyes | Realistic | High | Good | Professional/serious |

## 🌟 Use Cases

### 1. Voice Assistant
Live chat giống Gemini trên điện thoại - nói chuyện tự nhiên, ngắt lời được

### 2. Smart Speaker
Giao diện đẹp cho smart speaker với voice feedback rõ ràng

### 3. Educational Robot
Biểu cảm phong phú giúp học sinh tương tác tốt hơn

### 4. Interactive Toy
Cute animations và bouncy effects cho đồ chơi thông minh

## 🔮 Future Improvements

- [ ] Particle effects cho thinking state
- [ ] Phoneme-based lip sync
- [ ] More animation engines (Matrix, Sketch, etc.)
- [ ] Gesture animations (wave, nod, shake)
- [ ] Settings UI trong menu
- [ ] Cloud sync preferences
- [ ] AI-generated custom faces

## 🙏 Credits & Inspiration

Cảm ơn các projects open source đã cung cấp inspiration:
- [tanmaywankar/Grobot_Animations](https://github.com/tanmaywankar/Grobot_Animations)
- [0015/lvgl_kawaii_face](https://github.com/0015/lvgl_kawaii_face)
- [AlleyBo55/BMO-ESP32](https://github.com/AlleyBo55/BMO-ESP32)
- [dzonder/mochi-esp32](https://github.com/dzonder/mochi-esp32)
- [playfultechnology/esp32-eyes](https://github.com/playfultechnology/esp32-eyes)
- [anthropoleo/esp32_mochi_clone](https://github.com/anthropoleo/esp32_mochi_clone)
- [twtomato/mimiclaw-lcd](https://github.com/twtomato/mimiclaw-lcd)

## 📝 License

Same as uhachatbot project

## 💬 Support

Nếu có vấn đề hoặc câu hỏi, vui lòng tạo issue trên GitHub repository.

---

**Enjoy your beautiful LiveChat UI! 🎉**
