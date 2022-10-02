// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL io convenience functions
#ifndef GLVERTEX_IO_H
#define GLVERTEX_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inline unsigned char *lglReadFiled(FILE *file, size_t *bytes, const size_t blocksize=1<<20)
{
   unsigned char *data, *data2;
   size_t cnt, blkcnt;

   data = NULL;
   *bytes = cnt = 0;

   do
   {
      if (data == NULL)
      {
         if ((data=(unsigned char *)malloc(blocksize)) == NULL) return(NULL);
      }
      else
      {
         if ((data2=(unsigned char *)realloc(data, cnt+blocksize)) == NULL) {free(data); return(NULL);}
         data = data2;
      }
      blkcnt = fread(&data[cnt], 1, blocksize, file);
      cnt += blkcnt;
   }
   while (blkcnt == blocksize);

   if (cnt == 0)
   {
      free(data);
      return(NULL);
   }

   if ((data2=(unsigned char *)realloc(data, cnt)) == NULL) {free(data); return(NULL);}
   else data = data2;

   *bytes = cnt;

   return(data);
}

inline unsigned char *lglReadFile(const char *filename, size_t *bytes)
{
   FILE *file;
   unsigned char *data;

   if ((file=fopen(filename, "rb"))==NULL) return(NULL);
   data = lglReadFiled(file, bytes);
   fclose(file);

   return(data);
}

inline std::string lglReadTextFile(const char *filename)
{
   std::string text;

   unsigned char *data;
   size_t bytes;

   data = lglReadFile(filename, &bytes);

   if (data)
   {
      text = std::string((char *)data, bytes);
      free(data);
   }

   return(text);
}

inline std::string lglReadTextFile(const std::string &filename)
{
   return(lglReadTextFile(filename.c_str()));
}

inline bool lglWriteFile(const char *filename, unsigned char *chunk, size_t bytes)
{
   FILE *file;
   size_t count;

   if ((file=fopen(filename, "wb"))==NULL) return(false);
   count = fwrite((void *)chunk, 1, bytes, file);
   fclose(file);

   return(count == bytes);
}

inline bool lglWriteTextFile(const char *filename, const std::string &text)
{
   return(lglWriteFile(filename, (unsigned char *)text.c_str(), text.size()));
}

inline bool lglWriteTextFile(const std::string &filename, const std::string &text)
{
   return(lglWriteTextFile(filename.c_str(), text));
}

#endif
