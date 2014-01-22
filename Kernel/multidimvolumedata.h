#ifndef MULTIDIMVOLUMEDATA_H
#define MULTIDIMVOLUMEDATA_H

#include "volumedata.h"

namespace ggraf
{

    class MultiDimVolumeData : public ggraf::VolumeData
    {
    private:
        int m_aTexIds[4];
    public:
        MultiDimVolumeData();
        MultiDimVolumeData(std::string volume_path, std::string vtb_path, std::string opacity_tf_path, std::string color_tf_path);
        virtual ~MultiDimVolumeData();

        virtual void loadOpacityTransferFunction(std::string path);
        virtual void loadColorTransferFunction(std::string path);
        virtual void loadVertexToBranchMap(std::string path);
    };

}

#endif // MULTIDIMVOLUMEDATA_H
