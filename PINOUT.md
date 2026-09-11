Resistive 240x320 LCD screen XPT2046, ST7789V

| Tín hiệu      | GPIO ESP32 | Vai trò                            |
| ------------- | ---------- | ---------------------------------- |
| SDI (MOSI)    | 26         | SPI MOSI dùng chung LCD+SD         |
| SCK           | 25         | SPI SCLK dùng chung LCD+SD         |
| SDO (MISO)    | 34         | SPI MISO dùng chung LCD+SD         |
| CS (LCD)      | 13         | Chip-select LCD                    |
| DC            | 27         | Data/Command LCD                   |
| RESET         | 14         | Reset LCD                          |
| LED (đèn nền) | 12         | Cấp nguồn + PWM sáng/tối           |
| T_IRQ         | 35         | Ngắt chạm (active LOW khi có chạm) |
| T_DO          | 32         | MISO riêng của touch (bit-bang)    |
| T_DIN         | 4          | MOSI riêng của touch (bit-bang)    |
| T_CLK         | 33         | SCLK riêng của touch (bit-bang)    |
| T_CS          | 23         | Chip-select touch                  |
| SD_CS         | 22         | Chip-select thẻ SD                 |
| GND           | GND        |                                    |
| VCC           | 3V3        |                                    |
