//
// Created by Holland Sersen on 1/13/24.
//


#pragma once

// Make it a struct so that it is easy to change
struct BowTable {
    float offset = 0.0f;
    float slope = 0.0f;
    float minOutput= 0.0f;
    float maxOutput = 0.0f;

    void Init(float offset_, float slope_, float min_, float max_){
        this->offset = offset_;
        this->slope = slope_;
        this->minOutput = min_;
        this->maxOutput = max_;
    }

    //TODO: Get Refersence. Taken from STK
    float Process(float input){
        float sample = input + offset;
        sample *= slope;

        float output = fabs(sample + 0.75);
        output = pow(output, -4.0f);

        if (output < minOutput)
            output = minOutput;
        if (output > maxOutput)
            output = maxOutput;

        return output;
    }


};