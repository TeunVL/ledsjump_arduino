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

unsigned long totalScore = 0;           //keeps score of all jumps together
unsigned long jumpScore = 0;            //keeps score of 1 jump
unsigned long jumpTime = 0;          //times how long user is in the air, in mili-sec
unsigned long previousjumpTime = 0;
int jumpMax = 2000;                     //sets the max time user can be in the air, in mili-sec

int ledWaveSpeed = 50;                  //speed of shockwave leds, high is slow, low is fast
bool session = false;                   //is true when user has start jumping, is false when sessionTime is reached and last jump is made
unsigned long previousSessionCounter = 0;
const long sessionTime = 30000;          //sets the session time in mili-secs
int waitTime = 5000;                    //sets the time after session to wait till next session can begin

void setup() {
  Serial.begin(9600);
  for (int i=0; i<4; i++){
    pinMode(ledsPin[i],OUTPUT);
  }

  ledsOff();

  int maxJumpTimes = sessionTime/jumpMax;
  long maxScore = pow(jumpMax,2) * maxJumpTimes;
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
//    Serial.println(ledsOneScore);
    
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
      scoreLeds();
    }
  }

  //start of the session
  else{ //if session == true
    analogButtonRead = analogRead(analogButton);
//    Serial.println(jumpTime);
    
    if (analogButtonRead < 200){presureState = LOW;}
    else {presureState = HIGH;}
  
    if (presureState == HIGH && previousPresureState == LOW){
      
      jumpTime = currentCounter - previousjumpTime;
      if (jumpTime > jumpMax){
        jumpTime = jumpMax;
      }
      previousjumpTime = currentCounter;
      Serial.print("Jump time: ");
      Serial.print(jumpTime);
      Serial.print(" total score: ");
      Serial.println(totalScore);
      jumpScore = pow(jumpTime,2);
      totalScore = totalScore + jumpScore;

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
//  digitalWrite(ledsPin[0],LOW);
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
  Serial.print("Je score = ");
  Serial.println(totalScore);
  jumpScore = 0;
  totalScore = 0;
  delay(waitTime);
  session = false;

}


