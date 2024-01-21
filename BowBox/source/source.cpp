#include "daisy_seed.h"
#include "daisysp.h"
#include "magictape_lib.h"
#include "BowBox.h"

constexpr u_int8_t NUM_BOWLS = 6;


using namespace daisy;
using namespace daisysp;

DaisySeed hw;
BowedString Bowls[NUM_BOWLS];
Mpr121I2C TouchPads;
uint16_t touchState;
uint16_t touchOutL, touchOutR, lastTL, lastTR;
CanonicalFirstOrderFilters allpassTest;
WhiteNoise NoiseSource;
Svf LowPassTable[NUM_BOWLS], LowpassOut;
FilterType fType = Allpass;
Oscillator SineWave;
BowTable ExciterTable;
Adsr simpleEnv[NUM_BOWLS];
Line simpleLineL, simpleLineR;
Port lPort, rPort;
float lAmp, rAmp;


float bowlFreqs[] = {110.f, 117.3f, 176, 330.f, 350.6, 392.f};
Switch buttons[NUM_BOWLS];

// Easier to debounce them like this. I'd rather not use a for loop for simple logic like this
void debounceButtons() {
    buttons[0].Debounce();
    buttons[1].Debounce();
    buttons[2].Debounce();
    buttons[3].Debounce();
    buttons[4].Debounce();
    buttons[5].Debounce();
}

// Init ADSR Use for loop since it is only being used once
void setADSR(float SR) {
    for (auto &i: simpleEnv) {
        i.Init(SR, 1);
        i.SetAttackTime(0.125f);
        i.SetDecayTime(0.1255f);
        i.SetSustainLevel(1.f);
        i.SetReleaseTime(1.f);
    }
}

void setSustain(u_int16_t sus) {
    for (auto &i: simpleEnv) {
        i.SetSustainLevel(sus);
    }
}


void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    //Debounce the buttons
    debounceButtons();


    lAmp = float(touchOutL) * .25f;
    rAmp = float(touchOutR) * .25f;



    for (size_t i = 0; i < size; i++) {
        // Create Filtered Noise

        float noiseOut = NoiseSource.Process();
        LowPassTable[0].Process(noiseOut);
        LowPassTable[1].Process(noiseOut);
        LowPassTable[2].Process(noiseOut);
        LowPassTable[3].Process(noiseOut);
        LowPassTable[4].Process(noiseOut);
        LowPassTable[5].Process(noiseOut);

        float filteredNoise[] = {LowPassTable[0].Low(), LowPassTable[1].Low(), LowPassTable[2].Low(),
                                 LowPassTable[3].Low(), LowPassTable[4].Low(), LowPassTable[5].Low()};

        float sineOut = ExciterTable.Process(1.0f - SineWave.Process());

        float lPortOut = lPort.Process(lAmp);
        float rPortOut = rPort.Process(rAmp);


        // Process Resonantors. AVoid using for loop inside For loop.
        // Close encounters of the third kind
        float output = Bowls[0].Process(
                filteredNoise[0] *  simpleEnv[0].Process(buttons[0].Pressed()) * lPortOut );
        output += Bowls[1].Process(
                filteredNoise[1] *  simpleEnv[1].Process(buttons[1].Pressed()) * lPortOut );
        output += Bowls[2].Process(
                filteredNoise[2] *  simpleEnv[2].Process(buttons[2].Pressed()) * lPortOut );
        output += Bowls[3].Process(
                filteredNoise[3] *  simpleEnv[3].Process(buttons[3].Pressed()) * rPortOut );
        output += Bowls[4].Process(
                filteredNoise[4] *  simpleEnv[4].Process(buttons[4].Pressed()) * rPortOut );
        output += Bowls[5].Process(
                filteredNoise[5] *  simpleEnv[5].Process(buttons[5].Pressed()) * rPortOut );


        // Set the Output to the Bowls
        out[0][i] = output * 0.16666f;
        out[1][i] = output * 0.16666f;
    }
}

int main(void) {
    hw.Init();
    hw.SetAudioBlockSize(4); // number of samples handled per callback
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    hw.StartAudio(AudioCallback);
    hw.StartLog(false);

    // Simple Test to see if the logger works
    //Logger<LOGGER_INTERNAL>::PrintLine("Patch Init");

//    buttons[0].Init(daisy::DaisySeed::GetPin(28), 1000);
//    buttons[1].Init(daisy::DaisySeed::GetPin(27), 1000);
//    buttons[2].Init(daisy::DaisySeed::GetPin(26), 1000);
//    buttons[3].Init(daisy::DaisySeed::GetPin(25), 1000);
//    buttons[4].Init(daisy::DaisySeed::GetPin(24), 1000);
//    buttons[5].Init(daisy::DaisySeed::GetPin(23), 1000);

    for (int i = 0; i < NUM_BOWLS; i++) {
        Bowls[i].Init(hw.AudioSampleRate(), bowlFreqs[i], 0.45f);
        buttons[i].Init(daisy::DaisySeed::GetPin(28 - i), 1000);
        LowPassTable[i].Init(hw.AudioSampleRate());
        LowPassTable[i].SetFreq(bowlFreqs[i]);
    }


    allpassTest.Init(hw.AudioSampleRate(), fType, 440.f, 2);
    NoiseSource.Init();

    setADSR(hw.AudioSampleRate());
    LowpassOut.Init(hw.AudioSampleRate());
    LowpassOut.SetFreq(492.f * 2.0f);

    ExciterTable.Init(0, 0.5, 0, 1);

    TouchPads.Init(Mpr121<Mpr121I2CTransport>::Config());
    lPort.Init(hw.AudioSampleRate(), 0.125f);
    rPort.Init(hw.AudioSampleRate(), 0.125f);


    SineWave.Init(hw.AudioSampleRate());
    SineWave.SetWaveform(daisysp::Oscillator::WAVE_SIN);
    SineWave.SetFreq(0.25f);

    while (1) {
//        buttons[3].Debounce();
        hw.SetLed(TouchPads.Touched());

        touchState = TouchPads.FilteredData(0);
        touchOutL = 0;
        touchOutR = 0;
//        touchRight = TouchPads.ReadRegister16(11);
//        char byte = touchLeft;
//
//        if (!isTouched and TouchPads.Touched()) {
            for (int i = 16; i > 0; i--) {
                if ((touchState & 0x8000) >> 15 == 1) {
                    if (i <= 4) {
                        touchOutL = i;
                    }

                    if (i >= 5) {
                        touchOutR = i - 4;
                    }

                }
                touchState <<= 1;
            }
//            Logger<LOGGER_INTERNAL>::Print("%d %d", touchOutL, touchOutR);
//            Logger<LOGGER_INTERNAL>::PrintLine("/n");
//            isTouched = true;

//        } else if (isTouched and !TouchPads.Touched()) {
//            isTouched = false;
//        }


    }
}


