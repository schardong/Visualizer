#include "viskernel.h"

#include <iostream>

using std::cerr;
using std::endl;

bool VisKernel::init(int majVersion, int minVersion)
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

bool VisKernel::destroy()
{
    initialized = false;
    return initialized;
}
