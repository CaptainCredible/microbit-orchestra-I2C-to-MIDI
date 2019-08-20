bool mumIsVolcaSample = false;
bool mumIsVolcaBeats = false;

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
int midiOnTime = 20;

#define NUMPIXELS 17
#define neoPin 10
byte myBrightness = 254;

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
	pixels.clear();
	pixels.show();
	
	for (int i = 0; i < 150; i++) {
		pixels.fill(pixels.Color(i, 0, 0));
		pixels.show();
		delay(3);
	}
	for (int i = 150; i > 0; i--) {
		pixels.fill(pixels.Color(i, 0, 0));
		pixels.show();
		delay(3);
	}
	pixels.clear();
	pixels.show();
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

unsigned long int lastMumTime = 0;
unsigned long int lastCatTime = 0;


byte drumColoursR[16] = { 255, 255, 255, 155,  255,  60, 155,   0 ,  0 , 80 ,0,0,0,0,0,0 };
byte drumColoursG[16] = {   0, 255, 255, 155,   0,  60,   0, 255 ,  0 , 80 ,0,0,0,0,0,0 };
byte drumColoursB[16] = {   0, 255, 255, 155,  255,  60, 155, 255 ,255 ,  0 ,0,0,0,0,0,0 };
byte volcaBeatNotes[16] = { 36,42,38,46,39,43,50,75,67,49,50,49,75,67,49,43 };

/*36 - C2 - Kick 
38 - D2 - Snare 
43 - G2 - Lo Tom 
50 - D3 - Hi Tom 
42 - F#2 - Closed Hat 
46 - A#2 - Open Hat 
39 - D#2 - Clap 
75 - D#5 - Claves
67 - G4 - Agogo 
49 - C#3 - Crash*/ 


void handle(int whot) {

	int port = 0;
	//Serial.print("port ");
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
	

	byte currentRed = 0;
	if (whot >= 127) {  // Cat
		int tempWhot = whot - 127;
		MIDI.sendNoteOn(tempWhot , 127, 1);
		Serial.println(" HWSERIAL ");
		
		Serial.println(tempWhot);
		if ((millis() - lastCatTime) > 2) {
			pixels.clear();
		}
		lastCatTime = millis();
		//pixels.setPixelColor(tempWhot%12,pixels.Color(255, 0, 255)); // 12pixel version
		int tempPixel = (tempWhot % 12) * 20;
		int temptempWhot = 16 - (tempWhot % 12);
		//Serial.print("temptempwhot ");
		//Serial.println(temptempWhot);
		byte taper = 20;
		pixels.setPixelColor(temptempWhot + 1, pixels.Color(taper, taper, taper)); //2 pixels
		pixels.setPixelColor(temptempWhot, pixels.Color(255, 255,255)); //2 pixels
		pixels.setPixelColor(temptempWhot - 1, pixels.Color(taper, taper, taper)); //2 pixels
		myBrightness = 254;
		pixels.setBrightness(myBrightness);
																							 //pixels.setBrightness(255);
		//for (int i = 0; i < 6; i++) {
		//	pixels.setPixelColor(i, pixels.Color(tempPixel, 255, 255 - tempPixel)); //2 pixels
		//}
		pixels.show();   // Send the updated pixel colors to the hardware.
		HWnoteIsOn[tempWhot] = true;
		HWnoteTimers[tempWhot] = millis();
		
		//delay(10);
	//	softMidi.sendNoteOff(whot-127 , 127, 1);
	}




	else {				//Mum
		
		lastMumTime = millis();
		int myChan = whot % 16;
		
			softMidi.sendNoteOn(myChan, 127, 1);
			Serial.print("Sent SoftMIDI  ")
			Serial.println(myChan)
		
		int tempPixel = (whot % 12) * 20;
		
		//pixels.setPixelColor(tempPixel, pixels.Color(tempPixel, 255, 255 - tempPixel));

		if (myChan == 0) {
			for (int i = 0; i < 5; i++) {
				pixels.setPixelColor(i, pixels.Color(drumColoursR[myChan], drumColoursG[myChan], drumColoursB[myChan])); //2 pixels
				myBrightness = 254;
				pixels.setBrightness(myBrightness);

			}
		}
		if (myChan == 2) {
			for (int i = 0; i < 5; i++) {
				pixels.setPixelColor(i, pixels.Color(drumColoursR[myChan], drumColoursG[myChan], drumColoursB[myChan])); //2 pixels
				myBrightness = 254;
				pixels.setBrightness(myBrightness);

			}
		}
		if (myChan == 4) {
			for (int i = 0; i < 5; i++) {
				pixels.setPixelColor(i, pixels.Color(drumColoursR[myChan], drumColoursG[myChan], drumColoursB[myChan])); //2 pixels
				myBrightness = 254;
				pixels.setBrightness(myBrightness);

			}
		}
		
		SWnoteIsOn[whot] = true;
		SWnoteTimers[whot] = millis();
		//Serial.println(" SWSERIAL ");
//		MIDI.sendNoteOff(whot , 127, 1);
		
	}
}

bool ledsAreOn = false;
byte interval = 0;
void handleNoteOffs() {
	//interval++;
	//if (interval == 1) {
		//Serial.println(myBrightness);
		if (myBrightness >= 2) {
			myBrightness--;
			pixels.setBrightness(myBrightness);
			pixels.show();
			ledsAreOn = true;
		}
		else if (ledsAreOn) {
			pixels.setBrightness(254);
			pixels.clear();
			pixels.show();
			ledsAreOn = false;
		}
		//interval = 0;
	//}

	for (int i = 0; i < 127; i++) {
		if (HWnoteIsOn[i]) {												//if the note is on
			if (millis() > (HWnoteTimers[i] + midiOnTime)) {						//if was turned on more than 30 ms ago
				MIDI.sendNoteOff(i , 127, 1);
				byte tempPixelOff = (i % 12);
				//tempPixelOff = 11 - tempPixelOff;
				//Serial.print("tempPixelOff = ");
				//Serial.println(tempPixelOff);
				//pixels.setPixelColor(tempPixelOff+1, pixels.Color(0, 0, 0));
				//pixels.setPixelColor(tempPixelOff, pixels.Color(0, 0, 0));
				//pixels.setPixelColor(tempPixelOff-1, pixels.Color(0, 0, 0));
				HWnoteIsOn[i] = false;
				//pixels.clear(); // Set all pixel colors to 'off'
				//pixels.show();   // Send the updated pixel colors to the hardware.
			}
		}
		if (SWnoteIsOn[i]) {												//if the note is on
			if (millis() > (SWnoteTimers[i] + midiOnTime)) {						//if was turned on more than 30 ms ago
				if (mumIsVolcaSample) {
					int myChan = i % 16;
					softMidi.sendNoteOff(i, 127, myChan+1);
				}
				else if (mumIsVolcaBeats) {
					softMidi.sendNoteOff(volcaBeatNotes[i], 127, 1);
				}
				else {
					softMidi.sendNoteOff(i, 127, 1);
				}
				
				SWnoteIsOn[i] = false;
				//pixels.clear(); // Set all pixel colors to 'off'
				//pixels.show();   // Send the updated pixel colors to the hardware.
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