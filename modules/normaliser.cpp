//
// Created by Bram on 2-10-2026.
//

#include "normaliser.h"

std::array<std::array<int, 16>, 16> normaliser::normalise() {
    std::array<std::array<int, 16>, 16> output_matrix{};
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if (input_matrix[i][j] > threshold_value) {
                output_matrix[i][j] = 0;
            }
            else {
                output_matrix[i][j] = 1;
            }
        }
    }
    return output_matrix;
}
