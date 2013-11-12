#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <string>
#include "singleton.h"

class ResourceManager : public Singleton<ResourceManager>
{
private:
    friend class Singleton<ResourceManager>;
    ResourceManager() {}
    ~ResourceManager() {}
public:
    void* loadVolumeData(std::string filename, int w, int h, int slices, size_t bytes_per_pixel);
    unsigned char* loadTransferFuncion(std::string filename, size_t bytes_per_pixel);
};

#endif // RESOURCEMANAGER_H
