
# Programming project

## Group elements

- up202002749 -> Tomás Gonçalves Ribeiro Ferreira
- up202109258 -> Sheila Maria Lourenço Albino
- up201900188 -> Daniel Bohrer Lopes


## Accomplished tasks

#### SVG Parsing and Element Representation:

1. Defined classes representing different SVG elements such as Ellipse, Circle, Polyline, Line, Polygon and Rectangle. Each class encapsulates the properties of the respective SVG element and provides a method to draw itself onto a PNG image.
2. The SVGElement base class serves as an interface for all SVG elements, enforcing the implementation of the draw method.


#### Transformations Handling:

- Implemented support for handling various transformations specified in the SVG elements, including origin, translation, rotation, and scaling. These transformations are parsed from the SVG attributes and applied to the points defining the SVG shapes.


#### Group Elements Processing:

- Added functionality to process SVG g (group) elements, which can contain nested SVG elements. This involves recursively parsing child elements and applying group-child transformations to them which we didn´t handle very well. Group_6 and group_7 tests still failed.


#### File Reading and Conversion:

1. Implemented functions to read an SVG file (readSVG) and parse its contents into a list of SVGElement objects.

#### Drawing and Rendering:

- Each SVG element class implements the draw method to render the element onto a PNGImage object. This involves calculating the positions and dimensions of the elements, applying transformations, and then rendering them using appropriate drawing functions.


### Key Functions and Classes

1. SVGElement: Base class for all SVG elements with a virtual draw method.
Ellipse, Circle, Polyline, Line, Polygon, Rectangle: Derived classes representing specific SVG elements, each implementing the draw method.
2. readSVG: Function to read and parse an SVG file, extracting dimensions and SVG elements.
3. parse-color: Function to parse a color from a string.
4. parsePoints: Function to parse points from a string and store them in a vector of Points.
5. processGroupElement: Function to recursively process group elements and apply transformations.


### Summary of Implementations

1. SVG Elements Parsing and Drawing:

  - Created classes for different SVG elements.
  - Implemented methods to parse attributes and draw the elements onto a PNG image.


2. Transformations:

- Implemented parsing and application of transformations (origin, translate, rotate, scale) for individual elements and groups.


3. Group Elements Handling:

- Implemented recursive parsing of group elements, applying group-level transformations to nested elements.


4. File Operations:

- Implemented functions for reading SVG files and converting them to PNG images.



-> This project enables users to convert SVG graphics into PNG format, supporting a wide range of SVG features including various shapes, colors, and transformations. The implementation ensures that SVG files are accurately rendered as PNG images, preserving their visual properties and structure.

