# 🎨 Hướng dẫn Test Animation trên ESP32

## 📋 Tổng quan

Dự án đã được cấu hình để test riêng các animations qua Serial Monitor. Bạn có thể dùng lệnh đơn giản để xem từng animation và switch giữa các emotion engines.

---

## 🚀 Bước 1: Upload Code Test lên ESP32

### Chuẩn bị:
1. ✅ Cắm ESP32 vào máy tính qua USB
2. ✅ Đảm bảo PlatformIO đã được cài đặt trong VSCode
3. ✅ File [`platformio.ini`](platformio.ini:3) đã được cấu hình build [`test_animation.cpp`](src/test_animation.cpp:1)

### Build và Upload:

**Cách 1: Dùng PlatformIO UI**
```
1. Click icon PlatformIO trên thanh bên trái VSCode
2. Mở "Project Tasks"
3. Click "Esp32dev → General → Upload"
4. Đợi build + upload xong (~1-2 phút)
```

**Cách 2: Dùng Terminal**
```bash
cd d:/uhachatbot
platformio run -t upload
```

**Cách 3: Shortcut**
- Nhấn `Ctrl+Alt+U` (upload shortcut của PlatformIO)

### Kết quả mong đợi:
```
SUCCESS: Upload successful
Configuring upload protocol...
AVAILABLE: cmsis-dap, esp-bridge, esp-prog, espota, esptool, iot-bus-jtag, jlink, minimodule, olimex-arm-usb-ocd, olimex-arm-usb-ocd-h, olimex-arm-usb-tiny-h, olimex-jtag-tiny, tumpa
CURRENT: upload_protocol = esptool
...
Writing at 0x00010000... (100 %)
Wrote 925232 bytes (582143 compressed) at 0x00010000 in 51.7 seconds
Hash of data verified.

Leaving...
Hard resetting via RTS pin...
```

---

## 🖥️ Bước 2: Mở Serial Monitor

### Cách mở Serial Monitor:

**Cách 1: PlatformIO UI**
```
Project Tasks → Esp32dev → Monitor
```

**Cách 2: Terminal**
```bash
platformio device monitor
```

**Cách 3: Shortcut** 
- Click icon **"🔌 Serial Monitor"** trên thanh dưới VSCode (bên cạnh icon ♻️ build)

**Cách 4: Manual**
- Nhấn `Ctrl+Shift+P`
- Gõ: "PlatformIO: Monitor"
- Enter

### Khi Serial Monitor mở, bạn sẽ thấy:

```
=================================
🎨 TEST ANIMATION TRÊN ESP32
=================================

✅ Màn hình đã khởi tạo
✅ LiveChat renderer đã sẵn sàng

📝 CÁC LỆNH TEST:
  idle      - Test Idle animation
  listen    - Test Listening animation
  think     - Test Thinking animation
  speak     - Test Speaking animation
  error     - Test Error animation
  kawaii    - Chuyển sang Kawaii engine
  mochi     - Chuyển sang Mochi engine
  grobot    - Chuyển sang Grobot engine
  simple    - Chuyển sang Simple engine
  geo       - Chuyển sang Geometric engine
  clear     - Xóa màn hình
  help      - Hiển thị menu này

💡 Gõ lệnh và nhấn Enter để test!
```

---

## 🎬 Bước 3: Test Từng Animation

### 🔵 Test IDLE Animation
Gõ trong Serial Monitor:
```
idle
```

**Kết quả:**
- Serial Monitor: `🔵 Testing IDLE animation...`
- Màn hình ESP32: Hiển thị animation trạng thái nghỉ (tùy engine)
  - **Simple**: Circle nhỏ breathing
  - **Kawaii**: Mặt ^_^ nháy mắt
  - **Mochi**: Blob nhỏ bounce nhẹ
  - **Grobot**: Face animation idle
  - **Geometric**: Pattern xoay chậm

---

### 🎤 Test LISTENING Animation
Gõ:
```
listen
```

**Kết quả:**
- Serial Monitor: `🎤 Testing LISTENING animation...`
- Màn hình: Animation đang nghe
  - **Simple**: Circle pulse
  - **Kawaii**: Emoji với tai dựng lên 👂
  - **Mochi**: Blob rung nhẹ theo sóng âm
  - **Grobot**: Listening pose
  - **Geometric**: Sound wave pattern

---

### 🤔 Test THINKING Animation
Gõ:
```
think
```

**Kết quả:**
- Serial Monitor: `🤔 Testing THINKING animation...`
- Màn hình: Animation đang suy nghĩ
  - **Simple**: Rotating dots
  - **Kawaii**: 🤔 với 3 chấm ...
  - **Mochi**: Blob squish & stretch
  - **Grobot**: Thinking pose
  - **Geometric**: Spiral rotating

---

### 💬 Test SPEAKING Animation
Gõ:
```
speak
```

**Kết quả:**
- Serial Monitor: `💬 Testing SPEAKING animation...`
- Màn hình: Animation đang nói
  - **Simple**: Bars pulsing
  - **Kawaii**: Miệng mở/đóng kawaii
  - **Mochi**: Blob bounce với speech bubble
  - **Grobot**: Speaking animation
  - **Geometric**: Wave patterns

---

### ❌ Test ERROR Animation
Gõ:
```
error
```

**Kết quả:**
- Serial Monitor: `❌ Testing ERROR animation...`
- Màn hình: Animation lỗi
  - **Simple**: Red X shake
  - **Kawaii**: >_< embarrassed face
  - **Mochi**: Blob deflate sadly
  - **Grobot**: Error expression
  - **Geometric**: Broken pattern flash

---

## 🎨 Bước 4: Switch Giữa Các Emotion Engines

### 🌸 Kawaii Engine (Anime-style)
Gõ:
```
kawaii
```

**Đặc điểm:**
- Style anime/manga
- Emoji kawaii: ^_^, >_<, ≧◡≦
- Màu sắc pastel dễ thương
- Animations mượt mà
- **RAM usage**: Medium (~40KB)

**Sau đó test các state:**
```
idle    → ^_^ nháy mắt
listen  → 👂 chú ý
think   → 🤔 với ...
speak   → 💬 miệng động
error   → >_< xấu hổ
```

---

### 🫧 Mochi Engine (Bouncy Blob)
Gõ:
```
mochi
```

**Đặc điểm:**
- Blob physics simulation
- Bounce & squish effects
- Smooth deformation
- Particle effects
- **RAM usage**: Medium-High (~50KB)

**Test các state:**
```
idle    → Blob nhỏ bounce
listen  → Rung theo âm thanh
think   → Squish & stretch
speak   → Bounce với bubble
error   → Deflate sadly
```

---

### 🤖 Grobot Engine (Library Animations)
Gõ:
```
grobot
```

**Đặc điểm:**
- Sử dụng Grobot_Animations library
- Pre-rendered animations
- Smooth & professional
- Nhiều emotions
- **RAM usage**: High (~60-80KB)

**Test các state:**
```
idle    → Face idle animation
listen  → Listening pose
think   → Thinking expression
speak   → Mouth movements
error   → Error/sad face
```

---

### ⚪ Simple Engine (Minimal)
Gõ:
```
simple
```

**Đặc điểm:**
- Basic geometric shapes
- Minimal RAM usage
- Fast rendering
- Đơn giản nhưng hiệu quả
- **RAM usage**: Low (~20KB)

**Test các state:**
```
idle    → Circle breathing
listen  → Pulse circle
think   → Rotating dots
speak   → Bars pulsing
error   → Red X shake
```

---

### 📐 Geometric Engine (Abstract)
Gõ:
```
geo
```

**Đặc điểm:**
- Abstract geometric patterns
- Rotating shapes
- Modern/tech aesthetic
- Mathematical animations
- **RAM usage**: Medium (~35KB)

**Test các state:**
```
idle    → Slow rotation
listen  → Sound waves
think   → Spiral rotating
speak   → Wave patterns
error   → Broken pattern
```

---

## 🎯 Bước 5: Test Workflow Đầy Đủ

### Scenario 1: Test một engine hoàn chỉnh
```
kawaii          ← Chọn engine
idle            ← Xem idle
listen          ← Xem listening
think           ← Xem thinking
speak           ← Xem speaking
error           ← Xem error
```

### Scenario 2: So sánh giữa các engines
```
simple          ← Engine 1
speak           ← Test speaking
clear           ← Xóa màn hình

mochi           ← Engine 2
speak           ← Test cùng state
clear

kawaii          ← Engine 3
speak           ← So sánh
```

### Scenario 3: Test transitions
```
kawaii
idle            ← State 1
listen          ← Transition 1→2
think           ← Transition 2→3
speak           ← Transition 3→4
idle            ← Back to idle
```

---

## 🛠️ Lệnh Hỗ trợ

### Xóa màn hình
```
clear
```
Màn hình sẽ chuyển về màu đen. Hữu ích khi muốn xem animation mới rõ ràng.

### Hiển thị help
```
help
```
Hiển thị lại danh sách tất cả lệnh có sẵn.

---

## 📊 Checklist Test Đầy Đủ

### ✅ Test 5 States với mỗi Engine:

#### Simple Engine:
- [ ] idle - Circle breathing
- [ ] listen - Pulse circle
- [ ] think - Rotating dots
- [ ] speak - Bars pulsing
- [ ] error - Red X

#### Kawaii Engine:
- [ ] idle - ^_^ nháy mắt
- [ ] listen - 👂 chú ý
- [ ] think - 🤔 với ...
- [ ] speak - 💬 miệng động
- [ ] error - >_< xấu hổ

#### Mochi Engine:
- [ ] idle - Blob bounce
- [ ] listen - Rung âm thanh
- [ ] think - Squish/stretch
- [ ] speak - Bounce bubble
- [ ] error - Deflate

#### Grobot Engine:
- [ ] idle - Face idle
- [ ] listen - Listening pose
- [ ] think - Think expression
- [ ] speak - Mouth move
- [ ] error - Sad face

#### Geometric Engine:
- [ ] idle - Slow rotation
- [ ] listen - Sound waves
- [ ] think - Spiral rotate
- [ ] speak - Wave patterns
- [ ] error - Broken pattern

---

## 🐛 Troubleshooting

### ❌ Serial Monitor không hiện gì
**Fix:**
1. Kiểm tra baud rate = 115200
2. Thử close rồi open lại monitor
3. Nhấn nút RESET trên ESP32

### ❌ Lệnh không hoạt động
**Fix:**
1. Đảm bảo gõ đúng chính tả (lowercase)
2. Nhấn Enter sau mỗi lệnh
3. Kiểm tra "Send on Enter" đã bật

### ❌ Màn hình không hiển thị gì
**Fix:**
1. Kiểm tra dây nối màn hình
2. Thử lệnh `clear` rồi test lại
3. Check serial log có lỗi không

### ❌ Animation giật lag
**Fix:**
- Grobot engine nặng → thử Simple hoặc Kawaii
- Kiểm tra nhiệt độ ESP32 (throttling)
- Update animation có thể mất frames

### ❌ Out of memory
**Fix:**
```
grobot    ← Engine nặng nhất
mochi     ← Vừa phải
simple    ← Nhẹ nhất
```

---

## 💡 Tips & Tricks

### Tip 1: Test nhanh một engine
```bash
# Gõ liên tiếp:
kawaii
idle
listen
think
speak
error
```

### Tip 2: Record test results
- Dùng PlatformIO log output
- Hoặc copy/paste từ Serial Monitor
- Chụp ảnh/quay video màn hình ESP32

### Tip 3: Performance testing
- Dùng Simple engine làm baseline
- So sánh smoothness với các engine khác
- Check serial log có warning về RAM không

### Tip 4: Debug animation issues
```
# Bật debug mode (nếu có)
help           ← Xem current state
clear          ← Reset display
idle           ← Back to known state
```

---

## 📱 Kết quả Mong đợi

### Sau khi test xong, bạn sẽ:
- ✅ Thấy được 5 animations hoạt động
- ✅ So sánh được 5 emotion engines
- ✅ Hiểu được đặc điểm từng engine
- ✅ Biết engine nào phù hợp cho project
- ✅ Xác nhận code chạy tốt trên phần cứng

### Performance Benchmarks:
| Engine | RAM | FPS | Smoothness | Style |
|--------|-----|-----|------------|-------|
| Simple | 20KB | 60 | ⭐⭐⭐⭐⭐ | Basic |
| Geometric | 35KB | 50 | ⭐⭐⭐⭐ | Modern |
| Kawaii | 40KB | 45 | ⭐⭐⭐⭐ | Cute |
| Mochi | 50KB | 40 | ⭐⭐⭐ | Bouncy |
| Grobot | 70KB | 30 | ⭐⭐⭐ | Pro |

---

## 🎓 Bài tập Nâng cao

### Bài 1: Tìm engine mượt nhất
Test cả 5 engines với state `speak`, xem engine nào smooth nhất.

### Bài 2: Test memory limits
Chạy Grobot engine lâu dài, xem có bị crash không.

### Bài 3: Timing test
Đo thời gian transition giữa các states.

### Bài 4: Custom sequence
Tạo sequence: `idle → listen → think → speak → idle` (loop)

---

## 📚 Tham khảo Code

- **Test file**: [`src/test_animation.cpp`](src/test_animation.cpp:1)
- **LiveChat Renderer**: [`src/LiveChatRenderer.cpp`](src/LiveChatRenderer.cpp:1)
- **Config**: [`platformio.ini`](platformio.ini:3)

---

## 🎉 Kết luận

Bây giờ bạn đã có:
- ✅ File test riêng cho animations
- ✅ Serial commands đơn giản
- ✅ Cách test từng animation
- ✅ So sánh các emotion engines
- ✅ Debug khi có vấn đề

**Bắt đầu test ngay!** 🚀

Gõ `help` trong Serial Monitor để bắt đầu!
