# LiveChat UI - Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                        ESP32 CHATBOT SYSTEM                      │
└─────────────────────────────────────────────────────────────────┘

┌──────────────────── USER INTERACTION ────────────────────────┐
│                                                                │
│  Double-tap     Tap         Tap          Long-press          │
│  (on Face)   (on Idle)   (Speaking)    (LiveChat)            │
│      │           │            │              │                │
│      ▼           ▼            ▼              ▼                │
│   Enter      Start        Interrupt       Exit               │
│  LiveChat   Listening       Bot         LiveChat             │
│                                                                │
└────────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────────── UI MANAGER ──────────────────────────────┐
│                                                                │
│  Modes:  [MODE_FACE]  [MODE_MENU]  [MODE_LIVECHAT]          │
│                                           │                    │
│  Touch Event Handling ◄──────────────────┘                    │
│  Mode Switching Logic                                         │
│  Service Orchestration                                        │
│                                                                │
└────────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────── LIVECHAT RENDERER ───────────────────────────┐
│                                                                │
│  ┌─────────────── STATE MACHINE ────────────────┐            │
│  │                                                │            │
│  │    ┌───────┐   tap    ┌──────────┐          │            │
│  │    │ IDLE  │─────────►│LISTENING │          │            │
│  │    └───────┘          └──────────┘          │            │
│  │       ▲                     │                 │            │
│  │       │                     │ audio end       │            │
│  │       │                     ▼                 │            │
│  │       │                ┌──────────┐          │            │
│  │       │                │THINKING  │          │            │
│  │       │                └──────────┘          │            │
│  │       │                     │                 │            │
│  │       │                     │ response        │            │
│  │       │                     ▼                 │            │
│  │       │   complete    ┌──────────┐          │            │
│  │       └───────────────│SPEAKING  │          │            │
│  │                       └──────────┘          │            │
│  │                             │ tap            │            │
│  │                             ▼                 │            │
│  │                       ┌────────────┐        │            │
│  │                       │INTERRUPTED │        │            │
│  │                       └────────────┘        │            │
│  │                             │ 300ms          │            │
│  │                             └────────────────┘            │
│  │                                                            │
│  └────────────────────────────────────────────────────────────┘
│                                                                │
│  ┌──────────── ANIMATION ENGINES ─────────────┐              │
│  │                                              │              │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐   │              │
│  │  │ GROBOT  │  │ KAWAII  │  │   BMO   │   │              │
│  │  │ Library │  │  Cute   │  │ Retro   │   │              │
│  │  │  Eyes   │  │Japanese │  │ 8-bit   │   │              │
│  │  └─────────┘  └─────────┘  └─────────┘   │              │
│  │                                              │              │
│  │  ┌─────────┐  ┌─────────┐                 │              │
│  │  │  MOCHI  │  │  EYES   │                 │              │
│  │  │ Bouncy  │  │Realistic│                 │              │
│  │  │  Blob   │  │Tracking │                 │              │
│  │  └─────────┘  └─────────┘                 │              │
│  │                                              │              │
│  │  Switch at runtime with setEngine()        │              │
│  └──────────────────────────────────────────────┘              │
│                                                                │
│  ┌──────────── VISUAL EFFECTS ─────────────┐                 │
│  │                                           │                 │
│  │  State: LISTENING                        │                 │
│  │  ├─ Waveform (audio level)              │                 │
│  │  ├─ Spectrum (16 bands)                 │                 │
│  │  └─ Pulse ring                           │                 │
│  │                                           │                 │
│  │  State: THINKING                         │                 │
│  │  ├─ Animated dots "..."                 │                 │
│  │  ├─ Pulse ring (faster)                 │                 │
│  │  └─ Emotion: wink/neutral               │                 │
│  │                                           │                 │
│  │  State: SPEAKING                         │                 │
│  │  ├─ Progress bar                         │                 │
│  │  ├─ Mouth sync (8Hz)                    │                 │
│  │  └─ Emotion: happy                       │                 │
│  │                                           │                 │
│  │  State: INTERRUPTED                      │                 │
│  │  ├─ Red X symbol                         │                 │
│  │  ├─ "STOPPED" text                       │                 │
│  │  └─ Flash effect                         │                 │
│  │                                           │                 │
│  └───────────────────────────────────────────┘                 │
│                                                                │
│  ┌──────────── CUSTOMIZATION ──────────────┐                 │
│  │                                           │                 │
│  │  Colors:     FG / BG / Accent           │                 │
│  │  Emotions:   happy/sad/angry/wink       │                 │
│  │  Themes:     Classic/Neon/Nature/Cool   │                 │
│  │                                           │                 │
│  └───────────────────────────────────────────┘                 │
│                                                                │
└────────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────── DISPLAY & HARDWARE ──────────────────────────┐
│                                                                │
│  TFT Display (ST7789)    Touch Screen (XPT2046)              │
│  320x240 RGB565          Resistive touch                      │
│                                                                │
│  I2S Microphone          I2S Speaker/DAC                      │
│  Voice input             Voice output                         │
│                                                                │
└────────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌──────────────── GEMINI API INTEGRATION ──────────────────────┐
│                                                                │
│  1. Record audio from mic                                     │
│  2. Send to Gemini Voice API                                 │
│  3. Receive voice response                                    │
│  4. Play through speaker                                      │
│                                                                │
│  Visual feedback tại mỗi bước qua LiveChat UI                │
│                                                                │
└────────────────────────────────────────────────────────────────┘


═══════════════════════════════════════════════════════════════
                    DATA FLOW EXAMPLE
═══════════════════════════════════════════════════════════════

User: [Double-tap screen]
  │
  ├─► UIManager: Enter MODE_LIVECHAT
  │
  └─► LiveChatRenderer: setState(CHAT_IDLE)
          │
          │ Display: Cute face waiting
          │
User: [Tap to start]
  │
  ├─► LiveChatRenderer: setState(CHAT_LISTENING)
  │
  ├─► Microphone: Start recording
  │       │
  │       ├─► Calculate audio level
  │       │
  │       └─► LiveChatRenderer.setAudioLevel(0.75)
  │                │
  │                └─► Display: Waveform + Pulse ring
  │
User: [Finishes speaking]
  │
  ├─► LiveChatRenderer: setState(CHAT_THINKING)
  │       │
  │       └─► Display: Pulse + "..." dots
  │
  ├─► Gemini API: Send audio data
  │       │
  │       └─► Processing...
  │
Gemini: [Response received]
  │
  ├─► LiveChatRenderer: setState(CHAT_SPEAKING)
  │       │
  │       └─► Display: Mouth sync + Progress bar
  │
  ├─► Speaker: Play audio response
  │       │
  │       └─► Update progress: 0% → 100%
  │
System: [Response complete]
  │
  └─► LiveChatRenderer: setState(CHAT_IDLE)
          │
          └─► Display: Back to cute face

═══════════════════════════════════════════════════════════════

FILES CREATED:
  ├─ include/LiveChatRenderer.h       (120 lines)
  ├─ src/LiveChatRenderer.cpp         (550 lines)
  ├─ LIVECHAT_DEMO.md                 (250 lines)
  ├─ examples/LiveChatDemo.cpp        (280 lines)
  └─ IMPLEMENTATION_SUMMARY.md        (300 lines)

FILES MODIFIED:
  ├─ include/UIManager.h              (+1 enum)
  ├─ src/UIManager.cpp                (+80 lines)
  └─ src/main.cpp                     (+5 lines)

TOTAL: ~1,500 lines code + 600 lines documentation
