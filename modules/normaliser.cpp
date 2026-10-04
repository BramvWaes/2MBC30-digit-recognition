//
// Created by Bram on 2-10-2026.
//

#include "normaliser.h"

std::array<std::array<int, 16>, 16> normaliser::normalise() {
    std::array<std::array<int, 16>, 16> output_matrix{};
    for (int i = 0; i < 256; i++) {
        int row = i / 16;
        int column = i % 16;
        if (input_array[i] > threshold_value) {
            output_matrix[row][column] = 1;
        }
        else {
            output_matrix[row][column] = 0;
        }
    }
    return output_matrix;
}
