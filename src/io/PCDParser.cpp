#include "io/PCDParser.hpp"
#include "io/Parser/ParserGen2.hpp"
#include <sstream>


namespace PCDParser
{

void Importer::data(const double value)
{
    _datas.push_back(value);
}

void Importer::data()
{
    size_t index = 0;
    float coord[3] = {0}, color[3] = {0};
    for (const DataType t : _data_type)
    {
        switch (t)
        {
        case DataType::X:
            coord[0] = _datas[index++];
            break;
        case DataType::Y:
            coord[1] = _datas[index++];
            break;
        case DataType::Z:
            coord[2] = _datas[index++];
            break;
        case DataType::R:
            color[0] = _datas[index++];
            break;
        case DataType::G:
            color[1] = _datas[index++];
            break;
        case DataType::B:
            color[2] = _datas[index++];
            break;
        case DataType::RGB:
            {
                int value = _datas[index++];
                color[2] = value % 1000 / 255.0;
                value /= 1000;
                color[1] = value % 1000 / 255.0;
                value /= 1000;
                color[0] = value / 255.0;
            }
            break;
        default:
            break;
        }
    }
    _datas.clear();
    _coords.push_back(coord[0]);
    _coords.push_back(coord[1]);
    _coords.push_back(coord[2]);
    _colors.push_back(color[0]);
    _colors.push_back(color[1]);
    _colors.push_back(color[2]);
}

void Importer::fields(const std::string &value)
{
    for (size_t i = 0, j = 1, count = value.length(); j < count; i = ++j)
    {
        while (j < count && value[j] != ' ')
        {
            ++j;
        }
        if (j >= count)
        {
            break;
        }
        if (value.substr(i, j - i) == "x")
        {
            _data_type.push_back(DataType::X);
        }
        else if (value.substr(i, j - i) == "y")
        {
            _data_type.push_back(DataType::Y);
        }
        else if (value.substr(i, j - i) == "z")
        {
            _data_type.push_back(DataType::Z);
        }
        else  if (value.substr(i, j - i) == "r")
        {
            _data_type.push_back(DataType::R);
        }
        else  if (value.substr(i, j - i) == "g")
        {
            _data_type.push_back(DataType::G);
        }
        else  if (value.substr(i, j - i) == "b")
        {
            _data_type.push_back(DataType::B);
        }
        else  if (value.substr(i, j - i) == "rgb")
        {
            _data_type.push_back(DataType::RGB);
        }
        else
        {
            _data_type.push_back(DataType::Other);
        }
    }
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

    _pd->size = {_max_x, _max_y, _max_z, _min_x, _min_y, _min_z,
        std::max(_max_x - _min_x, std::max(_max_y - _min_y, _max_z - _min_z))};
}

void Importer::reset()
{
    _coords.clear();
    _colors.clear();
    _data_type.clear();
    _datas.clear();

    _min_x = _min_y = _min_z = FLT_MAX;
    _max_x = _max_y = _max_z = -FLT_MAX;
}

void Importer::bind_data(PointCloud::PointCloud &pd)
{
    _pd = &pd;
}


Importer importer;

Action<double> data_0_a(&importer, &Importer::data);
Action<void> data_1_a(&importer, &Importer::data);
Action<std::string> fields_a(&importer, &Importer::fields);


Parser<std::string> annotation = confix_p(ch_p('#'), eol_p());
Parser<std::string> version = confix_p(str_p("PCD_VERSION"), eol_p());
Parser<std::string> fields = confix_p(str_p("FIELDS"), list(+alphab_p(), ch_p(' '))[fields_a], eol_p());
Parser<std::string> size = confix_p(str_p("SIZE"), eol_p());
Parser<std::string> type = confix_p(str_p("TYPE"), eol_p());
Parser<std::string> count = confix_p(str_p("COUNT"), eol_p());
Parser<std::string> width = confix_p(str_p("WIDTH"), eol_p());
Parser<std::string> hieght = confix_p(str_p("HEIGHT"), eol_p());
Parser<std::string> view_point = confix_p(str_p("VIEWPOINT"), eol_p());
Parser<std::string> points = confix_p(str_p("POINTS"), eol_p());
Parser<std::string> data = confix_p(str_p("DATA"), eol_p());
Parser<bool> row = list(float_p()[data_0_a], ch_p(' '))[data_1_a] >> !ch_p(' ') >> eol_p(); 

Parser<bool> pcd = !annotation >> version >> fields >> size >> type >> count >> width >> hieght
    >> view_point >> points >> data >> +row;


bool PCDParser::parse(std::string_view &stream, PointCloud::PointCloud &pd)
{
    importer.reset();
    importer.bind_data(pd);
    return pcd(stream);
}

bool PCDParser::parse(std::ifstream &stream, PointCloud::PointCloud &pd)
{
    importer.reset();
    importer.bind_data(pd);
    std::stringstream sstream;
    sstream << stream.rdbuf();
    std::string str(sstream.str());
    std::string_view temp(str);
    return pcd(temp);
}

};