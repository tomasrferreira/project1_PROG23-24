#include <iostream>
#include <sstream>
#include <map>
#include "SVGElements.hpp"
#include "Color.hpp"
#include "external/tinyxml2/tinyxml2.h"

using namespace std;
using namespace tinyxml2;

namespace svg
{
    const map<string, Color> NAMES_TO_COLORS = {
        {"black", {0, 0, 0}},
        {"white", {255, 255, 255}},
        {"red", {255, 0, 0}},
        {"green", {0, 255, 0}},
        {"blue", {0, 0, 255}},
        {"yellow", {255, 255, 0}}
    };

    Color parse_color(const std::string& str)
    {
        Color c;
        if (str.at(0) == '#')
        {
            int v;
            std::istringstream ss(str.substr(1));
            ss >> std::hex >> v;
            c.red = (v >> 16);
            c.green = (v >> 8) & 0xFF;
            c.blue = v & 0xFF;
        }
        else
        {
            c = NAMES_TO_COLORS.at(str);
        }
        return c;
    }

    void parsePoints(const char* points_str, vector<Point>& points) {
        points.clear();
        istringstream iss(points_str);
        string point_str;
        while (getline(iss, point_str, ' ')) {
            size_t comma_pos = point_str.find(',');
            if (comma_pos != string::npos) {
                int x = stoi(point_str.substr(0, comma_pos));
                int y = stoi(point_str.substr(comma_pos + 1));
                points.push_back({x, y});
            }
        }
    }

    void readSVGGroup(XMLElement *group_elem, Point& dimensions, vector<SVGElement *>& svg_elements)
    {
        int translate_x = 0, translate_y = 0;
        const char *transform_attr = group_elem->Attribute("transform");
        if(transform_attr) {
            sscanf(transform_attr, "translate(%d,%d)", &translate_x, &translate_y);
        }

        XMLElement *child = group_elem->FirstChildElement();

        while(child) {
            const char* element_name = child->Name();
            if(strcmp(element_name, "ellipse") == 0) {
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
            else if(strcmp(element_name, "circle") == 0) {
                int cx = child->IntAttribute("cx");
                int cy = child->IntAttribute("cy");
                int r = child->IntAttribute("r");
                Color fill = parse_color(child->Attribute("fill"));
                svg_elements.push_back(new Circle(fill, Point({cx, cy}), r));
            }
            else if(strcmp(element_name, "polygon") == 0) {
                const char* points_str = child->Attribute("points");
                vector<Point> points;
                parsePoints(points_str, points);

                for (Point& point : points) {
                    point.x += translate_x;
                    point.y += translate_y;
                }
                Color fill = parse_color(child->Attribute("fill"));
                svg_elements.push_back(new Polygon(fill, points));
            }
            child = child->NextSiblingElement();
        }
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
            if(strcmp(element_name, "ellipse") == 0) {
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
            else if(strcmp(element_name, "circle") == 0) {
                int cx = child->IntAttribute("cx");
                int cy = child->IntAttribute("cy");
                int r = child->IntAttribute("r");
                Color fill = parse_color(child->Attribute("fill"));
                svg_elements.push_back(new Circle(fill, Point({cx, cy}), r));
            }
            else if(strcmp(element_name, "g") == 0) {
                readSVGGroup(child, dimensions, svg_elements);
            }
            child = child->NextSiblingElement();
        }   
    }  
}
