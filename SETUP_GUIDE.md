# 🚀 Hướng dẫn Setup để Test Giao diện

Để test được giao diện LiveChat UI trên ESP32, bạn cần:

## 📋 Yêu cầu

### 1. Phần cứng (Hardware)
- ✅ **ESP32** (đã có trong project gốc)
- ✅ **ST7789 Display 240x320** (đã có)
- ✅ **Touch screen XPT2046** (đã có)
- ⚡ **Cáp USB** để nạp code

### 2. Phần mềm (Software)
- [ ] **VSCode** (bạn đang dùng rồi)
- [ ] **PlatformIO Extension** (cần cài)

## 🔧 Setup từng bước

### Bước 1: Cài PlatformIO Extension

#### Trong VSCode:
1. Nhấn `Ctrl+Shift+X` (mở Extensions)
2. Tìm kiếm: **"PlatformIO IDE"**
3. Click **Install**
4. Đợi cài đặt xong (có thể mất 5-10 phút)
5. Reload VSCode khi được yêu cầu

Hoặc cài qua link: https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide

### Bước 2: Mở Project

1. Trong VSCode, mở folder project:
   ```
   File → Open Folder → d:/uhachatbot
   ```

2. PlatformIO sẽ tự detect project (nhìn file `platformio.ini`)

3. Chờ PlatformIO download libraries:
   - TFT_eSPI
   - Grobot_Animations
   - XPT2046_Touchscreen
   - GUIslice

### Bước 3: Build Project

#### Cách 1: Dùng PlatformIO Toolbar
1. Nhìn thanh bên trái VSCode
2. Click icon **PlatformIO** (hình con kiến/alien)
3. Mở **Project Tasks**
4. Click **Esp32dev → General → Build**
5. Đợi compile xong (lần đầu lâu ~5 phút)

#### Cách 2: Dùng Command Palette
1. Nhấn `Ctrl+Shift+P`
2. Gõ: **"PlatformIO: Build"**
3. Enter

#### Cách 3: Terminal
```bash
cd d:/uhachatbot
platformio run
```

### Bước 4: Upload lên ESP32

1. **Cắm ESP32 vào máy tính qua USB**

2. Kiểm tra COM port:
   - Windows: Device Manager → Ports → ghi nhớ COM số mấy (vd: COM3)

3. Upload code:

   **Cách 1: PlatformIO UI**
   - Project Tasks → Esp32dev → General → **Upload**

   **Cách 2: Terminal**
   ```bash
   platformio run -t upload
   ```

4. Đợi upload xong (~30 giây)

### Bước 5: Test Giao diện

#### Mở Serial Monitor:

**Cách 1: PlatformIO**
- Project Tasks → Esp32dev → **Monitor**

**Cách 2: Terminal**
```bash
platformio device monitor
```

**Cách 3: Shortcut**
- Click icon **"Serial Monitor"** trên thanh dưới VSCode

#### Test Commands:
```
> livechat        # Vào LiveChat mode
> listen          # Xem listening animation
> think           # Xem thinking animation  
> speak           # Xem speaking animation
> engine kawaii   # Đổi sang Kawaii style
> engine mochi    # Đổi sang Mochi bouncy blob
> help            # Xem tất cả commands
```

#### Test Touch:
- **Double-tap** màn hình → Vào LiveChat
- **Tap** (idle) → Start listening
- **Tap** (speaking) → Interrupt
- **Long-press** → Exit LiveChat

## 📱 Kết quả mong đợi

### Sau khi upload thành công:

**Serial Monitor sẽ hiện:**
```
RSC ready (with LiveChat)
```

**Màn hình ESP32:**
- Hiển thị face animation từ Grobot library
- Có thể double-tap để vào LiveChat mode
- Animations chạy mượt mà

## 🐛 Troubleshooting

### ❌ "PlatformIO not found"
**Fix:** Cài lại PlatformIO Extension, reload VSCode

### ❌ "Upload failed"
**Fix:** 
- Kiểm tra ESP32 đã cắm USB chưa
- Thử nhấn nút BOOT trên ESP32 khi upload
- Đổi USB cable (có cable chỉ charge không data)

### ❌ "Library not found"
**Fix:**
```bash
cd d:/uhachatbot
platformio lib install
```

### ❌ "Compilation error"
**Có thể do:**
1. Libraries chưa download xong → Đợi thêm
2. Path có dấu tiếng Việt → Di chuyển project ra path không dấu
3. PlatformIO version cũ → Update: `platformio upgrade`

### ❌ "Out of memory"
**Fix:** Code dùng ~200KB RAM, ESP32 cần 4MB
- Kiểm tra board đúng: `board = esp32dev`
- Nếu vẫn lỗi, comment out engine nặng (Grobot) test trước

## 💡 Tips

### Faster Build:
```bash
# Build chỉ code thay đổi
platformio run --target upload
```

### Clean Build:
```bash
# Build lại từ đầu nếu lỗi lạ
platformio run --target clean
platformio run --target upload
```

### Monitor với filter:
```bash
# Chỉ xem output quan trọng
platformio device monitor --filter send_on_enter
```

## 🎯 Quick Setup Checklist

- [ ] Cài PlatformIO Extension trong VSCode
- [ ] Mở project folder `d:/uhachatbot`
- [ ] Đợi PlatformIO download libraries (lần đầu)
- [ ] Cắm ESP32 vào USB
- [ ] Build project: `platformio run`
- [ ] Upload: `platformio run -t upload`
- [ ] Monitor: `platformio device monitor`
- [ ] Test commands qua serial
- [ ] Test touch interactions

## 📚 Resources

- **PlatformIO Docs:** https://docs.platformio.org/
- **Quick Start:** https://docs.platformio.org/en/latest/integration/ide/vscode.html
- **Troubleshooting:** https://docs.platformio.org/en/latest/faq.html

## ⏱️ Thời gian ước tính

| Task | Time |
|------|------|
| Cài PlatformIO | 5-10 phút |
| Download libraries (lần đầu) | 3-5 phút |
| First build | 3-5 phút |
| Upload | 30 giây |
| **Tổng lần đầu** | **~15-20 phút** |
| Lần sau (rebuild) | < 1 phút |

## 🎉 Sau khi setup xong

Bạn sẽ có thể:
- ✅ Test tất cả animations
- ✅ Switch giữa 5 engines
- ✅ Xem state transitions
- ✅ Test touch interactions
- ✅ Debug qua serial monitor
- ✅ Modify code và test nhanh

---

**Bắt đầu từ Bước 1: Cài PlatformIO Extension! 🚀**
