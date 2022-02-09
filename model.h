// Copyright (c) 2022 Asaf Zebulon
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>

typedef struct stochastic_mean {
    uint32_t win_size;
    uint32_t count;
    float mean;
} stochastic_mean_t;

typedef struct stochastic_var {
    uint32_t win_size;
    uint32_t count;
    float mean;
    float var;
} stochastic_var_t;

typedef struct cluster {
    uint32_t win_size;
    float initial_value;
    stochastic_mean_t *smean;
    stochastic_var_t *svar;
} cluster_t;

typedef struct model {
    uint8_t count;
    cluster_t **clusters;
} model_t;

/* Stochastic update mean */
void stochasticMeanUpdate(stochastic_mean_t *this, float val);

/* Stochastic update var */
void stochasticVarUpdate(stochastic_var_t *this, float val);

/* Create stochastic mean object */
stochastic_mean_t *sMeanCreate(uint32_t win_size, float initial_value);

/* Create stochastic var object */
stochastic_var_t *sVarCreate(uint32_t win_size, float initial_value);


/* Cluster related functions 
 * Each cluster store stochastic mean and stochastic var
 */

/* Create cluster object */
cluster_t *clusterCreate(uint32_t win_size, float initial_value);

/* Update existing cluster (training) */
void clusterUpdate(cluster_t *this, float val);

/* Show cluster information */
void clusterShow(cluster_t this);

/* Delete cluster object */
void clusterDelete(cluster_t *this);

/* Get cluster distance */
float getDistanceToCluster(cluster_t *this, float val);


/* Model related functions 
 * Each model store multiple clusters. Each value input to model
 * will update the cluster located closest to the input value 
 */

/* Create model object */
model_t *modelCreate(uint32_t win_sizes[], float initial_values[], uint8_t count);

/* Update model (and clusters) based on input value */
void modelUpdate(model_t *this, float val);

/* Predict rescaled value using input model 
 * Set peak_max to 0 to disable peak rescaling 
 */
float modelTransform(model_t this, float val, float peak_max);

/* Update model and return rescaled value prediction */
float modelUpdateTransform(model_t *this, float val, float peak_max);

/* Show statistics of current model */
void modelShow(model_t this);

/* Delete model object */
void modelDelete(model_t *this);

#endif /* MODEL_H */