#include "Stroke.h"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <utility>

namespace {
    double distance_to_line(Point point, Point a, Point b) {
        double dx = b.x - a.x;
        double dy = b.y - a.y;
        double length = std::hypot(dx, dy);
        if (length == 0) {
            return std::hypot(point.x - a.x, point.y - a.y);
        }
        return std::abs(dx * (point.y - a.y) - dy * (point.x - a.x)) / length;
    }

    // Ramer-Douglas-Peucker: keep the point furthest from the chord as long as it is further than the tolerance.
    void simplify(const std::vector<Point>& points, std::size_t first, std::size_t last, double tolerance,
                  std::vector<std::size_t>& vertices) {
        double furthest = 0;
        std::size_t index = first;
        for (std::size_t i = first + 1; i < last; i++) {
            double distance = distance_to_line(points[i], points[first], points[last]);
            if (distance > furthest) {
                furthest = distance;
                index = i;
            }
        }
        if (furthest > tolerance) {
            simplify(points, first, index, tolerance, vertices);
            vertices.push_back(index);
            simplify(points, index, last, tolerance, vertices);
        }
    }
}

double heading(Point from, Point to) {
    double degrees = std::atan2(to.y - from.y, to.x - from.x) * 180 / std::numbers::pi;
    return degrees < 0 ? degrees + 360 : degrees;
}

double turn_between(double from_heading, double to_heading) {
    double turn = std::fmod(to_heading - from_heading, 360.0);
    if (turn <= -180) {
        turn += 360;
    }
    else if (turn > 180) {
        turn -= 360;
    }
    return turn;
}

Stroke::Stroke(std::vector<Point> points, double tolerance) : points(std::move(points)) {
    vertices.push_back(0);
    if (this->points.size() > 1) {
        simplify(this->points, 0, this->points.size() - 1, tolerance, vertices);
        vertices.push_back(this->points.size() - 1);
    }
    if (!this->points.empty()) {
        auto by_x = [](Point a, Point b) { return a.x < b.x; };
        auto by_y = [](Point a, Point b) { return a.y < b.y; };
        auto [lowest_x, highest_x] = std::minmax_element(this->points.begin(), this->points.end(), by_x);
        auto [lowest_y, highest_y] = std::minmax_element(this->points.begin(), this->points.end(), by_y);
        min_x = lowest_x->x;
        max_x = highest_x->x;
        min_y = lowest_y->y;
        max_y = highest_y->y;
    }
    size = std::max({max_x - min_x, max_y - min_y, 1.0});
}

Stroke Stroke::from_path(const std::vector<Node*>& path, double tolerance) {
    std::vector<Point> points;
    for (Node* node : path) {
        points.push_back(Point{static_cast<double>(node->column), static_cast<double>(-node->row)});
    }
    return Stroke(std::move(points), tolerance);
}

Stroke Stroke::reversed() const {
    Stroke stroke = *this;
    std::reverse(stroke.points.begin(), stroke.points.end());
    std::reverse(stroke.vertices.begin(), stroke.vertices.end());
    for (std::size_t& vertex : stroke.vertices) {
        vertex = points.size() - 1 - vertex;
    }
    return stroke;
}

Piece Stroke::piece(std::size_t first_vertex, std::size_t last_vertex) const {
    Point start = points[vertices[first_vertex]];
    Point end = points[vertices[last_vertex]];
    double dx = end.x - start.x;
    double dy = end.y - start.y;
    double chord = std::hypot(dx, dy);

    Piece piece{start, end, heading(start, end), chord / size, 0, 0, 0,
                heading(start, points[vertices[first_vertex + 1]]),
                heading(points[vertices[last_vertex - 1]], end)};

    for (std::size_t v = first_vertex + 1; v < last_vertex; v++) {
        double turn = turn_between(heading(points[vertices[v - 1]], points[vertices[v]]),
                                   heading(points[vertices[v]], points[vertices[v + 1]]));
        piece.turning += turn;
        piece.sharpest_turn = std::max(piece.sharpest_turn, std::abs(turn));
    }

    if (chord > 0) {
        double furthest = 0;
        for (std::size_t i = vertices[first_vertex]; i <= vertices[last_vertex]; i++) {
            double offset = (dx * (points[i].y - start.y) - dy * (points[i].x - start.x)) / chord;
            if (std::abs(offset) > std::abs(furthest)) {
                furthest = offset;
            }
        }
        piece.bulge = furthest / chord;
    }
    return piece;
}

Point Stroke::normalised(Point point) const {
    double width = max_x - min_x;
    double height = max_y - min_y;
    return Point{width > 0 ? (point.x - min_x) / width : 0.5, height > 0 ? (point.y - min_y) / height : 0.5};
}
