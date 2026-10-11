#include <Wire.h>
#include <LiquidCrystal_I2C.h>

byte sadFace[8] = {
  0b00000,
  0b01010, 
  0b01010, 
  0b00000,
  0b00000,
  0b01110, 
  0b10001,
  0b00000
};

byte happyFace[8] = {
  0b00000, 
  0b01010, 
  0b01010, 
  0b00000, 
  0b00000, 
  0b10001, 
  0b01110, 
  0b00000  
};

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SEG_A 11
#define SEG_B 12
#define SEG_C 13
#define SEG_D A2
#define SEG_E A3
#define SEG_F A0
#define SEG_G A1

const int segmentPins[7] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G};

const byte numbers[10][7] = {
  {1,1,1,1,1,1,0}, 
  {0,1,1,0,0,0,0}, 
  {1,1,0,1,1,0,1}, 
  {1,1,1,1,0,0,1}, 
  {0,1,1,0,0,1,1}, 
  {1,0,1,1,0,1,1}, 
  {1,0,1,1,1,1,1}, 
  {1,1,1,0,0,0,0}, 
  {1,1,1,1,1,1,1}, 
  {1,1,1,1,0,1,1}  
};

#define LED_YELLOW 6
#define LED_RED    7
#define LED_WHITE  8
#define LED_BLUE   9
const int ledPins[4] = {LED_YELLOW, LED_RED, LED_WHITE, LED_BLUE};

#define BUZZER 10

#define NOTE_C5 523   
#define NOTE_E5 659   
#define NOTE_G5 784   
#define NOTE_C6 1047  

const int ledTones[4] = {
  NOTE_C5,  
  NOTE_E5,  
  NOTE_G5,  
  NOTE_C6   
};

#define MUX_S0 2
#define MUX_S1 3
#define MUX_S2 4
#define MUX_Y  5

const int buttonValues[4] = {0,1,2,3}; 

const int MAX_LEVEL = 100;       
int sequence[MAX_LEVEL];          

int level = 1;
int currentSpeed = 500;
long gameSeed;

int readButton();
void playSequence();
void showLevel7Segment(int num);
void gameOver();
void beep(int duration);
void countdown();
void waitForRedStart();

void setup() {
  // MUX pins
  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_S2, OUTPUT);
  pinMode(MUX_Y, INPUT);

  // LEDs
  for(int i=0; i<4; i++) pinMode(ledPins[i], OUTPUT);

  // Buzzer
  pinMode(BUZZER, OUTPUT);

  // 7-segment
  for(int i=0;i<7;i++) pinMode(segmentPins[i], OUTPUT);

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, sadFace); 
  lcd.createChar(1, happyFace);   
  lcd.setCursor(0,0);
  lcd.print("Memory Game");  
  lcd.setCursor(12,0);       
  lcd.write(byte(1));        
  delay(2000);
  lcd.clear();

  gameSeed = analogRead(A5);  
  waitForRedStart();            
}

void loop() {

lcd.clear();

if(level == 1){
  lcd.setCursor(0,0);
  lcd.print("Get Ready!");
 } 
else{
 lcd.print("Level ");
lcd.print(level);
lcd.setCursor(0,1);
lcd.print("Get Ready!");
}

delay(1500);

countdown();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Level: ");
  lcd.print(level);

  delay(500);

  playSequence();

int buttonsToPress = level + 1;

for(int i = 0; i < buttonsToPress; i++){
  int button = readButton();

  if(button != sequence[i]){   
    gameOver();                  
    return;
  }

  correctFeedback(button, ledTones[button]);
}
  level++;
  delay(500);
}

int readButton(){
  while(true){
    for(int i=0;i<4;i++){
      digitalWrite(MUX_S0, i&0x01);
      digitalWrite(MUX_S1, (i>>1)&0x01);
      digitalWrite(MUX_S2, 0); 
      if(digitalRead(MUX_Y) == HIGH){
        while(digitalRead(MUX_Y) == HIGH);
        return i;
      }
    }
  }
}

void playSequence() {
  currentSpeed = 500 - (level * 20);
  if(currentSpeed < 120) currentSpeed = 120;

  for(int i = 0; i < level + 1; i++) {
    int b = random(0,4);  
    sequence[i] = b;        
    digitalWrite(ledPins[b], HIGH);
    tone(BUZZER, ledTones[b]);
    delay(currentSpeed);
    digitalWrite(ledPins[b], LOW);
    noTone(BUZZER);
    delay(150);
  }
}

void showLevel7Segment(int num){
  if(num>9) num=9; 
  for(int i=0;i<7;i++){
    digitalWrite(segmentPins[i], numbers[num][i]);
  }
}

void gameOver(){

int goMelody[] = {880, 1047, 784}; 
int goDurations[] = {150, 150, 150}; 

for(int i=0; i<3; i++){
    tone(BUZZER, goMelody[i], goDurations[i]);
    delay(goDurations[i] + 30); 
    noTone(BUZZER);
}
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Game Over!");
lcd.setCursor(0,1);  
lcd.print("You lose ");
lcd.setCursor(9,1); 
lcd.write(byte(0)); 
tone(BUZZER, 500, 300);
delay(300);
tone(BUZZER, 300, 400);
delay(400);
noTone(BUZZER);
 
  for(int j=0;j<3;j++){
    for(int i=0;i<4;i++) digitalWrite(ledPins[i], HIGH);
    delay(300);
    for(int i=0;i<4;i++) digitalWrite(ledPins[i], LOW);
    delay(300);
  }
 level = 1;
 gameSeed = analogRead(A5); 
 waitForRedStart();
}

void beep(int duration){
  tone(BUZZER, 880, duration);   
  delay(duration);
  noTone(BUZZER);
}

void correctFeedback(int button, int toneFreq){
  digitalWrite(ledPins[button], HIGH);

  tone(BUZZER, toneFreq);      
  delay(currentSpeed);         

  digitalWrite(ledPins[button], LOW);
  noTone(BUZZER);

  delay(150);                
}

void playfulStartLCD() {
  String msg = "   LET'S PLAY!   "; 
  int melody[]    = {1047, 1319, 1568, 2093, 1568, 1319};  
  int durations[] = {120, 120, 120, 120, 120, 120};

  lcd.clear();             
  lcd.setCursor(0, 0);     

  for(int scroll = 0; scroll < msg.length() - 15; scroll++) {
    lcd.setCursor(0,0);    
    lcd.print(msg.substring(scroll, scroll + 16));

    for(int i = 0; i < 4; i++) {
      digitalWrite(ledPins[i], HIGH);
      tone(BUZZER, melody[i % 6], durations[i % 6]);
      delay(durations[i % 6]);
      digitalWrite(ledPins[i], LOW);
      noTone(BUZZER);
    }
    delay(50);
  }
  lcd.clear(); 
}

void countdown() {
  for(int i = 3; i >= 1; i--) {
    showLevel7Segment(i);       

    lcd.setCursor(0,1);      
    lcd.print(i);
    lcd.print("   ");           

    tone(BUZZER, 700 + (i * 200), 200); 
    delay(1000);                     
    noTone(BUZZER);
  }

  for(int j = 0; j < 7; j++) {
    digitalWrite(segmentPins[j], LOW);
  }

  lcd.setCursor(0,1);
  lcd.print("               
}
// Wait for RED button to start
// Wait for RED button to start
void waitForRedStart(){
  while(true){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Press RED Button");
    lcd.setCursor(0,1);
    lcd.print("to Start!");

    digitalWrite(LED_RED, HIGH);

    int button = readButton();

   // ✅ Correct button (RED = 1)
 if(button == 1){
    digitalWrite(LED_RED, LOW);
    playfulStartLCD(); 
    lcd.clear();

    // 🎵 Playful "game start" fanfare
    int melody[] = {NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6, NOTE_G5, NOTE_E5};
    int durations[] = {150, 150, 150, 300, 150, 150};

    for(int i=0; i<6; i++){
        digitalWrite(ledPins[i%4], HIGH);  // cycle LEDs for fun effect
        tone(BUZZER, melody[i], durations[i]);
        delay(durations[i]);
        digitalWrite(ledPins[i%4], LOW);
        noTone(BUZZER);
        delay(50); // small gap between notes
    }
    
    lcd.clear();
    return;  // game starts now
}
    // ❌ Wrong button pressed
    else{
      digitalWrite(LED_RED, LOW);

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Invalid!");

      // Error sound
      tone(BUZZER, 400, 300);
      delay(400);
      noTone(BUZZER);

      delay(1000);   // show invalid message for 1 second
    }
  }
}