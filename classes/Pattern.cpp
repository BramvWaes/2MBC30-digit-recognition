#include "Pattern.h"

#include <algorithm>
#include <cmath>
#include <utility>

bool Range::contains(double value) const {
    return value >= min && value <= max;
}

Range left_turn(double min, double max) {
    return Range{min, max};
}

Range right_turn(double min, double max) {
    return Range{-max, -min};
}

bool Direction::contains(double heading) const {
    return std::abs(turn_between(centre, heading)) <= tolerance;
}

Direction Direction::right(double tolerance) { return Direction{0, tolerance}; }
Direction Direction::up(double tolerance) { return Direction{90, tolerance}; }
Direction Direction::left(double tolerance) { return Direction{180, tolerance}; }
Direction Direction::down(double tolerance) { return Direction{270, tolerance}; }

bool Region::contains(Point normalised) const {
    return x.contains(normalised.x) && y.contains(normalised.y);
}

Region Region::bottom_left() { return Region{{0, 0.5}, {0, 0.5}}; }
Region Region::bottom_right() { return Region{{0.5, 1}, {0, 0.5}}; }
Region Region::top_left() { return Region{{0, 0.5}, {0.5, 1}}; }
Region Region::top_right() { return Region{{0.5, 1}, {0.5, 1}}; }

bool Shape::accepts(const Piece& piece) const {
    return (!heading || heading->contains(piece.heading))
        && (!length || length->contains(piece.length))
        && (!turning || turning->contains(piece.turning))
        && (!bulge || bulge->contains(piece.bulge))
        && (!sharpest_turn || piece.sharpest_turn <= *sharpest_turn);
}

bool Step::accepts(const Piece& piece) const {
    return shape.accepts(piece) && (!extra_check || extra_check(piece));
}

Step line(Shape shape) {
    if (!shape.bulge) {
        shape.bulge = Range{-0.2, 0.2};
    }
    if (!shape.sharpest_turn) {
        shape.sharpest_turn = 35;
    }
    return Step{"line", false, shape};
}

Step arc(Shape shape) {
    if (!shape.sharpest_turn) {
        shape.sharpest_turn = 90;
    }
    return Step{"arc", false, shape};
}

Step semicircle(Rotation rotation, Shape shape) {
    bool anticlockwise = rotation == Rotation::anticlockwise;
    if (!shape.turning) {
        shape.turning = anticlockwise ? left_turn(60, 270) : right_turn(60, 270);
    }
    if (!shape.bulge) {
        // Turning anticlockwise, the arc lies to the right of the chord.
        shape.bulge = anticlockwise ? Range{-1, -0.2} : Range{0.2, 1};
    }
    Step step = arc(shape);
    step.name = "semicircle";
    return step;
}

Step corner(Range turn, double max_size) {
    return Step{"corner", true, {}, turn, {}, max_size};
}

Step reversal(double tolerance, double max_size) {
    Step step = corner(Range{180 - tolerance, 180 + tolerance}, max_size);
    step.name = "reversal";
    step.either_side = true;
    return step;
}

Step custom(std::string name, std::function<bool(const Piece&)> check) {
    return Step{std::move(name), false, {}, {}, std::move(check)};
}

Pattern::Pattern(std::string label) : label(std::move(label)) {}

Pattern& Pattern::start_in(Region region) {
    start_region = region;
    return *this;
}

Pattern& Pattern::end_in(Region region) {
    end_region = region;
    return *this;
}

Pattern& Pattern::then(Step step) {
    steps.push_back(std::move(step));
    return *this;
}

Pattern& Pattern::smooth_joins_up_to(double degrees) {
    smooth_join = degrees;
    return *this;
}

MatchResult Pattern::match(const Stroke& stroke) const {
    MatchResult forwards = match_in_order(stroke);
    if (forwards.matched) {
        return forwards;
    }
    MatchResult backwards = match_in_order(stroke.reversed());
    return backwards.matched || backwards.steps_matched > forwards.steps_matched ? backwards : forwards;
}

MatchResult Pattern::match_in_order(const Stroke& stroke) const {
    MatchResult result;
    if (start_region && !start_region->contains(stroke.normalised(stroke.points.front()))) {
        result.failed_step = "start";
        return result;
    }
    if (end_region && !end_region->contains(stroke.normalised(stroke.points.back()))) {
        result.failed_step = "end";
        return result;
    }
    result.matched = search(stroke, 0, 0, Join{}, result);
    if (result.matched) {
        result.steps_matched = steps.size();
    }
    else {
        result.failed_step = result.steps_matched < steps.size() ? steps[result.steps_matched].name : "end";
    }
    return result;
}

// Depth-first search over the ways of dividing the stroke's vertices between the steps.
bool Pattern::search(const Stroke& stroke, std::size_t step, std::size_t vertex, Join join,
                     MatchResult& result) const {
    result.steps_matched = std::max(result.steps_matched, step);
    if (step == steps.size()) {
        return vertex == stroke.vertices.size() - 1 && !join.corner;
    }
    const Step& current = steps[step];
    if (current.is_corner) {
        if (!join.heading || join.corner) {
            return false;
        }
        if (search(stroke, step + 1, vertex, Join{join.heading, current.turn, current.either_side, 0}, result)) {
            return true;
        }
        for (std::size_t next = vertex + 1; next < stroke.vertices.size(); next++) {
            Piece rounding = stroke.piece(vertex, next);
            if (rounding.length > current.corner_size) {
                break;
            }
            double turned = turn_between(*join.heading, rounding.start_heading) + rounding.turning;
            if (search(stroke, step + 1, next, Join{rounding.end_heading, current.turn, current.either_side, turned},
                       result)) {
                return true;
            }
        }
        return false;
    }
    for (std::size_t next = vertex + 1; next < stroke.vertices.size(); next++) {
        Piece piece = stroke.piece(vertex, next);
        if (!current.accepts(piece)) {
            continue;
        }
        if (join.heading) {
            double turn = join.turned + turn_between(*join.heading, piece.start_heading);
            if (join.either_side) {
                turn = std::abs(turn);
            }
            if (join.corner ? !join.corner->contains(turn) : std::abs(turn) > smooth_join) {
                continue;
            }
        }
        if (search(stroke, step + 1, next, Join{piece.end_heading}, result)) {
            return true;
        }
    }
    return false;
}
