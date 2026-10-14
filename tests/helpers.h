#ifndef INC_2MBC30_PATH_TRAVERSAL_HELPERS_H
#define INC_2MBC30_PATH_TRAVERSAL_HELPERS_H

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include "classes/Stroke.h"
#include "modules/compactor.h"
#include "modules/normaliser.h"

// Runs a drawing ('#' is ink, anything else is background, missing rows/columns are background) through the
// normaliser and compactor. compactor holds pointers into itself, so it is filled in place rather than returned.
inline void compact_drawing(const std::vector<std::string>& rows, compactor& output) {
    normaliser input{};
    input.threshold_value = 0.5f;
    for (int i = 0; i < 256; i++) {
        int row = i / 16;
        int column = i % 16;
        bool ink = row < static_cast<int>(rows.size()) && column < static_cast<int>(rows[row].size())
                   && rows[row][column] == '#';
        input.input_array[i] = ink ? 0.9f : 0.1f;
    }
    output.input_matrix = input.normalise();
    output.compact();
}

// Stroke through the given points, with x to the right and y upwards.
inline Stroke stroke_through(const std::vector<Point>& corners) {
    std::vector<Point> points;
    for (std::size_t i = 0; i + 1 < corners.size(); i++) {
        Point from = corners[i];
        Point to = corners[i + 1];
        int steps = static_cast<int>(std::max(std::abs(to.x - from.x), std::abs(to.y - from.y)));
        for (int s = 0; s < steps; s++) {
            points.push_back(Point{from.x + (to.x - from.x) * s / steps, from.y + (to.y - from.y) * s / steps});
        }
    }
    points.push_back(corners.back());
    return Stroke(points);
}


#endif //INC_2MBC30_PATH_TRAVERSAL_HELPERS_H
