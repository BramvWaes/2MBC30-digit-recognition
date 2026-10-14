#ifndef INC_2MBC30_PATH_TRAVERSAL_STROKE_H
#define INC_2MBC30_PATH_TRAVERSAL_STROKE_H

#include <cstddef>
#include <vector>
#include "Node.h"

// x grows to the right and y grows upwards, so headings follow the usual convention:
// 0 = right, 90 = up, 180 = left, 270 = down. Positive turns are to the left (anticlockwise).
struct Point {
    double x;
    double y;
};

// Direction from one point to another in degrees, in [0, 360).
double heading(Point from, Point to);

// Signed change from one heading to another in degrees, in (-180, 180].
double turn_between(double from_heading, double to_heading);

// Measurements of the part of a stroke between two of its vertices.
struct Piece {
    Point start;
    Point end;
    double heading;        // direction of the chord from start to end
    double length;         // chord length relative to the size of the whole figure
    double turning;        // sum of the signed turns at the vertices inside the piece
    double sharpest_turn;  // largest absolute turn at a single vertex inside the piece
    double bulge;          // furthest distance from the chord relative to its length, positive when left of travel
    double start_heading;  // direction of the first edge
    double end_heading;    // direction of the last edge
};

// A single line through the figure, simplified to a polyline. Its vertices are the places where one step of a
// pattern can end and the next one can begin.
class Stroke {
public:
    std::vector<Point> points;
    std::vector<std::size_t> vertices;  // indices into points, first and last point always included
    double size;                        // longest side of the bounding box

    explicit Stroke(std::vector<Point> points, double tolerance = 0.6);
    static Stroke from_path(const std::vector<Node*>& path, double tolerance = 0.6);

    Stroke reversed() const;
    Piece piece(std::size_t first_vertex, std::size_t last_vertex) const;

    // Position inside the bounding box, both coordinates in [0, 1] with (0, 0) at the bottom left.
    Point normalised(Point point) const;

private:
    double min_x = 0;
    double max_x = 0;
    double min_y = 0;
    double max_y = 0;
};


#endif //INC_2MBC30_PATH_TRAVERSAL_STROKE_H
