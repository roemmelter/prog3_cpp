// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL PNM format loader
#ifndef GLVERTEX_PNMFORMAT_H
#define GLVERTEX_PNMFORMAT_H

#include "string.h"
#include "glvertex_io.h"

// read a PNM image
inline unsigned char *lglReadPnmImage(const char *filename,
                                      int *width, int *height,
                                      int *components)
{
   const int maxstr = 100;

   char str[maxstr];

   unsigned char *data, *ptr1, *ptr2;
   size_t bytes;

   int pnmtype, maxval;
   unsigned char *image;

   data = lglReadFile(filename, &bytes);

   if (data == NULL) return(NULL);
   if (bytes < 4) return(NULL);

   memcpy(str, data, 3);
   str[3] = '\0';

   if (sscanf(str, "P%1d\n", &pnmtype) != 1) return(NULL);

   ptr1 = data+3;
   while (*ptr1=='\n' || *ptr1=='#')
   {
      while (*ptr1 == '\n')
         if (++ptr1 >= data+bytes) {free(data); return(NULL);}
      while (*ptr1 == '#')
         if (++ptr1 >= data+bytes) {free(data); return(NULL);}
         else
            while (*ptr1 != '\n')
               if (++ptr1 >= data+bytes) {free(data); return(NULL);}
   }

   ptr2 = ptr1;
   while (*ptr2!='\n' && *ptr2!=' ')
      if (++ptr2 >= data+bytes) {free(data); return(NULL);}
   if (++ptr2 >= data+bytes) {free(data); return(NULL);}
   while (*ptr2!='\n' && *ptr2!=' ')
      if (++ptr2 >= data+bytes) {free(data); return(NULL);}
   if (++ptr2 >= data+bytes) {free(data); return(NULL);}
   while (*ptr2!='\n' && *ptr2!=' ')
      if (++ptr2 >= data+bytes) {free(data); return(NULL);}
   if (++ptr2 >= data+bytes) {free(data); return(NULL);}

   if (ptr2-ptr1 >= maxstr) {free(data); return(NULL);}
   memcpy(str, ptr1, ptr2-ptr1);
   str[ptr2-ptr1] = '\0';

   if (sscanf(str, "%d %d\n%d\n", width, height, &maxval) != 3) {free(data); return(NULL);}

   if (*width<1 || *height<1) {free(data); return(NULL);}

   if (pnmtype==5 && maxval==255) *components = 1;
   else if (pnmtype==5 && maxval==65535) *components = 2;
   else if (pnmtype==6 && maxval==255) *components = 3;
   else {free(data); return(NULL);}

   if (data+bytes != ptr2+(*width)*(*height)*(*components)) {free(data); return(NULL);}
   if ((image=(unsigned char *)malloc((*width)*(*height)*(*components))) == NULL) {free(data); return(NULL);}

   memcpy(image, ptr2, (*width)*(*height)*(*components));
   free(data);

   return(image);
}

inline unsigned char *lglReadPnmImage(std::string filename,
                                      int *width, int *height,
                                      int *components)
{
   return(lglReadPnmImage(filename.c_str(),
                          width, height,
                          components));
}

// write a PNM image
inline bool lglWritePnmImage(const char *filename,
                             unsigned char *image,
                             int width, int height,
                             int components)
{
   FILE *file;

   if (width<1 || height<1) return(false);
   if (components<1 || components>4) return(false);

   if ((file=fopen(filename, "wb")) == NULL) return(false);

   if (components==1 || components==2) fprintf(file, "P5");
   else if (components==3) fprintf(file, "P6");
   else if (components==4) fprintf(file, "P8");

   fprintf(file, "\n%d %d\n", width, height);

   if (components==1 || components==3 || components==4) fprintf(file, "255\n");
   else fprintf(file, "65535\n");

   if (fwrite(image, width*height*components, 1, file) != 1) {fclose(file); return(false);}
   fclose(file);

   return(true);
}

inline bool lglWritePnmImage(std::string filename,
                             unsigned char *image,
                             int width, int height,
                             int components)
{
   return(lglWritePnmImage(filename.c_str(),
                           image,
                           width, height,
                           components));
}

#endif
