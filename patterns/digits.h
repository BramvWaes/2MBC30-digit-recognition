#ifndef INC_2MBC30_PATH_TRAVERSAL_DIGITS_H
#define INC_2MBC30_PATH_TRAVERSAL_DIGITS_H

#include <vector>
#include "../classes/Pattern.h"

// Each pattern describes one way of tracing the digit's skeleton in one go. Digits that are commonly written in
// clearly different shapes have one pattern per shape, all with the same label.
namespace digits {
    Pattern zero();
    Pattern one();
    Pattern one_with_flag();
    Pattern two();
    Pattern three();
    Pattern three_with_cusp();
    Pattern four();
    Pattern four_closed();
    Pattern five();
    Pattern six();
    Pattern seven();
    Pattern eight();
    Pattern eight_with_wide_waist();
    Pattern nine();

    std::vector<Pattern> all();
}


#endif //INC_2MBC30_PATH_TRAVERSAL_DIGITS_H
