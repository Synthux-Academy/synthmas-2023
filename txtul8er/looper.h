#pragma once

#include "Arduino.h";

namespace synthux {

class Looper {
  public:
    void Init(float *buf, size_t length) {
      _buffer = buf;
      _buffer_length = length;
            // Reset buffer contents to zero
      memset(_buffer, 0, sizeof(float) * _buffer_length);
      // Setup the playheads
      for(int i = 0; i < _grain_count; i++){
        _grain_pos[i] = _rec_head + (i + 1) * _buffer_length / (_grain_count + 1); 
      }
    }

    void setTaps(uint8_t touched, const float grain_env){
      _env_step = fmap(grain_env, _min_env_step, _max_env_step);
      
      for (uint8_t i = 0; i < _grain_count; i++){
        if(touched & 1 << i && _envelope[i] < _env_range){
          _envelope[i] += _env_step;
        }
        else if (!(touched & 1 << i) && _envelope[i] > 0){
          _envelope[i] -= _env_step;
        }

      }
    }


    float Process(float in) {
      // Record to the buffer
      _buffer[_rec_head] = in;
      _rec_head++;
      _rec_head %= _buffer_length;

      // Playback from the buffer
      _output = 0;
      for(int i = 0; i < _grain_count; i++){
        _output += _buffer[_grain_pos[i]] * _envelope[i] / _env_range;
        _grain_pos[i]++;
        _grain_pos[i] %= _buffer_length;
        }
      _output = _output * sqrt(_grain_count);
      return _output;

    }

  private:

    float* _buffer;

    size_t _buffer_length = 0;

    static const int _grain_count = 4;
    size_t _grain_pos[_grain_count] = {0}; 
    size_t _rec_head  = 0;

    int _envelope[_grain_count] = {0};
    static const int _min_env_step = 2;
    static const int _max_env_step = 100;
    static const int _env_range = 1000;

    float _env_step = _min_env_step;

    float _output = 0;
    float _mix = sqrt(_grain_count);
    bool _is_empty  = true;
};
};