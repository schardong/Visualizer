#ifndef CTFUNC_H
#define CTFUNC_H

#include "data.h"
#include "mesh.h"
#include "featureset.h"

extern "C"
{
#include <tourtre/tourtre.h>
}

size_t count_branches(ctBranch*);
void calc_branch_features(ctBranch*, ctBranch**, Data*);
double calc_avg_importance(ctBranch*, double (*importance_measure)(ctBranch*));
void calc_branch_depth(ctBranch*, size_t*, size_t);
void calc_branch_num_children(ctBranch*);
void normalize_features(ctBranch*);

//Functions - @Netto
void calc_residue_flow(ctBranch* root_branch, double alpha_d, double rate_Q);
void calc_saddle_min_max(ctBranch* root_branch, Data* data);

//Callback functions used on my implementation.
double std_avg_importance(ctBranch*);
double std_log_importance(ctBranch*);

//Callback functions provided to libtourtre.
double std_value(size_t, void*);
size_t std_neighbors(size_t, size_t*, void*);
void vertex_proc(size_t, ctArc*, void*);
void arc_merge_proc(ctArc*, ctArc*, void*);
double arc_priority_proc(ctNode*, void*);


#endif // CTFUNC_H
