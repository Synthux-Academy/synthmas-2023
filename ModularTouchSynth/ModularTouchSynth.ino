// Title: Modular Touch Synth
// Description: Patchable 3 voice synth using Simple Synth Touch Kit.
// Hardware: Daisy Seed with Simple Touch Synth
// Author: Sam Knight


#include "DaisyDuino.h"
#include "simple-daisy.h"
#include "term.h"

DaisyHardware hw;
synthux::Terminal terminal;

// -------------------- KNOB MAPPING -------------------- 
#define K1 A0
#define K2 A1
#define K3 A2
#define K4 A3
#define K5 A4
#define K6 A5

enum ModuleMode
{
  Osc1Mode,
  Osc2Mode,
  Osc3Mode,
  EnvMode,
  LPFMode,
  ChorusMode,
  DelayMode,
  MixerMode
};

enum GlobalMode
{
  PatchMode,
  RecordMode,
  PlayMode,
  EditMode
};

class Osc
{
  private:
    int modulating = -1; // which osc it's modulating. -1 if nothing
    int modulated = -1; // which osc is modulating it. -1 if nothing
  public:
    Oscillator oscil;
    
    int modMixerLevel = 1; // either 0 or 1 based on if modulater or not
    float mixerLevel = 1.f;
    float freq = 0; // Out of 12. Transposes midi note that was hit
    float noteHit = 440;
    bool reverb = false;
    bool delay = false;
    float fmAmount = 1.f;
    float pulseWidth = 0.f;
    float waveform = 2;
    float modulatingFreq = 440.f;

    Osc(float _samplerate)
    {
      oscil.Init(_samplerate);
      oscil.SetFreq(440);
      oscil.SetAmp(1.f);
      oscil.SetWaveform(Oscillator::WAVE_SAW);
    }
    void SetModulating(float _modulatingOsc) // which osc it's modulating
    {
      modulating = _modulatingOsc;
      if(modulating == -1)
        modMixerLevel = 1;
      else
      {
        oscil.SetWaveform(Oscillator::WAVE_SIN);
        modMixerLevel = 0;
      }
    }
    void SetModulated(float _modulatedOsc) // which osc is modulating it 
    {
      modulated = _modulatedOsc;
      // if(modulated == -1)
      //   modMixerLevel = 0;
      // else
      //   modMixerLevel = 1;
    }

    int GetModulating() { return modulating; } // which osc it's modulating

    int GetModulated(){ return modulated; } // which osc is modulating it 
   
    void SetFreq(float freq)
    {
      oscil.SetFreq(freq);
    }
    void SetWaveform(float waveform)
    {
      oscil.SetWaveform((int) waveform);
    }
    float Process()
    {
      return oscil.Process();
    }
};


// -------------------- MODULES -------------------- 
Osc osc1(48000);
Osc osc2(48000);
Osc osc3(48000);
Osc oscillators[3] = {{osc1}, {osc2}, {osc3}};
AdEnv sequencerEnv;
Adsr env;
Svf LPF;
Chorus chorus;
Metro metronome;
#define MAX_DELAY static_cast<size_t>(48000 * 0.75f)
static DelayLine<float, MAX_DELAY> del;

// ---------------- GLOBAL VARIABLES ----------------
int _storedModule = -1;
int _nonPatchableModules[5] = {3, 4, 5, 6, 9};  // everything except Oscillators, reverb, and delay
ModuleMode _moduleMode = Osc1Mode;
GlobalMode _globalMode = PlayMode;

// Play Mode Variables
bool _gate = false;
int _midiNotes[7] = { 48, 50, 52, 53, 55, 57, 59};
int _transposition = 2; // 2 is normal octave

// Patch Mode Variables
bool _patchHasReleased = true;
int _patchHitNote = -1;

// Record Mode Variables
int _recordedNotes[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
bool _isRecording = false;
bool _sequenced = false;
int _recordHitNote = -1;
bool _metronomeCanIterate = true;
int _sequencerResetCounter = 16;
int _sequencerRecordingIndex = 0;
int _sequencerIndex = 0;
int _bpm = 120;

// knobs
float _k1 = 0;
float _k2 = 0;
float _k3 = 0; 
float _k4 = 0;
float _k5 = 0;
float _k6 = 0;

// ADSR
float _attack = 0.01f;
float _decay = 0.f;
float _sustain = 1.f;
float _release = 0.5f;

// LPF
float _lpfModAmount = 0.f;
float _lpfResonance = 0.f;
float _lpfCutoff = 10000.f;

// Chorus
float _chorusDryWet = .0f;
float _chorusFeedback = .3f;
float _chorusDelay = 0.5f;
float _chorusDepth = 0.5f;
float _chorusFreq = .3f;

// Delay
float _delayDryWet = 0.f;
float _delayLPFCutoff = 10000.f;
float _delayTime, _delayCurrentTime = .4f;
float _delayFeedback = 0.3f;

// Mixer
float _mixerLfoAmount = 0.f;

size_t num_channels;

void MyCallback(float **in, float **out, size_t size) {
  float osc_sig, del_out, feedback, sig_out;
  if(_globalMode == EditMode)
    ProcessADC();

  LPF.SetFreq(_lpfCutoff);
  LPF.SetRes(_lpfResonance);


  for (size_t i = 0; i < size; i++) {

    PlaySequence();

    for(int i = 0; i < 3; i++)
    {
      if(oscillators[i].GetModulated() != -1) // if it's being modulated
      {
        oscillators[i].SetFreq(oscillators[i].noteHit + (oscillators[i].noteHit * (oscillators[oscillators[i].GetModulated()].Process() * oscillators[i].fmAmount)));
      }
      else // not being modulated
      {
        if(oscillators[i].GetModulating() == -1) // if it is not modulating something
          oscillators[i].SetFreq(oscillators[i].noteHit);
      }
    }
    osc_sig = (((oscillators[0].Process() * oscillators[0].modMixerLevel * oscillators[0].mixerLevel) * 0.3f) +
          ((oscillators[1].Process() * oscillators[1].modMixerLevel * oscillators[1].mixerLevel) * 0.3f) +
          ((oscillators[2].Process() * oscillators[2].modMixerLevel * oscillators[2].mixerLevel) * 0.3f));


    // EFFECTS
    del_out = del.Read();

    sig_out = (del_out * _delayDryWet) + (osc_sig * (1.f - _delayDryWet));
    feedback = (del_out * _delayFeedback) + osc_sig;

    del.Write(feedback);
    sig_out = (chorus.Process(sig_out) * _chorusDryWet) + (sig_out * (1.f - _chorusDryWet));


    LPF.Process(sig_out);
    sig_out = LPF.Low();

    for (size_t chn = 0; chn < num_channels; chn++) {
      for(int i = 0; i < 3; i++)
      {
        if(oscillators[i].GetModulating() == -1)
        {
          oscillators[i].oscil.SetAmp(env.Process(_gate) + sequencerEnv.Process());
        }
      }


      out[chn][i] = sig_out;
    }
  }
}

void setup() {
  float samplerate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_POD, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  samplerate = DAISY.get_samplerate();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D(S07), INPUT_PULLUP);
  pinMode(D(S09), INPUT_PULLUP);

  terminal.Init();

  metronome.Init(_bpm / 60, samplerate);

  del.SetDelay(samplerate * _delayTime);

  LPF.Init(samplerate);
  LPF.SetFreq(_lpfCutoff);
  LPF.SetRes(_lpfResonance);

  chorus.Init(samplerate);
  chorus.SetFeedback(_chorusFeedback);
  chorus.SetDelay(_chorusDelay);
  chorus.SetLfoDepth(_chorusDepth);
  chorus.SetLfoFreq(_chorusFreq);

  sequencerEnv.Init(samplerate);
  sequencerEnv.SetTime(ADENV_SEG_ATTACK, 0.01);
  sequencerEnv.SetMin(0.0);
  sequencerEnv.SetMax(1);
  sequencerEnv.SetTime(ADENV_SEG_DECAY, 0.8f);

  env.Init(samplerate);
  env.SetAttackTime(0.01f);
  env.SetReleaseTime(0.5f);
  // digitalWrite(LED_BUILTIN, HIGH);

  DAISY.begin(MyCallback);
}

void loop() {
  terminal.Process();

  ChangeMode();
  Transposition();



  _bpm = fmap(analogRead(A(S37)) / 1023.0, 120, 800); // left slider changes global BPM
  metronome.SetFreq(_bpm / 60);

  if(terminal.IsTouched(10)) // Resets sequence
  {
    for(int i = 0; i < 16; i++)
      _recordedNotes[i] = -1;
    _sequencerResetCounter = 16;
    _sequencerRecordingIndex = 0;
    _sequencerIndex = 0;
    _sequenced = false;
  }

  _gate = false;

  
  if(_globalMode == PatchMode)
  {
    UnPatch();
    for(int i = 0; i < 10; i++) // on touch
    {
      if(terminal.IsTouched(i))
      {
        if(_patchHasReleased)
        {
          PadTouched(i);
          _patchHitNote = i;
          _patchHasReleased = false;
        }
      }
    }
    // for holding down
    if(_patchHitNote != -1)
    {
      if(!terminal.IsTouched(_patchHitNote))
      {
        _patchHasReleased = true;
        _patchHitNote = -1;
      }
    }
  }
  else if(_globalMode == EditMode)
  {
    UnPatch();

    for(int i = 0; i < 10; i++) // on touch, change which module to edit
    {
      if(terminal.IsTouched(i))
      {
        _moduleMode = (ModuleMode) i;
      }
    }
  }
  else if(_globalMode == PlayMode)
  {
    UnPatch();

    for (int i = 3; i < 10; i++) {
      bool isTouched = terminal.IsTouched(i);
      if (isTouched) {
        oscillators[0].noteHit = mtof(_midiNotes[i - 3] + _transposition + int(trunc(oscillators[0].freq)));
        oscillators[1].noteHit = mtof(_midiNotes[i - 3] + _transposition + int(trunc(oscillators[1].freq)));
        oscillators[2].noteHit = mtof(_midiNotes[i - 3] + _transposition + int(trunc(oscillators[2].freq)));
        _gate = true;
      }
    }
  }
  else if(_globalMode == RecordMode)
  {
    Sequencer();
    for (int i = 3; i < 10; i++) {
      bool isTouched = terminal.IsTouched(i);
      if (isTouched) {
        oscillators[0].noteHit = mtof(_midiNotes[i - 3] + _transposition + int(trunc(oscillators[0].freq)));
        oscillators[1].noteHit = mtof(_midiNotes[i - 3] + _transposition + int(trunc(oscillators[1].freq)));
        oscillators[2].noteHit = mtof(_midiNotes[i - 3] + _transposition + int(trunc(oscillators[2].freq)));
        _gate = true;
      }
    }
  }
}


// ===============================================================================================================================================================
void UnPatch()
{
  if(terminal.IsTouched(11))
  {
    for(int i = 0; i < 3; i++)
    {
      oscillators[i].SetModulated(-1);
      oscillators[i].SetModulating(-1);
    }
  }
}
void Transposition()
{
  int transpositionKnob = (int) fmap(analogRead(A(S36)) / 1023.0, 0, 3.9);
  switch(transpositionKnob)
  {
    case 0:
      _transposition = -24;
      break;
    case 1:
      _transposition = -12;
      break;
    case 2:
      _transposition = 0;
      break;
    case 3:
      _transposition = 12;
      break;
  }
}
void ChangeMode()
{
  if(!digitalRead(D(S09)) && !digitalRead(D(S07)))
  {
    
    _globalMode = PlayMode;
    _isRecording = false;
  }
  else if(!digitalRead(D(S09)) && digitalRead(D(S07)))
  {
    // digitalWrite(LED_BUILTIN, HIGH);
    _globalMode = RecordMode;
  }
  else if (digitalRead(D(S09))  && !digitalRead(D(S07)))
  {
    _globalMode = EditMode;
    _isRecording = false;
  }
  else if (digitalRead(D(S09))  && digitalRead(D(S07)))
  {
    _globalMode = PatchMode;
    _isRecording = false;
  }
}

// have to put variables up here that aren't gonna be 0-1
float osc1Waveform, osc2Waveform, osc3Waveform = 0;
float osc1Freq, osc2Freq, osc3Freq = 0;
void ProcessADC()
{

  switch(_moduleMode)
  {
    case Osc1Mode:
      ConditionalParameter(_k1, analogRead(K1) / 1023.f, osc1Waveform); // Waveform (0-4)
      ConditionalParameter(_k3, analogRead(K3) / 1023.f, oscillators[0].fmAmount); // FM Amt
      ConditionalParameter(_k4, analogRead(K4) / 1023.f, osc1Freq); // Frequency (out of 12 to transpose hit note)
      ConditionalParameter(_k5, analogRead(K5) / 1023.f, oscillators[0].pulseWidth); // Pulse Width



      if(oscillators[0].GetModulating() == -1)
      {
        oscillators[0].SetWaveform(osc1Waveform * 4);
        oscillators[0].freq = fmap(osc1Freq, 0, 12.9f);
      }
      else
      {
        oscillators[0].modulatingFreq = fmap(osc1Freq, 10, 6000, daisysp::Mapping::EXP);
        oscillators[0].SetFreq(oscillators[0].modulatingFreq);
      }
      break;
    case Osc2Mode:
      ConditionalParameter(_k1, analogRead(K1) / 1023.f, osc2Waveform); // Waveform (0-4)
      ConditionalParameter(_k3, analogRead(K3) / 1023.f, oscillators[1].fmAmount); // FM Amt
      ConditionalParameter(_k4, analogRead(K4) / 1023.f, osc2Freq); // Frequency (out of 12 to transpose hit note)
      ConditionalParameter(_k5, analogRead(K5) / 1023.f, oscillators[1].pulseWidth); // Pulse Width


      if(oscillators[1].GetModulating() == -1)
      {
        oscillators[1].SetWaveform(osc2Waveform * 4);
        oscillators[1].freq = fmap(osc2Freq, 0, 12.9f);
      }
      else
      {
        oscillators[1].modulatingFreq = fmap(osc2Freq, 10, 6000, daisysp::Mapping::EXP);
        oscillators[1].SetFreq(oscillators[1].modulatingFreq);
      }
      break;
    case Osc3Mode:
      ConditionalParameter(_k1, analogRead(K1) / 1023.f, osc3Waveform); // Waveform (0-4)
      ConditionalParameter(_k3, analogRead(K3) / 1023.f, oscillators[2].fmAmount); // FM Amt
      ConditionalParameter(_k4, analogRead(K4) / 1023.f, osc3Freq); // Frequency (out of 12 to transpose hit note)
      ConditionalParameter(_k5, analogRead(K5) / 1023.f, oscillators[2].pulseWidth); // Pulse Width

      oscillators[2].SetWaveform(osc3Waveform * 4);

      if(oscillators[2].GetModulating() == -1)
        oscillators[2].freq = fmap(osc3Freq, 0, 12.9f);
      else
      {
        oscillators[2].modulatingFreq = fmap(osc3Freq, 10, 6000, daisysp::Mapping::EXP);
        oscillators[2].SetFreq(oscillators[2].modulatingFreq);
      }
      break;
    case EnvMode:
      ConditionalParameter(_k3, analogRead(K3) / 1023.f, _attack); // Attack
      ConditionalParameter(_k4, analogRead(K4) / 1023.f, _release); // Release

      sequencerEnv.SetTime(ADENV_SEG_ATTACK, fmap(_attack, 0.01f, 1, daisysp::Mapping::EXP));
      sequencerEnv.SetTime(ADENV_SEG_DECAY, fmap(_release, 0.01f, 1, daisysp::Mapping::EXP));
      break;
    case LPFMode: 
      // ConditionalParameter(_k1, analogRead(K1) / 1023.f, _lpfModAmount); // LPF Mod Amt
      ConditionalParameter(_k3, analogRead(K3) / 1023.f, _lpfResonance); // LPF Resonance
      ConditionalParameter(_k4, fmap(analogRead(K4) / 1023.f, 50, 10000, daisysp::Mapping::EXP), _lpfCutoff); // LPF Cutoff

      break;
    case ChorusMode:
      ConditionalParameter(_k1, analogRead(K1) / 1023.f, _chorusDryWet); // Chorus Dry/Wet
      ConditionalParameter(_k2, analogRead(K2) / 1023.f, _chorusFeedback); // Chorus Feedback
      ConditionalParameter(_k3, analogRead(K3) / 1023.f, _chorusFreq); // Chorus Freq
      ConditionalParameter(_k4, analogRead(K4) / 1023.f, _chorusDepth); // Chorus Depth

      chorus.SetFeedback(fmap(_chorusFeedback, 0, 1, daisysp::Mapping::EXP));
      chorus.SetLfoDepth(fmap(_chorusDepth, 0, 1, daisysp::Mapping::EXP));
      chorus.SetLfoFreq(fmap(_chorusFreq, 0, 1, daisysp::Mapping::EXP));
      break;
    case DelayMode:
      ConditionalParameter(_k1, analogRead(K1) / 1023.f, _delayDryWet); // Delay Dry/Wet
      ConditionalParameter(_k3, analogRead(K3) / 1023.f, _delayTime); // Delay Time
      ConditionalParameter(_k4, fmap(analogRead(K4) / 1023.f, 0.01f, 1), _delayFeedback);

      fonepole(_delayCurrentTime, fmap(_delayTime, 0.01f, 1, daisysp::Mapping::EXP), .00007f);
      del.SetDelay(48000 * _delayCurrentTime);
      break;
    case MixerMode:
      ConditionalParameter(_k1, analogRead(K1) / 1023.f, _mixerLfoAmount); // Mixer LFO Amount
      ConditionalParameter(_k3, analogRead(K3) / 1023.f, oscillators[0].mixerLevel); // Oscillator 1 Mixer Level
      ConditionalParameter(_k4, analogRead(K4) / 1023.f, oscillators[1].mixerLevel); // Oscillator 2 Mixer Level
      ConditionalParameter(_k5, analogRead(K5) / 1023.f, oscillators[2].mixerLevel); // Oscillator 3 Mixer Level
      break;

  }
}
void PlaySequence()
{
  if(_isRecording == false && _sequenced)
    {
      if(metronome.Process())
      {

        if(_metronomeCanIterate)
        {
          if(_recordedNotes[_sequencerIndex % _sequencerResetCounter] == -1)
          {
            _sequencerResetCounter = _sequencerIndex;
          }
          if(_recordedNotes[_sequencerIndex % _sequencerResetCounter] != -5)
          {
            for(int i = 0; i < 3; i++)
            {
              oscillators[i].noteHit = mtof(_recordedNotes[_sequencerIndex % _sequencerResetCounter] + (int)trunc(oscillators[i].freq));
            }
            sequencerEnv.Trigger();
          }
          _metronomeCanIterate = false; 
          _sequencerIndex++;
        }
      }
      else // this is so it only sequences once per metronome hit
      {

        _metronomeCanIterate = true;
      }
    }
}
void PadTouched(int pad)
{
  // can't patch module into the same module
  if(pad == _storedModule)
  {
   digitalWrite(LED_BUILTIN, LOW);
   _storedModule = -1.f;
   return;
  }
  // make sure they hit a module that is patchable
  for(int i = 0; i < 5; i++)
  {
    if(_nonPatchableModules[i] == pad)
    {
      digitalWrite(LED_BUILTIN, LOW);
      _storedModule = -1.f;
      return;
    }
  }
  // for when it's the first time a pad is being routed
  if(_storedModule == -1)
  {
    // can't patch reverb or delay into something
    if(pad == 7 | pad == 8) 
    {
      digitalWrite(LED_BUILTIN, LOW);
      _storedModule = -1;
      return;
    }

    digitalWrite(LED_BUILTIN, HIGH);
    _storedModule = pad;
    return;
  }
  else // second time being patched
  {
    if(pad == 7) // if patching into reverb
    {
      oscillators[_storedModule].reverb = !oscillators[_storedModule].reverb; // set it to whatever it's not (so they can also unpatch)
      digitalWrite(LED_BUILTIN, LOW);
      _storedModule = -1;
      return;
    }
    else if(pad == 8) // if patching into delay
    {
      oscillators[_storedModule].delay = !oscillators[_storedModule].delay;
      digitalWrite(LED_BUILTIN, LOW);
      _storedModule = -1;
      return;
    }


    // ----- now everything being patched is simply oscillator to oscillator


    // can't patch something being modulated into whatever is modulating it
    if(oscillators[_storedModule].GetModulated() != -1 && oscillators[pad].GetModulating() != -1) 
    {
      digitalWrite(LED_BUILTIN, LOW);
      _storedModule = -1;
      return;
    }
    // unpatch
    if(oscillators[_storedModule].GetModulated() == oscillators[pad].GetModulating() && oscillators[_storedModule].GetModulated() != -1)
    {
      oscillators[pad].SetModulating(-1);
      oscillators[_storedModule].SetModulated(-1);

      digitalWrite(LED_BUILTIN, LOW);
      _storedModule = -1;
      return;
    }
    // patch oscillators
    oscillators[_storedModule].SetModulating(pad);
    oscillators[pad].SetModulated(_storedModule);
    
    digitalWrite(LED_BUILTIN, LOW);
    _storedModule = -1;
  }
}

void Sequencer()
{
  _isRecording = true;
  if(_sequencerRecordingIndex == 16) return; // if sequence is filled
  // digitalWrite(LED_BUILTIN, HIGH);
  for (int i = 0; i < 12; i++) {
    if(i == 10 ) continue;
    bool isTouched = terminal.IsTouched(i);
    if (isTouched) {
      if(i == 11)
      {
        _recordedNotes[_sequencerRecordingIndex] = -5;
      }
      else {
        _recordedNotes[_sequencerRecordingIndex] = _midiNotes[i - 3] + _transposition;
      }
      _recordHitNote = i;
      _sequenced = true;
    }
  }
  if(_recordHitNote != -1) // so it doesn't spam the sequencerRecordingIndex++
  {
    if(!terminal.IsTouched(_recordHitNote))
    {
      _sequencerRecordingIndex++;
      _recordHitNote = -1;
    }
  }
  
}

void ConditionalParameter(float  &oldVal, float  newVal, float &param)
{
    if(abs(oldVal - newVal) > 0.005f)
    {
      param = newVal;
		  oldVal = newVal;
    }
}