#include <Adafruit_NeoPixel.h>

#define UP true
#define DOWN false
#define PIN 6
#define NUMLEDS 144

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMLEDS, PIN, NEO_GRB + NEO_KHZ800);

//pressure sensor
int analogButton = A0;
int analogButtonRead;
bool presureState = LOW;
bool previousPresureState = LOW;

long scoreLed = 0;

float totalScore = 0;           //keeps score of all jumps together
long jumpScore = 0;            //keeps score of 1 jump
unsigned long jumpTime = 0;             //times how long user is in the air, in mili-sec
unsigned long previousJumpTime = 0;
int jumpMaxTime = 2000;                 //sets the max time user can be in the air, in mili-sec
float maxJumpTimes = 0;
float jumpMaxScore = 0;                 //calculates the max score you can get in one jump
unsigned long totalScoreRecal = 0;      //totalScore recalculated on a scale from 0 to 1000
unsigned long jumpScoreRecal = 0;
float maxScore = 0;

bool session = false;                   //is true when user has start jumping, is false when sessionTime is reached and last jump is made
unsigned long previousSessionCounter = 0;
const long sessionTime = 30000;         //sets the session time in mili-secs
int waitTime = 500;                     //sets the time after session to wait till next session can begin

void setup() {
  Serial.begin(9600);
  strip.setBrightness(64);
  strip.begin();
  strip.show();

  jumpMaxScore = pow(jumpMaxTime,2);
  maxJumpTimes = sessionTime/jumpMaxTime;
  maxScore = pow(jumpMaxTime,2) * maxJumpTimes;
}

void loop() {
  unsigned long currentCounter = millis();

  //waits for the user to make the first jump
  if (session == false){
    analogButtonRead = analogRead(analogButton);

    if (analogButtonRead < 200){presureState = LOW;}
    else {presureState = HIGH;}
  
    if (presureState == HIGH && previousPresureState == LOW){
      jumpHitLeds(strip.Color(0, 0, 255), 0);
      previousSessionCounter = currentCounter;
      session = true;
      previousJumpTime = currentCounter;
    }
    else {
      session = false;
    }
    
  idle(10);

  }

  //start of the session
  else{ //if session == true
    analogButtonRead = analogRead(analogButton);
    
    if (analogButtonRead < 200){presureState = LOW;}
    else {presureState = HIGH;}
  
    if (presureState == HIGH && previousPresureState == LOW || currentCounter - previousSessionCounter >= sessionTime + 3000){
      
      jumpTime = currentCounter - previousJumpTime;
      if (jumpTime > jumpMaxTime){
        jumpTime = jumpMaxTime;
      }
      previousJumpTime = currentCounter;
      
      jumpScore = pow(jumpTime,2);
      jumpScoreRecal = (jumpScore/jumpMaxScore)*(1000/maxJumpTimes);
      totalScore = totalScore + jumpScore;
      totalScoreRecal = (totalScore/maxScore)*1000;

      scoreLed = (totalScore/maxScore)*NUMLEDS;
      
      Serial.print("Jump score: ");
      Serial.print(jumpScoreRecal);
      Serial.print(" total score: ");
      Serial.println(totalScoreRecal);

      jumpHitLeds(strip.Color(0, 0, 255), 0);
      
      if(currentCounter - previousSessionCounter >= sessionTime){
        restartGame();
      }
      else{
        session = true;
      }
    }
    previousPresureState = presureState;
  }
  previousPresureState = presureState;
}

void showScore(){
  if(totalScoreRecal < 250){
        for(uint16_t x=0; x<scoreLed; x++){
          strip.setPixelColor(x,255,0,0);
        }
      }
      else if(totalScoreRecal < 500){
        for(uint16_t x=0; x<scoreLed; x++){
          strip.setPixelColor(x,204,83,20);
        }
      }
      else if(totalScoreRecal < 750){
        for(uint16_t x=0; x<scoreLed; x++){
          strip.setPixelColor(x,59,255,0);
        }
      }
      else {
        for(uint16_t x=0; x<scoreLed; x++){
          strip.setPixelColor(x,55,178,18);
        }  
  }
}

void jumpHitLeds(uint32_t c, uint8_t wait){
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      showScore();
      strip.setPixelColor(i+1, c);
      strip.setPixelColor(i, c);
      strip.setPixelColor(i-1, 0);
      strip.show();
  }
  strip.setPixelColor(strip.numPixels()-1,0);
  strip.show();
}

void ledsOff(){
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
}

void restartGame(){
  Serial.print("Je eind score = ");
  Serial.println(totalScoreRecal);
  jumpHitLeds(strip.Color(0, 0, 255), 0);

  for(int i=0; i<7; i++){
    ledsOff();
    strip.show();
    delay(waitTime);
    showScore();
    strip.show();
    delay(waitTime);
  }
  
  jumpScore = 0;
  totalScore = 0;
  session = false;
}

void idle(uint8_t wait) {
//  uint16_t i, j;
//
//  for(j=0; j<256; j++) {
//    for(i=0; i<strip.numPixels(); i++) {
//      strip.setPixelColor(i, Wheel((i+j) & 255));
//    }
//    strip.show();
//    delay(wait);
//  }
}

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

