#ifndef RESOURCES_H
#define RESOURCES_H

#define LOGGING_LEVEL_1

#include "GL3/gl3w.h"
#include <string>

GLuint load3dTex(std::string filename, int w, int h, int slices, size_t bytes_per_pixel);

//GLuint create3dTex(int w, int h, int slices, size_t bytes_per_pixel, void* data); //

void printFramebufferInfo(GLenum target, GLuint fbo);

//GLuint createCube(); //

GLuint createCubeCenter();

//void destroyCube(GLuint cube_id);

GLuint loadTransferFunction(std::string filename, size_t bytes_per_pixel);

//GLuint createTransferFuncion(size_t bytes_per_pixel, unsigned char* data); //

#endif // RESOURCES_H
