#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define LCD_ROW1 0
#define LCD_ROW2 1
#define LCD_ROW3 2
#define LCD_ROW4 3

#define bufSize 100
char buffer[bufSize];
char closedSq[] = "GLG,,,,,,,,,,,,";

int idx;
char freq[8];
char alphaTag[17];

int spinIdx;
long mark;


void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.clear();
  Serial.begin(19200);
  DisplayTitle();
  spinIdx = 0;
  mark = millis();
  lcd.setCursor(19, 3);
  lcd.print("-\0");
}

void loop()
{
  Serial.print("GLG\r");
  clearBuffer(buffer);
  Serial.readBytesUntil('\r', buffer, 100);

  if (isValidData(buffer))
  {
    if (isStoppedOnChan(buffer))
    {
      getElement(buffer, 2, freq);
      freq[7] = '\0';
      lcd.setCursor(0,0);
      lcd.print(freq);

      getElement(buffer, 8, alphaTag);
      alphaTag[16] = '\0';
      lcd.setCursor(0,1);
      lcd.print(alphaTag);            
    }
  }

  if (millis() - mark > 250)
  {
    mark = millis();
    spinIdx = 1 - spinIdx;
    lcd.setCursor(19, 3);
    if (spinIdx)
    {
      lcd.print("+\0");
    }
    else
    {
      lcd.print("-\0");
    }
  }
}

void clearBuffer(char* buf)
{
  for (int i = 0; i < bufSize; i++)
  {
    buf[i] = '\0';
  }
}

bool isStoppedOnChan(char* buf)
{
  if (strcmp(buf, closedSq))
  {
    return true;
  }
  return false;
}

bool isValidData(char* scannerData)
{
	int count = 0;
	for (int i = 0; i < strlen(scannerData); i++)
	{
		if (scannerData[i] == ',')
		{
			count++;
		}
	}

	return (count == 12);
}

void getElement(char* scannerData, int elementIdx, char* element)
{
	int idx;
	int commaNum;
	int commaCount;

	idx = 0;
	commaNum = elementIdx - 1;
	commaCount = 0;
	while (commaCount != commaNum)
	{
		if (scannerData[idx] == ',')
		{
			commaCount++;
		}
		idx++;
	}

	elementIdx = 0;
	while (scannerData[idx] != ',')
	{
		element[elementIdx++] = scannerData[idx++];
	}
	return;
}

bool isSquelchClosed(char* buffer, char* sq)
{
  int idx = 0;
  while (idx < 15)
  {
    if (buffer[idx] != sq[idx])
    {
      return false;
    }
    idx++;
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
  delay(1000);
  lcd.clear();
}