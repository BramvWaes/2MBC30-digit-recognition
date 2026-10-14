#ifndef INC_2MBC30_PATH_TRAVERSAL_PATTERN_H
#define INC_2MBC30_PATH_TRAVERSAL_PATTERN_H

#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <string>
#include <vector>
#include "Stroke.h"

struct Range {
    double min = -std::numeric_limits<double>::infinity();
    double max = std::numeric_limits<double>::infinity();

    bool contains(double value) const;
};

// Turn ranges in degrees, e.g. right_turn(90, 170) for "an angle to the right between 90 and 170 degrees".
Range left_turn(double min, double max);
Range right_turn(double min, double max);

// A heading of centre +- tolerance degrees.
struct Direction {
    double centre;
    double tolerance;

    bool contains(double heading) const;

    static Direction right(double tolerance = 45);
    static Direction up(double tolerance = 45);
    static Direction left(double tolerance = 45);
    static Direction down(double tolerance = 45);
};

// Part of the figure's bounding box, in normalised coordinates with (0, 0) at the bottom left.
struct Region {
    Range x{0, 1};
    Range y{0, 1};

    bool contains(Point normalised) const;

    static Region bottom_left();
    static Region bottom_right();
    static Region top_left();
    static Region top_right();
};

enum class Rotation { anticlockwise, clockwise };

// Constraints on one piece of the stroke; anything left empty is not checked. See Piece for what each one measures.
struct Shape {
    std::optional<Direction> heading;
    std::optional<Range> length;
    std::optional<Range> turning;
    std::optional<Range> bulge;
    std::optional<double> sharpest_turn;  // upper bound

    bool accepts(const Piece& piece) const;
};

// One instruction. A piece step consumes part of the stroke. A corner step constrains the total turn between the
// pieces before and after it, and may absorb short edges in between so that rounded corners still count.
struct Step {
    std::string name;
    bool is_corner = false;
    Shape shape;
    Range turn;
    std::function<bool(const Piece&)> extra_check;
    double corner_size = 0;  // longest stretch a corner may absorb, relative to the figure size

    bool accepts(const Piece& piece) const;
};

Step line(Shape shape = {});
Step arc(Shape shape = {});
// Recognised by how far it bulges from its chord rather than by its turning alone, because a small arc simplifies
// to only a few edges whose turns add up to well under 180 degrees.
Step semicircle(Rotation rotation, Shape shape = {});
Step corner(Range turn, double max_size = 0.25);
Step custom(std::string name, std::function<bool(const Piece&)> check);

struct MatchResult {
    bool matched = false;
    std::size_t steps_matched = 0;  // how far the best attempt got, useful when tweaking a pattern
    std::string failed_step;
};

// A figure described as a sequence of steps that, followed from start to end, trace the whole stroke.
class Pattern {
public:
    std::string label;
    std::vector<Step> steps;

    explicit Pattern(std::string label);

    Pattern& start_in(Region region);
    Pattern& end_in(Region region);
    Pattern& then(Step step);
    // Largest turn allowed between two consecutive pieces that have no corner step between them.
    Pattern& smooth_joins_up_to(double degrees);

    // Tries the stroke in both directions.
    MatchResult match(const Stroke& stroke) const;

private:
    std::optional<Region> start_region;
    std::optional<Region> end_region;
    double smooth_join = 90;

    // State carried from one step to the next.
    struct Join {
        std::optional<double> heading;  // direction the previous piece ended in
        std::optional<Range> corner;    // turn required by a pending corner step
        double turned = 0;              // turn already made inside the pending corner
    };

    MatchResult match_in_order(const Stroke& stroke) const;
    bool search(const Stroke& stroke, std::size_t step, std::size_t vertex, Join join, MatchResult& result) const;
};


#endif //INC_2MBC30_PATH_TRAVERSAL_PATTERN_H
