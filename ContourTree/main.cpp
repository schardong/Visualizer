#include <iostream>
#include <cmath>
#include <fstream>
#include <cstring>
#include <queue>
#include <vector>
#include <map>
#include <tbb/tbb.h>
#include <set>

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
#include "resourcemanager.h"
#include "logger.h"

using std::cin;
using std::cout;
using std::clog;
using std::cerr;
using std::endl;

void outputTree(std::ostream& out, ctBranch* b)
{
    FeatureSet* fs = (FeatureSet*) b->data;
    if(fs->remove) return;
    out << "  (" << b->extremum << ' ' << b->saddle << " [" << fs->label << " " << fs->v << " " << fs->hv << " " << fs->p << " " << fs->depth << " " << fs->remove << " " << fs->num_children << "]";
    for( ctBranch * c = b->children.head; c != NULL; c = c->nextChild ) {
        outputTree( out, c );
    }
    out << ")";
}

void output_branch_map(std::ostream& out, ctBranch** b_map, size_t data_size)
{
    for(size_t i = 0; i < data_size; i++) {
        FeatureSet* fs = (FeatureSet*) b_map[i]->data;
        if(fs->remove) continue;
        out << i << "\t" << fs->label << endl;
    }
}

ctBranch* myAlloc(void*)
{
    return (ctBranch*) calloc(1, sizeof(ctBranch));
}

void myFree(ctBranch* b, void*)
{
    if(b->data != NULL) {
        free(b->data);
        b->data = NULL;
    }
    free(b);
    b = NULL;
}

double opacity_max = 0.9;

int main(int argc, char** argv)
{
//    std::string path = "/home/guilherme/Pictures/datasets/marschnerlobb.41x41x41.uint8";
//    std::string path = "/home/guilherme/Pictures/datasets/nucleon.41x41x41.uint8";
    std::string path = "/home/guilherme/Pictures/datasets/hydrogenAtom.128x128x128.uint8";
//    std::string path = "/home/guilherme/Pictures/datasets/neghip.64x64x64.uint8";
//    std::string path = "/home/guilherme/Pictures/datasets/foot.256x256x256.uint8";
    //std::string path = "/home/guilherme/Pictures/datasets/stent.512x512x174.uint8";

    //    std::string path = "/home/netto/datasets/hydrogenAtom.128x128x128.uint8";
    //    std::string path = "/home/netto/datasets/nucleon.41x41x41.uint8";

    Logger::getInstance()->setLogStream(&std::cout);

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

    zero_branches(root_branch);
    size_t max_depth = 0;
    calc_branch_depth(root_branch, &max_depth, 0);

    cout << count_branches(root_branch) << " branches before simplification." << endl;
    cout << "Tree depth = " << max_depth << endl;

    //    tbb::tick_count a = tbb::tick_count::now();
    calc_branch_features(branch_map, &data);
    //    tbb::tick_count b = tbb::tick_count::now();
    //    cout << (b - a).seconds() << endl;

    double avg_importance = calc_avg_importance(root_branch, &std_avg_importance);
    simplify_tree_dfs(root_branch, branch_map, data.totalSize, &std_avg_importance, avg_importance / 10000);
    calc_branch_features(branch_map, &data);
    int last_label = label_branches(root_branch);
    outputTree(std::cout, root_branch);

    calc_branch_num_children(root_branch);

    max_depth = 0;
    calc_branch_depth(root_branch, &max_depth, 0);
    cout << count_branches(root_branch) << " branches after simplification." << endl;
    cout << "Tree depth = " << max_depth << "\nOpacity per depth level = " << opacity_max / max_depth << endl;
    normalize_features(root_branch);

    calc_residue_flow(root_branch, opacity_max / (double) max_depth, 300.0, &data);

    save_transfer_functions(root_branch,
                            "/home/guilherme/Pictures/datasets/transfer-functions/hydrogenAtom." + std::to_string(count_branches(root_branch)) + ".uint8",
                            last_label);

    save_vertex_branch_volume(branch_map,
                              "/home/guilherme/Pictures/datasets/vertex-branch-maps/hydrogenAtom.128x128x128.uint16",
                              data.size[0], data.size[1], data.size[2], last_label);

//    std::set<ctBranch*> test_set;
//    for(int i = 0; i < data.totalSize; i++)
//        test_set.insert(branch_map[i]);

//    cout << test_set.size() << endl;

    ct_cleanup(ctx);
    free(root_branch);
    free(branch_map);
    ctx = NULL;
}
