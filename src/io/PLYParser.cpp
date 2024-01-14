#include "io/PLYParser.hpp"
#include "io/Parser.hpp"
#include <sstream>


namespace PLYParser
{

void Importer::value(const double value)
{
    _cache.push_back(value);
}

void Importer::row()
{
    switch (_cache.size())
    {
    case 3:
        for (int i = 0; i < 3; ++i)
        {
            _coords.push_back(_cache[i]);
        }
        break;
    case 6:
        for (int i = 0; i < 3; ++i)
        {
            _coords.push_back(_cache[i]);
            _colors.push_back(_cache[i + 3]);
        }
        break;
    default:
        break;
    }

    _min_x = std::min(_cache[0], _min_x);
    _max_x = std::max(_cache[0], _max_x);
    _min_y = std::min(_cache[1], _min_y);
    _max_y = std::max(_cache[1], _max_y);
    _min_z = std::min(_cache[2], _min_z);
    _max_z = std::max(_cache[2], _max_z);

    _cache.clear();
}


void Importer::end()
{
    const float x = (_min_x + _max_x) / 2.0f;
    const float y = (_min_y + _max_y) / 2.0f;
    const float z = (_min_z + _max_z) / 2.0f;

    if (_colors.empty())
    {
        for (size_t i = 0, count = _coords.size(); i < count;)
        {
            _pd->data.push_back(_coords[i++] - x);
            _pd->data.push_back(_coords[i++] - y);
            _pd->data.push_back(_coords[i++] - z);
            _pd->data.push_back(1.0f);
            _pd->data.push_back(1.0f);
            _pd->data.push_back(1.0f);
        }
    }
    else
    {
        for (size_t i = 0, j = 0, count = _coords.size(); i < count;)
        {
            _pd->data.push_back(_coords[i++] - x);
            _pd->data.push_back(_coords[i++] - y);
            _pd->data.push_back(_coords[i++] - z);
            _pd->data.push_back(_colors[j++]);
            _pd->data.push_back(_colors[j++]);
            _pd->data.push_back(_colors[j++]);
        }
    }

    _pd->size = {_max_x, _max_y, _max_z, _min_x, _min_y, _min_z,
        std::max(_max_x - _min_x, std::max(_max_y - _min_y, _max_z - _min_z))};
}

void Importer::reset()
{
    _coords.clear();
    _colors.clear();
    _surface_indexs.clear();
    _cache.clear();

    _min_x = _min_y = _min_z = FLT_MAX;
    _max_x = _max_y = _max_z = -FLT_MAX;
}

void Importer::bind_data(PointCloud::PointCloud &pd)
{
    _pd = &pd;
}



Importer importer;

Action<double> value_a(&importer, &Importer::value);
Action<void> row_a(&importer, &Importer::row);
Action<void> end_a(&importer, &Importer::end);

Parser<bool> head = confix_p(str_p("ply"), *anychar_p(), str_p("end_header")) >> eol_p();
Parser<bool> row = (+(float_p()[value_a] >> !ch_p(' ')) >> eol_p())[row_a];
Parser<bool> data = +row;

Parser<bool> ply = (head >> data)[end_a];



bool PLYParser::parse(std::string_view &stream, PointCloud::PointCloud &pd)
{
    importer.reset();
    importer.bind_data(pd);
    return ply(stream);
}

bool PLYParser::parse(std::ifstream &stream, PointCloud::PointCloud &pd)
{
    importer.reset();
    importer.bind_data(pd);
    std::stringstream sstream;
    sstream << stream.rdbuf();
    std::string str(sstream.str());
    std::string_view temp(str);
    return ply(temp);
}

}