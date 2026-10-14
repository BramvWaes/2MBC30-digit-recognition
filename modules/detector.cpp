#include "detector.h"

std::optional<std::string> detector::detect(const compactor& compacted) const {
    if (compacted.segments.size() != 1) {
        return std::nullopt;
    }
    Stroke stroke = Stroke::from_path(compacted.segments.front().path);
    for (const Pattern& pattern : patterns) {
        if (pattern.match(stroke).matched) {
            return pattern.label;
        }
    }
    return std::nullopt;
}
