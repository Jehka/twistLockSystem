#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <EEPROM.h>

// === LCD ===
LiquidCrystal_I2C lcd(0x27, 16, 2);

// === Keypad ===
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// === Servo & Buzzer & LEDs ===
Servo lockServo;
const int servoPin = 11;
const int buzzerPin = 10;
const int statusLED = 13;
const int failLED = 12;
const int adminLED = A0;
const int redLED = A3;
const int greenLED = A4;

// === EEPROM Constants ===
const int EEPROM_ADDR = 0;
String correctCode = "";

// === Variables ===
String enteredCode = "";
int failCount = 0;
bool lockedOut = false;
bool adminMode = false;
int adminChallenge = 0;
const int ADMIN_KEY = 4011;

// === Function Prototypes ===
void resetDisplay(String message = "Enter Code");
void loadCodeFromEEPROM();
void saveCodeToEEPROM(String newCode);
void promptForNewCode();

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(buzzerPin, OUTPUT);
  pinMode(statusLED, OUTPUT);
  pinMode(failLED, OUTPUT);
  pinMode(adminLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  digitalWrite(statusLED, LOW);
  digitalWrite(failLED, LOW);
  digitalWrite(adminLED, LOW);
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);

  lockServo.attach(servoPin);
  lockServo.write(0);

  randomSeed(analogRead(A5));
  loadCodeFromEEPROM();
  resetDisplay();
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    Serial.print("Keypad: "); Serial.println(key);
    handleInput(key);
  }
}

void handleInput(char key) {
  if (lockedOut) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LOCKED OUT!");
    return;
  }

  if (key == '#') {
    if (adminMode) {
      checkAdminCode();
    } else {
      checkUserCode();
    }
  } else if (key == '*') {
    enteredCode = "";
    resetDisplay("Cleared");
  } else if (key == 'A') {
    enterAdminMode();
  } else {
    enteredCode += key;
    lcd.setCursor(0, 1);
    lcd.print("Code: " + enteredCode);
  }
}

void enterAdminMode() {
  adminMode = true;
  enteredCode = "";
  adminChallenge = random(1000, 9999);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ADMIN CHALLENGE:");
  lcd.setCursor(0, 1);
  lcd.print(adminChallenge);
  digitalWrite(adminLED, HIGH);
  Serial.println("Admin challenge: " + String(adminChallenge));
}

void checkAdminCode() {
  int userResponse = enteredCode.toInt();
  int expected = adminChallenge ^ ADMIN_KEY;

  if (userResponse == expected) {
    resetDisplay("Admin Access");
    Serial.println("Admin unlocked");
    unlockSequence(true);
  } else {
    resetDisplay("Wrong Admin Code");
    failCount++;
    checkFailCount();
  }

  adminMode = false;
  enteredCode = "";
  digitalWrite(adminLED, LOW);
}

void checkUserCode() {
  if (enteredCode == correctCode) {
    resetDisplay("Access Granted!");
    Serial.println("User unlocked");
    unlockSequence(false);
  } else {
    resetDisplay("Wrong Code!");
    failCount++;
    checkFailCount();
  }
  enteredCode = "";
}

void checkFailCount() {
  if (failCount >= 3) {
    lockedOut = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LOCKED OUT!");
    tone(buzzerPin, 1000);
    digitalWrite(failLED, HIGH);
    Serial.println("System locked out!");
  } else {
    digitalWrite(failLED, HIGH);
    tone(buzzerPin, 1500, 300);
    delay(300);
    digitalWrite(failLED, LOW);
    noTone(buzzerPin);
  }
}

void unlockSequence(bool isAdmin) {
  digitalWrite(statusLED, HIGH);
  digitalWrite(greenLED, HIGH);
  lockServo.write(90);
  tone(buzzerPin, 2000, 300);
  delay(5000);
  lockServo.write(0);
  digitalWrite(statusLED, LOW);
  digitalWrite(greenLED, LOW);
  failCount = 0;
  lockedOut = false;
  enteredCode = "";
  digitalWrite(failLED, LOW);
  noTone(buzzerPin);

  if (isAdmin) {
    promptForNewCode();
  }
}

void promptForNewCode() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set New Code:");
  String newCode = "";

  while (newCode.length() < 4) {
    char key = keypad.getKey();
    if (key && isDigit(key)) {
      newCode += key;
      lcd.setCursor(newCode.length() - 1, 1);
      lcd.print("*");
    }
  }

  saveCodeToEEPROM(newCode);
  correctCode = newCode;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("New Code Saved!");
  delay(1000);
}

void resetDisplay(String message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  lcd.setCursor(0, 1);
}

void loadCodeFromEEPROM() {
  char codeBuf[5];
  for (int i = 0; i < 4; i++) {
    codeBuf[i] = EEPROM.read(EEPROM_ADDR + i);
  }
  codeBuf[4] = '\0';
  correctCode = String(codeBuf);

  if (correctCode == "" || correctCode.length() != 4) {
    correctCode = "1234";
    saveCodeToEEPROM(correctCode);
  }

  Serial.println("Loaded code: " + correctCode);
}

void saveCodeToEEPROM(String newCode) {
  for (int i = 0; i < 4; i++) {
    EEPROM.write(EEPROM_ADDR + i, newCode[i]);
  }
}
