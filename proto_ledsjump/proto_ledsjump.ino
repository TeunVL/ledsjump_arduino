#define UP true
#define DOWN false

int ledsPin[] = {2,3,4,5};

long ledsOneScore = 0;
long ledsTwoScore = 0;
long ledsThreeScore = 0;
long ledsFourScore = 0;

//pressure sensor
int analogButton = A0;
int analogButtonRead;
bool presureState = LOW;
bool previousPresureState = LOW;

float totalScore = 0;           //keeps score of all jumps together
long jumpScore = 0;            //keeps score of 1 jump
long jumpTime = 0;             //times how long user is in the air, in mili-sec
long previousjumpTime = 0;
int jumpMaxTime = 2000;                 //sets the max time user can be in the air, in mili-sec
float maxJumpTimes = 0;
float jumpMaxScore = 0;                 //calculates the max score you can get in one jump
long totalScoreRecal = 0;      //totalScore recalculated on a scale from 0 to 1000
long jumpScoreRecal = 0;
float maxScore = 0;

int ledWaveSpeed = 50;                  //speed of shockwave leds, high is slow, low is fast
bool session = false;                   //is true when user has start jumping, is false when sessionTime is reached and last jump is made
unsigned long previousSessionCounter = 0;
const long sessionTime = 30000;         //sets the session time in mili-secs
int waitTime = 500;                     //sets the time after session to wait till next session can begin

int ledIdleSpeedSet = 1;
int ledIdleSpeedPrimary = 1000;                 //first speed of leds switching when session is false
int ledIdleSpeedSecondary = 100;                  //secondary speed of leds switching when session is false 
bool ledDirection = UP;                  //defines if leds or moving up or down when idle
int ledState = 0x01;                     //specifies which led turns on and of
unsigned long previousIdleCounter = 0;

void setup() {
  Serial.begin(9600);
  for (int i=0; i<4; i++){
    pinMode(ledsPin[i],OUTPUT);
  }

  ledsOff();

  //specifies when the leds react at the score of the player
  jumpMaxScore = pow(jumpMaxTime,2);
  maxJumpTimes = sessionTime/jumpMaxTime;
  maxScore = pow(jumpMaxTime,2) * maxJumpTimes;
  long eightMaxScore = maxScore/8;
  ledsOneScore = 1 * eightMaxScore;
  ledsTwoScore = 3 * eightMaxScore;
  ledsThreeScore = 5 * eightMaxScore;
  ledsFourScore = 7 * eightMaxScore;
}

void loop() {
  unsigned long currentCounter = millis();

  //waits for the user to make the first jump
  if (session == false){
    analogButtonRead = analogRead(analogButton);
    
    if (analogButtonRead < 200){presureState = LOW;}
    else {presureState = HIGH;}
  
    if (presureState == HIGH && previousPresureState == LOW){
      jumpHitLeds();
      previousSessionCounter = currentCounter;
      session = true;
      previousjumpTime = currentCounter;
    }
    else {
      session = false;
    }

    
//    if (ledIdleSpeedSet == 1){
//      for (int i=0; i <3; i++){
        if (currentCounter - previousIdleCounter >= ledIdleSpeedPrimary) {
          previousIdleCounter = currentCounter;
          idle();
        }
//      }
//      ledIdleSpeedSet = 2;
//    }
//    if (ledIdleSpeedSet == 2){
//      for (int i=0; i <1; i++){
//        if (currentCounter - previousIdleCounter >= ledIdleSpeedSecondary) {
//          previousIdleCounter = currentCounter;
//          idle();
//        }
//      }
//      ledIdleSpeedSet = 1;
//    }
  }

  //start of the session
  else{ //if session == true
    analogButtonRead = analogRead(analogButton);
    
    if (analogButtonRead < 200){presureState = LOW;}
    else {presureState = HIGH;}
  
    if (presureState == HIGH && previousPresureState == LOW || currentCounter - previousSessionCounter >= sessionTime + 3000){
      
      jumpTime = currentCounter - previousjumpTime;
      if (jumpTime > jumpMaxTime){
        jumpTime = jumpMaxTime;
      }
      previousjumpTime = currentCounter;
      
      jumpScore = pow(jumpTime,2);
      jumpScoreRecal = (jumpScore/jumpMaxScore)*(1000/maxJumpTimes);
      totalScore = totalScore + jumpScore;
      totalScoreRecal = (totalScore/maxScore)*1000;
      Serial.print("Jump score: ");
      Serial.print(jumpScoreRecal);
      Serial.print(" total score: ");
      Serial.println(totalScoreRecal);

      jumpHitLeds();
      scoreLeds();
      
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

void jumpHitLeds(){
  ledsOff();
  for (int i=0; i<5; i++){
    digitalWrite(ledsPin[i],HIGH);
    delay(ledWaveSpeed);
    digitalWrite(ledsPin[i],LOW);
  }
}

void scoreLeds(){
  if (totalScore > ledsFourScore){
    digitalWrite(ledsPin[0], HIGH);
    digitalWrite(ledsPin[1], HIGH);
    digitalWrite(ledsPin[2], HIGH);
    digitalWrite(ledsPin[3], HIGH);
  }
  else if (totalScore > ledsThreeScore){
    digitalWrite(ledsPin[0], HIGH);
    digitalWrite(ledsPin[1], HIGH);
    digitalWrite(ledsPin[2], HIGH);
    digitalWrite(ledsPin[3], LOW);
  }
  else if (totalScore > ledsTwoScore){
    digitalWrite(ledsPin[0], HIGH);
    digitalWrite(ledsPin[1], HIGH);
    digitalWrite(ledsPin[2], LOW);
    digitalWrite(ledsPin[3], LOW);
  }
  else if (totalScore > ledsOneScore){
    digitalWrite(ledsPin[0], HIGH);
    digitalWrite(ledsPin[1], LOW);
    digitalWrite(ledsPin[2], LOW);
    digitalWrite(ledsPin[3], LOW);
  }
  else {
    ledsOff();
  }
}

void ledsOff(){
  for (int i=0; i<5; i++){
    digitalWrite(ledsPin[i], LOW);
  }
}

void restartGame(){
  Serial.print("Je eind score = ");
  Serial.println(totalScoreRecal);
  for (int i=0; i<7; i++){
    scoreLeds();
    delay(waitTime);
    ledsOff();
    delay(waitTime);
  }
  jumpScore = 0;
  totalScore = 0;
  session = false;

}

void idle(){
  // Set the pins of each LED during each iteration
  // You'll only see something change when "LEDpins" gets updated
  for (int x=0; x < 4; x++)
    digitalWrite(ledsPin[x], bitRead(ledState,x));
    
    if (ledDirection==UP) {
      // Use "<<" to "bit-shift" everything to the left once
      ledState = ledState << 1;
      // 0x20 is the "last" LED, another shift makes the value 0x40
      if (ledState == 0x08) {
        // turn on the one before "0x20" and reverse direction
        ledState = 0x08;
        ledDirection = DOWN;
      }
    }
    else {
      // use ">>" to "bit-shift" all bits in LEDstate once to the right
      ledState = ledState >> 1;
      // This means we ran out of bits!
      if (ledState == 0x00) {
        // set one ahead so no extra delay
        ledState = 0x02;
        ledDirection = UP;
      }
    }
}

