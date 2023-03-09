#include <Adafruit_TFTLCD.h> 
#include <Adafruit_GFX.h>    
#include <TouchScreen.h>
#include <Wire.h>


#define LCD_CS A3 
#define LCD_CD A2 
#define LCD_WR A1 
#define LCD_RD A0 
#define LCD_RESET A4 

#define TS_MINX 122
#define TS_MINY 111
#define TS_MAXX 942
#define TS_MAXY 890

#define YP A3
#define XM A2
#define YM 9
#define XP 8

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 364);

boolean buttonEnabled = true;

int frequency=0;

String state = "UNMUTED";
double stations[] = {100.6, 104.2,90.90};
int currentstation = 0;

int Txcommand = 0;


void setup() {
  Wire.begin(5);
  Wire.onRequest(requestEvent);  
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.setRotation(1);
  tft.fillScreen(WHITE);
  tft.drawRect(0,0,319,240,BLACK);
  
  tft.setCursor(30,20);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print("SMART CAR CONTROLLER");
  
  

  //Frequency Incrementer
  tft.fillRect(250,50, 40, 40, RED);
  tft.drawRect(250,50,40,40,BLACK);
  tft.setCursor(263,60);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("+");

  //Frequency Decrementer
  tft.fillRect(250,100, 40, 40, RED);
  tft.drawRect(250,100,40,40,BLACK);
  tft.setCursor(263,110);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("-");

  //Frequency Viewer
  tft.fillRect(205,75, 40, 40, WHITE);
  tft.drawRect(205,75,40,40,BLACK);

  //Mute
  tft.fillRect(30,175, 60, 40, BLUE);
  tft.drawRect(30,175,60,40,BLACK);
  tft.setCursor(38,185);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print("MUTE");

  //UnMute
  tft.fillRect(120,175, 90, 40, BLUE);
  tft.drawRect(120,175,90,40,BLACK);
  tft.setCursor(128,185);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print("UNMUTE");

  //UnMute
  tft.fillRect(70,125, 100, 40, WHITE);
  tft.drawRect(70,125,100,40,BLACK);
  tft.setCursor(78,135);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print("UNMUTED");

}

void requestEvent() {
  Wire.write(Txcommand);
}

void loop() {
  
  
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  if (p.z > ts.pressureThreshhold) {
    
   p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);
   p.y = map(p.y, TS_MAXY, TS_MINY, 0, 480);

   /*String y = String(p.y);
   String x = String(p.x);
    
    tft.fillRect(150,150, 40, 40, WHITE);
    tft.drawRect(150,150,40,40,BLACK);
    tft.setCursor(160,160);
    tft.setTextColor(BLACK);
    tft.setTextSize(1);
    tft.print(x);

    tft.fillRect(200,150, 40, 40, WHITE);
    tft.drawRect(200,150,40,40,BLACK);
    tft.setCursor(210,160);
    tft.setTextColor(BLACK);
    tft.setTextSize(1);
    tft.print(y);
    */
       
   if(p.x>74 && p.x<120 && p.y>398 && p.y<455 && buttonEnabled){
      buttonEnabled = false;
      if(currentstation<2){
        currentstation++;  
        
      }
      Txcommand = currentstation;
   }  

   if(p.x>144 && p.x<195 && p.y>398 && p.y<455 && buttonEnabled){
      buttonEnabled = false;
      if(currentstation>0){
        currentstation--;  
      }
      Txcommand = currentstation;
   }

    if(p.x>245 && p.x<300 && p.y>40 && p.y<140 && buttonEnabled){
      buttonEnabled = false;
      state = "MUTED";
      Txcommand = 3;
   }
   if(p.x>245 && p.x<300 && p.y>186 && p.y<330 && buttonEnabled){
      buttonEnabled = false;
      state = "UNMUTED";
      Txcommand = 4;
   }
   
  }else if (p.z < ts.pressureThreshhold) {
    buttonEnabled = true;
  }

    
    
    String f = String(stations[currentstation]);
    tft.fillRect(205,75, 40, 40, WHITE);
    tft.drawRect(205,75,40,40,BLACK);
    tft.setCursor(215,85);
    tft.setTextColor(BLACK);
    tft.setTextSize(1);
    tft.print(f);

  tft.fillRect(70,125, 100, 40, WHITE);
  tft.drawRect(70,125,100,40,BLACK);
  tft.setCursor(78,135);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print(state);
    
  
  
  
}
