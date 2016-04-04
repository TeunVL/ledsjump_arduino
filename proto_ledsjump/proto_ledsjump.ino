#define UP true
#define DOWN false


//pressure sensor
int analogButton = A0;
int analogButtonRead;
bool presureState = LOW;
bool previousPresureState = LOW;

float totalScore = 0;           //keeps score of all jumps together
long jumpScore = 0;            //keeps score of 1 jump
unsigned long jumpTime = 0;             //times how long user is in the air, in mili-sec
unsigned long previousjumpTime = 0;
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
  ledsOff();

  //specifies when the leds react at the score of the player
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
      jumpHitLeds();
      previousSessionCounter = currentCounter;
      session = true;
      previousjumpTime = currentCounter;
    }
    else {
      session = false;
    }

    if (currentCounter - previousIdleCounter >= ledIdleSpeedPrimary) {
      previousIdleCounter = currentCounter;
      idle();
    }

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

}

void ledsOff(){

}

void restartGame(){
  Serial.print("Je eind score = ");
  Serial.println(totalScoreRecal);

  jumpScore = 0;
  totalScore = 0;
  session = false;

}

void idle(){

}

