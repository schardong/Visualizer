#include "GL3/gl3w.h"
#include "volumedata.h"
#include "resourcemanager.h"
#include "logger.h"

#include <cstring>
#include <iostream>
#include <vector>
#include <sstream>
#include <boost/algorithm/string.hpp>

using std::cout;
using std::cerr;
using std::endl;

namespace ggraf
{
    VolumeData::VolumeData()
    {
        m_aTexIds = std::vector<int>(2);
        m_dataTypes = std::vector<size_t>(2);
        m_iVaoId = ggraf::ResourceManager::getInstance()->createCubeVAO();
    }

    VolumeData::VolumeData(std::string volume_path, std::string tf_path)
    {
        m_aTexIds = std::vector<int>(2);
        m_dataTypes = std::vector<size_t>(2);

        loadVolume(volume_path);
        loadTransferFunction(tf_path);

        m_iVaoId = ggraf::ResourceManager::getInstance()->createCubeVAO();
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
        ResourceManager::getInstance()->renderCubeVAO(m_iVaoId);
    }

    void VolumeData::loadVolume(std::string path)
    {
        Logger::getInstance()->log("ggraf::VolumeData::loadVolume(" + path + ")");

        if(path.empty()) {
            Logger::getInstance()->error("Invalid path provided, volume not loaded.");
            return;
        }

        ParsedVolPath* v = parseVolumePath(path);

        void* voxels = ggraf::ResourceManager::getInstance()->loadVolumeData(v->path, v->dim[0], v->dim[1], v->dim[2], v->bytes_per_pixel);

        if(m_aTexIds[0] != 0) {
            if(ggraf::ResourceManager::getInstance()->uploadVolumeData(v->dim[0], v->dim[1], v->dim[2], v->bytes_per_pixel, voxels, m_aTexIds[0]) == false) {
                Logger::getInstance()->error("Failed to upload the volume to the GPU.");
                return;
            }
        } else
            m_aTexIds[0] = ggraf::ResourceManager::getInstance()->createVolumeTex(v->dim[0], v->dim[1], v->dim[2], v->bytes_per_pixel, voxels);

        m_dataTypes[0] = v->bytes_per_pixel;

        m_vDimensions = glm::vec3(v->dim[0], v->dim[1], v->dim[2]);
        m_vScaleFactor = glm::normalize(m_vDimensions);

        m_mModelMatrix = glm::scale(glm::mat4(1.f), m_vScaleFactor);
        m_mModelMatrix = glm::translate<float>(m_mModelMatrix, glm::vec3(-.5, -.5, -.5));

        free(voxels);
        voxels = NULL;
    }

    void VolumeData::loadTransferFunction(std::string path)
    {
        Logger::getInstance()->log("ggraf::VolumeData::loadTransferFunction(" + path + ")");
        if(path.empty()) {
            Logger::getInstance()->error("Invalid path provided, transfer function not loaded.");
            return;
        }

        ParsedTFPath* tfp = parseTFPath(path);

        unsigned char* tf = ggraf::ResourceManager::getInstance()->loadTransferFunction(tfp->path, tfp->bytes_per_pixel);

        if(m_aTexIds[1] != 0) {
            if(ggraf::ResourceManager::getInstance()->uploadTransferFuncData(tfp->bytes_per_pixel, tf, m_aTexIds[1]) == false) {
                Logger::getInstance()->error("Failed to upload the transfer function to the GPU.");
                return;
            }
        } else
            m_aTexIds[1] = ggraf::ResourceManager::getInstance()->createTransferFuncTex(tfp->bytes_per_pixel, tf);

        m_dataTypes[1] = tfp->bytes_per_pixel;

        free(tf);
        tf = NULL;
    }

    VolumeData::ParsedVolPath* VolumeData::parseVolumePath(std::string path)
    {
        if(path.empty())
            return NULL;

        ParsedVolPath* v = new ParsedVolPath;
        std::vector<std::string> strs;
        boost::algorithm::split(strs, path, boost::is_any_of("."));

        v->path = path;

        v->bytes_per_pixel = strs[2] == "uint8" ? sizeof(GLubyte) :
                                                  (strs[2] == "uint32" ? sizeof(GLuint) :
                                                                         (strs[2] == "float" ? sizeof(GLfloat) : sizeof(GLushort)));

        std::vector<std::string> dims_strs;
        boost::algorithm::split(dims_strs, strs[1], boost::is_any_of("x"));

        std::istringstream(dims_strs[0]) >> v->dim[0];
        std::istringstream(dims_strs[1]) >> v->dim[1];
        std::istringstream(dims_strs[2]) >> v->dim[2];

        return v;
    }

    VolumeData::ParsedTFPath* VolumeData::parseTFPath(std::string path)
    {
        if(path.empty())
            return NULL;

        ParsedTFPath* tf = new ParsedTFPath;
        std::vector<std::string> strs;
        boost::algorithm::split(strs, path, boost::is_any_of("."));

        tf->path = path;
        tf->bytes_per_pixel = strs.back() == "uint8" ? sizeof(GLubyte) : sizeof(GLushort);

        return tf;
    }
}
