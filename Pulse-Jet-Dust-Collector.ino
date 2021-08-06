#include <ezButton.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Initialize program variables
long unsigned int timer1_acc,timer1_dn, timer1_preset,timer1_ON,  timer2_acc,timer2_dn, timer2_preset,timer2_ON,  timer3_acc,timer3_dn, timer3_preset,timer3_ON;
int state,motorCmd,ev1Cmd,ev2Cmd,ev3Cmd,ev4Cmd,setupCount,startCount,stopCount,startVar;

unsigned int lastDisplay, currentDisplay;
ezButton setupBtn(2);
ezButton startBtn(3);
ezButton stopBtn(4);

void setup() {
  
//Initialize LCD
  lcd.backlight();
  lcd.begin();
  lastDisplay=0;
  currentDisplay=0;

  digitalWrite(12,HIGH);
//Initilize DI/DO
pinMode(8, OUTPUT);
pinMode(9, OUTPUT);
pinMode(10, OUTPUT);
pinMode(11, OUTPUT);
pinMode(12, OUTPUT);

pinMode(2, INPUT);
pinMode(3, INPUT);
pinMode(4, INPUT);
pinMode(5, INPUT);

//Buttons setup
setupBtn.setDebounceTime(100);
startBtn.setDebounceTime(50);
stopBtn.setDebounceTime(50);


//Initialize motor command value
 motorCmd=0;
  
// Initialize timers

//Timer1 used for pause interval 
timer1_preset=map(analogRead(A0), 0, 1024, 10000,300000);
timer1_dn=0;
timer1_ON=0;
timer1_acc=0;

//Timer2 used for pulse interval 
timer2_preset=50;
timer2_dn=0;
timer2_ON=0;
timer2_acc=0;

//Timer3 used for pause interval between pulses 
timer3_preset=map(analogRead(A1), 0, 1024, 3000,120000);
timer3_dn=0;
timer3_ON=0;
timer3_acc=0;

//Start serial communication
Serial.begin(115200);


//Activating Arduino timer and interrupts, obtaining a base for time
TCCR1A=0;
TCCR1B=0;
OCR1A=62;
TCCR1B |= (1<<CS12) | (1<<WGM12);  //prescaler 256, CTC Mode
TIMSK1 |= (1<<OCIE1A);  //enable OCR1A
}

ISR(TIMER1_COMPA_vect){

  //Core of timer1
  if(timer1_acc<timer1_preset&&timer1_ON&&timer1_dn==0)
{

  timer1_acc+=1;
   
  
}  
else
  if(timer1_acc>=timer1_preset) 
  timer1_dn=1;
  else
  timer1_dn=0;
  
  if(!timer1_ON)
  timer1_acc=0;

  //Core of timer2
 if(timer2_acc<timer2_preset&&timer2_ON&&timer2_dn==0)
{  
  timer2_acc+=1;
   
} 
else
  if(timer2_acc>=timer2_preset) 
  timer2_dn=1;
  else 
  timer2_dn=0;
 
  if(!timer2_ON)
  timer2_acc=0;

  //Core of timer3
 if(timer3_acc<timer3_preset&&timer3_ON&&timer3_dn==0)
{ 
  timer3_acc+=1;
} 
else
  if(timer3_acc>=timer3_preset) 
  timer3_dn=1;
  else 
  timer3_dn=0;
       
  if(!timer3_ON)
  timer3_acc=0;
}


void loop() {
  //Initialize buttons value reading
setupBtn.loop();
startBtn.loop();
stopBtn.loop();

//Take action based on buttons state
setupCount=setupBtn.getCount();
if(startBtn.getState()==0)
startVar=1;

if(stopBtn.getState()==0)
startVar=0;

//Clearing LCD screen before showing the new menu
if(setupBtn.isPressed())
lcd.clear();

//Normal working mode
if(setupCount==0)
{
  
  //Sequencing filters cleaning
switch(state) {
  case 0:
    // Equipment stop 
    motorCmd=0;
    ev1Cmd=0;
     ev2Cmd=0;
      ev3Cmd=0;
       ev4Cmd=0;

       timer1_ON=0;
       timer2_ON=0;
       timer3_ON=0;
       
       if(startVar)
       state=1;
         break;
    
      case 1:
     // Filter cleaning cycle pause 
     motorCmd=1;
     ev1Cmd=0;
     ev2Cmd=0;
     ev3Cmd=0;
     ev4Cmd=0;

     timer1_ON=1;
     if (timer1_dn)
     {
      timer1_ON=0;
      state=2;
     }
    if(startVar==0)
       state=0;
       
     break;
    
      case 2:
    // Pulse EV1(SelenoidValve1)
    ev1Cmd=1;
    timer2_ON=1;
    if(timer2_dn)
    {
    state=3;
    timer2_ON=0;
    }
    if(startVar==0)
     state=0;
    break;
    
      case 3:
    // Pause EV1
    ev1Cmd=0;
    timer3_ON=1;
    if(timer3_dn)
    {
      timer3_ON=0;
      state=4;
      }
    if(startVar==0)
      state=0;
    break;
    
      case 4:
     // Pulse EV2
    ev2Cmd=1;
    timer2_ON=1;
    if(timer2_dn)
    {
    state=5;
    timer2_ON=0;
    }
    if(startVar==0)
      state=0;
    break;
    
      case 5:
    // Pause EV2
      ev2Cmd=0;
    timer3_ON=1;

    if(timer3_dn)
    {
      timer3_ON=0;
      state=6;
      }
    if(startVar==0)
      state=0;
    break;
    
      case 6:
    // Pulse EV3
      ev3Cmd=1;
    timer2_ON=1;
    if(timer2_dn)
    {
    state=7;
    timer2_ON=0;
    }
    if(startVar==0)
      state=0;
    break;
    
      case 7:
    // Pause EV3
      ev3Cmd=0;
    timer3_ON=1;

    if(timer3_dn)
    {
      timer3_ON=0;
      state=8;
      }     
    if(startVar==0)
      state=0;
    break;
    
      case 8:
    // Pulse EV4
      ev4Cmd=1;
    timer2_ON=1;
    if(timer2_dn)
    {
    state=9;
    timer2_ON=0;
    }
    if(startVar==0)
      state=0;
    break;

      case 9:
    // Pause EV4, reinitialising cleaning cycle
     ev4Cmd=1;
     if(startVar==0)
     state=1;
     else 
     state=0;
    break;
   
}

//Show on LCD data from normal working mode
  normalModeDisplay();

}

 //Enter in settings mode
else
{
  
  timer1_preset=map(analogRead(A0), 0, 1024, 10000,300000);
  timer3_preset=map(analogRead(A1), 0, 1024, 3000,120000);

//Set display to show data from settings mode
  setupModeDisplay();

 }

//Set Arduino outputs
actionareIesiri();

 //Return to normal working mode
  if(setupCount>=2)
  setupBtn.resetCount();


}

//Set Arduino outputs
void actionareIesiri()

{
  if(ev1Cmd)
digitalWrite(8,HIGH);
else
digitalWrite(8,LOW);

if(ev2Cmd)
digitalWrite(9,HIGH);
else
digitalWrite(9,LOW);

if(ev3Cmd)
digitalWrite(10,HIGH);
else
digitalWrite(10,LOW);

if(ev4Cmd)
digitalWrite(11,HIGH);
else
digitalWrite(11,LOW);

if(motorCmd)
digitalWrite(12,LOW);
else
digitalWrite(12,HIGH);

 Serial.println(startVar);
  }


void normalModeDisplay()
{

currentDisplay=millis();

 if(currentDisplay-lastDisplay>100)
 {
// Show cycle pause time 
 lcd.setCursor(0,0);
 lcd.print("Urmeaza");
 lcd.setCursor(8,0);
 
 switch(state)
 {
 case 0:
 lcd.print("EV1");
 lcd.setCursor(0,1);
 lcd.print("In");
 lcd.setCursor(3,1);
 lcd.print((float)(timer1_preset-timer1_acc)/1000);
 lcd.setCursor(9,1);
 lcd.print("s");
 break;

 case 1:
 
 lcd.print("EV1");
 lcd.setCursor(0,1);
 lcd.print("In");
 lcd.setCursor(3,1);
 lcd.print((float)(timer1_preset-timer1_acc)/1000);
 lcd.setCursor(9,1);
 lcd.print("s");
 break;

 case 3:

 lcd.print("EV2");
 lcd.setCursor(0,1);
 lcd.print("In");
 lcd.setCursor(3,1);
 lcd.print((float)(timer3_preset-timer3_acc)/1000);
 lcd.setCursor(9,1);
 lcd.print("s");
 break; 

  case 5:
 lcd.print("EV3");
 lcd.setCursor(0,1);
 lcd.print("In");
 lcd.setCursor(3,1);
 lcd.print((float)(timer3_preset-timer3_acc)/1000);
 lcd.setCursor(9,1);
 lcd.print("s");
 break;

   case 7:
 lcd.print("EV4");
 lcd.setCursor(0,1);
 lcd.print("In");
 lcd.setCursor(3,1);
 lcd.print((float)(timer3_preset-timer3_acc)/1000);
 lcd.setCursor(9,1);
 lcd.print("s");
 break;
  
  }
  
 lastDisplay=currentDisplay;
 }
  
  }

  void setupModeDisplay()
{

// Set time for display refresh
currentDisplay=millis();
 if(currentDisplay-lastDisplay>100)
 {
//  Show cycle pause time
 lcd.setCursor(0,0);
 lcd.print("Tpc:");
 lcd.setCursor(4,0);
 lcd.print((float)timer1_preset/1000);
 lcd.setCursor(10,0);
 lcd.print("s");
//  Show selenoid valve pause time 
 lcd.setCursor(0,1);
 lcd.print("Tpv:");
 lcd.setCursor(4,1);
 lcd.print((float)timer3_preset/1000);
 lcd.setCursor(9,1);
 lcd.print("s");
  
 lastDisplay=currentDisplay;
 }
 
  }
