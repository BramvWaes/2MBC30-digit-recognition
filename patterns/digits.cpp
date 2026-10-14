#include "digits.h"

namespace digits {
    namespace {
        // A stroke that comes back round to where it started. Its chord is (close to) zero, so only how far it turns
        // says anything about it.
        Step loop(Rotation rotation) {
            return arc({.length = Range{0, 0.45},
                        .turning = rotation == Rotation::anticlockwise ? left_turn(220, 420) : right_turn(220, 420)});
        }

        Region top(double from) {
            return Region{{0, 1}, {from, 1}};
        }

        Region bottom(double to) {
            return Region{{0, 1}, {0, to}};
        }
    }

    Pattern zero() {
        return Pattern("0")
            .then(loop(Rotation::anticlockwise));
    }

    Pattern one() {
        return Pattern("1")
            .then(line({.heading = Direction::down(20), .length = Range{0.9}}));
    }

    Pattern one_with_flag() {
        return Pattern("1")
            // The flag runs up and to the right into the top of the stem.
            .then(line({.heading = Direction{45, 35}, .length = Range{0.15, 0.6}}))
            .then(corner(right_turn(100, 170)))
            .then(line({.heading = Direction::down(20), .length = Range{0.7}}));
    }

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
            // Not much more than straight up or down, or a path that doubles back through a junction (a closed 4)
            // would fit as well.
            .then(semicircle(Rotation::anticlockwise, {.heading = Direction::left(70)}));
    }

    Pattern three() {
        return Pattern("3")
            .start_in(top(0.6))
            .end_in(bottom(0.4))
            .then(semicircle(Rotation::clockwise, {.heading = Direction::down(70)}))
            // The middle of a 3 skeletonises to a short dead end pointing left, traced out and back.
            .then(line({.heading = Direction::left(50), .length = Range{0, 0.6}}))
            .then(reversal())
            .then(semicircle(Rotation::clockwise, {.heading = Direction::down(70)}));
    }

    Pattern three_with_cusp() {
        return Pattern("3")
            .start_in(top(0.6))
            .end_in(bottom(0.4))
            .then(semicircle(Rotation::clockwise, {.heading = Direction::down(70)}))
            .then(corner(left_turn(90, 180)))
            .then(semicircle(Rotation::clockwise, {.heading = Direction::down(70)}));
    }

    Pattern four() {
        return Pattern("4")
            .start_in(Region::top_left())
            .then(line({.heading = Direction::down(30)}))
            .then(corner(left_turn(50, 130)))
            .then(line({.heading = Direction::right(30)}))
            .then(corner(left_turn(50, 130)))
            // The stem above the crossbar is a dead end, traced up and back down.
            .then(line({.heading = Direction::up(30), .length = Range{0.1}}))
            .then(reversal())
            .then(line({.heading = Direction::down(30), .length = Range{0.5}}));
    }

    Pattern four_closed() {
        return Pattern("4")
            // From the right end of the crossbar to the left, up the diagonal and down the stem.
            .then(line({.heading = Direction::left(30), .length = Range{0.4}}))
            .then(corner(right_turn(80, 165)))
            .then(line({.heading = Direction{55, 35}, .length = Range{0.4}}))
            .then(corner(right_turn(110, 180)))
            .then(line({.heading = Direction::down(25), .length = Range{0.6}}));
    }

    Pattern five() {
        return Pattern("5")
            .start_in(Region::top_right())
            .then(line({.heading = Direction::left(30), .length = Range{0.25}}))
            .then(corner(left_turn(50, 130)))
            .then(line({.heading = Direction::down(30), .length = Range{0.15}}))
            .then(corner(left_turn(40, 150)))
            // Measured from the start of the middle bar, the bowl can stick out further than its chord is long.
            .then(semicircle(Rotation::clockwise, {.heading = Direction::down(70), .bulge = Range{0.2, 1.6}}));
    }

    Pattern six() {
        return Pattern("6")
            .start_in(top(0.6))
            // The bowl closes on the left side, below the top; otherwise a 0 split in two would fit as well.
            .end_in(Region{{0, 0.5}, {0, 0.7}})
            // Down the left side, carrying on anticlockwise into the bowl.
            .then(arc({.heading = Direction{240, 55}, .length = Range{0.3}, .turning = Range{-45, 150}}))
            .then(loop(Rotation::anticlockwise));
    }

    Pattern seven() {
        return Pattern("7")
            .start_in(Region::top_left())
            .then(line({.heading = Direction::right(25), .length = Range{0.4}}))
            .then(corner(right_turn(90, 170)))
            // The stem often straightens out towards the bottom, so it may bend a little.
            .then(line({.heading = Direction{245, 40}, .length = Range{0.6}, .bulge = Range{-0.2, 0.2},
                        .sharpest_turn = 60}));
    }

    Pattern eight() {
        return Pattern("8")
            // Through the crossing, one loop turns one way and the other loop the other way.
            .then(loop(Rotation::anticlockwise))
            .then(loop(Rotation::clockwise));
    }

    Pattern eight_with_wide_waist() {
        return Pattern("8")
            // Where the loops share a stretch of the waist, the skeleton becomes two arcs joined by a bar, like a
            // theta. Traced as an 8: over the top, back along the bar and round the bottom.
            .then(semicircle(Rotation::clockwise, {.heading = Direction::right(45)}))
            .then(corner(right_turn(45, 170)))
            .then(line({.heading = Direction::left(30)}))
            .then(corner(left_turn(45, 170)))
            .then(semicircle(Rotation::anticlockwise, {.heading = Direction::right(45)}));
    }

    Pattern nine() {
        return Pattern("9")
            .end_in(bottom(0.4))
            // Round the bowl and carry on down the right side into the stem.
            .then(loop(Rotation::clockwise))
            .then(arc({.heading = Direction::down(45), .length = Range{0.3}, .turning = Range{-120, 90}}));
    }

    std::vector<Pattern> all() {
        return {zero(), one(), one_with_flag(), two(), three(), three_with_cusp(), four(), four_closed(), five(),
                six(), seven(), eight(), eight_with_wide_waist(), nine()};
    }
}
