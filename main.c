/**
 * Copyright (c) 2022 Asaf Zebulon
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#include "model.h"

/* Synthetic data generation for testing */
static float getSynthData(float x, float A, float b, bool noise) {
    float n;
    if (noise) {
        n = (float)(rand() % ((int)(A))) * 0.1;
    } else {
        n = 0;
    }
    float y = A * sinf(2 * 3.1418 * 0.02 * x + 5) + b;
    y = (y < b) ? b : y;
    y += n;
    return y;
}

int main(int argc, char *argv[]) {

    /* Hyperparameters */
    uint32_t win_sizes[2] = {30, 30}; // 0 means INF win_size, may be unsafe
    float initial_values[2] = {64.0, 192.0}; // Initial cluster starting point

    /* Example parameter */
    const float BIAS = 50;
    const float AMPLITUDE = 150;
    const float PEAK_VALUE = 255;

    printf("Initial state\n==============================\n");
    model_t *new = modelCreate(win_sizes, initial_values, 2);
    modelShow(*new);

    printf("\nTraining-phase\n==============================\n");

    // srand(time(NULL));
    srand(0);
    printf("[InputValue, RescaledValue]\n");
    float this_val, rebias_val;
    for (int i = 0; i < 100; i++) {
        this_val = getSynthData(i, AMPLITUDE, BIAS, true);
        modelUpdate(new, this_val);
        // rebias_val = modelTransform(*new, this_val, 0); // Rescale lower limit only
        rebias_val = modelTransform(*new, this_val, PEAK_VALUE); // Rescale upper limit as well
        printf("[%.3f, %.3f]\n", this_val, rebias_val);
    }

    printf("\n");
    printf("Updated state\n==============================\n");
    modelShow(*new);

    modelDelete(new);

    return 0;
}