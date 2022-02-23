#include <Arduino.h>
#include <FastLED.h>


//LED Stuff
#define NUM_LEDS 15 // Number of LEDs on 1 stripe
#define DATA_PINR 22 // Data Pin of right LEDs
#define DATA_PINL 21 // Data pin of left LEDs

CRGB ledsAll[NUM_LEDS*2]; // Array of all LEDs
int orderMap[NUM_LEDS*2]; //Right order of the LEDs on the two stripes (e.G: 5,4,3,2,1,0,0,1,2,3,4,5)
int maxCurrent = 0;   // Idx of focus LED


//Mat Stuff
const int veloLeft = 18;
const int veloRight = 17;
//bool inDerLuft = false;
//bool isUpVerlagerung = false;


//int upCounter = 5001;
//bool idle = true;

int stateIdx=0; //Represent diffrent states of the mat:
// 0= Idle/Pause; 1= Shift weight; 2= direction logged; 3= jump; 4= ball in the air; 5= level done; 6= GameOver/Time over; 7= win
int inByte; // pass right byte from game to mat


//Idle Animation
float idleAnimationCnt = 0;
int dir = 1; //Helper of fadeAnimation in Idle State

//int counter=0;

//Bounce Animation
int bounceCnt=1;
int bouncedir=1;

// youWin Animation
int youWinCnt=0;
int delayCntWin=0;
int speedYouWin=5;
int youWinCntLeft=NUM_LEDS-1;
int youWinCntRight=NUM_LEDS;
int dirYouWin=1;

// Game/Time over Animation
int gameOverCnt=0;
int delayCntOver=0;
int speedGameOver=10;
int stateAnimationGameOver=0;

//TimerAnimation
int timerByte;
float timerCnt;
float oldTime;
float deltaTime;
float timerMax=10000;

//JumpAmimation
int jumpCnt=0;


//Filter 
float diffNow;
float diffOld;
float filter;


void setup() {
  FastLED.addLeds<WS2812B, DATA_PINL, GRB>(ledsAll,0, NUM_LEDS);  // Adds stripe to ledsAll at idx=0 with NUM_LEDS(15) 
  FastLED.addLeds<WS2812B, DATA_PINR, GRB>(ledsAll,NUM_LEDS, NUM_LEDS);  // Adds stripe to ledsAll at idx=15 mit NUM_LEDS(15) 

  Serial.begin(9600);
  // Set Pinmode of mat
  pinMode(veloLeft, INPUT);
  pinMode(veloRight, INPUT);

  // Setup of ordermap
  for(int i=0; i<NUM_LEDS*2; i++)
  {
    orderMap[i]=i;
    if(i<NUM_LEDS)
    {
      orderMap[i]=(NUM_LEDS)-1-i;
    }
  }

  timerCnt=timerMax;

}

//Change all LEDs at once
void makeAllLEDs(int r, int g, int b)
{
  for( int i=0; i<NUM_LEDS*2; i++)
        {
          ledsAll[i]= CRGB(r,g,b); 
        }
}

//Press the right keys for game controll, translate data to keys
void verlagerung(float veloLeftState, float veloRightState)
{

  if((veloLeftState<80 || veloRightState<80)) // if someone stands on the mat
  {
   
    float diff = veloLeftState - veloRightState;
    Keyboard.release(KEY_V);
    //Shift Left
    if (diff < -180)
    {
      //Serial.println("-4");
      Keyboard.press(KEY_S);
      Keyboard.release(KEY_D);
      Keyboard.release(KEY_F);
      Keyboard.release(KEY_G);
      Keyboard.release(KEY_H);
      Keyboard.release(KEY_J);
      Keyboard.release(KEY_K);
      Keyboard.release(KEY_L);
      Keyboard.release(KEY_T);
    }
    else if (diff < -100)
    {
      //Serial.println("-3");
      Keyboard.release(KEY_S);
      Keyboard.press(KEY_D);
      Keyboard.release(KEY_F);
      Keyboard.release(KEY_G);
      Keyboard.release(KEY_H);
      Keyboard.release(KEY_J);
      Keyboard.release(KEY_K);
      Keyboard.release(KEY_L);
      Keyboard.release(KEY_T);
    }
    else if (diff < -40)
    {
      //Serial.println("-2");
      Keyboard.release(KEY_S);
      Keyboard.release(KEY_D);
      Keyboard.press(KEY_F);
      Keyboard.release(KEY_G);
      Keyboard.release(KEY_H);
      Keyboard.release(KEY_J);
      Keyboard.release(KEY_K);
      Keyboard.release(KEY_L);
      Keyboard.release(KEY_T);
    }
    else if (diff < -20)
    {
      //Serial.println("-1");
      Keyboard.release(KEY_S);
      Keyboard.release(KEY_D);
      Keyboard.release(KEY_F);
      Keyboard.press(KEY_G);
      Keyboard.release(KEY_H);
      Keyboard.release(KEY_J);
      Keyboard.release(KEY_K);
      Keyboard.release(KEY_L);
      Keyboard.release(KEY_T);
    }
    else if (diff < 20)
    {
      //Serial.println("0");
      Keyboard.release(KEY_S);
      Keyboard.release(KEY_D);
      Keyboard.release(KEY_F);
      Keyboard.release(KEY_G);
      Keyboard.release(KEY_H);
      Keyboard.release(KEY_J);
      Keyboard.release(KEY_K);
      Keyboard.release(KEY_L);
      Keyboard.press(KEY_T);
    }
    else if (diff < 40)
    {
      //Serial.println("1");
      Keyboard.release(KEY_S);
      Keyboard.release(KEY_D);
      Keyboard.release(KEY_F);
      Keyboard.release(KEY_G);
      Keyboard.press(KEY_H);
      Keyboard.release(KEY_J);
      Keyboard.release(KEY_K);
      Keyboard.release(KEY_L);
      Keyboard.release(KEY_T);
    }
    else if (diff < 100)
    {
      //Serial.println("2");
      Keyboard.release(KEY_S);
      Keyboard.release(KEY_D);
      Keyboard.release(KEY_F);
      Keyboard.release(KEY_G);
      Keyboard.release(KEY_H);
      Keyboard.press(KEY_J);
      Keyboard.release(KEY_K);
      Keyboard.release(KEY_L);
      Keyboard.release(KEY_T);
    }
    else if (diff < 180)
    {
      //Serial.println("3");
      Keyboard.release(KEY_S);
      Keyboard.release(KEY_D);
      Keyboard.release(KEY_F);
      Keyboard.release(KEY_G);
      Keyboard.release(KEY_H);
      Keyboard.release(KEY_J);
      Keyboard.press(KEY_K);
      Keyboard.release(KEY_L);
      Keyboard.release(KEY_T);
    }
    else if (diff >= 180)
    {
      //Serial.println("4");
      Keyboard.release(KEY_S);
      Keyboard.release(KEY_D);
      Keyboard.release(KEY_F);
      Keyboard.release(KEY_G);
      Keyboard.release(KEY_H);
      Keyboard.release(KEY_J);
      Keyboard.release(KEY_K);
      Keyboard.press(KEY_L);
      Keyboard.release(KEY_T);
      
    } 
    
  }
  else //if(!stateIdx==0)  // jumping
    {
      Keyboard.release(KEY_S);
      Keyboard.release(KEY_D);
      Keyboard.release(KEY_F);
      Keyboard.release(KEY_G);
      Keyboard.release(KEY_H);
      Keyboard.release(KEY_J);
      Keyboard.release(KEY_K);
      Keyboard.release(KEY_L);
      Keyboard.release(KEY_T);
      Keyboard.press(KEY_V);

      if(stateIdx==2)
      {
        stateIdx=3;
      }
    }


}


//Check if values of mat changed significantly
bool filterFunc(float left, float right, int impact)
{
  diffNow=abs(right-left);
  filter=abs(abs(diffNow)-diffOld);

  if(filter>impact)
  {
    diffOld=diffNow;
    return true;
  }
  else{
    return false;
  }
   
}
//LEDs for shift weight
void verlaufLEDsL(int left, int right)
{
  int diff = left - right; //Diffrence of left and right mat

  if(filterFunc(left,right, 5))
  {
      if (diff< -100)  //left top limit
      {
        maxCurrent=0;
      }
      else if(diff> 100) //right top limit
      {
        maxCurrent=NUM_LEDS*2-1;
      }
      else  
      {
        maxCurrent = map(diff, -100, 100, 0, NUM_LEDS*2-1);  //Translate mat data to focus LED
      }
  }

  int maxCounter = -maxCurrent ;
  int potenz;

  for (int i = 0; i < NUM_LEDS*2; i++) //calculate brightness (br) of all LEDs for fadeOut
  {
    potenz = 8;
    if (maxCounter > -8 && maxCounter < 8)  
    {
      potenz = abs(maxCounter);
    }
    int br = (pow(2, 8 - potenz))-1 ;  // calculate brightness by 2^(8 to 0) -1 
    
    ledsAll[orderMap[i]] = CRGB(br, br, br); //Set brightness of all LEDs (Ordermap for right order of LEDs)
    maxCounter += 1;
  }

  FastLED.show();
}

//Idle Animation
void simpleFade()
{

  idleAnimationCnt += 0.2f * dir;
  if (idleAnimationCnt > 100 || idleAnimationCnt <= -20) //Limits of FadeCounter
  {
    dir *= -1;
  }

  int br = int(idleAnimationCnt);
  if (br < 0)
  {
    br = 0;
  }
  for (int i = 0; i < NUM_LEDS*2; i++)
  {
    ledsAll[i]= CRGB(int(br * 0.15f), int(br * 0.8f), br);
    
  }
  FastLED.show();
}

/*void youWin()
{
    delayCntWin= (delayCntWin+1)%speedYouWin;
    if(delayCntWin==0)
    {
      youWinCnt= (youWinCnt+dirYouWin)%(NUM_LEDS*2);
      if(youWinCnt==(NUM_LEDS*2)-1 || youWinCnt==0)
      {
        dirYouWin*=-1;
      }
      makeAllLEDs(0,0,0);


      for(int i=0; i<=6; i++)
      {
        if(youWinCnt+dir*i>0 && youWinCnt+dir*i<NUM_LEDS*2)
        {
          ledsAll[orderMap[youWinCnt+dir*i]]= CRGB(int(255*(1/(i+1))), int (120*(1/(i+1))),0);
        }
      }
      
    }
    FastLED.show();
    Serial.println(youWinCnt);
}  */


/*void youWinNew( int r, int g, int b)
{
  delayCntWin= (delayCntWin+1)%speedYouWin;
  if(delayCntWin==0)
  {
    makeAllLEDs(0,0,0);
    //Linke Seite
    //youWinCntLeft= (youWinCntLeft-1)%NUM_LEDS;
    //youWinCntRight= ((youWinCntRight+1)%NUM_LEDS);
    ledsAll[orderMap[youWinCntLeft]]= CRGB(r,g,b);
    if(youWinCntLeft<NUM_LEDS-2){ledsAll[orderMap[youWinCntLeft+1]]= CRGB (int(r*0.5), int (g*0.5), b);}
    if(youWinCntLeft<NUM_LEDS-3){ledsAll[orderMap[youWinCntLeft+2]]= CRGB (int(r*0.1), int (g*0.1), b);}


    ledsAll[orderMap[youWinCntRight]]= CRGB(r,g,b);
    if(youWinCntRight>NUM_LEDS+1){ledsAll[orderMap[youWinCntRight-1]]= CRGB (int(r*0.5), int (g*0.5), b);}
    if(youWinCntRight>NUM_LEDS+2){ledsAll[orderMap[youWinCntRight-2]]= CRGB (int(r*0.1), int (g*0.1), b);}

    youWinCntLeft--;
    if(youWinCntLeft<0){youWinCntLeft=NUM_LEDS-1;}

    youWinCntRight++;
    if (youWinCntRight>= NUM_LEDS*2) { youWinCntRight=NUM_LEDS;}
    
  }
  FastLED.show();
} */

void youWinDouble(int r, int g, int b)  // Two light, one from left to right, othe from right to left
{
  delayCntWin= (delayCntWin+1)%speedYouWin;  // Own delay to wait by runtime
  if(delayCntWin==0)
  {
     FastLED.clear(); // Reset all LEDs
     youWinCntRight=(youWinCntRight+1)%(NUM_LEDS*2);  // Counts up circulary
     youWinCntLeft= ((NUM_LEDS*2)-1)-youWinCntRight; // Counts down circulary
     
    

    ledsAll[orderMap[youWinCntLeft]]= CRGB(r,g,b);  // Set left main LEDs


    // Speical states at limits of LED stripes (left)
    if(youWinCntLeft== (NUM_LEDS*2)-1){
      ledsAll[orderMap[0]]= CRGB(int (r*0.5), int (g*0.5),int (b*0.5));
      ledsAll[orderMap[1]]= CRGB(int (r*0.1), int (g*0.1),int (b*0.1));
    }
    else if(youWinCntLeft== (NUM_LEDS*2)-2){
      ledsAll[orderMap[(NUM_LEDS*2)-1]]= CRGB(int (r*0.5), int (g*0.5),int (b*0.5));
      ledsAll[orderMap[0]]= CRGB(int (r*0.1), int (g*0.1),int (b*0.1));
    }
    else if(youWinCntLeft== (NUM_LEDS*2)-3){
      ledsAll[orderMap[(NUM_LEDS*2)-2]]= CRGB(int (r*0.5), int (g*0.5),int (b*0.5));
      ledsAll[orderMap[(NUM_LEDS*2)-1]]= CRGB(int (r*0.1), int (g*0.1),int (b*0.1));
    }
    //Set left 2nd & 3rd LED 
    else{
      ledsAll[orderMap[youWinCntLeft+1]]= CRGB(int (r*0.5), int (g*0.5),int (b*0.5));
      ledsAll[orderMap[youWinCntLeft+2]]= CRGB(int (r*0.1), int (g*0.1),int (b*0.1));
    }

    
    ledsAll[orderMap[youWinCntRight]]= CRGB(r,b,g); // Set right main LED

    // Speical states at limits of LED stripes (right)
    if(youWinCntRight==1)
    {
      ledsAll[orderMap[0]]= CRGB(int (r*0.5), int (b*0.5),int (g*0.5));
      ledsAll[orderMap[NUM_LEDS*2-1]]= CRGB(int (r*0.1), int (b*0.1),int (g*0.1));
    }
    else if(youWinCntRight==0)
    {
      ledsAll[orderMap[NUM_LEDS*2-1]]= CRGB(int (r*0.5), int (b*0.5),int (g*0.5));
      ledsAll[orderMap[NUM_LEDS*2-2]]= CRGB(int (r*0.1), int (b*0.1),int (g*0.1));
    }
    // Set right 2nd & 3rd LED
    else{
      ledsAll[orderMap[youWinCntRight-1]]= CRGB(int (r*0.5), int (b*0.5),int (g*0.5));
      ledsAll[orderMap[youWinCntRight-2]]= CRGB(int (r*0.1), int (b*0.1),int (g*0.1)); 
    }  
  }
  FastLED.show();

}

void gameOver(int r, int g, int b) //Blink two times, short break, repeat
{
  delayCntOver= (delayCntOver+1)%speedGameOver;   // Own delay to wait by runtime
  if(delayCntOver==0)
  {
    if(stateAnimationGameOver==0 || stateAnimationGameOver==8)  //Blink 
    {
      makeAllLEDs(r,g,b);
    }
    if(stateAnimationGameOver==4 || stateAnimationGameOver== 12) //Stop blink
    {
      FastLED.clear();
    }
    stateAnimationGameOver= (stateAnimationGameOver+1)%24; 

  }

  FastLED.show();
}

void bounce(int r, int g, int b)  // One light goes from left to right
{
  delayCntWin= (delayCntWin+1)%speedYouWin; // Own delay to wait by runtime
  if(delayCntWin==0)
  {
    FastLED.clear();
    float factor=1;
    if(bounceCnt<=0 || bounceCnt>=(NUM_LEDS*2)-1)
    {
      bouncedir*=-1;
    }
    bounceCnt+= bouncedir;
 


    for(int i=0; i<8; i++)
    {
      factor=factor/2;
      int helper= bounceCnt- (bouncedir*i);
      if(helper>= 0 && helper<NUM_LEDS*2)
      {
        
        ledsAll[orderMap[helper]]= CRGB(int(r*factor), int(g*factor), int(b*factor));
        //ledsAll[orderMap[helper]]= CRGB(int(r*(1/(i+1))), int(g*(1/(i+1))),int(b*(1/(i+1))));
      }
    } 

  } 
  FastLED.show();
}


void timerAnimation(int r, int g, int b)
{
  /*if(Serial.available()>0 && stateIdx==8)
  {
    timerByte=Serial.read();
    int helper= 65-timerByte;
    makeAllLEDs(0,0,0);
    for(int i=0; i<=helper; i++)
    {
      ledsAll[NUM_LEDS-1-i]= CRGB(255,0,0);
      ledsAll[NUM_LEDS+i]= CRGB(255,0,0);
    }
  }*/
  timerCnt-= deltaTime;
  if(timerCnt<=0)
  {timerCnt=0;};
  int helper = map(timerCnt, 0, timerMax, 0,NUM_LEDS);
  FastLED.clear();
  for(int i=0; i<=helper; i++)
  {
    ledsAll[orderMap[NUM_LEDS-1-i]]= CRGB(r, g, b);
    ledsAll[orderMap[NUM_LEDS+i]]= CRGB(r, g, b);
  }
  FastLED.show();
 
}

void jumpAnimation(int r, int g, int b)
{
  
  FastLED.clear();
  for(int i=0; i<=jumpCnt; i++)
  {
    if(NUM_LEDS-1-i>=0 && NUM_LEDS+1 <NUM_LEDS*2)
    {
    ledsAll[orderMap[NUM_LEDS-1-i] ]= CRGB(r,g,b);
    ledsAll[orderMap[NUM_LEDS+i] ]= CRGB(r,g,b);
    }
  }
  if(jumpCnt<=NUM_LEDS-1)
  {
    jumpCnt++;
  }
  FastLED.show();
}


void timerFunc()
{
  deltaTime= millis()-oldTime;
  oldTime=millis();

}

void makeRightState(int left, int right)   // Translate game state to right LED animation
{
    inByte= Serial.read();

    if(stateIdx==0) //Idle/Pause
    { 
        simpleFade();
    }
    else if(stateIdx==1) //Verlagerung
    { 
        verlaufLEDsL(left, right);
    }
    else if(stateIdx==2) //Richtung fest
    { 
        for( int i=0; i<NUM_LEDS*2; i++)
        {
          ledsAll[i]= CRGB(100,100,100); //weiß
        }
        FastLED.show();
    }
    else if(stateIdx==3) //Springen
    { 
        /*for (int i = 0; i < NUM_LEDS*2; i++)
        {
          ledsAll[i] = CRGB(255, 15, 20); //pink
        }
        FastLED.show(); */ 

        jumpAnimation(255,15,20);
    }
    else if(stateIdx==4) // Ball in der Luft
    { ;
        for (int i = 0; i < NUM_LEDS*2; i++)
        {
          ledsAll[i] = CRGB(10, 10, 10); //leicht hell
        }
        FastLED.show();
    }
    else if(stateIdx==5) // Level geschafft
    { 
   
       bounce(255,120,0);
    }
    else if(stateIdx==6) // GameOver/Time Over
    { 
      gameOver(190,4,4);
    }
    else if(stateIdx==7) // Gewonnen
    { 
        youWinDouble(255,120,0);
    }
    else if(stateIdx==8)
    {
      timerAnimation(190,4,4);
    }

    if(!(stateIdx==8))
    {
      timerCnt=timerMax;
    } 
    if(!(stateIdx==3))
    {
      jumpCnt=0;
    } 
     

}




void loop() {
  // put your main code here, to run repeatedly:
  timerFunc();
  delay(10); //Global delay //10

  float veloLeftState = analogRead(veloLeft);
  float veloRightState = analogRead(veloRight);
   Serial.println(veloLeftState);
   Serial.println(veloRightState);
   Serial.println("---------------");

  //Suit gameState to matState (ASCII)
  if(Serial.available()>0)
  {
    inByte=Serial.read();
    if(inByte==48) {stateIdx=0;}
    else if(inByte==49) {stateIdx=1;}
    else if(inByte==50) {stateIdx=2;}
    else if(inByte==51) {stateIdx=3;}
    else if(inByte==52) {stateIdx=4;}
    else if(inByte==53) {stateIdx=5;}
    else if(inByte==54) {stateIdx=6;}
    else if(inByte==55) {stateIdx=7;}
    else if(inByte==56) {stateIdx=8;}

  }

  

  makeRightState(int(veloLeftState), int(veloRightState));
  verlagerung(veloLeftState, veloRightState);
}