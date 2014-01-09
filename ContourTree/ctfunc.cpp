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

//size_t calc_volume_branch(ctBranch* b, ctBranch** b_map, size_t num_vertices)
//{
//    size_t v = 0;

//    for(size_t i = 0; i < num_vertices; i++)
//        if(b_map[i]->extremum == b->extremum && b_map[i]->saddle == b->saddle)
//            v++;

//    return v;
//}

//size_t calc_hypervolume_branch(ctBranch* b, ctBranch** b_map, Data* data)
//{
//    size_t hv = 0;

//    for(size_t i = 0; i < data->totalSize; i++)
//        if(b_map[i]->extremum == b->extremum && b_map[i]->saddle == b->saddle)
//            hv += data->data[i];

//    return hv;
//}

size_t* calc_vol_hypervol_branch(ctBranch* b, ctBranch** b_map, Data* data)
{
    size_t* fs = (size_t*) calloc(2, sizeof(size_t));

    for(size_t i = 0; i < data->totalSize; i++)
        if(b_map[i]->extremum == b->extremum && b_map[i]->saddle == b->saddle) {
            fs[0]++;
            fs[1] += data->data[i];
        }

    return fs;
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
        b->data = (FeatureSet*) calloc(1,sizeof(FeatureSet));

    FeatureSet* branch_data = (FeatureSet*) b->data;
    if(!branch_data->remove) {
        branch_data->depth = depth;
        if(depth > *max_depth)
            *max_depth = depth;
    } else {
        //std::cout << "True ";
    }

    for(ctBranch* c = b->children.head; c != NULL; c = c->nextChild)
        calc_branch_depth(c, max_depth, depth + 1);
}

//void calc_branch_features(ctBranch* root_branch, ctBranch** b_map, Data* data)
//{
//    if(root_branch == NULL) return;

//    if(root_branch->data == NULL)
//        root_branch->data = FeatureSet_new();

//    FeatureSet* ptr = (FeatureSet*) root_branch->data;
//    size_t* f = parallel_calc_vol_hypervol_branch(root_branch, b_map, data);//calc_vol_hypervol_branch(root_branch, b_map, data);
//    ptr->v = f[0];//calc_volume_branch(root_branch, b_map, data->totalSize);
//    ptr->p = calc_persistence_branch(root_branch, data);
//    ptr->hv = f[1];//calc_hypervolume_branch(root_branch, b_map, data);
//    free(f); f = NULL;

//    for(ctBranch* c = root_branch->children.head; c!= NULL; c = c->nextChild) {
//        calc_branch_features(c, b_map, data);
//    }
//}

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
    data_a->v += 1;
    data_a->hv += mesh_ptr->data[v];
    data_a->p = std::abs(mesh_ptr->data[a->hi->i] - mesh_ptr->data[a->lo->i]);
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
    //FeatureSet_del((FeatureSet*) b->data);
    free((FeatureSet*) b->data);
    b->data = NULL;
    //std::cout << "arc_merge_proc called\t" << data_a->v << "\t" << data_a->hv << "\t" << data_a->p << std::endl;
}

double arc_priority_proc(ctNode* leaf_node, void*)
{
    ctArc* leaf_arc = ctNode_leafArc(leaf_node);
    FeatureSet* data_arc = (FeatureSet*) leaf_arc->data;
    size_t v = data_arc->v;
    size_t hv = data_arc->hv;
    size_t p = data_arc->p;
    double arc_importance = sqrt(pow(hv * p, 2) + pow(v * p, 2) + pow(hv * v, 2));
    //std::cout << "Called arc_priority_proc " << leaf_node->i << "\t" << arc_importance << std::endl;
    return arc_importance;
}

/* ctOpFluid - @Netto */

void calc_residue_flow(ctBranch* root_branch, double alpha_d, double rate_Q)
{
    if(root_branch == NULL) return;

    if(root_branch->data == NULL)
        root_branch->data = (FeatureSet*) calloc(1, sizeof(FeatureSet));

    std::queue<ctBranch*> branch_queue;
    branch_queue.push(root_branch);

    //root node calcs
    FeatureSet* branch_data = (FeatureSet*) root_branch->data;
    branch_data->delta_h = (1.0*rate_Q*(((double)branch_data->p)/255.0))/(300.0*((double)branch_data->num_children));
    branch_data->alpha_i = 0;
    branch_data->delta_alpha_i = 0;
    std::cout << branch_data->depth << " - " << branch_data->delta_h << ", ";


    do {
        ctBranch* curr_branch = branch_queue.front();
        branch_queue.pop();

        if(curr_branch->data == NULL) {
            curr_branch->data = (FeatureSet*) calloc(1, sizeof(FeatureSet));
        }        

        for(ctBranch* c = curr_branch->children.head; c != NULL; c = c->nextChild) {
            FeatureSet* c_data = (FeatureSet*) c->data;
            if(!c_data->remove) {
                std::cout << c_data->depth << " - " << c_data->num_children << ", ";
                if(c_data->num_children != 0) {
                    c_data->delta_h = (1.0*rate_Q*(((double)c_data->p)/255.0))/(300.0*((double)c_data->num_children));
                    c_data->alpha_i = 0;
                    c_data->delta_alpha_i = 0;
                    std::cout << c_data->depth << " - " << c_data->delta_h << ", ";
                }
                branch_queue.push(c);
            }
        }

    } while(!branch_queue.empty());
}
