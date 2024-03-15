#include "io/PCDParser.hpp"
#include "io/Parser/ParserGen2.hpp"


namespace PCDParser
{

void Importer::coord(const double value)
{
    _coords.push_back(value);
}

void Importer::color(const double value)
{
    _colors.push_back(value);
}


void Importer::end()
{

}

void Importer::reset()
{
    _coords.clear();
    _colors.clear();

    min_x, min_y, min_z, max_x, max_y, max_z;
}




Importer importer;

Action<double> coord_a(&importer, &Importer::coord);
Action<double> color_a(&importer, &Importer::color);


Parser<std::string> annotation = confix_p(ch_p('#'), *anychar_p(), eol_p());
Parser<std::string> version = confix_p(str_p("PCD_VERSION"), *anychar_p(), eol_p());


};