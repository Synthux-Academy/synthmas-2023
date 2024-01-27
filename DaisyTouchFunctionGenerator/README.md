# DaisyTouchFunctionGenerator

## Intro
An arduino library for a touch based function generator used to produce sounds using Daisy Seed and MPR121.
This was created as part of [Synthux](https://www.synthux.academy/) 2023 Synthmas End of Year Contest.
[See this video for a demonstration of some functionallity](https://youtu.be/KqnuhWOUvzw)

This directory introduces a header file defining a function generator / waveform generator where the wave form is created by using a MPR121 touch (capacity) sensor.

## What does it do?
This functionallity can generate both "classic" waveforms like SQUARE, SAW, TRIANGLE, but more interestingly it can generate new and exciting waveforms in a creative, intuitive, less controlled manner. \

These in turn can be used in many ways, some examples:
- As waveforms for "oscilators" generating sound.
- As waveforms for "lfo"s patched into other components such as filters.
- As functions for arppegiators or sequencer.

## Installation
Download or clone the entire repository into your arduino library directory. \
You should then be able to include the library in any sketch, and the code examples will have the examples for this library as well.

## Generating a function/waveform

The waveform is reset every time a new "touch sequence" occurs, and is defined by the duration of time each pad is touched.

- A touch sequence starts when a pad has been touch after no pads are touched.
- A touch sequence ends when no more pads are being touched.

for example, to create a "square" waveform, touch the four first pads simultaneously and then let go of them all at once.
Set smoothing to the lowest value.
You can control the square waves "pulse width" by touching a different number of pads.

4 first pads = 0.5 pulse width.
2 first pads = 0.25 pulse width.
etc.

** But it is much more interesting to play around and create unique waveforms and functions by touching different pads for variable durations **

## "Segmented" Smothing
An aditional functionallity is variable smoothing of the waveform, when no smoothing is applied the function is just 8 values, but while increasing the smoothing factor it's "smeared" across more and more values until it simmulates a smooth curve.


## What's in the box
This repository contains an Arduino Library for the Touch Function Generator and the following examples

### Oscillator Waveform
An example for using the TouchGenerator as sort of a wavefrom generator sort of an oscilator to generate sounds.

- At low frequencies (<30Hz) it sounds like a blip generating machine, where the touch sequence controls the pitch and arrangement of those bleeps.
- At audible frequencies it functions as a unique oscilator where the waveform is conroled by the touch sequence.

[See this video for a demonstration of this functionallity](https://youtu.be/KqnuhWOUvzw)

#### Wiring
The code example expects a [Synthux Simple Fixed Synth](https://www.synthux.academy/shop/kit-simple-fix) with an MP121 touch sensor connected to i2c4 pins. It can be easily adjusted to any daisy seed.

#### Explicit Wiring
| Daisy pin | SimpleSynth Socket | Function
|-----------|--------------------|---------
|    A2     |        S32         |  Master Volume Potentiometer
|    A1     |        S31         |  Frequency Potentiometer
|    A0     |        S30         |  Smoothing Potentiometer
|    D14    |                    |  MR121 SDA pin
|    D13    |                    |  MR121 SCL pin

### LFO Filter.

An example of using the generator as an LFO to control the cuttoff of a filter over the input audio.

I'll hopefully add more examples to this repository.

#### Wiring
The code example expects a [Synthux Simple Fixed Synth](https://www.synthux.academy/shop/kit-simple-fix) with an MP121 touch sensor connected to i2c4 pins. It can be easily adjusted to any daisy seed.

#### Explicit Wiring
| Daisy pin | SimpleSynth Socket | Function
|-----------|--------------------|---------
|    A2     |        S32         |  Filter cutoff range
|    A1     |        S31         |  Frequency Potentiometer
|    A0     |        S30         |  Smoothing Potentiometer
|    D14    |                    |  MR121 SDA pin
|    D13    |                    |  MR121 SCL pin

### Sequencer
An example for using the TouchGenerator as a sequencer selecting notes out of a predefined
array of notes.
Smoothing the generated function causes more notes being played between the main click notes.

#### Wiring
The code example expects a [Synthux Simple Fixed Synth](https://www.synthux.academy/shop/kit-simple-fix) with an MP121 touch sensor connected to i2c4 pins. It can be easily adjusted to any daisy seed.

#### Explicit Wiring
| Daisy pin | SimpleSynth Socket | Function
|-----------|--------------------|---------
|    A2     |        S32         |  Distance from lowest to highest note
|    A1     |        S31         |  Sequencer Tempo Potentiometer
|    A0     |        S30         |  Smoothing Potentiometer
|    D14    |                    |  MR121 SDA pin
|    D13    |                    |  MR121 SCL pin

## Attribution
This repository contains and uses a slightly edited version of [simple-daisy-touch.h](https://github.com/Synthux-Academy/simple-examples-touch/blob/main/daisyduino/simple-drum-machine-touch/simple-daisy-touch.h) and [aknob](https://github.com/Synthux-Academy/simple-examples-touch/blob/main/daisyduino/simple-drum-machine-touch/aknob.h) created by [bleeptools](https://github.com/bleeptools). The main changes were to allow determining i2c pins, and enabling determening in the _on_touch callback whether after it's done the sensor will be in a state of touch or untouched. 
