//  
//          __      __            _               
//     ____/ /_  __/ /_     _____(_)_______  ____ 
//    / __  / / / / __ \   / ___/ / ___/ _ \/ __ \
//   / /_/ / /_/ / /_/ /  (__  ) / /  /  __/ / / /
//   \__,_/\__,_/_.___/  /____/_/_/   \___/_/ /_/ 
//    adam hammad 


// SETUP LIBRARIES

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// CREATE AUDIO OBJECTS

AudioSynthWaveform       waveform1;      //xy=159.99999999999994,512.4999999999998
AudioSynthWaveform       waveform2;      //xy=179.99999999999994,807.4999999999998
AudioInputUSB            usb2;           //xy=206.5,1271
AudioInputI2S            i2s2;           //xy=215.5,1120
AudioSynthWaveformModulated waveformMod1;   //xy=373.5,777
AudioSynthSimpleDrum     drum1;          //xy=380.5,311
AudioSynthKarplusStrong  string1;        //xy=413.5,905
AudioEffectEnvelope      envelope1;      //xy=460.5,639
AudioMixer4              mixerInput;     //xy=460.5,1075
AudioMixer4              mixerSounds;    //xy=640.5,588
AudioFilterStateVariable filter1;        //xy=902.5,1212
AudioMixer4              mixerMain;      //xy=925.5,607
AudioMixer4              mixerDelay;     //xy=1011.5,807
AudioMixer4              mixerOutput;    //xy=1129.5,339
AudioEffectChorus        chorus1;        //xy=1206.5,543
AudioEffectDelay         delay1;         //xy=1238.5,1227
AudioAmplifier           ampReverb;      //xy=1305.5,210
AudioMixer4              mixerReverb;    //xy=1442.5,505
AudioEffectReverb        reverb1;        //xy=1478.5,209
AudioAmplifier           ampOutput;      //xy=1625.5,506
AudioOutputI2S           i2s1;           //xy=1762.5,373
AudioOutputUSB           usb1;           //xy=1790.5,636
AudioConnection          patchCord1(waveform1, 0, waveformMod1, 0);
AudioConnection          patchCord2(waveform2, 0, waveformMod1, 1);
AudioConnection          patchCord3(usb2, 0, mixerInput, 2);
AudioConnection          patchCord4(usb2, 1, mixerInput, 3);
AudioConnection          patchCord5(i2s2, 0, mixerInput, 0);
AudioConnection          patchCord6(i2s2, 1, mixerInput, 1);
AudioConnection          patchCord7(waveformMod1, envelope1);
AudioConnection          patchCord8(drum1, 0, mixerSounds, 0);
AudioConnection          patchCord9(string1, 0, mixerSounds, 2);
AudioConnection          patchCord10(envelope1, 0, mixerSounds, 1);
AudioConnection          patchCord11(mixerInput, 0, mixerDelay, 0);
AudioConnection          patchCord12(mixerInput, 0, mixerMain, 1);
AudioConnection          patchCord13(mixerSounds, 0, mixerMain, 0);
AudioConnection          patchCord14(mixerSounds, 0, mixerDelay, 2);
AudioConnection          patchCord15(filter1, 0, delay1, 0);
AudioConnection          patchCord16(mixerMain, 0, mixerOutput, 0);
AudioConnection          patchCord17(mixerDelay, 0, filter1, 0);
AudioConnection          patchCord18(mixerDelay, 0, filter1, 1);
AudioConnection          patchCord19(mixerDelay, 0, mixerOutput, 1);
AudioConnection          patchCord20(mixerOutput, ampReverb);
AudioConnection          patchCord21(mixerOutput, 0, mixerReverb, 1);
AudioConnection          patchCord22(mixerOutput, chorus1);
AudioConnection          patchCord23(chorus1, 0, mixerReverb, 2);
AudioConnection          patchCord24(delay1, 0, mixerDelay, 1);
AudioConnection          patchCord25(ampReverb, reverb1);
AudioConnection          patchCord26(mixerReverb, ampOutput);
AudioConnection          patchCord27(reverb1, 0, mixerReverb, 0);
AudioConnection          patchCord28(ampOutput, 0, usb1, 1);
AudioConnection          patchCord29(ampOutput, 0, usb1, 0);
AudioConnection          patchCord30(ampOutput, 0, i2s1, 1);
AudioConnection          patchCord31(ampOutput, 0, i2s1, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=166.5,110

// end CREATE AUDIO OBJECTS


// SETUP OLED DISPLAY

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix SSD1306.h!");
#endif


// SETUP DIGITAL BUTTONS

Bounce button_sound = Bounce(26, 15);
Bounce button_trigger = Bounce(25, 15);
Bounce low_battery = Bounce(2, 15);

Bounce switch1 = Bounce(27, 15); // WAVEFORM1
Bounce switch2 = Bounce(30, 15); // WAVEFORM2
Bounce switch3 = Bounce(29, 15);
Bounce switch_trigger = Bounce(24, 15);


// CONFIG CHORUS  
#define CHORUS_DELAY_LENGTH (16*AUDIO_BLOCK_SAMPLES)
short l_delayline[CHORUS_DELAY_LENGTH];

// GLOBAL VARIABLES


// Sweep
float t_ampx = 0.8;
int t_lox = 10;
int t_hix = 22000;
// Length of time for the sweep in seconds
float t_timex = 10;

int current_waveform = 0; 
int sound_selection = 1; // Selection for sound switch
int button_value = HIGH; 
int switch_value = HIGH; 
boolean isNoteOn = true; 
String sound_name = "Siren"; // Initialise for display



void setup() {

  // INIT DEVICE
  Serial.begin(9600);
  
  pinMode(2, INPUT_PULLUP); 
  pinMode(24, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(27, INPUT_PULLUP);
  pinMode(29, INPUT_PULLUP);
  pinMode(30, INPUT_PULLUP);
  
  delay(300);
  
  Serial.println("Pounding System [Hydro Delay 1000]");

  // INIT DISPLAY
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);    // initialize with the I2C addr 0x3C (for the 128x32) 
  display.clearDisplay();
  robojaxText(sound_name, 0, 0, 2, false);
  display.display();

  
  // BEGIN AUDIO CONNECTIONS
  AudioMemory(500);                             // allocate RAM to audio device
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);   // input declaration
  sgtl5000_1.volume(0.5);                       // volume declaration
  sgtl5000_1.lineInLevel(12);                   // input level declaration
  
  // INIT WAVEFORMS + MODULATOR
  waveformMod1.frequency(261.63);
  waveformMod1.amplitude(1.0);
  waveform1.frequency(20.3);                        // Sine waves are low frequency oscillators (LFO)
//  sine2.frequency(1.2);
//  sine2.amplitude(0);
  current_waveform = WAVEFORM_SINE;

  waveformMod1.begin(current_waveform);
  waveform1.begin(current_waveform);
  waveform2.begin(1, 1.2, current_waveform);


  // CONFIGURE MIXER + AMPS
  mixerMain.gain(0, 0.5);        // Synthesised sounds to Main Mixer
  mixerMain.gain(1, 0.5);        // Audio Input to Main Mixer
  
  mixerSounds.gain(0, 0.25);     // Drum Synth
  mixerSounds.gain(1, 0.25);     // Waveforms
  mixerSounds.gain(2, 0.25);     // String
  mixerSounds.gain(3, 0.25);     // 

  mixerReverb.gain(0, 0.25);      // Reverb In
  mixerReverb.gain(1, 0.25);      // Main Thru
  mixerReverb.gain(2, 0.25);      // Chorus In
  
  mixerDelay.gain(2, 0.25);      // Siren to Delay

  mixerInput.gain(0, 0.25);      // Line/Mic L
  mixerInput.gain(1, 0.25);      // Line/Mic R
  mixerInput.gain(0, 0.25);      // USB L
  mixerInput.gain(1, 0.25);      // USB R
  
  mixerOutput.gain(0, 0.5);      // Main to Mixer Output
  mixerOutput.gain(1, 0.5);      // Delay to Mixer Output

  ampReverb.gain(0);            // Init reverb as OFF


  // INIT DELAY
  delay1.delay(0, 1000);

  

  // ENVELOPE SETUP
  envelope1.attack(20);
  envelope1.sustain(1.0);
  envelope1.decay(200);


  // DRUM SETUP
  drum1.frequency(1000);
  drum1.length(350);

  // INIT CHORUS
  chorus1.begin(l_delayline,50,4);
  
}
   
void loop() {
  
  // READ AND UPDATE BUTTONS AND INPUTS
  button_sound.update();
  button_trigger.update();
  low_battery.update();

//  Serial.print("Battery: ");
//  Serial.println(low_battery.read());

  switch1.update();           // Waveform 1
  switch2.update();           // Waveform 2
  switch3.update();           // Min/Line
  switch_trigger.update();    // Siren trigger 'HOLD'

  // Convert analog inputs to range from 0.0 TO 1.0

  float knob_delay = (float)analogRead(A7) / 1023.0; // KNOB_DELAY
  float knob_amplitude = (float)analogRead(A20) / 1023.0; // KNOB_SIRENVOL
  float knob_modulation = (float)analogRead(A16) / 1023.0; // KNOB_MOD
  float knob_frequency = (float)analogRead(A17) / 1023.0; // KNOB_FREQ
  float knob_filter = (float)analogRead(A15) / 1023.0; // KNOB_FILTER 
  float knob_feedback = (float)analogRead(A14) / 1023.0; // KNOB_FEEDBACK
  float knob_inthru = (float)analogRead(A17) / 1023.0; // KNOB_INPUTTHRU
  float knob_todelay = (float)analogRead(A16) / 1023.0; // KNOB_INPUTTODELAY
  float knob_involume = (float)analogRead(A12) / 1023.0; // KNOB_INPUT
  float knob_outvolume = (float)analogRead(A13) / 1023.0; // KNOB_OUTPUT
  float knob_speed = (float)analogRead(A19) / 1023.0; // KNOB_SPEED
  float knob_effect = (float)analogRead(A18) / 1023.0; // KNOB_EFFECT
  float knob_reverbTime = (float)analogRead(A20) / 1023.0; // KNOB_REVERBTIME
  float knob_reverbVolume = (float)analogRead(A10) / 1023.00 // KNOB_REVERBVOLUME

  // MIC/LINE SWITCH
  if (switch3.read() == 1) {
      sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  }
  else if (switch3.read() == 0) {
      sgtl5000_1.inputSelect(AUDIO_INPUT_MIC);
      sgtl5000_1.micGain(knob_involume * 63);
  }

  // Filter Frequency
  filter1.frequency(5 + knob_filter * 1000);

  // Siren Volume
  waveformMod1.amplitude(knob_amplitude * 1);

  // Siren Frequency
  waveformMod1.frequency(5 + knob_frequency * 3000);

  // Siren Modulation
  waveformMod1.frequencyModulation(knob_modulation * 2);

  // Feedback
  mixerDelay.gain(1, knob_feedback * 0.99);

  // Speed
  waveform1.frequency(knob_speed * 20);
  
  // Input to Delay + Thru
  mixerDelay.gain(0, knob_todelay * 0.5);

  if (knob_inthru > knob_todelay) {
  mixerMain.gain(1, (knob_inthru - knob_todelay) * 0.5);
  }
  if (knob_inthru < knob_todelay) {
  mixerMain.gain(1, 0);
  }


  // Reverb Time
  reverb1.reverbTime(knob_reverbTime * 10);
  
  // Turn off Reverb Volume
  if (knob_reverbVolume < 0.01) {
    Serial.print("Knob Volume: ");
    Serial.println(knob_reverbVolume);
    ampReverb.gain(0);
  }
  else {
    ampReverb.gain(0.5);
  }

  // Output Volume
  ampOutput.gain(knob_outvolume * 8);

  // Natural Latency
  delay(5);
  
  // Siren Triggers
  button_value = button_trigger.read();
  switch_value = switch_trigger.read();


  // SIREN + SPACE SIREN
  
  if (sound_selection == 1 || sound_selection == 4) {
    if (( button_value == LOW && isNoteOn == false ) || ( switch_value == HIGH && isNoteOn == false )) {
      envelope1.noteOn();
      delay1.delay(0, knob_delay * 1000);
      waveform1.phase(0);
      Serial.println("Trigger Pressed");
      isNoteOn = true;
    }
    else if ( switch_value == LOW && button_value == HIGH ) {
      envelope1.noteOff();
      isNoteOn = false;
    }

    // Add latency for Space Siren Effect
    if (sound_selection == 4) {
      delay(knob_effect * 200);
    }
  }

  
  // BUBBLES SOUND
  
  if (sound_selection == 2) {
    if ( button_value == LOW && isNoteOn == false ) { 
      drum1.frequency(5 + knob_frequency * 3000);
      drum1.length(150);
      drum1.secondMix(0.0);
      drum1.pitchMod(0.0);
      drum1.noteOn();
      isNoteOn = true;
    }
    else if (button_value == HIGH) {
          envelope1.noteOff();
              isNoteOn = false;
    }
  }

  // TONE SWEEP

  if (sound_selection == 3) {
    if ( button_value == LOW && isNoteOn == false ) {   
//        tonesweep1.play(t_ampx,t_lox,t_hix,t_timex);
        isNoteOn = true;
    }
    else if (button_value == HIGH) {
          envelope1.noteOff();
          isNoteOn = false;
    }
  }
  
  // WAVEFORM SHAPE SWITCH

  if ( switch1.read() == HIGH && switch2.read() == LOW ) {
    current_waveform = WAVEFORM_SQUARE;
  }
  if ( switch1.read() == LOW && switch2.read() == HIGH ) {
    current_waveform = WAVEFORM_SAWTOOTH;
  }
  if ( switch1.read() == HIGH && switch2.read() == HIGH ) {
    current_waveform = WAVEFORM_SINE;
  }
 
  waveformMod1.begin(current_waveform);
  waveform1.begin(1, knob_involume * 20, current_waveform);
  //waveform2.begin(1, knob_modulation * 20, WAVEFORM_SINE);
  
  
  // CHANGE THE SIREN SOUND
  
  if (button_sound.fallingEdge()) {
    switch (sound_selection) {
      case 1:
        sound_selection = 2;
        sound_name = "Bubbles";
        Serial.println("Sound 2");
        break;
      case 2:
        sound_selection = 4;
        sound_name = "Space     Siren";
        Serial.println("Space Siren");
        break;
      case 3:
        sound_selection = 4;
        sound_name = "Blank";
        Serial.println("Blank");
        break;
      case 4:
        sound_selection = 1;
        sound_name = "Siren";
        Serial.println("Sound 1");
        break;
      }
        display.clearDisplay();
        robojaxText(sound_name, 0, 0, 2, true);    
    }

  // end LOOP
}

// TEXT GENERATOR FOR DISPLAY

void robojaxText(String text, int x, int y,int size, boolean d) {

  display.setTextSize(size);
  display.setTextColor(WHITE);
  display.setCursor(x,y);
  display.println(text);
  if(d){
    display.display();
  }
  
  
}
