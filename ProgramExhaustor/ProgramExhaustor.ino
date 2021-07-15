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
  
//initialize LCD
  lcd.backlight();
  lcd.begin();
  lastDisplay=0;
  currentDisplay=0;

  digitalWrite(12,HIGH);
//initializare intrari/iesiri
pinMode(8, OUTPUT);
pinMode(9, OUTPUT);
pinMode(10, OUTPUT);
pinMode(11, OUTPUT);
pinMode(12, OUTPUT);

pinMode(2, INPUT);
pinMode(3, INPUT);
pinMode(4, INPUT);
pinMode(5, INPUT);

//setare butoane
setupBtn.setDebounceTime(100);
setupBtn.setCountMode(COUNT_FALLING);

startBtn.setDebounceTime(50);
//startBtn.setCountMode(COUNT_FALLING);

stopBtn.setDebounceTime(50);
//stopBtn.setCountMode(COUNT_FALLING);

//initializare butoane
 motorCmd=0;
  
// initializare timere

//Timer1 pentru inerval pauza
timer1_preset=map(analogRead(A0), 0, 1024, 10000,300000);
timer1_dn=0;
timer1_ON=0;
timer1_acc=0;

//Timer2 pentru inerval impuls
timer2_preset=50;
timer2_dn=0;
timer2_ON=0;
timer2_acc=0;

//Timer3 pentru interval pauza intre impulsuri
timer3_preset=map(analogRead(A1), 0, 1024, 3000,120000);
timer3_dn=0;
timer3_ON=0;
timer3_acc=0;

//Pornire comunicare seriala
Serial.begin(115200);


//Activare timer arduino si intreruperi la un anumit interval de timp pentru a obtine baza de timp pentru timere
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
   //Serial.println("timer1: ");
   //Serial.println(timer1_acc);
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
    //Serial.println("timer2: ");
    //Serial.println(timer2_acc);
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
    //Serial.println("timer3: ");
    //Serial.println(timer3_acc);
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
  //start button loop reading
setupBtn.loop();
startBtn.loop();
stopBtn.loop();

//Button command actions
setupCount=setupBtn.getCount();
if(startBtn.getState()==0)
startVar=1;

if(stopBtn.getState()==0)
startVar=0;

//Curatare ecran LCD inainte de afisarea noului meniu
if(setupBtn.isPressed())
lcd.clear();

//Modul normal de lucru
if(setupCount==0)
{
  //Bucla principala de lucru a filtrului--secventiere curatare filtre--
switch(state) {
  case 0:
    // Sistem in repaus
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
     // Pauza ciclu curatare filtre
    // Serial.println("Pauza ciclu curatare filtre ");
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
    // Impuls EV1
   // Serial.println("Impuls EV1 ");
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
    // Pauza EV1
   // Serial.println("Pauza EV1 ");
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
     // Impuls EV2
    // Serial.println("Impuls EV2 ");
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
    // Pauza EV2
  //  Serial.println("Pauza EV2 ");
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
    // Impuls EV3
   // Serial.println("Impuls EV3 ");
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
    // Pauza EV3
   // Serial.println("Pauza EV3 ");
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
    // Impuls EV4
   // Serial.println("Impuls EV4 ");
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
    // Pauza EV4, reinitializare ciclu curatare
   // Serial.println("Reluare");
     ev4Cmd=1;
     if(startVar==0)
     state=1;
     else 
     state=0;
    break;
   
}

//Afisare date mod de lucru normal pe display
  normalModeDisplay();

}

 //Modul setare 
else
{
  
  timer1_preset=map(analogRead(A0), 0, 1024, 10000,300000);
  timer3_preset=map(analogRead(A1), 0, 1024, 3000,120000);

//set display Setup mode
  setupModeDisplay();

 }

//Actionare iesiri Arduino
actionareIesiri();

 //Resetare counter buton= trecere in modul normal de lucru
  if(setupCount>=2)
  setupBtn.resetCount();


}

//Actionare iesiri Arduino
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
//  afisare timp pauza ciclu 
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

//  Afisare display la x ms informatiile ce urmeaza a fi setate
currentDisplay=millis();
 if(currentDisplay-lastDisplay>100)
 {
//  afisare timp pauza ciclu 
 lcd.setCursor(0,0);
 lcd.print("Tpc:");
 lcd.setCursor(4,0);
 lcd.print((float)timer1_preset/1000);
 lcd.setCursor(10,0);
 lcd.print("s");
//  afisare timp pauza ventil
 lcd.setCursor(0,1);
 lcd.print("Tpv:");
 lcd.setCursor(4,1);
 lcd.print((float)timer3_preset/1000);
 lcd.setCursor(9,1);
 lcd.print("s");
  
 lastDisplay=currentDisplay;
 }
 
  }
