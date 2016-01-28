/*
 * 
 *  try print [!nxt13] [!on13] [!off13] [!termo] [!tmp1]  [!nxt9]
  
 */


 //#include "boardOne.c"
 //#include "boardTwo.c"
 #include "boardThree.c"
 //#define debug_serial 10,11
 
int ready = 0; // async reports starts after [!hello]


int SCAN=A3; // scan this pin on input change

int d=1;// 2 for 8mhz

#ifdef ir_pin
#include <IRremote.h>
IRrecv irrecv(ir_pin);
decode_results results;
#endif


#ifdef lcd_HD44780
#ifndef lcd_size
#define lcd_size 16,2
#endif

#include <LiquidCrystal.h>
LiquidCrystal lcd( lcd_HD44780 );

#endif

#ifdef rtc_DS1307
#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 rtc;
#endif

#ifdef rf_recv
#include <RCSwitch.h>
RCSwitch rfRecv = RCSwitch();
#endif

#ifdef rf_send
RCSwitch rfSend = RCSwitch();
#endif 

#ifdef debug_serial

#include <SoftwareSerial.h>
SoftwareSerial dbgSerial(debug_serial); // RX, TX

void debug(String A) {
  dbgSerial.println(A);
}
#else

#define debug(A)

#endif

Stream *w; // open wrt console (on mega ->Serial2)
Stream *dbg;; // debug console (on mega -> Serial3)

void out(String txt) {
  Serial.print("# "); // for not interact with linux console
  Serial.println(txt);
  debug(txt);
}

#ifdef dht11
#include <DHT.h>
DHT dht(dht11,DHT11); // define PIN & TYPE
#endif

void setup()  
{
    pinMode(13, OUTPUT); 
    pinMode(SCAN,INPUT);
  // Open serial communications and wait for port to open:
  
  // Mega Here
   //Serial2.begin(57600); w=&Serial2; // as HardwareSerial implements abstact stream
   //Serial3.begin(57600); dbg=&Serial3;  // on screen -PC USB
    
   Serial.begin(bridgeSpeed); w=&Serial; // openwrt connected on hardware serial

   #ifdef debug_serial
   
   dbgSerial.begin(9600); dbg=&dbgSerial; // if compile with 8mhz - use 19200
   #endif

   #ifdef rf_recv
       rfRecv.enableReceive(rf_recv);  // Receiver onpin2
   #endif
  
   #ifdef rf_send 
       rfSend.enableTransmit(rf_send); // out pin. [!rf12345]
   #endif

   #ifdef dht11
   dht.begin();
   #endif

   #ifdef rtc_DS1307
   rtc.begin();
   //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
   #endif

   #ifdef lcd_HD44780
    lcd.begin(lcd_size);
    lcd.setCursor(0,0);
    lcd.print(boardName); // print board name after reboot
   #endif

   
#ifdef ir_pin
   irrecv.enableIRIn(); 
#endif


  
  //out("Hello " boardName " is here ");
}

void serialResend(Stream *s1, Stream *s2) {
  
  if (s1->available()) s2->write(s1->read());
  if (s2->available()) s1->write(s2->read());
  
}

char buf[80]; int len=0; int collect=0;

int on=0;
void swap_light() {
  //on++;  if (on>2) on=0;
  on=!on; 
  //on=1;
  if (on)   digitalWrite(13,HIGH);
    else  digitalWrite(13,LOW);
}

void do_gpio(char *Pin,int state) {
 int pin=atoi(Pin);
 pinMode(pin,OUTPUT);
 if (state == 2) state=(digitalRead(pin)==HIGH)?LOW:HIGH; // toggle
 digitalWrite(pin,state?HIGH:LOW);
// /out("gpio:"+String(pin)+" is "+String(state));
}

void w_put(char *b) {
  while(*b) {w->write(*b);b++;}
}

void do_report(String cmd) {
char buf[20];
  w_put("\n/usr/bin/ard rep ");
  cmd.getBytes((unsigned char*)buf,sizeof(buf));
  w_put(buf);
  w_put("\n");
}

#ifdef termo

/** Steihart-hart formulae  */

double Therm(int RawADC) {  //Function to perform the fancy math of the Steinhart-Hart equation
  //[!termo]
 double Temp;
 out("RAW-ACD:"+String(RawADC));  // 498 -> 26.16 but - same on 3.3v ADC=650, 12.94
 #ifdef thermo_pulldown
  Temp = log(((10240000/RawADC) - 10000)); // Pull -down
 #else
   Temp =log(10000.0/(1024.0/RawADC-1)) ; // Pull-up
#endif

 Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
 Temp = Temp - 273.15;              // Convert Kelvin to Celsius
 //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Celsius to Fahrenheit - comment out this line if you need Celsius
 return Temp;
}

 
void do_temp(int pin) {
 int p[2]={termo} ; //termo; // pins
  double T ;//= Therm(analogRead(pin));
  int cnt = sizeof(p)/sizeof(p[0]);
  if ( (pin<0) || (pin>=cnt )) { // out-of-bound
      T=-1;
     } else
  T = Therm(analogRead( p[pin] ));
   // Serial.println(T);
  //int t = 100*T;
  //T=t/100.;
  //out("T:"+String(t));  
  do_report("termo "+String(T));
}

#endif

char buf2[20];

void do_command(char *cmd) { // command processor
  
//out(">>>> command: "); 
  out(cmd);
if (memcmp(cmd,"hello",5)==0) {
   ready=1;
   do_report("hello " boardName); // report a board
   }
else if (memcmp(cmd,"on",2)==0)  do_gpio(cmd+2,1);
else if (memcmp(cmd,"off",3)==0) do_gpio(cmd+3,0);
else if (memcmp(cmd,"nxt",3)==0) do_gpio(cmd+3,2);
#ifdef  rtc_DS1307
else if (memcmp(cmd,"now",3)==0) {
   DateTime now = rtc.now();
 
   sprintf(buf2,"now %04d%02d%02d_%02d%02d%02d",now.year(),now.month(),now.day(),now.hour(),now.minute(),now.second());
   // sprintf(buf2,"now %02d%02d%02d",now.hour(),now.minute(),now.second());
   //do_report("now very_long_string_buf2");
   do_report(buf2);
   //do_report("now ZU");
   }
else if (memcmp(cmd,"dt=",3)==0) { // set date exactly "YYYYMMDD hhmmss"
   cmd+=3;
   int ss=atoi(cmd+13); cmd[13]=0;
   int mm=atoi(cmd+11); cmd[11]=0;
   int hh=atoi(cmd+9);  cmd[8]=0;
   int DD=atoi(cmd+6);  cmd[6]=0;
   int MM=atoi(cmd+4);  cmd[4]=0;
  // do_report("REST"+ String(cmd));
   int YY=atoi(cmd);
   rtc.adjust(DateTime(YY,MM,DD,hh,mm,ss));
}
#endif
#ifdef lcd_HD44780
else if (memcmp(cmd,"lcd",3)==0) { // lcd<LINE>text...
  cmd+=3; buf2[0]=cmd[0]; buf2[1]=0; int x = atoi(buf2); cmd+=1; // rest is a text
  buf2[0]=cmd[0]; buf2[1]=0; int  y = atoi(buf2); cmd+=1; // rest is a text
  lcd.setCursor(x, y );
  lcd.print(cmd); //print a rest
  }
#endif

#ifdef termo
else if (memcmp(cmd,"termo",5)==0) do_temp(atoi(cmd+5));
#endif
#ifdef rf_send
else if (memcmp(cmd,"rf",2)==0) {
    long code = atol(cmd+2);
    debug("RFSEND:"+String(code));
    rfSend(code,24);
    
    } 
#endif
#ifdef dht11
else if (memcmp(cmd,"dht",3)==0) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    do_report("dht "+String(t)+" "+String(h));
    }
#endif
else { 
   debug("unknown cmd "+String(cmd));
}    
}

int mode=-1;

void loop() // run over and over
{
#ifdef ir_pin
 if (irrecv.decode(&results)) {
    sprintf(buf2,"%lX",results.value);
    if (ready)  do_report("ir "+String(buf2)); // only after [!hello]
//    /Serial.println(results.value, HEX);
   // dump(&results);
    irrecv.resume(); // Receive the next value
  }
#endif
  int imode = digitalRead(SCAN)==HIGH; if (imode!=mode) {
    mode=imode;
    // out("ScanChanged:"+String(mode)); --> report for buttons
  }
  //serialResend(&mySerial,&Serial);
//  serialResend(w,w);
//serialResend(&Serial2,&Serial1); // transparent send

//serialResend(&Serial3,&Serial2); // transparent send
 
 //serialResend(&Serial,&Serial); //echo
 //swap_light(); delay(1000);


  int l = w->available();
  if (l>0) { // have out of wrt
    unsigned char ch=w->read();
    if (dbg) dbg->write(ch);  // resend
    //if (ch == '1') digitalWrite(13,HIGH);
    //if (ch == '0') digitalWrite(13,LOW);
    if ((ch == ']') && collect) {
        //swap_light();
        if (len>0) {
         //   swap_light();
            //if ( buf[0] == '!') swap_light();
            if (buf[0]=='!') {
                 do_command(buf+1);
                 /*if (dbg) {
                 dbg->print("try_command->"); // debug it
                 dbg->println(buf);
                 }
                 */
                } 
            //mySerial.print("#found  command: ");
            //mySerial.write(buf,len); // emu read from a user
            //smySerial.println();
            }
      len=0; buf[0]=0; collect=0; // stop collect
      } else {
        if (collect && (len+1 <sizeof(buf))) { buf[len]=ch; len++; buf[len]=0;   }
      }
    if (ch == '[') collect=1; // start collect
      
    // mySerial.write('[');    mySerial.write(ch);    mySerial.write(']'); //echo
    //Serial.write('*');
   
    //Serial.println("\nls -la\n");
    }
  if (dbg && dbg->available()) { // human debug input
    unsigned char ch = dbg->read();
    //if (ch=='*') swap_light();
    w->write(ch);
  }

#ifdef rf_recv
  if (rfRecv.available()) {
    int value = rfRecv.getReceivedValue();
    if (value == 0) {
     debug("rfRecv: Unknown encoding");
    } else {
      debug("rfRecv: Received ");
      unsigned long code=rfRecv.getReceivedValue();
      if (ready) do_report("rf_recv "+String(code));
      /*
      Serial.print( rfRecv.getReceivedValue() );
      Serial.print(" / ");
      Serial.print( rfRecv.getReceivedBitlength() );
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println( rfRecv.getReceivedProtocol() );
      */
    }
    rfRecv.resetAvailable();
  }
#endif
  
   


}

