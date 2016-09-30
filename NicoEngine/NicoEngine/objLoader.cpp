#define _CRT_SECURE_NO_WARNINGS
#include "objLoader.h"


stObjModel *LoadOBJModel(char *fileName)
{
   FILE *file;
   char *data = NULL;
   CToken lexer, tempLex;
   char tempLine[512];
   char token[512];

   // Open file for input.
   file = fopen(fileName, "r");
   if(!file) return NULL;

      // Get the length of the file.
      fseek(file, 0, SEEK_END);
      int length = ftell(file);
      fseek(file, 0, SEEK_SET);

      // Read in all data from the file.
      data = new char[(length + 1) * sizeof(char)];
      if(!data) return NULL;
      fread(data, length, 1, file);
      data[length] = '\0';

   // Close the file when we are done.
   fclose(file);

   // Set our file to our lexer.
   lexer.SetTokenStream(data);

   // No longer need.
   delete[] data; data = NULL;

   bool validFile = false;

   // Look for the word Wavefront somewhere in the file to
   // determine if this .obj is compatiable since so modelers export
   // to slightly different formats.
   while(lexer.GetNextToken(token))
      {
         if(strcmp(token, "Wavefront") == 0)
            {
               validFile = true;
               break;
            }
      }

   if(!validFile) return NULL;

   // Reset for next pass.
   lexer.Reset();

   // Used to get the total number of each declared in a file.
   // Since faces uses indices these number could be different.
   int totalVertices = 0, totalNormals = 0,
       totalTexC = 0, totalFaces = 0;

   // Get the first (or next) line.
   while(lexer.MoveToNextLine(tempLine))
      {
         // Set line to the temp lexer.
         tempLex.SetTokenStream(tempLine);

         // Read the new line character.
         lexer.GetNextToken(NULL);

         // If something was set to the temp lex then we keep going.
         if(!tempLex.GetNextToken(token)) continue;

         // If the first token of the line  is a v, vn, vt, or f
         // increment the respective counter.
         if(strcmp(token, "v") == 0) totalVertices++;
         else if(strcmp(token, "vn") == 0) totalNormals++;
         else if(strcmp(token, "vt") == 0) totalTexC++;
         else if(strcmp(token, "f") == 0) totalFaces++;

         token[0] = '\0';
      }

   // Allocate temp space to hold the data.  Face are by 9 since there are
   // 3 vertices each with 3 values (v index/vt index/vn index).
   float *verts = new float[totalVertices * 3];
   float *norms = new float[totalNormals * 3];
   float *texC = new float[totalTexC * 2];
   int *faces = new int[totalFaces * 9];
   int vIndex = 0, nIndex = 0, tIndex = 0, fIndex = 0, index = 0;

   // Move to the beginning of the file.
   lexer.Reset();

   // Do it all again but this time we get the data.
   while(lexer.MoveToNextLine(tempLine))
      {
         // Set to temp lex, read past newline, get token.
         tempLex.SetTokenStream(tempLine);
         lexer.GetNextToken(NULL);
         if(!tempLex.GetNextToken(token)) continue;

         // If v then we get the vertex x, y, z.
         if(strcmp(token, "v") == 0)
            {
               // Get the x and save it.
               tempLex.GetNextToken(token);
               verts[vIndex] = (float)atof(token);
               vIndex++;
               
               // Get the y and save it.
               tempLex.GetNextToken(token);
               verts[vIndex] = (float)atof(token);
               vIndex++;
               
               // Get the z and save it.
               tempLex.GetNextToken(token);
               verts[vIndex] = (float)atof(token);
               vIndex++;
            }
         // Else If vn then we get the normal x, y, z.
         else if(strcmp(token, "vn") == 0)
            {
               // Get the x and save it.
               tempLex.GetNextToken(token);
               norms[nIndex] = (float)atof(token);
               nIndex++;
               
               // Get the y and save it.
               tempLex.GetNextToken(token);
               norms[nIndex] = (float)atof(token);
               nIndex++;
               
               // Get the z and save it.
               tempLex.GetNextToken(token);
               norms[nIndex] = (float)atof(token);
               nIndex++;
            }
         // Else If vt then we get the tex coord u, v.
         else if(strcmp(token, "vt") == 0)
            {
               // Get the u and save it.
               tempLex.GetNextToken(token);
               texC[tIndex] = (float)atof(token);
               tIndex++;
               
               // Get the v and save it.
               tempLex.GetNextToken(token);
               texC[tIndex] = (float)atof(token);
               tIndex++;
            }
         // Else If f then get each vertex 3 indices set.
         else if(strcmp(token, "f") == 0)
            {
               // Load for each vertex (3 in a triangle).
               for(int i = 0; i < 3; i++)
                  {
                     // Get first set.  Get the length of it.
                     tempLex.GetNextToken(token);
                     int len = strlen(token);

                     // Since there are no spaces between a set (1/1/1)
                     // we can't simply read tokens so we must loop
                     // through and take out each value before the / sign.
                     for(int s = 0; s < len + 1; s++)
                        {
                           char buff[64];

                           // If this is not a / or if not at the end.
                           if(token[s] != '/' && s < len)
                              {
                                 buff[index] = token[s];
                                 index++;
                              }
                           else
                              {
                                 // Else end the string, convert it, save it.
                                 buff[index] = '\0';
                                 faces[fIndex] = (int)atoi(buff);
                                 fIndex++;
                                 index = 0;
                              }
                        }
                  }
            }

         token[0] = '\0';
      }

   // No longer need.
   lexer.Shutdown();

   // Create the model object by allocating.
   stObjModel *model = new stObjModel;
   if(!model) return NULL;
   memset(model, 0, sizeof(stObjModel));

   // Save face count.
   model->numFaces = totalFaces;

   // Reset temp counters.
   vIndex = 0, nIndex = 0, tIndex = 0, fIndex = 0, index = 0;

   // Allocate data for each part of the model.
   model->vertices = new float[totalFaces * 3 * 3];
   if(totalNormals) model->normals = new float[totalFaces * 3 * 3];
   if(totalTexC) model->texCoords = new float[totalFaces * 3 * 2];

   // Loop through and fill in our model.
   for(int f = 0; f < totalFaces * 9; f+=3)
      {
         // Get vertex. We subtract by 1 since we need our indices to be
         // in the range of 0 to max - 1 not 1 to max.  We multiply by 3
         // since there are 3 componets (x, y, z) in a vertex.
         model->vertices[vIndex + 0] = verts[(faces[f + 0] - 1) * 3 + 0];
         model->vertices[vIndex + 1] = verts[(faces[f + 0] - 1) * 3 + 1];
         model->vertices[vIndex + 2] = verts[(faces[f + 0] - 1) * 3 + 2];
         vIndex += 3;

         // We do the same with the texture coordinate data. Since tex coord
         // data in the second thing we app 1 to f (v/vt/vn).
         if(model->texCoords)
            {
               model->texCoords[tIndex + 0] = texC[(faces[f + 1] - 1) * 2 + 0];
               model->texCoords[tIndex + 1] = texC[(faces[f + 1] - 1) * 2 + 1];
               tIndex += 2;
            }

         // We do the same with the normal coordinate data.
         if(model->normals)
            {
               model->normals[nIndex + 0] = norms[(faces[f + 2] - 1) * 3 + 0];
               model->normals[nIndex + 1] = norms[(faces[f + 2] - 1) * 3 + 1];
               model->normals[nIndex + 2] = norms[(faces[f + 2] - 1) * 3 + 2];
               nIndex += 3;
            }
      }

   // Delete temp data.
   delete[] verts;
   delete[] norms;
   delete[] texC;
   delete[] faces;

   return model;
}


void FreeOBJModel(stObjModel *model)
{
   if(!model) return;

   // Release all resources.
   if(model->vertices) delete[] model->vertices;
   model->vertices = NULL;
   if(model->normals) delete[] model->normals;
   model->normals = NULL;
   if(model->texCoords) delete[] model->texCoords;
   model->texCoords = NULL;

   delete model;
   model = NULL;
}