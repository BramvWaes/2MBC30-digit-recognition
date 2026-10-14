#include "digits.h"

namespace digits {
    Pattern two() {
        return Pattern("2")
            .start_in(Region::bottom_right())
            // Move to the left in a straight line or an arc that peaks slightly upwards
            // (when moving left, upwards is to the right of travel, hence the negative bulge). The bulge already
            // limits how far this may curve, so pixel steps in a slight arc are not held to the usual sharpness.
            .then(line({.heading = Direction::left(35), .length = Range{0.25}, .bulge = Range{-0.3, 0.15},
                        .sharpest_turn = 90}))
            .then(corner(right_turn(90, 170)))
            .then(line({.length = Range{0.25}}))
            .then(semicircle(Rotation::anticlockwise, {.heading = Direction::left(90)}));
    }

    std::vector<Pattern> all() {
        return {two()};
    }
}
