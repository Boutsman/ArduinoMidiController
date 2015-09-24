/**
* @author Stijn Boutsen
* @link boutsman.be
*
* Notes:
* Orginal code from: https://sites.google.com/site/bharatbhushankonka/home/diy-midi-over-usb-using-arduino-uno
* Has some quirks though
* 
*/

#include <CapacitiveSensor.h>
#define SENDPIN 13
#define TRESHOLD 40 //Sensitivity treshold for the capacitive surfaces
#define READPIN0 2  //Pin for btn1
#define READPIN1 4  //Pin for btn2
#define READPIN2 6  //Pin for btn3
#define READPIN3 8  //Pin for btn4

#define MIDI_COMMAND_NOTE_ON 0x90
#define MIDI_COMMAND_NOTE_OF 0x80

const int aantalKnoppen = 4; //Aanpassen naargelang aantal knoppen

CapacitiveSensor   cs_2_0 = CapacitiveSensor(SENDPIN,READPIN0);        // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
CapacitiveSensor   cs_2_1 = CapacitiveSensor(SENDPIN,READPIN1);
CapacitiveSensor   cs_2_2 = CapacitiveSensor(SENDPIN,READPIN2);
CapacitiveSensor   cs_2_3 = CapacitiveSensor(SENDPIN,READPIN3);

//Array for storing the notes
//Each button corresponds to a specific midi-note
int note[aantalKnoppen] = {40, 41, 42, 43};  

//Values of the Buttons
int val[aantalKnoppen];
int lastVal[aantalKnoppen];
int toets[aantalKnoppen];
int chan[aantalKnoppen] = {0,0,0,0};

// the format of the message to send Via serial 
typedef union {
    struct {
 uint8_t command;
 uint8_t note;
 uint8_t data;
    } msg;
    uint8_t raw[3];
} t_midiMsg;

t_midiMsg midiMsg; //MIDI message for Faders/pots 
t_midiMsg midiMsg2; //MIDI message for Buttons

void setup() {
  //Pin setup
  pinMode(READPIN0, INPUT);
  pinMode(READPIN1, INPUT);
  pinMode(READPIN2, INPUT);
  pinMode(READPIN3, INPUT);
  
  //Autocallibration
  cs_2_0.set_CS_AutocaL_Millis(0xFFFFFFFF);
  cs_2_1.set_CS_AutocaL_Millis(0xFFFFFFFF);
  cs_2_2.set_CS_AutocaL_Millis(0xFFFFFFFF);
  cs_2_3.set_CS_AutocaL_Millis(0xFFFFFFFF);

  //Start Serial Port
  Serial.begin(115200);
}

void loop(){    
  //inlezen van 4 drukknoppen
  long temp[4];
  temp[0] = cs_2_0.capacitiveSensor(30);
  temp[1] = cs_2_1.capacitiveSensor(30);
  temp[2] = cs_2_2.capacitiveSensor(30);
  temp[3] = cs_2_3.capacitiveSensor(30);
  
  for(int i=0;i<aantalKnoppen;i++)
  {
    convertToBoolean(temp[i], i);
    readDigital(i); //(inputPin, number of the button)
  }
}

void readDigital(int btn)
{
  //Digital Values
  val[btn] = toets[btn];

  if(val[btn] == 1){
     if(lastVal[btn] == 0){
       //Send status byte (status + messageType + channel)
        midiMsg2.msg.command = MIDI_COMMAND_NOTE_ON+chan[btn];
        //Send Data byte 1
        midiMsg2.msg.note    = note[btn];
        //Send Data byte 2
        midiMsg2.msg.data    = 127;

        /* Send note on */
        Serial.write(midiMsg2.raw, sizeof(midiMsg2));        
     }
  }
  if(val[btn] == 0){
     if(lastVal[btn] == 1){
        midiMsg2.msg.command = MIDI_COMMAND_NOTE_OF+chan[btn];
        midiMsg2.msg.note    = note[btn];
        midiMsg2.msg.data    = 0;
            
        /* Send note off */
        Serial.write(midiMsg2.raw, sizeof(midiMsg2));
     }
  }
  lastVal[btn] = val[btn];     
}

void convertToBoolean(int capsWaarde, int nr)
{
  if(capsWaarde > TRESHOLD)
  {
    toets[nr] = 1;
  }
  else
  {
    toets[nr] = 0;
  }
}
