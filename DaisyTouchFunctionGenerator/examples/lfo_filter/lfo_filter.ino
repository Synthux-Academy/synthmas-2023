// Oscilator Example for DaisyTouchFunctionGeneartor

///////////////////////////////////////////////////////////////
///////////////////// LIBRARIES & HARDWARE ////////////////////
#include <DaisyTouchFunctionGenerator.h>

#include "simple-daisy-touch.h"
#include "aknob.h"
#include "DaisyDuino.h"

using namespace synthux;
using namespace simpletouch;
using namespace touchgenerator;

////////////////////////////////////////////////////////////
////////////// KNOBS, SWITCHES, PADS, JACKS ////////////////
static AKnob f_knob(A(S31));
static AKnob c_knob(A(S32));
static AKnob s_knob(A(S30));

////////////////////////////////////////////////////////////
/////////////////// Generator instance /////////////////////
static TouchGenerator generator;
static MoogLadder filter;

///////////////////////////////////////////////////////////////
///////////////////// AUDIO CALLBACK (PATCH) //////////////////
void AudioCallback(float** in, float** out, size_t size) {
	// Update generator properties.
	generator.SetFreq(powf(f_knob.Process(), 4) * 3.0f);
	generator.SetSmooth(s_knob.Process());
	// Update filter properties
	float max_cutoff = c_knob.Process() * 1023.0f;
	for (size_t i = 0; i < size; i++) {
		filter.SetFreq(1023.0 - max_cutoff * generator.Process());
		out[0][i] = filter.Process(in[0][i]);
		out[1][i] = filter.Process(in[1][i]);
	}
}

///////////////////////////////////////////////////////////////
///////////////////// SETUP ///////////////////////////////////

void setup() {
	Serial.begin(9600);
	// SETUP DAISY
	DAISY.init(DAISY_SEED, AUDIO_SR_48K);
	auto sample_rate = DAISY.get_samplerate();

	// Init generator.
	generator.Init(sample_rate);
	generator.SetRange(0.0, 1.0);
	// Allow generator debug prints
	generator.setDebug(true);

	filter.Init(sample_rate);

	// BEGIN CALLBACK
	DAISY.begin(AudioCallback);
}

void loop() {
	// Update generator function if necessary
	generator.Update();

	delay(4);
}
