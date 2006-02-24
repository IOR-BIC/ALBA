#ifndef __shader_h__
#define __shader_h__

// plastic -- vabbe ...
//-----------------------------------------------------------------
char plastic_vert[] = 
//-----------------------------------------------------------------
" uniform vec3 view_position;                                   \n"
"                                                               \n"
" varying vec3 vNormal;                                         \n"
" varying vec3 vViewVec;                                        \n"
"                                                               \n"
" void main(void)                                               \n"
" {                                                             \n"
"   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;     \n"
"                                                               \n"
"   vNormal = gl_Normal;                                        \n"
"   vViewVec = view_position - gl_Vertex.xyz;                   \n"
"                                                               \n" 
" }                                                             \n";

//-----------------------------------------------------------------
char plastic_frag[] = 
//-----------------------------------------------------------------
" uniform vec4 color;                                          \n"
"                                                              \n"
" varying vec3 vNormal;                                        \n"
" varying vec3 vViewVec;                                       \n"
"                                                              \n"
" void main(void)                                              \n"
" {                                                            \n"
"   float v = 0.5 * (1.0 + dot(normalize(vViewVec), vNormal)); \n"
"   gl_FragColor = v * color;                                  \n"
"                                                              \n"
" }                                                            \n";

// il mini non usa uniforms
//-----------------------------------------------------------------
static const char *mini_vert = 
//-----------------------------------------------------------------
{
    "// mini_shader - colors a fragment based on its position\n"
    "varying vec4 color;\n"
    "void main(void)\n"
    "{\n"
    "    color = gl_Vertex;\n"
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
    "}\n"
};
//-----------------------------------------------------------------
static const char *mini_frag = 
//-----------------------------------------------------------------
{
  "varying vec4 color;\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = clamp( color, 0.0, 1.0 );\n"
    "}\n"
};


# endif __shader_h__