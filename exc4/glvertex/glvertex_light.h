// (c) by Stefan Roettger, licensed under MIT license

//! \file
//! LGL light classes
#ifndef GLVERTEX_LIGHT_H
#define GLVERTEX_LIGHT_H

#include "glvertex_core.h"

//! Blinn-Phong light source
class lgl_LightSource
{
public:

   lgl_LightSource()
      : light_(0,0,1,0),
        camera_light_(true),
        Ia_(1), Id_(1), Is_(1),
        falloff_(1,0,0)
   {}

   //! set light direction
   void setDirection(vec3f direction, bool camera_light = true)
   {
      light_ = vec4f(direction, 0);
      camera_light_ = camera_light;
   }

   //! set light position
   void setPosition(vec3f position, bool camera_light = true)
   {
      light_ = vec4f(position, 1);
      camera_light_ = camera_light;
   }

   //! set light vector
   void setLightVector(vec4f light, bool camera_light = true)
   {
      light_ = light;
      camera_light_ = camera_light;
   }

   //! get light vector
   vec3f getLightVector() const
   {
      return(light_);
   }

   //! is the light source a camera light?
   bool isCameraLight() const
   {
      return(camera_light_);
   }

   //! set ambient light source intensity
   void setAmbientColor(vec3f color)
   {
      Ia_ = color;
   }

   //! get ambient light source intensity
   vec3f getAmbientColor() const
   {
      return(Ia_);
   }

   //! set diffuse light source intensity
   void setDiffuseColor(vec3f color)
   {
      Id_ = color;
   }

   //! get diffuse light source intensity
   vec3f getDiffuseColor() const
   {
      return(Id_);
   }

   //! set specular light source intensity
   void setSpecularColor(vec3f color)
   {
      Is_ = color;
   }

   //! get specular light source intensity
   vec3f getSpecularColor() const
   {
      return(Is_);
   }

   //! set intensity falloff
   void setIntensityFalloff(vec3f falloff)
   {
      falloff_ = falloff;
   }

   //! get intensity falloff
   vec3f getIntensityFalloff() const
   {
      return(falloff_);
   }

   //! set the light source parameters
   void setLightSourceParameters(vec3f Ia = vec3f(1), vec3f Id = vec3f(1), vec3f Is = vec3f(1),
                                 vec3f falloff = vec3f(1,0,0))
   {
      Ia_ = Ia;
      Id_ = Id;
      Is_ = Is;

      falloff_ = falloff;
   }

   //! get the light source parameters
   void getLightSourceParameters(vec3f &Ia, vec3f &Id, vec3f &Is,
                                 vec3f &falloff) const
   {
      Ia = Ia_;
      Id = Id_;
      Is = Is_;

      falloff = falloff_;
   }

   //! activate light source
   void begin()
   {
      prev_light_ = lglGetLightVector();
      lglGetLightSourceParameters(prev_Ia_, prev_Id_, prev_Is_, prev_falloff_);

      lglLightVector(light_, camera_light_);
      lglLightSourceParameters(Ia_, Id_, Is_, falloff_);
   }

   //! deactivate light source
   void end()
   {
      lglLightVector(prev_light_);
      lglLightSourceParameters(prev_Ia_, prev_Id_, prev_Is_, prev_falloff_);
   }

protected:

   vec4f light_;
   bool camera_light_;
   vec3f Ia_, Id_, Is_;
   vec3f falloff_;

   vec4f prev_light_;
   vec3f prev_Ia_, prev_Id_, prev_Is_;
   vec3f prev_falloff_;
};

//! Blinn-Phong material
class lgl_Material
{
public:

   lgl_Material()
      : ka_(0.1f), kd_(0.7f), ks_(0.2f),
        exponent_(30)
   {}

   //! set ambient material reflectance
   void setAmbientColor(vec3f color)
   {
      ka_ = color;
   }

   //! get ambient material reflectance
   vec3f getAmbientColor() const
   {
      return(ka_);
   }

   //! set diffuse material reflectance
   void setDiffuseColor(vec3f color)
   {
      kd_ = color;
   }

   //! get diffuse material reflectance
   vec3f getDiffuseColor() const
   {
      return(kd_);
   }

   //! set specular material reflectance
   void setSpecularColor(vec3f color)
   {
      ks_ = color;
   }

   //! get specular material reflectance
   vec3f getSpecularColor() const
   {
      return(ks_);
   }

   //! set specular exponent
   void setExponent(float exponent)
   {
      exponent_ = exponent;
   }

   //! get specular exponent
   float getExponent() const
   {
      return(exponent_);
   }

   //! set the material parameters
   void setMaterialParameters(vec3f ka = vec3f(0.1f), vec3f kd = vec3f(0.7f), vec3f ks = vec3f(0.2f),
                              float exponent = 30)
   {
      ka_ = ka;
      kd_ = kd;
      ks_ = ks;

      exponent_ = exponent;
   }

   //! get the material parameters
   void getMaterialParameters(vec3f &ka, vec3f &kd, vec3f &ks,
                              float &exponent) const
   {
      ka = ka_;
      kd = kd_;
      ks = ks_;

      exponent = exponent_;
   }

   //! activate material
   void begin()
   {
      lglGetMaterialParameters(prev_ka_, prev_kd_, prev_ks_, prev_exponent_);
      lglMaterialParameters(ka_, kd_, ks_, exponent_);
   }

   //! deactivate material
   void end()
   {
      lglMaterialParameters(prev_ka_, prev_kd_, prev_ks_, prev_exponent_);
   }

protected:

   vec3f ka_, kd_, ks_;
   float exponent_;

   vec3f prev_ka_, prev_kd_, prev_ks_;
   float prev_exponent_;
};

#endif
