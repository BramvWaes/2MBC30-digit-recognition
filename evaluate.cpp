#include <array>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include "modules/compactor.h"
#include "modules/detector.h"
#include "modules/normaliser.h"

// Runs the detector on every image of a data set and reports, per digit, how often it was recognised.
//
// Usage: evaluate [data directory] [set name...]
// The data directory defaults to the project's data folder and the sets to "test" and "training". A set named <name>
// consists of <name>images.csv, with one row per pixel (row-major, 16x16) and one column per image, holding values
// from -1 (background) to 1 (ink), and <name>imageslabels.csv, with the correct digit of each image on its own line.

namespace {
    constexpr float threshold = 0.0f;
    // Column index for images where the detector found nothing.
    constexpr int none = 10;

    struct DataSet {
        std::vector<std::array<float, 256>> images;
        std::vector<int> labels;
    };

    std::vector<std::string> split(const std::string& line) {
        std::vector<std::string> fields;
        std::stringstream stream(line);
        std::string field;
        while (std::getline(stream, field, ',')) {
            fields.push_back(field);
        }
        return fields;
    }

    std::optional<DataSet> load(const std::string& directory, const std::string& name) {
        std::string images_path = directory + "/" + name + "images.csv";
        std::string labels_path = directory + "/" + name + "imageslabels.csv";
        std::ifstream images_file(images_path);
        std::ifstream labels_file(labels_path);
        if (!images_file) {
            std::cerr << "cannot open " << images_path << "\n";
            return std::nullopt;
        }
        if (!labels_file) {
            std::cerr << "cannot open " << labels_path << "\n";
            return std::nullopt;
        }

        DataSet data;
        std::string line;
        while (std::getline(labels_file, line)) {
            if (line.find_first_not_of(" \t\r") == std::string::npos) {
                continue;
            }
            data.labels.push_back(std::stoi(line));
        }
        data.images.resize(data.labels.size());

        int pixel = 0;
        while (std::getline(images_file, line)) {
            if (line.find_first_not_of(" \t\r") == std::string::npos) {
                continue;
            }
            if (pixel >= 256) {
                std::cerr << images_path << " has more than 256 rows\n";
                return std::nullopt;
            }
            std::vector<std::string> fields = split(line);
            if (fields.size() != data.labels.size()) {
                std::cerr << images_path << " row " << pixel + 1 << " has " << fields.size() << " values, but there are "
                          << data.labels.size() << " labels\n";
                return std::nullopt;
            }
            for (std::size_t image = 0; image < fields.size(); image++) {
                data.images[image][pixel] = std::stof(fields[image]);
            }
            pixel++;
        }
        if (pixel != 256) {
            std::cerr << images_path << " has " << pixel << " rows instead of 256\n";
            return std::nullopt;
        }
        return data;
    }

    // Detected digit, or none.
    int classify(const std::array<float, 256>& image, const detector& detect) {
        normaliser input{};
        input.threshold_value = threshold;
        for (int i = 0; i < 256; i++) {
            input.input_array[i] = image[i];
        }
        // compactor holds pointers into itself, so it is created fresh for every image and never copied.
        compactor compacted;
        compacted.input_matrix = input.normalise();
        compacted.compact();
        std::optional<std::string> label = detect.detect(compacted);
        if (!label || label->size() != 1 || (*label)[0] < '0' || (*label)[0] > '9') {
            return none;
        }
        return (*label)[0] - '0';
    }

    double percentage(int part, int whole) {
        return whole == 0 ? 0.0 : 100.0 * part / whole;
    }

    void report(const std::string& name, const DataSet& data) {
        detector detect;
        // confusion[actual][detected], where detected == none means nothing was detected.
        std::array<std::array<int, 11>, 10> confusion{};
        for (std::size_t i = 0; i < data.images.size(); i++) {
            int actual = data.labels[i];
            if (actual < 0 || actual > 9) {
                std::cerr << "skipping image " << i << " with label " << actual << "\n";
                continue;
            }
            confusion[actual][classify(data.images[i], detect)]++;
        }

        std::printf("\n=== %s set (%zu images) ===\n\n", name.c_str(), data.images.size());
        std::printf("digit  images  correct  wrong  none   recall  precision\n");
        int total_images = 0;
        int total_correct = 0;
        int total_wrong = 0;
        int total_none = 0;
        for (int digit = 0; digit < 10; digit++) {
            int images = 0;
            for (int detected = 0; detected <= none; detected++) {
                images += confusion[digit][detected];
            }
            int correct = confusion[digit][digit];
            int nothing = confusion[digit][none];
            int wrong = images - correct - nothing;
            int detected_as_digit = 0;
            for (int actual = 0; actual < 10; actual++) {
                detected_as_digit += confusion[actual][digit];
            }
            std::printf("%5d  %6d  %7d  %5d  %4d  %6.1f%%  %8.1f%%\n", digit, images, correct, wrong, nothing,
                        percentage(correct, images), percentage(correct, detected_as_digit));
            total_images += images;
            total_correct += correct;
            total_wrong += wrong;
            total_none += nothing;
        }
        std::printf("total  %6d  %7d  %5d  %4d  %6.1f%%\n", total_images, total_correct, total_wrong, total_none,
                    percentage(total_correct, total_images));

        std::printf("\nconfusion matrix (rows: actual, columns: detected)\n");
        std::printf("      ");
        for (int detected = 0; detected < 10; detected++) {
            std::printf("%5d", detected);
        }
        std::printf(" none\n");
        for (int actual = 0; actual < 10; actual++) {
            std::printf("%5d ", actual);
            for (int detected = 0; detected <= none; detected++) {
                std::printf("%5d", confusion[actual][detected]);
            }
            std::printf("\n");
        }
    }
}

int main(int argc, char* argv[]) {
    std::string directory = argc > 1 ? argv[1] : DATA_DIR;
    std::vector<std::string> sets;
    for (int i = 2; i < argc; i++) {
        sets.emplace_back(argv[i]);
    }
    if (sets.empty()) {
        sets = {"test", "training"};
    }

    int status = 0;
    for (const std::string& name : sets) {
        std::optional<DataSet> data = load(directory, name);
        if (!data) {
            status = 1;
            continue;
        }
        report(name, *data);
    }
    return status;
}
