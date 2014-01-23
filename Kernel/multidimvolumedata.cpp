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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &tex3);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, 0);
        glDeleteTextures(1, &tex4);
    }

    void MultiDimVolumeData::render()
    {

    }

    void MultiDimVolumeData::loadOpacityTransferFunction(std::string path)
    {/*
        Logger::getInstance()->warn("MultiDimVolumeData::loadOpacityTransferFunction(" +
                                    path + ") not yet implemented");*/

        Logger::getInstance()->log("ggraf::MultiDimVolumeData::loadOpacityTransferFunction(" + path + ")");
        if(path.empty()) {
            Logger::getInstance()->error("Invalid path provided, opacity transfer function not loaded.");
            return;
        }

        ParsedMultiDimTFPath* tfp = parseMultiDimTFPath(path);

//        unsigned char* tf_data = ggraf::ResourceManager::getInstance()->loadMultiDimensionalTransferFunction(tfp->path, tfp->w, tfp->h, tfp->bytes_per_pixel);
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

    /*Logger::getInstance()->log("ggraf::VolumeData::loadTransferFunction(" + path + ")");
        if(path.empty()) {
            Logger::getInstance()->error("Invalid path provided, transfer function not loaded.");
            return;
        }

        ParsedTFPath* tfp = parseTFPath(path);

        unsigned char* tf = ggraf::ResourceManager::getInstance()->loadTransferFuncion(tfp->path, tfp->bytes_per_pixel);

        if(m_aTexIds[1] != 0) {
            if(ggraf::ResourceManager::getInstance()->uploadTransferFuncData(tfp->bytes_per_pixel, tf, m_aTexIds[1]) == false) {
                Logger::getInstance()->error("Failed to upload the transfer function to the GPU.");
                return;
            }
        } else
            m_aTexIds[1] = ggraf::ResourceManager::getInstance()->createTransferFuncTex(tfp->bytes_per_pixel, tf);

        m_dataTypes.second = tfp->bytes_per_pixel;

        free(tf);
        tf = NULL;*/

    MultiDimVolumeData::ParsedVolPath* MultiDimVolumeData::parseVTBVolumePath(std::string path)
    {
        if(path.empty())
            return NULL;

        ParsedVolPath* v = new ParsedVolPath;
        std::vector<std::string> strs;
        boost::algorithm::split(strs, path, boost::is_any_of("."));

        v->path = path;

        v->bytes_per_pixel = strs[2] == "uint8" ? sizeof(GLubyte) : sizeof(GLushort);

        std::vector<std::string> dims_strs;
        boost::algorithm::split(dims_strs, strs[1], boost::is_any_of("x"));

        std::istringstream(dims_strs[0]) >> v->dim[0];
        std::istringstream(dims_strs[1]) >> v->dim[1];
        std::istringstream(dims_strs[2]) >> v->dim[2];

        return v;
    }

    MultiDimVolumeData::ParsedMultiDimTFPath* MultiDimVolumeData::parseMultiDimTFPath(std::string path)
    {
        if(path.empty())
            return NULL;

        ParsedMultiDimTFPath* tf = new ParsedMultiDimTFPath;
        std::vector<std::string> strs;
        boost::algorithm::split(strs, path, boost::is_any_of("."));
        std::clog << strs[0] << " " << strs[1] << " " << strs[2] << std::endl;

        tf->path = path;
        tf->bytes_per_pixel = strs.back() == "uint8" ? sizeof(GLubyte) : sizeof(GLushort);

        return tf;
    }

}
