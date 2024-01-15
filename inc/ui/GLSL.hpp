#pragma once


namespace GLSL
{
const char *base_vss = "#version 450 core\n"
    "layout (location = 0) in vec3 pos;\n"
    "layout (location = 1) in vec3 inColor;\n"
    "uniform int w;\n"
    "uniform int h;\n"
    "uniform vec4 row0;\n"
    "uniform vec4 row1;\n"
    "uniform vec4 row2;\n"
    "uniform vec4 row3;\n"
    "out vec3 outColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(pos.x * row0.x + pos.y * row0.y + pos.z * row0.z + row0.w,"
            "pos.x * row1.x + pos.y * row1.y + pos.z * row1.z + row1.w,"
            "pos.x * row2.x + pos.y * row2.y + pos.z * row2.z + row2.w,"
            "pos.x * row3.x + pos.y * row3.y + pos.z * row3.z + row3.w);\n"
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