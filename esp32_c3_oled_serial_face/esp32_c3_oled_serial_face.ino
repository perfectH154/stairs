#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED hardware settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define OLED_ADDR 0x3C

// ESP32-C3 I2C pins. Keep these exactly as your wiring uses them.
#define OLED_SDA 5
#define OLED_SCL 6

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

enum FaceMode {
  FACE_NEUTRAL,
  FACE_HAPPY,
  FACE_SAD,
  FACE_THINK,
  FACE_ANGRY,
  FACE_SLEEP
};

FaceMode currentFace = FACE_NEUTRAL;
String serialLine = "";

// Animation timing. Every face updates quickly and never blocks for long.
unsigned long lastFrameTime = 0;
int animationFrame = 0;

const unsigned long FRAME_INTERVAL_MS = 250;

void setup() {
  Serial.begin(115200);
  delay(200);

  // Start I2C on the pins from your hardware description.
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED init failed");
    while (true) {
      delay(100);
    }
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  drawNeutral();
  Serial.println("OLED face ready");
  Serial.println("commands: happy sad think angry sleep neutral");
  Serial.println("face = neutral");
}

void loop() {
  readSerialCommand();
  updateAnimation();
}

void readSerialCommand() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      serialLine.trim();
      if (serialLine.length() > 0) {
        handleCommand(serialLine);
        serialLine = "";
      }
    } else {
      serialLine += c;

      // Protect RAM if the user pastes a very long line by mistake.
      if (serialLine.length() > 40) {
        serialLine = "";
        Serial.println("unknown command");
      }
    }
  }
}

void handleCommand(String command) {
  command.trim();
  command.toLowerCase();

  if (command == "happy") {
    setFace(FACE_HAPPY, "happy");
  } else if (command == "sad") {
    setFace(FACE_SAD, "sad");
  } else if (command == "think") {
    setFace(FACE_THINK, "think");
  } else if (command == "angry") {
    setFace(FACE_ANGRY, "angry");
  } else if (command == "sleep") {
    setFace(FACE_SLEEP, "sleep");
  } else if (command == "neutral") {
    setFace(FACE_NEUTRAL, "neutral");
  } else {
    Serial.println("unknown command");
  }
}

void setFace(FaceMode face, const char *name) {
  currentFace = face;
  animationFrame = 0;
  lastFrameTime = 0;

  Serial.print("face = ");
  Serial.println(name);

  redrawCurrentFace();
}

void updateAnimation() {
  unsigned long now = millis();

  if (now - lastFrameTime < FRAME_INTERVAL_MS) {
    return;
  }

  lastFrameTime = now;
  animationFrame++;
  redrawCurrentFace();
}

void redrawCurrentFace() {
  switch (currentFace) {
    case FACE_HAPPY:
      animateHappy();
      break;
    case FACE_SAD:
      animateSad();
      break;
    case FACE_THINK:
      animateThink();
      break;
    case FACE_ANGRY:
      drawAngry();
      break;
    case FACE_SLEEP:
      animateSleep();
      break;
    case FACE_NEUTRAL:
    default:
      drawNeutral();
      break;
  }
}

void clearFace() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void drawPixelFaceBox() {
  // A soft square head made from simple lines, good for a pixel-cute style.
  display.drawRect(22, 2, 84, 28, SSD1306_WHITE);
  display.drawPixel(22, 2, SSD1306_BLACK);
  display.drawPixel(105, 2, SSD1306_BLACK);
  display.drawPixel(22, 29, SSD1306_BLACK);
  display.drawPixel(105, 29, SSD1306_BLACK);
}

void drawCheeks() {
  display.drawPixel(40, 22, SSD1306_WHITE);
  display.drawPixel(41, 22, SSD1306_WHITE);
  display.drawPixel(42, 23, SSD1306_WHITE);

  display.drawPixel(86, 22, SSD1306_WHITE);
  display.drawPixel(87, 22, SSD1306_WHITE);
  display.drawPixel(85, 23, SSD1306_WHITE);
}

void drawOpenEyes() {
  display.fillCircle(49, 13, 3, SSD1306_WHITE);
  display.fillCircle(79, 13, 3, SSD1306_WHITE);
  display.drawPixel(50, 12, SSD1306_BLACK);
  display.drawPixel(80, 12, SSD1306_BLACK);
}

void drawNeutral() {
  clearFace();
  drawPixelFaceBox();
  drawOpenEyes();
  drawCheeks();
  display.drawLine(58, 22, 70, 22, SSD1306_WHITE);
  display.display();
}

void drawHappyFrame1() {
  clearFace();
  drawPixelFaceBox();

  display.drawLine(45, 13, 48, 10, SSD1306_WHITE);
  display.drawLine(48, 10, 52, 13, SSD1306_WHITE);
  display.drawLine(75, 13, 78, 10, SSD1306_WHITE);
  display.drawLine(78, 10, 82, 13, SSD1306_WHITE);

  display.drawLine(55, 20, 58, 23, SSD1306_WHITE);
  display.drawLine(58, 23, 70, 23, SSD1306_WHITE);
  display.drawLine(70, 23, 73, 20, SSD1306_WHITE);
  drawCheeks();
  display.display();
}

void drawHappyFrame2() {
  clearFace();
  drawPixelFaceBox();

  // Blink frame: both eyes become cute flat lines.
  display.drawLine(44, 13, 53, 13, SSD1306_WHITE);
  display.drawLine(74, 13, 83, 13, SSD1306_WHITE);

  display.drawLine(55, 20, 58, 24, SSD1306_WHITE);
  display.drawLine(58, 24, 70, 24, SSD1306_WHITE);
  display.drawLine(70, 24, 73, 20, SSD1306_WHITE);
  drawCheeks();
  display.display();
}

void drawHappyFrame3() {
  clearFace();
  drawPixelFaceBox();
  drawOpenEyes();

  display.drawLine(54, 20, 58, 24, SSD1306_WHITE);
  display.drawLine(58, 24, 70, 24, SSD1306_WHITE);
  display.drawLine(70, 24, 74, 20, SSD1306_WHITE);
  drawCheeks();
  display.display();
}

void animateHappy() {
  int frame = animationFrame % 4;

  if (frame == 2) {
    drawHappyFrame2();
  } else if (frame == 3) {
    drawHappyFrame3();
  } else {
    drawHappyFrame1();
  }
}

void drawSadFrame1() {
  clearFace();
  drawPixelFaceBox();

  display.drawLine(45, 11, 53, 14, SSD1306_WHITE);
  display.drawLine(75, 14, 83, 11, SSD1306_WHITE);
  display.fillCircle(49, 15, 2, SSD1306_WHITE);
  display.fillCircle(79, 15, 2, SSD1306_WHITE);

  display.drawLine(57, 24, 61, 21, SSD1306_WHITE);
  display.drawLine(61, 21, 67, 21, SSD1306_WHITE);
  display.drawLine(67, 21, 71, 24, SSD1306_WHITE);

  display.drawPixel(86, 17, SSD1306_WHITE);
  display.drawPixel(86, 18, SSD1306_WHITE);
  display.display();
}

void drawSadFrame2() {
  clearFace();
  drawPixelFaceBox();

  display.drawLine(45, 11, 53, 14, SSD1306_WHITE);
  display.drawLine(75, 14, 83, 11, SSD1306_WHITE);
  display.fillCircle(49, 15, 2, SSD1306_WHITE);
  display.fillCircle(79, 15, 2, SSD1306_WHITE);

  display.drawLine(57, 24, 61, 21, SSD1306_WHITE);
  display.drawLine(61, 21, 67, 21, SSD1306_WHITE);
  display.drawLine(67, 21, 71, 24, SSD1306_WHITE);

  // Tear falls down in this frame.
  display.drawPixel(86, 21, SSD1306_WHITE);
  display.drawPixel(86, 22, SSD1306_WHITE);
  display.drawPixel(87, 22, SSD1306_WHITE);
  display.display();
}

void animateSad() {
  if (animationFrame % 2 == 0) {
    drawSadFrame1();
  } else {
    drawSadFrame2();
  }
}

void drawThinkFrame1() {
  clearFace();
  drawPixelFaceBox();
  drawOpenEyes();

  display.drawLine(58, 23, 70, 21, SSD1306_WHITE);

  display.drawCircle(99, 8, 3, SSD1306_WHITE);
  display.drawCircle(108, 5, 5, SSD1306_WHITE);
  display.setCursor(106, 2);
  display.print("?");

  display.display();
}

void drawThinkFrame2() {
  clearFace();
  drawPixelFaceBox();
  drawOpenEyes();

  display.drawLine(58, 21, 70, 23, SSD1306_WHITE);

  display.drawCircle(98, 10, 2, SSD1306_WHITE);
  display.drawCircle(107, 7, 5, SSD1306_WHITE);
  display.setCursor(105, 4);
  display.print("?");

  display.display();
}

void animateThink() {
  if (animationFrame % 2 == 0) {
    drawThinkFrame1();
  } else {
    drawThinkFrame2();
  }
}

void drawAngry() {
  clearFace();
  drawPixelFaceBox();

  display.drawLine(43, 9, 54, 14, SSD1306_WHITE);
  display.drawLine(85, 9, 74, 14, SSD1306_WHITE);
  display.fillCircle(49, 15, 2, SSD1306_WHITE);
  display.fillCircle(79, 15, 2, SSD1306_WHITE);

  display.drawLine(57, 24, 71, 21, SSD1306_WHITE);

  // Tiny anger mark.
  display.drawLine(96, 8, 101, 8, SSD1306_WHITE);
  display.drawLine(98, 5, 98, 11, SSD1306_WHITE);
  display.drawLine(94, 5, 96, 7, SSD1306_WHITE);
  display.drawLine(102, 5, 100, 7, SSD1306_WHITE);

  display.display();
}

void drawSleepFrame1() {
  clearFace();
  drawPixelFaceBox();

  display.drawLine(44, 14, 54, 14, SSD1306_WHITE);
  display.drawLine(74, 14, 84, 14, SSD1306_WHITE);

  display.drawLine(58, 22, 62, 24, SSD1306_WHITE);
  display.drawLine(62, 24, 66, 22, SSD1306_WHITE);
  display.drawLine(66, 22, 70, 24, SSD1306_WHITE);

  display.setCursor(93, 5);
  display.print("Z");
  display.setCursor(104, 1);
  display.print("z");
  display.display();
}

void drawSleepFrame2() {
  clearFace();
  drawPixelFaceBox();

  display.drawLine(44, 14, 54, 14, SSD1306_WHITE);
  display.drawLine(74, 14, 84, 14, SSD1306_WHITE);

  display.drawLine(58, 23, 62, 25, SSD1306_WHITE);
  display.drawLine(62, 25, 66, 23, SSD1306_WHITE);
  display.drawLine(66, 23, 70, 25, SSD1306_WHITE);

  display.setCursor(91, 8);
  display.print("z");
  display.setCursor(101, 3);
  display.print("Z");
  display.setCursor(114, 0);
  display.print("z");
  display.display();
}

void animateSleep() {
  if (animationFrame % 2 == 0) {
    drawSleepFrame1();
  } else {
    drawSleepFrame2();
  }
}
