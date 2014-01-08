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
//    if set to true, the branch is removed from the tree.
    bool remove;
    double delta_h;
    double alpha;
} FeatureSet;

#endif // FEATURESET_H
