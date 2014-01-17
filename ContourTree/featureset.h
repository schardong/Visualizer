#ifndef FEATURESET_H
#define FEATURESET_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    size_t v;
    size_t p;
    size_t hv;
    size_t depth;
    size_t num_children;

    bool remove; //if set to true, the branch is removed from the tree.
    double delta_h; // (non leaf nodes)
    double* alpha; //opacity transfer function.
    double alpha_i; //initial alpha value (non leaf nodes)
    double alpha_i_j; //
    double alpha_lo; //final lower opacity value band
    double alpha_hi; //final upper opacity value band
    double delta_alpha_i; //residue
    double c_s_min; //children minimum saddle value
    double c_s_max; //children maximum saddle value

    double norm_v;
    double norm_p;
    double norm_hv;
} FeatureSet;

#endif // FEATURESET_H
