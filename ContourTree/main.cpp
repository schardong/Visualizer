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

void save_transfer_functions(ctBranch* root_branch, std::string filename, int num_tfs)
{
    if(root_branch == NULL || filename.empty() || num_tfs <= 0) return;

    unsigned char* tf_arr = (unsigned char*) calloc(num_tfs * 256, sizeof(unsigned char));

    std::queue<ctBranch*> branch_queue;
    branch_queue.push(root_branch);

    do {
        ctBranch* curr_branch = branch_queue.front();
        branch_queue.pop();

        FeatureSet* branch_data = (FeatureSet*) curr_branch->data;

        unsigned char tmp_tf[256];
        memset(tmp_tf, 0, 256 * sizeof(unsigned char));
        for(int i = 0; i < 256; i++)
            tmp_tf[i] = static_cast<unsigned char>(branch_data->alpha[i] * 255);

        unsigned char* tf_begin = tf_arr + branch_data->label * 256;
        memcpy(tf_begin, tmp_tf, 256 * sizeof(unsigned char));
        memset(tmp_tf, 0, 256 * sizeof(unsigned char));

        for(ctBranch* c = curr_branch->children.head; c != NULL; c = c->nextChild) {
            FeatureSet* c_data = (FeatureSet*) c->data;
            if(!c_data->remove)
                branch_queue.push(c);
        }

    } while(!branch_queue.empty());

    ggraf::ResourceManager::getInstance()->saveMultiDimensionalTransferFunction(filename, 256, num_tfs, sizeof(unsigned char), tf_arr);
    memset(tf_arr, 0, num_tfs * 256 * sizeof(unsigned char));
    free(tf_arr);
    tf_arr = NULL;

}

double opacity_max = 0.9;

int main(int argc, char** argv)
{
    std::string path = "/home/guilherme/Pictures/datasets/nucleon.41x41x41.uint8";
    //    std::string path = "/home/guilherme/Pictures/datasets/hydrogenAtom.128x128x128.uint8";
    //    std::string path = "/home/guilherme/Pictures/datasets/bonsai.256x256x256.uint8";
    //    std::string path = "/home/guilherme/Pictures/datasets/stent.512x512x174.uint8";

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
    //    outputTree(std::cout, root_branch);
    calc_branch_features(branch_map, &data);
    int last_label = label_branches(root_branch);
    //    outputTree(std::cout, root_branch);

    calc_branch_num_children(root_branch);

    max_depth = 0;
    calc_branch_depth(root_branch, &max_depth, 0);
    cout << count_branches(root_branch) << " branches after simplification." << endl;
    cout << "Tree depth = " << max_depth << "\nOpacity per depth level = " << opacity_max / max_depth << endl;
    normalize_features(root_branch);

    //    std::ofstream out_file;
    //    out_file.open(path + "-vtb");
    //    outputTree(out_file, root_branch);
    //    out_file.close();

    //calc_gsd(root_branch, &data);
    calc_residue_flow(root_branch, opacity_max / (double) max_depth, 300.0, &data);

    save_transfer_functions(root_branch,
                            "/home/guilherme/Pictures/datasets/transfer-functions/nucleon." + std::to_string(count_branches(root_branch)) + ".uint8",
                            last_label);

    save_vertex_branch_volume(branch_map,
                              "/home/guilherme/Pictures/datasets/vertex-branch-maps/nucleon.41x41x41.uint8",
                              data.size[0], data.size[1], data.size[2]);

    ct_cleanup(ctx);
    free(root_branch);
    free(branch_map);
    ctx = NULL;
}
