#pragma once

#include <fstream>
#include <vector>
#include "base/PointCloud.hpp"


namespace PCDParser
{

class Importer
{
private:
    enum DataType {X, Y, Z, R, G, B, RGB, Other};

private:
    std::vector<float> _coords;
    std::vector<float> _colors;

    float _min_x, _min_y, _min_z, _max_x, _max_y, _max_z;
    PointCloud::PointCloud *_pd = nullptr;

    std::vector<DataType> _data_type;
    std::vector<float> _datas;

public:

    void data(const double value);

    void data();

    void fields(const std::string &value);
    

    void end();

    void reset();

    void bind_data(PointCloud::PointCloud &pd);
};


bool parse(std::string_view &stream, PointCloud::PointCloud &pd);

bool parse(std::ifstream &stream, PointCloud::PointCloud &pd);
}