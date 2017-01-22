#include <Arduino.h>

#include <SD.h>
#include <SoftwareSerial.h>
#include <DS3232RTC.h>
#include <Time.h>
#include <Wire.h>

File RuleFile;

#define MaxRuleCount 3
#define rlcount 13
int ruleArray[MaxRuleCount][rlcount];
// set delay for reading buttons
unsigned long btnDelay;
const int del=100;

byte StartChar;
const int cs = 4;
boolean dbg = true;
boolean occupied = false;
boolean timechecked= false;
// Buttons variabler
int pc=51;
//port,currentstate,prevstate,prev now()
int ports[58][4] = {
  {9, 0, 0, 0} ,
  {8, 0, 0, 0} ,
  {7, 0, 0, 0} ,
  {6, 0, 0, 0} ,
  {5, 0, 0, 0} ,
  {4, 0, 0, 0} ,
  {3, 0, 0, 0} ,
 // {1, 0, 0, 0} ,
  {0, 0, 0, 0} ,
  {22, 0, 0, 0} ,
  {23, 0, 0, 0} ,
  {24, 0, 0, 0} ,
  {25, 0, 0, 0} ,
  {26, 0, 0, 0} ,
  {27, 0, 0, 0} ,
  {28, 0, 0, 0} ,
  {29, 0, 0, 0} ,
  {30, 0, 0, 0} ,
  {31, 0, 0, 0} ,
  {32, 0, 0, 0} ,
  {33, 0, 0, 0} ,
  {34, 0, 0, 0} ,
  {35, 0, 0, 0} ,
  {36, 0, 0, 0} ,
  {37, 0, 0, 0} ,
  {38, 0, 0, 0} ,
  {39, 0, 0, 0} ,
  {40, 0, 0, 0} ,
  {41, 0, 0, 0} ,
  {42, 0, 0, 0} ,
  {43, 0, 0, 0} ,
  {44, 0, 0, 0} ,
  {45, 0, 0, 0} ,
  {46, 0, 0, 0} ,
  {47, 0, 0, 0} ,
  {48, 0, 0, 0} ,
  {49, 0, 0, 0} ,
//  {50, 0, 0, 0} ,
 // {51, 0, 0, 0} ,
//  {52, 0, 0, 0} ,
  {A15, 0, 0, 0} ,
  {A14, 0, 0, 0} ,
  {A13, 0, 0, 0} ,
  {A12, 0, 0, 0} ,
  {A11, 0, 0, 0} ,
  {A10, 0, 0, 0} ,
  {A9, 0, 0, 0} ,
  {A8, 0, 0, 0} ,
  {A7, 0, 0, 0} ,
  {A6, 0, 0, 0} ,
  {A5, 0, 0, 0} ,
  {A4, 0, 0, 0} ,
  {A3, 0, 0, 0} ,
  {A2, 0, 0, 0} ,
  {A1, 0, 0, 0}

};



void setup() {
dbg=true;

  Serial.begin(57600);
    Serial1.begin(14400);
    Serial2.begin(14400);
    Serial3.begin(14400);
    SendDebug("Initializing SD card...");
      pinMode(53, OUTPUT);

//   pinMode(53, OUTPUT);

while (!SD.begin(53)) {
  //  SendDebug("initialization failed");
  //  return;
  digitalWrite(53, HIGH);
  delay(50);
  digitalWrite(53, LOW);
  }
  SendDebug("initialization done.");
    setSyncProvider(RTC.get);   // the function to get the time from the RTC
    if(timeStatus() != timeSet)
        SendDebug("Unable to sync with the RTC");
    else
        SendDebug("RTC has set the system time");

  printCurrentTime();

  for(int i = 0;i<pc;i++){
  pinMode(ports[i][0], OUTPUT);
  digitalWrite(ports[i][0],HIGH);


}
printButtons();
dbg = false;
}


void loop() {




}
