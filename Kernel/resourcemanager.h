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
        void* loadVolumeData(std::string filename, int w, int h, int slices, size_t bytes_per_pixel);
        void* loadVertexToBranchMap(std::string filename, int w, int h, int slices);
        size_t saveVertexToBranchMap(std::string filename, int w, int h, int slices, unsigned int* branch_map);
        unsigned char* loadTransferFuncion(std::string filename, size_t bytes_per_pixel);
        unsigned char* loadMultiDimensionalTransferFunction(std::string filename, int width, int height, size_t bytes_per_pixel);
        size_t saveMultiDimensionalTransferFunction(std::string filename, int width, int height, size_t bytes_per_pixel, void* tf_data);

        GLuint createVolumeTex(int w, int h, int slices, size_t bytes_per_pixel, void* data);
        GLuint createTransferFuncTex(size_t bytes_per_pixel, unsigned char* data);
        GLuint createCubeVAO();

        bool uploadVolumeData(int w, int h, int slices, size_t bytes_per_pixel, void* data, GLuint texId);
        bool uploadTransferFuncData(size_t bytes_per_pixel, unsigned char* data, GLuint texId);

        void destroyCubeVao(GLuint cube_id);
    };
}

#endif // RESOURCEMANAGER_H
