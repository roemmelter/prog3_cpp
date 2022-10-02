// (c) by Stefan Roettger, licensed under MIT license

#ifndef VOLUME_H
#define VOLUME_H

#include "dicom/dicombase.h"
#include "pvm/ddsbase.h"

// read a DICOM series or a PVM volume
inline unsigned char *readXYZvolume(const char *filename,
                                    long long *width,long long *height,long long *depth,
                                    unsigned int *components,
                                    int *msb=NULL)
   {
   if (strchr(filename, '*'))
      {
      if (msb) *msb=0;
      return(readDICOMvolume(filename,width,height,depth,components));
      }
   else
      {
      unsigned int iwidth,iheight,idepth;
      unsigned char *data=readPVMvolume(filename,&iwidth,&iheight,&idepth,components);
      *width=iwidth;
      *height=iheight;
      *depth=idepth;
      if (msb) *msb=1;
      return(data);
      }
   }

// normalize volume data to 8 bit
inline unsigned char *normalizeVolume(unsigned char *volume,
                                      long long width,long long height,long long depth,
                                      int components,
                                      int msb=0)
   {
   if (components==2) return(quantize(volume,width,height,depth,msb));
   else if (components==1) return(volume);
   else return(NULL);
   }

#endif
