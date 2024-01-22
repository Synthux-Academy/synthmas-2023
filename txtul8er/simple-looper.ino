
#include "simple-daisy.h"
#include "looper.h"
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

// Setup pins
static const int mix_control_pin = A(S30);
static const int grain_env_pin   = A(S31);
static const int g_play_speed_pin = A(S32);

static const float kKnobMax = 1023;

// Allocate Audio buffer in SDRAM 
static const uint32_t kBufferLengthSec = 6;
static const uint32_t kSampleRate = 48000;
static const size_t kBufferLengthSamples = kBufferLengthSec * kSampleRate;
static float DSY_SDRAM_BSS buffer[kBufferLengthSamples];

// grain playback stuff
unsigned long gCurrentMillis;
unsigned long gPreviousMillis = 0;
static const uint32_t gSampleIntervalStart = 100;  //milliseconds between samples. 1000 ms = 1 s
static const uint32_t gBufferLengthSec = 300;
static const size_t gBufferLengthSamples = gBufferLengthSec * 1000 / gSampleIntervalStart; 
uint16_t gBuffer[gBufferLengthSamples] = {0};
uint32_t gSampleInterval = gSampleIntervalStart; 

static const uint16_t grain_rec_button = 10;
static const uint16_t grain_play_button = 11;
bool grain_is_recording = false;
bool grain_is_playing = false;
size_t g_headposition = 0;
size_t gloopend = 0;
uint16_t g_read = 0;

static synthux::Looper looper;
float mix_control = 0;

Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;


void AudioCallback(float **in, float **out, size_t size) {
  // mix 
  for (size_t i = 0; i < size; i++) {
    auto looper_out = looper.Process(in[1][i]);
    float mix = ( mix_control * in[1][i] + (1 - mix_control) * looper_out ) * 0.707;
    out[0][i] = out[1][i] = mix;
  }
  
}

void setup() {
  DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  float sample_rate = DAISY.get_samplerate();

  // Setup looper
  looper.Init(buffer, kBufferLengthSamples);

  // Setup serial for cap touch debug
    Serial.begin(9600);

  // Set up cap touch on I2C default address
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");

  // Setup audio callback
  DAISY.begin(AudioCallback);
}

void loop() {

  mix_control = fmap(analogRead(mix_control_pin) / kKnobMax, 0.f, 1.f);
  gSampleInterval = fmap(analogRead(g_play_speed_pin) / kKnobMax, 0.f, 1.f);
  // Read cap touch stuff
  // Get the currently touched pads
  currtouched = cap.touched();

  // read/set play/pause and re cord states from currtouched
  // set grain record state
  if ((currtouched & _BV(grain_rec_button)) && !(lasttouched & _BV(grain_rec_button)) ) {
      grain_is_playing = false;
      g_read = 0;
      if(grain_is_recording){ 
        grain_is_recording = false; //stop recording
        grain_is_playing = true;    //start playing
      }
      else {
        grain_is_playing = false;  //stop playing
        grain_is_recording = true; //start recording
      }
    }
  // set grain playback state
  if ((currtouched & _BV(grain_play_button)) && !(lasttouched & _BV(grain_play_button)) ) {
      grain_is_playing = !grain_is_playing;
      g_read = 0;
    }
  if((currtouched & _BV(grain_play_button)) && (currtouched & _BV(grain_rec_button))){
    // both touched. erase grain buffer
    grain_is_recording = false;
    grain_is_playing = false;
    g_headposition = 0;
    gloopend = 0;
    delay(100);
  }

  lasttouched = currtouched;

  // Record or Play the gloop
  gCurrentMillis = millis();
  if(gCurrentMillis - gPreviousMillis >= gSampleInterval){
    gPreviousMillis = gCurrentMillis;
    // if recording, write currtouched to buffer at headposition on, update loopend if needed
    if(grain_is_recording){
      gBuffer[g_headposition] = currtouched & 15; //bitwise 'and' w/ 15 to mask out the play/pause buttons
      g_headposition++;
      if(g_headposition > gBufferLengthSamples){ //end of buffer. loop back to beginning of buffer. set gloopend to buffer length
        g_headposition = 0;
        gloopend = gBufferLengthSamples;
      }
      if(g_headposition > gloopend){ // we haven't overrun the buffer, but we are past the previous loopend.
        gloopend = g_headposition;
      }
    }
    // if playing, read from buffer at headposition, wrapping at loopend
    if(grain_is_playing){
      g_read = gBuffer[g_headposition];
      g_headposition++;
      g_headposition %= gloopend;
    }

  }
  // 'or' together the currtouched and the current grainloop before passing currtouched to setTaps below
  currtouched = currtouched | g_read;

  auto grain_env = analogRead(grain_env_pin) / kKnobMax;

  looper.setTaps(currtouched, grain_env);
}