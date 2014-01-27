#include "multidimvolumedata.h"
#include "resourcemanager.h"
#include "logger.h"

#include <cstring>
#include <iostream>
#include <vector>
#include <sstream>
#include <boost/algorithm/string.hpp>

namespace ggraf
{

    MultiDimVolumeData::MultiDimVolumeData()
    {}

    MultiDimVolumeData::MultiDimVolumeData(std::string volume_path, std::string vtb_path, std::string opacity_tf_path, std::string color_tf_path)
    {
        m_aTexIds = std::vector<int>(4);
        m_dataTypes = std::vector<size_t>(4);

        loadVolume(volume_path);
        loadVertexToBranchMap(vtb_path);
        loadOpacityTransferFunction(opacity_tf_path);
        loadColorTransferFunction(color_tf_path);
    }

    MultiDimVolumeData::~MultiDimVolumeData()
    {
        ggraf::ResourceManager::getInstance()->destroyCubeVao(m_iVaoId);
        m_mModelMatrix = glm::mat4(1.f);
        m_vDimensions = glm::vec3(1.f);
        m_vScaleFactor = glm::vec3(1.f);

        const GLuint tex1 = m_aTexIds[0];
        const GLuint tex2 = m_aTexIds[1];
        const GLuint tex3 = m_aTexIds[2];
        const GLuint tex4 = m_aTexIds[3];

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, 0);
        glDeleteTextures(1, &tex1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, 0);
        glDeleteTextures(1, &tex2);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &tex3);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_1D, 0);
        glDeleteTextures(1, &tex4);
    }

    void MultiDimVolumeData::loadOpacityTransferFunction(std::string path)
    {
        Logger::getInstance()->log("ggraf::MultiDimVolumeData::loadOpacityTransferFunction(" + path + ")");
        if(path.empty()) {
            Logger::getInstance()->error("Invalid path provided, opacity transfer function not loaded.");
            return;
        }

        ParsedMultiDimTFPath* tfp = parseMultiDimTFPath(path);

        unsigned char* tf = ResourceManager::getInstance()->loadMultiDimensionalTransferFunction(tfp->path, tfp->w, tfp->h, tfp->bytes_per_pixel);

        if(m_aTexIds[2] != 0) {
            if(ggraf::ResourceManager::getInstance()->uploadMultiDimTransferFuncData(tfp->w, tfp->h, tfp->bytes_per_pixel, tf, m_aTexIds[2]) == false) {
                Logger::getInstance()->error("Failed to upload the transfer function to the GPU.");
                return;
            }
        } else
            m_aTexIds[2] = ggraf::ResourceManager::getInstance()->createMultiDimTransferFuncTex(tfp->w, tfp->h, tfp->bytes_per_pixel, tf);

        m_dataTypes[2] = tfp->bytes_per_pixel;

        free(tf);
        tf = NULL;
    }

    void MultiDimVolumeData::loadColorTransferFunction(std::string path)
    {
        Logger::getInstance()->log("ggraf::MultiDimVolumeData::loadColorTransferFunction(" + path + ")");
        if(path.empty()) {
            Logger::getInstance()->error("Invalid path provided, color transfer function not loaded.");
            return;
        }

        ParsedTFPath* tfp = parseTFPath(path);

        unsigned char* tf = ResourceManager::getInstance()->loadColorTransferFunction(tfp->path, tfp->bytes_per_pixel);

        if(m_aTexIds[3] != 0) {
            if(ggraf::ResourceManager::getInstance()->uploadColorTransferFuncData(tfp->bytes_per_pixel, tf, m_aTexIds[3]) == false) {
                Logger::getInstance()->error("Failed to upload the transfer function to the GPU.");
                return;
            }
        } else
            m_aTexIds[3] = ggraf::ResourceManager::getInstance()->createColorTransferFuncTex(tfp->bytes_per_pixel, tf);

        m_dataTypes[3] = tfp->bytes_per_pixel;

        free(tf);
        tf = NULL;
    }

    void MultiDimVolumeData::loadVertexToBranchMap(std::string path)
    {
        Logger::getInstance()->log("ggraf::MultiDimVolumeData::loadVertexToBranchMap(" + path + ")");

        if(path.empty()) {
            Logger::getInstance()->error("Invalid path provided, vertex to branch map not loaded.");
            return;
        }

        ParsedVolPath* v = parseVolumePath(path);

        float* voxels = ggraf::ResourceManager::getInstance()->loadVertexToBranchMap(v->path, v->dim[0], v->dim[1], v->dim[2]);

        if(m_aTexIds[1] != 0) {
            if(ggraf::ResourceManager::getInstance()->uploadVertexBranchData(v->dim[0], v->dim[1], v->dim[2], voxels, m_aTexIds[1]) == false) {
                Logger::getInstance()->error("Failed to upload the vertex to branch map to the GPU.");
                return;
            }
        } else
            m_aTexIds[1] = ggraf::ResourceManager::getInstance()->createVertexBranchTex(v->dim[0], v->dim[1], v->dim[2], voxels);

        m_dataTypes[1] = v->bytes_per_pixel;
        m_vDimensions = glm::vec3(v->dim[0], v->dim[1], v->dim[2]);
        m_vScaleFactor = glm::normalize(m_vDimensions);

        m_mModelMatrix = glm::scale(glm::mat4(1.f), m_vScaleFactor);
        m_mModelMatrix = glm::translate<float>(m_mModelMatrix, glm::vec3(-.5, -.5, -.5));

        free(voxels);
        voxels = NULL;

    }

    MultiDimVolumeData::ParsedMultiDimTFPath* MultiDimVolumeData::parseMultiDimTFPath(std::string path)
    {
        if(path.empty())
            return NULL;

        ParsedMultiDimTFPath* tf = new ParsedMultiDimTFPath;
        std::vector<std::string> strs(3);
        boost::algorithm::split(strs, path, boost::is_any_of("."));

        tf->path = path;

        if(strs.back() == "uint8") {
            tf->bytes_per_pixel = sizeof(GLubyte);
            tf->w = 256;
        } else {
            tf->bytes_per_pixel = sizeof(GLushort);
            tf->w = 4096;
        }
        std::istringstream(strs[1]) >> tf->h;

        return tf;
    }

}
