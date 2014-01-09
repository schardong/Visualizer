/*
#ifndef CONTOURTREE_H
#define CONTOURTREE_H

#include "singleton.h"
#include <tourtre/tourtre.h>

namespace ggraf
{

    class ContourTree : public Singleton<ContourTree>
    {
    private:
        friend class Singleton<ContourTree>;
        bool initialized;

        ContourTree() {}
        ~ContourTree() {}
    public:
        bool init();
        bool buildTree();
        ctBranch* getRootBranch();
        bool isInitialized();
    };

}

#endif // CONTOURTREE_H
*/
