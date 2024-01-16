#pragma once

#include <fstream>
#include <vector>
#include <list>
#include "base/PointCloud.hpp"


namespace PLYParser
{

class Importer
{
public:
    enum Element {UnknownElement, Vertex, Face, Edge};
    enum Property {UnknownProperty, X, Y, Z, R, G, B, Intensity, VertexIndices, Vertex1, Vertex2};

private:
    std::vector<float> _coords;
    std::vector<float> _colors;
    size_t _coord_count = 0;
    std::vector<unsigned int> _surface_indexs;
    size_t _surface_count = 0;
    std::vector<unsigned int> _edge_indexs;
    size_t _edge_count = 0;
    std::vector<float> _cache;

    float _min_x, _min_y, _min_z, _max_x, _max_y, _max_z;
    PointCloud::PointCloud *_pd = nullptr;

    std::list<std::pair<Element, int>> _elements;
    Element _current_element = Element::UnknownElement;
    int _element_count = 0;
    std::vector<Property> _properties;
    std::list<int> _property_count;

public:
    void value(const double value);

    void data();

    void element(const std::string &value);

    void element_count(const int value);

    void property_name(const std::string &value);


    void end();

    void reset();

    void bind_data(PointCloud::PointCloud &pd);
};

bool parse(std::string_view &stream, PointCloud::PointCloud &pd);

bool parse(std::ifstream &stream, PointCloud::PointCloud &pd);

};