#include "resourcemanager.h"

#include <iostream>
#include <cstdio>

void* ResourceManager::loadVolumeData(std::string filename, int w, int h, int slices, size_t bytes_per_pixel)
{
    FILE* fp;
    size_t num_voxels = w * h * slices;

    if(!(fp = fopen(filename.c_str(), "rb"))) {
        std::cerr << "ERROR: Failed to open the file " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    void* voxels = calloc(num_voxels, bytes_per_pixel);
    size_t bytes_read = fread(voxels, bytes_per_pixel, num_voxels, fp);
    fclose(fp);
    fp = NULL;

    if(bytes_read != num_voxels) {
        free(voxels);
        voxels = NULL;
        exit(EXIT_FAILURE);
    }

    return voxels;
}

unsigned char* ResourceManager::loadTransferFuncion(std::string filename, size_t bytes_per_pixel)
{
    FILE* fp;
    size_t num_rgba = bytes_per_pixel == sizeof(unsigned short) ? 4096 * 4 : 256 * 4;

    if(!(fp = fopen(filename.c_str(), "rb"))) {
        std::cerr << "ERROR: Failed to open the file " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    unsigned char* data = (unsigned char*) calloc(num_rgba, sizeof(unsigned char));
    size_t bytes_read = fread(data, sizeof(unsigned char), num_rgba, fp);
    fclose(fp);
    fp = NULL;

    if(bytes_read != num_rgba) {
        free(data);
        data = NULL;
        exit(EXIT_FAILURE);
    }

    return data;
}
