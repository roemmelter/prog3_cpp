// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL RAW format loader
#ifndef GLVERTEX_RAWFORMAT_H
#define GLVERTEX_RAWFORMAT_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#ifdef _MSC_VER
#define strdup _strdup
#define snprintf _snprintf
#endif

// read raw data
//  the raw file format is encoded into the filename
//   as in name.size_cellformat_cellsize.raw
//    e.g. name.256x256.raw (grayscale image)
//    e.g. name.256x256_3.raw (rgb image)
//    e.g. name.256x256_u2m.raw (16-bit msb unsigned grayscale image)
//    e.g. name.256x256x256.raw (8-bit volume)
//    e.g. name.256x256x256_2_100x100x50.raw (16-bit volume with voxel dimensions)
//   cell format modifiers:
//    1 = 8 bit
//    2 = 16 bit
//    3 = rgb
//    4 = rgba
//    6 = rgb 16 bit
//    8 = rgba 16 bit
//    u = unsigned
//    s = signed
//    m = msb
//    l = lsb
//   default modifiers = u1m
unsigned char *lglReadRawData(const char *filename,
                              long long *width,long long *height,long long *depth,
                              unsigned int *components=NULL,unsigned int *bits=NULL,bool *sign=NULL,bool *msb=NULL,
                              float *scalex=NULL,float *scaley=NULL,float *scalez=NULL); // meters

// analyze raw file format
bool lglReadRawInfo(char *filename,
                    long long *width,long long *height,long long *depth,
                    unsigned int *components=NULL,unsigned int *bits=NULL,bool *sign=NULL,bool *msb=NULL,
                    float *scalex=NULL,float *scaley=NULL,float *scalez=NULL); // meters

// write raw data
char *lglWriteRawData(const char *filename, // /wo suffix .raw
                      unsigned char *data,
                      long long width,long long height,long long depth=1,
                      unsigned int components=1,unsigned int bits=8,bool sign=false,bool msb=true,
                      float scalex=1.0f,float scaley=1.0f,float scalez=1.0f); // meters

// define raw file format
char *lglMakeRawInfo(long long width,long long height,long long depth,
                     unsigned int components,unsigned int bits,bool sign,bool msb,
                     float scalex,float scaley,float scalez); // meters

// convert a raw array to a 16-bit unsigned array
unsigned short int *lglConvertRaw(unsigned char *data,
                                  long long width,long long height,long long depth,
                                  unsigned int &components,
                                  unsigned int &bits,bool sign,bool msb);

// quantize 16-bit raw data to 8-bit using a linear mapping
unsigned char *lglStretchRaw(unsigned short int *data,
                             long long width,long long height,long long depth,
                             unsigned int components);

// quantize 16-bit raw data to 8-bit using a non-linear mapping
unsigned char *lglQuantizeRaw(unsigned short int *data,
                              long long width,long long height,long long depth,
                              bool linear=false);

// load and quantize raw data
unsigned char *lglLoadRawData(const char *filename,
                              long long *width,long long *height,long long *depth,
                              float *scalex=NULL,float *scaley=NULL,float *scalez=NULL); // meters

// load and quantize raw image
unsigned char *lglLoadRawImage(const char *filename,
                               int *width,int *height,
                               int *components);

// load and quantize raw image
unsigned char *lglLoadRawImage(std::string filename,
                               int *width,int *height,
                               int *components);

// write raw image
bool lglWriteRawImage(const char *filename, // /wo suffix .raw
                      unsigned char *image,
                      int width,int height,
                      int components);

// write raw image
bool lglWriteRawImage(std::string filename, // /wo suffix .raw
                      unsigned char *image,
                      int width,int height,
                      int components);

// analyze raw file format
inline bool lglReadRawInfo(char *filename,
                           long long *width,long long *height,long long *depth,
                           unsigned int *components,unsigned int *bits,bool *sign,bool *msb,
                           float *scalex,float *scaley,float *scalez)
{
   unsigned int rawcomps=1; // scalar
   bool rawsign=false; // unsigned
   unsigned int rawbits=8; // byte
   bool rawmsb=true; // most significant byte first
   int rawscalex=1000000,rawscaley=1000000,rawscalez=1000000; // 1E6um = 1m
   int rawmaxscale=0; // 0um

   char *dot,*dotdot;
   int count;

   dot=strrchr(filename,'.');

   if (dot==NULL) return(false);

   if (strcmp(dot,".raw")!=0) return(false);

   *dot='\0';
   dotdot=strrchr(filename,'.');
   *dot='.';

   if (dotdot==NULL) return(false);

   *width=*height=*depth=1;

   dotdot++;

   if (sscanf(dotdot,"%lldx%lld%n",width,height,&count)!=2) return(false);

   dotdot+=count;

   if (*dotdot=='x')
   {
      dotdot++;

      if (sscanf(dotdot,"%lld%n",depth,&count)!=1) return(false);

      dotdot+=count;
   }

   if (*dotdot=='_')
   {
      dotdot++;

      while (*dotdot!='.' && *dotdot!='_')
      {
         switch (*dotdot)
         {
            case '1': rawcomps=1; rawbits=8; break; // char
            case '2': rawcomps=1; rawbits=16; break; // short
            case '3': rawcomps=3; rawbits=8; break; // rgb
            case '4': rawcomps=4; rawbits=8; break; // rgba
            case '6': rawcomps=3; rawbits=16; break; // rgb 16-bit
            case '8': rawcomps=4; rawbits=16; break; // rgba 16-bit
            case 'u': rawsign=false; break; // unsigned
            case 's': rawsign=true; break; // signed
            case 'm': rawmsb=true; break; // MSB
            case 'l': rawmsb=false; break; // LSB
            default: return(false);
         }

         dotdot++;
      }
   }

   if (*dotdot=='_')
   {
      dotdot++;

      if (sscanf(dotdot,"%dx%d%n",&rawscalex,&rawscaley,&count)!=2) return(false);

      dotdot+=count;

      if (*dotdot=='x')
      {
         dotdot++;

         if (sscanf(dotdot,"%d%n",&rawscalez,&count)!=1) return(false);

         dotdot+=count;
      }
   }

   if (*dotdot!='.') return(false);

   if (bits==NULL)
   {
      if (rawcomps==1 && rawbits==16)
      {
         rawcomps=2;
         rawbits=8;
      }
      else if (rawcomps==3 && rawbits==16)
      {
         rawcomps=6;
         rawbits=8;
      }
      else if (rawcomps==4 && rawbits==16)
      {
         rawcomps=8;
         rawbits=8;
      }
   }

   if (rawcomps!=1 && components==NULL) return(false);
   if (rawbits!=8 && bits==NULL) return(false);
   if (rawsign!=false && sign==NULL) return(false);
   if (rawmsb!=true && msb==NULL) return(false);

   if (components!=NULL) *components=rawcomps;
   if (bits!=NULL) *bits=rawbits;
   if (sign!=NULL) *sign=rawsign;
   if (msb!=NULL) *msb=rawmsb;

   if (rawscalex>rawmaxscale) rawmaxscale=rawscalex;
   if (rawscaley>rawmaxscale) rawmaxscale=rawscaley;
   if (rawscalez>rawmaxscale) rawmaxscale=rawscalez;

   if (rawmaxscale==0) return(false);

   if (scalex!=NULL) *scalex=rawscalex/1E6f;
   if (scaley!=NULL) *scaley=rawscaley/1E6f;
   if (scalez!=NULL) *scalez=rawscalez/1E6f;

   return(true);
}

// define raw file format
inline char *lglMakeRawInfo(long long width,long long height,long long depth,
                            unsigned int components,unsigned int bits,bool sign,bool msb,
                            float scalex,float scaley,float scalez)
{
   static const int maxlen=100;

   char info[maxlen];
   float maxscale=0.0f;

   snprintf(info,maxlen,".%lldx%lld",width,height);
   if (depth>1) snprintf(&info[strlen(info)],maxlen-strlen(info),"x%lld",depth);

   if (components!=1 || bits!=8 || sign!=false || msb!=true ||
       scalex!=1.0f || scaley!=1.0f || scalez!=1.0f)
   {
      snprintf(&info[strlen(info)],maxlen-strlen(info),"_");

      if (sign==false) snprintf(&info[strlen(info)],maxlen-strlen(info),"u");
      else snprintf(&info[strlen(info)],maxlen-strlen(info),"s");

      if (components==1 && bits==8) snprintf(&info[strlen(info)],maxlen-strlen(info),"1");
      else if (components==1 && bits==16) snprintf(&info[strlen(info)],maxlen-strlen(info),"2");
      else if (components==2 && bits==8) snprintf(&info[strlen(info)],maxlen-strlen(info),"2");
      else if (components==3 && bits==8) snprintf(&info[strlen(info)],maxlen-strlen(info),"3");
      else if (components==4 && bits==8) snprintf(&info[strlen(info)],maxlen-strlen(info),"4");
      else if (components==3 && bits==16) snprintf(&info[strlen(info)],maxlen-strlen(info),"6");
      else if (components==4 && bits==16) snprintf(&info[strlen(info)],maxlen-strlen(info),"8");
      else return(NULL);

      if (components==2 || bits==16)
      {
         if (msb==true) snprintf(&info[strlen(info)],maxlen-strlen(info),"m");
         else snprintf(&info[strlen(info)],maxlen-strlen(info),"l");
      }

      if (scalex>maxscale) maxscale=scalex;
      if (scaley>maxscale) maxscale=scaley;
      if (scalez>maxscale) maxscale=scalez;

      if (maxscale==0.0f) return(NULL);

      if (scalex!=1.0f || scaley!=1.0f || scalez!=1.0f)
      {
         snprintf(&info[strlen(info)],maxlen-strlen(info),"_%dx%d",int(1E6f*scalex+0.5f),int(1E6f*scaley+0.5f));
         if (depth>1) snprintf(&info[strlen(info)],maxlen-strlen(info),"x%d",int(1E6f*scalez+0.5f));
      }
   }

   snprintf(&info[strlen(info)],maxlen-strlen(info),".raw");

   return(strdup(info));
}

// remove .raw suffix
inline char *lglRemoveRawSuffix(const char *filename)
{
   char *filename2,*dot;

   long long rawwidth,rawheight,rawdepth;
   unsigned int rawcomps,rawbits;
   bool rawsign,rawmsb;
   float rawscalex,rawscaley,rawscalez;

   filename2=strdup(filename);

   if (lglReadRawInfo(filename2,
                      &rawwidth,&rawheight,&rawdepth,
                      &rawcomps,&rawbits,&rawsign,&rawmsb,
                      &rawscalex,&rawscaley,&rawscalez))
   {
      dot=strrchr(filename2,'.');
      if (dot!=NULL)
      {
         *dot='\0';
         dot=strrchr(filename2,'.');
         if (dot!=NULL) *dot='\0';
      }
   }
   else
   {
      dot=strrchr(filename2,'.');
      if (dot!=NULL)
         if (strcmp(dot,".raw")==0) *dot='\0';
   }

   return(filename2);
}

// append raw file format suffix
inline char *lglAppendRawInfo(const char *filename,
                              long long width,long long height,long long depth,
                              unsigned int components,unsigned int bits,bool sign,bool msb,
                              float scalex,float scaley,float scalez)
{
   char *filename2;
   char *info;
   char *filename3;

   // define raw info
   info=lglMakeRawInfo(width,height,depth,
                       components,bits,sign,msb,
                       scalex,scaley,scalez);

   if (info==NULL) return(NULL);

   // remove suffix
   filename2=lglRemoveRawSuffix(filename);

   // append raw info to filename
   filename3=(char *)malloc(strlen(filename2)+strlen(info)+1);
   if (filename3!=NULL)
   {
      memcpy(filename3,filename2,strlen(filename2));
      memcpy(filename3+strlen(info),info,strlen(info)+1);
   }
   free(filename2);
   free(info);

   return(filename3);
}

// read raw data
inline unsigned char *lglReadRawData(const char *filename,
                                     long long *width,long long *height,long long *depth,
                                     unsigned int *components,unsigned int *bits,bool *sign,bool *msb,
                                     float *scalex,float *scaley,float *scalez)
{
   FILE *file;

   char *name;

   unsigned char *volume;
   long long bytes;

   // open raw file
   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   // analyze raw info
   name=strdup(filename);
   if (!lglReadRawInfo(name,
                       width,height,depth,
                       components,bits,sign,msb,
                       scalex,scaley,scalez))
   {
      free(name);
      fclose(file);
      return(NULL);
   }
   free(name);

   bytes=(*width)*(*height)*(*depth)*(*components);

   if (bits!=NULL)
      if (*bits==16) bytes*=2;

   if ((volume=(unsigned char *)malloc((size_t)bytes))==NULL) return(NULL);

   // read raw chunk
   if (fread(volume,(size_t)bytes,1,file)!=1)
   {
      free(volume);
      fclose(file);
      return(NULL);
   }

   fclose(file);

   return(volume);
}

// write raw data
inline char *lglWriteRawData(const char *filename, // /wo suffix .raw
                             unsigned char *volume,
                             long long width,long long height,long long depth,
                             unsigned int components,unsigned int bits,bool sign,bool msb,
                             float scalex,float scaley,float scalez)
{
   FILE *file;

   char *output;
   long long bytes;

   // make raw info
   output=lglAppendRawInfo(filename,
                           width,height,depth,
                           components,bits,sign,msb,
                           scalex,scaley,scalez);

   if (output==NULL) return(NULL);

   // open raw output file
   if ((file=fopen(output,"wb"))==NULL)
   {
      free(output);
      return(NULL);
   }

   bytes=width*height*depth*components;

   if (bits==16) bytes*=2;
   else if (bits==32) bytes*=4;

   // write raw chunk
   if (fwrite(volume,(size_t)bytes,1,file)!=1)
   {
      fclose(file);
      return(NULL);
   }

   fclose(file);

   return(output);
}

// convert a raw array to a 16-bit unsigned array
inline unsigned short int *lglConvertRaw(unsigned char *data,
                                         long long width,long long height,long long depth,
                                         unsigned int &components,
                                         unsigned int &bits,bool sign,bool msb)
{
   long long i;

   unsigned short int *shorts=NULL;
   long long cells=width*height*depth;

   if (components==2 && bits==8)
   {
      components=1;
      bits=16;
   }
   else if (components==6 && bits==8)
   {
      components=3;
      bits=16;
   }
   else if (components==8 && bits==8)
   {
      components=4;
      bits=16;
   }

   cells*=components;

   if (bits==8)
   {
      if ((shorts=(unsigned short int *)malloc((size_t)cells*sizeof(unsigned short int)))==NULL) return(NULL);

      if (sign)
         for (i=0; i<cells; i++) shorts[i]=data[i]+128;
      else
         for (i=0; i<cells; i++) shorts[i]=data[i];
   }
   else if (bits==16)
   {
      if ((shorts=(unsigned short int *)malloc((size_t)cells*sizeof(unsigned short int)))==NULL) return(NULL);

      if (msb)
         if (sign)
            for (i=0; i<cells; i++) shorts[i]=(signed short)(256*data[i<<1]+data[(i<<1)+1])+32768;
         else
            for (i=0; i<cells; i++) shorts[i]=(unsigned short)(256*data[i<<1]+data[(i<<1)+1]);
      else
         if (sign)
            for (i=0; i<cells; i++) shorts[i]=(signed short)(data[i<<1]+256*data[(i<<1)+1])+32768;
         else
            for (i=0; i<cells; i++) shorts[i]=(unsigned short)(data[i<<1]+256*data[(i<<1)+1]);
   }

   return(shorts);
}

// quantize 16-bit raw data to 8-bit using a linear mapping
inline unsigned char *lglStretchRaw(unsigned short int *data,
                                    long long width,long long height,long long depth,
                                    unsigned int components)
{
   long long i;

   unsigned char *data2;
   long long cells;

   int v,vmin,vmax;

   cells=width*height*depth*components;

   vmin=65535;
   vmax=0;

   for (i=0; i<cells; i++)
   {
      v=data[i];
      if (v<vmin) vmin=v;
      if (v>vmax) vmax=v;
   }

   if (vmin==vmax) vmax=vmin+1;

   if ((data2=(unsigned char *)malloc((size_t)cells))==NULL)
      return(NULL);

   for (i=0; i<cells; i++)
      data2[i]=(int)((data[i]-vmin)*255.0/(vmax-vmin)+0.5);

   return(data2);
}

// helper to get a raw value
inline int lglGetRawValue(unsigned short int *data,
                          long long width,long long height,long long /*depth*/,
                          long long i,long long j,long long k)
{
   return(data[i+(j+k*height)*width]);
}

// helper to get a gradient magnitude value
inline double lglGetRawGradMag(unsigned short int *data,
                               long long width,long long height,long long depth,
                               long long i,long long j,long long k)
{
   double gx,gy,gz;

   if (i>0)
      if (i<width-1) gx=(lglGetRawValue(data,width,height,depth,i+1,j,k)-lglGetRawValue(data,width,height,depth,i-1,j,k))/2.0;
      else gx=lglGetRawValue(data,width,height,depth,i,j,k)-lglGetRawValue(data,width,height,depth,i-1,j,k);
   else
      if (i<width-1) gx=lglGetRawValue(data,width,height,depth,i+1,j,k)-lglGetRawValue(data,width,height,depth,i,j,k);
      else gx=0.0;

   if (j>0)
      if (j<height-1) gy=(lglGetRawValue(data,width,height,depth,i,j+1,k)-lglGetRawValue(data,width,height,depth,i,j-1,k))/2.0;
      else gy=lglGetRawValue(data,width,height,depth,i,j,k)-lglGetRawValue(data,width,height,depth,i,j-1,k);
   else
      if (j<height-1) gy=lglGetRawValue(data,width,height,depth,i,j+1,k)-lglGetRawValue(data,width,height,depth,i,j,k);
      else gy=0.0;

   if (k>0)
      if (k<depth-1) gz=(lglGetRawValue(data,width,height,depth,i,j,k+1)-lglGetRawValue(data,width,height,depth,i,j,k-1))/2.0;
      else gz=lglGetRawValue(data,width,height,depth,i,j,k)-lglGetRawValue(data,width,height,depth,i,j,k-1);
   else
      if (k<depth-1) gz=lglGetRawValue(data,width,height,depth,i,j,k+1)-lglGetRawValue(data,width,height,depth,i,j,k);
      else gz=0.0;

   return(sqrt(gx*gx+gy*gy+gz*gz));
}

// quantize 16-bit raw data to 8-bit using a non-linear mapping
inline unsigned char *lglQuantizeRaw(unsigned short int *data,
                                     long long width,long long height,long long depth,
                                     bool linear)
{
   long long i,j,k;

   unsigned char *data2;
   long long cells;

   int v,vmin,vmax;

   double *err,eint;

   bool done;

   vmin=65535;
   vmax=0;

   for (k=0; k<depth; k++)
      for (j=0; j<height; j++)
         for (i=0; i<width; i++)
         {
            v=lglGetRawValue(data,width,height,depth,i,j,k);
            if (v<vmin) vmin=v;
            if (v>vmax) vmax=v;
         }

   if (vmin==vmax) vmax=vmin+1;

   if (vmax-vmin<256) linear=true;

   err=new double[65536];

   if (linear)
      for (i=0; i<65536; i++) err[i]=255*(double)(i-vmin)/(vmax-vmin);
   else
   {
      for (i=0; i<65536; i++) err[i]=0.0;

      for (k=0; k<depth; k++)
         for (j=0; j<height; j++)
            for (i=0; i<width; i++)
               err[lglGetRawValue(data,width,height,depth,i,j,k)]+=sqrt(lglGetRawGradMag(data,width,height,depth,i,j,k));

      for (i=0; i<65536; i++) err[i]=pow(err[i],1.0/3);

      err[vmin]=err[vmax]=0.0;

      for (k=0; k<256; k++)
      {
         for (eint=0.0,i=0; i<65536; i++) eint+=err[i];

         done=true;

         for (i=0; i<65536; i++)
            if (err[i]>eint/256)
            {
               err[i]=eint/256;
               done=false;
            }

         if (done) break;
      }

      for (i=1; i<65536; i++) err[i]+=err[i-1];

      if (err[65535]>0.0f)
         for (i=0; i<65536; i++) err[i]*=255.0/err[65535];
   }

   cells=width*height*depth;

   if ((data2=(unsigned char *)malloc((size_t)cells))==NULL)
   {
      delete err;
      return(NULL);
   }

   for (i=0; i<cells; i++)
      data2[i]=(int)(err[data[i]]+0.5);

   delete err;

   return(data2);
}

// load and quantize raw data
inline unsigned char *lglLoadRawData(const char *filename,
                                     long long *width,long long *height,long long *depth,
                                     float *scalex,float *scaley,float *scalez) // meters
{
   unsigned char *data=NULL;
   unsigned int components=0;
   unsigned int bits=0;
   bool sign=false,msb=false;

   data=lglReadRawData(filename,
                       width,height,depth,
                       &components,&bits,&sign,&msb,
                       scalex,scaley,scalez);

   if (data)
      if ((bits==8 && sign) || bits==16)
      {
         unsigned short int *shorts=lglConvertRaw(data,*width,*height,*depth,components,bits,sign,msb);
         free(data);

         unsigned char *data2;

         if (components==1)
            data2=lglQuantizeRaw(shorts,*width,*height,*depth);
         else
            data2=lglStretchRaw(shorts,*width,*height,*depth,components);

         free(shorts);

         data=data2;
      }

   return(data);
}

// load and quantize raw data
inline unsigned char *lglLoadRawData(std::string filename,
                                     long long *width,long long *height,long long *depth,
                                     float *scalex,float *scaley,float *scalez) // meters
{
   return(lglLoadRawData(filename.c_str(),
                         width,height,depth,
                         scalex,scaley,scalez));
}

// load and quantize raw image
inline unsigned char *lglLoadRawImage(const char *filename,
                                      int *width,int *height,
                                      int *components)
{
   unsigned char *data=NULL;
   long long lwidth=0,lheight=0,ldepth=0;
   unsigned int ucomponents=0;
   unsigned int bits=0;
   bool sign=false,msb=false;

   data=lglReadRawData(filename,
                       &lwidth,&lheight,&ldepth,
                       &ucomponents,&bits,&sign,&msb);

   if (data)
   {
      if (ldepth!=1)
      {
         free(data);
         return(NULL);
      }

      if ((bits==8 && sign) || bits==16)
      {
         unsigned short int *shorts=lglConvertRaw(data,lwidth,lheight,ldepth,ucomponents,bits,sign,msb);
         free(data);

         unsigned char *data2=lglStretchRaw(shorts,lwidth,lheight,ldepth,ucomponents);
         free(shorts);

         data=data2;
      }

      *width=(int)lwidth;
      *height=(int)lheight;
      *components=ucomponents;
   }

   return(data);
}

// load and quantize raw image
inline unsigned char *lglLoadRawImage(std::string filename,
                                      int *width,int *height,
                                      int *components)
{
   return(lglLoadRawImage(filename.c_str(),
                          width,height,
                          components));
}

// write raw image
inline bool lglWriteRawImage(const char *filename, // /wo suffix .raw
                             unsigned char *image,
                             int width,int height,
                             int components)
{
   char *raw;

   raw=lglWriteRawData(filename,
                       image,
                       width,height,1,
                       components);

   if (raw!=NULL)
   {
      free(raw);
      return(true);
   }

   return(false);
}

// write raw image
inline bool lglWriteRawImage(std::string filename, // /wo suffix .raw
                             unsigned char *image,
                             int width,int height,
                             int components)
{
   return(lglWriteRawImage(filename.c_str(),
                           image,
                           width,height,
                           components));
}

#endif
