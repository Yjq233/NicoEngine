#define _CRT_SECURE_NO_WARNINGS
#include"UMFLoader.h"

bool SaveUMF(char *file, stUMFModel *meshes, int numMeshes)
{
   FILE *fp;
   int id = 9004;
   int f = 0;

   // Make sure we have valid data.
   if(!file) return false;
   if(!meshes || numMeshes <= 0) return false;

   // Open file.
   fp = fopen(file, "wb");
   if(!fp) return false;

   // Write ID (9004).
   fwrite(&id, 4, 1, fp);

   // Write total meshes.
   fwrite(&numMeshes, 4, 1, fp);

   // Save each mesh to the file.
   for(int m = 0; m < numMeshes; m++)
      {
         // Write material data.
         fwrite(&meshes[m].diffuse, 4 * 3, 1, fp);
         fwrite(&meshes[m].specular, 4 * 3, 1, fp);
         fwrite(&meshes[m].power, 4, 1, fp);

         // Write number of verts then vertices.
         fwrite(&meshes[m].totalVertices, 4, 1, fp);
         fwrite(meshes[m].vertices, sizeof(CVector3) * meshes[m].totalVertices, 1, fp);

         // Write normals.  Start with a flag indicating there is data.
         if(!meshes[m].normals) fwrite(&f, 4, 1, fp);
         else
            {
               fwrite(&meshes[m].totalVertices, 4, 1, fp);
               fwrite(meshes[m].normals, sizeof(CVector3) * meshes[m].totalVertices, 1, fp);
            }

         // Write colors.
         if(!meshes[m].colors) fwrite(&f, 4, 1, fp);
         else
            {
               fwrite(&meshes[m].totalVertices, 4, 1, fp);
               fwrite(meshes[m].colors, sizeof(CVector3) * meshes[m].totalVertices, 1, fp);
            }

         // Write texture coords.
         if(!meshes[m].texCoords) fwrite(&f, 4, 1, fp);
         else
            {
               fwrite(&meshes[m].totalVertices, 4, 1, fp);
               fwrite(meshes[m].texCoords, sizeof(stTexCoord) * meshes[m].totalVertices, 1, fp);
            }

         // Write total faces, and face data.
         fwrite(&meshes[m].totalFaces, 4, 1, fp);
         for(int i = 0; i < meshes[m].totalFaces; i++)
            {
               fwrite(meshes[m].faces[i].indices, 4 * 3, 1, fp);
               fwrite(&meshes[m].faces[i].normal, sizeof(CVector3), 1, fp);
            }

         // Write bone indices.
         if(!meshes[m].boneIndices) fwrite(&f, 4, 1, fp);
         else
            {
               fwrite(&meshes[m].totalVertices, 4, 1, fp);
               fwrite(meshes[m].boneIndices, sizeof(int) * meshes[m].totalVertices, 1, fp);
            }

         // Write bones.
         if(!meshes[m].bones) fwrite(&f, 4, 1, fp);
         else
            {
               fwrite(&meshes[m].totalBones, 4, 1, fp);

               for(int b = 0; b < meshes[m].totalBones; b++)
                  {
                     fwrite(meshes[m].bones[b].name, 32, 1, fp);
                     fwrite(&meshes[m].bones[b].id, sizeof(unsigned long), 1, fp);
                     fwrite(meshes[m].bones[b].parent, 32, 1, fp);
                     fwrite(&meshes[m].bones[b].parentID, sizeof(int), 1, fp);
                     fwrite(&meshes[m].bones[b].totalFrames, sizeof(int), 1, fp);
                     fwrite(meshes[m].bones[b].keyFrames,
                            sizeof(stKeyFrame) * meshes[m].bones[b].totalFrames, 1, fp);
                  }
            }
      }

   fclose(fp);
   return true;
}


bool LoadUMF(char *file, stUMFModel **model, int *totalModels)
{
   FILE *fp = NULL;
   int id = 0, temp = 0, numVerts = 0, numMeshes = 0;
   CVector3 *tVec = NULL;
   stTexCoord *pTexC = NULL;
   stFace *pFaces = NULL;
   stUMFModel *meshes = NULL;

   // Error checking.
   if(!file || !model) return false;

   // Open file.
   fp = fopen(file, "rb");
   if(!fp) return false;

   // All UMF files have an id of 9004.
   fread(&id, 4, 1, fp);
   if(id != 9004) return false;

   fread(&numMeshes, 4, 1, fp);
   if(!numMeshes) { fclose(fp); return false; }

   meshes = new stUMFModel[numMeshes];
   if(!meshes) { fclose(fp); return false; } 

   for(int i = 0; i < numMeshes; i++)
      {
         // Read material data.
         fread(&meshes[i].diffuse, 4 * 3, 1, fp);
         fread(&meshes[i].specular, 4 * 3, 1, fp);
         fread(&meshes[i].power, 4, 1, fp);

         // Read vertices.  Start with total then read vertices.
         fread(&numVerts, 4, 1, fp);
         meshes[i].totalVertices = numVerts;

         // Read vertex data.
         tVec = new CVector3[numVerts];
         fread(tVec, sizeof(CVector3) * numVerts, 1, fp);
         meshes[i].vertices = tVec;

         // Read normals.  Start with the total normals. If any then we have normals.
         fread(&temp, 4, 1, fp);
         if(temp > 0)
            {
               tVec = new CVector3[numVerts];
               fread(tVec, sizeof(CVector3) * numVerts, 1, fp);
               meshes[i].normals = tVec;
            }

         // Read colors.
         fread(&temp, 4, 1, fp);
         if(temp > 0)
            {
               tVec = new CVector3[numVerts];
               fread(tVec, sizeof(CVector3) * numVerts, 1, fp);
               meshes[i].colors = tVec;
            }

         // Read texture coords.
         fread(&temp, 4, 1, fp);
         if(temp > 0)
            {
               pTexC = new stTexCoord[numVerts];
               fread(pTexC, sizeof(stTexCoord) * numVerts, 1, fp);
               meshes[i].texCoords = pTexC;
            }

         // Read face data.
         fread(&temp, 4, 1, fp);
         if(temp > 0)
            {
               // Get number, allocate space, then read face data.
               meshes[i].totalFaces = temp;

               pFaces = new stFace[meshes[i].totalFaces];

               for(int f = 0; f < meshes[i].totalFaces; f++)
                  {
                     fread(pFaces[f].indices, 4 * 3, 1, fp);
                     fread(&pFaces[f].normal, sizeof(CVector3), 1, fp);
                  }

               meshes[i].faces = pFaces;
            }

         // Read bone indices.
         fread(&temp, 4, 1, fp);
         if(temp > 0)
            {
               int *pIndices = new int[numVerts];
               fread(pIndices, sizeof(int) * numVerts, 1, fp);
               meshes[i].boneIndices = pIndices;
            }

         // Read bones.
         fread(&temp, 4, 1, fp);
         if(temp > 0)
            {
               meshes[i].totalBones = temp;
               stBone *pBones = new stBone[meshes[i].totalBones];
               
               for(int b = 0; b < meshes[i].totalBones; b++)
                  {
                     fread(pBones[b].name, 32, 1, fp);
                     fread(&pBones[b].id, sizeof(unsigned long), 1, fp);
                     fread(pBones[b].parent, 32, 1, fp);
                     fread(&pBones[b].parentID, sizeof(int), 1, fp);
                     fread(&pBones[b].totalFrames, sizeof(int), 1, fp);

                     stKeyFrame *pKeys = new stKeyFrame[pBones[b].totalFrames];
                     fread(pKeys, sizeof(stKeyFrame) * pBones[b].totalFrames, 1, fp);
                  
                     pBones[b].keyFrames = pKeys;
                  }

               meshes[i].bones = pBones;
            }

         // Calculate model's bounding box.
         tVec = meshes[i].vertices;
         for(int k = 0; k < numVerts; k++)
            {
               if(tVec[k].x < meshes[i].bbMin.x) meshes[i].bbMin.x = tVec[k].x;
               if(tVec[k].y < meshes[i].bbMin.y) meshes[i].bbMin.y = tVec[k].y;
               if(tVec[k].z < meshes[i].bbMin.z) meshes[i].bbMin.z = tVec[k].z;

               if(tVec[k].x > meshes[i].bbMax.x) meshes[i].bbMax.x = tVec[k].x;
               if(tVec[k].y > meshes[i].bbMax.y) meshes[i].bbMax.y = tVec[k].y;
               if(tVec[k].z > meshes[i].bbMax.z) meshes[i].bbMax.z = tVec[k].z;
            }
      }

   // Save information to pointer parameters.
   *model = meshes;
   if(totalModels) *totalModels = numMeshes;

   fclose(fp);
   return true;
}


void FreeModel(stUMFModel *mesh)
{
   if(!mesh) return;

   // Release all resources.
   if(mesh->faces) delete[] mesh->faces;
   mesh->faces = NULL;

   if(mesh->vertices) delete[] mesh->vertices;
   mesh->vertices = NULL;

   if(mesh->texCoords) delete[] mesh->texCoords;
   mesh->texCoords = NULL;

   if(mesh->boneIndices) delete[] mesh->boneIndices;
   mesh->boneIndices = NULL;

   if(mesh->bones) delete[] mesh->bones;
   mesh->bones = NULL;

   if(mesh->colors) delete[] mesh->colors;
   mesh->colors = NULL;

   if(mesh->normals) delete[] mesh->normals;
   mesh->normals = NULL;
}


void FreeModel(stUMFModel *meshes, int numMeshes)
{
   if(!meshes || numMeshes <= 0) return;

   for(int i = 0; i < numMeshes; i++) FreeModel(&meshes[i]);
}