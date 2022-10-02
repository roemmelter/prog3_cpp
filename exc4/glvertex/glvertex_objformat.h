// (c) by Stefan Roettger / Matthias Hopf, licensed under MIT license

//! \file
//! LGL OBJ format loader
#ifndef GLVERTEX_OBJFORMAT_H
#define GLVERTEX_OBJFORMAT_H

#include "glvertex_core.h"

//! load OBJ into vbo from file descriptor
inline bool lglLoadObjInto(lglVBO *vbo,
                           FILE *file,
                           lgl_texgenmode_enum texgen = LGL_TEXGEN_NONE,
                           const char *name = "obj file")
{
   bool hasv, hasvc, hasvn, hasvt;

   std::vector<vec3> vertices;
   std::vector<vec3f> colors;
   std::vector<vec3f> normals;
   std::vector<vec2f> uvs;

   std::vector<int> vertexIndices, normalIndices, uvIndices;

   if (file == NULL)
   {
      lglError("improper obj file handle");
      return(false);
   }

   hasv = hasvc = hasvn = hasvt = false;

   while (true)
   {
      char lineHeader[128];
      lineHeader[127] = 0;

      // read the first word of the line
      int result = fscanf(file, "%127s", lineHeader);

      // check for end-of-file
      if (result == EOF) break;

      // check first word of the line
      if (strcmp(lineHeader, "v") == 0)
      {
         vec3 vertex;
         vec3f color;
         long int rewind = ftell(file);
         int result = fscanf(file, "%lf %lf %lf %f %f %f\n", &vertex.x, &vertex.y, &vertex.z, &color.r, &color.g, &color.b);
         if (result != 6)
         {
            fseek(file, rewind, SEEK_SET);
            int result = fscanf(file, "%lf %lf %lf\n", &vertex.x, &vertex.y, &vertex.z);
            if (result != 3) lglError("invalid vertex specification");
            else vertices.push_back(vertex);
            colors.push_back(vec3f(1,1,1));
         }
         else
         {
            vertices.push_back(vertex);
            colors.push_back(color);
            hasvc = true;
         }
         hasv = true;
      }
      else if (strcmp(lineHeader, "vn") == 0)
      {
         vec3f normal;
         int result = fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
         if (result != 3) lglError("invalid normal specification");
         else normals.push_back(normal);
         hasvn = true;
      }
      else if (strcmp(lineHeader, "vt") == 0)
      {
         vec2f uv;
         int result = fscanf(file, "%f %f\n", &uv.x, &uv.y);
         if (result != 2) lglError("invalid uv specification");
         else uvs.push_back(uv);
         hasvt = true;
      }
      else if (strcmp(lineHeader, "f") == 0)
      {
         int vertexIndex[3], normalIndex[3], uvIndex[3];

         if (hasv)
         {
            long int rewind = ftell(file);

            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                                 &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                                 &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

            if (matches != 9)
            {
               fseek(file, rewind, SEEK_SET);

               int matches = fscanf(file, "%d//%d %d//%d %d//%d\n",
                                    &vertexIndex[0], &normalIndex[0],
                                    &vertexIndex[1], &normalIndex[1],
                                    &vertexIndex[2], &normalIndex[2]);

               uvIndex[0] = uvIndex[1] = uvIndex[2] = -1;

               if (matches != 6)
               {
                  fseek(file, rewind, SEEK_SET);

                  int matches = fscanf(file, "%d/%d %d/%d %d/%d\n",
                                       &vertexIndex[0], &uvIndex[0],
                                       &vertexIndex[1], &uvIndex[1],
                                       &vertexIndex[2], &uvIndex[2]);

                  normalIndex[0] = normalIndex[1] = normalIndex[2] = -1;

                  if (matches != 6)
                  {
                     fseek(file, rewind, SEEK_SET);

                     int matches = fscanf(file, "%d %d %d\n",
                                          &vertexIndex[0],
                                          &vertexIndex[1],
                                          &vertexIndex[2]);

                     if (hasvn && normals.size() >= vertices.size())
                     {
                        normalIndex[0] = vertexIndex[0];
                        normalIndex[1] = vertexIndex[1];
                        normalIndex[2] = vertexIndex[2];
                     }
                     else
                        normalIndex[0] = normalIndex[1] = normalIndex[2] = -1;

                     if (hasvt && uvs.size() >= vertices.size())
                     {
                        uvIndex[0] = vertexIndex[0];
                        uvIndex[1] = vertexIndex[1];
                        uvIndex[2] = vertexIndex[2];
                     }
                     else
                        uvIndex[0] = uvIndex[1] = uvIndex[2] = -1;

                     if (matches != 3)
                     {
                        lglError("invalid index specification");
                        return(false);
                     }
                  }
               }
            }

            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);

            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);

            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
         }
         else
         {
            lglError("invalid face specification");
            return(false);
         }
      }
      else if (strcmp(lineHeader, "#") == 0)
      {
         while (!feof(file))
            if (fgetc(file) == '\n') break;
      }
      else if (strcmp(lineHeader, "o") == 0 || // skip object names
               strcmp(lineHeader, "s") == 0 || // skip object smoothing
               strcmp(lineHeader, "g") == 0 || // skip group names
               strcmp(lineHeader, "l") == 0 || // skip indexed lines
               strcmp(lineHeader, "mtllib") == 0 || // skip material libraries
               strcmp(lineHeader, "usemtl") == 0) // skip materials
      {
         while (!feof(file))
            if (fgetc(file) == '\n') break;
      }
      else if (strlen(lineHeader) == 0) // skip empty lines
      {
         while (!feof(file))
            if (fgetc(file) == '\n') break;
      }
      else
      {
         lglError("invalid command specification");
         return(false);
      }
   }

   // check for vertices
   if (!hasv)
   {
      lglError("invalid vertex count");
      return(false);
   }

   // check vertex colors
   if (hasvc)
      if (colors.size() != vertices.size())
      {
         lglError("invalid color count");
         return(false);
      }

   // check vertex indices
   for (unsigned int i=0; i<vertexIndices.size(); i++)
   {
      if (vertexIndices.at(i) <= 0 ||
          vertexIndices.at(i) > (int)vertices.size())
      {
         lglError("invalid vertex index");
         return(false);
      }
   }

   // check normal indices
   for (unsigned int i=0; i<normalIndices.size(); i++)
   {
      if (normalIndices.at(i) == 0 ||
          normalIndices.at(i) > (int)normals.size())
      {
         lglError("invalid normal index");
         return(false);
      }
   }

   // check uv indices
   for (unsigned int i=0; i<uvIndices.size(); i++)
   {
      if (uvIndices.at(i) == 0 ||
          uvIndices.at(i) > (int)uvs.size())
      {
         lglError("invalid uv index");
         return(false);
      }
   }

   // setup vbo
   if (!hasvt && texgen != LGL_TEXGEN_NONE) vbo->lglTexCoordGen(texgen);

   // start vbo
   vbo->lglBegin(LGL_TRIANGLES);

   // for each vertex of each triangle
   for (unsigned int i=0; i<vertexIndices.size(); i++)
   {
      // define vertex color attribute
      if (hasvc)
         vbo->lglColor(colors.at(vertexIndices.at(i)-1));

      // define vertex normal attribute
      if (hasvn)
      {
         int index = normalIndices.at(i);
         if (index < 0) vbo->lglNormal(0,0,0);
         else vbo->lglNormal(normals.at(index-1));
      }

      // define vertex texture coordinate attribute
      if (hasvt)
      {
         int index = uvIndices.at(i);
         if (index < 0) vbo->lglTexCoord(0,0);
         else vbo->lglTexCoord(uvs.at(index-1));
      }

      // put the vertex and its attributes into vbo
      vbo->lglVertex(vertices.at(vertexIndices.at(i)-1));
   }

   // finish vbo
   vbo->lglEnd();

   // set vbo name
   vbo->setName(name);

   return(true);
}

//! load OBJ into vbo from file path
inline bool lglLoadObjInto(lglVBO *vbo, const char *path, lgl_texgenmode_enum texgen = LGL_TEXGEN_NONE)
{
   FILE *file = fopen(path, "rb");

   if (file)
   {
      bool ok = lglLoadObjInto(vbo, file, texgen, path);
      fclose(file);
      return(ok);
   }

   return(false);
}

//! load OBJ into vbo from file path (and from default installation paths)
inline bool lglLoadObj(lglVBO *vbo, const char *path, bool silent = false, lgl_texgenmode_enum texgen = LGL_TEXGEN_NONE)
{
   bool ok = lglLoadObjInto(vbo, path, texgen);

   if (!ok) ok = lglLoadObjInto(vbo, (std::string("../") + path).c_str(), texgen);
   if (!ok) ok = lglLoadObjInto(vbo, (std::string("obj/") + path).c_str(), texgen);
   if (!ok) ok = lglLoadObjInto(vbo, (std::string("../obj/") + path).c_str(), texgen);
   if (!ok) ok = lglLoadObjInto(vbo, (std::string("data/") + path).c_str(), texgen);
   if (!ok) ok = lglLoadObjInto(vbo, (std::string("../data/") + path).c_str(), texgen);
#ifndef _WIN32
   if (!ok) ok = lglLoadObjInto(vbo, (std::string("/usr/local/share/") + path).c_str(), texgen);
#else
   if (!ok) ok = lglLoadObjInto(vbo, (std::string("C:\\glvertex\\") + path).c_str(), texgen);
#endif

   if (!ok)
      if (!silent) lglError("cannot read obj file");

   return(ok);
}

//! load OBJ from file path
inline lglVBO *lglLoadObj(const char *path, bool silent = false, lgl_texgenmode_enum texgen = LGL_TEXGEN_MIXED_HEDGEHOG)
{
   // create vbo
   lglVBO *vbo = new lglVBO;

   // load vbo
   if (!lglLoadObj(vbo, path, silent, texgen))
   {
      delete vbo;
      return(NULL);
   }

   return(vbo);
}

//! load OBJ from file path
inline lglVBO *lglLoadObj(std::string path, bool silent = false, lgl_texgenmode_enum texgen = LGL_TEXGEN_MIXED_HEDGEHOG)
{
   return(lglLoadObj(path.c_str(), silent, texgen));
}

//! load OBJ into vbo from memory chunk
inline bool lglLoadObjData(lglVBO *vbo,
                           const char *data,
                           lgl_texgenmode_enum texgen = LGL_TEXGEN_NONE,
                           const char *name = "obj data")
{
   bool hasv, hasvc, hasvn, hasvt;

   std::vector<vec3> vertices;
   std::vector<vec3f> colors;
   std::vector<vec3f> normals;
   std::vector<vec2f> uvs;

   std::vector<int> vertexIndices, normalIndices, uvIndices;

   int n;

   if (data == NULL)
   {
      lglError("improper obj data");
      return(false);
   }

   hasv = hasvc = hasvn = hasvt = false;

   while (true)
   {
      char lineHeader[128];
      lineHeader[127] = 0;

      // read the first word of the line
      int result = sscanf(data, "%127s%n", lineHeader, &n);
      data += n;

      // check for end-of-file
      if (result == EOF) break;

      // check first word of the line
      if (strcmp(lineHeader, "v") == 0)
      {
         vec3 vertex;
         vec3f color;
         int result = sscanf(data, "%lf %lf %lf %f %f %f%n\n", &vertex.x, &vertex.y, &vertex.z, &color.r, &color.g, &color.b, &n);
         if (result != 6)
         {
            int result = sscanf(data, "%lf %lf %lf%n\n", &vertex.x, &vertex.y, &vertex.z, &n);
            if (result != 3) lglError("invalid vertex specification");
            else vertices.push_back(vertex);
            colors.push_back(vec3f(1,1,1));
         }
         else
         {
            vertices.push_back(vertex);
            colors.push_back(color);
            hasvc = true;
         }
         hasv = true;
         data += n;
      }
      else if (strcmp(lineHeader, "vn") == 0)
      {
         vec3f normal;
         int result = sscanf(data, "%f %f %f%n\n", &normal.x, &normal.y, &normal.z, &n);
         if (result != 3) lglError("invalid normal specification");
         else normals.push_back(normal);
         hasvn = true;
         data += n;
      }
      else if (strcmp(lineHeader, "vt") == 0)
      {
         vec2f uv;
         int result = sscanf(data, "%f %f%n\n", &uv.x, &uv.y, &n);
         if (result != 2) lglError("invalid uv specification");
         else uvs.push_back(uv);
         hasvt = true;
         data += n;
      }
      else if (strcmp(lineHeader, "f") == 0)
      {
         int vertexIndex[3], normalIndex[3], uvIndex[3];

         if (hasv)
         {
            int matches = sscanf(data, "%d/%d/%d %d/%d/%d %d/%d/%d%n\n",
                                 &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                                 &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                                 &vertexIndex[2], &uvIndex[2], &normalIndex[2], &n);

            if (matches != 9)
            {
               int matches = sscanf(data, "%d//%d %d//%d %d//%d%n\n",
                                    &vertexIndex[0], &normalIndex[0],
                                    &vertexIndex[1], &normalIndex[1],
                                    &vertexIndex[2], &normalIndex[2], &n);

               uvIndex[0] = uvIndex[1] = uvIndex[2] = -1;

               if (matches != 6)
               {
                  int matches = sscanf(data, "%d/%d %d/%d %d/%d%n\n",
                                       &vertexIndex[0], &uvIndex[0],
                                       &vertexIndex[1], &uvIndex[1],
                                       &vertexIndex[2], &uvIndex[2], &n);

                  normalIndex[0] = normalIndex[1] = normalIndex[2] = -1;

                  if (matches != 6)
                  {
                     int matches = sscanf(data, "%d %d %d%n\n",
                                          &vertexIndex[0],
                                          &vertexIndex[1],
                                          &vertexIndex[2], &n);

                     if (hasvn && normals.size() >= vertices.size())
                     {
                        normalIndex[0] = vertexIndex[0];
                        normalIndex[1] = vertexIndex[1];
                        normalIndex[2] = vertexIndex[2];
                     }
                     else
                        normalIndex[0] = normalIndex[1] = normalIndex[2] = -1;

                     if (hasvt && uvs.size() >= vertices.size())
                     {
                        uvIndex[0] = vertexIndex[0];
                        uvIndex[1] = vertexIndex[1];
                        uvIndex[2] = vertexIndex[2];
                     }
                     else
                        uvIndex[0] = uvIndex[1] = uvIndex[2] = -1;

                     if (matches != 3)
                     {
                        lglError("invalid index specification");
                        return(false);
                     }
                  }
               }
            }

            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);

            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);

            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);

            data += n;
         }
         else
         {
            lglError("invalid face specification");
            return(false);
         }
      }
      else if (strcmp(lineHeader, "#") == 0)
      {
         while (*data!='\0')
            if (*data++ == '\n') break;
      }
      else if (strcmp(lineHeader, "o") == 0 || // skip object names
               strcmp(lineHeader, "s") == 0 || // skip object smoothing
               strcmp(lineHeader, "g") == 0 || // skip group names
               strcmp(lineHeader, "l") == 0 || // skip indexed lines
               strcmp(lineHeader, "mtllib") == 0 || // skip material libraries
               strcmp(lineHeader, "usemtl") == 0) // skip materials
      {
         while (*data!='\0')
            if (*data++ == '\n') break;
      }
      else if (strlen(lineHeader) == 0) // skip empty lines
      {
         while (*data!='\0')
            if (*data++ == '\n') break;
      }
      else
      {
         lglError("invalid command specification");
         return(false);
      }
   }

   // check for vertices
   if (!hasv)
   {
      lglError("invalid vertex count");
      return(false);
   }

   // check vertex colors
   if (hasvc)
      if (colors.size() != vertices.size())
      {
         lglError("invalid color count");
         return(false);
      }

   // check vertex indices
   for (unsigned int i=0; i<vertexIndices.size(); i++)
   {
      if (vertexIndices.at(i) <= 0 ||
          vertexIndices.at(i) > (int)vertices.size())
      {
         lglError("invalid vertex index");
         return(false);
      }
   }

   // check normal indices
   for (unsigned int i=0; i<normalIndices.size(); i++)
   {
      if (normalIndices.at(i) == 0 ||
          normalIndices.at(i) > (int)normals.size())
      {
         lglError("invalid normal index");
         return(false);
      }
   }

   // check uv indices
   for (unsigned int i=0; i<uvIndices.size(); i++)
   {
      if (uvIndices.at(i) == 0 ||
          uvIndices.at(i) > (int)uvs.size())
      {
         lglError("invalid uv index");
         return(false);
      }
   }

   // setup vbo
   if (!hasvt && texgen != LGL_TEXGEN_NONE) vbo->lglTexCoordGen(texgen);

   // start vbo
   vbo->lglBegin(LGL_TRIANGLES);

   // for each vertex of each triangle
   for (unsigned int i=0; i<vertexIndices.size(); i++)
   {
      // define vertex color attribute
      if (hasvc)
         vbo->lglColor(colors.at(vertexIndices.at(i)-1));

      // define vertex normal attribute
      if (hasvn)
      {
         int index = normalIndices.at(i);
         if (index < 0) vbo->lglNormal(0,0,0);
         else vbo->lglNormal(normals.at(index-1));
      }

      // define vertex texture coordinate attribute
      if (hasvt)
      {
         int index = uvIndices.at(i);
         if (index < 0) vbo->lglTexCoord(0,0);
         else vbo->lglTexCoord(uvs.at(index-1));
      }

      // put the vertex and its attributes into vbo
      vbo->lglVertex(vertices.at(vertexIndices.at(i)-1));
   }

   // finish vbo
   vbo->lglEnd();

   // set vbo name
   vbo->setName(name);

   return(true);
}

//! load OBJ from memory chunk
inline lglVBO *lglLoadObjData(const char *data,
                              lgl_texgenmode_enum texgen = LGL_TEXGEN_NONE,
                              const char *name = "obj data")
{
   // create vbo
   lglVBO *vbo = new lglVBO;

   // load vbo
   if (!lglLoadObjData(vbo, data, texgen, name))
   {
      delete vbo;
      return(NULL);
   }

   return(vbo);
}

//! save vbo to file descriptor
inline bool lglSaveObjInto(lglVBO *vbo, FILE *file, int &index, int &nindex, int &tindex)
{
   if (file == NULL)
   {
      lglError("improper obj file handle");
      return(false);
   }

   if (vbo->lglGetVertexMode() != LGL_TRIANGLES &&
       vbo->lglGetVertexMode() != LGL_QUADS &&
       vbo->lglGetVertexMode() != LGL_TRIANGLE_STRIP &&
       vbo->lglGetVertexMode() != LGL_QUAD_STRIP)
   {
      lglError("unsupported primitive type");
      return(false);
   }

   std::vector<vec4> vertices = vbo->lglGetVertexCoordinates();
   std::vector<vec4f> colors = vbo->lglGetColorAttributes();
   std::vector<vec3f> normals = vbo->lglGetNormalAttributes();
   std::vector<vec4f> texcoords = vbo->lglGetTexCoordAttributes();

   for (unsigned int i=0; i<vertices.size(); i++)
      if (!vbo->lglActiveColoring())
         fprintf(file, "v %f %f %f\n", vertices[i].x, vertices[i].y, vertices[i].z);
      else
         fprintf(file, "v %f %f %f %f %f %f\n", vertices[i].x, vertices[i].y, vertices[i].z, colors[i].r, colors[i].g, colors[i].b);

   for (unsigned int i=0; i<normals.size(); i++)
      if (vbo->lglActiveLighting())
         fprintf(file, "vn %f %f %f\n", normals[i].x, normals[i].y, normals[i].z);

   for (unsigned int i=0; i<texcoords.size(); i++)
      if (vbo->lglActiveTexturing())
         fprintf(file, "vt %f %f\n", texcoords[i].x, texcoords[i].y);

   if (vbo->lglGetVertexMode() == LGL_TRIANGLES)
   {
      for (unsigned int i=2; i<vertices.size(); i+=3)
      {
         int j = i+index;
         int k = i+tindex;
         int l = i+nindex;

         if (vbo->lglActiveLighting() && vbo->lglActiveTexturing())
            fprintf(file, "f %u/%u/%u %u/%u/%u %u/%u/%u\n", j-2,k-2,l-2, j-1,k-1,l-1, j,k,l);
         else if (vbo->lglActiveTexturing())
            fprintf(file, "f %u/%u %u/%u %u/%u\n", j-2,k-2, j-1,k-1, j,k);
         else if (vbo->lglActiveLighting())
            fprintf(file, "f %u//%u %u//%u %u//%u\n", j-2,l-2, j-1,l-1, j,l);
         else
            fprintf(file, "f %u %u %u\n", j-2, j-1, j);
      }
   }
   else if (vbo->lglGetVertexMode() == LGL_QUADS)
   {
      for (unsigned int i=3; i<vertices.size(); i+=4)
      {
         int j = i+index;
         int k = i+tindex;
         int l = i+nindex;

         if (vbo->lglActiveLighting() && vbo->lglActiveTexturing())
            fprintf(file, "f %u/%u/%u %u/%u/%u %u/%u/%u\n", j-3,k-3,l-3, j-2,k-2,l-2, j-1,k-1,l-1);
         else if (vbo->lglActiveTexturing())
            fprintf(file, "f %u/%u %u/%u %u/%u\n", j-3,k-3, j-2,k-2, j-1,k-1);
         else if (vbo->lglActiveLighting())
            fprintf(file, "f %u//%u %u//%u %u//%u\n", j-3,l-3, j-2,l-2, j-1,l-1);
         else
            fprintf(file, "f %u %u %u\n", j-3, j-2, j-1);

         if (vbo->lglActiveLighting() && vbo->lglActiveTexturing())
            fprintf(file, "f %u/%u/%u %u/%u/%u %u/%u/%u\n", j,k,l, j-2,k-2,l-2, j-1,k-1,l-1);
         else if (vbo->lglActiveTexturing())
            fprintf(file, "f %u/%u %u/%u %u/%u\n", j,k, j-2,k-2, j-1,k-1);
         else if (vbo->lglActiveLighting())
            fprintf(file, "f %u//%u %u//%u %u//%u\n", j,l, j-2,l-2, j-1,l-1);
         else
            fprintf(file, "f %u %u %u\n", j, j-2, j-1);
      }
   }
   else if (vbo->lglGetVertexMode() == LGL_TRIANGLE_STRIP ||
            vbo->lglGetVertexMode() == LGL_QUAD_STRIP)
   {
      vec4 v1(0), v2(0), v3(0);

      for (unsigned int i=0; i<vertices.size(); i++)
      {
         v1 = v2;
         v2 = v3;
         v3 = vertices[i];

         if (i > 1)
            if (v1!=v2 && v2!=v3 && v3!=v1)
            {
               int j = i+index;
               int k = i+tindex;
               int l = i+nindex;

               if (i%2 == 0)
               {
                  if (vbo->lglActiveLighting() && vbo->lglActiveTexturing())
                     fprintf(file, "f %u/%u/%u %u/%u/%u %u/%u/%u\n", j-2,k-2,l-2, j-1,k-1,l-1, j,k,l);
                  else if (vbo->lglActiveTexturing())
                     fprintf(file, "f %u/%u %u/%u %u/%u\n", j-2,k-2, j-1,k-1, j,k);
                  else if (vbo->lglActiveLighting())
                     fprintf(file, "f %u//%u %u//%u %u//%u\n", j-2,l-2, j-1,l-1, j,l);
                  else
                     fprintf(file, "f %u %u %u\n", j-2, j-1, j);
               }
               else
               {
                  if (vbo->lglActiveLighting() && vbo->lglActiveTexturing())
                     fprintf(file, "f %u/%u/%u %u/%u/%u %u/%u/%u\n", j,k,l, j-1,k-1,l-1, j-2,k-2,l-2);
                  else if (vbo->lglActiveTexturing())
                     fprintf(file, "f %u/%u %u/%u %u/%u\n", j,k, j-1,k-1, j-2,k-2);
                  else if (vbo->lglActiveLighting())
                     fprintf(file, "f %u//%u %u//%u %u//%u\n", j,l, j-1,l-1, j-2,l-2);
                  else
                     fprintf(file, "f %u %u %u\n", j, j-1, j-2);
               }
            }
      }
   }

   index += vertices.size();
   if (vbo->lglActiveLighting()) nindex += normals.size();
   if (vbo->lglActiveTexturing()) tindex += texcoords.size();

   return(true);
}

//! save vbo into OBJ file
inline bool lglSaveObj(lglVBO *vbo, const char *path, bool silent = false)
{
   bool ok = false;

   FILE *file = fopen(path, "wb");

   if (file)
   {
      fprintf(file, "# glVertex OBJ File\n\n");

      int index = 1;
      int nindex = 1;
      int tindex = 1;

      ok = lglSaveObjInto(vbo, file, index, nindex, tindex);

      fclose(file);
   }

   if (!ok)
      if (!silent) lglError("cannot write obj file");

   return(ok);
}

//! save vbo into OBJ file
inline bool lglSaveObj(lglVBO *vbo, std::string path, bool silent = false)
{
   return(lglSaveObj(vbo, path.c_str(), silent));
}

#endif
