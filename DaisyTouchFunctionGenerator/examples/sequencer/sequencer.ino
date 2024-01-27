// Sequencer Example for DaisyTouchFunctionGeneartor

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
static Oscillator osc;

// Define note sequence.
constexpr size_t kNumNotes = 16;
static float notes[kNumNotes] = {
	220.00,
	261.63,
	293.66,
	311.13,
	329.63,
	392.00,
	220.00 * 2.0,
	261.63 * 2.0,
	293.66 * 2.0,
	311.13 * 2.0,
	329.63 * 2.0,
	392.00 * 2.0,
	220.00 * 4.0,
	261.63 * 4.0,
	293.66 * 4.0,
	311.13 * 4.0,
};

///////////////////////////////////////////////////////////////
///////////////////// AUDIO CALLBACK (PATCH) //////////////////
void AudioCallback(float** in, float** out, size_t size) {
	// Update generator properties.
	generator.SetFreq(powf(f_knob.Process(), 4) * 5.0f);
	generator.SetSmooth(s_knob.Process());
	generator.SetRange(0.0f, c_knob.Process() * (float(kNumNotes) - 0.00000001f));
	for (size_t i = 0; i < size; i++) {
		osc.SetFreq(notes[int(generator.Process())]);
		float value = filter.Process(osc.Process());
		out[0][i] = value;
		out[1][i] = value;
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
	generator.SetRange(0.0, float(kNumNotes) - 0.0001f);
	// Allow generator debug prints
	generator.setDebug(true);

	osc.Init(sample_rate);
	osc.SetWaveform(Oscillator::WAVE_SAW);
	osc.SetAmp(1.0f);

	filter.Init(sample_rate);
	filter.SetFreq(800.0f);

	// BEGIN CALLBACK
	DAISY.begin(AudioCallback);
}

void loop() {
	// Update generator function if necessary
	generator.Update();

	delay(4);
}
