# ⚡ Quick Test Commands - Cheat Sheet

## 🎬 CÁC LỆNH ANIMATION

### States (Trạng thái)
```
idle      - Trạng thái nghỉ
listen    - Đang nghe
think     - Đang suy nghĩ
speak     - Đang nói
error     - Lỗi
```

### Engines (Phong cách)
```
simple    - Đơn giản, nhẹ nhất (20KB RAM)
kawaii    - Anime cute (40KB RAM)
mochi     - Blob bouncy (50KB RAM)
grobot    - Professional (70KB RAM)
geo       - Geometric abstract (35KB RAM)
```

### Khác
```
clear     - Xóa màn hình
help      - Hiển thị menu
```

---

## 🚀 WORKFLOW TEST NHANH

### Test một engine đầy đủ (30 giây)
```
kawaii
idle
listen
think
speak
error
clear
```

### So sánh 3 engines (1 phút)
```
simple
speak
clear

kawaii
speak
clear

mochi
speak
```

### Test transitions (45 giây)
```
kawaii
idle
listen
think
speak
idle
```

---

## 📊 BẢNG SO SÁNH NHANH

| Engine | RAM | Speed | Style | Khi nào dùng |
|--------|-----|-------|-------|--------------|
| **simple** | 20KB | ⚡⚡⚡⚡⚡ | Tối giản | RAM ít, cần nhanh |
| **geo** | 35KB | ⚡⚡⚡⚡ | Hiện đại | Tech/abstract look |
| **kawaii** | 40KB | ⚡⚡⚡⚡ | Dễ thương | Friendly UI |
| **mochi** | 50KB | ⚡⚡⚡ | Vui nhộn | Interactive/fun |
| **grobot** | 70KB | ⚡⚡⚡ | Chuyên nghiệp | Best quality |

---

## 💡 GỢI Ý

**Đề xuất cho production:**
- Dùng `kawaii` hoặc `simple` - cân bằng tốt
- `mochi` nếu cần vui nhộn
- `grobot` nếu RAM đủ và cần đẹp nhất

**Debug issues:**
```
clear          # Reset màn hình
simple         # Về engine nhẹ nhất
idle           # Về trạng thái an toàn
help           # Xem lại commands
```

---

## 🎯 TEST COMMANDS MẪU

Copy-paste vào Serial Monitor:

### Test 1: Kawaii emotions
```
kawaii
idle
listen
think
speak
error
```

### Test 2: All engines (speaking)
```
simple
speak
mochi
speak
kawaii
speak
grobot
speak
geo
speak
```

### Test 3: State cycle
```
kawaii
idle
listen
think
speak
idle
```

---

**Chi tiết đầy đủ:** Xem [`ANIMATION_TEST_GUIDE.md`](ANIMATION_TEST_GUIDE.md)
