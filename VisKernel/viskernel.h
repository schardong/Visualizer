#ifndef VISKERNEL_H
#define VISKERNEL_H

#include "GL3/gl3w.h"
#include "resourcemanager.h"
#include "resources.h"
#include "scene.h"
#include "shader.h"
#include "singleton.h"
#include "volumedata.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class VisKernel : public Singleton<VisKernel>
{
private:
    friend class Singleton<VisKernel>;
    bool initialized;

    VisKernel() {}
    ~VisKernel() {}
public:
    bool init(int majVersion, int minVersion);
    bool destroy();
};

#endif // VISKERNEL_H
