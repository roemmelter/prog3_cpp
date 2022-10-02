// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL scene object classes
#ifndef GLVERTEX_OBJECT_H
#define GLVERTEX_OBJECT_H

#include "glvertex.h"
#include "glvertex_nodes.h"

#include <list>

//! graphical object class (base class)
//! * abstraction of a graphical object
//! * provides a pure virtual renderObject() method for rendering the graphical object
//! * has a color vector plus transparency flag
//! * has a 4x4 transformation matrix
//! * has a local scale vector
class lgl_Object
{
public:

   //! ctor
   lgl_Object()
      : color_(1,1,1),
        transparency_(false),
        partial_trnsp_(false),
        transformation_(1),
        position_(0,0,0),
        scale_(1),
        hidden_(false),
        init_(true)
   {}

   virtual ~lgl_Object()
   {}

   virtual void initObject() {} // reimplement to initialize object state
   virtual void renderObject() = 0; // reimplement to render a specific object
   virtual void preupdateObject(double dt) {} // reimplement to pre-update the object
   virtual void updateObject(double dt) {} // reimplement to update and change the object

   //! get object color
   vec4 getColor()
   {
      return(color_);
   }

   //! set object color
   void setColor(const vec4 &c)
   {
      color_ = c;
      transparency_ = color_.a < 1;
   }

   //! get object transparency flag
   bool hasTransparency()
   {
      return(transparency_);
   }

   //! set object transparency flag
   void hasTransparency(bool flag)
   {
      transparency_ = flag;
   }

   //! get partial object transparency flag
   bool hasPartialTransparency()
   {
      return(partial_trnsp_);
   }

   //! set partial object transparency flag
   void hasPartialTransparency(bool flag)
   {
      partial_trnsp_ = flag;
   }

   //! get object transformation
   mat4 getTransformation()
   {
      return(transformation_);
   }

   //! set object transformation
   void setTransformation(const mat4 &m)
   {
      transformation_ = m;
      position_ = m.getTranslation();
   }

   //! get object position
   vec3 getPosition()
   {
      return(position_);
   }

   //! get object rotation
   mat3 getRotation()
   {
      return(transformation_.getRotation());
   }

   //! get object scale
   vec3 getScale()
   {
      return(scale_);
   }

   //! get maximum object scale
   double getMaxScale()
   {
      double s = scale_.x;
      if (scale_.y > s) s = scale_.y;
      if (scale_.z > s) s = scale_.z;
      return(s);
   }

   //! object has uniform scale?
   bool hasUniformScale()
   {
      return(scale_.x==scale_.y && scale_.y==scale_.z);
   }

   //! get object barycenter relative to object position
   virtual vec3 getCenter()
   {
      return(vec3(0,0,0));
   }

   //! get bounding box extent
   virtual vec3 getExtent()
   {
      return(vec3(0,0,0));
   }

   //! get bounding object radius
   virtual double getRadius()
   {
      return(0);
   }

   //! get object norm
   virtual double getNorm()
   {
      return(0);
   }

   //! move object by delta vector
   void move(const vec3 &delta)
   {
      transformation_ <<= mat4::translate(delta);
      position_ = transformation_.getTranslation();
   }

   //! rotate object by quaternion
   void rotate(const quat &q)
   {
      transformation_ *= mat3(q);
   }

   //! scale object by factor
   void scale(double factor)
   {
      scale_ *= factor;
   }

   //! scale object by non-uniform factor
   void scale(const vec3 &factor)
   {
      scale_ *= factor;
   }

   //! is the object hidden?
   bool hidden()
   {
      return(hidden_);
   }

   //! is the object shown?
   bool shown()
   {
      return(!hidden_);
   }

   //! hide object
   void hide(bool yes = true)
   {
      hidden_ = yes;
   }

   //! show object
   void show(bool yes = true)
   {
      hidden_ = !yes;
   }

   //! render transformed object
   void renderTransformedObject()
   {
      if (!hidden_)
      {
         lglPushMatrix();
         lglMultMatrix(transformation_);
         if (scale_ != vec3(1)) lglScale(scale_);
         lglColor(color_);
         renderObject();
         lglPopMatrix();
      }
   }

   //! compute minimum distance to transformed object
   virtual double getDistanceToObject(const vec3 &v)
   {
      return(getDistanceToSphere(getPosition(), v, getRadius()));
   }

protected:

   vec4 color_;
   bool transparency_;
   bool partial_trnsp_;
   mat4 transformation_;
   vec3 position_;
   vec3 scale_;
   bool hidden_;
   bool init_;

   bool init()
   {
      if (init_)
      {
         init_ = false;
         return(true);
      }

      return(false);
   }

   //! compute minimum distance to sphere
   static double getDistanceToSphere(const vec3 &p, const vec3 &v, double radius)
   {
      vec3 d = p-v;
      double l = d.length()-radius;
      return(l);
   }

   //! compute minimum distance to box
   static double getDistanceToBox(const vec3 &p, const vec3 &v, const vec3 &ext)
   {
      vec3 d = p-v;
      if (d.x < 0) d.x = -d.x;
      if (d.y < 0) d.y = -d.y;
      if (d.z < 0) d.z = -d.z;
      d -= ext/2;
      if (d.x<0 && d.y<0 && d.z<0)
      {
         double l = d.x;
         if (d.y > l) l = d.y;
         if (d.z > l) l = d.z;
         return(l);
      }
      else
      {
         if (d.x < 0) d.x = 0;
         if (d.y < 0) d.y = 0;
         if (d.z < 0) d.z = 0;
         return(d.length());
      }
   }

   //! compute minimum distance to rotated box
   static double getDistanceToBox(const vec3 &p, const mat3 &rotation, const vec3 &v, const vec3 &ext)
   {
      return(getDistanceToBox(p, rotation.invert()*(v-p)+p, ext));
   }

   //! compute minimum distance to rotated and displaced box
   static double getDistanceToBox(const vec3 &p, const mat3 &rotation, const vec3 &offset, const vec3 &v, const vec3 &ext)
   {
      return(getDistanceToBox(p+offset, rotation.invert()*(v-p)+p, ext));
   }

   friend class lgl_Objects;
   friend class lgl_DepthSortedObjects;
};

//! geometry object
//! * graphical object that references a vbo as geometric item
class lgl_GeometryObject: public lgl_Object
{
public:

   //! ctor
   lgl_GeometryObject(lglVBO *vbo = NULL)
      : lgl_Object(),
        vbo_(vbo)
   {}

   virtual ~lgl_GeometryObject()
   {}

   //! get vbo
   lglVBO *getVBO()
   {
      return(vbo_);
   }

   //! set vbo
   void setVBO(lglVBO *vbo)
   {
      vbo_ = vbo;
   }

   virtual vec3 getCenter()
   {
      if (vbo_)
         return(vbo_->lglGetCenter()*getScale());

      return(vec3(0,0,0));
   }

   virtual vec3 getExtent()
   {
      if (vbo_)
         return(vbo_->lglGetExtent()*getScale());

      return(vec3(0,0,0));
   }

   virtual double getRadius()
   {
      if (vbo_)
         return(vbo_->lglGetRadius()*getMaxScale());

      return(0);
   }

   virtual double getNorm()
   {
      if (vbo_)
      {
         double s = getMaxScale();
         return(vbo_->lglGetNorm()*s*s);
      }

      return(0);
   }

   virtual void renderObject()
   {
      if (vbo_)
         vbo_->lglRender();
   }

   virtual double getDistanceToObject(const vec3 &v)
   {
      return(getDistanceToBox(getPosition(), getRotation(), v, getExtent()));
   }

protected:

   lglVBO *vbo_;
};

//! scene graph object
//! * graphical object that references a scene graph as complex composite geometric item
//! * the scene graph is owned by the object
class lgl_SceneGraphObject: public lgl_Object
{
public:

   //! ctor
   lgl_SceneGraphObject(lgl_Node *root = NULL)
      : lgl_Object(),
        root_(NULL)
   {
      setSceneGraph(root);
   }

   virtual ~lgl_SceneGraphObject()
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

      if (root_->hasTransparency())
      {
         hasTransparency(true);
         hasPartialTransparency(true);
      }
   }

   virtual vec3 getCenter()
   {
      if (root_)
         return(root_->getCenter()*getScale());

      return(vec3(0,0,0));
   }

   virtual vec3 getExtent()
   {
      if (root_)
         return(root_->getExtent()*getScale());

      return(vec3(0,0,0));
   }

   virtual double getRadius()
   {
      if (root_)
         return(root_->getRadius()*getMaxScale());

      return(0);
   }

   virtual double getNorm()
   {
      if (root_)
      {
         double s = getMaxScale();
         return(root_->getNorm()*s*s);
      }

      return(0);
   }

   virtual void renderObject()
   {
      if (root_)
         root_->drawSceneGraph();
   }

   virtual void preupdateObject(double dt)
   {
      if (root_)
         root_->preupdateSceneGraph(dt);
   }

   virtual void updateObject(double dt)
   {
      if (root_)
         root_->updateSceneGraph(dt);
   }

   virtual double getDistanceToObject(const vec3 &v)
   {
      return(getDistanceToBox(getPosition(), getRotation(), getCenter(), v, getExtent()));
   }

protected:

   lgl_Node *root_;
};

//! cube object
//! * graphical object that references a shared cube vbo
class lgl_CubeObject: public lgl_GeometryObject
{
public:

   //! ctor
   lgl_CubeObject()
      : lgl_GeometryObject()
   {
      static lglCube cube;
      setVBO(&cube);
   }

};

//! sphere object
//! * graphical object that references a shared sphere vbo
class lgl_SphereObject: public lgl_GeometryObject
{
public:

   //! ctor
   lgl_SphereObject()
      : lgl_GeometryObject()
   {
      static lglSphere sphere;
      setVBO(&sphere);
   }

   virtual double getDistanceToObject(const vec3 &v)
   {
      return(getDistanceToSphere(getPosition(), v, getRadius()));
   }

protected:

   virtual double getRadius()
   {
      return(0.5*getMaxScale());
   }

   virtual double getNorm()
   {
      double s = getMaxScale();
      return(0.25*s*s);
   }

};

// helper class that provides the 'less' compare operator for depth sorting
class lgl_DepthSortCompare
{
public:

   lgl_DepthSortCompare(vec3 eye)
      : eye_(eye)
   {}

   bool operator() (lgl_Object *a, lgl_Object *b) const
   {
      return((a->getPosition()-eye_).norm() >
             (b->getPosition()-eye_).norm());
   }

protected:

   vec3 eye_;
};

//! container class for multiple depth-sorted graphical objects
//! * the container does not own the objects
class lgl_DepthSortedObjects: public std::vector<lgl_Object*>
{
public:

   typedef std::vector<lgl_Object*>::iterator iterator;

   //! ctor
   lgl_DepthSortedObjects()
      : std::vector<lgl_Object*>()
   {}

   virtual ~lgl_DepthSortedObjects()
   {}

   //! draw all objects with depth sorting
   virtual void drawObjects(vec3 eye)
   {
      depthSort(eye);

      for (iterator it=begin(); it!=end(); it++)
      {
         if ((*it)->init())
            (*it)->initObject();

         (*it)->renderTransformedObject();
      }
   }

protected:

   void depthSort(vec3 eye)
   {
      lgl_DepthSortCompare customLess(eye);
      std::sort(begin(), end(), customLess);
   }

};

//! container class for multiple graphical objects
//! * the container owns the objects
class lgl_Objects: protected std::list<lgl_Object*>
{
public:

   typedef std::list<lgl_Object*>::iterator iterator;

   //! ctor
   lgl_Objects()
      : std::list<lgl_Object*>(),
        selected_(end())
   {}

   virtual ~lgl_Objects()
   {
      for (iterator it=begin(); it!=end(); it++)
         delete *it;
   }

   //! add graphical object
   virtual iterator addObject(lgl_Object *obj)
   {
      push_back(obj);
      return(--end());
   }

   //! find graphical object
   iterator findObject(lgl_Object *obj)
   {
      for (iterator it=begin(); it!=end(); it++)
         if (*it == obj)
            return(it);

      return(end());
   }

   //! find nearest graphical object within search radius
   iterator findObject(vec3 center, double radius)
   {
      iterator result = end();
      double distance = radius;

      for (iterator it=begin(); it!=end(); it++)
      {
         double d = (*it)->getDistanceToObject(center);

         if (d <= radius)
            if (d <= distance)
            {
               result = it;
               distance = d;
            }
      }

      return(result);
   }

   //! select graphical object
   void selectObject(iterator it)
   {
      selected_ = it;
   }

   //! get selected graphical object
   iterator getSelectedObject()
   {
      return(selected_);
   }

   //! remove graphical object
   virtual void removeObject(iterator it)
   {
      if (it == selected_)
         selected_ = end();

      delete *it;
      erase(it);
   }

   //! begin iterator
   iterator begin()
   {
      return(std::list<lgl_Object*>::begin());
   }

   //! end iterator
   iterator end()
   {
      return(std::list<lgl_Object*>::end());
   }

   //! render and update all objects
   void renderObjects(double dt = 0)
   {
      preupdateObjects(dt);
      drawObjects();
      updateObjects(dt);
   }

   //! render and update all objects
   void renderObjects(vec3 eye, double dt = 0)
   {
      preupdateObjects(dt);
      drawObjects(eye);
      updateObjects(dt);
   }

   //! draw all objects without updating
   virtual void drawObjects()
   {
      for (iterator it=begin(); it!=end(); it++)
      {
         if ((*it)->init())
            (*it)->initObject();

         (*it)->renderTransformedObject();
      }
   }

   //! draw all objects with depth sorting
   virtual void drawObjects(vec3 eye)
   {
      lgl_DepthSortedObjects objs;
      lgl_DepthSortedObjects partial_objs;

      // opaque bin
      for (iterator it=begin(); it!=end(); it++)
      {
         if ((*it)->init())
            (*it)->initObject();

         if (!((*it)->hasTransparency()))
            (*it)->renderTransformedObject();
         else
            if (!((*it)->hasPartialTransparency()))
               objs.push_back(*it);
            else
               partial_objs.push_back(*it);
      }

      // transparent bin
      partial_objs.drawObjects(eye);
      bool zwrite = lglGetZWrite();
      lglZWrite(false);
      objs.drawObjects(eye);
      lglZWrite(zwrite);
   }

   //! pre-update all objects
   virtual void preupdateObjects(double dt)
   {
      for (iterator it=begin(); it!=end(); it++)
      {
         if ((*it)->init())
            (*it)->initObject();

         (*it)->preupdateObject(dt);
      }
   }

   //! update all objects
   virtual void updateObjects(double dt)
   {
      for (iterator it=begin(); it!=end(); it++)
      {
         if ((*it)->init())
            (*it)->initObject();

         (*it)->updateObject(dt);
      }
   }

protected:

   iterator selected_;
};

//! physical object (base template)
template<typename T>
class lgl_PhysicalObject: public lgl_Object
{
public:

   //! ctor
   lgl_PhysicalObject()
      : lgl_Object(),
        mass_(0), restitution_(1),
        velocity_(0,0,0),
        data_(NULL)
   {}

   virtual ~lgl_PhysicalObject()
   {
      if (data_)
         delete data_;
   }

   //! get physical data
   T *getPhysicalData()
   {
      return(data_);
   }

   //! set physical data
   void setPhysicalData(T *data)
   {
      if (data_)
         delete data_;

      data_ = data;
   }

   //! create physical data
   //! * to be reimplemented by physics engine
   virtual T *createPhysicalData()
   {
      return(NULL);
   }

   //! get physical mass
   double getPhysicalMass()
   {
      return(mass_);
   }

   //! set physical mass
   //! a mass of zero creates a static (immovable) object
   void setPhysicalMass(double mass)
   {
      mass_ = mass;
   }

   //! get physical restitution
   double getPhysicalRestitution()
   {
      return(restitution_);
   }

   //! set physical restitution
   void setPhysicalRestitution(double restitution)
   {
      restitution_ = restitution;
   }

   void setTransformation(const mat4 &m)
   {
      lgl_Object::setTransformation(m);

      if (getPhysicalData())
         transformationChanged(transformation_);
   }

   virtual mat4 getPhysicalTransformation() = 0; // to be implemented by physics engine

   void setPhysicalTransformation(const mat4 &m)
   {
      lgl_Object::setTransformation(m);
   }

   void move(const vec3 &delta)
   {
      lgl_Object::move(delta);

      if (getPhysicalData())
         transformationChanged(transformation_);
   }

   void rotate(const quat &q)
   {
      lgl_Object::rotate(q);

      if (getPhysicalData())
         transformationChanged(transformation_);
   }

   void scale(double factor)
   {
      if (!getPhysicalData())
         lgl_Object::scale(factor);
   }

   void scale(vec3 factor)
   {
      if (!getPhysicalData())
         lgl_Object::scale(factor);
   }

   //! set object velocity
   void setVelocity(vec3 velocity)
   {
      velocity_ = velocity;

      if (getPhysicalData())
         velocityChanged(velocity_);
   }

   //! get object velocity
   vec3 getVelocity()
   {
      return(velocity_);
   }

   virtual vec3 getPhysicalVelocity() = 0; // to be implemented by physics engine

   void setPhysicalVelocity(vec3 velocity)
   {
      velocity_ = velocity;
   }

   //! apply force to point relative to center of mass
   virtual void applyForce(vec3 force, vec3 rel_pos = vec3(0,0,0)) = 0; // to be implemented by physics engine

   //! apply linear impulse to center of mass
   virtual void applyImpulse(vec3 impulse) = 0; // to be implemented by physics engine

   //! apply angular impulse to center of mass
   virtual void applyTorque(vec3 torque) = 0; // to be implemented by physics engine

protected:

   double mass_;
   double restitution_;
   vec3 velocity_;

   T *data_;

   virtual void transformationChanged(const mat4 &m) = 0; // to be implemented by physics engine
   virtual void velocityChanged(const vec3 &v) = 0; // to be implemented by physics engine
};

//! physical geometry object (base template)
//! * physical object that references a vbo as geometric item
template<typename T>
class lgl_PhysicalGeometryObject: public lgl_PhysicalObject<T>
{
public:

   //! ctor
   lgl_PhysicalGeometryObject(lglVBO *vbo = NULL)
      : lgl_PhysicalObject<T>(),
        vbo_(vbo)
   {}

   virtual ~lgl_PhysicalGeometryObject()
   {}

   //! get vbo
   lglVBO *getVBO()
   {
      return(vbo_);
   }

   //! set vbo
   void setVBO(lglVBO *vbo)
   {
      vbo_ = vbo;
   }

   virtual vec3 getCenter()
   {
      if (vbo_)
         return(vbo_->lglGetCenter()*lgl_PhysicalObject<T>::getScale());

      return(vec3(0,0,0));
   }

   virtual vec3 getExtent()
   {
      if (vbo_)
         return(vbo_->lglGetExtent()*lgl_PhysicalObject<T>::getScale());

      return(vec3(0,0,0));
   }

   virtual double getRadius()
   {
      if (vbo_)
         return(vbo_->lglGetRadius()*lgl_PhysicalObject<T>::getMaxScale());

      return(0);
   }

   virtual double getNorm()
   {
      if (vbo_)
      {
         double s = lgl_PhysicalObject<T>::getMaxScale();
         return(vbo_->lglGetNorm()*s*s);
      }

      return(0);
   }

   virtual void renderObject()
   {
      if (vbo_)
         vbo_->lglRender();
   }

protected:

   lglVBO *vbo_;
};

//! no physical geometry object (empty physics implementation)
class lgl_NoPhysicalGeometryObject: public lgl_PhysicalGeometryObject<void*>
{
public:

   virtual mat4 getPhysicalTransformation()
   {
      return(getTransformation());
   }

   virtual vec3 getPhysicalVelocity()
   {
      return(getVelocity());
   }

   virtual void applyForce(vec3 force, vec3 rel_pos = vec3(0,0,0)) {}
   virtual void applyImpulse(vec3 impulse) {}
   virtual void applyTorque(vec3 torque) {}

protected:

   virtual void transformationChanged(const mat4 &m) {}
   virtual void velocityChanged(const vec3 &v) {}
};

//! physical scene graph object (base template)
//! * physical object that references a scene graph as complex composite geometric item
template<typename T>
class lgl_PhysicalSceneGraphObject: public lgl_PhysicalObject<T>
{
public:

   //! ctor
   lgl_PhysicalSceneGraphObject(lgl_Node *root = NULL)
      : lgl_PhysicalObject<T>(),
        root_(NULL)
   {
      setSceneGraph(root);
   }

   virtual ~lgl_PhysicalSceneGraphObject()
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

      if (root_->hasTransparency())
      {
         lgl_PhysicalObject<T>::hasTransparency(true);
         lgl_PhysicalObject<T>::hasPartialTransparency(true);
      }
   }

   virtual vec3 getCenter()
   {
      if (root_)
         return(root_->getCenter()*lgl_PhysicalObject<T>::getScale());

      return(vec3(0,0,0));
   }

   virtual vec3 getExtent()
   {
      if (root_)
         return(root_->getExtent()*lgl_PhysicalObject<T>::getScale());

      return(vec3(0,0,0));
   }

   virtual double getRadius()
   {
      if (root_)
         return(root_->getRadius()*lgl_PhysicalObject<T>::getMaxScale());

      return(0);
   }

   virtual double getNorm()
   {
      if (root_)
      {
         double s = lgl_PhysicalObject<T>::getMaxScale();
         return(root_->getNorm()*s*s);
      }

      return(0);
   }

   virtual void renderObject()
   {
      if (root_)
         root_->drawSceneGraph();
   }

   virtual void preupdateObject(double dt)
   {
      if (root_)
         root_->preupdateSceneGraph(dt);
   }

   virtual void updateObject(double dt)
   {
      if (root_)
         root_->updateSceneGraph(dt);
   }

protected:

   lgl_Node *root_;
};

//! no physical scene graph object (empty physics implementation)
class lgl_NoPhysicalSceneGraphObject: public lgl_PhysicalSceneGraphObject<void*>
{
public:

   virtual mat4 getPhysicalTransformation()
   {
      return(getTransformation());
   }

   virtual vec3 getPhysicalVelocity()
   {
      return(getVelocity());
   }

   virtual void applyForce(vec3 force, vec3 rel_pos = vec3(0,0,0)) {}
   virtual void applyImpulse(vec3 impulse) {}
   virtual void applyTorque(vec3 torque) {}

protected:

   virtual void transformationChanged(const mat4 &m) {}
   virtual void velocityChanged(const vec3 &v) {}
};

//! physical cube object (base template)
//! * physical object that references a shared cube vbo
template<typename T>
class lgl_PhysicalCubeObject: public lgl_PhysicalGeometryObject<T>
{
public:

   //! ctor
   lgl_PhysicalCubeObject()
      : lgl_PhysicalGeometryObject<T>()
   {
      static lglCube cube;
      lgl_PhysicalGeometryObject<T>::setVBO(&cube);
   }

};

//! no physical cube object (empty physics implementation)
class lgl_NoPhysicalCubeObject: public lgl_PhysicalCubeObject<void*>
{
public:

   virtual mat4 getPhysicalTransformation()
   {
      return(getTransformation());
   }

   virtual vec3 getPhysicalVelocity()
   {
      return(getVelocity());
   }

   virtual void applyForce(vec3 force, vec3 rel_pos = vec3(0,0,0)) {}
   virtual void applyImpulse(vec3 impulse) {}
   virtual void applyTorque(vec3 torque) {}

protected:

   virtual void transformationChanged(const mat4 &m) {}
   virtual void velocityChanged(const vec3 &v) {}
};

//! physical sphere object (base template)
//! * physical object that references a shared sphere vbo
template<typename T>
class lgl_PhysicalSphereObject: public lgl_PhysicalGeometryObject<T>
{
public:

   //! ctor
   lgl_PhysicalSphereObject()
      : lgl_PhysicalGeometryObject<T>()
   {
      static lglSphere sphere;
      lgl_PhysicalGeometryObject<T>::setVBO(&sphere);
   }

protected:

   virtual double getRadius()
   {
      return(0.5*lgl_PhysicalGeometryObject<T>::getMaxScale());
   }

   virtual double getNorm()
   {
      double s = lgl_PhysicalGeometryObject<T>::getMaxScale();
      return(0.25*s*s);
   }

};

//! no physical sphere object (empty physics implementation)
class lgl_NoPhysicalSphereObject: public lgl_PhysicalSphereObject<void*>
{
public:

   virtual mat4 getPhysicalTransformation()
   {
      return(getTransformation());
   }

   virtual vec3 getPhysicalVelocity()
   {
      return(getVelocity());
   }

   virtual void applyForce(vec3 force, vec3 rel_pos = vec3(0,0,0)) {}
   virtual void applyImpulse(vec3 impulse) {}
   virtual void applyTorque(vec3 torque) {}

protected:

   virtual void transformationChanged(const mat4 &m) {}
   virtual void velocityChanged(const vec3 &v) {}
};

//! container class for multiple physical objects (base template)
template<typename T>
class lgl_PhysicalObjects: public lgl_Objects
{
public:

   typedef lgl_Objects::iterator iterator;

   //! ctor
   lgl_PhysicalObjects()
      : lgl_Objects(),
        ground_o(0,0,0),
        ground_n(0,1,0),
        ground_g(-9.81),
        ground_r(1.0)
   {}

   virtual ~lgl_PhysicalObjects()
   {}

   //! add physical object
   virtual iterator addObject(lgl_Object *obj)
   {
      if (lgl_PhysicalObject<T> *pobj=dynamic_cast<lgl_PhysicalObject<T>*>(obj))
      {
         T *data = pobj->createPhysicalData();
         pobj->setPhysicalData(data);
         objectAdded(pobj);
      }

      return(lgl_Objects::addObject(obj));
   }

   //! find nearest non-static graphical object within search radius
   iterator findNonStaticObject(vec3 center, double radius)
   {
      iterator result = end();
      double distance = radius;

      for (iterator it=begin(); it!=end(); it++)
         if (lgl_PhysicalObject<T> *pobj=dynamic_cast<lgl_PhysicalObject<T>*>(*it))
            if (pobj->getPhysicalMass() > 0)
            {
               double d = pobj->getDistanceToObject(center);

               if (d <= radius)
                  if (d <= distance)
                  {
                     result = it;
                     distance = d;
                  }
            }

      return(result);
   }

   //! remove physical object
   virtual void removeObject(iterator it)
   {
      if (lgl_PhysicalObject<T> *pobj=dynamic_cast<lgl_PhysicalObject<T>*>(*it))
         objectRemoved(pobj);

      lgl_Objects::removeObject(it);
   }

   //! deactivate physical object
   virtual void deactivateObject(iterator it, bool yes = true)
   {
      if (lgl_PhysicalObject<T> *pobj=dynamic_cast<lgl_PhysicalObject<T>*>(*it))
         objectDeactivated(pobj, yes);
   }

   //! begin iterator
   iterator begin()
   {
      return(lgl_Objects::begin());
   }

   //! end iterator
   iterator end()
   {
      return(lgl_Objects::end());
   }

   //! set ground plane
   //! * parameters o and n are a point on the plane and the plane normal
   //! * a normal with zero norm is interpreted as missing ground plane
   void setGroundPlane(vec3 o, vec3 n, double r = 1.0)
   {
      ground_o = o;
      ground_n = n;
      ground_r = r;
   }

   //! set gravity
   void setGravity(double g)
   {
      ground_g = g;
   }

protected:

   vec3 ground_o;
   vec3 ground_n;

   double ground_g;
   double ground_r;

   virtual void updateObjects(double dt)
   {
      updatePhysicalObjects(dt);

      for (iterator it=begin(); it!=end(); it++)
      {
         if (lgl_PhysicalObject<T> *pobj=dynamic_cast<lgl_PhysicalObject<T>*>(*it))
         {
            mat4 m = pobj->getPhysicalTransformation();
            pobj->setPhysicalTransformation(m);

            vec3 v = pobj->getPhysicalVelocity();
            pobj->setPhysicalVelocity(v);
         }
      }

      lgl_Objects::updateObjects(dt);
   }

   virtual void objectAdded(lgl_PhysicalObject<T> *pobj) = 0; // to be implemented by physics engine
   virtual void objectRemoved(lgl_PhysicalObject<T> *pobj) = 0; // to be implemented by physics engine
   virtual void objectDeactivated(lgl_PhysicalObject<T> *pobj, bool yes) = 0; // to be implemented by physics engine
   virtual void updatePhysicalObjects(double dt) = 0; // to be implemented by physics engine
};

//! container class for multiple non-physical objects (empty physics implementation)
class lgl_NoPhysicalObjects: public lgl_PhysicalObjects<void*>
{
protected:

   virtual void objectAdded(lgl_PhysicalObject<void*> *pobj) {}
   virtual void objectRemoved(lgl_PhysicalObject<void*> *pobj) {}
   virtual void objectDeactivated(lgl_PhysicalObject<void*> *pobj, bool yes) {}
   virtual void updatePhysicalObjects(double dt) {}
};

#endif
