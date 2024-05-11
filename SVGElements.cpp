#include "SVGElements.hpp"
#include "PNGImage.hpp"
#include "Point.hpp"

namespace svg
{
    // These must be defined!
    // SVGElement::SVGElement() {}
    SVGElement::~SVGElement() {}

    // Ellipse (initial code provided)
    Ellipse::Ellipse(const Color &fill, const Point &center, const Point &radius)
        : fill(fill), center(center), radius(radius)
    {
    }
    void Ellipse::draw(PNGImage &img) const
    {
        img.draw_ellipse(center, radius, fill);
    }

    // @todo provide the implementation of SVGElement derived classes

    Circle::Circle(const Color &fill, const Point &center, int radius)
        : fill(fill), center(center), radius(radius)
    {
    }
    void Circle::draw(PNGImage &img) const
    {
        img.draw_ellipse(center, {radius, radius}, fill);
    }

    Polyline::Polyline(const Color &stroke_color, const std::vector<Point>& polyline_points)
        : stroke(stroke_color), points(polyline_points)
    {
    }
    void Polyline::draw(PNGImage &img) const
    {
        for (size_t i = 0; i < points.size() - 1; ++i) {
            img.draw_line(points[i], points[i + 1], stroke);
        }
    }

    Line::Line(const Color &stroke_color, Point start_point, Point end_point)
        : stroke(stroke_color), start(start_point), end(end_point)
    {  
    }
    void Line::draw(PNGImage &img) const
    {
        img.draw_line(start, end, stroke);
    }

    Polygon::Polygon(const Color &fill, const std::vector<Point>& points)
        : fill(fill), points(points) 
    {
    }
    void Polygon::draw(PNGImage &img) const
    {
        img.draw_polygon(points, fill);
    }

    Rectangle::Rectangle(const Color &fill, const Point &start, const Point &end)
        : fill(fill), start(start), end(end)
    {
    }
    void Rectangle::draw(PNGImage &img) const
    {
        std::vector<Point> points = {start, {end.x, start.y}, end, {start.x, end.y}, start};
        img.draw_polygon(points, fill);
    }
}
