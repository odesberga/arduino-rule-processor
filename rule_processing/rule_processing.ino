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

  String sval="";
  while (Serial.available() > 0) {
      StartChar = Serial.read();  
  if (StartChar == 33) {
    sval=Serial.readStringUntil('#'); 
  Processinpstring(0,sval);
  break;
  }
  }
  
  if (occupied == false){
 if (Serial1.available()) { 
   while (Serial1.available() > 0) {
  StartChar = Serial1.read();  
  if (StartChar == 33) {
  sval=Serial1.readStringUntil('#'); 
  Processinpstring(1,sval);
  break;
  }
 }
     Serial1.flush();
 }   
    while (Serial2.available() > 0) {
     StartChar = Serial2.read();     
    if (StartChar == 33) {
  sval=Serial2.readStringUntil('#');   
  Processinpstring(2,sval);
  break;
  }
  }
    if (Serial3.available()) { 
      while (Serial3.available() > 0) {
      StartChar =Serial3.read(); 
      if (StartChar == 33) {
  sval=Serial3.readStringUntil('#'); 
  Processinpstring(3,sval);
  break;
    }
    }
    Serial3.flush();
    }
    }



readButtons();

time_t t = now();
if ((second(t) ==0) && (timechecked==false)) {
  timechecked=true;
  checktimers(t);

}

if (second(t) ==1) {timechecked=false;}


}


void checktimers(time_t t){
int currentminute=minute(t);
int currenthour=hour(t);
  RuleFile = SD.open("rulefile");    
 String l_line = "";
      while (RuleFile.available()) {
        
        l_line = RuleFile.readStringUntil('\n');            
        String finpbus = GetValFromString(l_line,1);       
  if (finpbus == "99") {    
           String fiportnum = GetValFromString(l_line,3);
           

    if (fiportnum == "99") {
      if ((GetValFromString(l_line,9)==String(currenthour) ) && (GetValFromString(l_line,10)==String(currentminute))){
      sendcode(GetValFromString(l_line,5),GetValFromString(l_line,6),GetValFromString(l_line,7),GetValFromString(l_line,8));
      }
     }
  }
 
}
 RuleFile.close();       



}


void readButtons() {
 if (millis()-btnDelay >= del) {
for(int i = 0;i<pc;i++){   
    if(digitalRead(ports[i][0]) == LOW){
     ports[i][1]=1; 
    }  
    else
    {
      ports[i][1]=0;
    }
    
    if(ports[i][1]!=ports[i][2]){
      if(ports[i][1]==0){      
       ports[i][2]=0;
       int secs = now() - ports[i][3];        
       ports[i][3]=0;
       Processinpstring(0,"20,"+String(i+1)+",0,"+String(secs)+"rulefile");
       
      }
      if(ports[i][1]==1){      
           ports[i][2]=1;  
           ports[i][3]=now();  
           Processinpstring(0,"20,"+String(i+1)+",1,0"+"rulefile");
           SendDebug("mapbutton:0:0:"+String(i+1));
    }

      
    }   
}
btnDelay=millis();
}
 
  
}


void PrintLineToFile(String Aval,String filename) {
  SendDebug(Aval);
    RuleFile = SD.open(filename.c_str(), FILE_WRITE);
    
  if (RuleFile) {
     RuleFile.println(Aval);
    RuleFile.close();
  } else {
    SendDebug("error opening file");
  }  
  
}

void EmtyFile(String filename) {
 //SD.remove(filename.c_str());
    dbg=true;
 SendDebug(filename+" Deleted");
    dbg=false; 
}

void SendDebug(String Aval) {
 if (dbg == true) {
  Serial.print("!"+Aval+"#");
 //Serial.print("\n");
  //delay(10);
 }

 
  
}

void Processinpstring(int Inpbus, String inpstring) {
  occupied=true;
  SendDebug("InpStr Bus:"+String(Inpbus)+" String: "+inpstring);
  String a=GetValFromString1(inpstring,1);
  
 switch (a.toInt()) {
   case 1:
      sendcode(GetValFromString1(inpstring,2),GetValFromString1(inpstring,3),GetValFromString1(inpstring,4),GetValFromString1(inpstring,5));
      occupied=false;
      break;
   case 2:   
     EmtyFile(GetValFromString1(inpstring,2));
     occupied=false;
     break;
   case 3:    
     PrintLineToFile(GetValFromString1(inpstring,2),GetValFromString1(inpstring,3));
     occupied=false;
     break; 
   case 4:   
     pfile(GetValFromString1(inpstring,2));
     occupied=false;
     break; 
     case 5:   
     toggledebug();
     occupied=false;
     break;    
     case 6:   
    // setTime(inpstring);
     occupied=false;
     break;    
     case 7:   
   // setTime(inpstring);
     occupied=false;
     break;     
   case 20:   
   //  parseandsendcode(Inpbus,GetValFromString(inpstring,2),GetValFromString(inpstring,3),GetValFromString(inpstring,4));
 
     parseandsendcodefrmfile(Inpbus,GetValFromString1(inpstring,2),GetValFromString1(inpstring,3),GetValFromString1(inpstring,4),GetValFromString1(inpstring,5));
     occupied=false;
     break;
     case 99: 
     dbg=true;  
     SendDebug(inpstring.substring(3,inpstring.length()));
     dbg=false;
     occupied=false;
     break;

}   
  
    
}
void printButtons() {
    SendDebug("Mapped : Physical");
  for (int i=0; i<pc;i++){
    SendDebug(String(i+1)+" : "+String(ports[i][0]));
  }
  

}

void toggledebug() { 
 if (dbg == false) {
   dbg=true;
   SendDebug("Debugging On");
 }
 else 
 {
    Serial.print("!Debugging Off#");
  dbg=false; 
 }
  
}

void pfile(String filename) {
dbg=true;
  String aString="";
 SendDebug("RuleFile Content: filename: "+filename);
RuleFile = SD.open(filename.c_str());
       while (RuleFile.available()) {        
        aString = RuleFile.readStringUntil('\n');
         SendDebug(aString);        
       }    
    
RuleFile.close();    

}




String GetValFromString(String string,int num)
{
  String res;
  String Result;
  int i = 1;
  int commaindex[12];  
  commaindex[0] = string.indexOf(';');
   if (num == i) {
    res = string.substring(0,commaindex[0]);
   }
    else
 {   
    for (i = 2; i < 13; i++) {
        commaindex[i-1] = string.indexOf(';',commaindex[i-2]+1); 
        if (num == i) {
        res = string.substring(commaindex[i-2]+1,commaindex[i-1]);
        break;
         }
       if (i == 12 and num == 13) {
       res = string.substring(commaindex[i-1]+1 ,string.length());
 
       }        
       }
 }
 Result=res;

 return Result; 
}


String GetValFromString1(String string,int num)
{
  String res;
  String Result;
  int i = 1;
  int commaindex[12];  
  commaindex[0] = string.indexOf(',');
   if (num == i) {
    res = string.substring(0,commaindex[0]);
   }
    else
 {   
    for (i = 2; i < 13; i++) {
        commaindex[i-1] = string.indexOf(',',commaindex[i-2]+1); 
        if (num == i) {
        res = string.substring(commaindex[i-2]+1,commaindex[i-1]);
        break;
         }
       if (i == 12 and num == 13) {
       res = string.substring(commaindex[i-1]+1 ,string.length());
 
       }        
       }
 }
 Result=res;

 return Result; 
}


void parseandsendcodefrmfile(int inpbus, String inpport, String inpstate, String longpress,String filename) 
{
   SendDebug("parseandsendcode " + String(inpbus)+","+String(inpport)+","+String(inpstate)+","+String(longpress));

    RuleFile = SD.open(filename.c_str());    
 String l_line = "";
      while (RuleFile.available()) {
        
        l_line = RuleFile.readStringUntil('\n');            
        String finpbus = GetValFromString(l_line,1);       
  if (finpbus == String(inpbus)) {    
           String fiportnum = GetValFromString(l_line,3);

    if (fiportnum == inpport) { 
     
 
            String fistate =GetValFromString(l_line,4);
      if (fistate == inpstate) {
         String flpress = GetValFromString(l_line,13); 
         int lp=longpress.toInt();
         int flp=flpress.toInt();
        if ((lp >= flp-1) && (lp < flp+2) || (flpress=="99")) {
       sendcode(GetValFromString(l_line,5),GetValFromString(l_line,6),GetValFromString(l_line,7),GetValFromString(l_line,8));
   
      }
      }
    }
  }
 
}
 RuleFile.close();       
}



void sendcode(String obus,String omod,String oport,String ostate) {
 SendDebug("Sending: " +String(omod)+","+String(0)+","+String(oport)+","+String(ostate));
  
  
  if (obus == "1") {
  Serial1.print(omod);
  Serial1.print(",");
  Serial1.print("0");
  Serial1.print(",");
  Serial1.print(oport);
  Serial1.print(",");
  Serial1.print(ostate);
  Serial1.print("\n");
  while (Serial1.available() > 0) {
    Serial1.read();
   }
   }
  
  
  
}
void printCurrentTime()
{
    // digital clock display of the time
    SendDebug (printDigits(hour())+":"+printDigits(minute())+":"+printDigits(second())+":"+String(day())+":"+String(month())+":"+String(year()));

}
String printDigits(int digits)
{
    if(digits < 10)
        return "0"+String(digits);
        else
        return "0"+String(digits);
    
}
