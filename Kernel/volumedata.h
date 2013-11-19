#ifndef VOLUMEDATA_H
#define VOLUMEDATA_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <string>

namespace ggraf
{
    class VolumeData
    {
    private:
        int m_iVaoId;
        int m_aTexIds[2];
        glm::vec3 m_vDimensions;
        glm::vec3 m_vScaleFactor;
        glm::mat4 m_mModelMatrix;

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

        ParsedVolPath* parseVolumePath(std::string path);
        ParsedTFPath* parseTFPath(std::string path);

    public:
        VolumeData();
        VolumeData(std::string volume_path, std::string tf_path);
        ~VolumeData();

        inline glm::mat4& getModelMatrix()
        {
            return m_mModelMatrix;
        }

        inline int getVolumeTexId()
        {
            return m_aTexIds[0];
        }

        inline int getTransferFuncTexId()
        {
            return m_aTexIds[1];
        }

        inline bool isVolumeLoaded()
        {
            return m_aTexIds[0] == 0? false : true;
        }

        inline bool isTfLoaded()
        {
            return m_aTexIds[1] == 0? false : true;
        }

        void loadVolume(std::string path);
        void loadTransferFunction(std::string path);
        void render();
    };
}

#endif // VOLUMEDATA_H
