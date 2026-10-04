//
// Created by Bram on 2-10-2026.
//

#ifndef INC_2MBC30_PATH_TRAVERSAL_NORMALISER_H
#define INC_2MBC30_PATH_TRAVERSAL_NORMALISER_H

#include <array>

class normaliser {
public:
    float threshold_value;
    float input_array[256];

    std::array<std::array<int, 16>, 16> normalise();
};


#endif //INC_2MBC30_PATH_TRAVERSAL_NORMALISER_H
