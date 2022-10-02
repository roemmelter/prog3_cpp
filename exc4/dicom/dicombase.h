// (c) by Stefan Roettger, licensed under MIT license

#ifndef DICOMBASE_H
#define DICOMBASE_H

#include <vector>
#include <string>

#include "defs.h"

#ifdef HAVE_DCMTK
#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dctk.h>
#endif

class DicomVolume
   {
   class ImageDesc
      {
      public:

      ImageDesc()
#ifdef HAVE_DCMTK
         : m_Image(0)
#endif
         {}

      virtual ~ImageDesc();

#ifdef HAVE_DCMTK
      DcmFileFormat *m_Image;
#endif
      float m_pos;

      private:

      ImageDesc(const ImageDesc&);
      ImageDesc& operator=(const ImageDesc&);
      };

   public:

   DicomVolume();
   virtual ~DicomVolume();

   bool loadImages(const char *filenamepattern,
                   void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL);

   bool loadImages(const std::vector<std::string> list,
                   void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL);

   unsigned char *getVoxelData() {return((unsigned char *)m_Voxels);}

   long long getVoxelNum() {return(getCols()*getRows()*getSlis());}
   long long getByteCount() {return(sizeof(unsigned short)*getCols()*getRows()*getSlis());}

   long long getCols() {return m_Cols;}
   long long getRows() {return m_Rows;}
   long long getSlis() {return m_Images.size();}

   float getBound(int c) {return(m_Bounds[c]);}

   private:

   bool dicomLoad(const char *filenamepattern,
                  void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL);

   bool dicomLoad(const std::vector<std::string> list,
                  void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL);

   bool dicomProcess();

   void deleteImages();
   void sortImages();

   std::vector<ImageDesc*> m_Images;

   long long m_Cols;
   long long m_Rows;

   float m_PixSpaceRow; // millimeters
   float m_PixSpaceCol; // millimeters
   float m_PixSpaceImg; // millimeters

   float m_Bounds[3]; // meters
   float m_VolDir[3];

   unsigned long m_SmallestPixVal;
   unsigned long m_LargestPixVal;

   unsigned short *m_Voxels;

   static int compareFunc(const void *elem1,const void *elem2);

   static bool check_intel();
   };

// read a DICOM series identified by the * in the filename pattern
unsigned char *readDICOMvolume(const char *filename,
                               long long *width,long long *height,long long *depth,unsigned int *components=NULL,
                               float *scalex=NULL,float *scaley=NULL,float *scalez=NULL,
                               void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL);

// read a DICOM series from a file name list
unsigned char *readDICOMvolume(const std::vector<std::string> list,
                               long long *width,long long *height,long long *depth,unsigned int *components=NULL,
                               float *scalex=NULL,float *scaley=NULL,float *scalez=NULL,
                               void (*feedback)(const char *info,float percent,void *obj)=NULL,void *obj=NULL);

#endif
