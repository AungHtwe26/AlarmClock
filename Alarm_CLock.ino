// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include <LiquidCrystal.h>
#include<EEPROM.h>
#define buttonPin 10    // press for 6s to enter setting mode
#define setButton 7     // one click to set value
#define outputA 8       //sw pin from rotary encoder
#define outputB 9       //clk pin from rotary encoder
#define speakerPin 6
#define backLight 13
RTC_DS3231 rtc;
const int rs = 12, en = 11, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
char anni[] ={"Happy 8th Year Anniversary MY Love"};
unsigned int selValue[42] ={};  //store temporary user alarm set value to wirte into eeprom
unsigned int clockSetVal[5]={}; //to store hr,min,sec to adjust time
char daysOfTheWeek[7][12] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
//unsigned int number=1,Index,exitState,Day,Hour,Min,Second,SettIndex,lastState,State,timeSel,selValueIndex=0,counter=0,inti,skipIndex=0,i,j=2,blinkIndex;
unsigned int number=1,exitState,SettIndex,lastState,State,selValueIndex=0,counter=0,skipIndex=0,i,k,j=2,blinkIndex;
unsigned long myTimer,previousState,currentState;
int numTones = 10;
int tones[] = {261, 277, 294, 311, 330, 349, 370, 392, 415, 440};
//            mid C  C#   D    D#   E    F    F#   G    G#   A
void setup () {
//  Serial.begin(57600);
    lcd.begin(16,2);
    rtc.begin();
    // if (! rtc.begin()) 
    // {
    //  Serial.println("Couldn't find RTC");
    //  Serial.flush();
    //  while (1) delay(10);
    // }
     pinMode(buttonPin,INPUT_PULLUP);
     pinMode(setButton,INPUT_PULLUP);
     pinMode(outputA,INPUT);
     pinMode(outputB,INPUT);      
     pinMode(backLight,OUTPUT);
    lastState = digitalRead(outputA);    //store clock state from rotary encoder
}
//------------------------------------------------------------------------------------------------
void loop () {
  //checking switch pressed state//
  //------------------------------------------
    digitalWrite(backLight,0);
    myTimer = millis()+6000;
    while(digitalRead(buttonPin)==LOW)
    {
      while(digitalRead(buttonPin)==LOW)
      {
        if(myTimer < millis())
        {
           currentState = millis();
           if(currentState - previousState>=2000)
           {
              previousState = currentState;
              myTimer = millis()+4000;
           }
              lcd.setCursor(3,0);
              lcd.print("Clock Setting");
              delay(500);
              lcd.clear();
              SettIndex=2;
            }
            else if(myTimer-4000<millis())
            {
              previousState = millis();
              lcd.setCursor(3,0);
              lcd.print("Alarm Setting");
              delay(500);
              lcd.clear();
              SettIndex=1;
           }
        }
    }
    //---------------------------------------------
    if(SettIndex==1)
    {
      exitState=0;
      alarmSetup();
    }
    else if(SettIndex==2)
    {
      exitState=0;
      clockSetting();
    }
    else
    {   
      alarm();   
    }
}
//---------------------------------------------------------------------------------------------------------------
void alarmSetup(){
  DateTime now = rtc.now();
  for(int z=0;z<42;z++)
  {
      selValue[z] = EEPROM.read(z);
  }
  lastState = digitalRead(outputA);
  while(exitState<1)
  { 
    myTimer = millis()+600;
    while(myTimer>millis())
    {
    //---------------------------------
    //---------------------------------
    if(digitalRead(buttonPin)==LOW)
    {
      while(digitalRead(buttonPin)==LOW);
        selValueIndex--;
        if(selValueIndex>1)
        {
          if(selValueIndex%2==0)
          {
            i--;
          }
        }
      lcd.clear();
    }
    //---------------------------------
    //---------------------------------
    if(digitalRead(setButton)==LOW)
    {
      while(digitalRead(setButton)==LOW);
      selValueIndex++;
      if(selValueIndex>1)
      {
        if(selValueIndex%2==0)
        {
          i++;
        }
      }
      lcd.clear();
    }
    //----------------------------------
    //----------------------------------
    State = digitalRead(outputA);
    if(State!=lastState)
    {
      if(digitalRead(outputB)!=State)
      {
        selValue[selValueIndex]++;
      }
      else
      {
        selValue[selValueIndex]--;
      }
      lcd.clear();
    }
    //----------------------------------
    //----------------------------------
    if(selValueIndex==0)//set day number
    {
      if(selValue[0]>6)
      {
        selValue[0]=0;
      }
      lcd.setCursor(1,0);
      lcd.print("day number");
      lcd.setCursor(6,1);
      lcd.print(selValue[selValueIndex]+1);
    }
    else if(selValueIndex==1)//set alarm number
    {
      if(selValue[1]>19)
      {
        selValue[1]=1;
      }
      lcd.setCursor(1,0);
      lcd.print("alarm number");
      lcd.setCursor(6,1);
      lcd.print(selValue[selValueIndex]+1);
    }
    else if(selValueIndex<((selValue[1]+1)*2)+2)//set alarm 
    {   
        if(selValueIndex%2==0)        //for hour
        {
        if(selValue[selValueIndex]>24)
        {
        selValue[selValueIndex]=1;
        }
        if(myTimer-300<millis())
        {
        lcd.setCursor(0,0);lcd.print("Alarm setting");
        lcd.setCursor(0,1);lcd.print("AL:");lcd.print(i);lcd.print("=>");
        lcd.setCursor(7,1);lcd.print(selValue[selValueIndex]);lcd.setCursor(9,1);lcd.print(":");lcd.setCursor(10,1);lcd.print(selValue[selValueIndex+1]);
        }
        else
        {
        lcd.setCursor(0,0);lcd.print("Alarm setting");
        lcd.setCursor(0,1);lcd.print("AL:");lcd.print(i);lcd.print("=>");
        lcd.setCursor(7,1);lcd.print("  ");lcd.setCursor(9,1);lcd.print(":");lcd.setCursor(10,1);lcd.print(selValue[selValueIndex+1]); 
        }
        }
        else if(selValueIndex%2==1)     //for minute
        {
        if(selValue[selValueIndex]>59)
        {
        selValue[selValueIndex]=0;
        }
        if(myTimer-300<millis())
        {
        lcd.setCursor(0,0);lcd.print("Alarm setting");
        lcd.setCursor(0,1);lcd.print("AL:");lcd.print(i);lcd.print("=>");
        lcd.setCursor(7,1);lcd.print(selValue[selValueIndex-1]);lcd.setCursor(9,1);lcd.print(":");lcd.setCursor(10,1);lcd.print(selValue[selValueIndex]);
        }
        else
        {
        lcd.setCursor(0,0);lcd.print("Alarm setting");
        lcd.setCursor(0,1);lcd.print("AL:");lcd.print(i);lcd.print("=>");
        lcd.setCursor(7,1);lcd.print(selValue[selValueIndex-1]);lcd.setCursor(9,1);lcd.print(":");lcd.setCursor(10,1);lcd.print("  ");
        }
        }
     }
     else
     {
      selValueIndex=0;
      i=0;
      exitState=2;
      SettIndex=0;
      for(int x=0;x<((selValue[1]+1)*2)+2;x++)
      {
        EEPROM.write(x,selValue[x]);
      }
     }
    lastState=State;
  }
 }
}
//--------------------------------------------------------------------------------------------------------------------
// alarm checking function//
void alarm(){
  DateTime now = rtc.now();
  int dayOfWeek = now.dayOfTheWeek();
  specialDay();
//--------------------------------------------check user set alarm----------------------------------------------------    
  if(dayOfWeek==0){dayOfWeek=7;}
  if(dayOfWeek<=(EEPROM.read(0)+1))           //check user set alarm 
  {
    for(int u=0;u<=(EEPROM.read(1)+1);u++)
    {          
      if(now.hour()==EEPROM.read(j)&&now.minute()==EEPROM.read(j+1))
      {
        digitalWrite(backLight,1);          
          for (int z = 0; z < numTones; z++)
          {
            tone(speakerPin, tones[z]);
            lcd.setCursor(3,0);
            lcd.print("Alarm => ");
            lcd.print(j/2);
            delay(500);
          }
          noTone(speakerPin);
      }
      j+=2;
      if(j>(EEPROM.read(1)+1)*2){j=2;}
    }
  }
//--------------------------------------------------------------------------------------  
    lcd.setCursor(4,1);lcd.print(now.hour(), DEC);lcd.print(':');lcd.print(now.minute(), DEC);lcd.print(':');lcd.print(now.second(), DEC);
    lcd.setCursor(0,0);lcd.print(now.year(), DEC);lcd.print('/');lcd.print(now.month(), DEC);lcd.print('/');lcd.print(now.day(), DEC);
    lcd.print(" (");lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);lcd.print(")");
    delay(500);
    lcd.clear();
}
//--------------------------------------------------------------------------------------------------------------------
void specialDay()
{
DateTime now = rtc.now();  
  if(now.year()==2023 && now.month()==2 && now.day()== 17)
  {
    lcd.setCursor(16,0);    
    for(int i=0;i<10;i++)
    {
      lcd.print(anni);
      delay(500);
    }
    lcd.noAutoscroll();
    lcd.clear();         
  }      
}
void clockSetting(){
     DateTime now = rtc.now();
     clockSetVal[0]=now.minute();
     clockSetVal[1]=now.hour();
     clockSetVal[2]=now.day();
     clockSetVal[3]=now.month();
     clockSetVal[4]=now.year();
  while(exitState<1)
  {
    myTimer = millis()+600;
    DateTime now = rtc.now();
    while(myTimer>millis())
    {
      //----------------------------------------
        if(digitalRead(buttonPin)==LOW)
        {
          while(digitalRead(buttonPin)==LOW);
          selValueIndex--;
          lcd.clear();
        }
      //--------------------------------------
        if(digitalRead(setButton)==LOW)
        {
          while(digitalRead(setButton)==LOW);
          selValueIndex++;
          lcd.clear();
        }
      //--------------------------------------
        State = digitalRead(outputA);
        if(State!=lastState)
        {
          if(digitalRead(outputB)!=State)
          {
            clockSetVal[selValueIndex]++;
          }
          else
          {
            clockSetVal[selValueIndex]--;
          }
          lcd.clear();
        }
       //---------------------------------------
       if(clockSetVal[0]>59)clockSetVal[0]=0;
       if(clockSetVal[1]>24)clockSetVal[1]=1;
       if(clockSetVal[2]>31)clockSetVal[2]=1;
       if(clockSetVal[3]>12)clockSetVal[3]=1;
       if(clockSetVal[4]>2060)clockSetVal[4]=0;
       if(selValueIndex==0) //for minute
       {
          if(myTimer-300<millis())
          {
            lcd.setCursor(4,0);lcd.print(clockSetVal[1]);
            lcd.setCursor(6,0);lcd.print(':');lcd.setCursor(7,0);lcd.print(clockSetVal[selValueIndex]);
            lcd.setCursor(1,1);lcd.print(clockSetVal[4]);
            lcd.setCursor(5,1);lcd.print('/');
            lcd.setCursor(6,1);lcd.print(clockSetVal[3]);
            lcd.setCursor(8,1);lcd.print('/');
            lcd.setCursor(9,1);lcd.print(clockSetVal[2]);
          }
          else
          {
            lcd.setCursor(4,0);lcd.print(clockSetVal[1]);
            lcd.setCursor(6,0);lcd.print(':');lcd.setCursor(7,0);lcd.print("  ");
            lcd.setCursor(1,1);lcd.print(clockSetVal[4]);
            lcd.setCursor(5,1);lcd.print('/');
            lcd.setCursor(6,1);lcd.print(clockSetVal[3]);
            lcd.setCursor(8,1);lcd.print('/');
            lcd.setCursor(9,1);lcd.print(clockSetVal[2]);
          }
       }
       else if(selValueIndex==1)//for hour
       {
        if(myTimer-300<millis())
          {
            lcd.setCursor(4,0);lcd.print(clockSetVal[selValueIndex]);
            lcd.setCursor(6,0);lcd.print(':');lcd.setCursor(7,0);lcd.print(clockSetVal[0]);
            lcd.setCursor(1,1);lcd.print(clockSetVal[4]);
            lcd.setCursor(5,1);lcd.print('/');
            lcd.setCursor(6,1);lcd.print(clockSetVal[3]);
            lcd.setCursor(8,1);lcd.print('/');
            lcd.setCursor(9,1);lcd.print(clockSetVal[2]);
          }
          else
          {
            lcd.setCursor(4,0);lcd.print("  ");
            lcd.setCursor(6,0);lcd.print(':');lcd.setCursor(7,0);lcd.print(clockSetVal[0]);
            lcd.setCursor(1,1);lcd.print(clockSetVal[4]);
            lcd.setCursor(5,1);lcd.print('/');
            lcd.setCursor(6,1);lcd.print(clockSetVal[3]);
            lcd.setCursor(8,1);lcd.print('/');
            lcd.setCursor(9,1);lcd.print(clockSetVal[2]);
          }
       }
       else if(selValueIndex==2)//for day
       {
          if(myTimer-300<millis())
          {
            lcd.setCursor(4,0);lcd.print(clockSetVal[1]);
            lcd.setCursor(6,0);lcd.print(':');lcd.setCursor(7,0);lcd.print(clockSetVal[0]);
            lcd.setCursor(1,1);lcd.print(clockSetVal[4]);
            lcd.setCursor(5,1);lcd.print('/');
            lcd.setCursor(6,1);lcd.print(clockSetVal[3]);
            lcd.setCursor(8,1);lcd.print('/');
            lcd.setCursor(9,1);lcd.print(clockSetVal[selValueIndex]);
          }
          else
          {
            lcd.setCursor(4,0);lcd.print(clockSetVal[1]);
            lcd.setCursor(6,0);lcd.print(':');lcd.setCursor(7,0);lcd.print(clockSetVal[0]);
            lcd.setCursor(1,1);lcd.print(clockSetVal[4]);
            lcd.setCursor(5,1);lcd.print('/');
            lcd.setCursor(6,1);lcd.print(clockSetVal[3]);
            lcd.setCursor(8,1);lcd.print('/');
            lcd.setCursor(9,1);lcd.print("  ");
          }
       }
       else if(selValueIndex==3)//for month
       {
          if(myTimer-300<millis())
          {
            lcd.setCursor(4,0);lcd.print(clockSetVal[1]);
            lcd.setCursor(6,0);lcd.print(':');lcd.setCursor(7,0);lcd.print(clockSetVal[0]);
            lcd.setCursor(1,1);lcd.print(clockSetVal[4]);
            lcd.setCursor(5,1);lcd.print('/');
            lcd.setCursor(6,1);lcd.print(clockSetVal[selValueIndex]);
            lcd.setCursor(8,1);lcd.print('/');
            lcd.setCursor(9,1);lcd.print(clockSetVal[2]);
          }
          else
          {
            lcd.setCursor(4,0);lcd.print(clockSetVal[1]);
            lcd.setCursor(6,0);lcd.print(':');lcd.setCursor(7,0);lcd.print(clockSetVal[0]);
            lcd.setCursor(1,1);lcd.print(clockSetVal[4]);
            lcd.setCursor(5,1);lcd.print('/');
            lcd.setCursor(6,1);lcd.print("  ");
            lcd.setCursor(8,1);lcd.print('/');
            lcd.setCursor(9,1);lcd.print(clockSetVal[2]);
          }
       }
       else if(selValueIndex==4)//for year
       {
          if(myTimer-300<millis())
          {
            lcd.setCursor(4,0);lcd.print(clockSetVal[1]);
            lcd.setCursor(6,0);lcd.print(':');lcd.setCursor(7,0);lcd.print(clockSetVal[0]);
            lcd.setCursor(1,1);lcd.print(clockSetVal[selValueIndex]);
            lcd.setCursor(5,1);lcd.print('/');
            lcd.setCursor(6,1);lcd.print(clockSetVal[3]);
            lcd.setCursor(8,1);lcd.print('/');
            lcd.setCursor(9,1);lcd.print(clockSetVal[2]);
          }
          else
          {
            lcd.setCursor(4,0);lcd.print(clockSetVal[1]);
            lcd.setCursor(6,0);lcd.print(':');lcd.setCursor(7,0);lcd.print(clockSetVal[0]);
            lcd.setCursor(1,1);lcd.print("    ");
            lcd.setCursor(5,1);lcd.print('/');
            lcd.setCursor(6,1);lcd.print(clockSetVal[3]);
            lcd.setCursor(8,1);lcd.print('/');
            lcd.setCursor(9,1);lcd.print(clockSetVal[2]);
          }
       }
       else
       {
        exitState=2;
        SettIndex=0;
        selValueIndex=0;
        rtc.adjust(DateTime(clockSetVal[4], clockSetVal[3], clockSetVal[2], clockSetVal[1], clockSetVal[0], 0));
       }
      lastState=State;
    }
  }
}
//-----------------------------------------------------------------------------------------------------------------------------
