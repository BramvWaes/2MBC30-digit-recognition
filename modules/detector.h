#ifndef INC_2MBC30_PATH_TRAVERSAL_DETECTOR_H
#define INC_2MBC30_PATH_TRAVERSAL_DETECTOR_H

#include <optional>
#include <string>
#include <vector>
#include "compactor.h"
#include "../classes/Pattern.h"
#include "../patterns/digits.h"

class detector {
public:
    std::vector<Pattern> patterns = digits::all();

    // Label of the first pattern that matches any way of tracing the compacted figure in one go.
    std::optional<std::string> detect(const compactor& compacted) const;
};


#endif //INC_2MBC30_PATH_TRAVERSAL_DETECTOR_H
