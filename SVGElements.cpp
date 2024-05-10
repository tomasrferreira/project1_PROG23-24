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

    Rectangle::Rectangle(const Color &fill, const Point &start, const Point &end)
        : fill(fill), start(start), end(end)
    {
    }
    void Rectangle::draw(PNGImage &img) const
    {
        std::vector<Point> points = {start, {end.x, start.y}, end, {start.x, end.y}};
        img.draw_polygon(points, fill);
    }

    Circle::Circle(const Color &fill, const Point &center, int radius)
        : fill(fill), center(center), radius(radius)
    {
    }
    void Circle::draw(PNGImage &img) const
    {
        img.draw_ellipse(center, {radius, radius}, fill);
    }

    Polygon::Polygon(const Color &fill, const std::vector<Point>& points)
        : fill(fill), points(points) 
    {
    }
    void Polygon::draw(PNGImage &img) const
    {
        img.draw_polygon(points, fill);
    }
}
