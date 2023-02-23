#include <LiquidCrystal.h>

// LCD rows
const int LCDROW1 = 0;
const int LCDROW2 = 1;
const int LCDROW3 = 2;
const int LCDROW4 = 3;

// I/O pins
const int BUTTON_DOWN = 6;
const int BUTTON_UP = 7;
const int BUTTON_MAN = 8;
const int BUTTON_SCAN = 9;
const int BACKLIGHT = 13;

// Definitions for button HIGH/LOW based on using pullup resistors
const int BUTTON_OPEN = HIGH;
const int BUTTON_CLOSED = LOW;

// Connections:
// rs (LCD pin 4) to Arduino pin 12
// rw (LCD pin 5) to Arduino pin 11
// enable (LCD pin 6) to Arduino pin 10
// LCD pin 15 to Arduino pin 13
// LCD pins d4, d5, d6, d7 to Arduino pins 5, 4, 3, 2
LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2);









int bufSize = 100;

char buffer[100];

void setup() {
  // Configure input pushbuttons
  pinMode(BUTTON_DOWN, INPUT);
  digitalWrite(BUTTON_DOWN, HIGH);     // turn on the built in pull-up resistor
  
  pinMode(BUTTON_UP, INPUT);
  digitalWrite(BUTTON_UP, HIGH);       // turn on the built in pull-up resistor
  
  pinMode(BUTTON_MAN, INPUT);
  digitalWrite(BUTTON_MAN, HIGH);      // turn on the built in pull-up resistor
  
  pinMode(BUTTON_SCAN, INPUT);
  digitalWrite(BUTTON_SCAN, HIGH);     // turn on the built in pull-up resistor

  // Configure backlight
  pinMode(BACKLIGHT, OUTPUT);
  digitalWrite(BACKLIGHT, HIGH);

  lcd.begin(20, 4);
  lcd.clear();

  Serial.begin(19200);

  DisplayTitle();
}

void loop() {
  //Serial.write("GLG\r");
  delay(100);
  //recvWithEndMarker();
  delay(100);
}


void recvWithEndMarker() {
    bool done = false;
    int idx = 0;
    char endMarker = '\r';
    char rc;
    
    if (Serial.available() == 0)
    {
      return;
    }

    while (Serial.available() > 0 && done == false) {
        rc = Serial.read();

        if (rc != endMarker) {
            buffer[idx] = rc;
            idx++;
            if (idx >= bufSize) {
                idx = bufSize - 1;
            }
        }
        else {
            buffer[idx] = '\0';
            done = true;
        }
    }
    Serial.println(buffer);
}

// Display the opening title
void DisplayTitle()
{
  lcd.setCursor(1, LCDROW1);
  lcd.print("BCT15X Hit Counter");
  lcd.setCursor(4, LCDROW2);
  lcd.print("Version 1.0.0");
  lcd.setCursor(2, LCDROW3);
  lcd.print("(c) Erik Orange");
  delay(2000);
}