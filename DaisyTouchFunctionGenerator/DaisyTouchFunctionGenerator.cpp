#include "DaisyTouchFunctionGenerator.h"

#include "simple-daisy-touch.h"
#include <DaisyDuino.h>
#include <Adafruit_MPR121.h>
#include <Arduino.h>

using namespace synthux;
using namespace simpletouch;

namespace touchgenerator {

	namespace {

		// A "segemented" linear interpulation function.
		// while a classic lerp function will give you a value between a and b in proportion to x,
		// this function simulates dividing the range between a and b to s segements, transition from a to b
		// and returns the value of the bucket where x falls.
		// if s <= 1, will return a,
		// if s is infinity, will behave like classic lerp function. 
		// This was meant to work with a, b and x between 0 and 1, and with s >= 1.
		// Other values might have undefined behavior.
		float slerp(float a, float b, float x, float s) {
			if (s <= 1) return a;
			return (a * s + floor(x * s * (b - a) / 10.0) * 10.0) / s;
		}

	}  // namespace


	void TouchGenerator::Init(float sample_rate, int scl, int sda) {
		sample_rate_ = sample_rate;

		osc_.Init(sample_rate_);
		osc_.SetWaveform(Oscillator::WAVE_RAMP);
		osc_.SetAmp(0.5f - 0.001f);
		osc_.SetFreq(880.0f);

		touch_.Init(scl, sda);
		touch_.SetOnTouch([&](uint16_t pad) -> void {OnPadTouch(pad); });
		touch_.SetOnRelease([&](uint16_t pad) -> void {OnPadRelease(pad); });
	}

	// Init with default i2c connections (daisy i2c4).
	void TouchGenerator::Init(float sample_rate) {
		Init(sample_rate, D13, D14);
	}

	void TouchGenerator::OnPadTouch(uint16_t pad) {
		const bool has_touch = touch_.HasTouch();
		if (!recording_touch_sequence && has_touch) {
			if (debug_) {
				Serial.println("Recording Function");
			}
			for (int i = 0; i < kNumSegments; i++) {
				vals_[i] = 0;
				starts_[i] = 0;
			}
		}
		if (pad < kNumSegments) {
			starts_[pad] = millis();
		}
		recording_touch_sequence = has_touch;
	}

	void TouchGenerator::OnPadRelease(uint16_t pad) {
		const bool has_touch = touch_.HasTouch();
		if (recording_touch_sequence && !has_touch) {
			size_t cur_max = 0;
			size_t cur_millis = millis();
			for (int i = 0; i < kNumSegments; i++) {
				vals_[i] = starts_[i] == 0 ? 0 : cur_millis - starts_[i];
				if (vals_[i] > cur_max) cur_max = vals_[i];
			}
			if (cur_max == 0) {
				if (debug_) {
					Serial.print("Did not touch pads from 0 to ");
					Serial.print(kNumSegments - 1);
					Serial.println(". Not changing function.");
				}
				return;
			}

			for (int i = 0; i < kNumSegments; i++) {
				cur_func_[i] = float(vals_[i]) * (max_val_ - min_val_) / float(cur_max) + min_val_;
			}

			if (debug_) {
				// Print generated function
				const int print_rows = 5;
				const float row_span = (max_val_ - min_val_) / float(print_rows);
				bool negative = false;
				for (int row = 0; row < print_rows; row++) {
					const float cur_mark = max_val_ - row * row_span;
					bool print_zero = false;
					if (cur_mark <= 0 && !negative) {
						negative = true;
						print_zero = true;
					}
					for (int i = 0; i < kNumSegments; i++) {
						if ((cur_func_[i] >= cur_mark && !negative) ||
							(cur_func_[i] <= cur_mark && negative)) {
							// print full block.
							Serial.write(0xE2);
							Serial.write(0x96);
							Serial.write(0x88);
						}
						else {
							// print empty block.
							Serial.write(0xE2);
							Serial.write(0x96);
							Serial.write(0x91);
						}
					}
					Serial.print(" ");
					if (row == 0) {
						Serial.print(max_val_, 2);
					}
					else if (row == print_rows - 1) {
						Serial.print(min_val_, 2);
					}
					else if (print_zero) {
						Serial.print("0.00");
					}

					Serial.println();
				}

				Serial.print("[");
				for (int i = 0; i < kNumSegments; i++) {
					Serial.print(cur_func_[i], 2);
					if (i < kNumSegments - 1) Serial.print(",");
				}
				Serial.println("]");
			}
		}
		recording_touch_sequence = has_touch;
	}

	float TouchGenerator::Process() {
		cur_pos_ = (osc_.Process() + 0.5f) * float(kNumSegments);

		float prev = cur_func_[int(cur_pos_)];
		float next = cur_func_[int(cur_pos_ + 1) % kNumSegments];
		float interpulation = cur_pos_ - int(cur_pos_);

		return slerp(prev, next, interpulation, smoothing_) * amp_;
	}

	void TouchGenerator::SetRange(float min_val, float max_val) {
		min_val_ = min_val;
		max_val_ = max_val;
	}


	void TouchGenerator::SetFreq(float f) {
		osc_.SetFreq(f);
	}

	void TouchGenerator::SetAmp(float a) {
		amp_ = a;
	}

	void TouchGenerator::SetSmooth(float s) {
		smoothing_ = powf(s, 4) * 1800.0f + 1.00001f;
	}

	void TouchGenerator::Update() {
		touch_.Process();
	}
}  // namespace touchgenerator
