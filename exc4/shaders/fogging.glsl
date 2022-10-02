#version 120
attribute vec4 vertex_position;
attribute vec4 vertex_color;
uniform mat4 mvp;
varying vec4 frag_color;
void main()
{
   frag_color = vertex_color;
   gl_Position = mvp * vertex_position;
}
---
#version 120
varying vec4 frag_color;
void main()
{
   const float density = 0.5f;
   float w = 1.0f/gl_FragCoord.w;
   w = 1.0f-exp(-density*w*w);
   gl_FragColor = (1.0f-w)*frag_color + w*vec4(1);
}
