# 🎯 Hướng dẫn Test Animation trên ESP32 - Phiên bản Cuối cùng

## ✅ Đã Hoàn thành

Các lệnh test animation đã được **tích hợp sẵn** vào chương trình chính! Bạn không cần file test riêng nữa.

---

## 🚀 Cách Test trên Phần cứng ESP32

### Bước 1: Build và Upload

**Option 1: Dùng PlatformIO UI**
```
1. Click icon PlatformIO (thanh bên trái VSCode)
2. Project Tasks → Esp32dev → Upload
3. Đợi build + upload xong (~1-2 phút)
```

**Option 2: Terminal**
```bash
cd d:/uhachatbot
platformio run -t upload
```

**Option 3: Shortcut VSCode**
- Nhấn `Ctrl+Alt+U`

---

### Bước 2: Mở Serial Monitor

**Cách mở:**
```bash
platformio device monitor
```

Hoặc click icon **"🔌 Serial Monitor"** trên thanh dưới VSCode

---

### Bước 3: Vào LiveChat Mode

Sau khi ESP32 boot xong, gõ lệnh:
```
livechat
```

Màn hình sẽ chuyển sang LiveChat mode và sẵn sàng test animations.

---

## 🎬 CÁC LỆNH TEST ANIMATION

### 📝 Test Trạng thái (States)

Gõ trực tiếp trong Serial Monitor:

```
idle          # Test animation IDLE (nghỉ)
listen        # Test animation LISTENING (đang nghe)
think         # Test animation THINKING (đang suy nghĩ)
speak         # Test animation SPEAKING (đang nói)
error         # Test animation ERROR (lỗi)
```

**Ví dụ:**
```
> livechat
OK: entered LIVECHAT mode

> idle
OK: animation IDLE

> listen
OK: animation LISTENING

> speak
OK: animation SPEAKING
```

---

### 🎨 Chuyển Đổi Emotion Engine

```
engine_simple     # SIMPLE engine (20KB RAM, nhanh nhất)
engine_kawaii     # KAWAII engine (40KB RAM, anime cute)
engine_mochi      # MOCHI engine (50KB RAM, bouncy blob)
engine_grobot     # GROBOT engine (70KB RAM, professional)
engine_geo        # GEOMETRIC engine (35KB RAM, abstract)
```

**Ví dụ:**
```
> engine_kawaii
OK: engine KAWAII (anime cute, 40KB RAM)

> speak
OK: animation SPEAKING
(Bây giờ bạn sẽ thấy speaking animation theo style Kawaii)

> engine_mochi
OK: engine MOCHI (bouncy blob, 50KB RAM)

> speak
OK: animation SPEAKING
(Bây giờ speaking animation theo style Mochi blob)
```

---

## 🧪 WORKFLOW TEST HOÀN CHỈNH

### Test 1: Một Engine với Tất cả States (30 giây)

```
livechat
engine_kawaii
idle
listen
think
speak
error
idle
```

### Test 2: So sánh Các Engines (1 phút)

```
livechat
engine_simple
speak

engine_kawaii
speak

engine_mochi
speak

engine_grobot
speak

engine_geo
speak
```

### Test 3: Test State Transitions (45 giây)

```
livechat
engine_kawaii
idle
listen
think
speak
idle
listen
speak
idle
```

---

## 📊 BẢNG SO SÁNH ENGINES

| Engine | RAM | FPS | Smoothness | Style | Khi nào dùng |
|--------|-----|-----|------------|-------|--------------|
| **simple** | 20KB | ⚡⚡⚡⚡⚡ | ⭐⭐⭐⭐⭐ | Tối giản | RAM ít, cần tốc độ |
| **geo** | 35KB | ⚡⚡⚡⚡ | ⭐⭐⭐⭐ | Hiện đại | Tech/abstract UI |
| **kawaii** | 40KB | ⚡⚡⚡⚡ | ⭐⭐⭐⭐ | Dễ thương | Friendly/cute UI |
| **mochi** | 50KB | ⚡⚡⚡ | ⭐⭐⭐ | Vui nhộn | Interactive/fun |
| **grobot** | 70KB | ⚡⚡⭐ | ⭐⭐⭐ | Chuyên nghiệp | Chất lượng cao nhất |

---

## 🔍 XEM TẤT CẢ LỆNH CÓ SẴN

Gõ trong Serial Monitor:
```
help
```

Bạn sẽ thấy tất cả lệnh bao gồm:

**Animation Commands:**
- `livechat` - Enter LiveChat mode
- `idle` - Test IDLE animation
- `listen` - Test LISTENING animation
- `think` - Test THINKING animation
- `speak` - Test SPEAKING animation
- `error` - Test ERROR animation
- `engine_simple` - Switch to SIMPLE engine
- `engine_kawaii` - Switch to KAWAII engine
- `engine_mochi` - Switch to MOCHI engine
- `engine_grobot` - Switch to GROBOT engine
- `engine_geo` - Switch to GEOMETRIC engine

**Plus các lệnh khác:**
- `mode:FACE` - Quay về Face mode
- `clear` - Xóa màn hình
- `status` - Xem trạng thái hệ thống
- `mem` - Xem RAM còn lại
- ... và nhiều lệnh khác

---

## 💡 MẸO TEST

### Mẹo 1: Test nhanh một engine
```bash
livechat
engine_kawaii
idle
listen
think
speak
```

### Mẹo 2: So sánh 2 engines
```bash
livechat
engine_simple
speak
# Quan sát animation

engine_kawaii
speak
# So sánh khác biệt
```

### Mẹo 3: Quay về Face mode
```bash
mode:FACE
```

### Mẹo 4: Xem RAM usage
```bash
mem
```
Kiểm tra xem engine nào dùng bao nhiêu RAM

---

## 🐛 TROUBLESHOOTING

### ❌ Lệnh không hoạt động
**Nguyên nhân:** Chưa vào LiveChat mode  
**Fix:**
```
livechat
```
Sau đó thử lại lệnh animation

### ❌ "ERR: unknown command"
**Fix:**
```
help
```
Xem danh sách lệnh chính xác

### ❌ Animation không mượt
**Fix:** Thử engine nhẹ hơn
```
engine_simple
```

### ❌ Out of memory
**Fix:**
```
mem               # Xem RAM còn lại
engine_simple     # Dùng engine nhẹ nhất
```

### ❌ Màn hình đen
**Fix:**
```
clear
livechat
idle
```

---

## 📋 CHECKLIST TEST ĐẦY ĐỦ

### Test Cơ bản (5 phút)
- [ ] Upload code thành công
- [ ] Mở Serial Monitor
- [ ] Gõ `help` - thấy danh sách lệnh
- [ ] Gõ `livechat` - vào LiveChat mode
- [ ] Test 5 states: idle, listen, think, speak, error
- [ ] Test ít nhất 2 engines khác nhau

### Test Nâng cao (10 phút)
- [ ] Test cả 5 engines
- [ ] Test transitions giữa các states
- [ ] So sánh smoothness của các engines
- [ ] Check RAM usage với `mem`
- [ ] Test quay về Face mode với `mode:FACE`

### Test Performance (15 phút)
- [ ] Test mỗi engine chạy lâu dài (2-3 phút)
- [ ] Check có bị lag không
- [ ] Đo FPS bằng mắt
- [ ] Test chuyển engine nhiều lần
- [ ] Check memory leak với `mem`

---

## 🎓 VÍ DỤ TEST SESSION HOÀN CHỈNH

```
# Khởi động
> help
(Xem danh sách lệnh)

> status
(Xem trạng thái hệ thống)

> mem
memory:         125K
(RAM khả dụng)

# Vào LiveChat mode
> livechat
OK: entered LIVECHAT mode

# Test Kawaii engine
> engine_kawaii
OK: engine KAWAII (anime cute, 40KB RAM)

> idle
OK: animation IDLE
(Quan sát animation trên màn hình)

> listen
OK: animation LISTENING
(Animation đổi sang listening)

> think
OK: animation THINKING

> speak
OK: animation SPEAKING

> error
OK: animation ERROR

# Đổi sang Mochi engine
> engine_mochi
OK: engine MOCHI (bouncy blob, 50KB RAM)

> speak
OK: animation SPEAKING
(Xem animation khác với Kawaii như thế nào)

# Check memory
> mem
memory:         95K
(Mochi dùng nhiều RAM hơn)

# Test Simple engine (nhẹ nhất)
> engine_simple
OK: engine SIMPLE (minimal, 20KB RAM)

> speak
OK: animation SPEAKING
(Nhanh và mượt nhất)

> mem
memory:         135K
(Simple dùng ít RAM nhất)

# Quay về Face mode
> mode:FACE
OK
```

---

## 📚 TÀI LIỆU THAM KHẢO

- **Hướng dẫn chi tiết**: [`ANIMATION_TEST_GUIDE.md`](ANIMATION_TEST_GUIDE.md)
- **Lệnh nhanh**: [`QUICK_TEST_COMMANDS.md`](QUICK_TEST_COMMANDS.md)
- **Code source**: 
  - [`src/Config.cpp`](src/Config.cpp) - Lệnh animation
  - [`src/LiveChatRenderer.cpp`](src/LiveChatRenderer.cpp) - Animation logic
  - [`src/main.cpp`](src/main.cpp) - Main program

---

## 🎉 KẾT LUẬN

Bạn đã có:
- ✅ Lệnh animation tích hợp sẵn trong chương trình chính
- ✅ Test được 5 states và 5 engines
- ✅ Không cần file test riêng
- ✅ Dễ dàng switch giữa các engines
- ✅ Có thể test ngay trên phần cứng ESP32

**Bắt đầu test ngay:**
1. Upload code: `platformio run -t upload`
2. Mở Serial Monitor: `platformio device monitor`
3. Gõ: `livechat`
4. Gõ: `help` để xem tất cả lệnh
5. Bắt đầu test: `idle`, `listen`, `speak`, etc.

**Chúc bạn test vui vẻ!** 🚀
