// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL multi-pass rendering classes
#ifndef GLVERTEX_SCENE_H
#define GLVERTEX_SCENE_H

#include "glvertex.h"
#include "glvertex_cam.h"
#include "glvertex_nodes.h"
#include "glvertex_object.h"

//! scene class
//! * abstraction of a graphical scene
//! * provides a pure virtual renderScene() method for rendering the scene
//! * provides additional hooks for multi-pass rendering
class lgl_Scene
{
public:

   //! ctor
   lgl_Scene()
      : time_(0),
        paused_(false),
        width_(0), height_(0),
        next_(NULL)
   {}

   virtual ~lgl_Scene()
   {}

   void updateTime(double dt)
   {
      time_ += dt;
   }

   //! get current time
   double getTime()
   {
      return(time_);
   }

   //! restart time
   void restart()
   {
      time_ = 0;
   }

   //! is the scene paused?
   bool paused()
   {
      return(paused_);
   }

   //! pause the scene
   void pause(bool yes = true)
   {
      paused_ = yes;
   }

   //! toggle the animation
   void toggle_animation()
   {
      paused_ = !paused_;
   }

   void setViewport(int width, int height)
   {
      width_ = width;
      height_ = height;
   }

   //! get the current viewport width
   int viewportWidth()
   {
      return(width_);
   }

   //! get the current viewport height
   int viewportHeight()
   {
      return(height_);
   }

   //! get the current viewport aspect
   double viewportAspect()
   {
      return((double)width_/height_);
   }

   //! trigger an update
   void update()
   {
      update_ = true;
   }

   bool check4update()
   {
      bool update = update_;
      update_ = false;
      return(update);
   }

   //! provide scene that will replace the actual one on next frame
   void provideNextScene(lgl_Scene *next)
   {
      next_ = next;
   }

   virtual void initScene() {} // reimplement to initialize scene state
   virtual void clearScene() {} // reimplement to clear the frame buffer
   virtual int getPasses() {return(1);} // reimplement to specify number of passes
   virtual int getScene(int pass) {return(pass);} // reimplement to specify a corresponding scene for each pass
   virtual void setupPass(int pass) {} // reimplement to setup a specific pass
   virtual void finishPass(int pass) {} // reimplement to finish a specific pass
   virtual void preupdateScene(double dt) {} // reimplement to pre-update the scene
   virtual void renderScene(int scene) = 0; // reimplement to render a specific scene
   virtual void updateScene(double dt) {} // reimplement to update the scene
   virtual void updateState() {} // reimplement for state updates
   virtual lglVBO *pickScene(vec3 o, vec3 d) {return(NULL);} // reimplement to pick the scene
   virtual lgl_Scene *nextScene() {return(next_);} // reimplement to change the scene
   virtual lgl_Cam *getCam() {return(NULL);} // reimplement to provide a cam
   virtual bool supportsStereo() {return(false);} // reimplement to signal stereo support
   virtual void keyPressed(char key) {} // reimplement to check for key press events
   virtual void keyReleased(char key) {} // reimplement to check for key release events
   virtual std::string getKeyInfo() {return("");} // reimplement to print key commands
   virtual void mouseClick(double x, double y) {} // reimplement to track mouse clicks
   virtual void doubleClick(double x, double y) {} // reimplement to track mouse double clicks
   virtual void interaction() {} // reimplement to track user interaction

   virtual void doubleClickCenter() {toggle_animation();}
   virtual void doubleClickLeftBottom() {}
   virtual void doubleClickRightBottom() {}
   virtual void doubleClickLeftTop() {}
   virtual void doubleClickRightTop() {lglTogglePolygonMode();}

   virtual std::string getDoubleClickInfo()
   {
      const char clicks[] =
         "center double-click: pause animation\n"
         "top right double-click: toggle polygon mode\n";

      return(clicks);
   }

protected:

   double time_;
   bool paused_;

   int width_, height_;
   bool update_;

   lgl_Scene *next_;

   void renderCube()
   {
      lglBegin(LGL_QUADS);
         lglNormal(0,0,-1);
         lglVertex(-0.5,-0.5,-0.5);
         lglVertex(-0.5,0.5,-0.5);
         lglVertex(0.5,0.5,-0.5);
         lglVertex(0.5,-0.5,-0.5);
         lglNormal(0,0,1);
         lglVertex(0.5,-0.5,0.5);
         lglVertex(0.5,0.5,0.5);
         lglVertex(-0.5,0.5,0.5);
         lglVertex(-0.5,-0.5,0.5);
         lglNormal(-1,0,0);
         lglVertex(-0.5,-0.5,0.5);
         lglVertex(-0.5,0.5,0.5);
         lglVertex(-0.5,0.5,-0.5);
         lglVertex(-0.5,-0.5,-0.5);
         lglNormal(1,0,0);
         lglVertex(0.5,-0.5,-0.5);
         lglVertex(0.5,0.5,-0.5);
         lglVertex(0.5,0.5,0.5);
         lglVertex(0.5,-0.5,0.5);
         lglNormal(0,-1,0);
         lglVertex(-0.5,-0.5,0.5);
         lglVertex(-0.5,-0.5,-0.5);
         lglVertex(0.5,-0.5,-0.5);
         lglVertex(0.5,-0.5,0.5);
         lglNormal(0,1,0);
         lglVertex(0.5,0.5,0.5);
         lglVertex(0.5,0.5,-0.5);
         lglVertex(-0.5,0.5,-0.5);
         lglVertex(-0.5,0.5,0.5);
      lglEnd();
   }

   void renderScreen()
   {
      // matrix setup
      lglProjection();
      lglModelView();

      // render quad /w unit texture coordinates
      lglDepthTest(false);
      lglColor(1,1,1);
      lglBegin(LGL_QUADS);
         lglTexCoord(0,0);
         lglVertex(-1,-1,0);
         lglTexCoord(1,0);
         lglVertex(1,-1,0);
         lglTexCoord(1,1);
         lglVertex(1,1,0);
         lglTexCoord(0,1);
         lglVertex(-1,1,0);
      lglEnd();
      lglDepthTest(true);
   }

   void renderLeftRight(bool left)
   {
      // matrix setup
      lglProjection();
      lglModelView();

      // render L or R for stereo setup
      if (left)
         lglColor(1,0,0);
      else
         lglColor(0,1,1);
      lglLineWidth(8);
      lglBegin(LGL_LINE_STRIP);
         lglNormal(0,0,1);
         if (left)
         {
            // L
            lglVertex(-0.25,0.5,0);
            lglVertex(-0.25,-0.5,0);
            lglVertex(0.25,-0.5,0);
         }
         else
         {
            // R
            lglVertex(-0.25,-0.5,0);
            lglVertex(-0.25,0.5,0);
            lglVertex(0.25,0.5,0);
            lglVertex(0.25,0,0);
            lglVertex(-0.25,0,0);
            lglVertex(0.25,-0.5,0);
         }
      lglEnd();
   }

};

class lgl_TestScene: public lgl_Scene
{
public:

   lgl_TestScene() : lgl_Scene(), teapot(NULL), loaded(false) {}
   virtual ~lgl_TestScene() {if (teapot) delete teapot;}

protected:

   lglVBO *teapot;
   bool loaded;

   void renderTable()
   {
      lglPushMatrix();
      lglTranslate(vec3(0,1,0));
      lglScale(vec3(3.1,0.1,1.1));
      renderCube();
      lglPopMatrix();
      lglPushMatrix();
      lglTranslate(vec3(-1.5,0.5,-0.5));
      lglScale(vec3(0.1,1,0.1));
      renderCube();
      lglPopMatrix();
      lglPushMatrix();
      lglTranslate(vec3(-1.5,0.5,0.5));
      lglScale(vec3(0.1,1,0.1));
      renderCube();
      lglPopMatrix();
      lglPushMatrix();
      lglTranslate(vec3(1.5,0.5,-0.5));
      lglScale(vec3(0.1,1,0.1));
      renderCube();
      lglPopMatrix();
      lglPushMatrix();
      lglTranslate(vec3(1.5,0.5,0.5));
      lglScale(vec3(0.1,1,0.1));
      renderCube();
      lglPopMatrix();
   }

   void renderTestScene(bool color = true)
   {
      // load teapot
      if (!loaded)
      {
         teapot = lglLoadObj("teapot.obj", true);
         if (!teapot) teapot = new lglCube();
         loaded = true;
      }

      // render table
      renderTable();

      // render colored cubes on table
      lglPushMatrix();
      lglTranslate(-0.5,1.1,0);
      lglScale(0.2);
      if (color) lglColor(1,0,0);
      renderCube();
      lglRotate(30, 0,1,0);
      lglTranslate(-2,0,0);
      if (color) lglColor(0,1,0);
      renderCube();
      lglRotate(-45, 0,1,0);
      lglTranslate(-2,0,0);
      if (color) lglColor(0,0,1);
      renderCube();
      lglPopMatrix();

      // render teapot on table
      if (teapot)
      {
         lglPushMatrix();
         lglTranslate(vec3(0.3,1.2,0));
         lglScale(0.005);
         if (color) teapot->lglColor(1.2,1.2,1.1);
         teapot->lglCloneProgram(getGL());
         teapot->lglCloneState(getGL());
         teapot->lglRender();
         lglPopMatrix();
      }
   }

};

//! scene class container for a scene graph
class lgl_SceneGraphScene: public lgl_Scene
{
public:

   //! ctor
   lgl_SceneGraphScene(lgl_Node *root = NULL)
      : lgl_Scene(),
        root_(root),
        shadow_(false),
        shadow_p_(0,0,0), shadow_n_(0,1,0), shadow_d_(0,1,0),
        shadow_c_(0),
        cull_(true),
        cull_minsize_(50)
   {}

   virtual ~lgl_SceneGraphScene()
   {
      if (root_)
         delete root_;
   }

   //! get scene graph
   lgl_Node *getSceneGraph()
   {
      return(root_);
   }

   //! set scene graph
   void setSceneGraph(lgl_Node *root)
   {
      if (root_)
         delete root_;

      root_ = root;
   }

   //! is the shadow plane enabled?
   bool isShadowPlaneEnabled()
   {
      return(shadow_);
   }

   //! enable the shadow plane
   void enableShadowPlane(bool on)
   {
      shadow_ = on;
   }

   //! get shadow plane origin position
   vec3 getShadowPlaneOrigin()
   {
      return(shadow_p_);
   }

   //! get shadow plane normal vector
   vec3 getShadowPlaneNormal()
   {
      return(shadow_n_);
   }

   //! get shadow plane light direction
   vec3 getShadowPlaneDirection()
   {
      return(shadow_d_);
   }

   //! set shadow plane
   void setShadowPlane(vec3 p, vec3 n, vec3 d)
   {
      shadow_p_ = p;
      shadow_n_ = n;
      shadow_d_ = d;
   }

   //! set shadow color
   void setShadowColor(vec4 c)
   {
      shadow_c_ = c;
   }

   //! enable view culling
   void enableViewCulling(int minsize = 0)
   {
      cull_ = true;
      cull_minsize_ = minsize;
   }

   //! disable view culling
   void disableViewCulling()
   {
      cull_ = false;
   }

protected:

   lgl_Node *root_;

   bool shadow_;
   vec3 shadow_p_, shadow_n_, shadow_d_;
   vec4 shadow_c_;

   bool cull_;
   int cull_minsize_;

   virtual void clearScene() {lglClear();}

   virtual int getPasses() {return(2);}

   virtual void renderScene(int scene)
   {
      if (scene == 0)
      {
         if (root_)
         {
            // set camera aspect
            if (lgl_Cam *cam = getCam())
               cam->setAspect(viewportAspect());

            // enable view culling
            if (cull_)
               lglEnableViewCulling(cull_minsize_);

            // render pass #1
            root_->drawSceneGraph();

            // disable view culling
            if (cull_)
               lglDisableViewCulling();
         }
      }
      else if (scene == 1)
      {
         if (shadow_)
         {
            if (root_)
            {
               bool disabled = false;

               // check shadow plane
               if (shadow_n_==vec3(0) || shadow_d_==vec3(0))
                  disabled = true;

               // get eye point
               if (lgl_Cam *cam = getCam())
               {
                  vec3 eye = cam->getEyeManip();

                  if ((eye-shadow_p_).dot(shadow_n_) <= 0)
                     disabled = true;
               }

               if (!disabled)
               {
                  // shadow color
                  lglColor(shadow_c_);

                  // disable lighting etc.
                  lglDisableColoring(true);
                  lglDisableLighting(true);
                  lglDisableTexturing(true);

                  // shadow projection matrix
                  mat4 P = mat4::parallel(shadow_p_, shadow_n_, shadow_d_);

                  // render pass #2
                  root_->drawSceneGraph(P);

                  // enable lighting etc.
                  lglDisableColoring(false);
                  lglDisableLighting(false);
                  lglDisableTexturing(false);
               }
            }
         }
      }
   }

   virtual void preupdateScene(double dt)
   {
      if (root_)
         root_->preupdateSceneGraph(dt);
   }

   virtual void updateScene(double dt)
   {
      if (root_)
         root_->updateSceneGraph(dt);
   }

   virtual lglVBO *pickScene(vec3 o, vec3 d)
   {
      if (root_)
         return(root_->pickSceneGraph(o, d));

      return(NULL);
   }

   virtual lgl_Cam *getCam()
   {
      if (root_)
         return(root_->find<lgl_CameraNode>());

      return(NULL);
   }

   virtual bool supportsStereo()
   {
      return(true);
   }

};

//! scene class container for multiple graphical objects
//! * scene can be semi-transparent on a per-object basis
class lgl_GraphicalObjectScene: public lgl_Scene
{
public:

   //! ctor
   lgl_GraphicalObjectScene(lgl_Cam *cam = NULL, lgl_Objects *objs = NULL)
      : lgl_Scene(),
        cam_(cam),
        objects_(objs),
        depth_sorting_(true)
   {}

   virtual ~lgl_GraphicalObjectScene()
   {
      if (cam_)
         delete cam_;

      if (objects_)
         delete objects_;
   }

   //! set scene camera
   void setCam(lgl_Cam *cam)
   {
      if (cam_)
         delete cam_;

      cam_ = cam;
   }

   //! set scene objects
   void setObjects(lgl_Objects *objs)
   {
      if (objects_)
         delete objects_;

      objects_ = objs;
   }

   //! get scene objects
   lgl_Objects *getObjects()
   {
      return(objects_);
   }

   //! enable depth sorting of semi-transparent objects
   void enableDepthSorting(bool on = true)
   {
      depth_sorting_ = on;
   }

protected:

   lgl_Cam *cam_;
   lgl_Objects *objects_;
   bool depth_sorting_;

   virtual void clearScene()
   {
      lglClear();
   }

   virtual void renderScene(int scene)
   {
      if (scene == 0)
      {
         // begin camera
         if (cam_)
         {
            cam_->setAspect(viewportAspect());
            cam_->begin();
         }

         // render graphical objects
         if (objects_)
         {
            if (depth_sorting_)
               objects_->drawObjects(cam_->getEyeManip());
            else
               objects_->drawObjects();
         }

         // end camera
         if (cam_)
         {
            cam_->end();
         }
      }
   }

   virtual void preupdateScene(double dt)
   {
      if (objects_)
         objects_->preupdateObjects(dt);
   }

   virtual void updateScene(double dt)
   {
      if (objects_)
         objects_->updateObjects(dt);
   }

   virtual lgl_Cam *getCam()
   {
      return(cam_);
   }

   virtual bool supportsStereo()
   {
      return(true);
   }

};

//! multi-pass renderer
//! * designed as an abstraction layer for multi-pass rendering
//! * referenced scene object is rendered multiple times
//! * scene object is required to provide hooks to setup the rendering state
//! * for each pass according hooks are called before and after the actual render call
class lgl_Renderer
{
public:

   //! ctor
   lgl_Renderer()
      : scene_(NULL),
        modified_(false),
        interlacing_(LGL_INTERLACE_NONE)
   {}

   virtual ~lgl_Renderer()
   {
      if (scene_) delete scene_;
   }

   //! set the actual scene
   void setScene(lgl_Scene *scene)
   {
      if (scene_) delete scene_;
      scene_ = scene;
      modified_ = true;
      update();
   }

   //! get the actual scene
   lgl_Scene *getScene()
   {
      return(scene_);
   }

   //! pick the actual scene
   lglVBO *pickScene(vec3 o, vec3 d)
   {
      if (scene_)
         return(scene_->pickScene(o, d));

      return(NULL);
   }

   //! set the stereo interlacing mode
   void setStereoMode(lgl_interlacing_enum interlacing = LGL_INTERLACE_VERTICAL_TOP)
   {
      interlacing_ = interlacing;
   }

protected:

   lgl_Scene *scene_;
   bool modified_;

   lgl_interlacing_enum interlacing_;

   virtual void config() {}
   virtual int getViewportWidth() = 0;
   virtual int getViewportHeight() = 0;
   virtual void update() = 0;

   virtual void initializeOpenGL()
   {
      // setup OpenGL
      lglInitializeOpenGL(1,1,1);
   }

   virtual void renderOpenGL(double dt)
   {
      if (scene_)
      {
         // update viewport
         scene_->setViewport(getViewportWidth(), getViewportHeight());

         // init scene
         if (modified_)
         {
            scene_->initScene();
            modified_ = false;
            config();
         }

         // check for stereo cam
         bool stereo = false;
         lgl_Cam *cam = NULL;
         if (scene_)
         {
            if (scene_->supportsStereo())
               cam = scene_->getCam();
         }
         if (cam)
         {
            stereo = cam->isStereoCam();
            if (interlacing_ == LGL_INTERLACE_NONE) stereo = false;
         }

         // clear scene
         scene_->clearScene();

         // pre-update scene
         scene_->preupdateScene(scene_->paused()?0:dt);

         // render scene
         if (!stereo)
         {
            for (int i=0; i<scene_->getPasses(); i++)
            {
               scene_->setupPass(i);
               scene_->renderScene(scene_->getScene(i));
               scene_->finishPass(i);
            }
         }
         else
         {
            cam->useLeftEye();
            lglInterlacingMode(interlacing_);

            for (int i=0; i<scene_->getPasses(); i++)
            {
               scene_->setupPass(i);
               scene_->renderScene(scene_->getScene(i));
               scene_->finishPass(i);
            }

            cam->useRightEye();
            lglComplementaryInterlacingMode(interlacing_);

            for (int i=0; i<scene_->getPasses(); i++)
            {
               scene_->setupPass(i);
               scene_->renderScene(scene_->getScene(i));
               scene_->finishPass(i);
            }

            lglInterlacingMode(LGL_INTERLACE_NONE);
         }

         // update scene
         scene_->updateScene(scene_->paused()?0:dt);

         // update state
         scene_->updateState();

         // check for updates
         if (scene_->check4update()) update();

         // update current time
         scene_->updateTime(scene_->paused()?0:dt);

         // next scene
         lgl_Scene *scene = scene_->nextScene();

         // proceed to next scene
         if (scene != NULL) setScene(scene);
      }
   }

   virtual void keyPressed(char key)
   {
      if (scene_)
         scene_->keyPressed(key);
   }

   virtual void keyReleased(char key)
   {
      if (scene_)
         scene_->keyReleased(key);
   }

   virtual std::string getKeyInfo()
   {
      if (scene_)
         return(scene_->getKeyInfo());

      return("");
   }

   virtual void mouseClick(double x, double y)
   {
      if (scene_)
         scene_->mouseClick(x, y);
   }

   virtual void doubleClick(double x, double y)
   {
      static const double corner = 0.15;

      if (scene_)
      {
         scene_->doubleClick(x, y);

         if (x<=corner && y<=corner) scene_->doubleClickLeftTop();
         else if (x>=1-corner && y<=corner) scene_->doubleClickRightTop();
         else if (x<=corner && y>=1-corner) scene_->doubleClickLeftBottom();
         else if (x>=1-corner && y>=1-corner) scene_->doubleClickRightBottom();
         else scene_->doubleClickCenter();
      }
   }

   virtual std::string getDoubleClickInfo()
   {
      if (scene_)
         return(scene_->getDoubleClickInfo());

      return("");
   }

   virtual void interaction()
   {
      if (scene_)
         scene_->interaction();
   }

};

#endif
