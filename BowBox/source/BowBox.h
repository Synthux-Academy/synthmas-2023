//
// Created by Holland Sersen on 12/30/23.
//

#pragma once
#include "daisysp.h"
#include <cmath>
#include "CanonicalFirstOrderFilters.h"

using namespace daisy;
using namespace daisysp;

float friction_curve(float staticFriction, float dynamicFriction, float initialVelocity, float relativeVelocity){
    float totalVelocity = (initialVelocity + relativeVelocity);
    if (totalVelocity == 0)
        return 0;
    else
        return (dynamicFriction + ((staticFriction - dynamicFriction) * initialVelocity) / totalVelocity);
}

template <typename T, size_t max_size>
class FractionalDelay  {
    float delayLength = 0.0f;
    float M = 0.0f;
    float frac = 0.0f;

public:
    void Init(){

    }
};

class ResonantBandpass{
    float sampleRate;
    float frequency;
    float bandWidth;

    //Coefficeints
    float b_ = 0.f;
    float a1_ = 0.f;
    float a2_ = 0.f;

    // Last Inputs and Outputs
    float x1_ = 0.f;
    float x2_ = 0.f;
    float y1_ = 0.f;
    float y2_= 0.f;

    //Calculate Filter Coefficients Here
    void CalculateCoefficients() {
        const float R = 1.0f - (bandWidth / 2);
        const float omega = std::cos(frequency);
        //const float w = 2 * PI_F * frequency;
        const float theta = ((2 * R) / (1 + R * R)) * omega;

        //Set Direct Form Filter Values
        b_ = 2 * R * theta;
        a1_ = R * R;
        a2_ = R;
    }

public:
    void Init(float Fs, float Freq, float Bandwidth){
        this->sampleRate = Fs;
        this->frequency = Freq;
        this->bandWidth = Bandwidth;
    }

    //Set Feq of Filter
    void SetFreq(float Freq){this->frequency = Freq;  CalculateCoefficients();}

    // Set Badnwdith of Filter
    void SetBandwidth(float BandWidth){this->bandWidth = BandWidth; CalculateCoefficients();}

    // Second Order Resonant Bandpass Filter
    float Process(float input){
        float output = b_ * y1_ - a1_ * y2_ - a2_ * x2_ + input;

        //Set Last Input Values
        x2_ = x1_;
        x1_ = input;

        //Set Feedback Values
        y2_ = y1_;
        y1_ = output;

        return output;
    }
};


class TunedWaveGuide {
    float sampleRate;
    float frequency;
    float couplingValue = 0.98f;
    float feedback = 0.0f;
    float waveSpeed = 0.0f;

    //Delay Line for Feedback
    //It shouldn't go over 1500 unless you go lower than 30hz
    daisysp::DelayLine<float, 1500> delayLine;

    //Band pass filter
    ResonantBandpass resFilt;

    CanonicalFirstOrderFilters allpass;

    Svf lowpass;
public:

    void setFreq(float Freq){this->frequency = Freq;
        resFilt.SetFreq(frequency);
        delayLine.SetDelay(sampleRate / frequency);
        lowpass.SetFreq(Freq);
    }

    void setCoupling(float Couple){this->couplingValue = Couple;}


    void Init(float Fs, float Freq){
        this->sampleRate = Fs;
        this->frequency = Freq;
        this->waveSpeed = 343000.0f / this->sampleRate;

        FilterType tempType = Allpass;

        //Init Objects
        resFilt.Init(Fs, Freq, 2);
        lowpass.Init(Fs);
        lowpass.SetFreq(Freq);
        allpass.Init(Fs, tempType, frequency, 2);
        delayLine.Init();
        delayLine.SetDelay(sampleRate / frequency);
    }

    float Process(float input){
        // Mix Source with Feedback
//        lowpass.Process(input);
//        const float newInput = lowpass.Low();

        const float source = input + feedback;

        // Get the Allpass output from Input
        //const float allpassOut = allpass.Process(source);

        // Calculate BandPass Out
        float bandpassOut = resFilt.Process(source);

        // Write Bandpass to Delay Line
        delayLine.Write(bandpassOut);

        //Get Output from delay line
        float output = delayLine.Read();

        // TODO: Fix this. Currently the friction properties sound like a bow on a bowl and is still a bit too noisy.
        couplingValue = friction_curve(0.96f, 0.9f, abs(input) * waveSpeed, abs(input + output) * waveSpeed);

        // Calculate feedback output
        feedback = output * couplingValue;

        return output;
    }
};

class BowedString {
// Init Values
    float sampleRate = 0.0f;
    float frequency = 0.0f;
    float beats = 0.0f;

    // Init the tuned waveguides
    TunedWaveGuide under_modals[4], over_modals[4];

public:
    void inline Init(float Fs, float Freq, float Beats){
        this->sampleRate = Fs;
        this->frequency = Freq;
        this->beats = Beats;

        // Init Modals
        for (int i = 0; i < 3; i++) {
            under_modals[i].Init(this->sampleRate, Freq + Beats);
            over_modals[i].Init(this->sampleRate, Freq - Beats);
        }
    }

    void inline setFreqs(float Freq, float Beats){
        this->frequency = Freq;
        this->beats = Beats;

        for (int i = 0; i < 3; i++) {
            under_modals[i].setFreq(frequency + beats);
            over_modals[i].setFreq(frequency - beats);
        }
    }

    inline float getFrequency() const{return this->frequency;}

    inline float getBeats() const{return this->beats;}

    inline float Process(float input){
        // Process first delay line with input
        float under_output = under_modals[0].Process(input);
        under_output += under_modals[1].Process(under_output) * -1.0f;
        //under_output += under_modals[2].Process(under_output) * -1.0f;
        //under_output += under_modals[3].Process(under_output) * -1.0f;


        float over_output = over_modals[0].Process(input) * -1.f;
        over_output += over_modals[1].Process(over_output) * -1.0f;
        //over_output += over_modals[2].Process(over_output) * -1.0f;
        //over_output += over_modals[3].Process(over_output) * -1.0f;

        // Process all of the other filters
//        for (int i = 0; i < 3; i++) {
//
//        }
//        over_output *= 0.25f;
//        under_output *= 0.25f;

        // Return output and manage gain
        return (under_output + over_output);
    }

};

