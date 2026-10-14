#include <gtest/gtest.h>
#include "classes/Pattern.h"
#include "helpers.h"

TEST(Building, TurnRangesUseLeftAsPositive) {
    EXPECT_TRUE(right_turn(90, 170).contains(-120));
    EXPECT_FALSE(right_turn(90, 170).contains(120));
    EXPECT_TRUE(left_turn(90, 170).contains(120));
}

TEST(Building, DirectionWrapsAroundZero) {
    EXPECT_TRUE(Direction::right(20).contains(350));
    EXPECT_TRUE(Direction::right(20).contains(10));
    EXPECT_FALSE(Direction::right(20).contains(30));
}

TEST(Building, LineRejectsBentPieces) {
    Stroke bent = stroke_through({{0, 0}, {10, 0}, {10, 10}});
    EXPECT_FALSE(line().accepts(bent.piece(0, 2)));
    EXPECT_TRUE(line().accepts(bent.piece(0, 1)));
}

TEST(Building, SemicircleChecksRotation) {
    // Half an octagon going over the top from right to left: anticlockwise.
    Stroke over = stroke_through({{10, 0}, {10, 4}, {7, 7}, {3, 7}, {0, 4}, {0, 0}});
    Piece piece = over.piece(0, over.vertices.size() - 1);
    EXPECT_TRUE(semicircle(Rotation::anticlockwise).accepts(piece));
    EXPECT_FALSE(semicircle(Rotation::clockwise).accepts(piece));
}

TEST(Building, CustomStepUsesCheck) {
    Step long_piece = custom("long", [](const Piece& piece) { return piece.length > 0.9; });
    Stroke stroke = stroke_through({{0, 0}, {10, 0}});
    EXPECT_TRUE(long_piece.accepts(stroke.piece(0, 1)));
}

namespace {
    // An L drawn from the top: down, then right.
    Pattern l_shape() {
        return Pattern("L")
            .start_in(Region::top_left())
            .then(line({.heading = Direction::down(20)}))
            .then(corner(left_turn(80, 100)))
            .then(line({.heading = Direction::right(20)}));
    }
}

TEST(PatternMatch, MatchesInEitherDirection) {
    Stroke drawn_from_top = stroke_through({{0, 10}, {0, 0}, {8, 0}});
    EXPECT_TRUE(l_shape().match(drawn_from_top).matched);
    EXPECT_TRUE(l_shape().match(drawn_from_top.reversed()).matched);
}

TEST(PatternMatch, RejectsWrongCorner) {
    // A mirrored L turns right instead of left.
    Stroke mirrored = stroke_through({{8, 10}, {8, 0}, {0, 0}});
    MatchResult result = l_shape().match(mirrored);
    EXPECT_FALSE(result.matched);
}

TEST(PatternMatch, RequiresWholeStrokeToBeUsed) {
    Stroke with_tail = stroke_through({{0, 10}, {0, 0}, {8, 0}, {8, 6}});
    MatchResult result = l_shape().match(with_tail);
    EXPECT_FALSE(result.matched);
}

TEST(PatternMatch, ReportsHowFarItGot) {
    Pattern pattern = Pattern("down then up")
        .then(line({.heading = Direction::down(20)}))
        .then(line({.heading = Direction::up(20)}));
    Stroke stroke = stroke_through({{0, 10}, {0, 0}, {8, 0}});
    MatchResult result = pattern.match(stroke);
    EXPECT_FALSE(result.matched);
    EXPECT_EQ(result.steps_matched, 1u);
    EXPECT_EQ(result.failed_step, "line");
}

TEST(PatternMatch, SmoothJoinLimitIsAdjustable) {
    Stroke stroke = stroke_through({{0, 0}, {10, 0}, {20, 7}});
    Pattern strict = Pattern("strict").smooth_joins_up_to(20).then(line()).then(line());
    Pattern loose = Pattern("loose").smooth_joins_up_to(45).then(line()).then(line());
    EXPECT_FALSE(strict.match(stroke).matched);
    EXPECT_TRUE(loose.match(stroke).matched);
}
