// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL physics simulation classes
#ifndef GLVERTEX_PHYSICS_H
#define GLVERTEX_PHYSICS_H

#include "glvertex_object.h"

#ifndef HAVE_BULLET_PHYSICS
typedef lgl_NoPhysicalGeometryObject lgl_BulletGeometryObject;
typedef lgl_NoPhysicalSceneGraphObject lgl_BulletSceneGraphObject;
typedef lgl_NoPhysicalCubeObject lgl_BulletCubeObject;
typedef lgl_NoPhysicalSphereObject lgl_BulletSphereObject;
typedef lgl_NoPhysicalObjects lgl_BulletObjects;
#else

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

//! bullet physics engine
class lgl_BulletPhysics
{
public:

   //! ctor
   lgl_BulletPhysics()
   {
      // build the broadphase
      broadphase = new btDbvtBroadphase();

      // set up the collision configuration and dispatcher
      collisionConfiguration = new btDefaultCollisionConfiguration();
      dispatcher = new btCollisionDispatcher(collisionConfiguration);

      // the actual physics solver
      solver = new btSequentialImpulseConstraintSolver;

      // the world
      dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

      // the ground
      groundRigidBody = NULL;
   }

   ~lgl_BulletPhysics()
   {
      if (groundRigidBody)
      {
         dynamicsWorld->removeRigidBody(groundRigidBody);
         deleteBulletData(groundRigidBody);
      }

      delete dynamicsWorld;
      delete solver;
      delete broadphase;
      delete dispatcher;
      delete collisionConfiguration;
   }

   //! set ground plane
   void setBulletGroundPlane(vec3 o, vec3 n, double restitution = 1.0)
   {
      if (groundRigidBody)
      {
         dynamicsWorld->removeRigidBody(groundRigidBody);
         deleteBulletData(groundRigidBody);
         groundRigidBody = NULL;
      }

      if (n.norm() > 0)
      {
         btCollisionShape *groundShape = new btStaticPlaneShape(btVector3(n.x, n.y, n.z), 0);
         btDefaultMotionState *groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(o.x, o.y, o.z)));

         btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
         groundRigidBody = new btRigidBody(groundRigidBodyCI);
         groundRigidBody->setRestitution(restitution);

         dynamicsWorld->addRigidBody(groundRigidBody);
      }
   }

   //! set gravity
   void setBulletGravity(vec3 g)
   {
      dynamicsWorld->setGravity(btVector3(g.x, g.y, g.z));
   }

   //! create a rigid body with a box collision shape for a bullet object
   static btRigidBody *createBulletBox(lgl_PhysicalObject<btRigidBody> *pobj)
   {
      vec3 ext2 = 0.5*pobj->getExtent();
      vec3 center = pobj->getCenter();
      btTransform offset(btQuaternion(0, 0, 0, 1), btVector3(-center.x, -center.y, -center.z));

      btTransform transform;
      transform.setFromOpenGLMatrix((const float *)mat4f(mat4::translate(center) * pobj->getTransformation()));

      btCollisionShape *shape = new btBoxShape(btVector3(ext2.x, ext2.y, ext2.z));
      btDefaultMotionState *motionState = new btDefaultMotionState(transform, offset);

      btScalar mass = pobj->getPhysicalMass();
      btVector3 inertia(0, 0, 0);
      shape->calculateLocalInertia(mass, inertia);

      btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, shape, inertia);
      btRigidBody *body = new btRigidBody(rigidBodyCI);
      body->setRestitution(pobj->getPhysicalRestitution());
      body->setDamping(0.01, 0.05);
      body->setSleepingThresholds(0.25, 0.25);

      return(body);
   }

   //! create a rigid body with a sphere collision shape for a bullet object
   static btRigidBody *createBulletSphere(lgl_PhysicalObject<btRigidBody> *pobj)
   {
      double radius = pobj->getRadius();
      vec3 center = pobj->getCenter();
      btTransform offset(btQuaternion(0, 0, 0, 1), btVector3(-center.x, -center.y, -center.z));

      btTransform transform;
      transform.setFromOpenGLMatrix((const float *)mat4f(mat4::translate(center) * pobj->getTransformation()));

      btCollisionShape *shape = new btSphereShape(radius);
      btDefaultMotionState *motionState = new btDefaultMotionState(transform, offset);

      btScalar mass = pobj->getPhysicalMass();
      btVector3 inertia(0, 0, 0);
      shape->calculateLocalInertia(mass, inertia);

      btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, shape, inertia);
      btRigidBody *body = new btRigidBody(rigidBodyCI);
      body->setRestitution(pobj->getPhysicalRestitution());
      body->setDamping(0.01, 0.25);
      body->setSleepingThresholds(0.25, 0.25);

      return(body);
   }

   //! create a rigid body with an ellipsoid collision shape for a bullet object
   static btRigidBody *createBulletEllipsoid(lgl_PhysicalObject<btRigidBody> *pobj)
   {
      vec3 ext = pobj->getExtent();
      vec3 center = pobj->getCenter();
      btTransform offset(btQuaternion(0, 0, 0, 1), btVector3(-center.x, -center.y, -center.z));

      btTransform transform;
      transform.setFromOpenGLMatrix((const float *)mat4f(mat4::translate(center) * pobj->getTransformation()));

      btVector3 pos(0, 0, 0);
      btScalar rad = 0.5;
      btCollisionShape *shape = new btMultiSphereShape(&pos, &rad, 1);
      shape->setLocalScaling(btVector3(ext.x, ext.y, ext.z));
      shape->setMargin(0);

      btDefaultMotionState *motionState = new btDefaultMotionState(transform, offset);

      btScalar mass = pobj->getPhysicalMass();
      btVector3 inertia(0, 0, 0);
      shape->calculateLocalInertia(mass, inertia);

      btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, shape, inertia);
      btRigidBody *body = new btRigidBody(rigidBodyCI);
      body->setRestitution(pobj->getPhysicalRestitution());
      body->setDamping(0.01, 0.05);
      body->setSleepingThresholds(0.25, 0.25);

      return(body);
   }

   //! create a rigid body with a convex hull shape for a bullet object
   static btRigidBody *createBulletHull(lgl_PhysicalObject<btRigidBody> *pobj, std::vector<vec3f> &points)
   {
      vec3 center = pobj->getCenter();
      btTransform offset(btQuaternion(0, 0, 0, 1), btVector3(-center.x, -center.y, -center.z));

      btTransform transform;
      transform.setFromOpenGLMatrix((const float *)mat4f(mat4::translate(center) * pobj->getTransformation()));

      btConvexHullShape *shape = new btConvexHullShape();
      for (unsigned int i=0; i<points.size(); i++) shape->addPoint(btVector3(points[i].x, points[i].y, points[i].z));
      shape->setMargin(0);

      btDefaultMotionState *motionState = new btDefaultMotionState(transform, offset);

      btScalar mass = pobj->getPhysicalMass();
      btVector3 inertia(0, 0, 0);
      shape->calculateLocalInertia(mass, inertia);

      btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, shape, inertia);
      btRigidBody *body = new btRigidBody(rigidBodyCI);
      body->setSleepingThresholds(0.25f, 0.25f);

      return(body);
   }

   //! create a rigid body with a simplified convex hull shape for a bullet object
   static btRigidBody *createSimplifiedBulletHull(lgl_PhysicalObject<btRigidBody> *pobj, std::vector<vec3f> &points)
   {
      vec3 center = pobj->getCenter();
      btTransform offset(btQuaternion(0, 0, 0, 1), btVector3(-center.x, -center.y, -center.z));

      btTransform transform;
      transform.setFromOpenGLMatrix((const float *)mat4f(mat4::translate(center) * pobj->getTransformation()));

      btConvexHullShape *shape = new btConvexHullShape();
      for (unsigned int i=0; i<points.size(); i++) shape->addPoint(btVector3(points[i].x, points[i].y, points[i].z));
      shape->setMargin(0);

      btShapeHull *hull = new btShapeHull(shape);
      hull->buildHull(shape->getMargin());
      delete shape;

      btConvexHullShape *simplified = new btConvexHullShape();
      for (int i=0; i<hull->numVertices(); i++) simplified->addPoint(hull->getVertexPointer()[i]);
      simplified->setMargin(0);
      delete hull;

      btDefaultMotionState *motionState = new btDefaultMotionState(transform, offset);

      btScalar mass = pobj->getPhysicalMass();
      btVector3 inertia(0, 0, 0);
      simplified->calculateLocalInertia(mass, inertia);

      btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, simplified, inertia);
      btRigidBody *body = new btRigidBody(rigidBodyCI);
      body->setSleepingThresholds(0.25f, 0.25f);

      return(body);
   }

   //! set the mass of a rigid body
   void setBulletMass(btRigidBody *body, btScalar mass)
   {
      btVector3 inertia(0, 0, 0);
      body->getCollisionShape()->calculateLocalInertia(mass, inertia);
      body->setMassProps(mass, inertia);
      body->activate();
   }

   //! set the linear and angular factor of a rigid body
   void setBulletFactor(btRigidBody *body, float factor)
   {
      if (factor == 0)
      {
        body->setLinearVelocity(btVector3(0, 0, 0));
        body->setAngularVelocity(btVector3(0, 0, 0));
        body->setLinearFactor(btVector3(0, 0, 0));
        body->setAngularFactor(btVector3(0, 0, 0));
        body->setActivationState(DISABLE_DEACTIVATION);
      }
      else
      {
         body->setLinearFactor(btVector3(factor, factor, factor));
         body->setAngularFactor(btVector3(factor, factor, factor));
         body->setActivationState(WANTS_DEACTIVATION);
      }
   }

   //! add a rigid body to the dynamics world
   void addBulletBody(btRigidBody *body)
   {
      dynamicsWorld->addRigidBody(body);
   }

   //! remove a rigid body from the dynamics world
   void removeBulletBody(btRigidBody *body)
   {
      dynamicsWorld->removeRigidBody(body);
   }

   //! update the state of all rigid bodies of the dynamics world
   void updateBullet(double dt)
   {
      dynamicsWorld->stepSimulation(dt, 3, 1.0/120);
   }

   //! get the actual transformation matrix of a bullet object from its rigid body state
   static mat4 getBulletTransformation(lgl_PhysicalObject<btRigidBody> *pobj)
   {
      if (btRigidBody *body=pobj->getPhysicalData())
      {
         btScalar matrix[16];
         body->getCenterOfMassTransform().getOpenGLMatrix(matrix);

         mat4f mtx;
         mtx.fromOpenGL(matrix);
         return(mtx);
      }

      return(pobj->getTransformation());
   }

   //! set the transformation matrix of a rigid body
   static void setBulletTransformation(lgl_PhysicalObject<btRigidBody> *pobj, const mat4 &m)
   {
      if (btRigidBody *body=pobj->getPhysicalData())
      {
         btTransform transform;
         transform.setFromOpenGLMatrix((const float *)mat4f(m));
         body->setWorldTransform(transform);
         body->setLinearVelocity(btVector3(0, 0, 0));
         body->setAngularVelocity(btVector3(0, 0, 0));
      }
   }

   //! get the actual velocity of a bullet object from its rigid body state
   static vec3 getBulletVelocity(lgl_PhysicalObject<btRigidBody> *pobj)
   {
      if (btRigidBody *body=pobj->getPhysicalData())
      {
         btVector3 velocity = body->getLinearVelocity();
         return(vec3(velocity.x(), velocity.y(), velocity.z()));
      }

      return(pobj->getVelocity());
   }

   //! set the velocity of a rigid body
   static void setBulletVelocity(lgl_PhysicalObject<btRigidBody> *pobj, const vec3 &v)
   {
      if (btRigidBody *body=pobj->getPhysicalData())
      {
         btVector3 velocity(v.x, v.y, v.z);
         body->setLinearVelocity(velocity);
      }
   }

   //! apply force to point relative to center of mass
   static void applyBulletForce(lgl_PhysicalObject<btRigidBody> *pobj, vec3 force, vec3 rel_pos = vec3(0,0,0))
   {
      if (btRigidBody *body=pobj->getPhysicalData())
      {
         body->activate();
         body->applyForce(btVector3(force.x, force.y, force.z), btVector3(rel_pos.x, rel_pos.y, rel_pos.z));
      }
   }

   //! apply linear impulse to center of mass
   static void applyBulletImpulse(lgl_PhysicalObject<btRigidBody> *pobj, vec3 impulse)
   {
      if (btRigidBody *body=pobj->getPhysicalData())
      {
         body->activate();
         body->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
      }
   }

   //! apply angular impulse to center of mass
   static void applyBulletTorque(lgl_PhysicalObject<btRigidBody> *pobj, vec3 torque)
   {
      if (btRigidBody *body=pobj->getPhysicalData())
      {
         body->activate();
         body->applyTorqueImpulse(btVector3(torque.x, torque.y, torque.z));
      }
   }

   //! set user pointer
   static void setUserPointer(btRigidBody *body, lgl_PhysicalObject<btRigidBody> *pobj)
   {
      body->getCollisionShape()->setUserPointer(pobj);
   }

   //! get user pointer
   static lgl_PhysicalObject<btRigidBody> *getUserPointer(const btRigidBody *body)
   {
      return((lgl_PhysicalObject<btRigidBody> *)body->getCollisionShape()->getUserPointer());
   }

   //! shoot a ray at the scene and return the closest hit
   lgl_PhysicalObject<btRigidBody> *rayTest(const vec3 &from, vec3 &to)
   {
      btVector3 f(from.x, from.y, from.z);
      btVector3 t(to.x, to.y, to.z);
      btCollisionWorld::ClosestRayResultCallback rayCallback(f, t);
      dynamicsWorld->rayTest(f, t, rayCallback);

      if (rayCallback.hasHit()) {
         const btRigidBody *body = btRigidBody::upcast(rayCallback.m_collisionObject);
         btVector3 hit = rayCallback.m_hitPointWorld;
         to = vec3(hit.x(), hit.y(), hit.z());
         return(getUserPointer(body));
      }

      return(NULL);
   }

   //! delete all data associated with a bullet object
   static void deleteBulletData(lgl_PhysicalObject<btRigidBody> *pobj)
   {
      if (btRigidBody *data=pobj->getPhysicalData())
         deleteBulletData(data);
   }

   //! delete all data associated with a rigid body
   static void deleteBulletData(btRigidBody *data)
   {
      delete data->getMotionState();
      delete data->getCollisionShape();
      delete data;
   }

protected:

   btDiscreteDynamicsWorld *dynamicsWorld;
   btSequentialImpulseConstraintSolver *solver;
   btBroadphaseInterface *broadphase;
   btCollisionDispatcher *dispatcher;
   btDefaultCollisionConfiguration *collisionConfiguration;

   btRigidBody *groundRigidBody;
};

//! bullet geometry object
class lgl_BulletGeometryObject: public lgl_PhysicalGeometryObject<btRigidBody>
{
public:

   //! ctor
   lgl_BulletGeometryObject(lglVBO *vbo = NULL)
      : lgl_PhysicalGeometryObject<btRigidBody>(vbo)
   {
      lgl_BulletPhysics::deleteBulletData(this);
   }

protected:

   virtual btRigidBody *createPhysicalData()
   {
      btRigidBody *body = lgl_BulletPhysics::createBulletBox(this);
      lgl_BulletPhysics::setUserPointer(body, this);
      return(body);
   }

   virtual mat4 getPhysicalTransformation()
   {
      return(lgl_BulletPhysics::getBulletTransformation(this));
   }

   virtual void transformationChanged(const mat4 &m)
   {
      lgl_BulletPhysics::setBulletTransformation(this, m);
   }

   virtual vec3 getPhysicalVelocity()
   {
      return(lgl_BulletPhysics::getBulletVelocity(this));
   }

   virtual void velocityChanged(const vec3 &v)
   {
      lgl_BulletPhysics::setBulletVelocity(this, v);
   }

   virtual void applyForce(vec3 force, vec3 rel_pos = vec3(0,0,0))
   {
      lgl_BulletPhysics::applyBulletForce(this, force, rel_pos);
   }

   virtual void applyImpulse(vec3 impulse)
   {
      lgl_BulletPhysics::applyBulletImpulse(this, impulse);
   }

   virtual void applyTorque(vec3 torque)
   {
      lgl_BulletPhysics::applyBulletTorque(this, torque);
   }

};

//! bullet scene graph object
class lgl_BulletSceneGraphObject: public lgl_PhysicalSceneGraphObject<btRigidBody>
{
public:

   //! ctor
   lgl_BulletSceneGraphObject(lgl_Node *root = NULL)
      : lgl_PhysicalSceneGraphObject<btRigidBody>(root)
   {
      lgl_BulletPhysics::deleteBulletData(this);
   }

protected:

   virtual btRigidBody *createPhysicalData()
   {
      btRigidBody *body = lgl_BulletPhysics::createBulletBox(this);
      lgl_BulletPhysics::setUserPointer(body, this);
      return(body);
   }

   virtual mat4 getPhysicalTransformation()
   {
      return(lgl_BulletPhysics::getBulletTransformation(this));
   }

   virtual void transformationChanged(const mat4 &m)
   {
      lgl_BulletPhysics::setBulletTransformation(this, m);
   }

   virtual vec3 getPhysicalVelocity()
   {
      return(lgl_BulletPhysics::getBulletVelocity(this));
   }

   virtual void velocityChanged(const vec3 &v)
   {
      lgl_BulletPhysics::setBulletVelocity(this, v);
   }

   virtual void applyForce(vec3 force, vec3 rel_pos = vec3(0,0,0))
   {
      lgl_BulletPhysics::applyBulletForce(this, force, rel_pos);
   }

   virtual void applyImpulse(vec3 impulse)
   {
      lgl_BulletPhysics::applyBulletImpulse(this, impulse);
   }

   virtual void applyTorque(vec3 torque)
   {
      lgl_BulletPhysics::applyBulletTorque(this, torque);
   }

};

//! bullet cube object
class lgl_BulletCubeObject: public lgl_PhysicalCubeObject<btRigidBody>
{
public:

   //! ctor
   lgl_BulletCubeObject()
      : lgl_PhysicalCubeObject<btRigidBody>()
   {
      lgl_BulletPhysics::deleteBulletData(this);
   }

protected:

   virtual btRigidBody *createPhysicalData()
   {
      btRigidBody *body = lgl_BulletPhysics::createBulletBox(this);
      lgl_BulletPhysics::setUserPointer(body, this);
      return(body);
   }

   virtual mat4 getPhysicalTransformation()
   {
      return(lgl_BulletPhysics::getBulletTransformation(this));
   }

   virtual void transformationChanged(const mat4 &m)
   {
      lgl_BulletPhysics::setBulletTransformation(this, m);
   }

   virtual vec3 getPhysicalVelocity()
   {
      return(lgl_BulletPhysics::getBulletVelocity(this));
   }

   virtual void velocityChanged(const vec3 &v)
   {
      lgl_BulletPhysics::setBulletVelocity(this, v);
   }

   virtual void applyForce(vec3 force, vec3 rel_pos = vec3(0,0,0))
   {
      lgl_BulletPhysics::applyBulletForce(this, force, rel_pos);
   }

   virtual void applyImpulse(vec3 impulse)
   {
      lgl_BulletPhysics::applyBulletImpulse(this, impulse);
   }

   virtual void applyTorque(vec3 torque)
   {
      lgl_BulletPhysics::applyBulletTorque(this, torque);
   }

};

//! bullet sphere object
class lgl_BulletSphereObject: public lgl_PhysicalSphereObject<btRigidBody>
{
public:

   //! ctor
   lgl_BulletSphereObject()
      : lgl_PhysicalSphereObject<btRigidBody>()
   {
      lgl_BulletPhysics::deleteBulletData(this);
   }

protected:

   virtual btRigidBody *createPhysicalData()
   {
      btRigidBody *body;

      if (hasUniformScale())
         body = lgl_BulletPhysics::createBulletSphere(this);
      else
         body = lgl_BulletPhysics::createBulletEllipsoid(this);

      lgl_BulletPhysics::setUserPointer(body, this);

      return(body);
   }

   virtual mat4 getPhysicalTransformation()
   {
      return(lgl_BulletPhysics::getBulletTransformation(this));
   }

   virtual void transformationChanged(const mat4 &m)
   {
      lgl_BulletPhysics::setBulletTransformation(this, m);
   }

   virtual vec3 getPhysicalVelocity()
   {
      return(lgl_BulletPhysics::getBulletVelocity(this));
   }

   virtual void velocityChanged(const vec3 &v)
   {
      lgl_BulletPhysics::setBulletVelocity(this, v);
   }

   virtual void applyForce(vec3 force, vec3 rel_pos = vec3(0,0,0))
   {
      lgl_BulletPhysics::applyBulletForce(this, force, rel_pos);
   }

   virtual void applyImpulse(vec3 impulse)
   {
      lgl_BulletPhysics::applyBulletImpulse(this, impulse);
   }

   virtual void applyTorque(vec3 torque)
   {
      lgl_BulletPhysics::applyBulletTorque(this, torque);
   }

};

//! container class for multiple bullet objects
class lgl_BulletObjects: public lgl_BulletPhysics, public lgl_PhysicalObjects<btRigidBody>
{
public:

   typedef lgl_PhysicalObjects<btRigidBody>::iterator iterator;

   //! ctor
   lgl_BulletObjects()
      : lgl_BulletPhysics(),
        lgl_PhysicalObjects<btRigidBody>()
   {
      setBulletGroundPlane(ground_o, ground_n, ground_r);
   }

   virtual ~lgl_BulletObjects()
   {}

   //! set ground plane
   void setGroundPlane(vec3 o, vec3 n, double r = 1.0)
   {
      lgl_PhysicalObjects<btRigidBody>::setGroundPlane(o, n, r);
      setBulletGroundPlane(ground_o, ground_n, ground_r);
   }

   //! set gravity
   void setGravity(double g)
   {
      lgl_PhysicalObjects<btRigidBody>::setGravity(g);
      setBulletGravity(ground_n*ground_g);
   }

   //! begin iterator
   iterator begin()
   {
      return(lgl_PhysicalObjects<btRigidBody>::begin());
   }

   //! end iterator
   iterator end()
   {
      return(lgl_PhysicalObjects<btRigidBody>::end());
   }

protected:

   virtual void objectAdded(lgl_PhysicalObject<btRigidBody> *pobj)
   {
      if (btRigidBody *body=pobj->getPhysicalData())
         addBulletBody(body);
   }

   virtual void objectRemoved(lgl_PhysicalObject<btRigidBody> *pobj)
   {
      if (btRigidBody *body=pobj->getPhysicalData())
         removeBulletBody(body);
   }

   virtual void objectDeactivated(lgl_PhysicalObject<btRigidBody> *pobj, bool yes)
   {
      if (btRigidBody *body=pobj->getPhysicalData())
         setBulletFactor(body, yes?0:1);
   }

   virtual void updatePhysicalObjects(double dt)
   {
      updateBullet(dt);
   }

};

#endif

#endif
