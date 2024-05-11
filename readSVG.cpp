#include <iostream>
#include <sstream>
#include <map>
#include "SVGElements.hpp"
#include "Color.hpp"
#include "Point.hpp"
#include "convert.cpp"
#include "external/tinyxml2/tinyxml2.h"

using namespace std;
using namespace tinyxml2;

namespace svg {
    const map<string, Color> NAMES_TO_COLORS = {
        {"black", {0, 0, 0}},
        {"white", {255, 255, 255}},
        {"red", {255, 0, 0}},
        {"green", {0, 255, 0}},
        {"blue", {0, 0, 255}},
        {"yellow", {255, 255, 0}}
    };


    Color parse_color(const std::string& str) {
        Color c;
        if(str.at(0) == '#')
        {
            unsigned int v;
            std::istringstream ss(str.substr(1));
            ss >> std::hex >> v;
            c.red = (v >> 16) & 0xFF;   
            c.green = (v >> 8) & 0xFF; 
            c.blue = v & 0xFF;          
        }
        else {
            c = NAMES_TO_COLORS.at(str);
        }
        return c;
    }


    void parsePoints(const char* points_str, vector<Point>& points) {
        if (points_str == nullptr) {
            cout << "null pointer" << endl;
            return;
        }

        points.clear();
        istringstream iss(points_str);
        string point_str;

        while(getline(iss, point_str, ' ')) {
            size_t comma_pos = point_str.find(',');

            if(comma_pos != string::npos && comma_pos != 0 && comma_pos != point_str.length() - 1) {
                int x = stoi(point_str.substr(0, comma_pos));
                int y = stoi(point_str.substr(comma_pos + 1));
                points.push_back({x, y});
            }
        }
    }


    void processGroupElement(XMLElement* group_child, vector<SVGElement*>& svg_elements, int group_translate_x, int group_translate_y) {
        while(group_child) {
            const char* group_element_name = group_child->Name();

            int child_translate_x = 0;
            int child_translate_y = 0;
            const char* child_transform_attr = group_child->Attribute("transform");
            if(child_transform_attr) {
                if(strstr(child_transform_attr, ",") && strstr(child_transform_attr, "translate")) {
                    sscanf(child_transform_attr, "translate(%d,%d)", &child_translate_x, &child_translate_y);
                } 
                else if(strstr(child_transform_attr, "translate")) {
                    sscanf(child_transform_attr, "translate(%d %d)", &child_translate_x, &child_translate_y);
                } 
            }

            if(strcmp(group_element_name, "ellipse") == 0) {
                int cx = group_child->IntAttribute("cx");
                int cy = group_child->IntAttribute("cy");
                int rx = group_child->IntAttribute("rx");
                int ry = group_child->IntAttribute("ry");
                Color fill = parse_color(group_child->Attribute("fill"));
                svg_elements.push_back(new Ellipse(fill, Point({cx + group_translate_x, cy + group_translate_y}), Point({rx, ry})));
            } 
            else if(strcmp(group_element_name, "circle") == 0) {
                cout << "Circle" << endl;
                int cx = group_child->IntAttribute("cx");
                int cy = group_child->IntAttribute("cy");
                int r = group_child->IntAttribute("r");
                Color fill = parse_color(group_child->Attribute("fill"));
                svg_elements.push_back(new Circle(fill, Point({cx + group_translate_x, cy + group_translate_y}), r));
            } 
            else if(strcmp(group_element_name, "polyline") == 0) {
                const char* points_str = group_child->Attribute("points");
                vector<Point> points;
                parsePoints(points_str, points);
                Color stroke_color = parse_color(group_child->Attribute("stroke"));
                svg_elements.push_back(new Polyline(stroke_color, points));
            } 
            else if(strcmp(group_element_name, "line") == 0) {
                int x1 = group_child->IntAttribute("x1");
                int y1 = group_child->IntAttribute("y1");
                int x2 = group_child->IntAttribute("x2");
                int y2 = group_child->IntAttribute("y2");
                Color stroke_color = parse_color(group_child->Attribute("stroke"));
                svg_elements.push_back(new Line(stroke_color, Point({x1, y1}), Point({x2, y2})));

            } 
            else if(strcmp(group_element_name, "polygon") == 0) {
                cout << "Polygon " << endl;
                const char* points_str = group_child->Attribute("points");
                const char* transform_str = group_child->Attribute("transform");
                const char* transform_origin_str = group_child->Attribute("transform-origin");
                int degree = 0;
                int scale = 0;
                int origin_x = 0;
                int origin_y = 0;
                Point origin = {0, 0};

                if(transform_str) {
                    if(strstr(transform_str, ",") && strstr(transform_str, "translate")) {
                        sscanf(transform_str, "translate(%d,%d)", &group_translate_x, &group_translate_y);
                    } 
                    else if(strstr(transform_str, "translate")) {
                        sscanf(transform_str, "translate(%d %d)", &group_translate_x, &group_translate_y);
                    } 
                    else if(strstr(transform_str, "rotate")) {
                        sscanf(transform_str, "rotate(%d)", &degree);
                    }
                    else sscanf(transform_str, "scale(%d)", &scale);
                }

                if(transform_origin_str) {
                    sscanf(transform_origin_str, "%d %d", &origin_x, &origin_y);
                    origin = {origin_x, origin_y};
                }

                vector<Point> points;
                parsePoints(points_str, points);

                for(Point& point : points) {
                    if(transform_origin_str && degree!=0) {
                        point = point.rotate(origin, degree);
                    }
                    else if(transform_origin_str && scale!=0) {
                        point = point.scale(origin, scale);
                    }
                    point.x += group_translate_x;
                    point.y += group_translate_y;
                }
                Color fill = parse_color(group_child->Attribute("fill"));
                svg_elements.push_back(new Polygon(fill, points));
            } 
            else if(strcmp(group_element_name, "rect") == 0) {
                int x = group_child->IntAttribute("x");
                int y = group_child->IntAttribute("y");
                int width = group_child->IntAttribute("width");
                int height = group_child->IntAttribute("height");
                Color fill = parse_color(group_child->Attribute("fill"));
                svg_elements.push_back(new Rectangle(fill, Point({x + group_translate_x, y + group_translate_y}), Point({x + group_translate_x + width, y + group_translate_y + height})));
            }
            else if(strcmp(group_element_name, "g") == 0){
                processGroupElement(group_child->FirstChildElement(), svg_elements, group_translate_x, group_translate_y);
            }
            group_child = group_child->NextSiblingElement();
        }
    }


    void readSVG(const string& svg_file, Point& dimensions, vector<SVGElement *>& svg_elements) {
        XMLDocument doc;
        XMLError r = doc.LoadFile(svg_file.c_str());
        if(r != XML_SUCCESS)
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

            int translate_x = 0;
            int translate_y = 0;
            const char *transform_attr = child->Attribute("transform");
            if(transform_attr) {
                if(strstr(transform_attr, ",") && strstr(transform_attr, "translate")) {
                    sscanf(transform_attr, "translate(%d,%d)", &translate_x, &translate_y);
                } 
                else if(strstr(transform_attr, "translate")) {
                    sscanf(transform_attr, "translate(%d %d)", &translate_x, &translate_y);
                } 
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
                cout << "First G: " << translate_x << ' ' << translate_y << endl;
                processGroupElement(child->FirstChildElement(), svg_elements, translate_x, translate_y);
            }
            child = child->NextSiblingElement();          
        }        
    }   
}