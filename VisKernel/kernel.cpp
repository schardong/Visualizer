#include "kernel.h"

#include <iostream>

using std::cerr;
using std::endl;

namespace ggraf
{
    bool Kernel::init(int majVersion, int minVersion)
    {
        initialized = false;

        if(gl3wInit()) {
            cerr << "Failed to initialize OpenGL." << endl;
            return initialized;
        }

        if(!gl3wIsSupported(majVersion, minVersion)) {
            cerr << "OpenGL " << majVersion << "." << minVersion << " is not supported." << endl;
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
