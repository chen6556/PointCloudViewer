#pragma once

#include <fstream>
#include <vector>
#include "base/PointCloud.hpp"


namespace PLYParser
{

class Importer
{
private:
    std::vector<float> _coords;
    std::vector<float> _colors;
    std::vector<unsigned int> _surface_indexs;
    std::vector<float> _cache;

    float _min_x, _min_y, _min_z, _max_x, _max_y, _max_z;
    PointCloud::PointCloud *_pd = nullptr;

public:
    void value(const double value);

    void row();


    void end();

    void reset();

    void bind_data(PointCloud::PointCloud &pd);
};

bool parse(std::string_view &stream, PointCloud::PointCloud &pd);

bool parse(std::ifstream &stream, PointCloud::PointCloud &pd);

};