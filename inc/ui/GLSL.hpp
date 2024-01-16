#pragma once


namespace GLSL
{
const char *base_vss = "#version 450 core\n"
    "layout (location = 0) in vec3 pos;\n"
    "layout (location = 1) in vec3 inColor;\n"
    "uniform vec3 size;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 model;\n"
    "out vec3 outColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * model * vec4(pos, 1.0);\n"
    "   outColor = inColor;\n"
    "}\0";

const char *base_fss = "#version 450 core\n"
    "out vec4 FragColor;\n"
    "in vec3 outColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(outColor, 1.0);\n"
    "}\0";
};