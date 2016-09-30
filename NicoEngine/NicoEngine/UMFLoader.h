#pragma once

#include <stdio.h>
#include <math.h>
#include "Matrix.h"


struct stTexCoord
{
   stTexCoord() : tu(0), tv(0) {}
   // tu, tv texture coordinates.
   float tu, tv;
};

struct stFace
{
   stFace() { indices[0] = indices[1] = indices[2] = 0; }

   // Vertex indexes and a surface normal.
   unsigned int indices[3];
   CVector3 normal;
};

struct stKeyFrame
{
   stKeyFrame() : time(0) {}

   float time;
   CVector3 rotation, translation;
};

struct stBone
{
   stBone() : id(0), totalFrames(0), currentFrame(0),
              keyFrames(0), parentID(0){}

   char name[32];
   unsigned long id;
   char parent[32];
   int parentID;

   CMatrix4x4 relative, absolute;
   int currentFrame, totalFrames;
   stKeyFrame *keyFrames;
};

struct stUMFModel
{
   stUMFModel() : vertices(0), faces(0), normals(0), colors(0), boneIndices(0),
                  bones(0), totalBones(0), totalVertices(0),
                  totalFaces(0), power(0), texCoords(0) {}

   // Material data.
   CVector3 diffuse;
   CVector3 specular;
   int power;

   // Model data.
   CVector3 *vertices;
   stTexCoord *texCoords;
   CVector3 *normals;
   CVector3 *colors;
   int *boneIndices;
   stBone *bones;
   stFace *faces;

   // Array counters.
   int totalVertices;
   int totalBones;
   int totalFaces;

   // Bounding box data.
   CVector3 bbMin, bbMax;
};


bool SaveUMF(char *file, stUMFModel *meshes, int numMeshes);
bool LoadUMF(char *file, stUMFModel **model, int *totalModels);
void FreeModel(stUMFModel *mesh);
void FreeModel(stUMFModel *meshes, int numMeshes);