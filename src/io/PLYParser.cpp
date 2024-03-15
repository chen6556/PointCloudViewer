#include "io/PLYParser.hpp"
#include "io/Parser/ParserGen2.hpp"
#include <sstream>


namespace PLYParser
{

void Importer::value(const double value)
{
    _cache.push_back(value);
}

void Importer::data()
{
    if (++_element_count > _elements.front().second)
    {
        _elements.pop_front();
        _properties.erase(_properties.begin(), _properties.begin() + _property_count.front());
        _property_count.pop_front();
        _element_count = 0;
    }
    if (_elements.front().first == Element::UnknownElement)
    {
        _cache.clear();
        return;
    }

    float x, y, z, r, g, b;
    switch (_elements.front().first)
    {
    case Element::Vertex:
        r = g = b = 1.0f;
        for (int i = 0; i < _property_count.front(); ++i)
        {
            switch (_properties[i])
            {
            case Property::X:
                x = _cache[i];
                break;
            case Property::Y:
                y = _cache[i];
                break;
            case Property::Z:
                z = _cache[i];
                break;
            case Property::R:
                r = _cache[i];
                break;
            case Property::G:
                g = _cache[i];
                break;
            case Property::B:
                b = _cache[i];
                break;
            case Property::Intensity:
                r = g = b = _cache[i];
            case Property::VertexIndices:
                break;
            default:
                break;
            }
        }
        _coords.push_back(x);
        _coords.push_back(y);
        _coords.push_back(z);
        _colors.push_back(r);
        _colors.push_back(g);
        _colors.push_back(b);
        _coord_count++;
        break;
    case Element::Face:
        for (int i = 0; i < _property_count.front(); ++i)
        {
            switch (_properties[i])
            {
            case Property::VertexIndices:
                for (int i = 1, count = _cache.front(); i <= count; ++i)
                {
                    _surface_indexs.push_back(_cache[i]);
                }
                _surface_indexs.push_back(UINT_MAX);
                _surface_count++;
                break;
            default:
                break;
            }
        }
        break;
    case Element::Edge:
        for (int i = 0; i < _property_count.front(); ++i)
        {
            switch (_properties[i])
            {
            case Property::Vertex1:
                _edge_indexs.push_back(_cache[i]);
            case Property::Vertex2:
                _edge_indexs.push_back(_cache[i]);
                _edge_count++;
                break;
            default:
                break;
            }
        }
        _edge_indexs.push_back(UINT_MAX);
        break;
    default:
        break;
    }

    if (_elements.front().first == Element::Vertex)
    {
        _min_x = std::min(_coords[_coord_count * 3 - 3], _min_x);
        _max_x = std::max(_coords[_coord_count * 3 - 3], _max_x);
        _min_y = std::min(_coords[_coord_count * 3 - 2], _min_y);
        _max_y = std::max(_coords[_coord_count * 3 - 2], _max_y);
        _min_z = std::min(_coords.back(), _min_z);
        _max_z = std::max(_coords.back(), _max_z);
    }

    _cache.clear();
}

void Importer::element(const std::string &value)
{
    if (value == "vertex")
    {
        _current_element = Element::Vertex;
    }
    else if (value == "face")
    {
        _current_element = Element::Face;
    }
    else if (value == "edge")
    {
        _current_element = Element::Edge;
    }
    else
    {
        _current_element = Element::UnknownElement;
    }
    _property_count.push_back(0);
}

void Importer::element_count(const int value)
{
    _elements.emplace_back(_current_element, value);
}

void Importer::property_name(const std::string &value)
{
    if (value == "x" || value == "X")
    {
        _properties.push_back(Property::X);
    }
    else if (value == "y" || value == "Y")
    {
        _properties.push_back(Property::Y);
    }
    else if (value == "z" || value == "Z")
    {
        _properties.push_back(Property::Z);
    }
    else if (value == "vertex_indices" || value == "vertex_index")
    {
        _properties.push_back(Property::VertexIndices);
    }
    else if (value == "red")
    {
        _properties.push_back(Property::R);
    }
    else if (value == "green")
    {
        _properties.push_back(Property::G);
    }
    else if (value == "blue")
    {
        _properties.push_back(Property::B);
    }
    else if (value == "intensity")
    {
        _properties.push_back(Property::Intensity);
    }
    else if (value == "vertex1")
    {
        _properties.push_back(Property::Vertex1);
    }
    else if (value == "vertex2")
    {
        _properties.push_back(Property::Vertex2);
    }
    else
    {
        _properties.push_back(Property::UnknownProperty);
    }
    _property_count.back()++;
}



void Importer::end()
{
    const float x = (_min_x + _max_x) / 2.0f;
    const float y = (_min_y + _max_y) / 2.0f;
    const float z = (_min_z + _max_z) / 2.0f;

    for (size_t i = 0, j = 0, count = _coords.size(); i < count;)
    {
        _pd->data.push_back(_coords[i++] - x);
        _pd->data.push_back(_coords[i++] - y);
        _pd->data.push_back(_coords[i++] - z);
        _pd->data.push_back(_colors[j++]);
        _pd->data.push_back(_colors[j++]);
        _pd->data.push_back(_colors[j++]);
    }

    _pd->surface = _surface_indexs;
    _pd->edge = _edge_indexs;

    _pd->size = {_max_x, _max_y, _max_z, _min_x, _min_y, _min_z,
        std::max(_max_x - _min_x, std::max(_max_y - _min_y, _max_z - _min_z))};
}

void Importer::reset()
{
    _coords.clear();
    _colors.clear();
    _surface_indexs.clear();
    _edge_indexs.clear();
    _cache.clear();
    _elements.clear();
    _properties.clear();
    _property_count.clear();

    _element_count = 0;
    _coord_count = _surface_count = _edge_count = 0;
    _min_x = _min_y = _min_z = FLT_MAX;
    _max_x = _max_y = _max_z = -FLT_MAX;
}

void Importer::bind_data(PointCloud::PointCloud &pd)
{
    _pd = &pd;
}



Importer importer;

Action<double> value_a(&importer, &Importer::value);
Action<void> data_a(&importer, &Importer::data);
Action<void> end_a(&importer, &Importer::end);
Action<std::string> element_a(&importer, &Importer::element);
Action<int> element_count_a(&importer, &Importer::element_count);
Action<std::string> proprety_name_a(&importer, &Importer::property_name);

Parser<std::string> type_name = str_p("char") | str_p("uchar") | str_p("short") | str_p("ushort")
    | str_p("int") | str_p("uint") | str_p("float") | str_p("double");
Parser<std::string> property = str_p("property ") >> !str_p("list ") >> +(type_name >> ch_p(' '))
    >> ((+anychar_p())[proprety_name_a] - eol_p()) >> eol_p();
Parser<bool> element = str_p("element ") >> ((+anychar_p())[element_a] - ch_p(' ')) >> ch_p(' ')
    >> int_p()[element_count_a] >> eol_p();
Parser<bool> elements = +(element >> +property);

Parser<std::string> format = confix_p(str_p("format"), +anychar_p(), eol_p());
Parser<std::string> comment = confix_p(str_p("comment"), +anychar_p(), eol_p());
Parser<std::string> obj_info = confix_p(str_p("obj_info"), +anychar_p(), eol_p());
Parser<bool> head = str_p("ply") >> eol_p() >> +format >> *comment >> *obj_info
    >> elements >> str_p("end_header") >> eol_p();
Parser<bool> data = (+(float_p()[value_a] >> !ch_p(' ')) >> eol_p())[data_a];

Parser<bool> ply = (head >> +data)[end_a];



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