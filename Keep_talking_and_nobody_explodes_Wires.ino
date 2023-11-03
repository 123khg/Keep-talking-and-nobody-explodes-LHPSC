#include <Wire.h>
#define SLAVE 7778
#define LEDWINPIN 12
#define LEDLOSEPIN 11

int wirepins[5] = {15, 16, 17, 20, 21};
int wireorder[5] = {0, 0, 0, 0, 0}; //0, 1, 2, 3 correspond to Brown/Orange, Black, Yellow, Red. Gets value when init module

int volt, tempvolttime=0, unplugOrder=1;
int wireTakenOut[5] = {0, 0, 0, 0, 0}; //Player's wire unplugging order
int wireSolutionOrder[5] = {0, 0, 0, 0, 0}; //Takes in index of wirepins and copy solution from scenario

const int scenario1[5] = {3, 0, 4, 2, 1};
const int scenario2[5] = {3, 4, 0, 1, 2};
const int scenario3[5] = {0, 1, 2, 3, 4};
const int scenario4[5] = {1, 4, 3, 2, 0};
const int scenario5[5] = {0, 4, 1, 3, 2};

int count=0, time = millis();
void setup() {
  Wire.begin();

  pinMode(LEDWINPIN, OUTPUT);
  pinMode(LEDLOSEPIN, OUTPUT);
  for (int i=0; i<5; i++)
    pinMode(i, INPUT);

  Serial.begin(9600);

  //Read input for wire order
  for (int i=0; i<5; i++){
    volt = analogRead(wirepins[i]);
    
    if (volt<20){
      while (True) {
        volt = analogRead(wirepins[i]);
        
        if (volt>20) {
          digitalWrite(LEDLOSEPIN, HIGH);

          if (tempvolttime==0)
            tempvolttime = millis();
          else if (millis()-tempvolttime>=2000) {
            digitalWrite(LEDLOSEPIN, LOW);
            break;
          }
        }
        else {
          digitalWrite(LEDLOSEPIN, HIGH);
          delay(200);
          digitalWrite(LEDLOSEPIN, LOW);
          delay(200);
        }
      } 
    }
    if (volt>360 && volt<450)
      wireorder[i] = 0;
    else if (volt>455 && volt<555)
      wireorder[i] = 1;
    else if (volt>590 && volt<655)
      wireorder[i] = 2;
    else if (volt>990)
      wireorder[i] = 3;
    Serial.print(wireorder[i]);
  }
  Serial.println("");

  //Get solution
  if (wireorder[1]==0 && wireorder[4]==1)
    for (int i=0; i<5; i++)
      wireSolutionOrder[i] = scenario1[i];
  else if (wireorder[0]==1 && wireorder[2]==3)
    for (int i=0; i<5; i++)
      wireSolutionOrder[i] = scenario2[i];
  else if (wireorder[0]==0 && wireorder[4]==2)
    for (int i=0; i<5; i++)
      wireSolutionOrder[i] = scenario3[i];
  else if (wireorder[4]==0)
    for (int i=0; i<5; i++)
      wireSolutionOrder[i] = scenario4[i];
  else
    for (int i=0; i<5; i++)
      wireSolutionOrder[i] = scenario5[i];
  for (int m=0; m<5; m++)
    Serial.print(wireSolutionOrder[m]);
  Serial.println("");
}

void loop() {
  for (int i=0; i<5; i++){
    volt = analogRead(wirepins[i]);

    if (volt==0 && wireTakenOut[i]==0){
      wireTakenOut[i] += unplugOrder;

      Serial.println("Wire taken: " + String(i) + " Unplug order: " + String(unplugOrder));
      Serial.println(wireSolutionOrder[unplugOrder-1]);
      Serial.println(wireSolutionOrder[unplugOrder-1] != i);

      if (wireSolutionOrder[unplugOrder-1] != i){
        digitalWrite(LEDLOSEPIN, HIGH);

        Wire.beginTransmission(SLAVE);
        Wire.write(0);
        Wire.endTransmission(SLAVE);

        count = 4;
      }
      else if (wireSolutionOrder[unplugOrder-1] == i){
        digitalWrite(LEDWINPIN, HIGH);
        count = 4;
      }

      unplugOrder++;
    }
  }

  if (millis()-time >= 1000){
    time = millis();
    if (count>0)
      count--;

    if (count <= 0){
      digitalWrite(LEDWINPIN, LOW);
      digitalWrite(LEDLOSEPIN, LOW);
    }
  }

  if (unplugOrder == 6){
    Wire.beginTransmission(SLAVE);
    Wire.write(1);
    Wire.endTransmission();
    unplugOrder++;
  }
  else if (unplugOrder>6)
    digitalWrite(LEDWINPIN, HIGH);
  // Serial.print("A1: " + String(analogRead(wirepins[0])) + "\t");
  // Serial.print("A2: " + String(analogRead(wirepins[1])) + "\t");
  // Serial.println("A3: " + String(analogRead(wirepins[2])) + "\t");
  // Serial.print("A6: " + String(analogRead(wirepins[3])) + "\t");
  // Serial.println("A7: " + String(analogRead(wirepins[4])));
}
