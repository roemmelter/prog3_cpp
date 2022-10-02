#version 120
attribute vec4 vertex_position;
uniform mat4 mvp;
void main()
{
   gl_Position = mvp * vertex_position;
}
---
#version 120
void main()
{
   float w = 1.0f/gl_FragCoord.w-1.0f;
   gl_FragColor = mix(vec4(1,0,0,1), vec4(0,0,0.25f,1), w);
}
