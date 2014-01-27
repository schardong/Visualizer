#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "singleton.h"
#include "GL3/gl3w.h"

#include <string>

namespace ggraf
{
    class ResourceManager : public Singleton<ResourceManager>
    {
    private:
        friend class Singleton<ResourceManager>;
        ResourceManager() {}
        ~ResourceManager() {}

    public:
        void* loadVolumeData(std::string filename, size_t w, size_t h, size_t slices, size_t bytes_per_pixel);
        float* loadVertexToBranchMap(std::string filename, size_t w, size_t h, size_t slices);
        size_t saveVertexToBranchMap(std::string filename, size_t w, size_t h, size_t slices, float* branch_map);
        unsigned char* loadTransferFuncion(std::string filename, size_t bytes_per_pixel);
        unsigned char* loadMultiDimensionalTransferFunction(std::string filename, size_t w, size_t h, size_t bytes_per_pixel);
        size_t saveMultiDimensionalTransferFunction(std::string filename, size_t w, size_t h, size_t bytes_per_pixel, void* tf_data);
        unsigned char* loadColorTransferFunction(std::string filename, size_t bytes_per_pixel);
        size_t saveColorTransferFunction(std::string filename, size_t bytes_per_pixel, void* tf_data);

        GLuint createVolumeTex(size_t w, size_t h, size_t slices, size_t bytes_per_pixel, void* data);
        GLuint createVertexBranchTex(size_t w, size_t h, size_t slices, float* data);
        GLuint createTransferFuncTex(size_t bytes_per_pixel, unsigned char* data);
        GLuint createMultiDimTransferFuncTex(size_t w, size_t h, size_t bytes_per_pixel, unsigned char* data);
        GLuint createColorTransferFuncTex(size_t bytes_per_pixel, unsigned char* data);

        bool uploadVolumeData(size_t w, size_t h, size_t slices, size_t bytes_per_pixel, void* data, GLuint texId);
        bool uploadVertexBranchData(size_t w, size_t h, size_t slices, float* data, GLuint texId);
        bool uploadTransferFuncData(size_t bytes_per_pixel, unsigned char* data, GLuint texId);
        bool uploadMultiDimTransferFuncData(size_t w, size_t h, size_t bytes_per_pixel, unsigned char* data, GLuint texId);
        bool uploadColorTransferFuncData(size_t bytes_per_pixel, unsigned char* data, GLuint texId);

        GLuint createCubeVAO();
        void destroyCubeVao(GLuint cube_id);
    };
}

#endif // RESOURCEMANAGER_H
