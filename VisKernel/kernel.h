#ifndef KERNEL_H
#define KERNEL_H

#include "GL3/gl3w.h"
#include "resourcemanager.h"
#include "scene.h"
#include "shader.h"
#include "singleton.h"
#include "volumedata.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ggraf
{
    class Kernel : public Singleton<Kernel>
    {
    private:
        friend class Singleton<Kernel>;
        bool initialized;

        Kernel() {}
        ~Kernel() {}
    public:
        bool init(int majVersion, int minVersion);
        bool destroy();

        bool isInitilized();
    };
}

#endif // KERNEL_H
