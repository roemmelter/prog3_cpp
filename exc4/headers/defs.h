// (c) by Stefan Roettger, licensed under MIT license

#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <math.h>

#include <iostream>

#include <time.h>
#ifndef _WIN32
#include <sys/time.h>
#else
#include <windows.h>
#include <winbase.h>
#endif

#ifndef NULL
#define NULL (0)
#endif

#define BOOLINT unsigned char

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#define WARNMSG(msg) errormsg(__FILE__,__LINE__,FALSE,msg)
#define ERRORMSG() errormsg(__FILE__,__LINE__,TRUE)

inline void errormsg(const char *file,int line,int fatal,const char *msg=NULL)
   {
   fprintf(stderr,"error in <%s> at line %d!\n",file,line);
   if (msg!=NULL) fprintf(stderr,"description: %s\n",msg);
   if (fatal) exit(EXIT_FAILURE);
   }

#define PI (3.14159265358979323846264338327950288)
#define RAD (PI/180.0)

#ifndef MAXFLOAT
#define MAXFLOAT (FLT_MAX)
#endif

#undef ffloor
#define ffloor(x) floor((double)(x))
#undef fceil
#define fceil(x) ceil((double)(x))
#define ftrc(x) (int)ffloor(x)

#ifndef min
inline int min(const int a,const int b) {return((a<b)?a:b);}
#endif

#ifndef max
inline int max(const int a,const int b) {return((a>b)?a:b);}
#endif

#ifdef _WIN32

#define strdup _strdup
#define snprintf _snprintf

inline int strcasecmp(const char *str1,const char *str2)
   {
   const char *ptr1,*ptr2;
   for (ptr1=str1,ptr2=str2; tolower(*ptr1)==tolower(*ptr2) && *ptr1!='\0' && *ptr2!='\0'; ptr1++,ptr2++);
   return(*ptr1-*ptr2);
   }

inline char *strcasestr(const char *str1,const char *str2)
   {
   unsigned int i,j;

   unsigned int len1,len2;

   len1=strlen(str1);
   len2=strlen(str2);

   for (i=0; i+len2<=len1; i++)
      {
      for (j=0; j<len2; j++)
         if (tolower(str2[j])!=tolower(str1[i+j])) break;

      if (j==len2) return((char *)&str1[i]);
      }

   return(NULL);
   }

#endif

inline char *strdup2(const char *str1,const char *str2)
   {
   char *str;

   if (str1==NULL && str2==NULL) return(NULL);

   if (str1==NULL) return(strdup(str2));
   if (str2==NULL) return(strdup(str1));

   if ((str=(char *)malloc(strlen(str1)+strlen(str2)+1))==NULL) ERRORMSG();

   memcpy(str,str1,strlen(str1));
   memcpy(str+strlen(str1),str2,strlen(str2)+1);

   return(str);
   }

inline double GETTIME()
   {
#ifndef _WIN32
   struct timeval t;
   gettimeofday(&t,NULL);
   return(t.tv_sec+t.tv_usec/1.0E6);
#else
   static int cpus=0;
   if (cpus==0)
      {
      SYSTEM_INFO SystemInfo;
      GetSystemInfo(&SystemInfo);
      cpus=SystemInfo.dwNumberOfProcessors;
      }
   if (cpus==1)
      {
      LARGE_INTEGER freq,count;
      if (QueryPerformanceFrequency(&freq)==0) ERRORMSG();
      QueryPerformanceCounter(&count);
      return((double)count.QuadPart/freq.QuadPart);
      }
   return((double)clock()/CLOCKS_PER_SEC);
#endif
   }

inline double gettime()
   {
   static double time;
   static BOOLINT settime=FALSE;

   if (!settime)
      {
      time=GETTIME();
      settime=TRUE;
      }

   return(GETTIME()-time);
   }

inline void waitfor(double secs)
   {
   if (secs<=0.0) return;
#ifndef _WIN32
   struct timespec dt,rt;
   dt.tv_sec=ftrc(secs);
   dt.tv_nsec=ftrc(1.0E9*(secs-ftrc(secs)));
   while (nanosleep(&dt,&rt)!=0) dt=rt;
#else
   double time=gettime()+secs;
   while (gettime()<time);
#endif
   }

#endif
