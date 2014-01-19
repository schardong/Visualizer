
#include "contourtree.h"

namespace ggraf
{

    bool ContourTree::init()
    {
        if(initialized) return true;
        initialized = true;
        return true;
    }

    bool ContourTree::isInitialized()
    {
        return initialized;
    }

}

