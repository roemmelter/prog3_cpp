#version 120
attribute vec4 vertex_position;
attribute vec4 vertex_color;
attribute vec2 vertex_texcoord;
uniform mat4 mvp;
varying vec4 frag_color;
varying vec2 frag_texcoord;
void main()
{
   gl_Position = mvp * vertex_position;
   frag_color = vertex_color;
   frag_texcoord = vec2(vertex_texcoord);
}
---
#version 120
varying vec4 frag_color;
varying vec2 frag_texcoord;
uniform sampler2D tex2D;
void main()
{
   gl_FragColor = frag_color * texture2D(tex2D, frag_texcoord);
}
