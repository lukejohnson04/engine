#pragma once

#include <random>

struct RandomState {
    std::mt19937 randomEngine;
    std::uniform_int_distribution<std::mt19937::result_type> distribution;
};

global_variable RandomState *random_engine_state;

namespace Random {
    internal_function
    void Init() {
        random_engine_state->randomEngine.seed(std::random_device()());
    }

    internal_function
    inline float Float() {
        return (float)random_engine_state->distribution(random_engine_state->randomEngine) / (float) std::numeric_limits<u32>::max();
    }

    internal_function
    inline float Float(float min, float max) {
        return (Random::Float() * (max-min)) + min;
    }
}
