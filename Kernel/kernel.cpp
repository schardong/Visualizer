#include "kernel.h"

#include <iostream>
#include <cstdio>

using std::cerr;
using std::endl;

namespace ggraf
{
    bool Kernel::init(int majVersion, int minVersion)
    {
        initialized = false;

        Logger::getInstance()->log("ggraf::Kernel::init");

        if(gl3wInit()) {
            Logger::getInstance()->error("Failed to initialize OpenGL");
            return initialized;
        }

        if(!gl3wIsSupported(majVersion, minVersion)) {
            char version[4];
            sprintf(version, "%d.%d", majVersion, minVersion);
            Logger::getInstance()->error("OpenGL " + std::string(version) + " is not supported.");
            return initialized;
        }

        initialized = true;
        return initialized;
    }

    bool Kernel::destroy()
    {
        initialized = false;
        return initialized;
    }

    bool Kernel::isInitilized()
    {
        return initialized;
    }
}
