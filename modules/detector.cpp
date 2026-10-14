#include "detector.h"

#include "tracer.h"

std::optional<std::string> detector::detect(const compactor& compacted) const {
    std::vector<Stroke> strokes;
    for (const std::vector<Node*>& path : traces(compacted)) {
        strokes.push_back(Stroke::from_path(path));
    }
    for (const Pattern& pattern : patterns) {
        for (const Stroke& stroke : strokes) {
            if (pattern.match(stroke).matched) {
                return pattern.label;
            }
        }
    }
    return std::nullopt;
}
