// (c) by Stefan Roettger, licensed under MIT license

#include "defs.h"
#include "gl.h"

#include <string>

#include "shaderprogram.h"

static std::string vtxerror;
static std::string frgerror;

void seterror(std::string e,bool vtxorfrg,bool fatal)
   {
   if (vtxorfrg) vtxerror=e;
   else frgerror=e;

   if (!e.empty()) WARNMSG(e.c_str());
   if (fatal) ERRORMSG();
   }

const char *getvtxerror()
   {return(vtxerror.c_str());}

const char *getfrgerror()
   {return(frgerror.c_str());}

int buildprog(const char *prog,bool vtxorfrg,bool fatal=true)
   {
   GLuint progid=0;

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)
   GLint errorPos,isNative;

   initwglprocs();

   glGenProgramsARB(1,&progid);

   if (vtxorfrg)
      {
      glBindProgramARB(GL_VERTEX_PROGRAM_ARB,progid);
      glProgramStringARB(GL_VERTEX_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB,strlen(prog),prog);
      }
   else
      {
      glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,progid);
      glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB,strlen(prog),prog);
      }

   glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB,&errorPos);

   if (errorPos==0)
      {
      seterror(std::string("shader program unavailable: ")+(char *)glGetString(GL_PROGRAM_ERROR_STRING_ARB),vtxorfrg,fatal);
      glDeleteProgramsARB(1,&progid);
      progid=0;
      }
   else
      {
      if (errorPos!=-1)
         {
         seterror((char *)glGetString(GL_PROGRAM_ERROR_STRING_ARB),vtxorfrg,fatal);
         glDeleteProgramsARB(1,&progid);
         progid=0;
         }
      else
         {
         if (vtxorfrg) glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB,&isNative);
         else glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB,&isNative);

         if (isNative!=1) seterror("shader program non-native",vtxorfrg,false);
         else seterror("",vtxorfrg,false);
         }
      }
#endif

   return(progid);
   }

void bindprog(int progid,bool vtxorfrg)
   {
#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)
   initwglprocs();

   if (progid!=0)
      if (vtxorfrg)
         {
         glBindProgramARB(GL_VERTEX_PROGRAM_ARB,progid);
         glEnable(GL_VERTEX_PROGRAM_ARB);
         }
      else
         {
         glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,progid);
         glEnable(GL_FRAGMENT_PROGRAM_ARB);
         }
   else
      if (vtxorfrg)
         {
         glBindProgramARB(GL_VERTEX_PROGRAM_ARB,0);
         glDisable(GL_VERTEX_PROGRAM_ARB);
         }
      else
         {
         glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,0);
         glDisable(GL_FRAGMENT_PROGRAM_ARB);
         }
#endif
   }

void setprogpar(int n,float p1,float p2,float p3,float p4,bool vtxorfrg)
   {
#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)
   initwglprocs();

   if (vtxorfrg) glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB,n,p1,p2,p3,p4);
   else glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,n,p1,p2,p3,p4);
#endif
   }

void setprogpars(int n,int count,const float *params,bool vtxorfrg)
   {
#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)
   initwglprocs();

   if (vtxorfrg)
      for (int i=0; i<count; i++)
         glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB,n+i,params[4*i],params[4*i+1],params[4*i+2],params[4*i+3]);
   else
      for (int i=0; i<count; i++)
         glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,n+i,params[4*i],params[4*i+1],params[4*i+2],params[4*i+3]);
#endif
   }

void deleteprog(int progid)
   {
#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)
   initwglprocs();

   if (progid!=0)
      {
      GLuint id=progid;
      glDeleteProgramsARB(1,&id);
      }
#endif
   }

int buildvtxprog(const char *prog,bool fatal) {return(buildprog(prog,TRUE,fatal));}
void bindvtxprog(int progid) {bindprog(progid,TRUE);}
void setvtxprogpar(int n,float p1,float p2,float p3,float p4) {setprogpar(n,p1,p2,p3,p4,TRUE);}
void setvtxprogpars(int n,int count,const float *params) {setprogpars(n,count,params,TRUE);}
void deletevtxprog(int progid) {deleteprog(progid);}

int buildfrgprog(const char *prog,bool fatal) {return(buildprog(prog,FALSE,fatal));}
void bindfrgprog(int progid) {bindprog(progid,FALSE);}
void setfrgprogpar(int n,float p1,float p2,float p3,float p4) {setprogpar(n,p1,p2,p3,p4,FALSE);}
void setfrgprogpars(int n,int count,const float *params) {setprogpars(n,count,params,FALSE);}
void deletefrgprog(int progid) {deleteprog(progid);}
