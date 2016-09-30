#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Token.h"

// Pretty straight forward don't you think?
struct stObjModel
{
   float *vertices;
   float *normals;
   float *texCoords;
   int numFaces;
};

stObjModel *LoadOBJModel(char *fileName);
void FreeOBJModel(stObjModel *model);