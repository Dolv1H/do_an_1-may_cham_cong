#include <Keypad.h>

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 16, /* data=*/ 17);   // ESP32 Thing, HW I2C with pin remapping

const int ledPins[] = { A1, A2, A3 }; // Green, Yellow, Red
int keyCount = 0; // Count digits
const char *correctPin = "15937";
char currentPin[10];

// Matrix Key Pad
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

// initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

// draw up to five digits w/o resetting the display
void drawPin(int count, char number) {
  do {
    u8g2.setFont(u8g2_font_logisoso32_tn);
    u8g2.setCursor(18 * count, 42);
    u8g2.print(number);
  } while ( u8g2.nextPage() );
  delay(200);
}

// draw access granted symbol and text
void drawGranted() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_open_iconic_check_6x_t);
    u8g2.setCursor(4, 58);
    u8g2.print("A");
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.setCursor(65, 25);
    u8g2.print("Access");
    u8g2.setCursor(65, 50);
    u8g2.print("Granted");
  } while ( u8g2.nextPage() );
  delay(2000);
}

// draw access denied symbol and text
void drawDenied() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_open_iconic_check_6x_t);
    u8g2.setCursor(4, 58);
    u8g2.print("B");
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.setCursor(65, 25);
    u8g2.print("Access");
    u8g2.setCursor(65, 50);
    u8g2.print("Denied");
  } while ( u8g2.nextPage() );
  delay(2000);
}

// reset all entered digits, digit counter and display
void reset() {
  char currentPin[10];
  keyCount = 0;
  u8g2.clearDisplay();
}

void setup() {
  Serial.begin(9600);
  pinMode( ledPins[0], OUTPUT );
  pinMode( ledPins[1], OUTPUT );
  pinMode( ledPins[2], OUTPUT );
  u8g2.begin();   
  u8g2.enableUTF8Print();
  u8g2.firstPage();
}
  
void loop() {
  char customKey = customKeypad.getKey(); // get key from keypad
  
  if ( customKey ){
    Serial.println(customKey);
    digitalWrite( ledPins[1], HIGH ); // Yellow
    delay(100);
    digitalWrite( ledPins[1], LOW );
    
    if ( isDigit(customKey) ) {  // test if customKey is a digit
      currentPin[keyCount] = customKey; // add newly entered digit to currentPin
      keyCount++;
      drawPin(keyCount, customKey);
    } else if ( customKey == 'D' ) { // keypad D pressed / reset counters and display
      reset();
    }

    if ( keyCount >= 5 ) { // five digits were entered
      if ( strcmp(correctPin, currentPin) == 0 ) { // compare correctPin with currentPin
        Serial.println("PIN correct");
        digitalWrite( ledPins[0], HIGH ); // Green
        drawGranted();
        digitalWrite( ledPins[0], LOW );
        reset();
      } else {
        Serial.println("PIN incorrect");
        digitalWrite( ledPins[2], HIGH ); // Red
        drawDenied();
        digitalWrite( ledPins[2], LOW );
        reset();
      }
      u8g2.firstPage(); // init display for next run
    }
  }
}
