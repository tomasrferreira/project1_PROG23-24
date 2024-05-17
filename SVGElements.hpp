//! @file shape.hpp
#ifndef __svg_SVGElements_hpp__
#define __svg_SVGElements_hpp__

#include "Color.hpp"
#include "Point.hpp"
#include "PNGImage.hpp"
using namespace std;

namespace svg
{
    //! Base class for SVG elements.
    class SVGElement
    {
    public:
        //! Default constructor for SVGElement.
        SVGElement(){}
        //! Virtual destructor for SVGElement.
        virtual ~SVGElement();

        //! Draw the SVG element onto a PNG image.
        //! @param img The PNG image to draw on.
        virtual void draw(PNGImage &img) const = 0;
    };
   
    // Declaration of namespace functions
    // readSVG -> implement it in readSVG.cpp
    // convert -> already given (DO NOT CHANGE) in convert.cpp


    //! Read and parse an SVG file.
    //! @param svg_file The path to the SVG file.
    //! @param dimensions The dimensions of the SVG canvas.
    //! @param svg_elements The list of SVG elements parsed from the file.
    void readSVG(const std::string &svg_file,
                 Point &dimensions,
                 std::vector<SVGElement *> &svg_elements);
                 
    //! Convert an SVG file to a PNG file.
    //! @param svg_file The path to the SVG file.
    //! @param png_file The path to the output PNG file.
    void convert(const std::string &svg_file,
                 const std::string &png_file);

    //! Class representing an ellipse in SVG.
    class Ellipse : public SVGElement
    {
    public:
        //! Constructor for Ellipse.
        //! @param fill The fill color of the ellipse.
        //! @param center The center point of the ellipse.
        //! @param radius The radius (x and y) of the ellipse.
        Ellipse(const Color &fill, const Point &center, const Point &radius);

        //! Draw the ellipse onto a PNG image.
        //! @param img The PNG image to draw on.
        void draw(PNGImage &img) const override;

    private:
        Color fill;
        Point center;
        Point radius;
    };

    //! Class representing a circle in SVG.
    class Circle : public SVGElement
    {
    public:
        //! Constructor for Circle.
        //! @param fill The fill color of the circle.
        //! @param center The center point of the circle.
        //! @param radius The radius of the circle.
        Circle(const Color &fill, const Point &center, int radius);

        //! Draw the circle onto a PNG image.
        //! @param img The PNG image to draw on.
        void draw(PNGImage &img) const override;

    private:
        Color fill;
        Point center;
        int radius;
    };

    //! Class representing a polyline in SVG.
    class Polyline : public SVGElement
    {
    public:
        //! Constructor for Polyline.
        //! @param stroke_color The stroke color of the polyline.
        //! @param polyline_points The points making up the polyline.
        Polyline(const Color &stroke_color, const vector<Point>& polyline_points);

        //! Draw the polyline onto a PNG image.
        //! @param img The PNG image to draw on.
        void draw(PNGImage &img) const override;

    private:
        Color stroke;
        vector<Point> points;
    };

    //! Class representing a line in SVG.
    class Line : public SVGElement
    {
    public:
        //! Constructor for Line.
        //! @param stroke_color The stroke color of the line.
        //! @param start_point The start point of the line.
        //! @param end_point The end point of the line.
        Line(const Color &stroke_color, Point start_point, Point end_point);

        //! Draw the line onto a PNG image.
        //! @param img The PNG image to draw on.
        void draw(PNGImage &img) const override;

    private:
        Color stroke;
        Point start;
        Point end;
    };

    //! Class representing a polygon in SVG.
    class Polygon : public SVGElement
    {
    public:
        //! Constructor for Polygon.
        //! @param fill The fill color of the polygon.
        //! @param points The points making up the polygon.
        Polygon(const Color& fill, const vector<Point>& points);

        //! Draw the polygon onto a PNG image.
        //! @param img The PNG image to draw on.
        void draw(PNGImage& img) const override;

    private:
        Color fill;
        vector<Point> points;
    };

    //! Class representing a rectangle in SVG.
    class Rectangle : public Polygon 
    {
    public:
        //! Constructor for Rectangle.
        //! @param fill The fill color of the rectangle.
        //! @param origin The origin point of the rectangle.
        //! @param width The width of the rectangle.
        //! @param height The height of the rectangle.
        Rectangle(const Color &fill, const Point &origin, int width, int height);
    };
}
#endif
