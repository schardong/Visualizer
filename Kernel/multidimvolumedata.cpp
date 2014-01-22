#include "multidimvolumedata.h"
#include "resourcemanager.h"
#include "logger.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

namespace ggraf
{

    MultiDimVolumeData::MultiDimVolumeData()
    {
        std::memset(m_aTexIds, 0, 4 * sizeof(int));
    }

    MultiDimVolumeData::MultiDimVolumeData(std::string volume_path, std::string vtb_path, std::string opacity_tf_path, std::string color_tf_path)
    {
        std::memset(m_aTexIds, 0, 4 * sizeof(int));

        loadVolume(volume_path);
        loadVertexToBranchMap(vtb_path);
        loadOpacityTransferFunction(opacity_tf_path);
        loadColorTransferFunction(color_tf_path);

        Logger::getInstance()->warn(std::to_string(m_iVaoId) + " is the VAO id");

    }

    MultiDimVolumeData::~MultiDimVolumeData()
    {

    }

    void MultiDimVolumeData::loadOpacityTransferFunction(std::string path)
    {
        Logger::getInstance()->warn("MultiDimVolumeData::loadOpacityTransferFunction(" +
                                    path + ") not yet implemented");
    }
    void MultiDimVolumeData::loadColorTransferFunction(std::string path)
    {
        Logger::getInstance()->warn("MultiDimVolumeData::loadColorTransferFunction(" +
                                    path + ") not yet implemented");
    }
    void MultiDimVolumeData::loadVertexToBranchMap(std::string path)
    {
        Logger::getInstance()->warn("MultiDimVolumeData::loadVertexToBranchMap(" +
                                    path + ") not yet implemented");
    }

}
