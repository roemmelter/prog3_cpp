#version 120
attribute vec4 vertex_position;
attribute vec4 vertex_color;
uniform mat4 mvp;
varying vec4 frag_color;
varying vec3 frag_position;
void main()
{
   gl_Position = mvp * vertex_position;
   frag_color = vertex_color;
   frag_position = vec3(vertex_position);
}
---
#version 120
varying vec4 frag_color;
varying vec3 frag_position;
void main()
{
   float s = sin(100.0f*frag_position.z)/2.0f+0.5f;
   gl_FragColor = frag_color * vec4(vec3(s),1);
}
