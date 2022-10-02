// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL scene graph node classes
#ifndef GLVERTEX_NODES_H
#define GLVERTEX_NODES_H

#include "vector"

#include "glvertex.h"
#include "glvertex_cam.h"
#include "glvertex_objformat.h"

//! scene graph node (base class)
//!
//! a scene graph node provides virtual render() and update() methods to be reimplemented in derived classes
//! * the default implementation just calls the corresponding methods of its child nodes
//! * child nodes can be linked with the add() method
//! * the add() method allows chaining (by means of a function template)
//! * added nodes are reference-counted
//! * the graph must not contain a cycle
class lgl_Node
{
public:

   //! default ctor
   lgl_Node(const std::string &id = "")
      : id_(id),
        enabled_(true),
        hidden_(false),
        children_(),
        refcount_(0),
        init_(false),
        updated_(false),
        visited_(false)
   {}

   //! copy ctor
   lgl_Node(const lgl_Node &node)
      : id_(""),
        enabled_(true),
        hidden_(false),
        children_(node.children_),
        refcount_(0),
        init_(false),
        updated_(false),
        visited_(false)
   {
      for (unsigned int i=0; i<children_.size(); i++)
         children_[i]->refcount_++;
   }

   //! custom ctor
   lgl_Node(lgl_Node *node, const std::string &id = "")
      : id_(id),
        enabled_(true),
        hidden_(false),
        children_(),
        refcount_(0),
        init_(false),
        updated_(false),
        visited_(false)
   {
      if (node)
         node->add(this);
   }

   //! dtor
   virtual ~lgl_Node()
   {
      visited_ = true;

      for (unsigned int i=0; i<children_.size(); i++)
      {
         children_[i]->refcount_--;
         if (children_[i]->refcount_ == 0)
            if (!children_[i]->visited_) delete children_[i];
      }

      if (refcount_ != 0)
         lglError("deleting node with non-zero reference count");
   }

   //! get the class id
   virtual std::string getClassId() const {return("");}

   //! get the node identifier
   std::string getId() const
   {
      return(id_);
   }

   //! set the node identifier
   void setId(const std::string &id = "")
   {
      id_ = id;
   }

   //! get the alternate node identifier
   std::string getAltId() const
   {
      if (id_ != "") return(id_);
      return(alt_);
   }

   //! set the alternate node identifier
   void setAltId(const std::string &id = "")
   {
      alt_ = id;
   }

   //! get the number of children of the node
   unsigned int children() const
   {
      return(children_.size());
   }

   //! is the node a leaf?
   bool leaf() const
   {
      return(children_.size()==0);
   }

   //! get the number of references to the node
   unsigned int references() const
   {
      return(refcount_);
   }

   //! is the node shared?
   bool shared() const
   {
      return(refcount_>1);
   }

   //! get a child node
   lgl_Node *get(unsigned int i = 0) const
   {
      if (i < children_.size())
         return(children_[i]);
      else
         return(NULL);
   }

protected:

   //! add a child to the node
   lgl_Node *add_node(lgl_Node *node)
   {
      children_.push_back(node);
      node->refcount_++;
      return(node);
   }

public:

   //! add a child node with a specific type
   template <class T>
   T *add(T *node)
   {
      if (lgl_Node *child=dynamic_cast<lgl_Node*>(node))
      {
         add_node(child);
         return(node);
      }

      return(NULL);
   }

   //! remove a child from the node
   void remove(unsigned int i = 0)
   {
      unsigned int n = children_.size();

      if (i < n)
      {
         children_[i]->refcount_--;
         if (children_[i]->refcount_ == 0) delete children_[i];

         if (n-- > 1)
            children_[i] = children_[n];

         children_.resize(n);
      }
   }

   //! replace a child of the node
   void replace(lgl_Node *node, unsigned int i = 0)
   {
      if (i < children_.size())
      {
         children_[i]->refcount_--;
         if (children_[i]->refcount_ == 0) delete children_[i];

         children_[i] = node;
         node->refcount_++;
      }
   }

   //! unlink a child and take ownership
   lgl_Node *take(unsigned int i = 0)
   {
      lgl_Node *node = NULL;
      unsigned int n = children_.size();

      if (i < n)
      {
         children_[i]->refcount_--;
         if (children_[i]->refcount_ == 0) node = children_[i];

         if (n-- > 1)
            children_[i] = children_[n];

         children_.resize(n);
      }

      return(node);
   }

   //! add the links of a node
   void link(lgl_Node *node)
   {
      for (unsigned int i=0; i<node->children(); i++)
         add(node->get(i));
   }

   //! remove the links of the node
   void unlink()
   {
      for (unsigned int i=0; i<children_.size(); i++)
      {
         children_[i]->refcount_--;
         if (children_[i]->refcount_ == 0) delete children_[i];
      }

      children_.clear();
   }

   //! check for graph cycles
   virtual bool checkForCycles()
   {
      if (visited_)
         return(true);

      visited_ = true;

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            if (children_[i]->checkForCycles())
            {
               visited_ = false;
               return(true);
            }

      visited_ = false;

      return(false);
   }

   //! is the node enabled?
   bool enabled() const
   {
      return(enabled_);
   }

   //! is the node disabled?
   bool disabled() const
   {
      return(!enabled_);
   }

   //! enable the node
   void enable(bool yes = true)
   {
      enabled_ = yes;
   }

   //! disable the node
   void disable(bool yes = true)
   {
      enabled_ = !yes;
   }

   //! toggle the node
   void toggle()
   {
      enabled_ = !enabled_;
   }

   //! is the node shown?
   bool shown() const
   {
      return(!hidden_);
   }

   //! is the node hidden?
   bool hidden() const
   {
      return(hidden_);
   }

   //! show the node and its children
   void show(bool yes = true)
   {
      hidden_ = !yes;
   }

   //! hide the node and its children
   void hide(bool yes = true)
   {
      hidden_ = yes;
   }

   //! find a node with a specific identifier
   virtual lgl_Node *find(const std::string &id)
   {
      if (id == id_)
         return(this);

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            if (lgl_Node *node=children_[i]->find(id))
               return(node);

      return(NULL);
   }

   //! find a node with a specific type
   template <class T>
   T *find()
   {
      if (T *node=dynamic_cast<T*>(this))
         return(node);

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            if (T *node=children_[i]->find<T>())
               return(node);

      return(NULL);
   }

   //! find a node with a specific identifier and type
   template <class T>
   T *find(const std::string &id)
   {
      return(dynamic_cast<T*>(find(id)));
   }

   //! render and update the entire scene subgraph
   void renderSceneGraph(double dt = 0)
   {
      preupdateSceneGraph(dt);
      drawSceneGraph();
      updateSceneGraph(dt);
   }

   //! draw the entire scene subgraph without updating
   void drawSceneGraph(vec4 color = vec4(1))
   {
      lglDepthTest(true);
      lglBackFaceCulling(false);
      lglColor(color);

      lglMatrixMode(LGL_TEXTURE);
      lglPushMatrix();
      lglMatrixMode(LGL_MODELVIEW);
      lglPushMatrix();

      pre_render();
      render();
      post_render();

      lglMatrixMode(LGL_TEXTURE);
      lglPopMatrix();
      lglMatrixMode(LGL_MODELVIEW);
      lglPopMatrix();
   }

   //! draw the entire scene subgraph without updating
   void drawSceneGraph(const mat4 &m)
   {
      lglDepthTest(true);
      lglBackFaceCulling(false);
      lglColor(1);

      lglMatrixMode(LGL_TEXTURE);
      lglPushMatrix();
      lglMatrixMode(LGL_MODELVIEW);
      lglPushMatrix();

      pre_render();
      lglMultMatrix(m);
      render();
      post_render();

      lglMatrixMode(LGL_TEXTURE);
      lglPopMatrix();
      lglMatrixMode(LGL_MODELVIEW);
      lglPopMatrix();
   }

   //! pre-update the entire scene subgraph
   void preupdateSceneGraph(double dt)
   {
      pre_update(dt);
   }

   //! update the entire scene subgraph
   void updateSceneGraph(double dt)
   {
      update(dt);
   }

   //! get the bounding box of the entire subgraph
   //! * the bounding box is invalid if a maximum component is less than the minimum component
   virtual void getBoundingBox(vec3 &bboxmin, vec3 &bboxmax)
   {
      pre_init();

      vec3 bbmin(DBL_MAX), bbmax(-DBL_MAX);

      if (!hidden_)
      {
         for (unsigned int i=0; i<children_.size(); i++)
            if (follow(i))
            {
               vec3 bmin, bmax;
               children_[i]->getBoundingBox(bmin, bmax);

               if (bmin.x <= bmax.x &&
                   bmin.y <= bmax.y &&
                   bmin.z <= bmax.z)
               {
                  growBoundingBox(bbmin, bbmax, bmin);
                  growBoundingBox(bbmin, bbmax, bmax);
               }
            }

         updateBoundingBox(bbmin, bbmax);
      }

      bboxmin = bbmin;
      bboxmax = bbmax;
   }

protected:

   static void growBoundingBox(vec3 &bboxmin, vec3 &bboxmax, const vec3 &p)
   {
      if (p.x < bboxmin.x) bboxmin.x = p.x;
      if (p.x > bboxmax.x) bboxmax.x = p.x;
      if (p.y < bboxmin.y) bboxmin.y = p.y;
      if (p.y > bboxmax.y) bboxmax.y = p.y;
      if (p.z < bboxmin.z) bboxmin.z = p.z;
      if (p.z > bboxmax.z) bboxmax.z = p.z;
   }

   virtual void updateBoundingBox(vec3 &bboxmin, vec3 &bboxmax) const {}

public:

   //! get the bounding sphere of the entire subgraph
   //! * returns the radius of the bounding sphere
   //! * the bounding sphere is invalid if the radius is zero
   double getBoundingSphere(vec3 &center)
   {
      vec3 bbmin, bbmax;
      getBoundingBox(bbmin, bbmax);

      if (bbmin.x <= bbmax.x &&
          bbmin.y <= bbmax.y &&
          bbmin.z <= bbmax.z)
      {
         center = 0.5*(bbmax+bbmin);
         return((0.5*(bbmax-bbmin)).length());
      }

      return(0);
   }

   //! get the bounding box center of the entire subgraph
   vec3 getCenter()
   {
      vec3 bbmin, bbmax;
      getBoundingBox(bbmin, bbmax);

      if (bbmin.x <= bbmax.x &&
          bbmin.y <= bbmax.y &&
          bbmin.z <= bbmax.z)
         return(0.5*(bbmax+bbmin));

      return(0);
   }

   //! get the bounding box extent of the entire subgraph
   vec3 getExtent()
   {
      vec3 bbmin, bbmax;
      getBoundingBox(bbmin, bbmax);

      if (bbmin.x <= bbmax.x &&
          bbmin.y <= bbmax.y &&
          bbmin.z <= bbmax.z)
         return(bbmax-bbmin);

      return(vec3(0,0,0));
   }

   //! get the bounding sphere radius of the entire subgraph
   //! * zero radius means that the subgraph does not contain valid geometry
   double getRadius()
   {
      vec3 bbmin, bbmax;
      getBoundingBox(bbmin, bbmax);

      if (bbmin.x <= bbmax.x &&
          bbmin.y <= bbmax.y &&
          bbmin.z <= bbmax.z)
         return((0.5*(bbmax-bbmin)).length());

      return(0);
   }

   //! get the bounding sphere norm of the entire subgraph
   //! * zero norm means that the subgraph does not contain valid geometry
   double getNorm()
   {
      vec3 bbmin, bbmax;
      getBoundingBox(bbmin, bbmax);

      if (bbmin.x <= bbmax.x &&
          bbmin.y <= bbmax.y &&
          bbmin.z <= bbmax.z)
         return((0.5*(bbmax-bbmin)).norm());

      return(0);
   }

   // return a copy of the transformed vertex coordinates of the entire subgraph
   //! * this is a computationally expensive operation
   virtual std::vector<vec4> getVertexCoordinates(mat4 trans = mat4())
   {
      pre_init();

      std::vector<vec4> geometry = get_vertices();

      trans = update_transform(trans);

      for (unsigned int i=0; i<geometry.size(); i++)
         geometry[i] = trans * geometry[i];

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
         {
            std::vector<vec4> geo = children_[i]->getVertexCoordinates(trans);
            geometry.insert(geometry.end(), geo.begin(), geo.end());
         }

      return(geometry);
   }

   //! get the exact bounding box of the entire subgraph
   //! * this is a computationally expensive operation
   void getExactBoundingBox(vec3 &bboxmin, vec3 &bboxmax)
   {
      vec3 bbmin(DBL_MAX), bbmax(-DBL_MAX);

      std::vector<vec4> geometry = getVertexCoordinates();

      for (unsigned int i=0; i<geometry.size(); i++)
         growBoundingBox(bbmin, bbmax, geometry[i]);

      bboxmin = bbmin;
      bboxmax = bbmax;
   }

   // save the colored and transformed vbos of the entire subgraph
   virtual bool saveSceneGraph(vec4 color, mat4 trans, FILE *file, int &index, int &nindex, int &tindex)
   {
      pre_init();

      bool ok = true;

      lglVBO *vbo = get_vbo();

      color = update_color(color);
      trans = update_transform(trans);

      if (vbo)
      {
         lglVBO copy;
         vbo->lglAppendVerticesTo(&copy);
         if (!vbo->lglAppliedColoring()) copy.lglApplyColor(color);
         copy.lglModel(trans);
         copy.lglApplyModelMatrix();

         fprintf(file, "#");
         if (vbo->getName() != "") fprintf(file, " name=%s", vbo->getName().c_str());
         fprintf(file, " vertices=%d", vbo->lglGetVertexCount());
         fprintf(file, " primitives=%d\n", vbo->lglGetPrimitiveCount());

         if (!lglSaveObjInto(&copy, file, index, nindex, tindex)) ok = false;
      }

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            if (!children_[i]->saveSceneGraph(color, trans, file, index, nindex, tindex)) ok = false;

      return(ok);
   }

   //! pick the entire scene subgraph
   lglVBO *pickSceneGraph(vec3 origin, vec3 direction, double mindist = 0)
   {
      pre_init();

      lglBeginRayCast(origin, direction, mindist);
      pre_pick();
      pick();
      post_pick();
      lglVBO *vbo = dynamic_cast<lglVBO*>(lglEndRayCast());

      return(vbo);
   }

   //! check the entire subgraph whether or not it contains semi-transparent geometry
   bool hasTransparency()
   {
      if (transparency()) return(true);

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            if (children_[i]->hasTransparency())
               return(true);

      return(false);
   }

   //! count all nodes (count multiply linked nodes)
   virtual unsigned int countAll()
   {
      unsigned int count = 1;

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            count += children_[i]->countAll();

      return(count);
   }

   //! count all nodes (count multiply linked nodes once)
   virtual unsigned int countAllOnce()
   {
      unsigned int count;

      count = countOnce();
      cleanAll();

      return(count);
   }

   //! count all primitives
   virtual unsigned int countAllPrimitives()
   {
      pre_init();

      unsigned int count = 0;

      if (lglVBO *vbo = get_vbo())
         count += vbo->lglGetPrimitiveCount();

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            count += children_[i]->countAllPrimitives();

      return(count);
   }

   //! count all vertices
   unsigned int countAllVertices()
   {
      pre_init();

      unsigned int count = 0;

      if (lglVBO *vbo = get_vbo())
         count += vbo->lglGetVertexCount();

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            count += children_[i]->countAllVertices();

      return(count);
   }

protected:

   virtual void cleanAll()
   {
      visited_ = false;

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            children_[i]->cleanAll();
   }

   virtual unsigned int countOnce()
   {
      if (visited_)
         return(0);

      visited_ = true;

      unsigned int count = 1;

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            count += children_[i]->countOnce();

      return(count);
   }

public:

   //! is the node stateless?
   virtual bool stateless() const
   {
      return(true);
   }

   //! is the node ordered?
   virtual bool ordered() const
   {
      return(false);
   }

   //! is the node convertable?
   bool convertable() const
   {
      return(!shared());
   }

   //! is the node cloneable?
   bool cloneable() const
   {
      return(leaf() && getId()=="");
   }

   //! is the node optimizable?
   bool optimizable() const
   {
      return(!shared() && getId()=="");
   }

   //! optimize all nodes
   virtual void optimizeAll();

protected:

   virtual lgl_Node *convert() {return(NULL);}
   virtual lgl_Node *clone() {return(NULL);}
   virtual bool optimize() {return(false);}
   virtual bool swappable() {return(false);}

public:

   //! restart all animations
   virtual void restartAll(double animation = 0)
   {
      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            children_[i]->restartAll(animation);
   }

   //! pause all animations
   virtual void pauseAll(bool yes = true)
   {
      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            children_[i]->pauseAll(yes);
   }

   //! speedup all animations
   virtual void speedupAll(double speedup = 1)
   {
      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            children_[i]->speedupAll(speedup);
   }

   //! finish all transitions
   virtual void finishAll()
   {
      finish();

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            children_[i]->finishAll();
   }

   //! get the scene graph GL singleton
   static GL *getGL()
   {
      static GL gl;
      return(&gl);
   }

   //! scene graph export modes
   enum lgl_export_enum
   {
      LGL_EXPORT_NODES,
      LGL_EXPORT_COMMANDS,
      LGL_EXPORT_GRAPH
   };

   //! export all nodes
   std::string exportAll(lgl_export_enum mode = LGL_EXPORT_NODES, int indentation = 0, int increment = 3);

   //! export graph nodes
   void exportNodes(std::string filename);

   //! export link graph
   void exportGraph(std::string filename);

   //! export graphics commands
   void exportCommands(std::string filename);

   //! save into OBJ file
   bool saveObj(std::string filename)
   {
      bool ok = false;

      FILE *file = fopen(filename.c_str(), "wb");

      if (file)
      {
         fprintf(file, "# glVertex OBJ File\n\n");

         int index = 1;
         int nindex = 1;
         int tindex = 1;

         if (saveSceneGraph(vec4(1,1,1), mat4(), file, index, nindex, tindex)) ok = true;

         fclose(file);
      }

      return(ok);
   }

protected:

   std::string id_, alt_;
   bool enabled_, hidden_;

   std::vector<lgl_Node*> children_;
   unsigned int refcount_;
   bool init_, updated_;

   virtual bool follow(unsigned int i) const
   {
      return(i < children_.size());
   }

   virtual void init() {}

   virtual void pre_init()
   {
      if (!init_)
      {
         init();
         init_ = true;
      }
   }

   virtual void pre_update(double dt)
   {
      if (!init_)
      {
         init();
         init_ = true;
      }

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            children_[i]->pre_update(dt);

      updated_ = false;
   }

   virtual void pre_render()
   {
      if (!init_)
      {
         init();
         init_ = true;
      }

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            children_[i]->pre_render();
   }

   //! reimplement this method to render a node
   virtual void render()
   {
      if (hidden_) return;

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            children_[i]->render();
   }

   virtual void render(unsigned int i)
   {
      if (hidden_) return;

      if (follow(i))
         children_[i]->render();
   }

   virtual void post_render()
   {
      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            children_[i]->post_render();
   }

   //! reimplement this method to update a node
   virtual void update(double dt)
   {
      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            if (!children_[i]->updated_)
               children_[i]->update(dt);

      updated_ = true;
   }

   virtual void update(unsigned int i, double dt)
   {
      if (follow(i))
         if (!children_[i]->updated_)
            children_[i]->update(dt);

      updated_ = true;
   }

   virtual void finish() {}

   virtual std::vector<vec4> get_vertices()
   {
      return(std::vector<vec4>());
   }

   virtual lglVBO *get_vbo() {return(NULL);}
   virtual vec4 update_color(vec4 c) {return(c);}
   virtual mat4 update_transform(mat4 m) {return(m);}

   virtual void pre_pick()
   {
      if (!init_)
      {
         init();
         init_ = true;
      }

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            children_[i]->pre_pick();
   }

   //! reimplement this method to pick a node
   virtual void pick()
   {
      if (hidden_) return;

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            children_[i]->pick();
   }

   virtual void pick(unsigned int i)
   {
      if (hidden_) return;

      if (follow(i))
         children_[i]->pick();
   }

   virtual void post_pick()
   {
      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            children_[i]->post_pick();
   }

   virtual bool transparency()
   {
      return(false);
   }

public:

   virtual std::string to_id(const std::string &classid, bool classname = true) const
   {
      std::stringstream s;

      if (classname)
         s << "lgl_" << classid << "Node";
      else
         s << lgl_string(classid).toLower();

      if (classname)
         if (getId()!="" || disabled() || hidden())
         {
            s << "(";

            if (getId() != "")
               s << "\"" << getId() << "\"";

            if (disabled())
            {
               if (getId() != "")
                  s << ", ";

               s << "disabled";
            }

            if (hidden())
            {
               if (getId() != "" || disabled())
                  s << ", ";

               s << "hidden";
            }

            s << ")";
         }

      return(s.str());
   }

   virtual std::string to_string(bool classname = true) const
   {
      return(to_id(getClassId(), classname));
   }

   virtual std::string to_label() const
   {
      return(getId());
   }

protected:

   bool visited_;

   void init_export(lgl_export_enum mode);
   void scan_export(unsigned int &id, lgl_export_enum mode);
   std::string pre_export(lgl_export_enum mode);
   std::string export_to_string(int indentation, int increment, lgl_export_enum mode);
   std::string post_export(lgl_export_enum mode);
   void clean_export(lgl_export_enum mode);

   friend class lgl_SubgraphNode;
};

inline void lgl_Node::optimizeAll()
{
#ifndef LGL_DO_NOT_OPTIMIZE

   // recursively optimize children
   for (unsigned int i=0; i<children_.size(); i++)
      if (follow(i))
         children_[i]->optimizeAll();

   // replace children by converting or cloning them
   for (unsigned int i=0; i<children_.size(); i++)
      if (follow(i))
      {
         lgl_Node *node = get(i);

         if (node->convertable())
         {
            if (lgl_Node *converted = node->convert())
            {
               converted->setId(node->getId());

               link(node);
               replace(converted, i);
            }
         }
         else if (node->cloneable())
         {
            if (lgl_Node *cloned = node->clone())
               replace(cloned, i);
         }
      }

   // optimize children by removing them
   for (unsigned int i=0; i<children_.size(); i++)
      if (follow(i))
      {
         lgl_Node *node = get(i);

         if (node->optimizable())
         {
            if (node->optimize())
            {
               link(node);
               remove(i);
            }
         }
      }

   // optimize children by swapping them
   for (unsigned int i=0; i<children_.size(); i++)
      if (follow(i))
      {
         lgl_Node *node = get(i);

         if (node->swappable())
            if (node->children() == 1)
               if (!node->shared() && !node->get()->shared())
               {
                  lgl_Node *node1 = take(i);
                  lgl_Node *node2 = node1->take();
                  node1->link(node2);
                  node2->unlink();
                  node2->add(node1);
                  add(node2);
               }
      }

   // optimize unordered children by merging stateless unshared leaf nodes
   if (!ordered())
   {
      int mergeable = 0;

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
         {
            lgl_Node *node = get(i);

            if (node->stateless() && node->leaf() && !node->shared())
               mergeable++;
         }

      if (mergeable > 1)
      {
         std::vector<lgl_Node*> trail;

         unsigned int i=0;
         while (i < children_.size())
         {
            if (follow(i))
            {
               lgl_Node *node = get(i);

               if (node->stateless() && node->leaf() && !node->shared())
               {
                  trail.push_back(take(i));
                  continue;
               }

               i++;
            }
         }

         if (trail.size() > 0)
         {
            lgl_Node *link = this;
            while (trail.size() > 0)
            {
               lgl_Node *node = trail.back();
               trail.pop_back();

               link->add(node);
               link = node;
            }

            optimizeAll();
         }
      }
   }

#endif
}

//! output operator
inline std::ostream& operator << (std::ostream &out, const lgl_Node &node)
   {return(out << node.to_string());}

//! controllable node (base class)
//!
//! a controllable node is the base class for nodes which support certain actions to be triggered by control nodes (lgl_ControlNode)
//! * a call of the control() method triggers a particular action such as enabling, disabling or hiding the node
//! * the mode of the action is determined with the setControlMode() method
//! * the boolean parameter of the control() method determines the type of the action being either a regular action (true) or its opposite action (false)
//! * if a controllable node is a child of a control node (lgl_ControlNode), a call of the control() method of the control node will be routed to all its controllable children
//! * all major node classes are derived from this base class
//! * it inherits reference-counting of linked children from the lgl_Node base class
class lgl_ControllableNode: public lgl_Node
{
public:

   //! control mode enum
   enum lgl_control_mode_enum
   {
      LGL_CONTROL_NONE,
      LGL_CONTROL_ON,
      LGL_CONTROL_OFF,
      LGL_CONTROL_ENABLE,
      LGL_CONTROL_DISABLE,
      LGL_CONTROL_INVERT,
      LGL_CONTROL_HIDE,
      LGL_CONTROL_SHOW,
      LGL_CONTROL_TOGGLE,
      LGL_CONTROL_RESTART_ALL,
      LGL_CONTROL_PAUSE_ALL
   };

   //! ctor
   lgl_ControllableNode(const std::string &id = "",
                        lgl_Node *node = NULL)
      : lgl_Node(node, id),
        control_mode_(LGL_CONTROL_NONE),
        delayed_signal_(false),
        delayed_override_(false)
   {}

   virtual ~lgl_ControllableNode() {}

   virtual std::string getClassId() const {return("Controllable");}

   //! control the node as specified by the control mode
   virtual void control(bool override = true)
   {
      switch (getControlMode())
      {
         case LGL_CONTROL_NONE: break;
         case LGL_CONTROL_ON: enable(); break;
         case LGL_CONTROL_OFF: disable(); break;
         case LGL_CONTROL_ENABLE: enable(override); break;
         case LGL_CONTROL_DISABLE: disable(override); break;
         case LGL_CONTROL_INVERT: if (override) disable(enabled()); break;
         case LGL_CONTROL_HIDE: hide(override); break;
         case LGL_CONTROL_SHOW: show(override); break;
         case LGL_CONTROL_TOGGLE: if (override) hide(shown()); break;
         case LGL_CONTROL_RESTART_ALL: if (override) restartAll(); break;
         case LGL_CONTROL_PAUSE_ALL: pauseAll(override); break;
      }
   }

   //! get the control mode
   lgl_control_mode_enum getControlMode() const
   {
      return(control_mode_);
   }

   //! set the control mode
   void setControlMode(lgl_control_mode_enum mode)
   {
      control_mode_ = mode;
   }

   virtual bool stateless() const
   {
      return(false);
   }

protected:

   lgl_control_mode_enum control_mode_;

   bool delayed_signal_;
   bool delayed_override_;

   virtual void pre_update(double dt)
   {
      if (dt != 0)
         if (delayed_signal_)
         {
            signal(delayed_override_);
            delayed_signal_ = false;
         }

      lgl_Node::pre_update(dt);
   }

   virtual void signal(bool override = true);

   void signal_delayed(bool override = true)
   {
      delayed_signal_ = true;
      delayed_override_ = override;
   }

public:

   virtual std::string to_id(const std::string &classid, bool classname = true) const
   {
      std::stringstream s;

      if (classname)
         s << lgl_Node::to_id(classid, classname);
      else
      {
         if (classid != lgl_ControllableNode::getClassId())
            s << lgl_Node::to_id(classid, classname);
      }

      if (classname)
         if (getControlMode() != LGL_CONTROL_NONE)
         {
            s << "(";

            switch (getControlMode())
            {
               case LGL_CONTROL_NONE: s << "none"; break;
               case LGL_CONTROL_ON: s << "on"; break;
               case LGL_CONTROL_OFF: s << "off"; break;
               case LGL_CONTROL_ENABLE: s << "enable"; break;
               case LGL_CONTROL_DISABLE: s << "disable"; break;
               case LGL_CONTROL_INVERT: s << "invert"; break;
               case LGL_CONTROL_HIDE: s << "hide"; break;
               case LGL_CONTROL_SHOW: s << "show"; break;
               case LGL_CONTROL_TOGGLE: s << "toggle"; break;
               case LGL_CONTROL_RESTART_ALL: s << "restart-all"; break;
               case LGL_CONTROL_PAUSE_ALL: s << "pause-all"; break;
            }

            s << ")";
         }

      return(s.str());
   }

   virtual std::string to_label() const
   {
      lgl_string label = lgl_Node::to_label();

      switch (getControlMode())
      {
         case LGL_CONTROL_ON: label += "\\non"; break;
         case LGL_CONTROL_OFF: label += "\\noff"; break;
         case LGL_CONTROL_ENABLE: label += "\\nenable"; break;
         case LGL_CONTROL_DISABLE: label += "\\ndisable"; break;
         case LGL_CONTROL_INVERT: label += "\\ninvert"; break;
         case LGL_CONTROL_HIDE: label += "\\nhide"; break;
         case LGL_CONTROL_SHOW: label += "\\nshow"; break;
         case LGL_CONTROL_TOGGLE: label += "\\ntoggle"; break;
         case LGL_CONTROL_RESTART_ALL: label += "\\nrestart-all"; break;
         case LGL_CONTROL_PAUSE_ALL: label += "\\npause-all"; break;
         default: break;
      }

      return(label.strip("\\n"));
   }

};

//! action node
//!
//! an action node triggers a specific action on linked child nodes
//! * a call of the control() method triggers a specific action on the set of linked children
//! * the action to be triggered is specified with the setActionMode() method
//! * an action node is a controllable node, so that it can be a child of a control node (lgl_ControlNode)
class lgl_ActionNode: public lgl_ControllableNode
{
public:

   //! action mode enum
   enum lgl_action_mode_enum
   {
      LGL_ACTION_NONE,
      LGL_ACTION_ON,
      LGL_ACTION_OFF,
      LGL_ACTION_ENABLE,
      LGL_ACTION_DISABLE,
      LGL_ACTION_HIDE,
      LGL_ACTION_SHOW,
      LGL_ACTION_PAUSE,
      LGL_ACTION_RESUME,
      LGL_ACTION_DIRECTION
   };

   //! ctor
   lgl_ActionNode(const std::string &id = "",
                  lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node)
   {}

   virtual ~lgl_ActionNode() {}

   virtual std::string getClassId() const {return("Action");}

   //! trigger the controllable child nodes
   virtual void control(bool override = true)
   {
      trigger_children(override);
   }

   //! get the action mode
   lgl_action_mode_enum getActionMode() const
   {
      return(action_mode_);
   }

   //! set the action mode
   void setActionMode(lgl_action_mode_enum mode)
   {
      action_mode_ = mode;
   }

protected:

   lgl_action_mode_enum action_mode_;

   virtual bool follow(unsigned int i) const
   {
      return(false);
   }

   virtual void trigger_children(bool override = true);

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_ControllableNode::to_string(classname);

      if (classname)
         if (getActionMode() != LGL_ACTION_NONE)
         {
            s << "(";

            switch (getActionMode())
            {
               case LGL_ACTION_NONE: s << "none"; break;
               case LGL_ACTION_ON: s << "on"; break;
               case LGL_ACTION_OFF: s << "off"; break;
               case LGL_ACTION_ENABLE: s << "enable"; break;
               case LGL_ACTION_DISABLE: s << "disable"; break;
               case LGL_ACTION_HIDE: s << "hide"; break;
               case LGL_ACTION_SHOW: s << "show"; break;
               case LGL_ACTION_PAUSE: s << "pause"; break;
               case LGL_ACTION_RESUME: s << "resume"; break;
               case LGL_ACTION_DIRECTION: s << "direction"; break;
            }

            s << ")";
         }

      return(s.str());
   }

   virtual std::string to_label() const
   {
      lgl_string label = lgl_ControllableNode::to_label();

      switch (getActionMode())
      {
         case LGL_ACTION_ON: label += "\\non"; break;
         case LGL_ACTION_OFF: label += "\\noff"; break;
         case LGL_ACTION_ENABLE: label += "\\nenable"; break;
         case LGL_ACTION_DISABLE: label += "\\ndisable"; break;
         case LGL_ACTION_HIDE: label += "\\nhide"; break;
         case LGL_ACTION_SHOW: label += "\\nshow"; break;
         case LGL_ACTION_PAUSE: label += "\\npause"; break;
         case LGL_ACTION_RESUME: label += "\\nresume"; break;
         case LGL_ACTION_DIRECTION: label += "\\ndirection"; break;
         default: break;
      }

      return(label.strip("\\n"));
   }

};

//! control node
//!
//! a control node triggers actions on linked child nodes
//! * a call of the control() method is propagated to the set of linked controllable children (lgl_ControllableNode)
//! * a particular action for a set of nodes can be triggered by adding the set of nodes to a control node, which propagates the action to its children
//! * a control node is by itself a controllable node, so that control chains can be constructed
//!  * the control chain may even contain a graph cycle for the purpose of feedback or control loops
//!  * the type of the triggered action can be modified on its path through the control chain
//!   * if the control mode is "on", the actual control type is modified to be true for all the controllable children
//!   * if the control mode is "off", the actual control type is modified to be false for all the controllable children
//!   * if the control mode is "invert", the actual control type is inverted and passed to all the controllable children
class lgl_ControlNode: public lgl_ControllableNode
{
public:

   //! ctor
   lgl_ControlNode(const std::string &id = "",
                   lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node),
        delay_(0), countdown_(0),
        override_(true)
   {}

   virtual ~lgl_ControlNode() {}

   virtual std::string getClassId() const {return("Control");}

   //! override the controllable child nodes
   virtual void control(bool override = true)
   {
      if (delay_ > 0)
      {
         countdown_ = delay_;
         override_ = override;
      }
      else
         control_children(override);
   }

   //! set the control delay
   void setControlDelay(double delay)
   {
      delay_ = delay;
   }

   //! get the control delay
   double getControlDelay() const
   {
      return(delay_);
   }

   //! finish pending control signal
   virtual void finish()
   {
      if (countdown_ > 0)
      {
         countdown_ = 0;
         control_children(override_);
      }
   }

protected:

   double delay_;
   double countdown_;
   bool override_;

   virtual bool follow(unsigned int i) const
   {
      if (i >= children_.size()) return(false);
      if (dynamic_cast<const lgl_ControlNode *>(get(i))) return(true);
      if (dynamic_cast<const lgl_ActionNode *>(get(i))) return(true);
      return(false);
   }

   virtual void control_children(bool override = true)
   {
      if (enabled_)
      {
         switch (getControlMode())
         {
            case LGL_CONTROL_NONE: break;
            case LGL_CONTROL_ON: override = true; break;
            case LGL_CONTROL_OFF: override = false; break;
            case LGL_CONTROL_ENABLE: override = true; break;
            case LGL_CONTROL_DISABLE: override = false; break;
            case LGL_CONTROL_INVERT: override = !override; break;
            default: return;
         }

         for (unsigned int i=0; i<children_.size(); i++)
         {
            if (lgl_ControlNode *ctrl=dynamic_cast<lgl_ControlNode*>(children_[i]))
               ctrl->control(override);
            else if (lgl_ControllableNode *ctrl=dynamic_cast<lgl_ControllableNode*>(children_[i]))
               control(ctrl, override);
         }
      }
   }

   //! reimplement to change control effect
   virtual void control(lgl_ControllableNode *ctrl, bool override)
   {
      ctrl->control(override);
   }

   virtual void pre_update(double dt)
   {
      if (dt == 0)
         finish();

      lgl_Node::pre_update(dt);
   }

   virtual void update(double dt)
   {
      if (countdown_ > 0)
      {
         countdown_ -= dt;

         if (countdown_ <= 0)
            control_children(override_);
      }

      lgl_Node::update(dt);
   }

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_ControllableNode::to_string(classname);

      if (classname)
         if (getControlDelay() != 0)
            s << "(" << getControlDelay() << ")";

      return(s.str());
   }

   virtual std::string to_label() const
   {
      lgl_string label = lgl_ControllableNode::to_label();

      if (getControlDelay() != 0)
         label += "\\ndelay=" + glslmath::to_string(getControlDelay());

      return(label.strip("\\n"));
   }

};

inline void lgl_ControllableNode::signal(bool override)
{
   for (unsigned int i=0; i<children_.size(); i++)
      if (lgl_ControlNode *ctrl=dynamic_cast<lgl_ControlNode*>(children_[i]))
         ctrl->control(override);
}

//! control barrier node
//!
//! a barrier node interrupts the propagation of actions in control chains
//! * if the barrier node is configured to block a particular type of action, those actions will not be propagated further to its children
class lgl_BarrierNode: public lgl_ControllableNode
{
public:

   //! ctor
   lgl_BarrierNode(const std::string &id = "",
                   lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node),
        restart_all_barrier_(false),
        pause_all_barrier_(false),
        resume_all_barrier_(false),
        speedup_all_barrier_(false)
   {}

   virtual ~lgl_BarrierNode() {}

   virtual std::string getClassId() const {return("Barrier");}

   //! get restart barrier
   bool getRestartBarrier() const
   {
      return(restart_all_barrier_);
   }

   //! set restart barrier
   void setRestartBarrier(bool yes = false)
   {
      restart_all_barrier_ = yes;
   }

   //! get pause barrier
   bool getPauseBarrier() const
   {
      return(pause_all_barrier_);
   }

   //! set pause barrier
   void setPauseBarrier(bool yes = false)
   {
      pause_all_barrier_ = yes;
   }

   //! get resume barrier
   bool getResumeBarrier() const
   {
      return(resume_all_barrier_);
   }

   //! set resume barrier
   void setResumeBarrier(bool yes = false)
   {
      resume_all_barrier_ = yes;
   }

   //! get speedup barrier
   bool getSpeedupBarrier() const
   {
      return(speedup_all_barrier_);
   }

   //! set speedup barrier
   void setSpeedupBarrier(bool yes = false)
   {
      speedup_all_barrier_ = yes;
   }

   virtual void restartAll(double animation = 0)
   {
      if (!enabled_ || !restart_all_barrier_)
         lgl_Node::restartAll(animation);
   }

   virtual void pauseAll(bool yes = true)
   {
      if (yes)
      {
         if (!enabled_ || !pause_all_barrier_)
            lgl_Node::pauseAll(true);
      }
      else
      {
         if (!enabled_ || !resume_all_barrier_)
            lgl_Node::pauseAll(false);
      }
   }

   virtual void speedupAll(double speedup = 1)
   {
      if (!enabled_ || !speedup_all_barrier_)
         lgl_Node::speedupAll(speedup);
   }

protected:

   bool restart_all_barrier_;
   bool pause_all_barrier_;
   bool resume_all_barrier_;
   bool speedup_all_barrier_;

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      if (classname)
      {
         s << lgl_ControllableNode::to_string(classname);

         s << "(";
         s << (getRestartBarrier()?"on":"off") << ", ";
         s << (getPauseBarrier()?"on":"off") << ", ";
         s << (getResumeBarrier()?"on":"off") << ", ";
         s << (getSpeedupBarrier()?"on":"off");
         s << ")";
      }
      else
      {
         if (getClassId() != lgl_BarrierNode::getClassId())
            s << lgl_ControllableNode::to_string(classname);
      }

      return(s.str());
   }

   virtual std::string to_label() const
   {
      lgl_string label = lgl_ControllableNode::to_label();

      if (getRestartBarrier() ||
          getPauseBarrier() ||
          getResumeBarrier() ||
          getSpeedupBarrier())
      {
         std::string barrier = "\\nblock:";
         if (getRestartBarrier()) barrier += " restart";
         if (getPauseBarrier()) barrier += " pause";
         if (getResumeBarrier()) barrier += " resume";
         if (getSpeedupBarrier()) barrier += " speedup";
         label += barrier;
      }

      return(label.strip("\\n"));
   }

};

//! steerable node (base class)
//!
//! a steerable node is like a controllable node except that the control type is not just boolean but also has a scalar component
//! * it supports actions, which need a scalar parameter such as restarting, resuming or rewinding animations to a particular point in time
class lgl_SteerableNode: public lgl_ControllableNode
{
public:

   //! ctor
   lgl_SteerableNode(const std::string &id = "",
                     lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node)
   {}

   virtual ~lgl_SteerableNode() {}

   virtual std::string getClassId() const {return("Steerable");}

   //! apply the steer level
   virtual void steer(double level, bool override = true) {}

   virtual std::string to_id(const std::string &classid, bool classname = true) const
   {
      std::stringstream s;

      if (classname)
         s << lgl_ControllableNode::to_id(classid, classname);
      else
      {
         if (classid != lgl_SteerableNode::getClassId())
            s << lgl_ControllableNode::to_id(classid, classname);
      }

      return(s.str());
   }

};

//! steer node
//!
//! a steer node triggers actions on linked child nodes that require a scalar action component
//! * the scalar action component (the steer level) is provided by the setSteerLevel() method
//! * if an action is triggered on a steer node, the steer level is propagated to all steerable child nodes
class lgl_SteerNode: public lgl_ControlNode
{
public:

   //! ctor
   lgl_SteerNode(const std::string &id = "",
                 lgl_Node *node = NULL)
      : lgl_ControlNode(id, node),
        level_(0)
   {}

   virtual ~lgl_SteerNode() {}

   virtual std::string getClassId() const {return("Steer");}

   //! get the steer level
   double getSteerLevel() const
   {
      return(level_);
   }

   //! set the steer level
   void setSteerLevel(double level)
   {
      level_ = level;
   }

protected:

   double level_;

   virtual void control(lgl_ControllableNode *ctrl, bool override)
   {
      lgl_ControlNode::control(ctrl, override);

      lgl_SteerableNode *node = dynamic_cast<lgl_SteerableNode*>(ctrl);
      if (node) node->steer(level_, override);
   }

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_ControlNode::to_string(classname);

      if (classname)
         if (getSteerLevel() != 0)
            s << "(" << getSteerLevel() << ")";

      return(s.str());
   }

   virtual std::string to_label() const
   {
      lgl_string label = lgl_ControlNode::to_label();

      if (getSteerLevel() != 0)
         label += "\\nlevel=" + glslmath::to_string(getSteerLevel());

      return(label.strip("\\n"));
   }

};

//! camera node
//!
//! a camera node defines the viewing and projection parameters
//! * the first enabled camera node is used to setup viewing and projection matrices
//!  * viewing parameters: eye point, lookat point, up vector
//!  * projection parameters: fovy, aspect, nearp, farp
//!  * the camera also defines stereo rendering parameters
//!   * stereo base: distance of eye points
//!   * if the stereo base is zero, stereo rendering is disabled
class lgl_CameraNode: public lgl_ControllableNode, public lgl_Cam
{
public:

   lgl_CameraNode(const std::string &id = "",
                  lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node),
        lgl_Cam()
   {}

   virtual ~lgl_CameraNode() {}

   virtual std::string getClassId() const {return("Camera");}

protected:

   virtual void pre_render()
   {
      lgl_Node::pre_render();
      if (enabled_) begin();
   }

   virtual void post_render()
   {
      if (enabled_) end();
      lgl_Node::post_render();
   }

   virtual void pre_pick()
   {
      lgl_Node::pre_pick();
      if (enabled_) begin();
   }

   virtual void post_pick()
   {
      if (enabled_) end();
      lgl_Node::post_pick();
   }

public:

   virtual std::string to_string(bool classname = true) const
   {
      mat4 p = getProjectionMatrix();
      mat4 m = getViewMatrix();

      if (lglIsManipApplied())
         m <<= lglGetManip();

      std::stringstream s;

      s << lgl_ControllableNode::to_string(classname);
      s << "(mat4" << p << ", mat4" << m << ")";

      return(s.str());
   }

};

//! geometry node
//!
//! a geometry node encapsulates a triangle mesh by storing a pointer to a vbo (lglVBO)
//! * the vbo pointer is not reference-counted
class lgl_GeometryNode: public lgl_ControllableNode
{
public:

   //! ctor
   lgl_GeometryNode(lglVBO *vbo = NULL,
                    const std::string &id = "",
                    lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node),
        vbo_(vbo), managed_(false)
   {}

   virtual ~lgl_GeometryNode()
   {
      if (managed_)
         if (vbo_) delete vbo_;
   }

   virtual std::string getClassId() const {return("Geometry");}

   //! get the vbo
   lglVBO *getVBO() const
   {
      return(vbo_);
   }

   //! set the vbo
   void setVBO(lglVBO *vbo)
   {
      if (managed_)
         if (vbo_) delete vbo_;

      vbo_ = vbo;
   }

   //! is the vbo managed?
   bool managed()
   {
      return(managed_);
   }

   //! manage the vbo
   void manage(bool yes = true)
   {
      managed_ = yes;
   }

   //! get the vbo name
   std::string getName() const
   {
      std::string name;

      if (vbo_)
         name = vbo_->getName();

      return(name);
   }

   virtual bool stateless() const
   {
      return(true);
   }

protected:

   lglVBO *vbo_;
   bool managed_;

   virtual void render()
   {
      if (hidden_) return;

      if (vbo_ && enabled_)
      {
         GL *gl = getGL();

         vbo_->lglRender(gl);
      }

      lgl_Node::render();
   }

   virtual void updateBoundingBox(vec3 &bboxmin, vec3 &bboxmax) const
   {
      if (vbo_ && enabled_)
      {
         vec3 bmin, bmax;
         vbo_->lglGetBoundingBox(bmin, bmax);

         growBoundingBox(bboxmin, bboxmax, bmin);
         growBoundingBox(bboxmin, bboxmax, bmax);
      }
   }

   virtual std::vector<vec4> get_vertices()
   {
      if (vbo_ && enabled_)
         return(vbo_->lglGetVertexCoordinates());

      return(std::vector<vec4>());
   }

   virtual lglVBO *get_vbo()
   {
      if (vbo_ && enabled_) return(vbo_);
      else return(NULL);
   }

   virtual void pick()
   {
      if (hidden_) return;

      if (vbo_ && enabled_)
         lglRayCast(vbo_);

      lgl_Node::pick();
   }

   virtual lgl_Node *convert();
   virtual lgl_Node *clone();

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_ControllableNode::to_string(classname);

      if (getName() != "")
         s << "(\"" << getName() << "\")";

      return(s.str());
   }

   virtual std::string to_label() const
   {
      lgl_string label = lgl_ControllableNode::to_label();

      if (getId() == "")
      {
         lgl_string name = getName();

         if (name != "")
         {
            name.remove("(", ")");
            label += "\\n" + name;
         }
      }

      if (vbo_)
      {
         label += "\\n[";
         label += glslmath::to_string(vbo_->lglGetVertexCount());
         label += "/";
         label += glslmath::to_string(vbo_->lglGetPrimitiveCount());
         label += "]";
      }

      return(label.strip("\\n"));
   }

};

//! geometry container node
//!
//! a geometry container node encapsulates a triangle mesh by storing a pointer to a vbo (lglVBO)
//! * the vbo pointer is reference-counted
class lgl_ContainerNode: public lgl_GeometryNode
{
public:

   //! ctor
   lgl_ContainerNode(lglVBO *vbo,
                     const std::string &id = "",
                     lgl_Node *node = NULL)
      : lgl_GeometryNode(vbo, id, node)
   {
      manage();
   }

   virtual std::string getClassId() const {return("Container");}
};

//! vbo node
//!
//! a vbo node encapsulates a triangle mesh by deriving from the lglVBO class
class lgl_VBONode: public lgl_ControllableNode, public lglVBO
{
public:

   lgl_VBONode(const std::string &id = "",
               lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node),
        lglVBO()
   {}

   virtual ~lgl_VBONode() {}

   virtual std::string getClassId() const {return("VBO");}

   virtual bool stateless() const
   {
      return(true);
   }

   virtual bool optimize();

protected:

   virtual void render()
   {
      if (hidden_) return;

      if (enabled_)
      {
         GL *gl = getGL();

         lglRender(gl);
      }

      lgl_Node::render();
   }

   virtual void updateBoundingBox(vec3 &bboxmin, vec3 &bboxmax) const
   {
      if (enabled_)
      {
         vec3 bmin, bmax;
         lglGetBoundingBox(bmin, bmax);

         growBoundingBox(bboxmin, bboxmax, bmin);
         growBoundingBox(bboxmin, bboxmax, bmax);
      }
   }

   virtual std::vector<vec4> get_vertices()
   {
      if (enabled_)
         return(lglGetVertexCoordinates());

      return(std::vector<vec4>());
   }

   virtual lglVBO *get_vbo()
   {
      if (enabled_) return(this);
      else return(NULL);
   }

   virtual void pick()
   {
      if (hidden_) return;

      if (enabled_)
         lglRayCast(this);

      lgl_Node::pick();
   }

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_ControllableNode::to_string(classname);

      if (getName() != "")
         s << "(\"" << getName() << "\")";

      return(s.str());
   }

   virtual std::string to_label() const
   {
      lgl_string label = lgl_ControllableNode::to_label();

      if (getId() == "")
      {
         lgl_string name = getName();

         if (name != "")
         {
            name.remove("(", ")");
            label += "\\n" + name;
         }
      }

      label += "\\n[";
      label += glslmath::to_string(lglGetVertexCount());
      label += "/";
      label += glslmath::to_string(lglGetPrimitiveCount());
      label += "]";

      return(label.strip("\\n"));
   }

};

inline lgl_Node *lgl_GeometryNode::convert()
{
   return(clone());
}

inline lgl_Node *lgl_GeometryNode::clone()
{
   lgl_VBONode *vbo = new lgl_VBONode();

   if (vbo_)
   {
      vbo_->lglAppendVerticesTo(vbo);
      vbo->setName(vbo_->getName());
   }

   return(vbo);
}

inline bool lgl_VBONode::optimize()
{
   int optimizable = 0;

   for (unsigned int i=0; i<children_.size(); i++)
      if (follow(i))
      {
         lgl_Node *node = get(i);

         if (lgl_VBONode *vbo = dynamic_cast<lgl_VBONode*>(node))
            if (vbo->lglGetVertexMode() == lglGetVertexMode())
               optimizable++;
      }

   if (optimizable == 1)
      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
         {
            lgl_Node *node = get(i);

            if (lgl_VBONode *vbo = dynamic_cast<lgl_VBONode*>(node))
               if (vbo->lglGetVertexMode() == lglGetVertexMode())
                   lglAppendVerticesTo(vbo);
         }

   return(optimizable==1);
}

//! color node
//!
//! a color node stores a single RGBA color
//! * the color affects all of its child nodes
class lgl_ColorNode: public lgl_ControllableNode
{
public:

   //! ctor
   lgl_ColorNode(double r, double g, double b, double a=1,
                 const std::string &id = "",
                 lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node),
        color_(vec4(r,g,b,a))
   {}

   //! ctor
   lgl_ColorNode(double color,
                 const std::string &id = "",
                 lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node),
        color_(vec4(color))
   {}

   //! ctor
   lgl_ColorNode(const vec4 &color = vec4(1,1,1),
                 const std::string &id = "",
                 lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node),
        color_(color)
   {}

   virtual ~lgl_ColorNode() {}

   virtual std::string getClassId() const {return("Color");}

   //! get the color
   vec4 getColor() const
   {
      return(color_);
   }

   //! set the color
   void setColor(const vec4 &c)
   {
      color_ = c;
   }

   //! check for transparency
   virtual bool hasTransparency()
   {
      if (!enabled_) return(false);
      return(color_.a < 1);
   }

protected:

   vec4 color_;

   virtual void render()
   {
      if (hidden_) return;

      vec4 color(0);

      if (enabled_)
      {
         color = lglGetColor();
         lglColor(color_);
      }

      lgl_Node::render();

      if (enabled_)
      {
         lglColor(color);
      }
   }

   virtual vec4 update_color(vec4 c)
   {
      if (enabled_) return(color_);
      else return(c);
   }

   virtual bool optimize();
   virtual bool swappable();

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_ControllableNode::to_string(classname);
      s << "(vec4" << getColor() << ")";

      return(s.str());
   }

};

inline bool lgl_ColorNode::optimize()
{
   bool optimizable = true;

   for (unsigned int i=0; i<children_.size(); i++)
      if (follow(i))
      {
         lgl_Node *node = get(i);
         lgl_VBONode *vbo = dynamic_cast<lgl_VBONode*>(node);

         if (!node->leaf())
            optimizable = false;

         if (!vbo)
            optimizable = false;
      }

   if (optimizable)
      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
         {
            lgl_Node *node = get(i);

            if (lgl_VBONode *vbo = dynamic_cast<lgl_VBONode*>(node))
               if (!vbo->lglAppliedColoring())
               {
                  vec4 c = getColor();
                  vbo->lglApplyColor(c);
               }
         }

   return(optimizable);
}

//! transformation node
//!
//! a transformation node stores a single 4x4 transformation matrix
//! * the transformation is applied to all of its child nodes
class lgl_TransformationNode: public lgl_SteerableNode
{
public:

   //! ctor
   lgl_TransformationNode(const std::string &id = "",
                          lgl_Node *node = NULL)
      : lgl_SteerableNode(id, node),
        transformation_(1),
        identity_(true)
   {}

   //! ctor
   lgl_TransformationNode(const mat4 &m,
                          const std::string &id = "",
                          lgl_Node *node = NULL)
      : lgl_SteerableNode(id, node),
        transformation_(m),
        identity_(false)
   {}

   //! ctor (translation)
   lgl_TransformationNode(const vec3 &v,
                          const std::string &id = "",
                          lgl_Node *node = NULL)
      : lgl_SteerableNode(id, node),
        transformation_(mat4::translate(v)),
        identity_(v==vec3(0))
   {}

   //! ctor (rotation)
   lgl_TransformationNode(double angle, const vec3 &v,
                          const std::string &id = "",
                          lgl_Node *node = NULL)
      : lgl_SteerableNode(id, node),
        transformation_(mat4::rotate(angle, v)),
        identity_(angle==0)
   {}

   //! ctor (scale)
   lgl_TransformationNode(double scale,
                          const std::string &id = "",
                          lgl_Node *node = NULL)
      : lgl_SteerableNode(id, node),
        transformation_(mat4::scale(scale)),
        identity_(scale==1)
   {}

   virtual ~lgl_TransformationNode() {}

   virtual std::string getClassId() const {return("Transformation");}

   //! transform a point
   vec3 transform(const vec3 &p) const
   {
      if (identity_) return(p);
      return(transformation_ * vec4(p));
   }

   //! transform a point
   vec4 transform(const vec4 &p) const
   {
      if (identity_) return(p);
      return(transformation_ * p);
   }

   //! get the transformation matrix
   mat4 getTransformation() const
   {
      return(transformation_);
   }

   //! set the transformation matrix
   void setTransformation(const mat4 &m)
   {
      transformation_ = m;
      identity_ = m==mat4(1);
   }

   //! set the transformation matrix (translation)
   void translate(const vec3 &v)
   {
      transformation_ <<= mat4::translate(v);
      identity_ = v==vec3(0);
   }

   //! set the transformation matrix (translation)
   void translate(double x, double y, double z)
   {
      vec3 v(x,y,z);
      transformation_ <<= mat4::translate(v);
      identity_ = v==vec3(0);
   }

   //! set the transformation matrix (rotation)
   void rotate(double angle, const vec3 &v)
   {
      transformation_ <<= mat4::rotate(angle, v);
      identity_ = angle==0;
   }

   //! set the transformation matrix (rotation)
   void rotate(double angle,
               double x, double y, double z)
   {
      vec3 v(x,y,z);
      transformation_ <<= mat4::rotate(angle, v);
      identity_ = angle==0;
   }

   //! set the transformation matrix (scale)
   void scale(double c)
   {
      transformation_ <<= mat4::scale(c);
      identity_ = c==1;
   }

   //! set the transformation matrix (scale)
   void scale(double x, double y, double z, double w=1)
   {
      vec4 c(x,y,z,w);
      transformation_ <<= mat4::scale(c);
      identity_ = c==vec4(1);
   }

   //! set the transformation matrix (scale)
   void scale(const vec4 &c)
   {
      transformation_ <<= mat4::scale(c);
      identity_ = c==vec4(1);
   }

protected:

   mat4 transformation_;
   bool identity_;

   virtual void render()
   {
      if (hidden_) return;

      if (!identity_ && enabled_)
      {
         lglPushMatrix();
         lglMultMatrix(transformation_);
      }

      lgl_Node::render();

      if (!identity_ && enabled_)
      {
         lglPopMatrix();
      }
   }

   virtual void updateBoundingBox(vec3 &bboxmin, vec3 &bboxmax) const
   {
      if (enabled_)
      {
         if (bboxmin.x <= bboxmax.x &&
             bboxmin.y <= bboxmax.y &&
             bboxmin.z <= bboxmax.z)
         {
            vec3 p1(bboxmin.x, bboxmin.y, bboxmin.z);
            vec3 p2(bboxmax.x, bboxmin.y, bboxmin.z);
            vec3 p3(bboxmin.x, bboxmax.y, bboxmin.z);
            vec3 p4(bboxmax.x, bboxmax.y, bboxmin.z);
            vec3 p5(bboxmin.x, bboxmin.y, bboxmax.z);
            vec3 p6(bboxmax.x, bboxmin.y, bboxmax.z);
            vec3 p7(bboxmin.x, bboxmax.y, bboxmax.z);
            vec3 p8(bboxmax.x, bboxmax.y, bboxmax.z);

            bboxmin = vec3(DBL_MAX);
            bboxmax = vec3(-DBL_MAX);

            growBoundingBox(bboxmin, bboxmax, transform(p1));
            growBoundingBox(bboxmin, bboxmax, transform(p2));
            growBoundingBox(bboxmin, bboxmax, transform(p3));
            growBoundingBox(bboxmin, bboxmax, transform(p4));
            growBoundingBox(bboxmin, bboxmax, transform(p5));
            growBoundingBox(bboxmin, bboxmax, transform(p6));
            growBoundingBox(bboxmin, bboxmax, transform(p7));
            growBoundingBox(bboxmin, bboxmax, transform(p8));
         }
      }
   }

   virtual mat4 update_transform(mat4 m)
   {
      if (!identity_ && enabled_) return(m * getTransformation());
      else return(m);
   }

   virtual void pick()
   {
      if (hidden_) return;

      if (!identity_ && enabled_)
      {
         lglPushMatrix();
         lglMultMatrix(transformation_);
      }

      lgl_Node::pick();

      if (!identity_ && enabled_)
      {
         lglPopMatrix();
      }
   }

   virtual bool optimize();
   virtual bool swappable();

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_SteerableNode::to_string(classname);
      s << "(mat4" << getTransformation() << ")";

      return(s.str());
   }

};

//! translation node
//!
//! a translation node stores a 4x4 translation matrix determined by a translation vector
//! * all child nodes are translated by that vector
class lgl_TranslationNode: public lgl_TransformationNode
{
public:

   //! ctor
   lgl_TranslationNode(const vec3 &v,
                       const std::string &id = "",
                       lgl_Node *node = NULL)
      : lgl_TransformationNode(id, node),
        translation_(v)
   {
      translate(v);
   }

   //! ctor
   lgl_TranslationNode(double x, double y, double z,
                       const std::string &id = "",
                       lgl_Node *node = NULL)
      : lgl_TransformationNode(id, node),
        translation_(vec3(x,y,z))
   {
      translate(x,y,z);
   }

   virtual std::string getClassId() const {return("Translation");}

   //! get the translation vector
   vec3 getTranslation() const
   {
      return(translation_);
   }

protected:

   vec3 translation_;

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_SteerableNode::to_string(classname);
      s << "(vec3" << getTranslation() << ")";

      return(s.str());
   }

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_TransformationNode::to_label() + "\\nT").strip("\\n"));
   }

};

//! rotation node
//!
//! a rotation node stores a 4x4 rotation matrix determined by a rotation angle and a rotation axis
//! * all child nodes are rotated around that axis
class lgl_RotationNode: public lgl_TransformationNode
{
public:

   //! ctor
   lgl_RotationNode(double angle, const vec3 &axis,
                    const std::string &id = "",
                    lgl_Node *node = NULL)
      : lgl_TransformationNode(id, node),
        rotation_angle_(angle),
        rotation_axis_(axis)
   {
      rotate(angle, axis);
   }

   //! ctor
   lgl_RotationNode(double angle,
                    double x, double y, double z,
                    const std::string &id = "",
                    lgl_Node *node = NULL)
      : lgl_TransformationNode(id, node),
        rotation_angle_(angle),
        rotation_axis_(vec3(x,y,z))
   {
      rotate(angle, x,y,z);
   }

   virtual std::string getClassId() const {return("Rotation");}

   //! get the rotation angle
   double getRotationAngle() const
   {
      return(rotation_angle_);
   }

   //! get the rotation axis
   vec3 getRotationAxis() const
   {
      return(rotation_axis_);
   }

protected:

   double rotation_angle_;
   vec3 rotation_axis_;

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_SteerableNode::to_string(classname);
      s << "(" << getRotationAngle() << ", vec3" << getRotationAxis() << ")";

      return(s.str());
   }

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_TransformationNode::to_label() + "\\nR").strip("\\n"));
   }

};

//! scale node
//!
//! a scale node stores a 4x4 scaling matrix determined by a single uniform or multiple non-uniform scaling factors
//! * all child nodes are scaled with those scaling factors
class lgl_ScaleNode: public lgl_TransformationNode
{
public:

   //! ctor
   lgl_ScaleNode(double c,
                 const std::string &id = "",
                 lgl_Node *node = NULL)
      : lgl_TransformationNode(id, node),
        scale_(vec4(c))
   {
      scale(c);
   }

   //! ctor
   lgl_ScaleNode(double x, double y, double z, double w=1,
                 const std::string &id = "",
                 lgl_Node *node = NULL)
      : lgl_TransformationNode(id, node),
        scale_(vec4(x,y,z,w))
   {
      scale(x,y,z,w);
   }

   //! ctor
   lgl_ScaleNode(const vec4 &c,
                 const std::string &id = "",
                 lgl_Node *node = NULL)
      : lgl_TransformationNode(id, node),
        scale_(c)
   {
      scale(c);
   }

   virtual std::string getClassId() const {return("Scale");}

   //! get the scale vector
   vec4 getScale() const
   {
      return(scale_);
   }

protected:

   vec4 scale_;

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_SteerableNode::to_string(classname);
      s << "(vec4" << getScale() << ")";

      return(s.str());
   }

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_TransformationNode::to_label() + "\\nS").strip("\\n"));
   }

};

//! texture transformation node
//!
//! a texture transformation node stores a single 4x4 transformation matrix, which is applied to objects with texture coordinates
//! * the texture transformation is applied to all of its children, which have texture coordinates
class lgl_TextureTransformationNode: public lgl_TransformationNode
{
public:

   //! ctor
   lgl_TextureTransformationNode(const std::string &id = "",
                                 lgl_Node *node = NULL)
      : lgl_TransformationNode(id, node)
   {}

   //! ctor
   lgl_TextureTransformationNode(const mat4 &m,
                                 const std::string &id = "",
                                 lgl_Node *node = NULL)
      : lgl_TransformationNode(m, id, node)
   {}

   //! ctor (translation)
   lgl_TextureTransformationNode(const vec3 &v,
                                 const std::string &id = "",
                                 lgl_Node *node = NULL)
      : lgl_TransformationNode(v, id, node)
   {}

   //! ctor (rotation)
   lgl_TextureTransformationNode(double angle, const vec3 &v,
                                 const std::string &id = "",
                                 lgl_Node *node = NULL)
      : lgl_TransformationNode(angle, v, id, node)
   {}

   //! ctor (scale)
   lgl_TextureTransformationNode(double scale,
                                 const std::string &id = "",
                                 lgl_Node *node = NULL)
      : lgl_TransformationNode(scale, id, node)
   {}

   virtual std::string getClassId() const {return("TextureTransformation");}

protected:

   virtual void render()
   {
      if (hidden_) return;

      if (!identity_ && enabled_)
      {
         lglMatrixMode(LGL_TEXTURE);
         lglPushMatrix();
         lglMultMatrix(transformation_);
         lglMatrixMode(LGL_MODELVIEW);
      }

      lgl_Node::render();

      if (!identity_ && enabled_)
      {
         lglMatrixMode(LGL_TEXTURE);
         lglPopMatrix();
         lglMatrixMode(LGL_MODELVIEW);
      }
   }

   virtual mat4 update_transform(mat4 m) {return(m);}

   virtual void pick()
   {
      lgl_Node::pick();
   }

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_TransformationNode::to_label() + "\\nTM").strip("\\n"));
   }

};

//! animation node (base class)
//!
//! an animation nodes stores a single 4x4 transformation matrix, which is updated regularly to account for the animation
//! * the transformation matrix is updated in the pure virtual animate() method
//! * a particular animation node must derive from this base class and implement that method
class lgl_AnimationNode: public lgl_TransformationNode
{
public:

   //! animation steer mode enum
   enum lgl_animation_steer_mode_enum
   {
      LGL_STEER_NONE,
      LGL_STEER_PAUSE,
      LGL_STEER_RESUME,
      LGL_STEER_SPEEDUP,
      LGL_STEER_RESTART,
      LGL_STEER_RESTART_RESUME,
      LGL_STEER_REWIND,
      LGL_STEER_REWIND_RESUME,
      LGL_STEER_REVERSE,
      LGL_STEER_DIRECTION,
      LGL_STEER_REWIND_REVERSE,
      LGL_STEER_REWIND_REVERSE_RESUME,
      LGL_STEER_REWIND_DIRECTION,
      LGL_STEER_REWIND_DIRECTION_RESUME,
      LGL_STEER_FORWARD,
      LGL_STEER_BACKWARD,
      LGL_STEER_RESTART_ALL,
      LGL_STEER_PAUSE_ALL,
      LGL_STEER_SPEEDUP_ALL
   };

   //! ctor
   lgl_AnimationNode(double delta = 0, vec3 vector = vec3(0), double limit = 0, double accel = 0, double reverse = 0,
                     const std::string &id = "",
                     lgl_Node *node = NULL)
      : lgl_TransformationNode(id, node),
        animation_(0),
        velocity_(delta),
        acceleration_(accel),
        delta_(delta),
        accel_(accel),
        reverse_(reverse),
        vector_(vector),
        limit_(fabs(limit)),
        stop_(false),
        signal_(false),
        sync_(false),
        paused_(false),
        speedup_(1)
   {}

   virtual std::string getClassId() const {return("Animation");}

   //! get the animation delta
   double getDelta() const
   {
      return(delta_);
   }

   //! get the animation vector
   vec3 getVector() const
   {
      return(vector_);
   }

   //! get the animation velocity
   double getVelocity() const
   {
      return(velocity_);
   }

   //! get the animation limit
   double getLimit() const
   {
      return(limit_);
   }

   //! stop on animation limit
   void stopOnLimit()
   {
      stop_ = true;
   }

   //! get stop on animation limit
   bool getStopOnLimit() const
   {
      return(stop_);
   }

   //! signal on stop
   void signalOnStop()
   {
      signal_ = true;
   }

   //! get signal on stop
   bool getSignalOnStop() const
   {
      return(signal_);
   }

   //! get the animation acceleration
   double getAcceleration() const
   {
      return(accel_);
   }

   //! get the reverse acceleration after limit has been reached
   double getReverse() const
   {
      return(reverse_);
   }

   //! get the actual animation state
   double animation() const
   {
      return(animation_);
   }

   //! restart the animation
   void restart(double animation = 0)
   {
      animation_ = animation;
      velocity_ = delta_;
      acceleration_ = accel_;

      if (limit_ != 0)
         if (fabs(animation_) >= limit_)
         {
            if (animation_ >= 0)
            {
               animation_ = limit_;
               velocity_ = -fabs(delta_);
            }
            else
            {
               animation_ = -limit_;
               velocity_ = fabs(delta_);
            }
         }

      transformation_ = animate(animation_, vector_);
      identity_ = false;
   }

   //! rewind the animation
   void rewind(double animation = 0)
   {
      animation_ = animation;

      transformation_ = animate(animation_, vector_);
      identity_ = false;
   }

   //! set the direction of the animation
   void direction(bool forward)
   {
      if (forward && velocity_<0) reverse();
      if (!forward && velocity_>0) reverse();
   }

   //! reverse the animation
   void reverse()
   {
      velocity_ = -velocity_;
   }

   //! move the animation forward
   void forward(double delta)
   {
      if (velocity_ >= 0)
         animation_ += delta;
      else
         animation_ -= delta;

      transformation_ = animate(animation_, vector_);
      identity_ = false;
   }

   //! move the animation backward
   void backward(double delta)
   {
      if (velocity_ >= 0)
         animation_ -= delta;
      else
         animation_ += delta;

      transformation_ = animate(animation_, vector_);
      identity_ = false;
   }

   //! is the animation paused?
   virtual bool paused() const
   {
      return(paused_);
   }

   //! pause the animation
   virtual void pause(bool yes = true)
   {
      paused_ = yes;
   }

   //! resume the animation
   virtual void resume(bool yes = true)
   {
      paused_ = !yes;
   }

   //! get the animation speedup
   virtual double speedup()
   {
      return(speedup_);
   }

   //! set the animation speedup
   virtual void setSpeedup(double speedup = 1)
   {
      speedup_ = speedup;
   }

   //! get the steer mode
   lgl_animation_steer_mode_enum getSteerMode() const
   {
      return(steer_mode_);
   }

   //! set the steer mode
   void setSteerMode(lgl_animation_steer_mode_enum mode)
   {
      steer_mode_ = mode;
   }

   //! apply the steer level as specified by the steer mode
   virtual void steer(double level, bool override = true)
   {
      switch (getSteerMode())
      {
         case LGL_STEER_NONE: break;
         case LGL_STEER_PAUSE: pause(override); break;
         case LGL_STEER_RESUME: resume(override); break;
         case LGL_STEER_SPEEDUP: if (override) setSpeedup(level); break;
         case LGL_STEER_RESTART: if (override) restart(level); break;
         case LGL_STEER_RESTART_RESUME: restart(level); resume(override); break;
         case LGL_STEER_REWIND: if (override) rewind(level); break;
         case LGL_STEER_REWIND_RESUME: rewind(level); resume(override); break;
         case LGL_STEER_REVERSE: if (override) reverse(); break;
         case LGL_STEER_DIRECTION: direction(override); break;
         case LGL_STEER_REWIND_REVERSE: if (override) {rewind(level); reverse();} break;
         case LGL_STEER_REWIND_REVERSE_RESUME: rewind(level); reverse(); resume(override); break;
         case LGL_STEER_REWIND_DIRECTION: rewind(level); direction(override); break;
         case LGL_STEER_REWIND_DIRECTION_RESUME: rewind(level); direction(override); resume(); break;
         case LGL_STEER_FORWARD: if (override) forward(level); break;
         case LGL_STEER_BACKWARD: if (override) backward(level); break;
         case LGL_STEER_RESTART_ALL: if (override) restartAll(level); break;
         case LGL_STEER_PAUSE_ALL: pauseAll(override); break;
         case LGL_STEER_SPEEDUP_ALL: if (override) speedupAll(level); break;
      }
   }

protected:

   double animation_;
   double velocity_;
   double acceleration_;

   double delta_;
   double accel_;
   double reverse_;
   vec3 vector_;
   double limit_;
   bool stop_;
   bool signal_;
   bool sync_;

   bool paused_;
   double speedup_;

   lgl_animation_steer_mode_enum steer_mode_;

   virtual void update(double dt)
   {
      if (!paused_)
      {
         if (velocity_!=0 || acceleration_!=0)
         {
            velocity_ += acceleration_*dt;
            animation_ += velocity_*speedup_*dt;

            if (limit_ != 0)
            {
               if (stop_)
               {
                  if (fabs(animation_) > limit_)
                  {
                     if (animation_ >= 0)
                        animation_ = limit_;
                     else
                        animation_ = -limit_;

                     paused_ = true;

                     if (signal_)
                        signal();
                  }
               }

               if (reverse_ == 0)
               {
                  if (fabs(animation_) > limit_)
                  {
                     if (animation_ >= 0) animation_ -= animation_ - limit_;
                     else animation_ += -animation_ - limit_;
                     velocity_ = -velocity_;
                  }
               }
               else
               {
                  if (fabs(animation_) > limit_)
                  {
                     double reverse;

                     if (animation_ <= 0)
                        reverse = fabs(reverse_);
                     else
                        reverse = -fabs(reverse_);

                     acceleration_ = reverse;
                     sync_ = true;
                  }
                  else
                  {
                     if (sync_)
                     {
                        if (acceleration_ >= 0)
                           velocity_ = fabs(delta_);
                        else
                           velocity_ = -fabs(delta_);

                        acceleration_ = 0;
                        sync_ = false;
                     }
                  }
               }
            }

            transformation_ = animate(animation_, vector_);
            identity_ = false;
         }
      }

      lgl_Node::update(dt);
   }

   //! to be implemented for a particular type of animation
   virtual mat4 animate(double animation, const vec3 &vector) = 0;

public:

   virtual void restartAll(double animation = 0)
   {
      restart(animation);
      lgl_Node::restartAll(animation);
   }

   virtual void pauseAll(bool yes = true)
   {
      pause(yes);
      lgl_Node::pauseAll(yes);
   }

   virtual void speedupAll(double speedup = 1)
   {
      setSpeedup(speedup);
      lgl_Node::speedupAll(speedup);
   }

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      if (classname)
      {
         s << lgl_SteerableNode::to_string(classname);

         s << "(";

         s << getDelta();
         s << ", vec3" << getVector();

         if (getLimit()!=0 || getAcceleration()!=0)
         {
            s << ", " << getLimit();
            s << ", " << getStopOnLimit();
            s << ", " << getSignalOnStop();
            s << ", " << getAcceleration();
            s << ", " << getReverse();
         }

         s << ")";

         if (getSteerMode() != LGL_STEER_NONE)
         {
            s << "(";

            switch (getSteerMode())
            {
               case LGL_STEER_NONE: s << "none"; break;
               case LGL_STEER_PAUSE: s << "pause"; break;
               case LGL_STEER_RESUME: s << "resume"; break;
               case LGL_STEER_SPEEDUP: s << "speedup"; break;
               case LGL_STEER_RESTART: s << "restart"; break;
               case LGL_STEER_RESTART_RESUME: s << "restart-resume"; break;
               case LGL_STEER_REWIND: s << "rewind"; break;
               case LGL_STEER_REWIND_RESUME: s << "rewind-resume"; break;
               case LGL_STEER_REVERSE: s << "reverse"; break;
               case LGL_STEER_DIRECTION: s << "direction"; break;
               case LGL_STEER_REWIND_REVERSE: s << "rewind-reverse"; break;
               case LGL_STEER_REWIND_REVERSE_RESUME: s << "rewind-reverse-resume"; break;
               case LGL_STEER_REWIND_DIRECTION: s << "rewind-direction"; break;
               case LGL_STEER_REWIND_DIRECTION_RESUME: s << "rewind-direction-resume"; break;
               case LGL_STEER_FORWARD: s << "forward"; break;
               case LGL_STEER_BACKWARD: s << "backward"; break;
               case LGL_STEER_RESTART_ALL: s << "restart-all"; break;
               case LGL_STEER_PAUSE_ALL: s << "pause-all"; break;
               case LGL_STEER_SPEEDUP_ALL: s << "speedup-all"; break;
            }

            s << ")";
         }
      }
      else
      {
         s << lgl_SteerableNode::to_id("Transformation", classname);
         s << "(mat4" << getTransformation() << ")";
      }

      return(s.str());
   }

   virtual std::string to_label() const
   {
      lgl_string label = lgl_SteerableNode::to_label();

      switch (getSteerMode())
      {
         case LGL_STEER_PAUSE: label += "\\npause"; break;
         case LGL_STEER_RESUME: label += "\\nresume"; break;
         case LGL_STEER_SPEEDUP: label += "\\nspeedup"; break;
         case LGL_STEER_RESTART: label += "\\nrestart"; break;
         case LGL_STEER_RESTART_RESUME: label += "\\nrestart-resume"; break;
         case LGL_STEER_REWIND: label += "\\nrewind"; break;
         case LGL_STEER_REWIND_RESUME: label += "\\nrewind-resume"; break;
         case LGL_STEER_REVERSE: label += "\\nreverse"; break;
         case LGL_STEER_DIRECTION: label += "\\ndirection"; break;
         case LGL_STEER_REWIND_REVERSE: label += "\\nrewind-reverse"; break;
         case LGL_STEER_REWIND_REVERSE_RESUME: label += "\\nrewind-reverse-resume"; break;
         case LGL_STEER_REWIND_DIRECTION: label += "\\nrewind-direction"; break;
         case LGL_STEER_REWIND_DIRECTION_RESUME: label += "\\nrewind-direction-resume"; break;
         case LGL_STEER_FORWARD: label += "\\nforward"; break;
         case LGL_STEER_BACKWARD: label += "\\nbackward"; break;
         case LGL_STEER_RESTART_ALL: label += "\\nrestart-all"; break;
         case LGL_STEER_PAUSE_ALL: label += "\\npause-all"; break;
         case LGL_STEER_SPEEDUP_ALL: label += "\\nspeedup-all"; break;
         default: break;
      }

      return(label.strip("\\n"));
   }

};

//! animation node (translation)
//!
//! a translation animation node translates all child nodes continuously in one direction
class lgl_TranslationAnimationNode: public lgl_AnimationNode
{
public:

   //! ctor
   lgl_TranslationAnimationNode(double delta, const vec3 &vector, double limit = 0, double accel = 0, double reverse = 0,
                                const std::string &id = "",
                                lgl_Node *node = NULL)
      : lgl_AnimationNode(delta, vector, limit, accel, reverse, id, node)
   {}

   virtual std::string getClassId() const {return("TranslationAnimation");}

protected:

   virtual mat4 animate(double animation, const vec3 &vector)
   {
      return(mat4::translate(animation * vector));
   }

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_AnimationNode::to_label() + "\\nT").strip("\\n"));
   }

};

//! animation node (rotation)
//!
//! a rotation animation node rotates all child nodes continuously around a rotation axis
class lgl_RotationAnimationNode: public lgl_AnimationNode
{
public:

   //! ctor
   lgl_RotationAnimationNode(double omega, const vec3 &axis, double limit = 0, double accel = 0, double reverse = 0,
                             const std::string &id = "",
                             lgl_Node *node = NULL)
      : lgl_AnimationNode(omega, axis, limit, accel, reverse, id, node)
   {}

   virtual std::string getClassId() const {return("RotationAnimation");}

protected:

   virtual mat4 animate(double animation, const vec3 &axis)
   {
      return(mat4::rotate(animation, axis));
   }

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_AnimationNode::to_label() + "\\nR").strip("\\n"));
   }

};

//! transition node (base class)
//!
//! a transition node performs a single continuous transformation operation and then stops after reaching a certain transformation limit
//! * linear and non-linear transition styles are supported via setTransitionStyle()
class lgl_TransitionNode: public lgl_AnimationNode
{
public:

   //! transition style
   enum lgl_transition_enum
   {
      LGL_TRANSITION_NEAREST,
      LGL_TRANSITION_LINEAR,
      LGL_TRANSITION_COS,
      LGL_TRANSITION_ATAN
   };

   //! ctor
   lgl_TransitionNode(double delta, const vec3 &vector,
                      const std::string &id = "",
                      lgl_Node *node = NULL)
      : lgl_AnimationNode(delta, vector, 0.5, 0, 0, id, node),
        style_(LGL_TRANSITION_LINEAR)
   {
      assert(delta != 0);

      stopOnLimit();
      signalOnStop();

      animation_ = -0.5;
      velocity_ = delta;
      lgl_AnimationNode::pause();
   }

   virtual std::string getClassId() const {return("Transition");}

   //! is the node enabled?
   bool enabled() const
   {
      return(animation() >= 0);
   }

   //! is the node disabled?
   bool disabled() const
   {
      return(animation() < 0);
   }

   //! enable the node
   void enable(bool yes = true)
   {
      if (enabled() != yes)
      {
         lgl_AnimationNode::restart(yes?0.5:-0.5);
         lgl_AnimationNode::pause();
      }
   }

   //! disable the node
   void disable(bool yes = true)
   {
      if (disabled() != yes)
      {
         lgl_AnimationNode::restart(yes?-0.5:0.5);
         lgl_AnimationNode::pause();
      }
   }

   //! toggle the node
   void toggle()
   {
      enable(disabled());
   }

   void restart(double animation = 0)
   {}

   virtual void pause(bool yes = true)
   {}

   virtual void resume(bool yes = true)
   {}

   virtual void setSpeedup(double speedup = 1)
   {}

   virtual void control(bool override = true)
   {
      switch (getControlMode())
      {
         case LGL_CONTROL_ON: if (disabled()) {lgl_AnimationNode::restart(-0.5); lgl_AnimationNode::resume();} break;
         case LGL_CONTROL_OFF: if (enabled()) {lgl_AnimationNode::restart(0.5); lgl_AnimationNode::resume();} break;
         case LGL_CONTROL_ENABLE: if (enabled() != override) {lgl_AnimationNode::restart(override?-0.5:0.5); lgl_AnimationNode::resume();} break;
         case LGL_CONTROL_DISABLE: if (disabled() != override) {lgl_AnimationNode::restart(override?0.5:-0.5); lgl_AnimationNode::resume();} break;
         case LGL_CONTROL_INVERT: lgl_AnimationNode::resume(); break;
         default: lgl_AnimationNode::control(override); break;
      }
   }

   virtual void steer(double level, bool override = true)
   {
      switch (getSteerMode())
      {
         case LGL_STEER_RESTART: lgl_AnimationNode::restart(level<0.5?-0.5:0.5); break;
         case LGL_STEER_RESTART_RESUME: lgl_AnimationNode::restart(level<0.5?-0.5:0.5); if (override) lgl_AnimationNode::resume(); break;
         default: lgl_AnimationNode::steer(level, override); break;
      }
   }

   //! get transition style
   lgl_transition_enum getTransitionStyle() const
   {
      return(style_);
   }

   //! set transition style
   void setTransitionStyle(lgl_transition_enum style)
   {
      style_ = style;
   }

   //! finish ongoing transition
   virtual void finish()
   {
      if (!paused())
      {
         lgl_AnimationNode::restart(velocity_>0?0.5:-0.5);
         lgl_AnimationNode::pause();

         transformation_ = animate(animation_, vector_);
         identity_ = false;

         if (signal_)
            signal_delayed();
      }
   }

protected:

   lgl_transition_enum style_;

   double transition(double w) const
   {
      static const double c = 2;

      switch (getTransitionStyle())
      {
         case LGL_TRANSITION_NEAREST: return(w<0.5?0:1);
         case LGL_TRANSITION_LINEAR: return(w);
         case LGL_TRANSITION_COS: return(0.5-0.5*cos(w*PI));
         case LGL_TRANSITION_ATAN: return(0.5*atan(c*(w-0.5))/atan(c*0.5)+0.5);
      }

      return(w);
   }

   virtual void pre_update(double dt)
   {
      if (dt == 0)
         finish();

      lgl_AnimationNode::pre_update(dt);
   }

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_AnimationNode::to_string(classname);

      if (classname)
      {
         s << "(";

         switch (getTransitionStyle())
         {
            case LGL_TRANSITION_NEAREST: s << "nearest"; break;
            case LGL_TRANSITION_LINEAR: s << "linear"; break;
            case LGL_TRANSITION_COS: s << "cos-lerp"; break;
            case LGL_TRANSITION_ATAN: s << "atan-lerp"; break;
         }

         s << ")";
      }

      return(s.str());
   }

   virtual std::string to_label() const
   {
      lgl_string label = lgl_AnimationNode::to_label();

      switch (getTransitionStyle())
      {
         case LGL_TRANSITION_NEAREST: label += "\\nnearest"; break;
         case LGL_TRANSITION_LINEAR: label += "\\nlinear"; break;
         case LGL_TRANSITION_COS: label += "\\ncos-lerp"; break;
         case LGL_TRANSITION_ATAN: label += "\\natan-lerp"; break;
      }

      return(label.strip("\\n"));
   }

};

//! animation node (translation transition)
//!
//! a translation transition node performs a single continuous translation operation and then stops after reaching a certain translation distance
class lgl_TranslationTransitionNode: public lgl_TransitionNode
{
public:

   //! ctor
   lgl_TranslationTransitionNode(const vec3 &translation, double seconds,
                                 const std::string &id = "",
                                 lgl_Node *node = NULL)
      : lgl_TransitionNode(fabs(1/seconds), translation, id, node)
   {}

   virtual std::string getClassId() const {return("TranslationTransition");}

protected:

   virtual mat4 animate(double animation, const vec3 &vector)
   {
      double w = transition(animation + 0.5);
      return(mat4::translate(w * vector));
   }

public:

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_TransitionNode::to_label() + "\\nT").strip("\\n"));
   }

};

//! animation node (rotation transition)
//!
//! a rotation transition node performs a single continuous rotation operation and then stops after reaching a certain rotation angle
class lgl_RotationTransitionNode: public lgl_TransitionNode
{
public:

   //! ctor
   lgl_RotationTransitionNode(double angle, const vec3 &axis, double seconds,
                              const std::string &id = "",
                              lgl_Node *node = NULL)
      : lgl_TransitionNode(fabs(1/seconds), axis, id, node),
        angle_(angle)
   {}

   virtual std::string getClassId() const {return("RotationTransition");}

protected:

   double angle_;

   virtual mat4 animate(double animation, const vec3 &axis)
   {
      double w = transition(animation + 0.5);
      return(mat4::rotate(angle_ * w, axis));
   }

public:

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_TransitionNode::to_label() + "\\nR").strip("\\n"));
   }

};

//! animation node (scale transition)
//!
//! a scale transition node performs a single continuous scaling operation and then stops after reaching a certain scaling factor
class lgl_ScaleTransitionNode: public lgl_TransitionNode
{
public:

   //! ctor
   lgl_ScaleTransitionNode(double scale, double seconds,
                           const vec3 &axis = vec3(1),
                           const std::string &id = "",
                           lgl_Node *node = NULL)
      : lgl_TransitionNode(fabs(1/seconds), axis, id, node),
        from_(1), to_(scale)
   {}

   //! ctor
   lgl_ScaleTransitionNode(double from, double to, double seconds,
                           const vec3 &axis = vec3(1),
                           const std::string &id = "",
                           lgl_Node *node = NULL)
      : lgl_TransitionNode(fabs(1/seconds), axis, id, node),
        from_(from), to_(to)
   {}

   virtual std::string getClassId() const {return("ScaleTransition");}

protected:

   double from_, to_;

   virtual mat4 animate(double animation, const vec3 &axis)
   {
      double w = transition(animation + 0.5);
      double s = (1-w)*from_ + w*to_;
      return(mat4::scale(s * axis));
   }

public:

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_TransitionNode::to_label() + "\\nS").strip("\\n"));
   }

};

//! animation node (warp)
//!
//! a warp animation node warps all child nodes to the designated place
//! * the warp is performed from the unit scale z-axis to the direction vector between front and back
//! * the scaling along the z-axis is adjusted to the length of the direction vector
class lgl_WarpAnimationNode: public lgl_AnimationNode
{
public:

   //! ctor
   lgl_WarpAnimationNode(double omega,
                         bool scale = true,
                         const std::string &id = "",
                         lgl_Node *node = NULL)
      : lgl_AnimationNode(omega, vec3(0), 0, 0, 0, id, node),
        scale_(scale)
   {}

   virtual std::string getClassId() const {return("WarpAnimation");}

protected:

   bool scale_;

   virtual void init()
   {
      restart(0);
   }

   virtual mat4 animate(double animation, const vec3 &axis)
   {
      vec3 f = front(animation);
      vec3 b = back(animation);
      vec3 u = up(animation);

      return(mat4::transform(b, f, u, scale_));
   }

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_AnimationNode::to_label() + "\\nW").strip("\\n"));
   }

   virtual vec3 front(double animation) = 0;
   virtual vec3 back(double animation) = 0;
   virtual vec3 up(double animation) = 0;
};

inline bool lgl_TransformationNode::optimize()
{
   bool optimizable = true;

   if (dynamic_cast<lgl_AnimationNode*>(this))
      optimizable = false;

   bool textrans = false;

   if (dynamic_cast<lgl_TextureTransformationNode*>(this))
      textrans = true;

   for (unsigned int i=0; i<children_.size(); i++)
      if (follow(i))
      {
         lgl_Node *node = get(i);
         lgl_TransformationNode *trans = dynamic_cast<lgl_TransformationNode*>(node);
         lgl_VBONode *vbo = dynamic_cast<lgl_VBONode*>(node);

         if (!trans && !vbo)
            optimizable = false;
         else if (trans)
         {
            if (dynamic_cast<lgl_AnimationNode*>(trans))
               optimizable = false;

            if (!textrans && dynamic_cast<lgl_TextureTransformationNode*>(trans))
               optimizable = false;

            if (textrans && !dynamic_cast<lgl_TextureTransformationNode*>(trans))
               optimizable = false;
         }
         else
         {
            if (!node->leaf())
               optimizable = false;
         }
      }

   if (optimizable)
      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
         {
            lgl_Node *node = get(i);

            if (lgl_TransformationNode *trans = dynamic_cast<lgl_TransformationNode*>(node))
            {
               mat4 m = getTransformation() * trans->getTransformation();
               trans->setTransformation(m);
            }
            else if (lgl_VBONode *vbo = dynamic_cast<lgl_VBONode*>(node))
            {
               if (!textrans)
               {
                  mat4 m = getTransformation();
                  vbo->lglModel(m);
                  vbo->lglApplyModelMatrix();
               }
               else
               {
                  mat4 m = lglGetTexMatrix();
                  vbo->lglTex(m);
                  vbo->lglApplyTexMatrix();
               }
            }
         }

   return(optimizable);
}

inline void lgl_ActionNode::trigger_children(bool override)
{
   if (enabled_)
   {
      for (unsigned int i=0; i<children_.size(); i++)
      {
         if (lgl_AnimationNode *anim=dynamic_cast<lgl_AnimationNode*>(children_[i]))
            switch (getActionMode())
            {
               case LGL_ACTION_PAUSE: anim->pause(override); break;
               case LGL_ACTION_RESUME: anim->resume(override); break;
               case LGL_ACTION_DIRECTION: anim->direction(override); break;
               default: break;
            }
         else if (lgl_ControllableNode *ctrl=dynamic_cast<lgl_ControllableNode*>(children_[i]))
            switch (getActionMode())
            {
               case LGL_ACTION_ON: ctrl->enable(); break;
               case LGL_ACTION_OFF: ctrl->disable(); break;
               case LGL_ACTION_ENABLE: ctrl->enable(override); break;
               case LGL_ACTION_DISABLE: ctrl->disable(override); break;
               case LGL_ACTION_HIDE: ctrl->hide(override); break;
               case LGL_ACTION_SHOW: ctrl->show(override); break;
               default: break;
            }
      }
   }
}

//! light node
//!
//! a light node stores the light vector of a single directional or positional light source and the associated ambient, diffuse and specular lighting parameters
//! * the light vector can be specified either in world or camera coordinates
class lgl_LightNode: public lgl_ControllableNode
{
public:

   //! ctor
   lgl_LightNode(vec4f light = vec4f(0,0,1,0), bool camera_light = true,
                 vec3f ka = vec3f(0.1f), vec3f kd = vec3f(0.7f), vec3f ks = vec3f(0.2f),
                 vec3f Ia = vec3f(1), vec3f Id = vec3f(1), vec3f Is = vec3f(1),
                 float exponent = 30, vec3f falloff = vec3f(1,0,0),
                 const std::string &id = "",
                 lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node),
        light_(light),
        camera_light_(camera_light),
        ka_(ka), kd_(kd), ks_(ks),
        Ia_(Ia), Id_(Id), Is_(Is),
        exponent_(exponent),
        falloff_(falloff)
   {}

   virtual ~lgl_LightNode() {}

   virtual std::string getClassId() const {return("Light");}

   //! get the light vector
   vec3f getLightVector() const
   {
      return(light_);
   }

   //! set the light vector
   void setLightVector(vec4f light)
   {
      light_ = light;
   }

   //! set the light direction
   void setLightDirection(vec3f light)
   {
      light_ = vec4f(light, 0);
   }

   //! set the light position
   void setLightPosition(vec3f light)
   {
      light_ = vec4f(light, 1);
   }

   //! is the light source a camera light?
   bool isCameraLight() const
   {
      return(camera_light_);
   }

   //! get the light parameters
   void getLightParameters(vec3f &ka, vec3f &kd, vec3f &ks,
                           vec3f &Ia, vec3f &Id, vec3f &Is,
                           float &exponent, vec3f &falloff) const
   {
      ka = ka_;
      kd = kd_;
      ks = ks_;

      Ia = Ia_;
      Id = Id_;
      Is = Is_;

      exponent = exponent_;
      falloff = falloff_;
   }

   //! set the light parameters
   void setLightParameters(vec3f ka = vec3f(0.1f), vec3f kd = vec3f(0.7f), vec3f ks = vec3f(0.2f),
                           vec3f Ia = vec3f(1), vec3f Id = vec3f(1), vec3f Is = vec3f(1),
                           float exponent = 30, vec3f falloff = vec3f(1,0,0))
   {
      ka_ = ka;
      kd_ = kd;
      ks_ = ks;

      Ia_ = Ia;
      Id_ = Id;
      Is_ = Is;

      exponent_ = exponent;
      falloff_ = falloff;
   }

protected:

   vec4f light_;
   bool camera_light_;
   vec3f ka_, kd_, ks_;
   vec3f Ia_, Id_, Is_;
   float exponent_;
   vec3f falloff_;

   virtual void render()
   {
      if (hidden_) return;

      GL *gl = getGL();

      bool lighting(false);
      vec4f light(0);
      vec3f ka(0), kd(0), ks(0);
      vec3f Ia(0), Id(0), Is(0);
      float exponent(0);
      vec3f falloff(0);

      if (enabled_)
      {
         lighting = gl->lglGetLighting();
         light = gl->lglGetLightVector();
         gl->lglGetLightParameters(ka, kd, ks, Ia, Id, Is, exponent, falloff);

         gl->lglLighting(true);
         gl->lglLightVector(light_, camera_light_);
         gl->lglLightParameters(ka_, kd_, ks_, Ia_, Id_, Is_, exponent_, falloff_);
      }

      lgl_Node::render();

      if (enabled_)
      {
         gl->lglLighting(lighting);
         gl->lglLightVector(light);
         gl->lglLightParameters(ka, kd, ks, Ia, Id, Is, exponent, falloff);
      }
   }

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_ControllableNode::to_string(classname);

      s << "(";
      s << "vec4" << getLightVector() << ", " << (isCameraLight()?"cam":"world") << ", ";
      s << "vec3" << ka_ << ", " << "vec3" << kd_ << "vec3" << ks_ << ", ";
      s << "vec3" << Ia_ << ", " << "vec3" << Id_ << "vec3" << Is_ << ", ";
      s << exponent_ << ", " << "vec3" << falloff_;
      s << ")";

      return(s.str());
   }

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_ControllableNode::to_label() + "\\n" + lgl_string(getClassId()).toLower(' ')).strip("\\n"));
   }

};

//! light source node
//!
//! a light node stores the lighting parameters of a single directional or positional light source
class lgl_LightSourceNode: public lgl_ControllableNode, public lgl_LightSource
{
public:

   //! ctor
   lgl_LightSourceNode(vec4f light = vec4f(0,0,1,0), bool camera_light = true,
                       vec3f Ia = vec3f(1), vec3f Id = vec3f(1), vec3f Is = vec3f(1),
                       vec3f falloff = vec3f(1,0,0),
                       const std::string &id = "",
                       lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node), lgl_LightSource()
   {
      setLightVector(light, camera_light);
      setLightSourceParameters(Ia, Id, Is, falloff);
   }

   virtual ~lgl_LightSourceNode() {}

   virtual std::string getClassId() const {return("LightSource");}

protected:

   virtual void render()
   {
      if (hidden_) return;

      GL *gl = getGL();

      bool lighting(false);
      vec4f light(0);
      vec3f Ia(0), Id(0), Is(0);
      vec3f falloff(0);

      if (enabled_)
      {
         lighting = gl->lglGetLighting();
         light = gl->lglGetLightVector();
         gl->lglGetLightSourceParameters(Ia, Id, Is, falloff);

         gl->lglLighting(true);
         gl->lglLightVector(light_, camera_light_);
         gl->lglLightSourceParameters(Ia_, Id_, Is_, falloff_);
      }

      lgl_Node::render();

      if (enabled_)
      {
         gl->lglLighting(lighting);
         gl->lglLightVector(light);
         gl->lglLightSourceParameters(Ia, Id, Is, falloff);
      }
   }

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_ControllableNode::to_string(classname);

      s << "(";
      s << "vec4" << getLightVector() << ", " << (isCameraLight()?"cam":"world") << ", ";
      s << "vec3" << Ia_ << ", " << "vec3" << Id_ << "vec3" << Is_ << ", ";
      s << "vec3" << falloff_;
      s << ")";

      return(s.str());
   }

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_ControllableNode::to_label() + "\\n" + lgl_string(getClassId()).toLower(' ')).strip("\\n"));
   }

};

//! light material node
//!
//! a light node stores the ambient, diffuse and specular material reflectance parameters
class lgl_MaterialNode: public lgl_ControllableNode, public lgl_Material
{
public:

   //! ctor
   lgl_MaterialNode(vec3f ka = vec3f(0.1f), vec3f kd = vec3f(0.7f), vec3f ks = vec3f(0.2f),
                    float exponent = 30,
                    const std::string &id = "",
                    lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node), lgl_Material()
   {
      setMaterialParameters(ka, kd, ks, exponent);
   }

   virtual ~lgl_MaterialNode() {}

   virtual std::string getClassId() const {return("Material");}

protected:

   virtual void render()
   {
      if (hidden_) return;

      GL *gl = getGL();

      vec3f ka(0), kd(0), ks(0);
      float exponent(0);

      if (enabled_)
      {
         gl->lglGetMaterialParameters(ka, kd, ks, exponent);
         gl->lglMaterialParameters(ka_, kd_, ks_, exponent_);
      }

      lgl_Node::render();

      if (enabled_)
      {
        gl->lglMaterialParameters(ka, kd, ks, exponent);
      }
   }

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_ControllableNode::to_string(classname);

      s << "(";
      s << "vec3" << ka_ << ", " << "vec3" << kd_ << "vec3" << ks_ << ", ";
      s << exponent_;
      s << ")";

      return(s.str());
   }

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_ControllableNode::to_label() + "\\n" + lgl_string(getClassId()).toLower(' ')).strip("\\n"));
   }

};

//! 2D texture node
//!
//! a 2D texture node stores the texture id of a 2D texture object to be enabled for all its child nodes
class lgl_Texture2DNode: public lgl_ControllableNode
{
public:

   //! ctor
   lgl_Texture2DNode(GLuint texid = 0,
                     const std::string &id = "",
                     lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node),
        texid_(texid)
   {}

   virtual ~lgl_Texture2DNode() {}

   virtual std::string getClassId() const {return("Texture2D");}

   GLuint getTexId() const
   {
      return(texid_);
   }

   void setTexId(GLuint texid)
   {
      texid_ = texid;
   }

protected:

   GLuint texid_;

   virtual void render()
   {
      if (hidden_) return;

      GL *gl = getGL();

      bool texturing = false;
      GLuint texid2D = 0;
      GLuint texid3D = 0;

      if (texid_!=0 && enabled_)
      {
         texturing = gl->lglGetTexturing();
         texid2D = gl->lglGetTexture2D();
         texid3D = gl->lglGetTexture3D();

         gl->lglTexture2D(texid_);
         gl->lglTexturing(true);
      }

      lgl_Node::render();

      if (texid_!=0 && enabled_)
      {
         gl->lglTexturing(texturing);
         gl->lglTexture2D(texid2D);
         gl->lglTexture3D(texid3D);
      }
   }

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_ControllableNode::to_string(classname);

      if (getTexId() != 0)
         s << "(" << getTexId() << ")";

      return(s.str());
   }

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_ControllableNode::to_label() + "\\n" + lgl_string(getClassId()).toLower(' ')).strip("\\n"));
   }

};

//! 3D texture node
//!
//! a 3D texture node stores the texture id of a 3D texture object to be enabled for all its child nodes
class lgl_Texture3DNode: public lgl_ControllableNode
{
public:

   //! ctor
   lgl_Texture3DNode(GLuint texid = 0,
                     const std::string &id = "",
                     lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node),
        texid_(texid)
   {}

   virtual ~lgl_Texture3DNode() {}

   virtual std::string getClassId() const {return("Texture3D");}

   GLuint getTexId() const
   {
      return(texid_);
   }

   void setTexId(GLuint texid)
   {
      texid_ = texid;
   }

protected:

   GLuint texid_;

   virtual void render()
   {
      if (hidden_) return;

      GL *gl = getGL();

      bool texturing = false;
      GLuint texid2D = 0;
      GLuint texid3D = 0;

      if (texid_!=0 && enabled_)
      {
         texturing = gl->lglGetTexturing();
         texid2D = gl->lglGetTexture2D();
         texid3D = gl->lglGetTexture3D();

         gl->lglTexture3D(texid_);
         gl->lglTexturing(true);
      }

      lgl_Node::render();

      if (texid_!=0 && enabled_)
      {
         gl->lglTexturing(texturing);
         gl->lglTexture2D(texid2D);
         gl->lglTexture3D(texid3D);
      }
   }

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_ControllableNode::to_string(classname);

      if (getTexId() != 0)
         s << "(" << getTexId() << ")";

      return(s.str());
   }

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_ControllableNode::to_label() + "\\n" + lgl_string(getClassId()).toLower(' ')).strip("\\n"));
   }

};

//! state change node (base class)
//!
//! a state node is a base class for switching a particular rendering state on or off
class lgl_StateNode: public lgl_ControllableNode
{
public:

   lgl_StateNode(bool state = true,
                 const std::string &id = "",
                 lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node),
        state_(state)
   {}

   virtual ~lgl_StateNode() {}

   virtual std::string getClassId() const {return("State");}

   bool state() const
   {
      return(state_);
   }

   void change(bool state)
   {
      state_ = state;
   }

protected:

   bool state_;

   virtual void render()
   {
      if (hidden_) return;

      bool state = false;

      if (enabled_)
      {
         state = saveState();
         setState(state_);
      }

      lgl_Node::render();

      if (enabled_)
      {
         restoreState(state);
      }
   }

   virtual bool getState() const = 0;
   virtual void setState(bool state) = 0;

   virtual bool saveState() {return(getState());}
   virtual void restoreState(bool state) {setState(state);}

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_ControllableNode::to_string(classname);
      s << "(" << (state()?"on":"off") << ")";

      return(s.str());
   }

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_ControllableNode::to_label() + "\\n" + lgl_string(getClassId()).toLower(' ') + " " + (state()?"on":"off")).strip("\\n"));
   }

};

//! state change node (depth write)
//!
//! a depth write node switches depth writing on or off
class lgl_DepthWriteNode: public lgl_StateNode
{
public:

   lgl_DepthWriteNode(bool state = true,
                      const std::string &id = "",
                      lgl_Node *node = NULL)
      : lgl_StateNode(state, id, node)
   {}

   virtual std::string getClassId() const {return("DepthWrite");}

protected:

   virtual bool getState() const
   {
      return(lglGetZWrite());
   }

   virtual void setState(bool state)
   {
      lglZWrite(state);
   }

};

//! state change node (depth test)
//!
//! a depth test node switches depth testing on or off
class lgl_DepthTestNode: public lgl_StateNode
{
public:

   lgl_DepthTestNode(bool state = true,
                     const std::string &id = "",
                     lgl_Node *node = NULL)
      : lgl_StateNode(state, id, node)
   {}

   virtual std::string getClassId() const {return("DepthTest");}

protected:

   virtual bool getState() const
   {
      return(lglGetDepthTest());
   }

   virtual void setState(bool state)
   {
      lglDepthTest(state);
   }

};

//! state change node (back-face culling)
//!
//! a back-face culling node switches back-face culling on or off
class lgl_BackFaceCullingNode: public lgl_StateNode
{
public:

   lgl_BackFaceCullingNode(bool state = true,
                           const std::string &id = "",
                           lgl_Node *node = NULL)
      : lgl_StateNode(state, id, node)
   {}

   virtual std::string getClassId() const {return("BackFaceCulling");}

protected:

   virtual bool getState() const
   {
      return(lglGetBackFaceCulling());
   }

   virtual void setState(bool state)
   {
      lglBackFaceCulling(state);
   }

};

//! state change node (blending)
//!
//! a blending node switches blending on or off
class lgl_BlendingNode: public lgl_StateNode
{
public:

   lgl_BlendingNode(lgl_blendmode_enum mode = LGL_BLEND_ADD,
                    bool state = true,
                    const std::string &id = "",
                    lgl_Node *node = NULL)
      : lgl_StateNode(state, id, node),
        blend_mode_(mode)
   {}

   virtual std::string getClassId() const {return("Blending");}

   //! get the blend mode
   lgl_blendmode_enum getBlendMode() const
   {
      return(blend_mode_);
   }

protected:

   lgl_blendmode_enum blend_mode_;

   virtual bool getState() const
   {
      return(lglGetBlending());
   }

   virtual void setState(bool state)
   {
      lglBlendMode(blend_mode_);
   }

   virtual bool saveState()
   {
      saved_blend_mode_ = lglGetBlendMode();
      return(getState());
   }

   virtual void restoreState(bool state)
   {
      lglBlendMode(saved_blend_mode_);
   }

   lgl_blendmode_enum saved_blend_mode_;

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      std::string mode;

      switch (getBlendMode())
      {
         case LGL_BLEND_NONE : mode = "none"; break;
         case LGL_BLEND_MULT : mode = "mult"; break;
         case LGL_BLEND_ALPHA : mode = "alpha"; break;
         case LGL_BLEND_ADD : mode = "add"; break;
         case LGL_BLEND_SUB : mode = "sub"; break;
         case LGL_BLEND_MAX : mode = "max"; break;
         case LGL_BLEND_MIN : mode = "min"; break;
      }

      s << lgl_ControllableNode::to_string(classname);
      s << "(" << mode << ")";

      return(s.str());
   }

};

//! state change node (alpha testing)
//!
//! an alpha test node switches alpha testing on or off
class lgl_AlphaTestNode: public lgl_StateNode
{
public:

   lgl_AlphaTestNode(float alpha_value = 0,
                     bool state = true,
                     bool greater = true,
                     bool equal = false,
                     const std::string &id = "",
                     lgl_Node *node = NULL)
      : lgl_StateNode(state, id, node),
        alpha_value_(alpha_value),
        alpha_greater_(greater),
        alpha_equal_(equal)
   {}

   virtual std::string getClassId() const {return("AlphaTest");}

   //! get the alpha test value
   float getAlphaTestValue() const
   {
      return(alpha_value_);
   }

   //! get the alpha test comparison mode
   bool getAlphaTestGreater() const
   {
      return(alpha_greater_);
   }

   //! get the alpha test equality mode
   bool getAlphaTestEqual() const
   {
      return(alpha_equal_);
   }

protected:

   float alpha_value_;
   bool alpha_greater_;
   bool alpha_equal_;

   virtual bool getState() const
   {
      return(lglGetAlphaTest());
   }

   virtual void setState(bool state)
   {
      lglAlphaTest(state, alpha_value_, alpha_greater_, alpha_equal_);
   }

   virtual bool saveState()
   {
      saved_alpha_value_ = lglGetAlphaTestValue();
      saved_alpha_greater_ = lglGetAlphaTestGreater();
      saved_alpha_equal_ = lglGetAlphaTestEqual();
      return(getState());
   }

   virtual void restoreState(bool state)
   {
      lglAlphaTest(state, saved_alpha_value_, saved_alpha_greater_, saved_alpha_equal_);
   }

   float saved_alpha_value_;
   bool saved_alpha_greater_;
   bool saved_alpha_equal_;

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_StateNode::to_string(classname);

      if (state())
         s << "(" << getAlphaTestValue() << ", "
                  << (getAlphaTestGreater()?"greater":"less") << ", "
                  << (getAlphaTestEqual()?"equal":"inequal") << ")";

      return(s.str());
   }

};

//! state change node (fogging)
//!
//! a fog node switches fogging on or off
class lgl_FogNode: public lgl_StateNode
{
public:

   lgl_FogNode(float fog_density = 0,
               vec4f fog_color = vec4f(1,1,1),
               bool state = true,
               const std::string &id = "",
               lgl_Node *node = NULL)
      : lgl_StateNode(state, id, node),
        fog_density_(fog_density),
        fog_color_(fog_color)
   {}

   virtual std::string getClassId() const {return("Fog");}

   //! get the fog density
   float getFogDensity() const
   {
      return(fog_density_);
   }

   //! get the fog color
   vec4f getFogColor() const
   {
      return(fog_color_);
   }

protected:

   float fog_density_;
   vec4f fog_color_;

   virtual bool getState() const
   {
      return(lglGetFog());
   }

   virtual void setState(bool state)
   {
      if (state)
         lglFog(fog_density_, fog_color_);
      else
         lglFog();
   }

   virtual bool saveState()
   {
      saved_fog_density_ = lglGetFogDensity();
      saved_fog_color_ = lglGetFogColor();
      return(getState());
   }

   virtual void restoreState(bool state)
   {
      lglFog(saved_fog_density_, saved_fog_color_);
   }

   float saved_fog_density_;
   vec4f saved_fog_color_;

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_StateNode::to_string(classname);

      if (state())
         s << "(" << getFogDensity() << ", vec4" << getFogColor() << ")";

      return(s.str());
   }

};

//! state change node (line width)
//!
//! a line width node sets the line width for subsequent line renderings
class lgl_LineWidthNode: public lgl_StateNode
{
public:

   lgl_LineWidthNode(float width = 1,
                     const std::string &id = "",
                     lgl_Node *node = NULL)
      : lgl_StateNode(true, id, node),
        line_width_(width)
   {}

   virtual std::string getClassId() const {return("LineWidth");}

   //! get the line width
   float getLineWidth() const
   {
      return(line_width_);
   }

protected:

   float line_width_;

   virtual bool getState() const
   {
      return(true);
   }

   virtual void setState(bool state)
   {}

   virtual bool saveState()
   {
      saved_line_width_ = lglGetLineWidth();
      return(getState());
   }

   virtual void restoreState(bool state)
   {
      lglLineWidth(saved_line_width_);
   }

   float saved_line_width_;

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_ControllableNode::to_string(classname);
      s << "(" << getLineWidth() << ")";

      return(s.str());
   }

};

//! state change node (coloring)
//!
//! a coloring node enables or disables the color attribute of vbo child nodes
class lgl_ColoringNode: public lgl_StateNode
{
public:

   lgl_ColoringNode(bool state = true,
                    const std::string &id = "",
                    lgl_Node *node = NULL)
      : lgl_StateNode(state, id, node)
   {}

   virtual std::string getClassId() const {return("Coloring");}

protected:

   virtual bool getState() const
   {
      GL *gl = getGL();

      return(gl->lglGetColoring());
   }

   virtual void setState(bool state)
   {
      GL *gl = getGL();

      gl->lglColoring(state);
   }

};

//! state change node (lighting)
//!
//! a lighting node enables or disables the normal attribute of vbo child nodes
class lgl_LightingNode: public lgl_StateNode
{
public:

   lgl_LightingNode(bool state = true,
                    const std::string &id = "",
                    lgl_Node *node = NULL)
      : lgl_StateNode(state, id, node)
   {}

   virtual std::string getClassId() const {return("Lighting");}

protected:

   virtual bool getState() const
   {
      GL *gl = getGL();

      return(gl->lglGetLighting());
   }

   virtual void setState(bool state)
   {
      GL *gl = getGL();

      gl->lglLighting(state);
   }

};

//! state change node (texturing)
//!
//! a texturing node enables or disables the texture coordinate attribute of vbo child nodes
class lgl_TexturingNode: public lgl_StateNode
{
public:

   lgl_TexturingNode(bool state = true,
                     const std::string &id = "",
                     lgl_Node *node = NULL)
      : lgl_StateNode(state, id, node)
   {}

   virtual std::string getClassId() const {return("Texturing");}

protected:

   virtual bool getState() const
   {
      GL *gl = getGL();

      return(gl->lglGetTexturing());
   }

   virtual void setState(bool state)
   {
      GL *gl = getGL();

      gl->lglTexturing(state);
   }

};

//! clipping node
//!
//! a clipping node enables or disables one a clip plane for vbo child nodes
//! * multiple clip planes are supported
class lgl_ClippingNode: public lgl_ControllableNode
{
public:

   //! ctor
   lgl_ClippingNode(vec4 equation = vec4(0,0,0,0),
                    unsigned int n = 0,
                    const std::string &id = "",
                    lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node),
        clipplane_(equation),
        n_(n)
   {}

   //! ctor
   lgl_ClippingNode(vec3 point, vec3 normal,
                    unsigned int n = 0,
                    const std::string &id = "",
                    lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node),
        clipplane_(lglGetClipPlaneEquation(point, normal)),
        n_(n)
   {}

   //! ctor
   lgl_ClippingNode(unsigned int n,
                    const std::string &id = "",
                    lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node),
        clipplane_(vec4(0,0,0,0)),
        n_(n)
   {}

   virtual ~lgl_ClippingNode() {}

   virtual std::string getClassId() const {return("Clipping");}

   //! get the clip plane equation
   vec4 getEquation() const
   {
      return(clipplane_);
   }

   //! set the clip plane equation
   void setEquation(vec4 equation)
   {
      clipplane_ = equation;
   }

   //! set the clip plane
   void setClipPlane(vec3 point, vec3 normal)
   {
      clipplane_ = lglGetClipPlaneEquation(point, normal);
   }

   //! get the clip plane number
   unsigned int getClipPlaneNumber() const
   {
      return(n_);
   }

protected:

   vec4 clipplane_;
   unsigned int n_;

   virtual void render()
   {
      if (hidden_) return;

      vec4 equation(0,0,0,0);

      if (enabled_)
      {
         equation = lglGetClipPlaneEquation(n_);
         lglClipPlane(clipplane_, n_, false);
      }

      lgl_Node::render();

      if (enabled_)
      {
         lglClipPlane(equation, n_, true);
      }
   }

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_ControllableNode::to_string(classname);

      if (getEquation() != vec4(0,0,0,0))
         s << "(vec4" << getEquation() << ", " << getClipPlaneNumber() << ")";
      else
         s << "(off, " << getClipPlaneNumber() << ")";

      return(s.str());
   }

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_ControllableNode::to_label() + "\\n" + lgl_string(getClassId()).toLower(' ') + " #" + glslmath::to_string(getClipPlaneNumber()) + " " + (getEquation()!=vec4(0,0,0,0)?"on":"off")).strip("\\n"));
   }

};

inline bool lgl_ColorNode::swappable()
{
   bool swappable = false;

   if (children() == 1)
      if (dynamic_cast<lgl_StateNode*>(get()) ||
          dynamic_cast<lgl_ClippingNode*>(get()))
         swappable = true;

   return(swappable);
}

inline bool lgl_TransformationNode::swappable()
{
   bool swappable = false;

   if (children() == 1)
      if (dynamic_cast<lgl_StateNode*>(get()) ||
          dynamic_cast<lgl_ClippingNode*>(get()))
         swappable = true;

   return(swappable);
}

//! switcher node
//!
//! a switcher node enables a single child node, the number of which is determined by the setSwitch() method
class lgl_SwitcherNode: public lgl_ControllableNode
{
public:

   lgl_SwitcherNode(const std::string &id = "",
                    lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node),
        switch_(0)
   {}

   virtual ~lgl_SwitcherNode() {}

   virtual std::string getClassId() const {return("Switcher");}

   //! get the switch index
   unsigned int getSwitch() const
   {
      return(switch_);
   }

   //! set the switch index
   void setSwitch(unsigned int i)
   {
      switch_ = i;
   }

   //! advance the switch index
   void advance(int delta = 1)
   {
      unsigned int n = children_.size();

      if (n > 0)
         switch_ = (switch_+n+delta)%n;
   }

   virtual bool ordered() const
   {
      return(true);
   }

protected:

   unsigned int switch_;

   virtual void render()
   {
      if (hidden_) return;

      if (enabled_)
         lgl_Node::render(switch_);
      else
         lgl_Node::render(0);
   }

   virtual void update(double dt)
   {
      if (enabled_)
         lgl_Node::update(switch_, dt);
      else
         lgl_Node::update(dt);
   }

   virtual void pick()
   {
      if (hidden_) return;

      if (enabled_)
         lgl_Node::pick(switch_);
      else
         lgl_Node::pick(0);
   }

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_ControllableNode::to_string(classname);

      if (classname)
         s << "(" << getSwitch() << ")";

      return(s.str());
   }

   virtual std::string to_label() const
   {
      return(lgl_string(lgl_ControllableNode::to_label() + "\\n" + lgl_string(getClassId()).toLower(' ')).strip("\\n"));
   }

};

//! level-of-detail node
//!
//! a level-of-detail node (LOD node) enables a single child node, the number of which is determined by the LOD distance parameter
class lgl_LevelOfDetailNode: public lgl_SwitcherNode
{
public:

   //! ctor
   lgl_LevelOfDetailNode(double distance = 0,
                         const std::string &id = "",
                         lgl_Node *node = NULL)
      : lgl_SwitcherNode(id, node),
        distance_(distance)
   {}

   virtual ~lgl_LevelOfDetailNode() {}

   virtual std::string getClassId() const {return("LevelOfDetail");}

   double getDistance() const
   {
      return(distance_);
   }

   void setDistance(double distance)
   {
      distance_ = distance;
   }

protected:

   double distance_;

   virtual void render()
   {
      if (hidden_) return;

      unsigned int i = 0;
      unsigned int n = children_.size();

      if (enabled_)
         if (distance_>0 && n>0)
         {
            static const double log2 = 1.0/log(2.0);

            mat4 mv = lglGetModelViewMatrix();
            vec3 o = mv * vec4(0);

            double d = o.length();
            double l = ceil(log(d/distance_)*log2);

            i = (unsigned int)l;
            if (i >= n) i = n-1;
         }

      setSwitch(i);
      lgl_SwitcherNode::render();
   }

public:

   virtual std::string to_string(bool classname = true) const
   {
      std::stringstream s;

      s << lgl_ControllableNode::to_string(classname);

      if (classname)
         s << "(" << distance_ << ")";

      return(s.str());
   }

};

//! subgraph node
//!
//! a subgraph node wraps a subgraph into a single node
class lgl_SubgraphNode: public lgl_ControllableNode
{
public:

   lgl_SubgraphNode(const std::string &id = "",
                    lgl_Node *node = NULL)
      : lgl_ControllableNode(id, node),
        graph_(NULL)
   {}

   virtual ~lgl_SubgraphNode()
   {
      if (graph_)
         delete graph_;
   }

   virtual std::string getClassId() const {return("Subgraph");}

   lgl_Node *getSubgraph()
   {
      if (!graph_)
         graph_ = createSubgraph();

      return(graph_);
   }

   void setSubgraph(lgl_Node *node)
   {
      if (graph_)
         delete graph_;

      graph_ = node;
   }

   virtual bool checkForCycles()
   {
      if (!graph_)
         graph_ = createSubgraph();

      if (graph_)
         if (graph_->checkForCycles())
            return(true);

      return(lgl_ControllableNode::checkForCycles());
   }

   virtual lgl_Node *find(const std::string &id)
   {
      if (!graph_)
         graph_ = createSubgraph();

      if (graph_)
         if (lgl_Node *node=graph_->find(id))
            return(node);

      return(lgl_ControllableNode::find(id));
   }

   virtual void getBoundingBox(vec3 &bboxmin, vec3 &bboxmax)
   {
      if (!graph_)
         graph_ = createSubgraph();

      lgl_ControllableNode::getBoundingBox(bboxmin, bboxmax);

      if (graph_ && enabled_)
      {
         vec3 bbmin, bbmax;
         graph_->getBoundingBox(bbmin, bbmax);

         growBoundingBox(bboxmin, bboxmax, bbmin);
         growBoundingBox(bboxmin, bboxmax, bbmax);
      }
   }

   virtual std::vector<vec4> getVertexCoordinates(mat4 trans = mat4())
   {
      if (!graph_)
         graph_ = createSubgraph();

      std::vector<vec4> geometry = lgl_ControllableNode::getVertexCoordinates(trans);

      if (graph_ && enabled_)
      {
         std::vector<vec4> geo = graph_->getVertexCoordinates(trans);
         geometry.insert(geometry.begin(), geo.begin(), geo.end());
      }

      return(geometry);
   }

   virtual bool saveSceneGraph(vec4 color, mat4 trans, FILE *file, int &index, int &nindex, int &tindex)
   {
      if (!graph_)
         graph_ = createSubgraph();

      bool ok = lgl_ControllableNode::saveSceneGraph(color, trans, file, index, nindex, tindex);

      if (graph_ && enabled_)
         if (!graph_->saveSceneGraph(color, trans, file, index, nindex, tindex)) ok = false;

      return(ok);
   }

   virtual unsigned int countAll()
   {
      unsigned int count = 0;

      if (!graph_)
         graph_ = createSubgraph();

      if (graph_)
         count += graph_->countAll();

      count += lgl_ControllableNode::countAll();

      return(count);
   }

   virtual unsigned int countAllOnce()
   {
      unsigned int count = 0;

      if (!graph_)
         graph_ = createSubgraph();

      if (graph_)
         count += graph_->countAllOnce();

      count += lgl_ControllableNode::countAllOnce();

      return(count);
   }

   virtual unsigned int countAllPrimitives()
   {
      unsigned int count = 0;

      if (!graph_)
         graph_ = createSubgraph();

      if (graph_)
         count += graph_->countAllPrimitives();

      count += lgl_ControllableNode::countAllPrimitives();

      return(count);
   }

   virtual unsigned int countAllVertices()
   {
      unsigned int count = 0;

      if (!graph_)
         graph_ = createSubgraph();

      if (graph_)
         count += graph_->countAllVertices();

      count += lgl_ControllableNode::countAllVertices();

      return(count);
   }

protected:

   virtual void cleanAll()
   {
      if (!graph_)
         graph_ = createSubgraph();

      if (graph_)
         graph_->cleanAll();

      lgl_ControllableNode::cleanAll();
   }

   virtual unsigned int countOnce()
   {
      unsigned int count = 0;

      if (!graph_)
         graph_ = createSubgraph();

      if (graph_)
         count += graph_->countOnce();

      count += lgl_ControllableNode::countOnce();

      return(count);
   }

public:

   virtual void optimizeAll()
   {
      if (!graph_)
         graph_ = createSubgraph();

      if (graph_)
         graph_->optimizeAll();

      lgl_ControllableNode::optimizeAll();
   }

   virtual void restartAll(double animation = 0)
   {
      if (!graph_)
         graph_ = createSubgraph();

      if (graph_)
         graph_->restartAll(animation);

      lgl_ControllableNode::restartAll(animation);
   }

   virtual void pauseAll(bool yes = true)
   {
      if (!graph_)
         graph_ = createSubgraph();

      if (graph_)
         graph_->pauseAll(yes);

      lgl_ControllableNode::pauseAll(yes);
   }

   virtual void speedupAll(double speedup = 1)
   {
      if (!graph_)
         graph_ = createSubgraph();

      if (graph_)
         graph_->speedupAll(speedup);

      lgl_ControllableNode::speedupAll(speedup);
   }

   virtual void finishAll()
   {
      if (!graph_)
         graph_ = createSubgraph();

      if (graph_)
         graph_->finishAll();

      lgl_ControllableNode::finishAll();
   }

protected:

   lgl_Node *graph_;

   virtual void init()
   {
      if (!graph_)
         graph_ = createSubgraph();
   }

   virtual void pre_update(double dt)
   {
      if (graph_)
         graph_->pre_update(dt);

      lgl_ControllableNode::pre_update(dt);
   }

   virtual void pre_render()
   {
      if (graph_)
         graph_->pre_render();

      lgl_ControllableNode::pre_render();
   }

   virtual void render()
   {
      if (hidden_) return;

      if (graph_ && enabled_)
         graph_->render();

      lgl_ControllableNode::render();
   }

   virtual void post_render()
   {
      if (graph_)
         graph_->post_render();

      lgl_ControllableNode::post_render();
   }

   virtual void update(double dt)
   {
      if (graph_)
         graph_->update(dt);

      lgl_ControllableNode::update(dt);
   }

   virtual void pick()
   {
      if (hidden_) return;

      if (graph_ && enabled_)
         graph_->pick();

      lgl_ControllableNode::pick();
   }

   virtual bool transparency()
   {
      if (!graph_)
         graph_ = createSubgraph();

      if (graph_ && enabled_)
         if (graph_->hasTransparency())
            return(true);

      return(false);
   }

   virtual lgl_Node *createSubgraph() {return(NULL);}
};

//! optimizing subgraph node
//!
//! an optimizing subgraph node wraps a subgraph into a single node
//! * the subgraph is optimized automatically
//! * the subgraph is also checked for cycles
class lgl_OptimizingSubgraphNode: public lgl_SubgraphNode
{
public:

   lgl_OptimizingSubgraphNode(const std::string &id = "",
                              lgl_Node *node = NULL)
      : lgl_SubgraphNode(id, node)
   {}

   virtual ~lgl_OptimizingSubgraphNode() {}

   virtual std::string getClassId() const {return("OptimizingSubgraph");}

protected:

   virtual lgl_Node *createSubgraph()
   {
      lgl_Node *graph = lgl_SubgraphNode::createSubgraph();

      if (graph)
      {
         graph->optimizeAll();

         if (graph->checkForCycles())
         {
            delete graph;
            graph = NULL;
         }
      }

      return(graph);
   }

};

inline std::string lgl_Node::exportAll(lgl_export_enum mode, int indentation, int increment)
{
   std::string s;
   unsigned int id = 0;

   bool tagged = false;

   // tag root
   if (getId() == "")
   {
      setId("root");
      tagged = true;
   }

   // init pass
   init_export(mode);
   scan_export(id, mode);

   // pre-export pass
   init_export(mode);
   s += pre_export(mode);

   // export pass
   init_export(mode);
   s += export_to_string(indentation, increment, mode);

   // post-export pass
   init_export(mode);
   s += post_export(mode);

   // cleanup pass
   clean_export(mode);

   // untag root
   if (tagged)
      setId();

   return(s);
}

inline void lgl_Node::init_export(lgl_export_enum mode)
{
   if (mode != LGL_EXPORT_COMMANDS)
      cleanAll();
}

inline void lgl_Node::scan_export(unsigned int &id, lgl_export_enum mode)
{
   if (mode != LGL_EXPORT_COMMANDS)
   {
      std::string prefix = "node-";
      if (mode == LGL_EXPORT_GRAPH) prefix = "#";

      if (visited_)
      {
         if (getAltId() == "")
         {
            id++;
            setAltId(prefix+glslmath::to_string(id));
         }
      }
      else
      {
         visited_ = true;

         if (mode == LGL_EXPORT_GRAPH)
         {
            if (getAltId() == "")
            {
               id++;
               setAltId(prefix+glslmath::to_string(id));
            }
         }

         for (unsigned int i=0; i<children_.size(); i++)
            if (follow(i))
               get(i)->scan_export(id, mode);
            else
            {
               if (get(i)->getAltId() == "")
               {
                  id++;
                  get(i)->setAltId(prefix+glslmath::to_string(id));
               }
            }
      }
   }
}

inline std::string lgl_Node::pre_export(lgl_export_enum mode)
{
   std::stringstream s;

   if (mode == LGL_EXPORT_COMMANDS)
   {
      lgl_CameraNode *camera = dynamic_cast<lgl_CameraNode*>(this);

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            s << get(i)->pre_export(mode);

      if (camera)
      {
         std::string node = camera->to_string(mode != LGL_EXPORT_COMMANDS);

         if (node != "")
            s << node << std::endl;
      }
   }
   else if (mode == LGL_EXPORT_GRAPH)
   {
      s << "digraph" << std::endl;
      s << "{" << std::endl;
      s << "node[shape=oval]" << std::endl;
      s << "edge[color=\"#000070\"]" << std::endl;
   }

   return(s.str());
}

inline std::string lgl_Node::export_to_string(int indentation, int increment, lgl_export_enum mode)
{
   std::stringstream s;

   if (visited_ && mode!=LGL_EXPORT_COMMANDS)
   {
      if (mode != LGL_EXPORT_GRAPH)
         s << glslmath::to_space(indentation) << "link(\"" << getAltId() << "\")" << std::endl;
   }
   else
   {
      if (mode != LGL_EXPORT_COMMANDS)
         visited_ = true;

      lgl_CameraNode *camera = dynamic_cast<lgl_CameraNode*>(this);
      lgl_TransformationNode *transformation = dynamic_cast<lgl_TransformationNode*>(this);
      lgl_TextureTransformationNode *textransformation = dynamic_cast<lgl_TextureTransformationNode*>(this);
      lgl_AnimationNode *animation = dynamic_cast<lgl_AnimationNode*>(this);
      lgl_SwitcherNode *switcher = dynamic_cast<lgl_SwitcherNode*>(this);
      lgl_LevelOfDetailNode *lod = dynamic_cast<lgl_LevelOfDetailNode*>(this);
      lgl_ControlNode *control = dynamic_cast<lgl_ControlNode*>(this);
      lgl_SteerNode *steer = dynamic_cast<lgl_SteerNode*>(this);
      lgl_BarrierNode *barrier = dynamic_cast<lgl_BarrierNode*>(this);
      lgl_ActionNode *action = dynamic_cast<lgl_ActionNode*>(this);
      lgl_ColorNode *color = dynamic_cast<lgl_ColorNode*>(this);
      lgl_GeometryNode *geo = dynamic_cast<lgl_GeometryNode*>(this);
      lgl_VBONode *vbo = dynamic_cast<lgl_VBONode*>(this);

      bool state = dynamic_cast<lgl_ColorNode*>(this) ||
                   dynamic_cast<lgl_LightNode*>(this) ||
                   dynamic_cast<lgl_Texture2DNode*>(this) ||
                   dynamic_cast<lgl_Texture3DNode*>(this) ||
                   dynamic_cast<lgl_StateNode*>(this) ||
                   dynamic_cast<lgl_ClippingNode*>(this);

      std::string node = to_string(mode != LGL_EXPORT_COMMANDS);
      bool empty = node.empty();

      unsigned int c = 0;
      unsigned int n = children();

      if (mode == LGL_EXPORT_NODES)
      {
         if (!empty)
         {
            if (n == 1)
               s << glslmath::to_space(indentation) << node << " ->" << std::endl;
            else
               s << glslmath::to_space(indentation) << node << std::endl;
         }

         if (n > 1)
            s << glslmath::to_space(indentation) << "{" << std::endl;

         for (unsigned int i=c; i<n; i++)
            if (follow(i))
               s << get(i)->export_to_string(indentation+increment, increment, mode);
            else
               s << glslmath::to_space(indentation+increment) << "link(\"" << get(i)->getAltId() << "\")" << std::endl;

         if (n > 1)
            s << glslmath::to_space(indentation) << "}" << std::endl;
      }
      else if (mode == LGL_EXPORT_GRAPH)
      {
         std::string name = getAltId();
         if (lgl_string(name).replaceNonAlphaNumeric() != name) name = "\"" + name + "\"";

         std::string shape;
         std::string label = to_label();

         if (camera) shape = "star";
         else if (geo || vbo) shape = "triangle";
         else if (animation) shape = "doublecircle";
         else if (textransformation) shape = "circle,label=\"\",width=0.25";
         else if (transformation) shape = "circle,width=0.25";
         else if (color)
         {
            shape = "circle,style=filled,fillcolor=";
            char buf[10];
            snprintf(buf, 10, "\"#%02x%02x%02x\"", int(255*color->getColor().r), int(255*color->getColor().g), int(255*color->getColor().b));
            shape += buf;
         }
         else if (state) shape = "box";
         else if (lod) shape = "tripleoctagon";
         else if (switcher) shape = "doubleoctagon";
         else if (steer) shape = "hexagon";
         else if (control) shape = "diamond";
         else if (barrier) shape = "octagon";
         else if (action) shape = "diamond";

         s << glslmath::to_space(indentation) << name;
         s << "[label=\"" << label << "\"";
         if (shape != "") s << ",shape=" << shape;
         s << "]" << std::endl;

         if (n > 0)
         {
            s << glslmath::to_space(indentation) << name << " -> ";

            if (n > 1) s << "{ ";

            for (unsigned int i=c; i<n; i++)
            {
               std::string link = get(i)->getAltId();
               if (lgl_string(link).replaceNonAlphaNumeric() != link) link = "\"" + link + "\"";

               s << link;
               if (i < n-1) s << " ";
            }

            if (n > 1) s << " }";

            if (control || action) s << " [style=dashed]";

            s << std::endl;
         }

         for (unsigned int i=c; i<n; i++)
            if (follow(i))
               s << get(i)->export_to_string(indentation+increment, increment, mode);
      }
      else
      {
         if (disabled())
            empty = true;

         if (hidden())
         {
            empty = true;
            n = 0;
         }

         if (camera)
         {
            empty = true;
         }
         else if (switcher)
         {
            empty = true;
            c = switcher->getSwitch();
            if (c < n) n = c+1;
            else n = 0;
         }
         else if (control || action)
         {
            empty = true;
            n = 0;
         }

         int inc = increment;
         if (n<=1 && !transformation && !state) inc = 0;
         if (empty) inc = 0;

         if (!empty)
         {
            if (textransformation)
               s << glslmath::to_space(indentation) << "push_tex_matrix" << std::endl;
            else if (transformation)
               s << glslmath::to_space(indentation) << "push_matrix" << std::endl;
            else if (state)
               s << glslmath::to_space(indentation) << "push_state" << std::endl;
         }

         if (!empty)
            s << glslmath::to_space(indentation+inc) << node << std::endl;

         for (unsigned int i=c; i<n; i++)
            if (follow(i))
               s << get(i)->export_to_string(indentation+inc, increment, mode);

         if (!empty)
         {
            if (textransformation)
               s << glslmath::to_space(indentation) << "pop_tex_matrix" << std::endl;
            else if (transformation)
               s << glslmath::to_space(indentation) << "pop_matrix" << std::endl;
            else if (state)
               s << glslmath::to_space(indentation) << "pop_state" << std::endl;
         }
      }
   }

   return(s.str());
}

inline std::string lgl_Node::post_export(lgl_export_enum mode)
{
   std::stringstream s;

   if (mode == LGL_EXPORT_GRAPH)
   {
      s << "}" << std::endl;
   }

   return(s.str());
}

inline void lgl_Node::clean_export(lgl_export_enum mode)
{
   if (mode != LGL_EXPORT_COMMANDS)
   {
      visited_ = false;

      if (getAltId() != "")
         setAltId();

      for (unsigned int i=0; i<children_.size(); i++)
         if (follow(i))
            get(i)->clean_export(mode);
         else
         {
            if (getAltId() != "")
               get(i)->setAltId();
         }
   }
}

inline void lgl_Node::exportNodes(std::string filename)
{
   lgl_string fn(filename);
   std::string suffix = fn.suffix(".");
   fn = fn.chop("."+suffix) + ".lgl";
   std::string graph = exportAll();
   lglWriteTextFile(fn, graph);
}

inline void lgl_Node::exportGraph(std::string filename)
{
   lgl_string fn(filename);
   std::string suffix = fn.suffix(".");
   fn = fn.chop("."+suffix) + ".dot";
   std::string graph = exportAll(lgl_Node::LGL_EXPORT_GRAPH, 0, 1);
   lglWriteTextFile(fn, graph);
}

inline void lgl_Node::exportCommands(std::string filename)
{
   lgl_string fn(filename);
   std::string suffix = fn.suffix(".");
   fn = fn.chop("."+suffix) + ".gl";
   std::string graph = exportAll(lgl_Node::LGL_EXPORT_COMMANDS, 0, 1);
   lglWriteTextFile(fn, graph);
}

#endif
