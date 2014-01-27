#ifndef VOLUMEDATA_H
#define VOLUMEDATA_H

#include "transferfunction.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <string>
#include <vector>

namespace ggraf
{
    class VolumeData
    {
    protected:

        typedef struct
        {
            std::string path;
            int dim[3];
            size_t bytes_per_pixel;
        } ParsedVolPath;

        typedef struct
        {
            std::string path;
            size_t bytes_per_pixel;
        } ParsedTFPath;

        std::vector<int> m_aTexIds;
        int m_iVaoId;
        glm::vec3 m_vDimensions;
        glm::vec3 m_vScaleFactor;
        glm::mat4 m_mModelMatrix;

        std::vector<size_t> m_dataTypes;

        ParsedVolPath* parseVolumePath(std::string path);
        ParsedTFPath* parseTFPath(std::string path);

    public:
        VolumeData();
        VolumeData(std::string volume_path, std::string tf_path);
        virtual ~VolumeData();

        inline glm::mat4& getModelMatrix()
        {
            return m_mModelMatrix;
        }

        virtual inline int getVolumeTexId()
        {
            return m_aTexIds[0];
        }

        virtual inline int getTransferFuncTexId()
        {
            return m_aTexIds[1];
        }

        virtual inline bool isVolumeLoaded()
        {
            return m_aTexIds[0] == 0? false : true;
        }

        virtual inline bool isTfLoaded()
        {
            return m_aTexIds[1] == 0? false : true;
        }

        virtual inline std::vector<size_t> getDataTypes()
        {
            return m_dataTypes;
        }

        void loadVolume(std::string path);
        void loadTransferFunction(std::string path);
        void render();
    };
}

#endif // VOLUMEDATA_H
