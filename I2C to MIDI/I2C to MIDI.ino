/*
 Name:		I2C_to_MIDI.ino
 Created:	4/2/2019 11:27:38 AM
 Author:	Stoodio 2nd
*/

#include <Adafruit_NeoPixel.h>
#define ledPin 4
#define ledStrip 5
#define portSelectPin 5
#define MIDI_IN 8
#define MIDI_OUT 9

#include <SoftwareSerial.h>
#include <MIDI.h>
#include <Wire.h>
SoftwareSerial softSerial(MIDI_IN, MIDI_OUT);
MIDI_CREATE_DEFAULT_INSTANCE();
MIDI_CREATE_INSTANCE(SoftwareSerial, softSerial, softMidi);

unsigned long int HWnoteTimers[127] = { 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0 };
unsigned long int SWnoteTimers[127] = { 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0 };
bool HWnoteIsOn[127] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
bool SWnoteIsOn[127] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
int midiOnTime = 30;

#define NUMPIXELS 12
#define neoPin 10

Adafruit_NeoPixel pixels(NUMPIXELS, neoPin, NEO_GRB + NEO_KHZ800);

void setup() {
	
	pinMode(ledPin, OUTPUT);
	pinMode(ledStrip, OUTPUT);
	digitalWrite(ledPin, HIGH);
	MIDI.begin(MIDI_CHANNEL_OMNI);
	softMidi.begin(MIDI_CHANNEL_OMNI);
	pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
	Wire.begin(0); // Initiate the Wire library and listen to adress #0
		Wire.onReceive(receiveEvent); // register event
		//delay(100);
		//Wire.write(8);
		//Wire.endTransmission();
		for (int i = 0; i < 3; i++) {
			digitalWrite(ledPin, HIGH);
			delay(100);
			digitalWrite(ledPin, LOW);
			delay(100);
		}
		
}

// the loop function runs over and over again until power down or reset
void loop() {
	//delay(1);
	checkLedStrip(); //remember to turn off ledstrip pin
	handleNoteOffs(); //turn off notes
}

int ledStripPulseDuration = 20;
unsigned long int ledStripOnTime = 0;
bool ledStripIsOn = false;
void checkLedStrip() {
	unsigned long int now = millis();
	if (now > ledStripOnTime + ledStripPulseDuration && ledStripIsOn) {
		digitalWrite(ledStrip, 0);
		ledStripIsOn = false;
	}
}

void triggerLedStrip() {
	digitalWrite(ledStrip, 1);
	ledStripOnTime = millis();
	ledStripIsOn = true;
}

void receiveEvent(int howMany) {
	digitalWrite(ledPin, HIGH);
	while (1 < Wire.available()) { // loop through all but the last
		char c = Wire.read(); // receive byte as a character
		handle(c);
	}
	int x = Wire.read();    // receive byte as an integer
	triggerLedStrip();
	handle(x);
	digitalWrite(ledPin, LOW);
}

void handle(int whot) {
	int port = 0;
	Serial.print("port ");
	//int port = digitalRead(portSelectPin);
	if (whot >= 127) {
		port = 1;
	}
	else {
		port = 0;
	}
	//Serial.println(port);
	//Serial.print(" selecter   note = ");
	//Serial.println(whot);
	//if (digitalRead(portSelectPin) == HIGH) {
	




	if (whot >= 127) {
		int tempWhot = whot - 127;
		MIDI.sendNoteOn(tempWhot , 127, 1);
		pixels.setPixelColor(tempWhot%12,pixels.Color(255, 0, 255));
		pixels.show();   // Send the updated pixel colors to the hardware.
		HWnoteIsOn[tempWhot] = true;
		HWnoteTimers[tempWhot] = millis();
		//Serial.println(" HWSERIAL ");
		//delay(10);
	//	softMidi.sendNoteOff(whot-127 , 127, 1);
	}
	else {
		softMidi.sendNoteOn(whot , 127, 1);
		pixels.setPixelColor(whot % 12, pixels.Color(0, 255, 0));
		pixels.show();   // Send the updated pixel colors to the hardware.
		SWnoteIsOn[whot] = true;
		SWnoteTimers[whot] = millis();
		//Serial.println(" SWSERIAL ");
//		MIDI.sendNoteOff(whot , 127, 1);
		
	}
}



void handleNoteOffs() {
	for (int i = 0; i < 127; i++) {
		if (HWnoteIsOn[i]) {												//if the note is on
			if (millis() > (HWnoteTimers[i] + midiOnTime)) {						//if was turned on more than 30 ms ago
				MIDI.sendNoteOff(i , 127, 1);
				HWnoteIsOn[i] = false;
				pixels.clear(); // Set all pixel colors to 'off'
				pixels.show();   // Send the updated pixel colors to the hardware.
			}
		}
		if (SWnoteIsOn[i]) {												//if the note is on
			if (millis() > (SWnoteTimers[i] + midiOnTime)) {						//if was turned on more than 30 ms ago
				softMidi.sendNoteOff(i, 127, 1);
				SWnoteIsOn[i] = false;
				pixels.clear(); // Set all pixel colors to 'off'
				pixels.show();   // Send the updated pixel colors to the hardware.
			}
		}
	}
}


/*
MICRO:BIT TEST CODE:

let Increase = 0
basic.showLeds(`
# # # # #
. . . . .
. . # . .
. . # . .
# # # # #
`)
pins.digitalWritePin(DigitalPin.P0, 0)
basic.forever(function () {
Increase += 1
basic.showNumber(Increase)
if (Increase > 7) {
Increase = 0
}
if (Increase % 2 == 0) {
led.toggleAll()
pins.digitalWritePin(DigitalPin.P0, 0)
} else {
pins.digitalWritePin(DigitalPin.P0, 1)
}
pins.i2cWriteNumber(
0,
Increase,
NumberFormat.Int8LE,
false
)
})


*/