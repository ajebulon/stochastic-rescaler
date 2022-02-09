/**
 * Copyright (c) 2022 Asaf Zebulon
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "model.h"

/* Stochastic update mean */
void stochasticMeanUpdate(stochastic_mean_t *this, float val) {
    if (this->win_size == 0) {
        this->mean = (1 / ((float)(this->count) + 1)) * ((float)(this->count) * this->mean + val);
        this->count++;
    } else {
        if (this->count >= this->win_size) {
            this->mean = (1 / ((float)(this->win_size) + 1)) * (this->win_size * this->mean + val);
        } else {
            this->mean = (1 / ((float)(this->count) + 1)) * ((float)(this->count) * this->mean + val);
            this->count++;
        }
    }
}

/* Stochastic update var */
void stochasticVarUpdate(stochastic_var_t *this, float val) {
    if (this->win_size == 0) {
        this->mean = (1 / ((float)(this->count) + 1)) * ((float)(this->count) * this->mean + val);
        this->var = ((float)(this->count) / ((float)(this->count) + 1) * this->var) + (float)(this->count) * powf(((val - this->mean) / ((float)(this->count) + 1)), 2);
        this->count++;
    } else {
        if (this->count >= this->win_size) {
            this->mean = (1 / ((float)(this->win_size) + 1)) * (this->win_size * this->mean + val);
            this->var = (this->win_size / (this->win_size + 1) * this->var) + this->win_size * powf(((val - this->mean) / (this->win_size + 1)), 2);
        } else {
            this->mean = (1 / ((float)(this->count) + 1)) * ((float)(this->count) * this->mean + val);
            this->var = ((float)(this->count) / ((float)(this->count) + 1) * this->var) + (float)(this->count) * powf(((val - this->mean) / ((float)(this->count) + 1)), 2);
            this->count++;
        }
    }
}

/* Create stochastic mean object */
stochastic_mean_t *sMeanCreate(uint32_t win_size, float initial_value) {
    stochastic_mean_t *new = (stochastic_mean_t *)malloc(sizeof(stochastic_mean_t));
    new->win_size = win_size;
    new->mean = initial_value;
    new->count = (initial_value > 0) ? 1 : 0;

    return new;
}

/* Create stochastic var object */
stochastic_var_t *sVarCreate(uint32_t win_size, float initial_value) {
    stochastic_var_t *new = (stochastic_var_t *)malloc(sizeof(stochastic_var_t));
    new->win_size = win_size;
    new->mean = initial_value;
    new->var = 0;
    new->count = (initial_value > 0) ? 1 : 0;

    return new;
}


/* Cluster related functions 
 * Each cluster store stochastic mean and stochastic var
 */

/* Create cluster object */
cluster_t *clusterCreate(uint32_t win_size, float initial_value) {
    cluster_t *new = (cluster_t *)malloc(sizeof(cluster_t));

    new->win_size = win_size;
    new->initial_value = initial_value;
    
    stochastic_mean_t *smean = sMeanCreate(win_size, initial_value);
    stochastic_var_t *svar = sVarCreate(win_size, initial_value);

    new->smean = smean;
    new->svar = svar;

    return new;
}

/* Update existing cluster (training) */
void clusterUpdate(cluster_t *this, float val) {
    stochasticMeanUpdate(this->smean, val);
    stochasticVarUpdate(this->svar, val);
}

/* Show cluster information */
void clusterShow(cluster_t this) {
    printf("Cluster [CNT / AVG / STD]: [%d / %.3f / %.3f]\n", this.smean->count, this.smean->mean, sqrtf(this.svar->var));
}

/* Delete cluster */
void clusterDelete(cluster_t *this) {
    free(this->svar);
    free(this->smean);
    free(this);
}

/* Get cluster distance */
float getDistanceToCluster(cluster_t *this, float val) {
    float to_loc = this->smean->mean;
    return fabsf(val - to_loc);
}


/* Model related functions 
 * Each model store multiple clusters. Each value input to model
 * will update the cluster located closest to the input value 
 */

/* Create model object */
model_t *modelCreate(uint32_t win_sizes[], float initial_values[], uint8_t count) {
    model_t *new = (model_t *)malloc(sizeof(model_t));
    
    cluster_t **new_clusters = (cluster_t **)malloc(count * sizeof(cluster_t*));
    for (int cluster_idx = 0; cluster_idx < count; cluster_idx++) {
        new_clusters[cluster_idx] = clusterCreate(win_sizes[cluster_idx], initial_values[cluster_idx]);
    }

    new->count = count;
    new->clusters = new_clusters;

    return new;
}

/* Update model (and clusters) based on input value */
void modelUpdate(model_t *this, float val) {
    float this_distance, distance_min = 0;
    int distance_min_idx = 0;
    for (int cluster_idx = 0; cluster_idx < this->count; cluster_idx++) {
        this_distance = getDistanceToCluster(this->clusters[cluster_idx], val);
        if (cluster_idx == 0) {
            distance_min = this_distance;
            distance_min_idx = cluster_idx;
        } else {
            if (this_distance < distance_min) {
                distance_min = this_distance;
                distance_min_idx = cluster_idx;
            }
        }
    }

    // Update cluster located closest to input value
    clusterUpdate(this->clusters[distance_min_idx], val);
}

/* Predict rescaled value using input model 
 * Set peak_value to 0 to disable peak rescaling 
 */
float modelTransform(model_t this, float val, float peak_value) {
    // Search for lower cluster
    float min_mean = 0;
    int min_mean_idx = 0;
    for (int cluster_idx = 0; cluster_idx < this.count; cluster_idx++) {
        float this_mean = this.clusters[cluster_idx]->smean->mean;
        if (cluster_idx == 0) {
            min_mean = this_mean;
            min_mean_idx = cluster_idx;
        } else {
            if (this_mean < min_mean) {
                min_mean = this_mean;
                min_mean_idx = cluster_idx;
            }
        }
    }
    cluster_t *cluster_lo = this.clusters[min_mean_idx];

    // Search for higher cluster
    float max_mean = 0;
    int max_mean_idx = 0;
    for (int cluster_idx = 0; cluster_idx < this.count; cluster_idx++) {
        float this_mean = this.clusters[cluster_idx]->smean->mean;
        if (cluster_idx == 0) {
            max_mean = this_mean;
            max_mean_idx = cluster_idx;
        } else {
            if (this_mean > max_mean) {
                max_mean = this_mean;
                max_mean_idx = cluster_idx;
            }
        }
    }
    cluster_t *cluster_hi = this.clusters[max_mean_idx];

    // Calculate boundary
    float llim = cluster_lo->smean->mean - 1 * sqrtf(cluster_lo->svar->var);
    float ulim = cluster_hi->smean->mean + 1 * sqrtf(cluster_hi->svar->var);

    // Rescaling
    float rescaled_val;
    if (peak_value == 0) {
        rescaled_val = (val - llim) * (ulim / (ulim - llim));
        rescaled_val = (rescaled_val < 0) ? 0 : rescaled_val;
    } else {
        rescaled_val = (val - llim) * (ulim / (ulim - llim)) * peak_value / ulim;
        rescaled_val = (rescaled_val < 0) ? 0 : rescaled_val;
        rescaled_val = (rescaled_val > peak_value) ? peak_value : rescaled_val;
    }

    return rescaled_val;
}

/* Update model and return rescaled value prediction */
float modelUpdateTransform(model_t *this, float val, float peak_value) {
    modelUpdate(this, val);
    modelTransform(*this, val, peak_value);
}

/* Show statistics of current model */
void modelShow(model_t this) {
    printf("%d-clusters\n", this.count);
    for (int cluster_idx = 0; cluster_idx < this.count; cluster_idx++) {
        clusterShow(*(this.clusters[cluster_idx]));
    }
}

/* Delete model object */
void modelDelete(model_t *to_delete) {
    for (int i = 0; i < to_delete->count; i++) {
        clusterDelete(to_delete->clusters[i]);
    }
    free(to_delete->clusters);
    free(to_delete);
}