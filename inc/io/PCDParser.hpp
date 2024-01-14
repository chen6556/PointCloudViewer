#pragma once

#include <fstream>
#include <vector>


namespace PCDParser
{

class Importer
{
private:
    std::vector<float> _coords;
    std::vector<float> _colors;

    float min_x, min_y, min_z, max_x, max_y, max_z;

public:

    void coord(const double value);

    void color(const double value);
    

    void end();

    void reset();
};


// bool parse(std::string_view &stream, float *graph);

// bool parse(std::ifstream &stream, float *graph);
}