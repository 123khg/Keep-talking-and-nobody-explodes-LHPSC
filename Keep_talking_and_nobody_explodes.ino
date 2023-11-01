#include <Wire.h>

#define RST 7
#define DATA 6
#define CLK 5  //Sends memory to output
#define LATCH 4  //Store bit in memory
#define BUZZ 3

// 3 digit 7 segment display
// The table below shows which pin on the display is HIGH/LOW to make the segment of that digit ON
// Example: Digit 1, segment a = pin 4 HIGH and pin 10 LOW
/*Digit|    1     2    3
Segment|
   a   |   4/10  1/7  4/6
   b   |   2/10  1/8  3/6
   c   |   3/10  2/8  2/6
   d   |   1/9   5/9  3/7
   e   |   2/9   4/9  2/7
   f   |   1/10  5/8  1/6
   g   |   5/10  3/8  4/7
   dp  |   3/9   4/8  5/6
*/

const int allLedOff[10] = {1,1,1,1,1,0,0,0,0,0};
int output[10] = {1,1,1,1,1,0,0,0,0,0}; //Output according to shiftpins
const int shiftpins[10] = {6, 7, 8, 9, 10, 1, 2, 3, 4, 5}; //Outputs from 74HC595 are connected to display's pins in this order
const int digitpins[3][2][8] = {{{4, 2, 3, 1, 2, 1, 5, 3}, {10, 10, 10, 9, 9, 10, 10, 9}}, 
                                {{1, 1, 2, 5, 4, 5, 3, 4}, {7, 8, 8, 9, 9, 8, 8, 8}}, 
                                {{4, 3, 2, 3, 2, 1, 4, 5}, {6, 6, 6, 7, 7, 6, 7, 6}}};
/*                                     digit 1 high               digit 1 low
                                       digit 2 high               digit 2 low
                                       digit 3 high               digit 3 low
*/
const int numbers[10][8] = {
  {1,1,1,1,1,1,0,0}, 
  {0,1,1,0,0,0,0,0}, 
  {1,1,0,1,1,0,1,0},
  {1,1,1,1,0,0,1,0}, 
  {0,1,1,0,0,1,1,0}, 
  {1,0,1,1,0,1,1,0},
  {1,0,1,1,1,1,1,0}, 
  {1,1,1,0,0,0,0,0}, 
  {1,1,1,1,1,1,1,0},
  {1,1,1,1,0,1,1,0}};
int pin;
int totalTime = 600;
unsigned long int runTime = millis();
byte buffer[10];

void setup() {
  Wire.begin(7778);
  Wire.onReceive(receiveSignal);

  pinMode(RST, OUTPUT);

  pinMode(DATA, OUTPUT);
  digitalWrite(DATA, LOW);
  pinMode(LATCH, OUTPUT);
  digitalWrite(LATCH, LOW);
  pinMode(CLK, OUTPUT);
  digitalWrite(CLK, LOW);
  
  //Reset outputs
  digitalWrite(RST, LOW);
  digitalWrite(RST, HIGH);
  digitalWrite(LATCH, HIGH);
  digitalWrite(LATCH, LOW);
  digitalWrite(CLK, HIGH);
  digitalWrite(CLK, LOW);
  
  Serial.begin(9600);
}

void loop() {
  // Serial.println("Digit: " + String(p) + " Num: " + String(u) + " " + String(millis()-runTime) + " " + String(millis())+ " " + String(runTime));
  
  //Timer
  if (millis()-runTime>=1000) {
    if (totalTime > 0)
      totalTime-=1;
    
    runTime = millis();
  }
   
  display_digit(totalTime);
  
}

void display_digit(int totalTime){
  int displayTime[3] = {totalTime%10, totalTime%100/10, totalTime/100};
  //Don't ask me why. I also don't know what the fck is wrong with the code that's causing totalTime = 0 and displayTime[0] = 0
  //This is how i deal with it.
  display_digit_1_segment(numbers[displayTime[0]], 0, totalTime, displayTime);
  display_digit_1_segment(numbers[displayTime[1]], 1, totalTime, displayTime);
  display_digit_1_segment(numbers[displayTime[2]], 2, totalTime, displayTime);
}

void display_digit_1_segment(int segments[8], int digit, int (&totalTime), int (&displayTime)[3]) {
  for (int i = 0; i <= 7; i++){
    if (segments[i]){
      pin = digitpins[digit][0][i];
      // Serial.print("High: "+ String(pin) + "    ");
      output[pin+4] = 1;
      pin = digitpins[digit][1][i];
      // Serial.println("Low: "+ String(pin));
      output[pin-6] = 0;
      writeBit(output);
      //RESET PINS
      for (int j = 0; j<10; j++)
        output[j] = allLedOff[j];
    }
  }
}

void writeBit(int output[10]) {
  for (int i = 9; i >=0; i--) {
    digitalWrite(DATA, output[i]);
    digitalWrite(LATCH, HIGH);
    digitalWrite(LATCH, LOW);
  }
  digitalWrite(CLK, HIGH);
  digitalWrite(CLK, LOW);
}

void receiveSignal(){
  while (Wire.available()){
    if (Wire.read()==1){
      totalTime -= 30;
    }
  }
}

//74hc595 test
  // if (Serial.available()>0){
  //   a = Serial.read();
  //   Serial.flush();
  //   Serial.println(a);
  //   if (a == 113){
  //     digitalWrite(DATA, HIGH);
  //   }
  //   else if (a == 101){
  //     digitalWrite(DATA, LOW);
  //   }
  //   else if (a == 97){
  //     digitalWrite(CLK, HIGH);
  //   }
  //   else if (a == 100){
  //     digitalWrite(CLK, LOW);
  //   }
  //   else if (a == 122){
  //     digitalWrite(LATCH, HIGH);
  //   }
  //   else if (a == 99){
  //     digitalWrite(LATCH, LOW);
  //   }
  //   else if (a == 119){
  //     digitalWrite(RST, HIGH);
  //   }
  //   else if (a == 115){
  //     digitalWrite(RST, LOW);
  //   }
  // }
