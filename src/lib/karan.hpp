#ifndef KARAN_H
#define KARAN_H

#include "Sphere.h"
#include "vmath.h"
using namespace vmath;

void karan_code(mat4 translationMatrix, mat4 modelMatrix, mat4 viewMatrix, mat4 rotationMatrix);

void drawSphere(void);

void drawCube(void);

#endif
