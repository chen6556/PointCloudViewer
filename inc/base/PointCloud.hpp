#pragma once
#include <vector>



namespace PointCloud
{

struct PointCloudSize
{
    float max_x = 0.0f;
    float max_y = 0.0f;
    float max_z = 0.0f;
    float min_x = 0.0f;
    float min_y = 0.0f;
    float min_z = 0.0f;
    float len = 0.0f;
};

struct PointCloud
{
    PointCloudSize size;
    std::vector<float> data;
    std::vector<unsigned int> surface;
    std::vector<unsigned int> edge;
};

const static float PI = 3.1415927f;

};