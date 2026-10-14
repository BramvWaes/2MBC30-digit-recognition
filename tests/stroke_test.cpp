#include <gtest/gtest.h>
#include "helpers.h"

TEST(Geometry, HeadingFollowsCompassWithYUp) {
    EXPECT_DOUBLE_EQ(heading({0, 0}, {1, 0}), 0);
    EXPECT_DOUBLE_EQ(heading({0, 0}, {0, 1}), 90);
    EXPECT_DOUBLE_EQ(heading({0, 0}, {-1, 0}), 180);
    EXPECT_DOUBLE_EQ(heading({0, 0}, {0, -1}), 270);
}

TEST(Geometry, TurnBetweenIsSignedAndWraps) {
    EXPECT_DOUBLE_EQ(turn_between(0, 90), 90);
    EXPECT_DOUBLE_EQ(turn_between(90, 0), -90);
    EXPECT_DOUBLE_EQ(turn_between(350, 10), 20);
    EXPECT_DOUBLE_EQ(turn_between(10, 350), -20);
    EXPECT_DOUBLE_EQ(turn_between(0, 180), 180);
}

TEST(Stroke, StraightLineSimplifiesToTwoVertices) {
    Stroke stroke = stroke_through({{0, 0}, {10, 0}});
    ASSERT_EQ(stroke.vertices.size(), 2u);
    Piece piece = stroke.piece(0, 1);
    EXPECT_DOUBLE_EQ(piece.heading, 0);
    EXPECT_DOUBLE_EQ(piece.length, 1);
    EXPECT_DOUBLE_EQ(piece.turning, 0);
    EXPECT_DOUBLE_EQ(piece.bulge, 0);
}

TEST(Stroke, CornerBecomesVertexWithTurn) {
    Stroke stroke = stroke_through({{0, 0}, {10, 0}, {10, 10}});
    ASSERT_EQ(stroke.vertices.size(), 3u);
    Piece whole = stroke.piece(0, 2);
    EXPECT_DOUBLE_EQ(whole.turning, 90);
    EXPECT_DOUBLE_EQ(whole.sharpest_turn, 90);
    EXPECT_DOUBLE_EQ(whole.start_heading, 0);
    EXPECT_DOUBLE_EQ(whole.end_heading, 90);
    // Travelling up and to the right, the corner at (10, 0) lies to the right of travel.
    EXPECT_LT(whole.bulge, 0);
}

TEST(Stroke, ReversedSwapsDirection) {
    Stroke stroke = stroke_through({{0, 0}, {10, 0}, {10, 10}}).reversed();
    EXPECT_DOUBLE_EQ(stroke.points.front().y, 10);
    EXPECT_EQ(stroke.vertices.front(), 0u);
    EXPECT_EQ(stroke.vertices.back(), stroke.points.size() - 1);
    EXPECT_DOUBLE_EQ(stroke.piece(0, 2).turning, -90);
}

TEST(Stroke, NormalisedUsesBottomLeftOrigin) {
    Stroke stroke = stroke_through({{2, -10}, {12, -10}, {12, 0}});
    Point bottom_right = stroke.normalised({12, -10});
    EXPECT_DOUBLE_EQ(bottom_right.x, 1);
    EXPECT_DOUBLE_EQ(bottom_right.y, 0);
}

TEST(Stroke, FromPathFlipsRowsSoUpIsPositive) {
    compactor compacted;
    compact_drawing({
        "................",
        "...#............",
        "...#............",
        "...#............",
        "...#............",
    }, compacted);
    ASSERT_EQ(compacted.segments.size(), 1u);
    Stroke stroke = Stroke::from_path(compacted.segments.front().path);
    double direction = stroke.piece(0, stroke.vertices.size() - 1).heading;
    EXPECT_TRUE(direction == 90 || direction == 270);
}
