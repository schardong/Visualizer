#include <iostream>
#include <cmath>
#include <fstream>
#include <cstring>
#include <queue>
#include <vector>
#include <map>
#include <tbb/tbb.h>

extern "C" 
{
#include <unistd.h>
#include <tourtre/tourtre.h>
}

#include "featureset.h"
#include "data.h"
#include "mesh.h"
#include "ctfunc.h"
#include "simplification.h"

using std::cin;
using std::cout;
using std::clog;
using std::cerr;
using std::endl;

void outputTree(std::ostream& out, ctBranch* b)
{
    FeatureSet* fs = (FeatureSet*) b->data;
    if(fs->remove) return;
    out << "  (" << b->extremum << ' ' << b->saddle << " [" << fs->v << " " << fs->hv << " " << fs->p << " " << fs->depth << " " << fs->remove << " " << fs->num_children << "]";
    for( ctBranch * c = b->children.head; c != NULL; c = c->nextChild ) {
        outputTree( out, c );
    }
    out << ")";
}

size_t test_avg_importance(ctBranch* root_branch, double (*importance_measure)(ctBranch*), double avg_importance)
{
    if(root_branch == NULL) return 0.0;

    std::queue<ctBranch*> branch_queue;
    branch_queue.push(root_branch);
    size_t larger = 0;

    do {
        ctBranch* curr_branch = branch_queue.front();
        branch_queue.pop();

        double branch_importance = importance_measure(curr_branch);
        if(branch_importance >= avg_importance)
            larger++;

        for(ctBranch* c = curr_branch->children.head; c != NULL; c = c->nextChild) {
            branch_queue.push(c);
        }
    } while(!branch_queue.empty());

    return larger;
}

ctBranch* myAlloc(void*)
{
    return (ctBranch*) calloc(1, sizeof(ctBranch));
}

void myFree(ctBranch* b, void*)
{
    free(b->data); b->data = NULL;
    free(b); b = NULL;
}

double opacity_max = 0.9;

int main(int argc, char** argv)
{
//    std::string path = "/home/guilherme/Pictures/datasets/nucleon.41x41x41.uint8";
//    std::string path = "/home/guilherme/Pictures/datasets/hydrogenAtom.128x128x128.uint8";
//    std::string path = "/home/guilherme/Pictures/datasets/bonsai.256x256x256.uint8";

    std::string path = "/home/netto/datasets/hydrogenAtom.128x128x128.uint8";

//    std::string path = "/home/netto/datasets/nucleon.41x41x41.uint8";
//    std::string path = "/home/netto/nucleon.41x41x41.uint8";

    char prefix[1024];
    bool compressed;
    Data data;
    data.load(path.c_str(), prefix, &compressed);
    Mesh mesh(data);

    std::vector<size_t> order;
    mesh.createGraph(order);

    ctContext* ctx = ct_init(data.totalSize, &(order.front()), std_value, std_neighbors, &mesh);
    ct_vertexFunc(ctx, &vertex_proc);
    ct_arcMergeFunc(ctx, &arc_merge_proc);
    ct_priorityFunc(ctx, &arc_priority_proc);

    ct_branchAllocator(ctx, &myAlloc, &myFree);

    ct_sweepAndMerge(ctx);
    ctBranch* root_branch = ct_decompose(ctx);
    ctBranch** branch_map = ct_branchMap(ctx);

    if (root_branch == NULL)
        cout << "FUCK" << endl;

    size_t max_depth = 0;
    calc_branch_depth(root_branch, &max_depth, 0);
    cout << count_branches(root_branch) << " branches before simplification." << endl;
    cout << "Tree depth = " << max_depth << endl;

    calc_branch_features(root_branch, branch_map, &data);

    double avg_importance = calc_avg_importance(root_branch, &std_avg_importance);
    simplify_tree_dfs(root_branch, branch_map, &data, ctx, &std_avg_importance, avg_importance / 10000);
    calc_branch_num_children(root_branch);

    cout << count_branches(root_branch) << " branches after simplification." << endl;

    cout << test_avg_importance(root_branch, &std_avg_importance, avg_importance / 10000) << " nodes are larger or equal than the threshold.\n";
    cout << "Average importance = " << avg_importance << endl;

    max_depth = 0;
    calc_branch_depth(root_branch, &max_depth, 0);
    cout << "Tree depth = " << max_depth << "\nOpacity per depth level = " << opacity_max / max_depth << endl;

    outputTree(std::cout, root_branch);
    cout << endl;

    calc_gsd(root_branch, &data);
    calc_residue_flow(root_branch, opacity_max /(double)max_depth, 300.0);

    ct_cleanup(ctx);
    free(root_branch);
    free(branch_map);
    ctx = NULL;
}
