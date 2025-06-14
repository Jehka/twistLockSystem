# 🔐 Arduino Admin Challenge-Response Lock System

A secure lock system with challenge-response authentication using XOR encryption for admin access. Users unlock with a 4-digit passcode, while admins are prompted with a pseudo-random challenge and must input the XOR-encrypted response.

> 🧠 XOR Challenge-Response ensures the real password is never typed, making this system smarter than your average doorknob.

![Initial Setup](assets/initial.jpg)
![Access Granted](assets/grant.jpg)
![Admin Challenge](assets/admin_challenge.jpg)
![Admin Access](assets/admin_access.jpg)
![Admin New Code](assets/admin_new_code.jpg)
![Code Saved](assets/code_saved.jpg)
![Final State](assets/final.jpg)

## 📽️ Demo Video

[▶️ Watch the demonstration](assets/demo_video.mp4)

---

## 📁 Folder Structure

```
arduino-lock-system/
├── assets/
│   ├── initial.jpg
│   ├── grant.jpg
│   ├── admin_challenge.jpg
│   ├── admin_access.jpg
│   ├── admin_new_code.jpg
│   ├── code_saved.jpg
│   ├── final.jpg
│   └── demo_video.mp4
├── code/
│   └── arduino_lock.ino
├── README.md
```

---

## ⚙️ Features

- 4x4 Keypad input for user and admin
- 16x2 I2C LCD display for feedback
- Servo motor for physical locking mechanism
- EEPROM-stored 4-digit access code
- Admin-only access to change the code
- Buzzer alerts for wrong entries or lockouts
- LED indicators for access, admin mode, and lockout
- Lockout mechanism after 3 wrong attempts

---

## 🔐 Admin Authentication Flow

Admins press **A** to receive a **random 4-digit challenge**. The correct response is:

```
response = challenge ^ 4011
```

> 💡 You can use any [online XOR calculator](https://xor.pw/) to compute the correct response. Just input the challenge and key `4011` to get the answer.

Once verified, admins can change the access code (currently hardcoded to `4321`—feel free to modify to accept custom admin entries).

---

## 🔌 Hardware Wiring Summary

### 🧾 Keypad Pin Mapping

| Keypad Pin | Arduino Pin |
|------------|-------------|
| R1         | D9          |
| R2         | D8          |
| R3         | D7          |
| R4         | D6          |
| C1         | D5          |
| C2         | D4          |
| C3         | D3          |
| C4         | D2          |

### 🖥️ LCD (I2C) Connections

| LCD Pin | Arduino Pin |
|---------|-------------|
| SDA     | A4          |
| SCL     | A5          |

### ⚙️ Servo Motor Wiring

| Servo Wire | Arduino Pin |
|------------|-------------|
| Signal     | D11         |
| VCC        | 5V (external supply recommended if servo draws high current) |
| GND        | GND         |

### 🔊 Buzzer Connection

| Buzzer Pin | Arduino Pin |
|------------|-------------|
| + (Signal) | D10         |
| - (GND)    | GND         |

---

### 🧩 Component Connection Overview

| **Component**         | **Connected to Arduino Pin** | **Notes**                              |
|-----------------------|------------------------------|----------------------------------------|
| **Keypad Rows (4x)**  | D9, D8, D7, D6               | Row pins (digital)                     |
| **Keypad Columns (4x)**| D5, D4, D3, D2              | Column pins (digital)                  |
| **LCD I2C (16x2)**    | A4 (SDA), A5 (SCL)           | Standard I2C on Uno                    |
| **Servo Lock**        | D11                          | PWM capable pin                        |
| **Buzzer**            | D10                          | Used for alerts and lockout tones      |
| **Green LED (Access)**| A4                           | Lights up for successful access        |
| **Red LED (Wrong Code)**| A3                         | Lights up on wrong code attempt        |
| **Admin Mode LED**    | A0                           | Indicates active admin challenge       |
| **Fail LED**          | D12                          | Signals lockout or failed attempts     |
| **Status LED**        | D13                          | Optional indicator (lock engaged)      |

---

## 🔧 Dependencies

- Arduino UNO (or compatible board)
- 4x4 Matrix Keypad
- 16x2 LCD with I2C backpack
- Servo Motor (e.g., SG90)
- Passive Buzzer
- LEDs (Green, Red, etc.)
- EEPROM (internal to UNO)

Libraries used:

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <EEPROM.h>
```

---

## 📥 Uploading the Code

1. Place `arduino_lock.ino` in the `code/` directory of your project folder.
2. Connect the components as per the wiring table.
3. Open the `.ino` file in Arduino IDE.
4. Select your board and COM port.
5. Upload and test.

---

## 🏁 Future Improvements

- Dynamic code entry instead of hardcoded `4321`
- Web-based code change using Wi-Fi module (ESP variant)
- Tamper detection and alert
- Encrypted EEPROM storage for code

