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
        unsigned char* loadTransferFuncion(std::string filename, size_t bytes_per_pixel);

        GLuint createVolumeTex(int w, int h, int slices, size_t bytes_per_pixel, void* data);
        GLuint createTransferFuncTex(size_t bytes_per_pixel, unsigned char* data);
        GLuint createCubeVAO();
        void destroyCubeVao(GLuint cube_id);
    };
}

#endif // RESOURCEMANAGER_H
