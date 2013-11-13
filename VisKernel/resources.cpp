#include "resources.h"
#include "resourcemanager.h"

#include <fstream>
#include <iostream>

GLuint load3dTex(std::string filename, int w, int h, int slices, size_t bytes_per_pixel)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_3D, tex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    GLvoid* data = ResourceManager::getInstance()->loadVolumeData(filename, w, h, slices, bytes_per_pixel);
    if(bytes_per_pixel == sizeof(GLushort)) {
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, w, h, slices, 0, GL_RED, GL_UNSIGNED_SHORT, data);
    } else if(bytes_per_pixel == sizeof(GLubyte)) {
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, w, h, slices, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    }

    glBindTexture(GL_TEXTURE_3D, 0);
    free(data);
    data = NULL;
    return tex;
}

GLuint create3dTex(int w, int h, int slices, size_t bytes_per_pixel, void* data)
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

void printFramebufferInfo(GLenum target, GLuint fbo) {

    int res, i = 0;
    GLint buffer;

    glBindFramebuffer(target, fbo);

    do {
        glGetIntegerv(GL_DRAW_BUFFER0 + i, &buffer);

        if (buffer != GL_NONE) {
            printf("Shader Output Location %d - color attachment %d\n", i, buffer - GL_COLOR_ATTACHMENT0);
            glGetFramebufferAttachmentParameteriv(target, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &res);
            printf("\tAttachment Type: %s\n", res==GL_TEXTURE?"Texture":"Renderbuffer");
            glGetFramebufferAttachmentParameteriv(target, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &res);
            printf("\tAttachment object name: %d\n",res);
        }
        ++i;

    } while (buffer != GL_NONE);

    glBindFramebuffer(target, 0);
}

GLuint createCube()
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

GLuint createCubeCenter()
{
    GLfloat cube_vertex[3] = {0.f, 0.f, 0.f};

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3, cube_vertex, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    return vao;
}

void destroyCube(GLuint cube_id)
{
    if(cube_id != 0)
        glDeleteVertexArrays(1, &cube_id);
}

GLuint loadTransferFunction(std::string filename, size_t bytes_per_pixel)
{
    size_t file_size = bytes_per_pixel == sizeof(GLushort) ? 4096 * 4 : 256 * 4;

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_1D, tex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLubyte* data = ResourceManager::getInstance()->loadTransferFuncion(filename, bytes_per_pixel);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, file_size / 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    free(data);
    data = NULL;
    glBindTexture(GL_TEXTURE_1D, 0);
    return tex;
}

GLuint createTransferFuncion(size_t bytes_per_pixel, unsigned char* data)
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
