/*
* 
* Project: Arduino MIDI Control 
* Version: V1.2
* Author: Stijn Boutsen (Boutsman.be)
* Author: Wouter Vandenneucker (woutervddn@techprojectmasters.com)
* Date: 08/08/2015
* 
* Description:
* The "Arduino MIDI Control" sketch enables you to have a bunch of input devices, that are connected to the arduino's analog inputs, that controls
* by sending out MIDI messages over USB.
* 
* Instructions: 
*      1: Define inputs
*      2: Change the methods in "void loop()"         
*
* Notes:
* Orginal code from: https://sites.google.com/site/bharatbhushankonka/home/diy-midi-over-usb-using-arduino-uno
* Has some quirks though
*
* Possible updates in the future: 
*    Integrate LCD for selecting the note played, change midi-channel, ... .   
*    Integrate rotary encoder with pushbutton for going through menus on the LCD
*    Integrate same functionality as the Akai mpd 24
*              
*         LCD circuit:  (not in this version!)
*           LCD RS pin to digital pin 10
*           LCD Enable pin to digital pin 9
*           LCD D4 pin to digital pin 8
*           LCD D5 pin to digital pin 7
*           LCD D6 pin to digital pin 6
*           LCD D7 pin to digital pin 4
*           LCD R/W pin to ground
*           10K resistor:
*           ends to +5V and ground
* 
* 
*/

#define MIDI_COMMAND_CONTROL_CHANGE 0xB0  //
#define MIDI_COMMAND_NOTE_ON 0x90         //
#define MIDI_COMMAND_NOTE_OF 0x80         //

#define MIDI_POLYAFTERTOUCH 0xA0          //
#define MIDI_CHANAFTERTOUCH 0xD0          //
#define MIDI_PROGCHANGE 0xC0              //1100 0000
#define MIDI_PITCHWHEEL 0xE0              //1110 0000

#define MIDI_START 0xfa                   //1111 1010
#define MIDI_STOP 0xfc                    //1111 1100
#define MIDI_CLOCK 0xf8                   //1111 1000
#define MIDI_CONTINUE 0xfb                //1111 1011
#define MIDI_RESET 0xff                   //1111 1111
#define MIDI_ACTIVESENS 0xfe              //1111 1110

//Stored channel
//int chan[16] = {0, 0, 0, 0, 1, 1, 1, 1, 3, 4, 5, 6, 9, 0, 1, 2};  //Array for storing the channel of each individual control.
int chan[16] = {0, 0, 0, 0, 1, 1, 1, 1, 3, 4, 5, 6, 9, 0, 1, 2};  //Array for storing the channel of each individual control.

//Stored notes & last used note.
//int note[16] = {48, 49, 50, 51, 52, 53, 54, 55, 7, 7, 7, 7, 7, 7, 7, 7};  //Array for storing the note of each individual controll.   Don't mind the weird order of the numbers, that's just for my personal midi-setup, you can arrange them just as you see fit.
int note[16] = {48, 49, 50, 51, 52, 53, 54, 55, 7, 7, 7, 7, 7, 7, 7, 7};

//Analog (8 potentiometers)
//Digital (8 pushbuttons)
//so 16 values
int val[16];        //Array containing all the momentary values of both the analoge and digital controlls
int lastVal[16];    //Array for storing the last value of each individual controll

// the format of the message to send Via serial 
typedef union {
    struct {
     uint8_t command;
     uint8_t note;
     uint8_t data;
    } 
    msg;
    uint8_t raw[3];
} 
t_midiMsg;

t_midiMsg midiMsg;

void setup() {  
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  Serial.begin(115200); //31250
  
  delay(1000);
}

void loop() {  
    //midiStart(2, 0); //(inputpin, value number)    
    //midiStop(3, 1);
    //midiContinue(4, 2);
    //midiClock(5, 3);
    readDigital(2,0);
    readDigital(3,1);
    readDigital(4,2);
    readDigital(5,3);
    readDigital(6, 4);
    readDigital(7, 5);
    readDigital(8, 6);
    readDigital(9, 7);
    readAnalog(A0,8);
    readAnalog(A1,9);
    readAnalog(A2,10);
    readAnalog(A3,11);
    readAnalog(A4,12);
    readAnalog(A5,13);
    readAnalog(A6,14);
    readAnalog(A7,15);
}


void readAnalog(int pin, int n)  
{
      //Analoge Values
      val[n] = map(analogRead(pin), 0, 1023, 0, 127);
    
      // check if analog input has changed
      if (val[n] != lastVal[n]) {
           // update lastval variable
           lastVal[n] = val[n];
           
           //send control change on cc#i
           midiMsg.msg.command = MIDI_COMMAND_CONTROL_CHANGE+chan[n];
           midiMsg.msg.note    = note[n];
           midiMsg.msg.data    = val[n];
           
           
           /* Send note on */
           Serial.write(midiMsg.raw, sizeof(midiMsg));                          
     }
}

void readDigital(int mux, int n)
{
  //Digital Values 1
  val[n] = digitalRead(mux);

  if(val[n] == 1){
     if(lastVal[n] == 0){
        midiMsg.msg.command = MIDI_COMMAND_NOTE_ON+chan[n];
        midiMsg.msg.note    = note[n];  
        midiMsg.msg.data    = 127;

        /* Send note on */
        Serial.write(midiMsg.raw, sizeof(midiMsg));        
     }
  }
  if(val[n] == 0){
     if(lastVal[n] == 1){
        midiMsg.msg.command = MIDI_COMMAND_NOTE_OF+chan[n];
        midiMsg.msg.note    = note[n];  
        midiMsg.msg.data    = 0;
            
        /* Send note off */
        Serial.write(midiMsg.raw, sizeof(midiMsg));
     }
  }
  lastVal[n] = val[n];     
}    

void midiStart(int mux, int n)
{
  //Digital Values 1
  val[n] = digitalRead(mux);

  if(val[n] == 1){
     if(lastVal[n] == 0){
        Serial.write(MIDI_START);        
     }
  }
  lastVal[n] = val[n];     
}

void midiStop(int mux, int n)
{
  //Digital Values 1
  val[n] = digitalRead(mux);

  if(val[n] == 1){
     if(lastVal[n] == 0){
        Serial.write(MIDI_STOP);        
     }
  }
  lastVal[n] = val[n];     
}

void midiClock(int mux, int n)
{
  //Digital Values 1
  val[n] = digitalRead(mux);

  if(val[n] == 1){
     if(lastVal[n] == 0){
        Serial.write(MIDI_CLOCK);        
     }
  }
  lastVal[n] = val[n];     
}

void midiContinue(int mux, int n)
{
  //Digital Values 1
  val[n] = digitalRead(mux);

  if(val[n] == 1){
     if(lastVal[n] == 0){
        Serial.write(MIDI_CONTINUE);        
     }
  }
  lastVal[n] = val[n];     
}
