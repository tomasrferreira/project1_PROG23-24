#include <iostream>
#include <sstream>
#include <map>
#include "SVGElements.hpp"
#include "Color.hpp"
#include "convert.cpp"
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
            unsigned int v;
            std::istringstream ss(str.substr(1));
            ss >> std::hex >> v;
            //cout << "v : " << v << endl;
            c.red = (v >> 16) & 0xFF;   // Extracting red component
            c.green = (v >> 8) & 0xFF;  // Extracting green component
            c.blue = v & 0xFF;          // Extracting blue component
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

    void readSVG(const string& svg_file, Point& dimensions, vector<SVGElement *>& svg_elements)
    {
        XMLDocument doc;
        XMLError r = doc.LoadFile(svg_file.c_str());
        cout << "Hello" << endl;
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
            int translate_x = 0, translate_y = 0;
            const char *transform_attr = child->Attribute("transform");
            if(transform_attr) {
                sscanf(transform_attr, "translate(%d %d)", &translate_x, &translate_y);
            }
            if(strcmp(element_name, "ellipse") == 0) {
                int cx = child->IntAttribute("cx");
                int cy = child->IntAttribute("cy");
                int rx = child->IntAttribute("rx");
                int ry = child->IntAttribute("ry");
                Color fill = parse_color(child->Attribute("fill"));
                svg_elements.push_back(new Ellipse(fill, Point({cx, cy}), Point({rx, ry})));
            }
            else if(strcmp(element_name, "circle") == 0) {
                int cx = child->IntAttribute("cx");
                int cy = child->IntAttribute("cy");
                int r = child->IntAttribute("r");
                Color fill = parse_color(child->Attribute("fill"));
                svg_elements.push_back(new Circle(fill, Point({cx, cy}), r));
            }
            else if(strcmp(element_name, "polyline") == 0) {
                const char* points_str = child->Attribute("points");
                vector<Point> points;
                parsePoints(points_str, points);
                Color stroke_color = parse_color(child->Attribute("stroke"));
                svg_elements.push_back(new Polyline(stroke_color, points));
            }
            else if(strcmp(element_name, "line") == 0) {
                int x1 = child->IntAttribute("x1");
                int y1 = child->IntAttribute("y1");
                int x2 = child->IntAttribute("x2");
                int y2 = child->IntAttribute("y2");
                Color stroke_color = parse_color(child->Attribute("stroke"));
                svg_elements.push_back(new Line(stroke_color, Point({x1, y1}), Point({x2, y2})));
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
            else if(strcmp(element_name, "rect") == 0) {
                int x = child->IntAttribute("x");
                int y = child->IntAttribute("y");
                int width = child->IntAttribute("width");
                int height = child->IntAttribute("height");
                Color fill = parse_color(child->Attribute("fill"));
                svg_elements.push_back(new Rectangle(fill, Point({x, y}), Point({x + width - 1, y + height - 1})));
            }
            else if(strcmp(element_name, "g") == 0) {
                int translate_x = 0, translate_y = 0;
                const char *transform_attr = child->Attribute("transform");
                if(transform_attr) {
                    sscanf(transform_attr, "translate(%d %d)", &translate_x, &translate_y);
                }

                XMLElement *group_child = child->FirstChildElement();
                while(group_child) {
                    const char* group_element_name = group_child->Name();
                    if(strcmp(group_element_name, "ellipse") == 0) {
                        int cx = group_child->IntAttribute("cx");
                        int cy = group_child->IntAttribute("cy");
                        int rx = group_child->IntAttribute("rx");
                        int ry = group_child->IntAttribute("ry");
                        Color fill = parse_color(group_child->Attribute("fill"));
                        svg_elements.push_back(new Ellipse(fill, Point({cx + translate_x, cy + translate_y}), Point({rx, ry})));
                    }
                    else if(strcmp(element_name, "circle") == 0) {
                        int cx = child->IntAttribute("cx");
                        int cy = child->IntAttribute("cy");
                        int r = child->IntAttribute("r");
                        Color fill = parse_color(child->Attribute("fill"));
                        svg_elements.push_back(new Circle(fill, Point({cx, cy}), r));
                    }
                    else if(strcmp(element_name, "polyline") == 0) {
                        const char* points_str = child->Attribute("points");
                        vector<Point> points;
                        parsePoints(points_str, points);
                        Color stroke_color = parse_color(child->Attribute("stroke"));
                        svg_elements.push_back(new Polyline(stroke_color, points));
                    }
                    else if(strcmp(element_name, "line") == 0) {
                        int x1 = child->IntAttribute("x1");
                        int y1 = child->IntAttribute("y1");
                        int x2 = child->IntAttribute("x2");
                        int y2 = child->IntAttribute("y2");
                        Color stroke_color = parse_color(child->Attribute("stroke"));
                        svg_elements.push_back(new Line(stroke_color, Point({x1, y1}), Point({x2, y2})));
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
                    else if(strcmp(group_element_name, "rect") == 0) {
                        int x = group_child->IntAttribute("x");
                        int y = group_child->IntAttribute("y");
                        int width = group_child->IntAttribute("width");
                        int height = group_child->IntAttribute("height");
                        Color fill = parse_color(group_child->Attribute("fill"));
                        svg_elements.push_back(new Rectangle(fill, Point({x + translate_x, y + translate_y}), Point({x + translate_x + width, y + translate_y + height})));
                    }
                    group_child = group_child->NextSiblingElement();
                }
            }
            child = child->NextSiblingElement();          
        }        
    }
}
