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
static AKnob a_knob(A(S32));
static AKnob s_knob(A(S30));

////////////////////////////////////////////////////////////
/////////////////// Generator instance /////////////////////
TouchGenerator generator;

///////////////////////////////////////////////////////////////
///////////////////// AUDIO CALLBACK (PATCH) //////////////////
void AudioCallback(float** in, float** out, size_t size) {
	// Update generator properties.
	generator.SetFreq(powf(f_knob.Process(), 4) * 2000.0);
	generator.SetSmooth(s_knob.Process());
	// Get master volume.
	const float masterVolume = a_knob.Process();

	for (size_t i = 0; i < size; i++) {
		const float cur_val = generator.Process();
		out[0][i] = cur_val * masterVolume;
		out[1][i] = cur_val * masterVolume;
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
	// Allow generator debug prints
	generator.setDebug(true);

	// BEGIN CALLBACK
	DAISY.begin(AudioCallback);
}

void loop() {
	// Update generator function if necessary
	generator.Update();

	delay(4);
}
