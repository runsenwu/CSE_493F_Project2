/**
 * Part of this code's inspiration and logic comes from the Pong game, the Flappy Bird game, and the
 * Physical Computing website by Professor Jon Froehlich
 * Pong game: https://github.com/makeabilitylab/arduino/blob/master/OLED/Pong/Pong.ino 
 * Flappy bird game: https://github.com/makeabilitylab/arduino/blob/master/OLED/FlappyBird/FlappyBird.ino
 * Physcial Computing online textbook: https://makeabilitylab.github.io/physcomp/
 *
 * This code requires Shape.hpp from the MakeabilityLab_Arduino_Library
 * 
 * To install and use these files:
 * 1. Get it from here: https://github.com/makeabilitylab/arduino/tree/master/MakeabilityLab_Arduino_Library
 * 2. Move the entire folder to the Arduino libraries folder on your computer.
 *   - On my Windows box, this is C:\Users\jonfr\Documents\Arduino\libraries
 *   - On Mac, it's: /Users/jonf/Documents/Arduino/libraries
 * 3. Then include the relevant libraries via #include <libraryname.h> or <libraryname.hpp>
 *
 * Adafruit Gfx Library:
 * https://learn.adafruit.com/adafruit-gfx-graphics-library/overview 
 *
 * Adafruit OLED tutorials:
 * https://learn.adafruit.com/monochrome-oled-breakouts
 * 
 * Code based on:
 * https://makeabilitylab.github.io/p5js/Games/FlappyBird
 * https://makeabilitylab.github.io/p5js/Games/FlappyBird2
 *  
 * By Runsen Wu
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */



#include <Wire.h>
#include <Shape.hpp>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET 4
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int FLAP_BUTTON_INPUT_PIN = 4;
const int TONE_OUTPUT_PIN = 8;
const int VIBROMOTOR_OUTPUT_PIN = 9;
const unsigned char _frog_sit [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x06, 0x60, 0x4d, 0xb2, 0xef, 0xf7, 0x4b, 0xd2, 
	0x7f, 0xfe, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x7f, 0xfe, 0xff, 0xff, 0x7f, 0xfe, 0x38, 0x1c
};
const unsigned char _frog_leap1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x04, 0x20, 0x08, 0x10, 0x4c, 0x32, 0xa8, 0x15, 0x78, 0x1e, 
	0x18, 0x18, 0x10, 0x08, 0x10, 0x08, 0x70, 0x0e, 0x48, 0x12, 0x47, 0xe2, 0x38, 0x1c, 0x00, 0x00
};
const unsigned char _frog_leap2 [] PROGMEM = {
	0x00, 0x00, 0x03, 0xc0, 0x04, 0x20, 0x08, 0x10, 0x0c, 0x30, 0x08, 0x10, 0x78, 0x1e, 0xb8, 0x1d, 
	0x50, 0x0a, 0x10, 0x08, 0x30, 0x0c, 0x48, 0x12, 0x47, 0xe2, 0x48, 0x12, 0x30, 0x0c, 0x00, 0x00
};
const unsigned char _frog_leap3 [] PROGMEM = {
	0x00, 0x00, 0x03, 0xc0, 0x06, 0x60, 0x0d, 0xb0, 0x0f, 0xf0, 0x0b, 0xd0, 0x7f, 0xfe, 0xff, 0xff, 
	0x5f, 0xfa, 0x1f, 0xf8, 0x3f, 0xfc, 0x7f, 0xfe, 0x7f, 0xfe, 0x78, 0x1e, 0x30, 0x0c, 0x00, 0x00
};
const unsigned char _frog_left [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x08, 0x40, 0x50, 0x22, 0xb8, 0x65, 0x50, 0x22, 
	0x70, 0x3e, 0x10, 0x20, 0x10, 0x10, 0x10, 0x10, 0x70, 0x0e, 0x88, 0x11, 0x47, 0xe2, 0x38, 0x1c
};
const unsigned char _frog_right [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x02, 0x10, 0x44, 0x0a, 0xa6, 0x1d, 0x44, 0x0a, 
	0x7c, 0x0e, 0x04, 0x08, 0x08, 0x08, 0x08, 0x08, 0x70, 0x0e, 0x88, 0x11, 0x47, 0xe2, 0x38, 0x1c
};

const char STR_PRESS_JUMP_TO_PLAY[] = "Press Jump to Play!";
const char STR_LOADSCREEN_APP_NAME_LINE1[] = "Jiggly";
const char STR_LOADSCREEN_APP_NAME_LINE2[] = "Froggy";
const char STR_MAKER[] = "Runsen Wu";
const char STR_GAME_OVER[] = "Game Over!";

class Car : public Rectangle {
  public:
    Car(int x, int y, int width, int height) : Rectangle(x, y, width, height)
    {
    }

    void set_random_speed() {
      speed = random(1, 6);
    }

    int getSpeed() {
      return speed;
    }

    int setSpeed(int new_speed) {
      speed = new_speed;
    }

    int speed;
};


class Frog {
  public:
    Frog(int x, int y) {
      _x = x;
      _y = y;
      previousMillis = 0;
      counter = 0;
    }

    void setX(int x) {
      _x = x;
    }

    void setY(int y) {
      _y = y;
    }

    int getX() {
      return _x;
    }
    
    int getY() {
      return _y;
    }

    int getRight() {
      return _x + 8;
    } 

    int getBottom() {
      return _y + 8;
    }

    int getLeft() {
      return _x - 8;
    }

    int getTop() {
      return _y - 8;
    }

    void draw_leap(Adafruit_SSD1306 input_display) {
      input_display.drawBitmap(_x - 8, _y - 8, _frog_leap3, 16, 16, WHITE);
      input_display.display();
    }

    void draw_sit(Adafruit_SSD1306 input_display) {
      input_display.drawBitmap(_x - 8, _y - 8, _frog_sit, 16, 16, WHITE);
      input_display.display();
    }

    void draw_idle(Adafruit_SSD1306 input_display, unsigned long currentMillis) {
      if (currentMillis - previousMillis > 500) {
        previousMillis = currentMillis;
        counter += 1;
      } 

      if (counter % 4 == 0 || counter % 4 == 2) {
        input_display.drawBitmap(_x - 8, _y - 8, _frog_sit, 16, 16, WHITE);
      } else if (counter % 4 == 1) {
        input_display.drawBitmap(_x - 8, _y - 8, _frog_left, 16, 16, WHITE);
      } else {
        input_display.drawBitmap(_x - 8, _y - 8, _frog_right, 16, 16, WHITE);
      }
      input_display.display();
    }

    int _x;
    int _y;
    int _right;
    int _left;
    int _top;
    int _bottom;
    int previousMillis;
    int counter;
};


enum GameState {
  NEW_GAME,
  PLAYING,
  GAME_OVER,
};

Car _cars[5] =  {
  Car(0, -14, 14, 14),
  Car(128, 2, 14, 14),
  Car(0, 18, 14, 14),
  Car(128, 50, 14, 14),
  Car(0, 34, 14, 14),
};

Frog _frog(64, 56);

int _x_speed;
int _y_speed;
int _points;
unsigned long previousMillis = 0UL;
const int DELAY_LOOP_MS = 5;
const int LOAD_SCREEN_SHOW_MS = 2000;
unsigned long _gameOverTimestamp = 0;
const int IGNORE_INPUT_AFTER_GAME_OVER_MS = 500; //ignores input for 500ms after game over

GameState _gameState = NEW_GAME;

void setup() { 
  _points = 0;

  Serial.begin(9600);
  pinMode(TONE_OUTPUT_PIN, OUTPUT);
  pinMode(VIBROMOTOR_OUTPUT_PIN, OUTPUT);

  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  showLoadScreen();

  _display.clearDisplay();

  for (int i = 0; i < 5; i++) {
    _cars[i].setDrawFill(true);
    _cars[i].draw(_display);
    _cars[i].set_random_speed();

    if (i % 2 != 0) {
      _cars[i].setSpeed(_cars[i].getSpeed() * -1);
    }
  }
  
  _display.display();
}

void loop() {
  _display.clearDisplay();

  // game is playing
  draw_status_bar();
  if (_gameState == PLAYING) {
    game_play_loop();
  } else {
    non_game_play_loop();
  }

  if (DELAY_LOOP_MS > 0) {
    delay(DELAY_LOOP_MS);
  }
}

void non_game_play_loop() {
  int16_t x1, y1;
  uint16_t w, h;

  int jumpButtonVal = digitalRead(FLAP_BUTTON_INPUT_PIN);

  if (_gameState == NEW_GAME) {
    _points = 0;
    _display.getTextBounds(STR_PRESS_JUMP_TO_PLAY, 0, 0, &x1, &y1, &w, &h);
    _display.setCursor(_display.width() / 2 - w / 2, 17);
    _display.print(STR_PRESS_JUMP_TO_PLAY);

    if (jumpButtonVal == LOW) {
      _gameState = PLAYING;
    }
  } else if (_gameState == GAME_OVER) {
    _display.setTextSize(2);
    _display.getTextBounds(STR_GAME_OVER, 0, 0, &x1, &y1, &w, &h);
    int yText = 15;
    _display.setCursor(_display.width() / 2 - w / 2, yText);
    _display.print(STR_GAME_OVER);

    yText = yText + h + 2;
    _display.setTextSize(1);
    _display.getTextBounds(STR_PRESS_JUMP_TO_PLAY, 0, 0, &x1, &y1, &w, &h);
    _display.setCursor(_display.width() / 2 - w / 2, yText);
    _display.print(STR_PRESS_JUMP_TO_PLAY);
    _display.setCursor(_display.width() / 2 - w / 2, yText + 16);
    char buffer[20];
    int temp = _points;
    Serial.println(temp);
    sprintf(buffer, "Your jumped %d times", temp);
    _display.print(buffer);
    // We ignore input a bit after game over so that user can see end game screen
    // and not accidentally start a new game
    if (jumpButtonVal == LOW && millis() - _gameOverTimestamp >= IGNORE_INPUT_AFTER_GAME_OVER_MS) {
      // if the current state is game over, need to reset
      // initializeGameEntities();
      _gameState = NEW_GAME;
    }
  }

  draw_road(0, true);
  int currentMillis = millis();
  _display.display();
  // _frog.draw_idle(_display, currentMillis);
}

void showLoadScreen() {
  _display.clearDisplay();
  _display.setTextSize(1);
  _display.setTextColor(WHITE, BLACK);

  int16_t x1, y1;
  uint16_t w, h;
  _display.setTextSize(1);

  int yText = 10;
  _display.getTextBounds(STR_MAKER, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_MAKER);

  _display.setTextSize(2);
  yText = yText + h + 1;
  _display.getTextBounds(STR_LOADSCREEN_APP_NAME_LINE1, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_LOADSCREEN_APP_NAME_LINE1);

  yText = yText + h + 1;
  _display.getTextBounds(STR_LOADSCREEN_APP_NAME_LINE2, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, yText);
  _display.print(STR_LOADSCREEN_APP_NAME_LINE2);

  _display.display();
  delay(LOAD_SCREEN_SHOW_MS);
  _display.clearDisplay();
  _display.setTextSize(1);

}

void game_play_loop() {
  // make the jump sequence
  car_move();
  // make the car go left, then car go right

  unsigned long currentMillis = millis();
  int jumpButtonVal = digitalRead(FLAP_BUTTON_INPUT_PIN);

  _frog.draw_sit(_display);
  _display.display();

  if (jumpButtonVal == LOW) {
    _points += 1;
    int increment = 0;
    increment += 8;
    update_screen(increment);
    update_screen(increment);
    _frog.draw_sit(_display);
    _display.display();
    tone(TONE_OUTPUT_PIN, 400, 50);  
  }

  for (int i = 0; i < 5; i++) {
    if (over_lap(_cars[i], _frog)) {
      digitalWrite(VIBROMOTOR_OUTPUT_PIN, HIGH);
      Serial.println("End game");
      _gameState = GAME_OVER;
      _gameOverTimestamp = millis();
      tone(TONE_OUTPUT_PIN, 200, 100); 
      delay(200); 
      tone(TONE_OUTPUT_PIN, 100, 100); 
      delay(200); 
      digitalWrite(VIBROMOTOR_OUTPUT_PIN, LOW);
    }
  }

  if (_points % 50 == 0 && _points != 0) {
    tone(TONE_OUTPUT_PIN, 600, 300); 
    tone(TONE_OUTPUT_PIN, 800, 300);  
    tone(TONE_OUTPUT_PIN, 1000, 300);
  }
}

void draw_status_bar() {
  _display.setTextSize(1);
  _display.setCursor(0, 3);
  _display.print(_points);
}

void car_move() {
  for (int i = 0; i < 5; i++) {
    int x_cordinate = _cars[i].getSpeed() + _cars[i].getX();
    if (x_cordinate > 140) {
      _cars[i].set_random_speed();
      _cars[i].setSpeed(-1 * _cars[i].getSpeed());
    } else if (x_cordinate < -30) {
      _cars[i].set_random_speed();
    }

    _cars[i].setX(_cars[i].getSpeed() + _cars[i].getX());
    _cars[i].draw(_display);
  }

  draw_road(0, true);
  _display.display();

}

void update_screen(int y_increment) {
  _display.clearDisplay();

  for (int i = 0; i < 5; i++) {
    _cars[i].setY((y_increment + _cars[i].getY()) % 80);
    _cars[i].draw(_display);
  }
  _frog.draw_leap(_display);
  draw_road(y_increment % 16 - 16, true);
  _display.display();
}

void draw_road(int y, bool white) {
  int color;

  if (white) {
    color = WHITE;
  } else {
    color = BLACK;
  }
  for (int i = 0; i < 5; i++) {
    _display.drawLine(0, i * 16 + y, 128, i * 16 + y, color);
    _display.drawLine(0, i * 16 + y + 1, 128, i * 16 + y + 1, color);
    _display.drawLine(0, i * 16 + y + 15, 128, i * 16 + y + 15, color);
    _display.drawLine(0, i * 16 + y + 16, 128, i * 16 + y + 16, color);

    _display.drawLine(8, i * 16 + y + 8, 16, i * 16 + y + 8, color);
    _display.drawLine(40, i * 16 + y + 8, 48, i * 16 + y + 8, color);
    _display.drawLine(72, i * 16 + y + 8, 80, i * 16 + y + 8, color);
    _display.drawLine(102, i * 16 + y + 8, 110, i * 16 + y + 8, color);
  }
}

bool over_lap(Car car, Frog frog) {
  return !(car.getRight() <= frog.getLeft() 
      || car.getBottom() <= frog.getTop() 
      || car.getLeft() >= frog.getRight() 
      || car.getTop() >= frog.getBottom());
}
