//
// Created by Holland Sersen on 1/13/24.
//

#pragma once
#include <math.h>
#include "daisysp.h"

namespace daisysp {

    enum FilterType{
        Lowpass = 0,
        Bandpass = 1,
        Highpass = 2,
        Bandreject = 3,
        Allpass = 4
    };

// DAFX PG. 50
    class CanonicalFirstOrderFilters {
        int fType;
        float sampleRate;
        float frequency;
        float qFactor;

        float K;
        float Q;
        float b0, b1, b2;
        float a1, a2;
        float xh1, xh2;

        void calculate_coefficients() {
            switch (fType) {
                case Allpass: {
                    K = tan((2.f * 3.14159f * frequency) / sampleRate);
                    float K2 = K * K;
                    Q = frequency / qFactor;

                    b0 = (K2 * Q - K + Q) / (K2 * Q + K + Q);
                    b1 = (2.f * Q * (K2 - 1.f)) / (K2 * Q + K + Q);
                    b2 = 1.f;
                    a1 = b1;
                    a1 = b0;
                    break;
                }

                case Lowpass: {
                    break;
                }
                case Bandpass: {
                    break;
                }
                case Highpass: {
                    break;
                }
                case Bandreject: {
                    break;
                }
            }
        }

    public:
        void Init(float sample_rate, int type, float frequency, float bandwidth) {
            fType = type;
            this->sampleRate = sample_rate;
            this->frequency = frequency;
            this->qFactor = bandwidth;

            calculate_coefficients();
        }

        float Process(const float input) {
            // Process Filter
            float xh = input - a1 * xh1 - a2 * xh2;
            float output = b0 * xh + b1 * xh1 + b2 * xh2;

            // Set Feedback
            xh2 = xh1;
            xh1 = xh;

            return output;
        }

        void setFrequency(const float frequency){this-> frequency = frequency; calculate_coefficients();}

        void setQ(const float Q){this-> qFactor = Q; calculate_coefficients();}

    };

} // daisysp


