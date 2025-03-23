#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd_1(0x27, 20, 4);

#define LCD_ROW1 0
#define LCD_ROW2 1
#define LCD_ROW3 2
#define LCD_ROW4 3

#define BUFSIZE 100
#define FREQ_LEN 8
#define TAG_LEN 17
#define SYSNAME_LEN 17

#define HIT_ARRAY_SIZE 25
#define HIT_ARRAY_FULL -1
#define HIT_NOT_FOUND -1

#define ACTIVITY_DOTS_X 15
#define NUM_ACTIVITY_DOTS 8

char buffer[BUFSIZE];

typedef struct {
    int count;
    char freq[FREQ_LEN];
    char alphaTag[TAG_LEN];
} Hit;

Hit hits[HIT_ARRAY_SIZE];


char freq[FREQ_LEN];
char alphaTag[TAG_LEN];
char sysName[SYSNAME_LEN];
bool gotHit;
bool foundAHit;
int spinIdx;
long serialMark, hitMark;
int hitDisplayIdx;
bool foundNextDisplayHit;

char *activityDots[] = {
  ".    \0",
  " .   \0",
  "  .  \0",
  "   . \0",
  "    .\0",
  "   . \0",
  "  .  \0",
  " .   \0"
};

void setup()
{
  lcd_1.init();
  lcd_1.backlight();
  lcd_1.clear();
  Serial.begin(19200);
  DisplayTitle();

  initializeHits();

  gotHit = false;
  foundAHit = false;
  serialMark = millis();
  hitMark = millis();

  hitDisplayIdx = 0;
  
  lcd_1.setCursor(ACTIVITY_DOTS_X, LCD_ROW4);
  spinIdx = 0;
  lcd_1.print(activityDots[spinIdx]);
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
        foundAHit = true;
        gotHit = true;
        
        /* lcd_1.clear(); */

        getFreq(buffer, freq);
        /*lcd_1.setCursor(0,LCD_ROW1);
        lcd_1.print(freq);
        lcd_1.setCursor(8, LCD_ROW1);
        lcd_1.print("MHz"); */

        getAlphaTag(buffer, alphaTag);
        /* lcd_1.setCursor(0,LCD_ROW2);
        lcd_1.print(alphaTag); */

        /*getSysName(buffer, sysName);
        lcd_1.setCursor(0,LCD_ROW3);
        lcd_1.print(sysName);
        */
        addToHitListfreq(freq, alphaTag);                 
      }

      if (gotHit && !isSquelchOpen(buffer))
      {
        gotHit = false;        
      }
    }
  }

/*
  if (millis() - serialMark > 250)
  {
    serialMark = millis();

    spinIdx++;
    if (spinIdx == NUM_ACTIVITY_DOTS)
    {
      spinIdx = 0;
    }
    
    lcd_1.setCursor(ACTIVITY_DOTS_X, LCD_ROW4);
    lcd_1.print(activityDots[spinIdx]);
  }
*/

  if (millis() - hitMark > 2000)
  {
    if (foundAHit)
    {
      foundNextDisplayHit = false;
      do
      {
        if (hits[hitDisplayIdx].count != 0)
        {
          foundNextDisplayHit = true;

          /* display the hit */
          lcd_1.clear();

          lcd_1.setCursor(0,LCD_ROW1);
          lcd_1.print(hits[hitDisplayIdx].freq);
          lcd_1.setCursor(8, LCD_ROW1);
          lcd_1.print("MHz");

          lcd_1.setCursor(0,LCD_ROW2);
          lcd_1.print(hits[hitDisplayIdx].alphaTag);
          lcd_1.setCursor(0,LCD_ROW3);
          lcd_1.print("Hits: ");
          lcd_1.setCursor(6, LCD_ROW3);
          lcd_1.print(hits[hitDisplayIdx].count);

          hitDisplayIdx++;
        }
        else
        {
          hitDisplayIdx++;
          if (hitDisplayIdx == HIT_ARRAY_SIZE)
          {
            hitDisplayIdx = 0;
          }
        }
      }
      while (!foundNextDisplayHit);
    }
    hitMark = millis();
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

void initializeHits()
{
  for (int i = 0; i < HIT_ARRAY_SIZE; i++)
  {
    hits[i].count = 0;

    for (int j = 0; j < FREQ_LEN; j++)
    {
      hits[i].freq[j] = '\0';
    }

    for (int j = 0; j < TAG_LEN; j++)
    {
      hits[i].alphaTag[j] = '\0';
    }
  }
}

void addToHitListfreq(char* freq, char* alphaTag)
{
  int i = findHit(freq, alphaTag);
  if (i == HIT_NOT_FOUND)
  {
    int j = findNextHitSlot();
    if (j == HIT_ARRAY_FULL)
    {
      return;
    }
    else
    {
      strcpy(hits[j].freq, freq);
      strcpy(hits[j].alphaTag, alphaTag);
      hits[j].count++;
    }
  }
  else
  {
    hits[i].count++;
  }
}

int findHit(char* freq, char* alphaTag)
{
  for (int i = 0; i < HIT_ARRAY_SIZE; i++)
  {
    if ( (strcmp(freq, hits[i].freq) == 0) && (strcmp(alphaTag, hits[i].alphaTag) == 0) )
    return i;
  }

  return HIT_NOT_FOUND;
}

int findNextHitSlot()
{
  for (int i = 0; i < HIT_ARRAY_SIZE; i++)
  {
    if (hits[i].count == 0)
    {
      return i;
    }
  }
  return HIT_ARRAY_FULL;
}


void DisplayTitle()
{
  lcd_1.setCursor(3, LCD_ROW1);
  lcd_1.print("BCT15X Display");
  lcd_1.setCursor(4, LCD_ROW2);
  lcd_1.print("Version 1.1.0");
  lcd_1.setCursor(2, LCD_ROW3);
  lcd_1.print("(c) Erik Orange");
  delay(1000);
  lcd_1.clear();
}