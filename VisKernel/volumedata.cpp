#include "GL3/gl3w.h"
#include "volumedata.h"
#include "resourcemanager.h"

#include <cstring>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

namespace ggraf
{
    VolumeData::VolumeData()
    {
        m_aTexIds[0] = m_aTexIds[1] = 0;
        m_iVaoId = ggraf::ResourceManager::getInstance()->createCubeVAO();
    }

    VolumeData::VolumeData(std::string volume_path, std::string tf_path, int width, int height, int slices, size_t bytes_per_pixel)
    {
        void* voxels = ggraf::ResourceManager::getInstance()->loadVolumeData(volume_path, width, height, slices, bytes_per_pixel);
        m_aTexIds[0] = ggraf::ResourceManager::getInstance()->createVolumeTex(width, height, slices, bytes_per_pixel, voxels);

        unsigned char* tf = ggraf::ResourceManager::getInstance()->loadTransferFuncion(tf_path, bytes_per_pixel);
        m_aTexIds[1] = ggraf::ResourceManager::getInstance()->createTransferFuncTex(bytes_per_pixel, tf);

        m_iVaoId = ggraf::ResourceManager::getInstance()->createCubeVAO();

        m_vDimensions = glm::vec3(width, height, slices);
        m_vScaleFactor = glm::normalize(m_vDimensions);

        m_mModelMatrix = glm::scale(glm::mat4(1.f), m_vScaleFactor);
        m_mModelMatrix = glm::translate<float>(m_mModelMatrix, glm::vec3(-.5, -.5, -.5));

        free(voxels);
        free(tf);
        voxels = NULL;
        tf = NULL;
    }

    VolumeData::VolumeData(void* voxels, unsigned char* tff, int width, int height, int slices, size_t bytes_per_pixel)
    {
        if(voxels == NULL || tff == NULL) {
            cerr << "Error: invalid data. (VolumeData::VolumeData(void* voxels, unsigned char* tff, int width, int height, int slices, size_t bytes_per_pixel))" << endl;
            return;
        }

        m_aTexIds[0] = ggraf::ResourceManager::getInstance()->createVolumeTex(width, height, slices, bytes_per_pixel, voxels);
        m_aTexIds[1] = ggraf::ResourceManager::getInstance()->createTransferFuncTex(bytes_per_pixel, tff);

        m_iVaoId = ggraf::ResourceManager::getInstance()->createCubeVAO();

        m_vDimensions = glm::vec3(width, height, slices);
        m_vScaleFactor = glm::normalize(m_vDimensions);

        m_mModelMatrix = glm::scale(glm::mat4(1.f), m_vScaleFactor);
        m_mModelMatrix = glm::translate<float>(m_mModelMatrix, glm::vec3(-.5, -.5, -.5));
    }

    VolumeData::~VolumeData()
    {
        ggraf::ResourceManager::getInstance()->destroyCubeVao(m_iVaoId);
        m_mModelMatrix = glm::mat4(1.f);
        m_vDimensions = glm::vec3(1.f);
        m_vScaleFactor = glm::vec3(1.f);

        const GLuint tex1 = m_aTexIds[0];
        const GLuint tex2 = m_aTexIds[1];
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, 0);
        glDeleteTextures(1, &tex1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_1D, 0);
        glDeleteTextures(1, &tex2);
    }

    void VolumeData::render()
    {
        glBindVertexArray(m_iVaoId);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
        glBindVertexArray(0);
    }

    void VolumeData::loadVolume(std::string path, int width, int height, int slices, int bytes_per_pixel)
    {
        if(path.empty()) {
            cerr << "Invalid path provided, volume not loaded." << endl;
            return;
        }

        void* voxels = ggraf::ResourceManager::getInstance()->loadVolumeData(path, width, height, slices, bytes_per_pixel);
        m_aTexIds[0] = ggraf::ResourceManager::getInstance()->createVolumeTex(width, height, slices, bytes_per_pixel, voxels);

        m_vDimensions = glm::vec3(width, height, slices);
        m_vScaleFactor = glm::normalize(m_vDimensions);

        m_mModelMatrix = glm::scale(glm::mat4(1.f), m_vScaleFactor);
        m_mModelMatrix = glm::translate<float>(m_mModelMatrix, glm::vec3(-.5, -.5, -.5));

        free(voxels);
        voxels = NULL;
    }

    void VolumeData::loadTransferFunction(std::string path, size_t bytes_per_pixel)
    {
        unsigned char* tf = ggraf::ResourceManager::getInstance()->loadTransferFuncion(path, bytes_per_pixel);
        m_aTexIds[1] = ggraf::ResourceManager::getInstance()->createTransferFuncTex(bytes_per_pixel, tf);

        free(tf);
        tf = NULL;
    }
}
