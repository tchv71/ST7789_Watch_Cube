#include <Arduino.h>
// ST7789 library example
// Analog watch/clock and STM32 RTC
// (c) 2019 Pawel A. Hernik
// YT videos:
// https://youtu.be/35Z0enhEYqM
// https://youtu.be/Xr-dxPhePhY

/*
 ST7789 240x240 IPS (without CS pin) connections (only 6 wires required):

 #01 GND -> GND
 #02 VCC -> VCC (3.3V only!)
 #03 SCL -> PA5/SCK
 #04 SDA -> PA7/MOSI
 #05 RES -> PA2 or any digital
 #06 DC  -> PA3 or any digital
 #07 BLK -> NC
 */

#define SCR_WD 240
#define SCR_HT 240

#include <SPI.h>
#include <Adafruit_GFX.h>
#define TFT_DC    PA3
#define TFT_RST   PA2
#include <Arduino_ST7789_STM.h>
#include <stdio.h>
#include "stm32f1xx_hal_rtc.h"


const int DEF_YEAR = 2000;

Arduino_ST7789 lcd = Arduino_ST7789(TFT_DC, TFT_RST);

//#include <RTClock.h>
//RTClock rtclock(RTCSEL_LSE);  // RTC set to LSE 32768Hz clock, counts seconds
typedef struct tm_t {
  uint8_t  year;    // years since 1970
  uint8_t  month;   // month of a year - [ 1 to 12 ]
  uint8_t  day;     // day of a month - [ 1 to 31 ]
  uint8_t  weekday; // day of a week (first day is Monday) - [ 0 to 6 ]
  uint8_t  pm;      // AM: 0, PM: 1
  uint8_t  hour;    // hour of a day - [ 0 to 23 ]
  uint8_t  minute;  // minute of an hour - [ 0 to 59 ]
  uint8_t  second;  // second of a minute - [ 0 to 59 ]
} tm_t;
tm_t curTime;
#include "smartq02.h"
#include "smartq08.h"
const unsigned char *clockface = clockface8;

#include "smartq04.h"
//const unsigned char *clockface = clockface4;
//#include "smartq09.h"
//const unsigned char *clockface = clockface9;
//#include "birds240.h"
//const unsigned char *clockface = birds240;
//#include "metal240.h"
//const unsigned char *clockface = metal240;
//#include "old240.h"
//const unsigned char *clockface = old240;
//#include "roman2_240.h"
//const unsigned char *clockface = roman2_240;
//#include "vintage240.h"
//const unsigned char *clockface = vintage240;

const unsigned char *(clockfaces[]) = { clockface8, clockface2, clockface4};

//#include "small4x6_font.h"

char buf[80];

uint8_t txt2num (const char *p)
{
  return 10 * (*p - '0') + *(p + 1) - '0';
}

// configuration

int cx = SCR_WD / 2, cy = SCR_HT / 2;  // clock center
int style = 1;
uint16_t selHandCol1 = RGBto565(250, 250, 0);
uint16_t selHandCol2 = RGBto565(180, 180, 0);
uint16_t mHandCol1 = RGBto565(220, 220, 220);
uint16_t mHandCol2 = RGBto565(150, 150, 150);
uint16_t hHandCol1 = RGBto565(220, 220, 220);
uint16_t hHandCol2 = RGBto565(150, 150, 150);
//uint16_t hHandCol = RGBto565(40,40,40);
//uint16_t mHandCol = RGBto565(80,80,80);
//uint16_t hHandCol = RGBto565(110,110,110);
//uint16_t mHandCol = RGBto565(180,180,180);
//uint16_t sHandCol = RED;
uint16_t sHandCol1 = RGBto565(250, 80, 80);
uint16_t sHandCol2 = RGBto565(200, 0, 0);
uint16_t cirCol = YELLOW;
int cirSize = 1;
int hHandL = 25 * 2, hHandW = 3 * 2;
int mHandL = 36 * 2, mHandW = 3 * 2;
int sHandL = 44 * 2, sHandW = 2 * 2;

int sDeg, mDeg, hDeg;
int sDegOld, mDegOld, hDegOld;
unsigned long styleTime, ms;
uint8_t hh = txt2num(__TIME__ + 0);
uint8_t mm = txt2num(__TIME__ + 3);
uint8_t ss = txt2num(__TIME__ + 6);
bool start = true;
enum ESetMode
{
  SET_NONE = 0,
  SET_HOURS,
  SET_MINUTES
}
setMode = SET_NONE;

// ----------------------------------------------------------------

uint16_t *palette = (uint16_t*) clockface + 3;
uint16_t line[SCR_WD + 2];

void imgLineH (int x, int y, int w)
{
  uint8_t v, *img = (uint8_t*) clockface + 16 * 2 + 6 + (y * SCR_WD + x) / 2;
  int ww = (x & 1) ? w + 1 : w;
  for (int i = 0; i < ww; i += 2)
  {
    v = pgm_read_byte(img++);
    line[i + 0] = palette[v >> 4];
    line[i + 1] = palette[v & 0xf];
  }
  lcd.drawImage(x, y, w, 1, (x & 1) ? line + 1 : line);
}

void imgRect (int x, int y, int w, int h)
{
  for (int i = y; i < y + w; i++)
    imgLineH(x, i, w);
}
// ----------------------------------------------------------------
#define swap(a, b) { int16_t t = a; a = b; b = t; }

void imgTriangle (int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t c = 0)
{
  int16_t t1x, t2x, y, minx, maxx, t1xp, t2xp;
  bool changed1 = false, changed2 = false;
  int16_t signx1, signx2, dx1, dy1, dx2, dy2;
  uint16_t e1, e2;

  if (y1 > y2)
  {
    swap(y1, y2);
    swap(x1, x2);
  }
  if (y1 > y3)
  {
    swap(y1, y3);
    swap(x1, x3);
  }
  if (y2 > y3)
  {
    swap(y2, y3);
    swap(x2, x3);
  }

  t1x = t2x = x1;
  y = y1;   // Starting points

  dx1 = x2 - x1;
  if (dx1 < 0)
  {
    dx1 = -dx1;
    signx1 = -1;
  }
  else
    signx1 = 1;
  dy1 = y2 - y1;

  dx2 = x3 - x1;
  if (dx2 < 0)
  {
    dx2 = -dx2;
    signx2 = -1;
  }
  else
    signx2 = 1;
  dy2 = y3 - y1;

  if (dy1 > dx1)
  {
    swap(dx1, dy1);
    changed1 = true;
  }
  if (dy2 > dx2)
  {
    swap(dy2, dx2);
    changed2 = true;
  }

  e2 = dx2 >> 1;
  if (y1 == y2)
    goto next;
  // Flat top, just process the second half
  e1 = dx1 >> 1;

  for (uint16_t i = 0; i < dx1;)
  {
    t1xp = 0;
    t2xp = 0;
    if (t1x < t2x)
    {
      minx = t1x;
      maxx = t2x;
    }
    else
    {
      minx = t2x;
      maxx = t1x;
    }
    // process first line until y value is about to change
    while (i < dx1)
    {
      i++;
      e1 += dy1;
      while (e1 >= dx1)
      {
        e1 -= dx1;
        if (changed1)
          t1xp = signx1;  //t1x += signx1;
        else
          goto next1;
      }
      if (changed1)
        break;
      else
        t1x += signx1;
    }
    // Move line
next1:
    // process second line until y value is about to change
    while (1)
    {
      e2 += dy2;
      while (e2 >= dx2)
      {
        e2 -= dx2;
        if (changed2)
          t2xp = signx2;  //t2x += signx2;
        else
          goto next2;
      }
      if (changed2)
        break;
      else
        t2x += signx2;
    }
next2: if (minx > t1x)
      minx = t1x;
    if (minx > t2x)
      minx = t2x;
    if (maxx < t1x)
      maxx = t1x;
    if (maxx < t2x)
      maxx = t2x;

    // line from min to max points found on the y
    if (c)
      lcd.drawFastHLine(minx, y, maxx - minx, c);
    else
      imgLineH(minx, y, maxx - minx);
    // increase y
    if (!changed1)
      t1x += signx1;
    t1x += t1xp;
    if (!changed2)
      t2x += signx2;
    t2x += t2xp;
    y += 1;
    if (y == y2)
      break;

  }
next:

  // Second half
  dx1 = x3 - x2;
  if (dx1 < 0)
  {
    dx1 = -dx1;
    signx1 = -1;
  }
  else
    signx1 = 1;
  dy1 = y3 - y2;
  t1x = x2;

  if (dy1 > dx1)
  {
    swap(dy1, dx1);
    changed1 = true;
  }
  else
    changed1 = false;

  e1 = dx1 >> 1;

  for (uint16_t i = 0; i <= dx1; i++)
  {
    t1xp = 0;
    t2xp = 0;
    if (t1x < t2x)
    {
      minx = t1x;
      maxx = t2x;
    }
    else
    {
      minx = t2x;
      maxx = t1x;
    }
    // process first line until y value is about to change
    while (i < dx1)
    {
      e1 += dy1;
      while (e1 >= dx1)
      {
        e1 -= dx1;
        if (changed1)
        {
          t1xp = signx1;
          break;
        }  //t1x += signx1;
        else
          goto next3;
      }
      if (changed1)
        break;
      else
        t1x += signx1;
      if (i < dx1)
        i++;
    }
next3:
    // process second line until y value is about to change
    while (t2x != x3)
    {
      e2 += dy2;
      while (e2 >= dx2)
      {
        e2 -= dx2;
        if (changed2)
          t2xp = signx2;
        else
          goto next4;
      }
      if (changed2)
        break;
      else
        t2x += signx2;
    }
next4: if (minx > t1x)
      minx = t1x;
    if (minx > t2x)
      minx = t2x;
    if (maxx < t1x)
      maxx = t1x;
    if (maxx < t2x)
      maxx = t2x;
    // line from min to max points found on the y
    if (c)
      lcd.drawFastHLine(minx, y, maxx - minx, c);
    else
      imgLineH(minx, y, maxx - minx);
    // increase y
    if (!changed1)
      t1x += signx1;
    t1x += t1xp;
    if (!changed2)
      t2x += signx2;
    t2x += t2xp;
    y += 1;
    if (y > y3)
      return;
  }
}

// ------------------------------------------------
#define MAXSIN 255
const uint8_t sinTab[91] PROGMEM =
  { 0, 4, 8, 13, 17, 22, 26, 31, 35, 39, 44, 48, 53, 57, 61, 65, 70, 74, 78, 83, 87, 91, 95, 99, 103, 107, 111, 115,
      119, 123, 127, 131, 135, 138, 142, 146, 149, 153, 156, 160, 163, 167, 170, 173, 177, 180, 183, 186, 189, 192, 195,
      198, 200, 203, 206, 208, 211, 213, 216, 218, 220, 223, 225, 227, 229, 231, 232, 234, 236, 238, 239, 241, 242, 243,
      245, 246, 247, 248, 249, 250, 251, 251, 252, 253, 253, 254, 254, 254, 254, 254, 255 };

int fastSin (int i)
{
  while (i < 0)
    i += 360;
  while (i >= 360)
    i -= 360;
  if (i < 90)
    return (pgm_read_byte(&sinTab[i]));
  else if (i < 180)
    return (pgm_read_byte(&sinTab[180 - i]));
  else if (i < 270)
    return (-pgm_read_byte(&sinTab[i - 180]));
  else
    return (-pgm_read_byte(&sinTab[360 - i]));
}

int fastCos (int i)
{
  return fastSin(i + 90);
}

// ------------------------------------------------

int px[8], py[8];

void drawHand (int deg, int style, int w, int l, int col1 = 0, int col2 = 0)
{
  int i, num = 4;
  cirCol = YELLOW;
  cirSize = 1;
  switch (style)
  {
    default:
    case 0:  // tris 013, 230, 024, rect with triangle
      px[0] = -w, py[0] = l - 5;
      px[1] = -w, py[1] = -10;
      px[2] = w, py[2] = l - 5;
      px[3] = w, py[3] = -10;
      px[4] = 0, py[4] = l - 5 + 7;
      num = 5;
      break;
    case 1:  // tris 013,023, peak style
      px[0] = 0, py[0] = l;
      px[1] = -w - 1, py[1] = 0;
      px[2] = w + 1, py[2] = 0;
      px[3] = 0, py[3] = -15;
      break;
    case 2:  // tris 013, 230, rect
      px[0] = -w, py[0] = l;
      px[1] = -w, py[1] = -10;
      px[2] = w, py[2] = l;
      px[3] = w, py[3] = -12;
      break;
    case 3:  // tris 013, 230, 024, rect with peak
      px[0] = -w - 1, py[0] = l - 15;
      px[1] = -w + 1, py[1] = -5;
      px[2] = w + 1, py[2] = l - 15;
      px[3] = w - 1, py[3] = -5;
      px[4] = 0, py[4] = l - 15 + 17;
      num = 5;
      cirCol = RED;
      cirSize = 3;
      break;
  }
  int x[5], y[5];
  int cc = fastCos(deg + 180);
  int ss = fastSin(deg + 180);
  for (i = 0; i < num; i++)
  {
    x[i] = px[i] * cc - py[i] * ss;
    y[i] = px[i] * ss + py[i] * cc;
    x[i] = cx + (x[i] + (x[i] > 0 ? MAXSIN / 2 : -MAXSIN / 2)) / MAXSIN;
    y[i] = cy + (y[i] + (y[i] > 0 ? MAXSIN / 2 : -MAXSIN / 2)) / MAXSIN;
  }
  /*
   imgTriangle(x[0],y[0], x[1],y[1], x[3],y[3], col);
   imgTriangle(x[2],y[2], x[3],y[3], x[0],y[0], col);
   if(num==5) imgTriangle(x[0],y[0], x[2],y[2], x[4],y[4], col);
   */
  imgTriangle(x[0], y[0], x[1], y[1], x[3], y[3], col2);
  imgTriangle(x[2], y[2], x[3], y[3], x[0], y[0], col1);
  //if(num==5) imgTriangle(x[0],y[0], x[2],y[2], x[4],y[4], col);
}

void drawHandS (int deg, int style, int w, int l, int col1 = 0, int col2 = 0)
{
  int i, num = 8;
  cirCol = YELLOW;
  cirSize = 3;
  px[0] = -w + 3, py[0] = l;
  px[1] = -w + 3, py[1] = -20;
  px[2] = w - 3, py[2] = l;
  px[3] = w - 3, py[3] = -20;
  px[4] = -w + 1, py[4] = -40;
  px[5] = -w + 1, py[5] = -15;
  px[6] = w - 1, py[6] = -40;
  px[7] = w - 1, py[7] = -15;
  int x[8], y[8];
  int cc = fastCos(deg + 180);
  int ss = fastSin(deg + 180);
  for (i = 0; i < num; i++)
  {
    x[i] = px[i] * cc - py[i] * ss;
    y[i] = px[i] * ss + py[i] * cc;
    x[i] = cx + (x[i] + (x[i] > 0 ? MAXSIN / 2 : -MAXSIN / 2)) / MAXSIN;
    y[i] = cy + (y[i] + (y[i] > 0 ? MAXSIN / 2 : -MAXSIN / 2)) / MAXSIN;
  }
  imgTriangle(x[0], y[0], x[1], y[1], x[3], y[3], col1);
  imgTriangle(x[2], y[2], x[3], y[3], x[0], y[0], col1);
  imgTriangle(x[4], y[4], x[5], y[5], x[7], y[7], col1);
  imgTriangle(x[6], y[6], x[7], y[7], x[4], y[4], col1);
}
/*
 void drawHandS(int deg, int style, int w, int l, int col1=0, int col2=0)
 {
 int i,num = 4;
 cirCol = YELLOW;
 cirSize = 1;
 switch(style) {
 default:
 case 0:  // tris 013, 230, 024, rect with triangle
 px[0]=-w+1, py[0]= l-5;
 px[1]=-w+1, py[1]=-10;
 px[2]= w-1, py[2]= l-5;
 px[3]= w-1, py[3]=-10;
 px[4]=   0, py[4]= l-5+7;
 num = 5;
 break;
 case 1:  // tris 013,023, peak style
 px[0]= 0, py[0]= l;
 px[1]=-w, py[1]= 0;
 px[2]= w, py[2]= 0;
 px[3]= 0, py[3]=-15;
 break;
 case 2:  // tris 013, 230, rect
 px[0]=-w+1, py[0]= l;
 px[1]=-w+1, py[1]=-10;
 px[2]= w-1, py[2]= l;
 px[3]= w-1, py[3]=-12;
 break;
 case 3:  // tris 013, 230, rect with peak, sec thin long
 px[0]=-w+1, py[0]= l;
 px[1]=-w+1, py[1]=-15;
 px[2]= w-1, py[2]= l;
 px[3]= w-1, py[3]=-15;
 cirCol = RED;
 cirSize = 3;
 break;
 }
 int x[5],y[5];
 int cc = fastCos(deg+180);
 int ss = fastSin(deg+180);
 for(i=0;i<num;i++) {
 x[i] = px[i]*cc - py[i]*ss;
 y[i] = px[i]*ss + py[i]*cc;
 x[i] = cx + (x[i]+(x[i]>0?MAXSIN/2:-MAXSIN/2))/MAXSIN;
 y[i] = cy + (y[i]+(y[i]>0?MAXSIN/2:-MAXSIN/2))/MAXSIN;
 }
 //imgTriangle(x[0],y[0], x[1],y[1], x[3],y[3], col);
 //imgTriangle(x[2],y[2], x[3],y[3], x[0],y[0], col);
 //if(num==5) imgTriangle(x[0],y[0], x[2],y[2], x[4],y[4], col);
 imgTriangle(x[0],y[0], x[1],y[1], x[3],y[3], col2);
 imgTriangle(x[2],y[2], x[3],y[3], x[0],y[0], col1);
 }
 */

void nextHandStyle ()
{
  if (millis() - styleTime < 15000)
    return;
  styleTime = millis();
  drawHand(hDegOld, style, hHandW, hHandL);
  drawHand(mDegOld, style, mHandW, mHandL);
  drawHandS(sDegOld, style, sHandW, sHandL);
  if (++style > 3)
    style = 0;
  start = 1;
}

void clockUpdate ()
{
  if (millis() - ms < 1000 && !start)
    return;
  ms = millis();
  if (setMode == SET_NONE)
  {
    hh = curTime.hour;
    mm = curTime.minute;
    ss = curTime.second;
  }

  sDeg = ss * 6;

  mDeg = mm * 6 + sDeg / 60;
  hDeg = hh * 30 + ((setMode == SET_HOURS) ? 0 : mDeg / 12);
  if (hDeg != hDegOld || mDeg != mDegOld || start)
  {
    start = 0;
    drawHand(hDegOld, style, hHandW, hHandL);
    drawHand(mDegOld, style, mHandW, mHandL);
    mDegOld = mDeg;
    hDegOld = hDeg;
  }

  drawHandS(sDegOld, style, sHandW, sHandL);
  if (setMode == SET_HOURS)
    drawHand(hDeg, style, hHandW, hHandL, selHandCol1, selHandCol2);
  else
    drawHand(hDeg, style, hHandW, hHandL, hHandCol1, hHandCol2);
  if (setMode != SET_HOURS)
  {
    if (setMode == SET_MINUTES)
      drawHand(mDeg, style, mHandW, mHandL, selHandCol1, selHandCol2);
    else
      drawHand(mDeg, style, mHandW, mHandL, mHandCol1, mHandCol2);
  }
  if (setMode == SET_NONE)
    drawHandS(sDeg, style, sHandW, sHandL, sHandCol1, sHandCol2);
  sDegOld = sDeg;

  //lcd.fillCircle(cx,cy, cirSize, cirCol);
  lcd.fillCircle(cx, cy, 4, RGBto565(40, 40, 40));

  //snprintf(buf,20,"%02d:%02d:%02d",hh,mm,ss);
  //lcd.setCursor(0,0); lcd.setTextColor(BLACK);
  //lcd.fillRect(0,0,6*7+8,8,WHITE);  lcd.print(buf);
}

// --------------------------------------------------------------------------
int face = 0;
void nextFace ()
{
  if (++face >= int(sizeof(clockfaces)/sizeof(clockfaces[0])))
    face = 0;
  clockface = clockfaces[face];
  palette = (uint16_t*) clockface + 3;
  imgRect(0, 0, SCR_WD, SCR_HT);
  start = 1;
}
// --------------------------------------------------------------------------
#define BUTTON PB9

int stateOld = HIGH;
static const long btDebounce = 30;
static const long btDoubleClick = 600;
static const long btLongClick = 700;
static const long btLongerClick = 2000;
long btTime = 0, btTime2 = 0;
int clickCnt = 1;

// 0=idle, 1,2,3=click, -1,-2=longclick
enum EButtonState
{
  eBsIdle = 0,
  eBsClick,
  eBsDoubleClick,
  eBsLongPress = -1,
  eBsLongLongPress = -2
};

EButtonState checkButton ()
{
  int state = digitalRead(BUTTON);
  if (state == LOW && stateOld == HIGH)
  {
    btTime = millis();
    stateOld = state;
    return eBsIdle;
  } // button just pressed
  if (state == HIGH && stateOld == LOW)
  { // button just released
    stateOld = state;
    int32 millisNow = millis();
    if (millisNow - btTime >= btDebounce && millisNow - btTime < btLongClick)
    {
      if (millisNow - btTime2 < btDoubleClick)
        clickCnt++;
      else
        clickCnt = 1;
      btTime2 = millis();
      return clickCnt == 1 ? eBsClick : eBsDoubleClick;
    }
  }
  int32 millisNow = millis();
  if (state == LOW && millisNow - btTime >= btLongerClick)
  {
    stateOld = state;
    return eBsLongLongPress;
  }
  if (state == LOW && millisNow - btTime >= btLongClick)
  {
    stateOld = state;
    return eBsLongPress;
  }
  return eBsIdle;
}


extern RTC_HandleTypeDef hrtc;

// --------------------------------------------------------------------------

const char *months[] =
  { "???", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
const char *delim = " :";
char bld[40];

uint8_t str2month (const char *d)
{
  uint8_t i = 13;
  while ((--i) && strcmp(months[i], d))
    ;
  return i;
}

void setRtcTime (struct tm_t &mt)
{
  //snprintf(bld,40,"Build: %02d-%02d-%02d %02d:%02d:%02d\n",mt.year+1970,mt.month,mt.day,mt.hour,mt.minute,mt.second); Serial.println(bld);
  RTC_TimeTypeDef t;
  t.Hours = mt.hour;
  t.Minutes = mt.minute;
  t.Seconds = mt.second;
  HAL_RTC_SetTime(&hrtc, &t, RTC_FORMAT_BIN);
  RTC_DateTypeDef d;
  d.Year = mt.year;
  d.Month = mt.month;
  d.Date = mt.day;
  HAL_RTC_SetDate(&hrtc, &d, RTC_FORMAT_BIN);
}

void setBuildTime (struct tm_t &mt)
{
  // Timestamp format: "Mar 3 2019 12:34:56"
  snprintf(bld, 40, "%s %s\n", __DATE__, __TIME__);
  char *token = strtok(bld, delim);
  while (token)
  {
    int m = str2month((const char*) token);
    if (m > 0)
    {
      mt.month = m;
      token = strtok(NULL, delim);
      mt.day = atoi(token);
      token = strtok(NULL, delim);
      mt.year = atoi(token) - DEF_YEAR;
      token = strtok(NULL, delim);
      mt.hour = atoi(token);
      token = strtok(NULL, delim);
      mt.minute = atoi(token);
      token = strtok(NULL, delim);
      mt.second = atoi(token);
    }
    token = strtok(NULL, delim);
  }
  //snprintf(bld,40,"Build: %02d-%02d-%02d %02d:%02d:%02d\n",mt.year+1970,mt.month,mt.day,mt.hour,mt.minute,mt.second); Serial.println(bld);
  setRtcTime(mt);
  //rtclock.setTime(makeTime(mt) + 10);
}

void readRtcTime(void)
{
  RTC_TimeTypeDef t;
  HAL_RTC_GetTime(&hrtc, &t, RTC_FORMAT_BIN);
  tm_t& mt = curTime;
  mt.hour = t.Hours;
  mt.minute= t.Minutes ;
  mt.second= t.Seconds ;

  RTC_DateTypeDef d;
  HAL_RTC_GetDate(&hrtc, &d, RTC_FORMAT_BIN);
  mt.year = d.Year + DEF_YEAR ;
  mt.month = d.Month;
  mt.day = d.Date;
}

void setup ()
{
  //Serial.begin(9600);
  //pinMode(BUTTON, INPUT_PULLUP);
  //pinMode(PC13, OUTPUT);
  lcd.init(SCR_WD, SCR_HT);
  //time_t now = RTC_ReadTimeCounter(&hrtc);//rtclock.now();
  //rtclock.breakTime(now, curTime);
  readRtcTime();
  if (curTime.year + DEF_YEAR < 2019)
    setBuildTime(curTime);  //  <2019 - invalid year
  imgRect(0, 0, SCR_WD, SCR_HT);
  ms = millis();
}


#define pin_CLK PB7
#define pin_DT  PB8
#define pin_Btn PB9
unsigned long CurrentTime, LastTime;
enum eEncoderState {eNone, eLeft, eRight, eButton};
uint8_t EncoderA, EncoderB, EncoderAPrev;
int16_t counter;
bool ButtonPrev;
eEncoderState GetEncoderState() {
  // Считываем состояние энкодера
  eEncoderState Result = eNone;
  CurrentTime = millis();
  if (CurrentTime - LastTime >= 5) {
    // Считываем не чаще 1 раза в 5 мс для уменьшения ложных срабатываний
    LastTime = CurrentTime;
    if (digitalRead(pin_Btn) == LOW ) {
      if (ButtonPrev) {
        Result = eButton; // Нажата кнопка
        ButtonPrev = 0;
      }
    }
    else {
      ButtonPrev = 1;
      EncoderA = digitalRead(pin_CLK);
      EncoderB = digitalRead(pin_DT);
      if ((!EncoderA) && (EncoderAPrev)) { // Сигнал A изменился с 1 на 0
        if (EncoderB) Result = eRight;     // B=1 => энкодер вращается по часовой
        else          Result = eLeft;      // B=0 => энкодер вращается против часовой
      }
      EncoderAPrev = EncoderA; // запомним текущее состояние
    }
  }
  return Result;
}
bool bWasReleased = false;
bool bNextFaceDone = false;
void loop ()
{
  //rtclock.breakTime(rtclock.now(), curTime);
  readRtcTime();
  EButtonState st = checkButton();
  if (st==eBsIdle)
    bNextFaceDone = false;
  if (st == eBsClick && setMode == SET_NONE)
  {
    setMode = SET_MINUTES;
    bWasReleased = false;
  }
  if (st == eBsDoubleClick && setMode == 0)
  {
    setMode = SET_HOURS;
    bWasReleased = false;
  }
  if (st == eBsDoubleClick)
  {
    setMode = SET_HOURS;
    bWasReleased = false;
    start = 1;
  }
  if (setMode != SET_NONE)
  {
    if (st == eBsClick && bWasReleased)
    {
//      if (++setMode > 2)
//        setMode = 0;
      setMode = SET_NONE;
    }
    if (st == eBsIdle)
      bWasReleased = true;
    eEncoderState s = GetEncoderState();
    if ((s == eLeft || s == eRight))
    {
      if (setMode == SET_HOURS /* && st < 0*/)
      {
        switch (s)
        {
          case eLeft:
            if (hh == 0)
              hh = 24;
            else
              --hh;
            break;
          case eRight:
            if (++hh >= 24)
              hh = 0;
            break;
          default:
            break;
        }
        start = 1;
      }
      if (setMode == SET_MINUTES /*&& st < 0*/)
      {
        static const uint8 SPEEDUP = 5;
        static unsigned STEP_TRESH = 50;
        uint8 step = 1;
        uint32_t m = millis();
        static uint32_t mOldLeft = 0;
        static uint32_t mOldRight = 0;
        switch (s)
        {
          case eLeft:
            if (m-mOldLeft<STEP_TRESH)
              step = SPEEDUP;
            mOldLeft = m;
            if (mm < step)
            {
              mm = 60 + 1 - step;
              if (hh == 0)
                hh = 24;
              else
                --hh;
            }
            else
              mm -= step;
            break;
          case eRight:
            if (m-mOldRight<STEP_TRESH)
              step = SPEEDUP;
            mOldRight = m;
            if ((mm+=step) >= 60)
            {
              mm = mm-60;
              if (++hh >= 24)
                hh = 0;
            }
            break;
          default:
            break;
        }
        start = 1;
      }
    }
    curTime.hour = hh;
    curTime.minute = mm;
    setRtcTime(curTime);
    //tclock.setTime(rtclock.makeTime(curTime));
  }
  else
  {
    if (st == eBsLongPress && !bNextFaceDone)
      nextFace(), bNextFaceDone = true;
  }
  clockUpdate();
}

extern "C"
{
  void loop_c()\
  {
    loop();
  }
  void setup_c()
  {
    setup();
  }
}
