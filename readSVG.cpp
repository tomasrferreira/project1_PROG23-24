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
    // Map of color names to Color objects
    const map<string, Color> NAMES_TO_COLORS = {
        {"black", {0, 0, 0}},
        {"white", {255, 255, 255}},
        {"red", {255, 0, 0}},
        {"green", {0, 255, 0}},
        {"blue", {0, 0, 255}},
        {"yellow", {255, 255, 0}}
    };

    /**
     * Parses a color from a string.
     * @param str The string representation of the color.
     * @return The parsed Color.
     */
    Color parse_color(const string& str) {
        Color c;
        if(str.at(0) == '#')
        {
            unsigned int v;
            istringstream ss(str.substr(1));
            ss >> hex >> v;
            c.red = (v >> 16) & 0xFF;   
            c.green = (v >> 8) & 0xFF; 
            c.blue = v & 0xFF;          
        }
        else {
            c = NAMES_TO_COLORS.at(str);
        }
        return c;
    }

    /**
     * Parses points from a string and stores them in a vector of Points.
     * @param points_str The string containing the points.
     * @param points The vector to store the parsed points.
     */
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

    /**
     * Processes a group element <g> in the SVG file.
     * @param group_child The group element to process.
     * @param svg_elements The vector to store the SVG elements.
     * @param group_translate_x The translation x for the group.
     * @param group_translate_y The translation y for the group.
     */
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
                // Handle ellipse element within the group
                int cx = group_child->IntAttribute("cx");
                int cy = group_child->IntAttribute("cy");
                int rx = group_child->IntAttribute("rx");
                int ry = group_child->IntAttribute("ry");

                Color fill = parse_color(group_child->Attribute("fill"));

                // Add the ellipse to svg_elements vector
                svg_elements.push_back(new Ellipse(fill, Point({cx + group_translate_x, cy + group_translate_y}), Point({rx, ry})));
            } 
            else if(strcmp(group_element_name, "circle") == 0) {
                // Handle circle element within the group
                int cx = group_child->IntAttribute("cx");
                int cy = group_child->IntAttribute("cy");
                int r = group_child->IntAttribute("r");

                Color fill = parse_color(group_child->Attribute("fill"));

                // Add the circle to svg_elements vector
                svg_elements.push_back(new Circle(fill, Point({cx + group_translate_x, cy + group_translate_y}), r));
            } 
            else if(strcmp(group_element_name, "polyline") == 0) {
                // Handle polyline element within the group
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

                // Apply transformations to the points of the polyline
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

                // Add the polyline to svg_elements vector
                svg_elements.push_back(new Polyline(stroke_color, points));
            }
            else if(strcmp(group_element_name, "line") == 0) {
                // Handle line element within the group
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

                // Add the line to svg_elements vector
                svg_elements.push_back(new Line(stroke_color, Point({x1, y1}), Point({x2, y2})));
            } 
            else if(strcmp(group_element_name, "polygon") == 0) {
                // Handle polygon element within the group
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

                // Apply transformations to the points of the polygon
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

                // Add the polygon to svg_elements vector
                svg_elements.push_back(new Polygon(fill, points));
            } 
           else if(strcmp(group_element_name, "rect") == 0) {
                // Handle rect element within the group
                int x = group_child->IntAttribute("x");
                int y = group_child->IntAttribute("y");
                int width = group_child->IntAttribute("width");
                int height = group_child->IntAttribute("height");

                Color fill = parse_color(group_child->Attribute("fill"));

                int degree = 0;
                int scale = 1;
                Point origin = {x + width / 2, y + height / 2};
                Point rect_origin = {x, y};
                int origin_x = origin.x;
                int origin_y = origin.y;

                const char* transform_str = group_child->Attribute("transform");
                const char* transform_origin_str = group_child->Attribute("transform-origin");

                if(transform_origin_str) {
                    sscanf(transform_origin_str, "%d %d", &origin_x, &origin_y);
                    origin = {origin_x, origin_y};
                }

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

                // Define the four corner points of the rectangle
                vector<Point> points = {rect_origin, rect_origin.translate({width - 1, 0}), 
                    rect_origin.translate({width - 1, height - 1}), rect_origin.translate({0, height - 1})};

                // Apply transformations to the points of the rectangle
                for(Point &point : points) {
                    if(degree != 0) {
                        point = point.rotate({origin_x, origin_y}, degree);
                    }
                    else if(scale > 1) {
                        point = point.scale({origin_x, origin_y}, scale);
                    }
                    point.x += group_translate_x;
                    point.y += group_translate_y;
                }

                // Add the polygon representing the rectangle to svg_elements vector
                svg_elements.push_back(new Polygon(fill, points));
            }
            else if(strcmp(group_element_name, "g") == 0){
                // Recursively handle nested group elements
                processGroupElement(group_child->FirstChildElement(), svg_elements, group_translate_x, group_translate_y);
            }
            // Move to the next sibling element in the group
            group_child = group_child->NextSiblingElement();
        }
    }

    /**
     * Reads an SVG file and extracts its elements.
     * @param svg_file The path to the SVG file.
     * @param dimensions A Point object to store the dimensions (width, height) of the SVG file.
     * @param svg_elements A vector to store the extracted SVG elements.
     */
    void readSVG(const string& svg_file, Point& dimensions, vector<SVGElement *>& svg_elements) {
        // Load the SVG file into an XML document, provided from the beggining
        XMLDocument doc;
        XMLError r = doc.LoadFile(svg_file.c_str());
        if(r != XML_SUCCESS)
        {
            throw runtime_error("Unable to load " + svg_file);
        }

        // Get the root element of the SVG file
        XMLElement *xml_elem = doc.RootElement();

        // Retrieve the dimensions of the SVG
        dimensions.x = xml_elem->IntAttribute("width");
        dimensions.y = xml_elem->IntAttribute("height");
        
        // TODO complete code -->

        // Get the first child element
        XMLElement *child = xml_elem->FirstChildElement();

        // Iterate through each child element
        while(child) {
            // Retrieve the name of the current child SVG element
            const char *element_name = child->Name();

            // Retrieve the 'transform' attribute of the current child SVG element, if it exists
            const char *transform_attr = child->Attribute("transform");

            // Retrieve the 'transform-origin' attribute of the current child SVG element, if it exists
            const char *transform_origin_attr = child->Attribute("transform-origin");

            // Initialize transformation parameters with default values
            Point origin = {0, 0};
            int translate_x = 0;
            int translate_y = 0;
            int origin_x;
            int origin_y;
            int scale = 1;                                      // Scaling factor
            int degree = 0;                                     // Rotation angle in degrees

            // Parse transform-origin attribute if present
            if(transform_origin_attr) {
                sscanf(transform_origin_attr, "%d %d", &origin_x, &origin_y);
                origin = {origin_x, origin_y};
            }

            // Parse transform attribute if present
            if(transform_attr) {
                // Handle translation transformation
                if(strstr(transform_attr, "translate")) {
                    if(strstr(transform_attr, ",")) {
                        sscanf(transform_attr, "translate(%d,%d)", &translate_x, &translate_y);
                    } else {
                        sscanf(transform_attr, "translate(%d %d)", &translate_x, &translate_y);
                    }
                } 
                // Handle rotation transformation
                if(strstr(transform_attr, "rotate")) {
                    sscanf(transform_attr, "rotate(%d)", &degree);
                }
                // Handle scaling transformation
                if(strstr(transform_attr, "scale")) {
                    sscanf(transform_attr, "scale(%d)", &scale);
                }
            }

            // Process <ellipse> element reading his elements
            if(strcmp(element_name, "ellipse") == 0) {
                // Get the center coordinates (cx, cy) and radius (rx, ry) of the ellipse
                int cx = child->IntAttribute("cx");
                int cy = child->IntAttribute("cy");
                int rx = child->IntAttribute("rx");
                int ry = child->IntAttribute("ry");

                // Parse the fill color attribute
                Color fill = parse_color(child->Attribute("fill"));

                // Get the transform and transform-origin attributes if present
                const char* transform_str = child->Attribute("transform");
                const char* transform_origin_str = child->Attribute("transform-origin");

                // Initialize transformation parameters
                int degree = 0;                             // Rotation angle in degrees
                int scale = 1;                              // Scaling factor
                Point origin = {0, 0};                      // Origin point for transformations                                 
                Point center = {cx, cy};                    // Center point of the ellipse
                Point radius = {rx ,ry};                    // Radius of the ellipse
                int origin_x;
                int origin_y;

                // Parse transform-origin attribute if present
                if(transform_origin_str) {
                    sscanf(transform_origin_str, "%d %d", &origin_x, &origin_y);
                    origin = {origin_x, origin_y};
                }

                // Parse transform attribute if present
                if(transform_str) {
                    // Handle translation transformation
                    if(strstr(transform_str, "translate")) {
                        if(strstr(transform_str, ",")) {
                            sscanf(transform_str, "translate(%d,%d)", &translate_x, &translate_y);
                            center.x += translate_x;
                            center.y += translate_y;
                        } else {
                            sscanf(transform_str, "translate(%d %d)", &translate_x, &translate_y);
                            center.x += translate_x;
                            center.y += translate_y;
                        }
                    }
                    // Handle rotation transformation
                    if(strstr(transform_str, "rotate")) {
                        sscanf(transform_str, "rotate(%d)", &degree);
                    }
                    // Handle scaling transformation
                    if(strstr(transform_str, "scale")) {
                        sscanf(transform_str, "scale(%d)", &scale);
                    }
                }

                // Apply transformations
                if(degree != 0) {
                    center = center.rotate(origin, degree);
                    radius = radius.rotate({0, 0}, degree);
                }
                if(scale > 1) {
                    center = center.scale(origin, scale);
                    radius = radius.scale({0, 0}, scale);
                }

                // Pushes the new Ellipse created to the svg_elements vector
                svg_elements.push_back(new Ellipse(fill, center, radius));
            }

            // Process <circle> element
            else if(strcmp(element_name, "circle") == 0) {
                // Get the center coordinates (cx, cy) and radius (r) of the circle
                int cx = child->IntAttribute("cx");
                int cy = child->IntAttribute("cy");
                int r = child->IntAttribute("r");

                Color fill = parse_color(child->Attribute("fill"));

                const char* transform_str = child->Attribute("transform");
                const char* transform_origin_str = child->Attribute("transform-origin");

                int degree = 0;
                int scale = 1;
                Point origin = {0, 0};                               
                Point center = {cx, cy};                        // Center point of the circle
                int origin_x;
                int origin_y;
            
                if(transform_origin_str) {
                    sscanf(transform_origin_str, "%d %d", &origin_x, &origin_y);
                    origin = {origin_x, origin_y};
                }

                if(transform_str) {
                    if(strstr(transform_str, "translate")) {
                        if(strstr(transform_str, ",")) {
                            sscanf(transform_str, "translate(%d,%d)", &translate_x, &translate_y);
                            center.x += translate_x;
                            center.y += translate_y;
                        } else {
                            sscanf(transform_str, "translate(%d %d)", &translate_x, &translate_y);
                            center.x += translate_x;
                            center.y += translate_y;
                        }
                    } 
                    if(strstr(transform_str, "rotate")) {
                        sscanf(transform_str, "rotate(%d)", &degree);
                    }
                    if(strstr(transform_str, "scale")) {
                        sscanf(transform_str, "scale(%d)", &scale);
                    }
                }

                // Apply transformations
                if(degree != 0) {
                    center = center.rotate(origin, degree);
                }
                if(scale > 1) {
                    center = center.scale(origin, scale);
                    r = r * scale;
                }

                // Pushes the new Circle created to the svg_elements vector
                svg_elements.push_back(new Circle(fill, center, r));
            }

            // Process <polyline> element
            else if(strcmp(element_name, "polyline") == 0) {
                // Get the points of the polyline.
                const char* points_str = child->Attribute("points");

                const char* transform_str = child->Attribute("transform");
                const char* transform_origin_str = child->Attribute("transform-origin");

                // Parse the points attribute to get the vector of points
                vector<Point> points;
                parsePoints(points_str, points);

                // Parse the stroke color attribute
                Color stroke_color = parse_color(child->Attribute("stroke"));

                int degree = 0;
                int scale = 1;
                int origin_x = 0;
                int origin_y = 0;
                Point origin = {0, 0};

                if(transform_origin_str) {
                    sscanf(transform_origin_str, "%d %d", &origin_x, &origin_y);
                    origin = {origin_x, origin_y};
                }

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

                // Apply transformations to the points of the polyline
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
                
                // Pushes the new Polyline created to the svg_elements vector
                svg_elements.push_back(new Polyline(stroke_color, points));
            }

            // Process <line> element
            else if(strcmp(element_name, "line") == 0) {
                // Get the coordinates of the line
                int x1 = child->IntAttribute("x1");
                int y1 = child->IntAttribute("y1");
                int x2 = child->IntAttribute("x2");
                int y2 = child->IntAttribute("y2");

                Color stroke_color = parse_color(child->Attribute("stroke"));

                const char* transform_str = child->Attribute("transform");
                const char* transform_origin_str = child->Attribute("transform-origin");

                int degree = 0;
                int scale = 1;
                Point origin = {0, 0};
                Point p1 = {x1, y1};                            // First endpoint of the line
                Point p2 = {x2, y2};                            // Second endpoint of the line
                int origin_x;
                int origin_y;

                if(transform_origin_str) {
                    sscanf(transform_origin_str, "%d %d", &origin_x, &origin_y);
                    origin = {origin_x, origin_y};
                }

                if(transform_str) {
                    if(strstr(transform_str, "translate")) {
                        if(strstr(transform_str, ",")) {
                            sscanf(transform_str, "translate(%d,%d)", &translate_x, &translate_y);
                            p1.x += translate_x;
                            p1.y += translate_y;
                            p2.x += translate_x;
                            p2.y += translate_y;
                        } else {
                            sscanf(transform_str, "translate(%d %d)", &translate_x, &translate_y);
                            p1.x += translate_x;
                            p1.y += translate_y;
                            p2.x += translate_x;
                            p2.y += translate_y;
                        }
                    } 
                    if(strstr(transform_str, "rotate")) {
                        sscanf(transform_str, "rotate(%d)", &degree);
                    }
                    if(strstr(transform_str, "scale")) {
                        sscanf(transform_str, "scale(%d)", &scale);
                    }
                }

                // Apply transformations
                if(degree != 0) {
                    p1 = p1.rotate(origin, degree);
                    p2 = p2.rotate(origin, degree);
                }
                if(scale > 1) {
                    p1 = p1.scale(origin, scale);
                    p2 = p2.scale(origin, scale);
                }

                // Pushes the new Line created to the svg_elements vector
                svg_elements.push_back(new Line(stroke_color, p1, p2));
            }

            // Process <polygon> element
            else if(strcmp(element_name, "polygon") == 0) {
                // Get the points attribute string from the polygon element
                const char* points_str = child->Attribute("points");

                const char* transform_str = child->Attribute("transform");
                const char* transform_origin_str = child->Attribute("transform-origin");

                Color fill = parse_color(child->Attribute("fill"));

                // Parse the points attribute string from the polygon element and store the resulting points in the 'points' vector
                vector<Point> points;
                parsePoints(points_str, points);

                int degree = 0;
                int scale = 1;
                int origin_x = 0;
                int origin_y = 0;
                Point origin = {0, 0};

                if(transform_origin_str) {
                    sscanf(transform_origin_str, "%d %d", &origin_x, &origin_y);
                    origin = {origin_x, origin_y};
                }

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

                // Apply transformations
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

                // Pushes the new Polygon created to the svg_elements vector
                svg_elements.push_back(new Polygon(fill, points));
            }

            // Process <rect> element as a polygon
            else if(strcmp(element_name, "rect") == 0) {
                // Get the rectangle's position (x, y) and size (width, height) attributes
                int x = child->IntAttribute("x");
                int y = child->IntAttribute("y");
                int width = child->IntAttribute("width");
                int height = child->IntAttribute("height");

                Color fill = parse_color(child->Attribute("fill"));

                int degree = 0;
                int scale = 1;

                // Calculate the origin point for the rectangle, which is its center. This is used for rotation and scaling transformations
                Point origin = {x + width / 2, y + height / 2};
                Point rect_origin = {x, y};

                const char* transform_str = child->Attribute("transform");
                const char* transform_origin_str = child->Attribute("transform-origin");

                if(transform_origin_str) {
                    sscanf(transform_origin_str, "%d %d", &origin_x, &origin_y);
                    origin = {origin_x, origin_y};
                }

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

                // Define the four corner points of the rectangle
                vector<Point> points = {rect_origin, rect_origin.translate({width - 1, 0}), 
                    rect_origin.translate({width - 1, height - 1}), rect_origin.translate({0, height - 1})};

                // Apply transformations
                for(Point &point : points) {
                    if(degree != 0) {
                        point = point.rotate({origin_x, origin_y}, degree);
                    }
                    else if(scale > 1) {
                        point = point.scale({origin_x, origin_y}, scale);
                    }
                    point.x += translate_x;
                    point.y += translate_y;
                }

                // Pushes the new Polygon created to the svg_elements vector
                svg_elements.push_back(new Polygon(fill, points));
            }
            
            // Process <g> (group) element recursively
            else if(strcmp(element_name, "g") == 0) {
                processGroupElement(child->FirstChildElement(), svg_elements, translate_x, translate_y);
            }

            // Move to the next sibling element
            child = child->NextSiblingElement();          
        }
    }   
}