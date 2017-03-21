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
//File root;
byte StartChar;
const int cs = 4;
boolean dbg = true;
boolean occupied = false;
boolean timechecked= false;
// Buttons variabler
int pc=50;
char serbuf[250];
//char rulebuf[62];
struct msg
{
  char inbus[3];
  char inport[3];
  char msgtype[3];
  char tobus[3];
  char tomodule[3];
  char modulecount[3];
  char port[3];
  char filename[11];
  char data[200];
  char longpress[3];
};
typedef struct msg Msg;
struct rl
{
  char inbus[3];
  char inpmodule[3];
  char inport[3];
  char inpstate[3];
  char outpbus[3];
  char outpmmodule[3];
  char outpport[3];
  char outpstate[3];
  char hrstart[3];
  char minstart[3];
  char hrend[3];
  char minend[3];
  char lpressec[3];
};
typedef struct rl Rule;

char intbus[3];
Msg emptymsg;
Msg imessage;
Msg omessage;
Rule rule;
Rule emptyrule;

//port,currentstate,prevstate,prev now()
int ports[50][4] = {
  {9, 0, 0, 0} ,
  {8, 0, 0, 0} ,
  {7, 0, 0, 0} ,
  {6, 0, 0, 0} ,
  {5, 0, 0, 0} ,
//  {4, 0, 0, 0} ,
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
 // {50, 0, 0, 0} ,
 // {51, 0, 0, 0} ,
 // {52, 0, 0, 0} ,
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



//   pinMode(53, OUTPUT);

// while (!SD.begin(53)) {
//   //  SendDebug("initialization failed");
//   //  return;
//   digitalWrite(53, HIGH);
//   delay(50);
//   digitalWrite(53, LOW);
//   }
//   SendDebug("initialization done.");
//  RuleFile = SD.open("/");

    // setSyncProvider(RTC.get);   // the function to get the time from the RTC
    // if(timeStatus() != timeSet)
    //     SendDebug("Unable to sync with the RTC");
    // else
    //     SendDebug("RTC has set the system time");
    SendDebug("Initializing SD card...");
    initSD();
    SendDebug("initialization done.");

    for(int i = 0;i<pc;i++){
    pinMode(ports[i][0], OUTPUT);
    if (ports[i][0] != 53){
    digitalWrite(ports[i][0],HIGH);
  } else{
    SendDebug(String(i));
  }



  }


//printDirectory(RuleFile,0);

  //printCurrentTime();



printButtons();
ListFiles();
//printDirectory(RuleFile,0);
dbg = false;
}


void loop() {
  memset(serbuf,0, sizeof(serbuf));
  imessage=emptymsg;
  char inchar;
  byte index=0;
  String inst;
  while (Serial.available() > 0)
  {
      inchar = Serial.read();
    if (inchar == '!') {
      inst= Serial.readStringUntil('#');
      inst.toCharArray(serbuf, 250);
      strcat(serbuf, '\0');
      populateiMsg(0);
      if (isthisforme()){

        processMsg();
      };
     prntrule();
      memset(serbuf,0, sizeof(serbuf));
      imessage=emptymsg;
      omessage=emptymsg;
    }
}

readButtons();

}



bool isthisforme(){
int tm = atoi(imessage.tomodule);
int mc = atoi(imessage.modulecount);
if ( tm == mc ){
  return true;
} else {
  return false;
};



}
void prntrule(){

  Serial.print(imessage.tobus);
  Serial.println();

  Serial.print(imessage.tomodule);
  Serial.println();

  Serial.print(imessage.data);
  Serial.println("");

}
void processMsg(){
  int pt = atoi(imessage.msgtype);
  switch (pt) {
    case 1:
      sendData();
      break;
    case 2:
      DeleteFile();
      break;
    case 3:
    SendFiles();
      break;
    case 4:
      pfile();
      break;
    case 6:
      PrintLineToFile();
      break;
    case 20:
      ExecuteRulesFromFile();
      break;

  }
  memset(serbuf,0, sizeof(serbuf));
  imessage=emptymsg;
  omessage=emptymsg;
}

bool initSD(){
  pinMode(53, OUTPUT);
  digitalWrite(53, LOW);
  while (!SD.begin(53)) {
        SendDebug("initialization failed");
       return false;
  }
  return true;
}


void ListFiles(){
  File root = SD.open("/");
root.rewindDirectory();
String filenames="";
//SD.close();
//RuleFile = SD.open("/");

  while(true) {
    File entry =  root.openNextFile();
    if (! entry) {
      // no more files
      //Serial.println("**nomorefiles**");
      break;
    }
    filenames = filenames + entry.name()+" ";

  }
  root.close();
  SendDebug(filenames);
}


void SendDebug(String Aval) {
 if (dbg == true) {
genOmessageAndSendData("01,"+Aval,"00","00");
  }
}

void SendInfo(String Aval) {

genOmessageAndSendData("01,"+Aval,"00","00");

}

void populateiMsg(int inpBus) {
  imessage=emptymsg;
  if (inpBus == 0){
  imessage.inbus[0]='0';
  imessage.inbus[1]='0';
  imessage.inbus[2]='\0';
}
if (inpBus == 1){
imessage.inbus[0]='0';
imessage.inbus[1]='1';
imessage.inbus[2]='\0';
}
if (inpBus == 2){
imessage.inbus[0]='0';
imessage.inbus[1]='2';
imessage.inbus[2]='\0';
}
if (inpBus == 3){
imessage.inbus[0]='0';
imessage.inbus[1]='3';
imessage.inbus[2]='\0';
}

  imessage.tomodule[0]=serbuf[0];
  imessage.tomodule[1]=serbuf[1];
  imessage.tomodule[2]='\0';
  imessage.modulecount[0]=serbuf[2];
  imessage.modulecount[1]=serbuf[3];
  imessage.modulecount[2]='\0';
  imessage.msgtype[0] = serbuf[5];
  imessage.msgtype[1] = serbuf[6];
  imessage.msgtype[3] = '\0';
  int commacount=0;
  int idx=0;

  for (int i =8; i< sizeof(serbuf)-1;i++){
      if (serbuf[i]=='\0'){break;};
      if (serbuf[i]==','){
        commacount++;
        idx=0;
      } else {
        genMsg(i,idx,commacount);
        idx++;
      }
  }
}

void genMsg(int index,int index2,int commacount){
  int pt = atoi(imessage.msgtype);
  switch (pt) {
    case 1:
          switch (commacount) {
            case 0:
              omessage.tobus[index2]= serbuf[index];
              break;
            case 1:
              omessage.tomodule[index2]= serbuf[index];
              break;
            case 2:
              omessage.data[index2]= serbuf[index];
              break;
            }
    break;

    case 2:
            switch (commacount) {
              case 0:
                imessage.filename[index2]= serbuf[index];
                break;
              }

    break;

        case 4:
          switch (commacount) {
            case 0:
              imessage.filename[index2]= serbuf[index];
              break;
            }
    break;
    case 6:
          switch (commacount) {
            case 0:
              imessage.filename[index2]= serbuf[index];
              break;
            case 1:
              imessage.data[index2]= serbuf[index];
              break;
            }
    break;
    case 20:

            switch (commacount) {
              case 0:
                imessage.inport[index2]= serbuf[index];
                break;
              case 1:
                imessage.data[index2]= serbuf[index];
                break;
              case 2:
                  imessage.longpress[index2]= serbuf[index];
                  break;
              case 3:
                    imessage.filename[index2]= serbuf[index];
                    break;
              }
  break;
    }
}

void populateRule(String rulestring){

  char rulebuf[62];
  int commacount=0;
  int idx=0;
  rulestring.toCharArray(rulebuf, 62);
  strcat(rulebuf, '\0');
rule=emptyrule;
  for (int i =0; i< sizeof(rulebuf)-1;i++){
      if (rulebuf[i]=='\0'){break;};
      if (rulebuf[i]==';'){
        commacount++;
        idx=0;
      } else {
        switch (commacount) {
          case 0:
            rule.inbus[idx]= rulebuf[i];
            break;
          case 1:
            rule.inpmodule[idx]= rulebuf[i];
            break;
          case 2:
            rule.inport[idx]= rulebuf[i];
            break;
            case 3:
              rule.inpstate[idx]= rulebuf[i];
              break;
              case 4:
                rule.outpbus[idx]= rulebuf[i];
                break;

              case 5:
                rule.outpmmodule[idx]= rulebuf[i];
                break;

              case 6:
                rule.outpport[idx]= rulebuf[i];
                break;

              case 7:
                rule.outpstate[idx]= rulebuf[i];
                break;

              case 8:
                rule.hrstart[idx]= rulebuf[i];
                break;

              case 9:
                rule.minstart[idx]= rulebuf[i];
                break;

              case 10:
                rule.hrend[idx]= rulebuf[i];
                break;

              case 11:
                rule.minend[idx]= rulebuf[i];
                break;

              case 12:
                rule.lpressec[idx]= rulebuf[i];
                break;
        }



        idx++;
      }
  }


}




void genOmessageAndSendData(String data,String obus,String omod){
omessage=emptymsg;
data.toCharArray(omessage.data, data.length()+1);
  if (obus.length() == 1) {
    omessage.tobus[0] = "0";
    omessage.tobus[1] = obus[0];
  } else {
    omessage.tobus[0] = obus[0];
    omessage.tobus[1] = obus[1];
  }
  if (omod.length() == 1) {
    omessage.tomodule[0] = "0";
    omessage.tomodule[1] = omod[0];
  } else {
    omessage.tomodule[0] = omod[0];
    omessage.tomodule[1] = omod[1];
  }
sendData();
omessage=emptymsg;
}

void sendData(){

  int pt = atoi(omessage.tobus);
  switch (pt) {
    case 0:
      Serial.print("!");
      Serial.print(omessage.tomodule);
    //  Serial.print(",");
      Serial.print("01");
      //Serial.print(",");
      Serial.print(omessage.data);
      Serial.print("#");
      break;
    case 1:
      Serial1.print("!");
      Serial1.print(omessage.tomodule);
      Serial1.print("01");
      Serial1.print(omessage.data);
      Serial1.print("#");
      break;
      case 2:
        Serial2.print("!");
        Serial2.print(omessage.tomodule);
        Serial2.print("01");
        Serial2.print(omessage.data);
        Serial2.print("#");
        break;

        case 3:
          Serial3.print("!");
          Serial3.print(omessage.tomodule);
          Serial3.print("01");
          Serial3.print(omessage.data);
          Serial3.print("#");
          break;


  }

  omessage=emptymsg;
}


void pfile() {
File RuleFile;
RuleFile = SD.open(imessage.filename);
       while (RuleFile.available()) {
      String aString = RuleFile.readStringUntil('\n');
         genOmessageAndSendData("06,"+String(imessage.filename)+","+aString,"00","01");
       }
 genOmessageAndSendData("06,"+String(imessage.filename)+",EOF","00","01");
RuleFile.close();

}
void ExecuteRulesFromFile(){
  File RuleFile;
  RuleFile = SD.open(imessage.filename);
      while (RuleFile.available()) {
            String aString = RuleFile.readStringUntil('\n');
            populateRule(aString);
          //  SendDebug("imessage.filename="+String(imessage.filename));
              if (String(imessage.filename) == "main.r"){
              //  SendDebug("imessage.filename="+String(imessage.filename));

                  if (atoi(rule.inbus) == atoi(imessage.inbus)) {
                //    SendDebug("rule.inbus="+String(rule.inbus));
              //      SendDebug("imessage.inbus="+String(imessage.inbus));

                      if (atoi(rule.inport) == atoi(imessage.inport)) {
                  //      SendDebug("rule.inport="+String(rule.inport));
                  //      SendDebug("imessage.inport="+String(imessage.inport));
                          if  (atoi(rule.inpstate) == atoi(imessage.data)) {
                      //      SendDebug("rule.inpstate[0]="+String(rule.inpstate[0]));
                      //      SendDebug("imessage.data[0]="+String(imessage.data[0]));
                      //      SendDebug("imessage.longpress="+String(imessage.longpress));
                      //      SendDebug("rule.lpressec="+String(rule.lpressec));
                                    if ( atoi(imessage.longpress) <= (atoi(rule.lpressec) + 3)  and  atoi(imessage.longpress) >= atoi(rule.lpressec))  {
                                          genOmessageAndSendData("21,"+String(rule.outpstate), String(rule.outpbus), String(rule.outpmmodule));
                                        SendDebug("Sending RULE: 21 State:" + String(rule.outpstate)+" Bus:"+ String(rule.outpbus)+" Module:"+ String(rule.outpmmodule)) ;
                                    }

                          }
                      }
                  }
            } else {
                genOmessageAndSendData("21,"+String(rule.outpstate), String(rule.outpbus), String(rule.outpmmodule));


            }
      }


  RuleFile.close();



}



void SendFiles() {
  File root = SD.open("/");
root.rewindDirectory();
String filenames="";
//SD.close();
//RuleFile = SD.open("/");

  while(true) {
    File entry =  root.openNextFile();
    if (! entry) {
      // no more files
      //Serial.println("**nomorefiles**");
      break;
    }
    entry = SD.open(entry.name());
           while (entry.available()) {
          String aString = entry.readStringUntil('\n');
             genOmessageAndSendData("06,"+String(entry.name())+","+aString,"00","01");
           }
     genOmessageAndSendData("06,"+String(entry.name())+",EOF","00","01");
    RuleFile.close();

  }
  root.close();


}

void PrintLineToFile() {

  File  f = SD.open(imessage.filename, FILE_WRITE);
  if (f) {
     f.println(String(imessage.data));
    f.close();
  } else {
    SendDebug("error opening file " + String(imessage.filename));
  }

}

void DeleteFile() {
 SD.remove(imessage.filename);
    dbg=true;
 SendDebug(String(imessage.filename) + " Deleted");
    dbg=false;
}


void printButtons() {
    SendDebug("Mapped : Physical");
  for (int i=0; i<pc;i++){
    SendDebug(String(i+1)+" : "+String(ports[i][0]));
  }


}
void incOmessageModCount(){
int pt = atoi(omessage.modulecount);
pt++;
String astring = String(pt);
if (astring.length()==1)  {
  omessage.modulecount[0]='0';
  omessage.modulecount[1]=astring[0];
} else {
  omessage.modulecount[0]=astring[0];
  omessage.modulecount[1]=astring[1];

}



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
      String inpMsg = "";
      if(ports[i][1]==0){
       ports[i][2]=0;
       int secs = now() - ports[i][3];
       ports[i][3]=0;
       // Processinpstring(0,"20,"+String(i+1)+",0,"+String(secs));
       inpMsg="0000,20,"+String(i+1)+",0,"+String(secs)+",main.r";
      }
      if(ports[i][1]==1){
           ports[i][2]=1;
           ports[i][3]=now();
           // Processinpstring(0,"20,"+String(i+1)+",1,0");
           inpMsg="0000,20,"+String(i+1)+",1,0,main.r";
           SendDebug("mapbutton;0;0;"+String(i+1));
    }
    memset(serbuf,0, sizeof(serbuf));
    inpMsg.toCharArray(serbuf, 250);
    strcat(serbuf, '\0');
    populateiMsg(0);
    processMsg();

    }
}
btnDelay=millis();
}


}
