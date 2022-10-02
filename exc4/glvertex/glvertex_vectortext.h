// (c) by Stefan Roettger, licensed under MIT license

#ifndef GLVERTEX_VECTORTEXT_H
#define GLVERTEX_VECTORTEXT_H

#include "glvertex_api.h"

inline void lglRenderLine(float x1,float y1,float z1,float x2,float y2,float z2,
                          lglVBO *vbo=NULL)
{
   if (vbo)
   {
      vbo->lglBegin(LGL_LINES);
         vbo->lglVertex(x1,y1,z1);
         vbo->lglVertex(x2,y2,z2);
      vbo->lglEnd();
   }
   else
   {
      lglBegin(LGL_LINES);
         lglVertex(x1,y1,z1);
         lglVertex(x2,y2,z2);
      lglEnd();
   }
}

inline void lglDrawSymbol(const char *symbol,
                          lglVBO *vbo=NULL)
{
   float px,py,lx,ly;

   bool draw=true;

   px=py=0.0f;

   while (*symbol!='\0')
   {
      lx=px;
      ly=py;

      switch (*symbol++)
      {
         case 'u': draw=false; break;
         case 'd': draw=true; break;
         case 'n': py+=1.0f; break;
         case 's': py-=1.0f; break;
         case 'e': px+=1.0f; break;
         case 'w': px-=1.0f; break;
         case 'N': py+=1.0f; px+=1.0f; break;
         case 'S': py-=1.0f; px-=1.0f; break;
         case 'E': px+=1.0f; py-=1.0f; break;
         case 'W': px-=1.0f; py+=1.0f; break;
      }

      if (draw)
         if (px!=lx || py!=ly) lglRenderLine(lx,ly,0.0f,px,py,0.0f,vbo);
   }
}

inline void lglDrawLetter(char letter,
                          lglVBO *vbo=NULL)
{
   lglPushMatrix();
   lglScale(1.0f/4,1.0f/6,0.0f);

   switch (toupper(letter))
   {
      // letters occupy a 7x4 grid
      case 'A': lglDrawSymbol("nnnnnNeEsssssunnndwww",vbo); break;
      case 'B': lglDrawSymbol("nnnnnneeEsSwwueedEsSww",vbo); break;
      case 'C': lglDrawSymbol("ueeNdSwWnnnnNeE",vbo); break;
      case 'D': lglDrawSymbol("nnnnnneeEssssSww",vbo); break;
      case 'E': lglDrawSymbol("ueeedwwwnnnnnneeeussswdww",vbo); break;
      case 'F': lglDrawSymbol("nnnnnneeeussswdww",vbo); break;
      case 'G': lglDrawSymbol("unnneedessSwWnnnnNeE",vbo); break;
      case 'H': lglDrawSymbol("nnnnnnueeedssssssunnndwww",vbo); break;
      case 'I': lglDrawSymbol("uedeeuwdnnnnnneuwdw",vbo); break;
      case 'J': lglDrawSymbol("undEeNnnnnnwww",vbo); break;
      case 'K': lglDrawSymbol("nnnnnnusssdNNNuSSSdEEE",vbo); break;
      case 'L': lglDrawSymbol("ueeedwwwnnnnnn",vbo); break;
      case 'M': lglDrawSymbol("nnnnnnEeNssssss",vbo); break;
      case 'N': lglDrawSymbol("nnnnnnusdEEEssunndnnnn",vbo); break;
      case 'O': lglDrawSymbol("uedWnnnnNeEssssSw",vbo); break;
      case 'P': lglDrawSymbol("nnnnnneeEsSww",vbo); break;
      case 'Q': lglDrawSymbol("uedWnnnnNeEssssSwuNdE",vbo); break;
      case 'R': lglDrawSymbol("nnnnnneeEsSwwuedEEs",vbo); break;
      case 'S': lglDrawSymbol("undEeNnWwWnNeE",vbo); break;
      case 'T': lglDrawSymbol("ueednnnnnnwwueedee",vbo); break;
      case 'U': lglDrawSymbol("unnnnnndsssssEeNnnnnn",vbo); break;
      case 'V': lglDrawSymbol("unnnnnndsssssENNnnnn",vbo); break;
      case 'W': lglDrawSymbol("unnnnnndssssssNeEnnnnnn",vbo); break;
      case 'X': lglDrawSymbol("nnNNNnuwwwdsEEEss",vbo); break;
      case 'Y': lglDrawSymbol("ueednnWWnnueeedsssS",vbo); break;
      case 'Z': lglDrawSymbol("ueeendswwwnnNNNnwwws",vbo); break;
      case '_': lglDrawSymbol("eee",vbo); break;
      case '0': lglDrawSymbol("uedWnnnnNeEssssSwuNNdWWW",vbo); break;
      case '1': lglDrawSymbol("ueednnnnnnSS",vbo); break;
      case '2': lglDrawSymbol("ueeedwwwNNNnnWwSs",vbo); break;
      case '3': lglDrawSymbol("undEeNnWwuedNnWwS",vbo); break;
      case '4': lglDrawSymbol("ueednnnnnnuwwdssseee",vbo); break;
      case '5': lglDrawSymbol("undEeNnWwwnnneee",vbo); break;
      case '6': lglDrawSymbol("unndNeEsSwWnnnnNeE",vbo); break;
      case '7': lglDrawSymbol("ueednnnnnnwwusssedee",vbo); break;
      case '8': lglDrawSymbol("uedWnNWnNeEsSwuedEsSw",vbo); break;
      case '9': lglDrawSymbol("undEeNnnnnWwSsEee",vbo); break;
      case '.': lglDrawSymbol("uednesw",vbo); break;
      case ',': lglDrawSymbol("uEdN",vbo); break;
      case ':': lglDrawSymbol("unedneswunndnesw",vbo); break;
      case ';': lglDrawSymbol("uneedwnesSunnndnesw",vbo); break;
      case '?': lglDrawSymbol("ueednnnNnWwS",vbo); break;
      case '!': lglDrawSymbol("ueednundnnnn",vbo); break;
      case '|': lglDrawSymbol("ueednnnnnn",vbo); break;
      case '-': lglDrawSymbol("unnnedee",vbo); break;
      case '+': lglDrawSymbol("unnnedeeunwdss",vbo); break;
      case '*': lglDrawSymbol("unnedNNuwwdEE",vbo); break;
      case '/': lglDrawSymbol("undNNN",vbo); break;
      case '>': lglDrawSymbol("unedNNWW",vbo); break;
      case '<': lglDrawSymbol("uneeedWWNN",vbo); break;
      case '=': lglDrawSymbol("unnedeeunndww",vbo); break;
      case '(': lglDrawSymbol("ueedWnnnnN",vbo); break;
      case ')': lglDrawSymbol("uedNnnnnW",vbo); break;
      case '[': lglDrawSymbol("ueedwnnnnnne",vbo); break;
      case ']': lglDrawSymbol("uedennnnnnw",vbo); break;
      case '{': lglDrawSymbol("ueedwnnWNnne",vbo); break;
      case '}': lglDrawSymbol("udennNWnnw",vbo); break;
      case '$': lglDrawSymbol("unndEeNWwWNeEuWndssssssuwdnnnnnn",vbo); break;
      case '%': lglDrawSymbol("undNNNusssdswneuwwnnndnwse",vbo); break;
      case '#': lglDrawSymbol("unndeeeunndwwwuNdssssuednnnn",vbo); break;
      case '&': lglDrawSymbol("ueeedWWWnnNEssSSseNN",vbo); break;
      case '@': lglDrawSymbol("ueeendSwWnnnnNeEssswwnnee",vbo); break;
      case '^': lglDrawSymbol("uennnndNE",vbo); break;
      case '~': lglDrawSymbol("unnnndNEN",vbo); break;
      case '"': lglDrawSymbol("uennnndnueds",vbo); break;
      case '\'': lglDrawSymbol("uennnndn",vbo); break;
   }

   lglPopMatrix();
}

inline void lglDrawString(float width,
                          const char *str,
                          bool centered=true,
                          lglVBO *vbo=NULL)
{
   const float linefeed=0.2f;

   int c,cmax,l;
   float scale;
   const char *ptr;

   if (str==NULL) return;

   for (c=0,cmax=l=1,ptr=str; *ptr!='\0'; ptr++)
   {
      if (*ptr!='\n') c++;
      else {c=0; l++;}
      if (c>cmax) cmax=c;
   }

   scale=width/cmax;

   if (vbo) lglMatrixMode(LGL_PREMODEL);

   lglPushMatrix();
   if (centered) lglTranslate(-0.5f,0.0f,0.0f);
   lglScale(scale,scale,0.0f);
   lglTranslate(0.0f,(l-1)*(1.0f+linefeed),0.0f);
   if (centered) lglTranslate(0.0f,-l*(1.0f+linefeed)/2,0.0f);
   lglPushMatrix();

   while (*str!='\0')
   {
      if (*str=='\n')
      {
         lglPopMatrix();
         lglTranslate(0.0f,-(1.0f+linefeed),0.0f);
         lglPushMatrix();
      }
      else
      {
         if (*str>='a' && *str<='z')
         {
            lglPushMatrix();
            lglTranslate(0.2f,0.0f,0.0f);
            lglScale(0.6f,0.75f,1.0f);

            lglDrawLetter(*str,vbo);

            lglPopMatrix();
            }
         else lglDrawLetter(*str,vbo);

         lglTranslate(1.0f,0.0f,0.0f);
      }

      str++;
   }

   lglPopMatrix();
   lglPopMatrix();

   if (vbo) lglMatrixMode(LGL_MODELVIEW);
}

#endif
