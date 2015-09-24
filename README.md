# ArduinoMidiController
This is an open-source midi-controller. More information can be found at boutsman.be

Data for the midi messages is stored in different arrays. Each button/fader gets a number that corresponds to the location in the arrays that store the data for the messages.

Should there be need for more inputs, a multiplexer an be used.

Adjustable parameters for each control:
- Midi Channel
- Midi Note

v1.1
- readAnalog(pinNr,arrayPosition)
- readDigital(pinNr,arrayPosition)

v1.2:
- midiStart(pinNr,arrayPosition)
- midiStop(pinNr,arrayPosition)
- midiContinue(pinNr,arrayPosition)

CapSenseMidiController:
- convertToBoolean(val,arrayPosition)
- readDigital(arrayPosition)

