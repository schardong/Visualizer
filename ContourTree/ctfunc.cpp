#include "ctfunc.h"
#include "featureset.h"

#include <queue>
#include <cstring>
#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>

size_t count_branches(ctBranch* b)
{
    if(b == NULL) return 0;

    size_t count = 0;
    FeatureSet* fs = (FeatureSet*) b->data;
    if(fs == NULL || fs->remove == false) count = 1;

    for(ctBranch*c = b->children.head; c != NULL; c = c->nextChild)
        count += count_branches(c);

    return count;
}

class VolHypervolAcc
{
    ctBranch** b_map;
    ctBranch* b;
    Data* data;
public:
    size_t v[2];

    VolHypervolAcc(ctBranch* _b, ctBranch** _b_map, Data* _data) : b_map(_b_map), b(_b), data(_data)
    {
        v[0] = v[1] = 0;
    }

    VolHypervolAcc(VolHypervolAcc& rhs, tbb::split) : b_map(rhs.b_map), b(rhs.b), data(rhs.data)
    {
        v[0] = v[1] = 0;
    }

    void operator()(const tbb::blocked_range<size_t>& r) {
        size_t begin = r.begin();
        size_t end = r.end();
        for(size_t i = begin; i < end; i++) {
            if(b_map[i]->extremum == b->extremum && b_map[i]->saddle == b->saddle) {
                v[0]++;
                v[1] += data->data[i];
            }
        }
    }

    void join(const VolHypervolAcc& rhs) {
        v[0] += rhs.v[0];
        v[1] += rhs.v[1];
    }
};

size_t* parallel_calc_vol_hypervol_branch(ctBranch* b, ctBranch** b_map, Data* data)
{
    VolHypervolAcc v(b, b_map, data);
    size_t* fs = (size_t*) calloc(2, sizeof(size_t));
    tbb::parallel_reduce(tbb::blocked_range<size_t>(0, data->totalSize), v);
    std::memcpy(fs, v.v, 2 * sizeof(size_t));
    return fs;
}

size_t calc_persistence_branch(ctBranch* b, Data* data)
{
    return std::abs(data->data[b->extremum] - data->data[b->saddle]);
}

void calc_branch_depth(ctBranch* b, size_t* max_depth, size_t depth)
{
    if(b == NULL) return;

    if(b->data == NULL)        
        b->data = (FeatureSet*) calloc(1, sizeof(FeatureSet));

    FeatureSet* branch_data = (FeatureSet*) b->data;
    if(!branch_data->remove) {
        branch_data->depth = depth;
        if(depth > *max_depth)
            *max_depth = depth;
    }

    for(ctBranch* c = b->children.head; c != NULL; c = c->nextChild)
        calc_branch_depth(c, max_depth, depth + 1);
}

void calc_branch_features(ctBranch* root_branch, ctBranch** b_map, Data* data)
{
    if(root_branch == NULL) return;

    if(root_branch->data == NULL)
        root_branch->data = (FeatureSet*) calloc(1, sizeof(FeatureSet));

    FeatureSet* branch_data = (FeatureSet*) root_branch->data;
    size_t* f = parallel_calc_vol_hypervol_branch(root_branch, b_map, data);

    branch_data->v = f[0];
    branch_data->hv = f[1];
    branch_data->p = calc_persistence_branch(root_branch, data);
    branch_data->c_s_min = 10000.0; //since maximum value is 255
    branch_data->c_s_max = 0;
    free(f); f = NULL;

    for(ctBranch* c = root_branch->children.head; c!= NULL; c = c->nextChild) {
        calc_branch_features(c, b_map, data);
    }
}

void calc_branch_num_children(ctBranch* root_branch)
{
    if(root_branch == NULL) return;

    if(root_branch->data == NULL)
        root_branch->data = (FeatureSet*) calloc(1, sizeof(FeatureSet));

    std::queue<ctBranch*> branch_queue;
    branch_queue.push(root_branch);

    do {
        ctBranch* curr_branch = branch_queue.front();
        branch_queue.pop();

        if(curr_branch->data == NULL)
            curr_branch->data = (FeatureSet*) calloc(1, sizeof(FeatureSet));

        FeatureSet* branch_data = (FeatureSet*) curr_branch->data;
        for(ctBranch* c = curr_branch->children.head; c != NULL; c = c->nextChild) {
            FeatureSet* c_data = (FeatureSet*) c->data;
            if(!c_data->remove) {
                branch_data->num_children++;
                branch_queue.push(c);
            }
        }
    } while(!branch_queue.empty());
}

FeatureSet* find_max_features_children(ctBranch* root_branch)
{
    FeatureSet* max_features = (FeatureSet*) calloc(1, sizeof(FeatureSet));
    if(root_branch == NULL) return max_features;

    for(ctBranch* c = root_branch->children.head; c != NULL; c = c->nextChild) {
        FeatureSet* c_data = (FeatureSet*) c->data;
        if(c_data->remove) continue;

        if(c_data->v > max_features->v)
            max_features->v = c_data->v;

        if(c_data->hv > max_features->hv)
            max_features->hv = c_data->hv;

        if(c_data->p > max_features->p)
            max_features->p = c_data->p;
    }

    return max_features;
}

void normalize_features(ctBranch* root_branch)
{
    if(root_branch == NULL) return;

    std::queue<ctBranch*> branch_queue;
    branch_queue.push(root_branch);

    FeatureSet* root_data = (FeatureSet*) root_branch->data;
    root_data->norm_v = root_data->norm_p = root_data->norm_hv = 1.0;

    do {
        ctBranch* curr_branch = branch_queue.front();
        branch_queue.pop();

        FeatureSet* max_features = find_max_features_children(curr_branch);

        for(ctBranch* c = curr_branch->children.head; c != NULL; c = c->nextChild) {
            FeatureSet* c_data = (FeatureSet*) c->data;
            if(!c_data->remove) {
                branch_queue.push(c);
                c_data->norm_v = (double) c_data->v / max_features->v;
                c_data->norm_p = (double) c_data->p / max_features->p;
                c_data->norm_hv = (double) c_data->hv / max_features->hv;

                std::cout << "    " << c_data->norm_v << ", " << c_data->norm_hv << ", " << c_data->norm_p << std::endl;
            }
        }

        free(max_features);
        max_features = NULL;

    } while(!branch_queue.empty());

}

double calc_avg_importance(ctBranch* root_branch, double (*importance_measure)(ctBranch*))
{
    if(root_branch == NULL) return 0.0;

    double avg_importance = 0.0;
    std::queue<ctBranch*> branch_queue;
    branch_queue.push(root_branch);

    do {
        ctBranch* curr_branch = branch_queue.front();
        branch_queue.pop();

        double branch_importance = importance_measure(curr_branch);
        avg_importance += branch_importance;

        for(ctBranch* c = curr_branch->children.head; c != NULL; c = c->nextChild) {
            branch_queue.push(c);
        }
    } while(!branch_queue.empty());

    return avg_importance / count_branches(root_branch);
}

double std_avg_importance(ctBranch* b)
{
    if(b == NULL) return 0.0;

    FeatureSet* ptr = (FeatureSet*) b->data;
    if(ptr == NULL) return 0.0;

    return sqrt(pow(ptr->hv * ptr->p, 2) + pow(ptr->v * ptr->p, 2) + pow(ptr->hv * ptr->v, 2));
}

double std_log_importance(ctBranch* b)
{
    if(b == NULL) return 0.0;

    FeatureSet* ptr = (FeatureSet*) b->data;
    if(ptr == NULL) return 0.0;

    double a = sqrt(pow(ptr->hv * ptr->p, 2) + pow(ptr->v * ptr->p, 2) + pow(ptr->hv * ptr->v, 2));
    if(a == 0) return 0.0;

    double c = log(a);
    return c;
}

double std_value(size_t v, void* d)
{
    Mesh * mesh = reinterpret_cast<Mesh*>(d);
    return mesh->data[v];
}

size_t std_neighbors(size_t v, size_t* nbrs, void* d)
{
    Mesh* mesh = static_cast<Mesh*>(d);
    size_t nbrs_size = mesh->getNeighbors(v, nbrs, 18);
    return nbrs_size;
}

void vertex_proc(size_t v, ctArc* a, void* cb_data)
{
    if(a->data == NULL)
        a->data = (FeatureSet*) calloc(1, sizeof(FeatureSet));//FeatureSet_new();

    Mesh* mesh_ptr = (Mesh*) cb_data;
    FeatureSet* data_a = (FeatureSet*) a->data;
    data_a->v += 1.0;
    data_a->hv += (double) mesh_ptr->data[v];
    data_a->p = (double) std::abs(mesh_ptr->data[a->hi->i] - mesh_ptr->data[a->lo->i]);
}

void arc_merge_proc(ctArc* a, ctArc* b, void* cb_data)
{
    /*"a" will become the arc while "b" will be deleted.*/
    Mesh* mesh_ptr = (Mesh*) cb_data;
    FeatureSet* data_a = (FeatureSet*) a->data;
    FeatureSet* data_b = (FeatureSet*) b->data;
    data_a->v += data_b->v;
    data_a->hv += data_b->hv;
    size_t max_vertex_addr;
    size_t min_vertex_addr;
    if(mesh_ptr->data[a->hi->i] > mesh_ptr->data[b->hi->i])
        max_vertex_addr = a->hi->i;
    else
        max_vertex_addr = b->hi->i;

    if(mesh_ptr->data[a->lo->i] > mesh_ptr->data[b->lo->i])
        min_vertex_addr = b->lo->i;
    else
        min_vertex_addr = a->lo->i;

    data_a->p = std::abs(mesh_ptr->data[max_vertex_addr] - mesh_ptr->data[min_vertex_addr]);
    free((FeatureSet*) b->data);
    b->data = NULL;
}

double arc_priority_proc(ctNode* leaf_node, void*)
{
    ctArc* leaf_arc = ctNode_leafArc(leaf_node);
    FeatureSet* data_arc = (FeatureSet*) leaf_arc->data;
    double v = data_arc->v;
    double hv = data_arc->hv;
    double p = data_arc->p;
    double arc_importance = sqrt(pow(hv * p, 2) + pow(v * p, 2) + pow(hv * v, 2));
    return arc_importance;
}

/* ctOpFluid - @Netto */

double half_std_avg_importance_normalized(ctBranch* b)
{
    if(b == NULL) return 0.0;

    FeatureSet* ptr = (FeatureSet*) b->data;
    if(ptr == NULL) return 0.0;

    return 0.5*sqrt(pow(ptr->norm_hv * ptr->norm_p, 2) + pow(ptr->norm_v * ptr->norm_p, 2) + pow(ptr->norm_hv * ptr->norm_v, 2));
}

void calc_residue_flow(ctBranch* root_branch, double alpha_d, double rate_Q, Data* data)
{
    if(root_branch == NULL) return;

    if(root_branch->data == NULL)
        root_branch->data = (FeatureSet*) calloc(1, sizeof(FeatureSet));

    std::queue<ctBranch*> branch_queue;
    branch_queue.push(root_branch);

    do {
        ctBranch* curr_branch = branch_queue.front();        
        branch_queue.pop();

        if(curr_branch->data == NULL) {
            curr_branch->data = (FeatureSet*) calloc(1, sizeof(FeatureSet));
        }

        FeatureSet* branch_data = (FeatureSet*) curr_branch->data;

        if(!branch_data->remove) {
            if (branch_data->num_children != 0) {
                branch_data->delta_h = (1.0*rate_Q*(((double)branch_data->p)/255.0))/(300.0*((double)branch_data->num_children));
                if(branch_data->depth == 0) { //root node calcs
                    branch_data->alpha_i = alpha_d*(1-branch_data->delta_h);
                    branch_data->delta_alpha_i = alpha_d - branch_data->alpha_i;
                } else {
                    ctBranch* parent_branch = curr_branch->parent;
                    FeatureSet* parent_data = (FeatureSet*) parent_branch->data;
                    branch_data->alpha_i = (alpha_d + parent_data->delta_alpha_i)*(1.0-branch_data->delta_h);
                    branch_data->delta_alpha_i = (alpha_d + parent_data->delta_alpha_i)*branch_data->delta_h;
                }
            } else { // leaf node
                ctBranch* parent_branch = curr_branch->parent;
                FeatureSet* parent_data = (FeatureSet*) parent_branch->data;
                branch_data->alpha_i = (alpha_d + parent_data->delta_alpha_i)*(1.0 - 0);
                branch_data->delta_alpha_i = 0.0;
            }
            branch_data->alpha_i_j = branch_data->alpha_i*(half_std_avg_importance_normalized(curr_branch))*calc_gsd(curr_branch,data);
            std::cout << "["<< branch_data->depth << "] Dh: " << branch_data->delta_h << " Ai: " << branch_data->alpha_i << " r: "
                      << branch_data->delta_alpha_i << " Aij: " << branch_data->alpha_i_j << std::endl;
            //std::cout << branch_data->depth << " - " << branch_data->num_children << ", ";
            branch_data->alpha_lo = calc_alpha_sum(curr_branch);
            branch_data->alpha_hi = calc_alpha_sum(curr_branch) + branch_data->alpha_i_j;
            branch_data->alpha = calc_final_alpha(curr_branch,LINEAR, data);
            std::cout << "     Alo: " << branch_data->alpha_lo << " Ahi: " << branch_data->alpha_hi << /* " Imp: " << std_avg_importance(curr_branch) << */ std::endl;
            std::cout << " Opacity: ";
            for(int i = 0; i < 256; i++)
                std::cout << branch_data->alpha[i] << " ";
            std::cout << "\n\n";

        }

        for(ctBranch* c = curr_branch->children.head; c != NULL; c = c->nextChild) {
            FeatureSet* c_data = (FeatureSet*) c->data;
            if(!c_data->remove) {                
                branch_queue.push(c);
            }
        }

    } while(!branch_queue.empty());
}

double* calc_final_alpha(ctBranch* b, TFShape shape, Data* data)
{
    if(b == NULL) return nullptr;
    double* alpha_tf = (double*) calloc(256, sizeof(double));

    FeatureSet* b_data = (FeatureSet*) b->data;

    switch(shape) {
    case HAT:
        std::cout << "HAT shape chosen.\n";
        break;
    case TRIANGLE:
        std::cout << "TRIANGLE shape chosen.\n";
        break;
    case TRAPEZOID:
        std::cout << "TRAPEZOID shape chosen.\n";
        break;
    case HTRAPEZOID:
        std::cout << "HTRAPEZOID shape chosen.\n";
        break;
    case LINEAR:
    default:
        std::cout << "LINEAR shape chosen.\n";
        double m = (1 - 0) / (b_data->alpha_hi - b_data->alpha_lo);

        double step = (b_data->alpha_hi - b_data->alpha_lo) / 256.0;
        double x = b_data->alpha_lo;

        std::cout << "m = " << m << std::endl;
        std::cout << "step = " << step << std::endl;
        std::cout << "x = " << x << std::endl;

        for(int i = 0; i < 256 && x <= b_data->alpha_hi; i++, x += step)
            alpha_tf[i] = m * (x - b_data->alpha_lo);
        break;
    }
    return alpha_tf;
}

double calc_gsd(ctBranch* b, Data* data) {
    if(b->parent != NULL) {
        FeatureSet* parent = (FeatureSet*) b->parent->data;
        if(parent->num_children > 1)
            return (((double)data->data[b->saddle]) - parent->c_s_min)/(parent->c_s_max - parent->c_s_min);
    }
    return 1.0;
}

double calc_alpha_sum(ctBranch* b) {
    double sum = 0;
    ctBranch* tmp = b->parent;
    while (tmp != NULL) {
        FeatureSet* parent = (FeatureSet*) tmp->data;
        sum += parent->alpha_i_j;
        free(parent);
        tmp = tmp->parent;
    }
    return sum;
}

void calc_saddle_min_max(ctBranch* root_branch, Data* data)
{
    if(root_branch == NULL) return;

    if(root_branch->data == NULL)
        root_branch->data = (FeatureSet*) calloc(1, sizeof(FeatureSet));

    std::queue<ctBranch*> branch_queue;
    branch_queue.push(root_branch);

    do {
        ctBranch* curr_branch = branch_queue.front();
        branch_queue.pop();

        if(curr_branch->data == NULL)
            curr_branch->data = (FeatureSet*) calloc(1, sizeof(FeatureSet));

        FeatureSet* branch_data = (FeatureSet*) curr_branch->data;
        for(ctBranch* c = curr_branch->children.head; c != NULL; c = c->nextChild) {
            FeatureSet* c_data = (FeatureSet*) c->data;
            if(!c_data->remove) {
                if(branch_data->num_children != 0) {
                    if(data->data[c->saddle] < branch_data->c_s_min) {
                        branch_data->c_s_min = data->data[c->saddle];
                    }
                    if (data->data[c->saddle] > branch_data->c_s_max){
                        branch_data->c_s_max = data->data[c->saddle];
                    }
                }
                branch_queue.push(c);
            }
        }
        if(!branch_data->remove && branch_data->num_children != 0) {
            std::cout << "nc: " << branch_data->num_children << " min: " << branch_data->c_s_min << " max: " << branch_data->c_s_max << std::endl;
        }
    } while(!branch_queue.empty());
}
