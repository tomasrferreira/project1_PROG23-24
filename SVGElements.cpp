#include "SVGElements.hpp"
#include "PNGImage.hpp"
#include "Point.hpp"

namespace svg
{
    // These must be defined!
    // SVGElement::SVGElement() {}
    SVGElement::~SVGElement() {}

    // Ellipse (initial code provided)
    Ellipse::Ellipse(const Color &fill,
                     const Point &center,
                     const Point &radius)
        : fill(fill), center(center), radius(radius)
    {
    }
    void Ellipse::draw(PNGImage &img) const
    {
        img.draw_ellipse(center, radius, fill);
    }

    // @todo provide the implementation of SVGElement derived classes
    // HERE -->

    Rectangle::Rectangle(const Color &fill, const Point &start, const Point &end)
    : fill(fill), start(start), end(end)
    {
    }
    void Rectangle::draw(PNGImage &img) const
    {
        img.draw_line(start, Point({end.x, start.y}), fill); // Top line
        img.draw_line(Point({start.x, end.y}), end, fill);   // Right line
        img.draw_line(end, Point({start.x, end.y}), fill);   // Bottom line
        img.draw_line(Point({end.x, start.y}), start, fill); // Left line
    }

    Circle::Circle(const Color &fill, const Point &center, int radius)
        : fill(fill), center(center), radius(radius)
    {
    }

    void Circle::draw(PNGImage &img) const
    {
        img.draw_ellipse(center, {radius, radius}, fill);
    }

}
