
#include <iostream>
#include <sstream>
#include "SVGElements.hpp"
#include "Color.hpp"
#include "external/tinyxml2/tinyxml2.h"

using namespace std;
using namespace tinyxml2;

namespace svg
{

    Color parse_color(const std::string& str)
    {
        // Remove '#' character from the beginning of the string
        std::string color_string = str;
        if (color_string[0] == '#')
        {
            color_string = color_string.substr(1);
        }

        // Convert hexadecimal color string to RGB values
        rgb_value r, g, b;
        std::stringstream ss;
        ss << std::hex << color_string;
        ss >> r >> g >> b;

        // Construct and return Color object
        return { r, g, b };
    }
    
    void readSVG(const string& svg_file, Point& dimensions, vector<SVGElement *>& svg_elements)
    {
        XMLDocument doc;
        XMLError r = doc.LoadFile(svg_file.c_str());
        if (r != XML_SUCCESS)
        {
            throw runtime_error("Unable to load " + svg_file);
        }
        XMLElement *xml_elem = doc.RootElement();

        dimensions.x = xml_elem->IntAttribute("width");
        dimensions.y = xml_elem->IntAttribute("height");
        
        // TODO complete code -->

        XMLElement *child = xml_elem->FirstChildElement();

        while(child) {
            const char* element_name = child->Name();
            if (strcmp(element_name, "ellipse") == 0) {
                int cx = child->IntAttribute("cx");
                int cy = child->IntAttribute("cy");
                int rx = child->IntAttribute("rx");
                int ry = child->IntAttribute("ry");
                Color fill = parse_color(child->Attribute("fill"));
                svg_elements.push_back(new Ellipse(fill, Point({cx, cy}), Point({rx, ry})));
            }
            else if(strcmp(element_name, "rectangle") == 0) {
                int x = child->IntAttribute("x");
                int y = child->IntAttribute("y");
                int width = child->IntAttribute("width");
                int height = child->IntAttribute("height");
                Color fill = parse_color(child->Attribute("fill"));
                svg_elements.push_back(new Rectangle(fill, Point({x, y}), Point({x + width, y + height})));
            }
            // Add support for other SVG elements
            child = child->NextSiblingElement();
        }
    }
        
}
