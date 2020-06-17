//This program plays the game Peppa Jump using an ultrasonic sensor and LCD
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // RS, E, D4, D5, D6, D7

const int trigPin = 10; //trigger ultrasonic sound pulses
const int echoPin = 9; //receive reflected signal
long duration;
int distance;
int n = 0;
int timer = 1000;
int counter = 0;
int w;
int starting_millis, current_millis;
int count[] = {15, random(17,19), random(21, 22),random(24,25),random(27,28), 30}; //using 6 obstacles
int obstacleBytes[6];
int obstacleHeights[6];
int x[6];
int y = 0;
int buffers = 0;

//initializes the byte values for the custom LCD characters
byte forward[] = 
{
  B10100,
  B11111,
  B11111,
  B11100,
  B01110,
  B11111,
  B01010,
  B00101
};

byte backwards[] = 
{
  B10100,
  B11111,
  B11111,
  B11100,
  B01110,
  B11111,
  B01010,
  B10100
};

byte jump[] = 
{
  B10100,
  B11111,
  B11111,
  B11100,
  B01110,
  B11111,
  B01010,
  B10001
};

byte pig[] = 
{
  B00101,
  B11111,
  B11111,
  B00111,
  B01110,
  B11111,
  B01010,
  B10001
};

byte scooter[] = 
{
  B00000,
  B00000,
  B11100,
  B01000,
  B01000,
  B01000,
  B11111,
  B11011
};

byte mudcloud[] = 
{
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111
};

byte rabbit[] = 
{
  B00101,
  B00110,
  B01100,
  B11100,
  B01100,
  B11100,
  B01100,
  B10100
};

byte bird[] = 
{
  B00100,
  B11100,
  B01100,
  B01101,
  B01110,
  B01101,
  B10100,
  B00000
};


void setup() 
{
  // put your setup code here, to run once:
  lcd.begin(16, 2); 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  lcd.createChar(0, forward);
  lcd.createChar(1, backwards);
  lcd.createChar(2, jump);
  lcd.createChar(3, pig);
  lcd.createChar(4, scooter);
  lcd.createChar(5, rabbit);
  lcd.createChar(6, mudcloud);
  lcd.createChar(7, bird);
}


void loop() 
{
  //starting sequence set to run only at the beginning of the game
  if (n == 0) 
  {
    starting_sequence();
    n = 1;
    starting_millis = millis();    
  }
  
  current_millis = millis();//used as a clock timer
  
  if (current_millis - starting_millis >= timer/2) //loops every period of timer/2
  {
    w = peppa_actions(counter);
    if (counter == 0) //loops every period of timer
    {
      for (int i = 0; i <= 5; i++) //repeats for every obstacle 
      {
        if (count[i] == 15) {
          obstacleBytes[i] = random(3,8);
          obstacleHeights[i] = random(2);
        }
        x[i] = move_obstacles(count[i], obstacleBytes[i], obstacleHeights[i],y);
        if (x[i] == 1 || x[i] == 2) {
          y++;//logs score
        }
        if (x[i] == w) //if obstacle and pig are in the same LCD cell, ends and restarts the game
        {
          end_game(y);
          count[0] = 16;
          count[1] = random(18,19);
          count[2] = random(21,22);
          count[3] = random(24,25);
          count[4] = random(27,29);
          count[5] = 31;
          n = 0;
          timer = 1000;
          y = 0;
        }
        count[i]--;
        if (count[i] < -1) {
          count[i] = 16;//set so that the obstacle resets itself after hitting 0
        }
      }
      counter = 1;
      if (timer >= 260 && buffers >= 20) 
      {
        timer = timer - 10; //game moves faster as time goes on after 20 loops
      }
      buffers++; 
    }
    else{
      counter = 0;
    }
    starting_millis = current_millis;
  }
}  

 

void peppa_move(int t) {
  lcd.setCursor(0,0);
  lcd.print(" ");
  if (t == 0) {
   lcd.setCursor(0,1);
   lcd.write(byte(0));
  }
  else{
   lcd.setCursor(0,1);
   lcd.write(byte(1));
  }
}

void peppa_jump() {
  lcd.setCursor(0,1);
  lcd.print(" ");
  lcd.setCursor(0,0);
  lcd.write(byte(2));
}

void starting_sequence() 
{
  
  String start_sequence[] = {"PEPPA JUMP!", "READY??", "3!", "2!", "1!"};
  for (int i = 0; i <= 5; i++) 
  {
    if(i%3 == 0) //script changes every 3 seconds
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(start_sequence[i/3]);
    }
  //Pig moving back and forth
  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  delay(500);
  lcd.setCursor(0, 1);
  lcd.write(byte(1));
  delay(500);
  }

  for (int i = 0; i <= 2; i++) //loops every second
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(start_sequence[i+2]);
    
    lcd.setCursor(0, 1);
    lcd.write(byte(0));
    delay(500);
    lcd.setCursor(0, 1);
    lcd.write(byte(1));
    delay(500);
  }
  lcd.clear();
}

int peppa_actions(int t) //jumps if sensor is triggered, walks if not
{
  //distance sensing 
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;
  if (distance < 15)
  {
    peppa_jump();
    return 1;
  }
  else
  {
    peppa_move(t);
    return 2;
  }
}

int move_obstacles (int m, int obstacleByte, int obstacleHeight,int y)//moves obstacles down the LCD screen for Peppa to dodge 
{
  if (m >= 0 && m <= 15) 
  {
    lcd.setCursor(m, obstacleHeight);
    lcd.write(byte(obstacleByte));
    lcd.setCursor(m + 1, obstacleHeight);
    lcd.write("  ");
  }
  if (y < 10) {
    lcd.setCursor(15,0);
    lcd.print(y);
  }
  if (y >= 10 && y < 100) {
    lcd.setCursor(14,0);
    lcd.print(y);
  }
  if (y >= 100) {
    lcd.setCursor(13,0);
    lcd.print(y);
  }
  //returns value to compare if the obstacle is the same cell as Peppa
  if (m == 0 && obstacleHeight == 0){
    return 1;
  }
  else if (m == 0 && obstacleHeight == 1) {
    return 2;
  }
  else {
    return 0;
  }
}

void end_game(int score) //Game over sequence after the player loses
{
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("SCORE:");
  lcd.setCursor(7,1);
  lcd.print(score);
  lcd.home();
  lcd.print("GAME OVER :(");
  delay(5000);
}
