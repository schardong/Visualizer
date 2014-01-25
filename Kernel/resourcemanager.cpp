#include "resourcemanager.h"
#include "logger.h"

#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>

using std::cout;
using std::cerr;
using std::endl;

namespace ggraf
{

    static void* loadRawFile(std::string filename, size_t num_elements, size_t bytes_per_element)
    {
        FILE* fp;
        Logger::getInstance()->log("ggraf::ResourceManager::loadRawFile(" +
                                   filename + ", " +
                                   std::to_string(num_elements) + ", " +
                                   std::to_string(bytes_per_element) + ")");

        if(!(fp = fopen(filename.c_str(), "rb"))) {
            Logger::getInstance()->error("Failed to open the file " + filename);
            return nullptr;
        }

        void* contents = calloc(num_elements, bytes_per_element);
        size_t bytes_read = fread(contents, bytes_per_element, num_elements, fp);
        fclose(fp);
        fp = nullptr;

        if(bytes_read != num_elements) {
            free(contents);
            contents = nullptr;
        }

        return contents;
    }

    static size_t saveRawFile(std::string filename, size_t num_elements, size_t bytes_per_element, void* contents)
    {
        if(filename.empty() || num_elements <= 0 || bytes_per_element <= 0 || contents == nullptr)
            return 0;

        FILE* fp;
        Logger::getInstance()->log("ggraf::ResourceManager::saveRawFile(" +
                                   filename + ", " +
                                   std::to_string(num_elements) + ", " +
                                   std::to_string(bytes_per_element) + ")");

        if(!(fp = fopen(filename.c_str(), "wb+"))) {
            Logger::getInstance()->error("Failed to open the file " + filename);
            return 0;
        }

//        fprintf(fp, "%s", contents);
        size_t bytes_written = fwrite(contents, bytes_per_element, num_elements, fp);
        fclose(fp);
        fp = nullptr;

        return bytes_written;
    }

    void* ResourceManager::loadVolumeData(std::string filename, size_t w, size_t h, size_t slices, size_t bytes_per_pixel)
    {
        size_t num_voxels = w * h * slices;

        Logger::getInstance()->log("ggraf::ResourceManager::loadVolumeData(" +
                                   filename + ", " +
                                   std::to_string(w) + ", " +
                                   std::to_string(h) + ", " +
                                   std::to_string(slices) + ", " +
                                   std::to_string(bytes_per_pixel) + ")");

        return loadRawFile(filename, num_voxels, bytes_per_pixel);
    }

    void* ResourceManager::loadVertexToBranchMap(std::string filename, size_t w, size_t h, size_t slices)
    {
        size_t num_voxels = w * h * slices;

        Logger::getInstance()->log("ggraf::ResourceManager::loadVertexToBranchMap(" +
                                   filename + ", " +
                                   std::to_string(w) + ", " +
                                   std::to_string(h) + ", " +
                                   std::to_string(slices) + ")");

        return loadRawFile(filename, num_voxels, sizeof(unsigned int));
    }

    size_t ResourceManager::saveVertexToBranchMap(std::string filename, size_t w, size_t h, size_t slices, unsigned int* branch_map)
    {
        size_t num_voxels = w * h * slices;

        Logger::getInstance()->log("ggraf::ResourceManager::saveVertexToBranchMap(" +
                                   filename + ", " +
                                   std::to_string(w) + ", " +
                                   std::to_string(h) + ", " +
                                   std::to_string(slices) + ")");

        return saveRawFile(filename, num_voxels, sizeof(unsigned int), branch_map);
    }

    unsigned char* ResourceManager::loadTransferFuncion(std::string filename, size_t bytes_per_pixel)
    {
        size_t num_rgba = bytes_per_pixel == sizeof(unsigned short) ? 4096 * 4 :
                                                                      256 * 4;

        Logger::getInstance()->log("ggraf::ResourceManager::loadTransferFuncion(" +
                                   filename + ", " +
                                   std::to_string(bytes_per_pixel) + ")");

        return (unsigned char*) loadRawFile(filename, num_rgba, 1);
    }

    unsigned char* ResourceManager::loadMultiDimensionalTransferFunction(std::string filename, size_t w, size_t h, size_t bytes_per_pixel)
    {
        size_t num_pixels = w * h;
//        size_t num_rgba = bytes_per_pixel == sizeof(unsigned short) ? num_pixels * sizeof(unsigned short) * 4 :
//                                                                      num_pixels * sizeof(unsigned char) * 4;

        Logger::getInstance()->log("ggraf::ResourceManager::loadMultiDimensionalTranferFunction(" +
                                   filename + ", " +
                                   std::to_string(w) + ", " +
                                   std::to_string(h) + ", " +
                                   std::to_string(bytes_per_pixel) + ")");

        return (unsigned char*) loadRawFile(filename, num_pixels, bytes_per_pixel);
    }

    size_t ResourceManager::saveMultiDimensionalTransferFunction(std::string filename, size_t w, size_t h, size_t bytes_per_pixel, void* tf_data)
    {
        size_t num_alpha = w * h;

        Logger::getInstance()->log("ggraf::ResourceManager::saveMultiDimensionalTransferFunction(" +
                                   filename + ", " +
                                   std::to_string(w) + ", " +
                                   std::to_string(h) + ", " +
                                   std::to_string(bytes_per_pixel) + ")");

        return saveRawFile(filename, num_alpha, bytes_per_pixel, tf_data);
    }

    GLuint ResourceManager::createVolumeTex(size_t w, size_t h, size_t slices, size_t bytes_per_pixel, void* data)
    {
        if(data == NULL)
            return 0;

        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_3D, tex);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        if(bytes_per_pixel == sizeof(GLushort)) {
            glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, w, h, slices, 0, GL_RED, GL_UNSIGNED_SHORT, data);
        } else if(bytes_per_pixel == sizeof(GLubyte)) {
            glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, w, h, slices, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        }

        glBindTexture(GL_TEXTURE_3D, 0);
        return tex;
    }

    GLuint ResourceManager::createTransferFuncTex(size_t bytes_per_pixel, unsigned char* data)
    {
        size_t file_size = bytes_per_pixel == sizeof(GLushort) ? 4096 * 4 : 256 * 4;

        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_1D, tex);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, file_size / 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_1D, 0);
        return tex;
    }

    GLuint ResourceManager::createMultiDimTransferFuncTex(size_t w, size_t h, size_t bytes_per_pixel, unsigned char* data)
    {
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);

        return tex;

//        size_t file_size = bytes_per_pixel == sizeof(GLushort) ? 4096 * 4 : 256 * 4;

//        GLuint tex;
//        glGenTextures(1, &tex);
//        glBindTexture(GL_TEXTURE_1D, tex);
//        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, file_size / 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//        glBindTexture(GL_TEXTURE_1D, 0);
//        return tex;

    }

    GLuint ResourceManager::createCubeVAO()
    {
        GLfloat cube_vertex[24] = {
            0.0, 0.0, 0.0,
            0.0, 0.0, 1.0,
            0.0, 1.0, 0.0,
            0.0, 1.0, 1.0,
            1.0, 0.0, 0.0,
            1.0, 0.0, 1.0,
            1.0, 1.0, 0.0,
            1.0, 1.0, 1.0
        };

        GLuint cube_index[36] = {
            1,5,7,
            7,3,1,
            0,2,6,
            6,4,0,
            0,1,3,
            3,2,0,
            7,5,4,
            4,6,7,
            2,3,7,
            7,6,2,
            1,0,4,
            4,5,1
        };

        GLuint vao, vbo, ibo;

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, cube_vertex, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 36, cube_index, GL_STATIC_DRAW);

        glBindVertexArray(0);

        return vao;
    }


    bool ResourceManager::uploadVolumeData(int w, int h, int slices, size_t bytes_per_pixel, void* data, GLuint texId)
    {
        if(texId == 0 || data == NULL)
            return false;

        glBindTexture(GL_TEXTURE_3D, texId);

        if(bytes_per_pixel == sizeof(GLushort))
            glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, w, h, slices, 0, GL_RED, GL_UNSIGNED_SHORT, data);
        else if(bytes_per_pixel == sizeof(GLubyte))
            glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, w, h, slices, 0, GL_RED, GL_UNSIGNED_BYTE, data);

        glBindTexture(GL_TEXTURE_3D, 0);

        return true;
    }

    bool ResourceManager::uploadTransferFuncData(size_t bytes_per_pixel, unsigned char* data, GLuint texId)
    {
        if(texId == 0 || data == NULL)
            return false;

        size_t sz = bytes_per_pixel == sizeof(GLushort) ? 4096 * 4 : 256 * 4;

        glBindTexture(GL_TEXTURE_1D, texId);

        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, sz / 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glBindTexture(GL_TEXTURE_1D, 0);

        return true;
    }

    bool ResourceManager::uploadMultiDimTransferFuncData(size_t w, size_t h, size_t bytes_per_pixel, unsigned char* data, GLuint texId)
    {
        if(texId == 0 || data == NULL)
            return false;

        glBindTexture(GL_TEXTURE_2D, texId);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);

        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    void ResourceManager::destroyCubeVao(GLuint cube_id)
    {
        if(cube_id != 0)
            glDeleteVertexArrays(1, &cube_id);
    }
}
