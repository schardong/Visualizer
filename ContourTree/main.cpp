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

        for(ctBranch* c = curr_branch->children.head; c != NULL; c = c->nextChild)
            branch_queue.push(c);

    } while(!branch_queue.empty());

    return larger;
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

size_t save_vertex_branch_volume(ctBranch** branch_map, std::string filename, size_t w, size_t h, size_t slices)
{
    if(branch_map == NULL || filename.empty()) return 0;

    size_t num_elements = w * h * slices;

    unsigned int* branch_vol = (unsigned int*) calloc(num_elements, sizeof(unsigned int));

    for(size_t i = 0; i < num_elements; i++) {
        FeatureSet* branch_data = (FeatureSet*) branch_map[i]->data;
        branch_vol[i] = branch_data->label;
    }

    size_t bytes_written = ggraf::ResourceManager::getInstance()->saveVertexToBranchMap(filename, w, h, slices, branch_vol);

    //    FILE* fp;
    //    if(!(fp = fopen(filename.c_str(), "wb+"))) {
    //        free(branch_vol);
    //        branch_vol = nullptr;
    //        return 0;
    //    }

    //    size_t bytes_written = fwrite(branch_vol, sizeof(char), num_elements, fp);
    //    fclose(fp);
    //    fp = nullptr;

    memset(branch_vol, 0, sizeof(unsigned int));
    free(branch_vol);
    branch_vol = nullptr;

    return bytes_written;
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
    //    cout << count_branches(root_branch) << " branches before simplification." << endl;
    //    cout << "Tree depth = " << max_depth << endl;

    //    tbb::tick_count a = tbb::tick_count::now();
    calc_branch_features(branch_map, &data);
    //    tbb::tick_count b = tbb::tick_count::now();
    //    cout << (b - a).seconds() << endl;

    double avg_importance = calc_avg_importance(root_branch, &std_avg_importance);
    simplify_tree_dfs(root_branch, /*branch_map, &data, ctx,*/ &std_avg_importance, avg_importance / 10000);
    label_branches(root_branch);
    save_vertex_branch_volume(branch_map, "/home/guilherme/Projects/Visualizer/nucleon-vtb.41x41x41.uint8", data.size[0], data.size[1], data.size[2]);
    calc_branch_num_children(root_branch);

    //    cout << count_branches(root_branch) << " branches after simplification." << endl;

    //    cout << test_avg_importance(root_branch, &std_avg_importance, avg_importance / 10000) << " nodes are larger or equal than the threshold.\n";
    //    cout << "Average importance = " << avg_importance << endl;

    max_depth = 0;
    calc_branch_depth(root_branch, &max_depth, 0);
    cout << "Tree depth = " << max_depth << "\nOpacity per depth level = " << opacity_max / max_depth << endl;
    normalize_features(root_branch);

    std::ofstream out_file;
    out_file.open("/home/guilherme/Projects/Visualizer/tree.txt");
    outputTree(out_file, root_branch);
    out_file.close();

    //calc_gsd(root_branch, &data);
    calc_residue_flow(root_branch, opacity_max / (double) max_depth, 300.0, &data);

    ct_cleanup(ctx);
    free(root_branch);
    free(branch_map);
    ctx = NULL;
}
