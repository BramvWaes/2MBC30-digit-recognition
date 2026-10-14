#include <gtest/gtest.h>
#include "helpers.h"
#include "modules/detector.h"
#include "patterns/digits.h"

namespace {
    MatchResult match_two(const std::vector<std::string>& drawing) {
        compactor compacted;
        compact_drawing(drawing, compacted);
        if (compacted.segments.size() != 1) {
            ADD_FAILURE() << "drawing compacted to " << compacted.segments.size() << " segments";
            return {};
        }
        return digits::two().match(Stroke::from_path(compacted.segments.front().path));
    }

    std::optional<std::string> detect(const std::vector<std::string>& drawing) {
        compactor compacted;
        compact_drawing(drawing, compacted);
        return detector{}.detect(compacted);
    }

    const std::vector<std::string> thick_two = {
        "................",
        "....######......",
        "...###..###.....",
        "..........##....",
        "..........##....",
        ".........###....",
        "........###.....",
        ".......###......",
        "......###.......",
        ".....###........",
        "....###.........",
        "...##########...",
        "...##########...",
    };

    const std::vector<std::string> thin_two = {
        "................",
        "....#####.......",
        "...#.....#......",
        "..........#.....",
        "..........#.....",
        ".........#......",
        "........#.......",
        ".......#........",
        "......#.........",
        ".....#..........",
        "....#...........",
        "...#########....",
    };

    // The bottom is an arc that peaks slightly upwards instead of a straight line.
    const std::vector<std::string> curved_bottom_two = {
        "................",
        ".....####.......",
        "....#....#......",
        ".........#......",
        ".........#......",
        "........#.......",
        ".......#........",
        "......#.........",
        ".....#..........",
        "....#...........",
        "...#............",
        "..#....###......",
        "...####...###...",
    };

    const std::vector<std::string> small_two = {
        "................",
        "................",
        "................",
        "................",
        "..........###...",
        ".........#...#..",
        ".............#..",
        "............#...",
        "...........#....",
        "..........#.....",
        ".........#####..",
    };

    // Same start as a 2, but a sharp corner at the top instead of a semicircle.
    const std::vector<std::string> z = {
        "................",
        "...#######......",
        "..........#.....",
        ".........#......",
        "........#.......",
        ".......#........",
        "......#.........",
        ".....#..........",
        "....#...........",
        "...#............",
        "...#########....",
    };

    // Same start as a 2, but the stroke stops after the diagonal.
    const std::vector<std::string> no_hook = {
        "................",
        "..........#.....",
        ".........#......",
        "........#.......",
        ".......#........",
        "......#.........",
        ".....#..........",
        "....#...........",
        "...#............",
        "...#########....",
    };

    const std::vector<std::string> seven = {
        "................",
        "...########.....",
        "..........#.....",
        ".........#......",
        "........#.......",
        ".......#........",
        "......#.........",
        ".....#..........",
        "....#...........",
        "....#...........",
        "....#...........",
    };

    const std::vector<std::string> five = {
        "................",
        "....######......",
        "....#...........",
        "....#...........",
        "....#####.......",
        ".........#......",
        "..........#.....",
        "..........#.....",
        "..........#.....",
        ".........#......",
        "....#####.......",
    };

    const std::vector<std::string> one = {
        "................",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
    };

    const std::vector<std::string> zero = {
        "................",
        ".....######.....",
        "....########....",
        "...###....###...",
        "...##......##...",
        "...##......##...",
        "...##......##...",
        "...##......##...",
        "...###....###...",
        "....########....",
        ".....######.....",
    };

    const std::vector<std::string> four = {
        "................",
        "...##.....##....",
        "...##.....##....",
        "...##.....##....",
        "...##.....##....",
        "...#########....",
        "...#########....",
        "..........##....",
        "..........##....",
        "..........##....",
    };
}

TEST(Two, MatchesThickTwo) {
    EXPECT_TRUE(match_two(thick_two).matched);
}

TEST(Two, MatchesThinTwo) {
    EXPECT_TRUE(match_two(thin_two).matched);
}

TEST(Two, MatchesTwoWithCurvedBottom) {
    EXPECT_TRUE(match_two(curved_bottom_two).matched);
}

TEST(Two, MatchesSmallTwoAwayFromTheCentre) {
    EXPECT_TRUE(match_two(small_two).matched);
}

TEST(Two, RejectsSharpCornerInsteadOfSemicircle) {
    MatchResult result = match_two(z);
    EXPECT_FALSE(result.matched);
    EXPECT_EQ(result.failed_step, "semicircle");
}

TEST(Two, RejectsStrokeWithoutHook) {
    EXPECT_FALSE(match_two(no_hook).matched);
}

TEST(Two, RejectsSeven) {
    EXPECT_FALSE(match_two(seven).matched);
}

TEST(Two, RejectsFive) {
    EXPECT_FALSE(match_two(five).matched);
}

TEST(Two, RejectsOne) {
    EXPECT_FALSE(match_two(one).matched);
}

TEST(Two, RejectsZero) {
    EXPECT_FALSE(match_two(zero).matched);
}

TEST(Detector, DetectsTwo) {
    EXPECT_EQ(detect(thick_two), "2");
    EXPECT_EQ(detect(thin_two), "2");
}

TEST(Detector, ReturnsNothingForOtherFigures) {
    EXPECT_EQ(detect(seven), std::nullopt);
    EXPECT_EQ(detect(zero), std::nullopt);
}

TEST(Detector, ReturnsNothingForFiguresWithJunctions) {
    EXPECT_EQ(detect(four), std::nullopt);
}
