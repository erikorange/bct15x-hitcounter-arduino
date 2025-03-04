#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define LCD_ROW1 0
#define LCD_ROW2 1
#define LCD_ROW3 2
#define LCD_ROW4 3

#define BUFSIZE 100
#define FREQ_LEN 8
#define TAG_LEN 17
#define SYSNAME_LEN 17

char buffer[BUFSIZE];

char freq[FREQ_LEN];
char alphaTag[TAG_LEN];
char sysName[SYSNAME_LEN];
bool gotHit;
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
  gotHit = false;
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
    if (!isScanning(buffer))
    {
      if (!gotHit && isSquelchOpen(buffer))
      {
        gotHit = true;
        lcd.clear();

        getFreq(buffer, freq);
        lcd.setCursor(0,0);
        lcd.print(freq);

        getAlphaTag(buffer, alphaTag);
        lcd.setCursor(0,1);
        lcd.print(alphaTag);

        getSysName(buffer, sysName);
        lcd.setCursor(0,2);
        lcd.print(sysName);                       
      }

      if (gotHit && !isSquelchOpen(buffer))
      {
        gotHit = false;        
      }
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
  for (int i = 0; i < BUFSIZE; i++)
  {
    buf[i] = '\0';
  }
}

bool isScanning(char* buf)
{
  static char closedSq[] = "GLG,,,,,,,,,,,,";
  if (strcmp(buf, closedSq))
  {
    return false;
  }
  return true;
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


void getFreq(char* buffer, char* freq)
{
      getElement(buffer, 2, freq);
      freq[FREQ_LEN-1] = '\0';
}

void getAlphaTag(char* buffer, char* tag)
{
  for (int i = 0; i < TAG_LEN; i++)    // clear array so short tags don't have long tag leftovers
  {
    tag[i] = '\0';
  }
  getElement(buffer, 8, tag);
}

void getSysName(char* buffer, char* sysName)
{
  for (int i = 0; i < SYSNAME_LEN; i++)    // clear array so short tags don't have long tag leftovers
  {
    sysName[i] = '\0';
  }
  getElement(buffer, 6, sysName);
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

bool isSquelchOpen(char* buffer)
{
  char sqFlag[1];
  
  getElement(buffer, 9, sqFlag);
  if (sqFlag[0] == '1')
  {
    return true;
  }
  return false;
}

void DisplayTitle()
{
  lcd.setCursor(3, LCD_ROW1);
  lcd.print("BCT15X Display");
  lcd.setCursor(4, LCD_ROW2);
  lcd.print("Version 1.0.1");
  lcd.setCursor(2, LCD_ROW3);
  lcd.print("(c) Erik Orange");
  delay(1000);
  lcd.clear();
}