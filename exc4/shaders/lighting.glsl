#version 120
attribute vec4 vertex_position;
attribute vec4 vertex_color;
attribute vec3 vertex_normal;
uniform mat4 mvp;
uniform mat4 mvit;
varying vec4 frag_color;
varying vec3 frag_normal;
void main()
{
   gl_Position = mvp * vertex_position;
   frag_color = vertex_color;
   frag_normal = mat3(mvit) * vertex_normal;
}
---
#version 120
varying vec4 frag_color;
varying vec3 frag_normal;
void main()
{
   gl_FragColor = frag_color * vec4(vec3(frag_normal.z),1);
}
