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
                int cx = group_child->IntAttribute("cx");
                int cy = group_child->IntAttribute("cy");
                int r = group_child->IntAttribute("r");

                Color fill = parse_color(group_child->Attribute("fill"));

                svg_elements.push_back(new Circle(fill, Point({cx + group_translate_x, cy + group_translate_y}), r));
            } 
            else if(strcmp(group_element_name, "polyline") == 0) {
                const char* points_str = group_child->Attribute("points");
                const char* transform_str = group_child->Attribute("transform");
                const char* transform_origin_str = group_child->Attribute("transform-origin");

                Color stroke_color = parse_color(group_child->Attribute("stroke"));

                int degree = 0;
                int scale = 1;
                int origin_x = 0;
                int origin_y = 0;
                Point origin = {0, 0};

                if(transform_str) {
                    if(strstr(transform_str, "translate")) {
                        if(strstr(transform_str, ",")) {
                            sscanf(transform_str, "translate(%d,%d)", &group_translate_x, &group_translate_y);
                        } else {
                            sscanf(transform_str, "translate(%d %d)", &group_translate_x, &group_translate_y);
                        }
                    } 
                    if(strstr(transform_str, "rotate")) {
                        sscanf(transform_str, "rotate(%d)", &degree);
                    }
                    if(strstr(transform_str, "scale")) {
                        sscanf(transform_str, "scale(%d)", &scale);
                    }
                }

                if(transform_origin_str) {
                    sscanf(transform_origin_str, "%d %d", &origin_x, &origin_y);
                    origin = {origin_x, origin_y};
                }

                vector<Point> points;
                parsePoints(points_str, points);

                for(Point &point : points) {
                    if(degree != 0) {
                        point = point.rotate(origin, degree);
                    }
                    if(scale > 1) {
                        point = point.scale(origin, scale);
                    }
                    point.x += group_translate_x;
                    point.y += group_translate_y;
                }
                svg_elements.push_back(new Polyline(stroke_color, points));
            }
            else if(strcmp(group_element_name, "line") == 0) {
                int x1 = group_child->IntAttribute("x1");
                int y1 = group_child->IntAttribute("y1");
                int x2 = group_child->IntAttribute("x2");
                int y2 = group_child->IntAttribute("y2");

                Color stroke_color = parse_color(group_child->Attribute("stroke"));

                int degree = 0;
                int scale = 1;
                int origin_x = 0;
                int origin_y = 0;
                Point origin = {0, 0};

                const char* transform_str = group_child->Attribute("transform");
                const char* transform_origin_str = group_child->Attribute("transform-origin");

                if(transform_str) {
                    if(strstr(transform_str, "translate")) {
                        if(strstr(transform_str, ",")) {
                            sscanf(transform_str, "translate(%d,%d)", &group_translate_x, &group_translate_y);
                        } else {
                            sscanf(transform_str, "translate(%d %d)", &group_translate_x, &group_translate_y);
                        }
                    } 
                    if(strstr(transform_str, "rotate")) {
                        sscanf(transform_str, "rotate(%d)", &degree);
                    }
                    if(strstr(transform_str, "scale")) {
                        sscanf(transform_str, "scale(%d)", &scale);
                    }
                }

                if(transform_origin_str) {
                    sscanf(transform_origin_str, "%d %d", &origin_x, &origin_y);
                    origin = {origin_x, origin_y};
                }
                svg_elements.push_back(new Line(stroke_color, Point({x1, y1}), Point({x2, y2})));
            } 
            else if(strcmp(group_element_name, "polygon") == 0) {
                const char* points_str = group_child->Attribute("points");
                const char* transform_str = group_child->Attribute("transform");
                const char* transform_origin_str = group_child->Attribute("transform-origin");

                Color fill = parse_color(group_child->Attribute("fill"));

                int degree = 0;
                int scale = 1;
                int origin_x = 0;
                int origin_y = 0;
                Point origin = {0, 0};

                if(transform_str) {
                    if(strstr(transform_str, "translate")) {
                        if(strstr(transform_str, ",")) {
                            sscanf(transform_str, "translate(%d,%d)", &group_translate_x, &group_translate_y);
                        } else {
                            sscanf(transform_str, "translate(%d %d)", &group_translate_x, &group_translate_y);
                        }
                    } 
                    if(strstr(transform_str, "rotate")) {
                        sscanf(transform_str, "rotate(%d)", &degree);
                    }
                    if(strstr(transform_str, "scale")) {
                        sscanf(transform_str, "scale(%d)", &scale);
                    }
                }

                if(transform_origin_str) {
                    sscanf(transform_origin_str, "%d %d", &origin_x, &origin_y);
                    origin = {origin_x, origin_y};
                }

                vector<Point> points;
                parsePoints(points_str, points);

                for(Point &point : points) {
                    if(degree != 0) {
                        point = point.rotate(origin, degree);
                    }
                    else if(scale > 1) {
                        point = point.scale(origin, scale);
                    }
                    point.x += group_translate_x;
                    point.y += group_translate_y;
                }
                svg_elements.push_back(new Polygon(fill, points));
            } 
           else if(strcmp(group_element_name, "rect") == 0) {
                int x = group_child->IntAttribute("x");
                int y = group_child->IntAttribute("y");
                int width = group_child->IntAttribute("width");
                int height = group_child->IntAttribute("height");

                Color fill = parse_color(group_child->Attribute("fill"));

                int degree = 0;
                int scale = 1;
                Point origin = {0, 0};
                /*Point upper_left = {x, y};
                Point upper_right = {x + width, y};
                Point lower_left = {x, height - y};
                Point lower_right = {width - x, height - y};*/

                const char* transform_str = group_child->Attribute("transform");
                if(transform_str) {
                    if(strstr(transform_str, "translate")) {
                        if(strstr(transform_str, ",")) {
                            sscanf(transform_str, "translate(%d,%d)", &group_translate_x, &group_translate_y);
                        } else {
                            sscanf(transform_str, "translate(%d %d)", &group_translate_x, &group_translate_y);
                        }
                    } 
                    if(strstr(transform_str, "rotate")) {
                        sscanf(transform_str, "rotate(%d)", &degree);
                    }
                    if(strstr(transform_str, "scale")) {
                        sscanf(transform_str, "scale(%d)", &scale);
                    }
                }

                Point rect_origin = {x, y};
                rect_origin = rect_origin.translate({group_translate_x, group_translate_y});
                rect_origin = rect_origin.rotate(origin, degree);
                rect_origin = rect_origin.scale(origin, scale);

                x = rect_origin.x;
                y = rect_origin.y;

                svg_elements.push_back(new Rectangle(fill, Point({x, y}), Point({x + width - 1, y + height - 1})));
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
            int scale = 1;
            int degree = 0;
            const char *transform_attr = child->Attribute("transform");

            if(transform_attr) {
                if(strstr(transform_attr, "translate")) {
                    if(strstr(transform_attr, ",")) {
                        sscanf(transform_attr, "translate(%d,%d)", &translate_x, &translate_y);
                    } else {
                        sscanf(transform_attr, "translate(%d %d)", &translate_x, &translate_y);
                    }
                } 
                if(strstr(transform_attr, "rotate")) {
                    sscanf(transform_attr, "rotate(%d)", &degree);
                }
                if(strstr(transform_attr, "scale")) {
                    sscanf(transform_attr, "scale(%d)", &scale);
                }
            }

            if(strcmp(element_name, "ellipse") == 0) {
                int cx = child->IntAttribute("cx");
                int cy = child->IntAttribute("cy");
                int rx = child->IntAttribute("rx");
                int ry = child->IntAttribute("ry");

                Color fill = parse_color(child->Attribute("fill"));

                const char* transform_str = child->Attribute("transform");
                const char* transform_origin_str = child->Attribute("transform-origin");

                int degree = 0;
                int scale = 1;
                Point origin = {0, 0};
                Point center = {cx, cy};
                Point radius = {rx ,ry};   
                int origin_x;
                int origin_y;

                if(transform_str) {
                    if(strstr(transform_str, "rotate")) {
                        sscanf(transform_str, "rotate(%d)", &degree);
                    }
                    if(strstr(transform_str, "scale")) {
                        sscanf(transform_str, "scale(%d)", &scale);
                    }
                }

                if(transform_origin_str) {
                    sscanf(transform_origin_str, "%d %d", &origin_x, &origin_y);
                    origin = {origin_x, origin_y};
                    cout << origin_x << ' ' << origin_y << endl;
                }

                if(degree != 0) {
                    center = center.rotate(origin, degree);
                    radius = radius.rotate(origin, degree);
                }
                if(scale > 1) {
                    center = center.scale(origin, scale);
                    radius = radius.scale(origin, scale);
                }
                svg_elements.push_back(new Ellipse(fill, center, radius));
            }
            else if(strcmp(element_name, "circle") == 0) {
                int cx = child->IntAttribute("cx");
                int cy = child->IntAttribute("cy");
                int r = child->IntAttribute("r");

                Color fill = parse_color(child->Attribute("fill"));

                const char* transform_str = child->Attribute("transform");
                const char* transform_origin_str = child->Attribute("transform-origin");

                int degree = 0;
                int scale = 1;
                Point origin = {0, 0};
                Point center = {cx, cy};    
                int origin_x;
                int origin_y;

                if(transform_str) {
                    if(strstr(transform_str, "rotate")) {
                        sscanf(transform_str, "rotate(%d)", &degree);
                    }
                    if(strstr(transform_str, "scale")) {
                        sscanf(transform_str, "scale(%d)", &scale);
                    }
                }

                if(transform_origin_str) {
                    sscanf(transform_origin_str, "%d %d", &origin_x, &origin_y);
                    origin = {origin_x, origin_y};
                }

                if(degree != 0) {
                    center = center.rotate(origin, degree);
                }
                if(scale > 1) {
                    center = center.scale(origin, scale);
                }
                svg_elements.push_back(new Circle(fill, center, r));
            }
            else if(strcmp(element_name, "polyline") == 0) {
                const char* points_str = child->Attribute("points");
                const char* transform_str = child->Attribute("transform");
                const char* transform_origin_str = child->Attribute("transform-origin");

                vector<Point> points;
                parsePoints(points_str, points);

                Color stroke_color = parse_color(child->Attribute("stroke"));

                int degree = 0;
                int scale = 1;
                int origin_x = 0;
                int origin_y = 0;
                Point origin = {0, 0};

                if(transform_str) {
                    if(strstr(transform_str, "translate")) {
                        if(strstr(transform_str, ",")) {
                            sscanf(transform_str, "translate(%d,%d)", &translate_x, &translate_y);
                        } else {
                            sscanf(transform_str, "translate(%d %d)", &translate_x, &translate_y);
                        }
                    } 
                    if(strstr(transform_str, "rotate")) {
                        sscanf(transform_str, "rotate(%d)", &degree);
                    }
                    if(strstr(transform_str, "scale")) {
                        sscanf(transform_str, "scale(%d)", &scale);
                    }
                }

                if(transform_origin_str) {
                    sscanf(transform_origin_str, "%d %d", &origin_x, &origin_y);
                    origin = {origin_x, origin_y};
                }

                for(Point &point : points) {
                    if(degree != 0) {
                        point = point.rotate(origin, degree);
                    }
                    if(scale > 1) {
                        point = point.scale(origin, scale);
                    }
                    point.x += translate_x;
                    point.y += translate_y;
                }

                svg_elements.push_back(new Polyline(stroke_color, points));
            }
            else if(strcmp(element_name, "line") == 0) {
                int x1 = child->IntAttribute("x1");
                int y1 = child->IntAttribute("y1");
                int x2 = child->IntAttribute("x2");
                int y2 = child->IntAttribute("y2");

                Point p1 = {x1, y1};
                Point p2 = {x2, y2};

                Color stroke_color = parse_color(child->Attribute("stroke"));

                const char* transform_str = child->Attribute("transform");
                const char* transform_origin_str = child->Attribute("transform-origin");

                int degree = 0;
                int scale = 1;
                Point origin = {0, 0};
                int origin_x;
                int origin_y;

                if(transform_str) {
                    if(strstr(transform_str, "rotate")) {
                        sscanf(transform_str, "rotate(%d)", &degree);
                    }
                    if(strstr(transform_str, "scale")) {
                        sscanf(transform_str, "scale(%d)", &scale);
                    }
                }
                if(transform_origin_str) {
                    sscanf(transform_origin_str, "%d %d", &origin_x, &origin_y);
                    origin = {origin_x, origin_y};
                }

                if(degree != 0) {
                    p1 = p1.rotate(origin, degree);
                    p2 = p2.rotate(origin, degree);
                }
                if(scale > 1) {
                    p1 = p1.scale(origin, scale);
                    p2 = p2.scale(origin, scale);
                }

                svg_elements.push_back(new Line(stroke_color, p1, p2));
            }
            else if(strcmp(element_name, "polygon") == 0) {
                const char* points_str = child->Attribute("points");
                const char* transform_str = child->Attribute("transform");
                const char* transform_origin_str = child->Attribute("transform-origin");

                vector<Point> points;
                parsePoints(points_str, points);

                Color fill = parse_color(child->Attribute("fill"));

                int degree = 0;
                int scale = 1;
                int origin_x = 0;
                int origin_y = 0;
                Point origin = {0, 0};

                if(transform_str) {
                    if(strstr(transform_str, "translate")) {
                        if(strstr(transform_str, ",")) {
                            sscanf(transform_str, "translate(%d,%d)", &translate_x, &translate_y);
                        } else {
                            sscanf(transform_str, "translate(%d %d)", &translate_x, &translate_y);
                        }
                    } 
                    if(strstr(transform_str, "rotate")) {
                        sscanf(transform_str, "rotate(%d)", &degree);
                    }
                    if(strstr(transform_str, "scale")) {
                        sscanf(transform_str, "scale(%d)", &scale);
                    }
                }

                if(transform_origin_str) {
                    sscanf(transform_origin_str, "%d %d", &origin_x, &origin_y);
                    origin = {origin_x, origin_y};
                }

                for(Point &point : points) {
                    if(degree != 0) {
                        point = point.rotate(origin, degree);
                    }
                    else if(scale > 1) {
                        point = point.scale(origin, scale);
                    }
                    point.x += translate_x;
                    point.y += translate_y;
                }
                svg_elements.push_back(new Polygon(fill, points));
            }
            else if(strcmp(element_name, "rect") == 0) {
                int x = child->IntAttribute("x");
                int y = child->IntAttribute("y");
                int width = child->IntAttribute("width");
                int height = child->IntAttribute("height");

                Color fill = parse_color(child->Attribute("fill"));

                Point origin = {0, 0};
                Point rect_origin = {x, y};

                rect_origin = rect_origin.translate({translate_x, translate_y});
                rect_origin = rect_origin.rotate(origin, degree);
                rect_origin = rect_origin.scale(origin, scale);

                x = rect_origin.x;
                y = rect_origin.y;

                svg_elements.push_back(new Rectangle(fill, Point({x, y}), Point({x + width - 1, y + height - 1})));
            }
            else if(strcmp(element_name, "g") == 0) {
                processGroupElement(child->FirstChildElement(), svg_elements, translate_x, translate_y);
            }
            child = child->NextSiblingElement();          
        }
    }   
}