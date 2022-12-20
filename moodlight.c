#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>


#define DEBUG_MODE_ENABLE   1
#define BT_RENAME_ENABLE    0

// 네오픽셀 led개수 설정
#define NUM_OF_PIXEL 4
#define CONTROL_PIN 9

// 블루투스 모듈 11, 12번핀으로 사용
SoftwareSerial BT_Serial(11, 12);

// 4 LED 네오픽셀 D13번핀 제어
Adafruit_NeoPixel rgbneo = Adafruit_NeoPixel(NUM_OF_PIXEL, CONTROL_PIN, NEO_GRB + NEO_KHZ800);

int pixelsInterval = 10;
unsigned long rainbowPreviousMillis = 0;
unsigned long rainbowblinkPreviousMillis = 0;
unsigned long blinkPreviousMillis = 0;
String RGBString, rawPacketData;
int rainbowCycles = 0;
int rainbowCycleCycles = 0;
int cmdMode,sig_setColorNeoPixel,sig_setblink, sig_setrainbowblink, sig_setrainbow, redColor, greenColor, blueColor;

void setup() {
  BT_Serial.begin(9600);
  
  #if(DEBUG_MODE_ENABLE)
    Serial.begin(9600);
    Serial.println("Setup initialized");
  #endif
  
  rgbneo.begin();
  rgbneo.show();
}

void loop(){
  myApplication_opp();
}

void myApplication_opp(void){
    if(BT_Serial.available()) {
          RGBString = BT_Serial.readStringUntil('\n');
          rawPacketData = RGBString;
          #if(DEBUG_MODE_ENABLE)
            Serial.println(RGBString);
          #endif
    
          if((RGBString == "OK+LOST" || RGBString == "OK+CONN")) RGBString = "0200000000003";
    
          mypacketparsehandler(RGBString);
    
      switch(cmdMode){
        case 0: break;
        case 2: sig_setColorNeoPixel = 1; sig_setblink = 0; sig_setrainbowblink = 0; sig_setrainbow = 0; break; 
        case 3: sig_setColorNeoPixel = 0; sig_setblink = 1; sig_setrainbowblink = 0; sig_setrainbow = 0; break; 
        case 4: sig_setColorNeoPixel = 0; sig_setblink = 0; sig_setrainbowblink = 1; sig_setrainbow = 0; break; 
        case 6: sig_setColorNeoPixel = 0; sig_setblink = 0; sig_setrainbowblink = 0; sig_setrainbow = 1; break; 
        default :
        break;
        }
     }    
    
    if(sig_setColorNeoPixel){setcontrolneopixel(NUM_OF_PIXEL);}

    if(sig_setblink){if((unsigned long)millis()- blinkPreviousMillis >= pixelsInterval){
        blinkPreviousMillis = millis();
        
        DiscoMode(NUM_OF_PIXEL);
      }
    }
    
    if(sig_setrainbowblink){
      if((unsigned long)millis()- rainbowblinkPreviousMillis >= pixelsInterval){
        rainbowblinkPreviousMillis = millis();
        rainbowblinkEffect();
      }
    }
    if(sig_setrainbow){
      if((unsigned long)millis()-rainbowPreviousMillis >= pixelsInterval){
        rainbowPreviousMillis = millis();
        rainbowEffect();
      }
    }
  }

  void mypacketparsehandler(String rawPacketData){
    cmdMode = rawPacketData.substring(0,2).toInt();
    redColor = rawPacketData.substring(2,5).toInt();
    greenColor = rawPacketData.substring(5,8).toInt();
    blueColor = rawPacketData.substring(8,11).toInt();
    #if(DEBUG_MODE_ENABLE)
       Serial.print("cmdMode: ");      Serial.println(cmdMode);
       Serial.print("Red Value: ");    Serial.println(redColor);
       Serial.print("Green Value: ");  Serial.println(greenColor);
       Serial.print("Blue Value: ");   Serial.println(blueColor);
     #endif
  }
  
  void setcontrolneopixel(int num_of_pixel){
    for(int i = 0; i < num_of_pixel; i++){
      rgbneo.setPixelColor(i,redColor,greenColor,blueColor);
    }
    rgbneo.show();
  }
  void DiscoMode(int num_of_pixel){
    for(int i = 0; i < num_of_pixel; i++){
      int x= random(0, 255);
      int y = random(0, 255);
      int z = random(0, 255);
      rgbneo.setPixelColor(i, x, y, z);
    }
    rgbneo.show();

  }
  
  void rainbowblinkEffect(){
    for(int i = 0; i < rgbneo.numPixels(); i++){
      rgbneo.setPixelColor(i,Wheel((i+rainbowCycles)&255));
    }
    rgbneo.show();
    rainbowCycles++;
    if(rainbowCycles >= 256) rainbowCycles = 0;
  }
  
  void rainbowEffect(){
    uint16_t i;
    for(int i = 0; i < rgbneo.numPixels(); i++){
      rgbneo.setPixelColor(i,Wheel(((i * 256 / rgbneo.numPixels()) + rainbowCycleCycles) & 255));
    }
    rgbneo.show();
    rainbowCycleCycles++;
    if(rainbowCycleCycles >= 256*5) rainbowCycleCycles = 0;
 }

 uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return rgbneo.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return rgbneo.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return rgbneo.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
