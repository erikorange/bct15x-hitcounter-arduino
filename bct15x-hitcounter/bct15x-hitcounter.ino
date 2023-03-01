#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 4);

#define LCD_ROW1 0
#define LCD_ROW2 1
#define LCD_ROW3 2
#define LCD_ROW4 3

#define bufSize 100
char buffer[bufSize];
bool gotResponse;

void setup()
{
  lcd.init();
  lcd.backlight();
  Serial.begin(19200);
  DisplayTitle();
}

void loop()
{
  Serial.print("GLG\r");

  while (!recvWithEndMarker())
  {
    lcd.setCursor(1, LCD_ROW1);
    lcd.print("Waiting for data");
    delay(200);
    lcd.clear();
  }

  lcd.clear();
  lcd.setCursor(1, LCD_ROW2);
  lcd.print("Got data");

  for (int i = 0; i<18; i++)
  {
    lcd.setCursor(i+1, LCD_ROW3);
    lcd.print(buffer[i]);
  }
  delay(2000);
    
  lcd.clear();
  delay(2000);
}

bool recvWithEndMarker() {
    bool done = false;
    int idx = 0;
    char endMarker = '\r';
    char rc;
    
    if (Serial.available() == 0)
    {
      return false;
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
    return true;
}

// Display the opening title
void DisplayTitle()
{
  lcd.setCursor(1, LCD_ROW1);
  lcd.print("BCT15X Hit Counter");
  lcd.setCursor(4, LCD_ROW2);
  lcd.print("Version 1.0.0");
  lcd.setCursor(2, LCD_ROW3);
  lcd.print("(c) Erik Orange");
  delay(2000);
  lcd.clear();
}