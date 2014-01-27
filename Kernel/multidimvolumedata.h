#ifndef MULTIDIMVOLUMEDATA_H
#define MULTIDIMVOLUMEDATA_H

#include "volumedata.h"

namespace ggraf
{

    class MultiDimVolumeData : public ggraf::VolumeData
    {
    private:

        typedef struct
        {
            std::string path;
            int w, h;
            size_t bytes_per_pixel;
        } ParsedMultiDimTFPath;

        ParsedMultiDimTFPath* parseMultiDimTFPath(std::string path);

    public:
        MultiDimVolumeData();
        MultiDimVolumeData(std::string volume_path, std::string vtb_path, std::string opacity_tf_path, std::string color_tf_path);
        virtual ~MultiDimVolumeData();

        virtual void loadOpacityTransferFunction(std::string path);
        virtual void loadColorTransferFunction(std::string path);
        virtual void loadVertexToBranchMap(std::string path);

        virtual inline int getVertexBranchTexId()
        {
            return m_aTexIds[1];
        }

        virtual inline int getOpacityTransferFuncTexId()
        {
            return m_aTexIds[2];
        }

        virtual inline int getColorTransferFuncTexId()
        {
            return m_aTexIds[3];
        }

        virtual inline bool isVertexBranchMapLoaded()
        {
            return m_aTexIds[1] == 0? false : true;
        }

        virtual inline bool isOpacityTfLoaded()
        {
            return m_aTexIds[2] == 0? false : true;
        }

        virtual inline bool isColorTfLoaded()
        {
            return m_aTexIds[3] == 0? false : true;
        }
    };

}

#endif // MULTIDIMVOLUMEDATA_H
