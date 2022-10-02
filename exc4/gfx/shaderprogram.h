// (c) by Stefan Roettger, licensed under MIT license

#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

int buildvtxprog(const char *prog,bool fatal=true);
void bindvtxprog(int progid);
void setvtxprogpar(int n,float p1,float p2,float p3,float p4);
void setvtxprogpars(int n,int count,const float *params);
void deletevtxprog(int progid);

int buildfrgprog(const char *prog,bool fatal=true);
void bindfrgprog(int progid);
void setfrgprogpar(int n,float p1,float p2,float p3,float p4);
void setfrgprogpars(int n,int count,const float *params);
void deletefrgprog(int progid);

const char *getvtxerror();
const char *getfrgerror();

static const char default_vtxprg[]=
   "!!ARBvp1.0\n"
   "OPTION ARB_position_invariant; \n"
   "MOV result.color,vertex.color; \n"
   "END\n";

static const char default_frgprg[]=
   "!!ARBfp1.0\n"
   "MOV result.color,fragment.color; \n"
   "END\n";

static const char cueing_frgprg[]=
   "!!ARBfp1.0\n"
   "MAD t.z,fragment.position.z,15,-13.5;\n"
   "SUB t.z,1,t.z;\n"
   "MUL result.color,t.z,fragment.color;\n"
   "END\n";

static const char striping_frgprg[]=
   "!!ARBfp1.0\n"
   "TEMP t;\n"
   "MUL t.z,100,fragment.position.z;\n"
   "FRC t.z,t.z;\n"
   "MUL result.color,t.z,fragment.color;\n"
   "END\n";

static const char contouring_vtxprg[]=
   "!!ARBfp1.0\n"
   "OPTION ARB_position_invariant;\n"
   "TEMP t;\n"
   "MOV result.color,vertex.color;\n"
   "MOV t,vertex.position;\n"
   "MOV result.texcoord[1],t;\n"
   "END\n";

static const char contouring_frgprg[]=
   "!!ARBfp1.0\n"
   "TEMP t;\n"
   "MOV t,fragment.texcoord[1];\n"
   "MAD t,t,10,0.5;\n"
   "FRC t,t;\n"
   "SUB t,t,0.1;\n"
   "CMP t,t,0.5,1;\n"
   "MUL result.color,t.y,fragment.color;\n"
   "END\n";

static const char diffuse_vtxprg[]=
   "!!ARBvp1.0\n"
   "OPTION ARB_position_invariant;\n"
   "PARAM invtra[4]={state.matrix.modelview.invtrans};\n"
   "TEMP n,v;\n"
   "MOV n,vertex.normal;\n"
   "DP4 v.x,invtra[0],n;\n"
   "DP4 v.y,invtra[1],n;\n"
   "DP4 v.z,invtra[2],n;\n"
   "DP4 v.w,invtra[3],n;\n"
   "MOV result.texcoord[2],v;\n"
   "MOV result.color,vertex.color;\n"
   "END\n";

static const char diffuse_frgprg[]=
   "!!ARBfp1.0\n"
   "TEMP n,l;\n"
   "MOV n,fragment.texcoord[2];\n"
   "DP3 l.x,n,n;\n"
   "RSQ l.x,l.x;\n"
   "MUL n,n,l.x;\n"
   "MUL result.color,fragment.color,n.z;\n"
   "END\n";

static const char tex2d_vtxprg[]=
   "!!ARBvp1.0\n"
   "OPTION ARB_position_invariant;\n"
   "PARAM invtra[4]={state.matrix.modelview.invtrans};\n"
   "TEMP n,v;\n"
   "MOV n,vertex.normal;\n"
   "DP4 v.x,invtra[0],n;\n"
   "DP4 v.y,invtra[1],n;\n"
   "DP4 v.z,invtra[2],n;\n"
   "DP4 v.w,invtra[3],n;\n"
   "MOV result.texcoord[2],v;\n"
   "MOV result.texcoord[0],vertex.texcoord[0];\n"
   "#MOV result.texcoord[0],vertex.position.xzyw;\n"
   "MOV result.color,vertex.color;\n"
   "END\n";

static const char tex2d_frgprg[]=
   "!!ARBfp1.0\n"
   "TEMP n,l,c,t;\n"
   "MOV n,fragment.texcoord[2];\n"
   "MOV c,fragment.texcoord[0];\n"
   "TEX t,c,texture[0],2D;\n"
   "DP3 l.x,n,n;\n"
   "RSQ l.x,l.x;\n"
   "MUL n,n,l.x;\n"
   "MUL t,fragment.color,t;\n"
   "MUL result.color,t,n.z;\n"
   "END\n";

static const char tex3d_vtxprg[]=
   "!!ARBvp1.0\n"
   "OPTION ARB_position_invariant;\n"
   "MOV result.color,vertex.color;\n"
   "MOV result.texcoord[0],vertex.position;\n"
   "END\n";

static const char tex3d_frgprg[]=
   "!!ARBfp1.0\n"
   "TEMP t;\n"
   "TEX t,fragment.texcoord[0],texture[1],3D;\n"
   "MAD t,t,0.8,0.2;\n"
   "MUL result.color,t,fragment.color;\n"
   "END\n";

#endif
