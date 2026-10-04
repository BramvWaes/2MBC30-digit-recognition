#include "compactor.h"

#include <set>
#include <utility>

namespace {
    int pixel(const std::array<std::array<int, 16>, 16>& matrix, int row, int column) {
        if (row < 0 || row >= 16 || column < 0 || column >= 16) {
            return 0;
        }
        return matrix[row][column];
    }

    std::pair<Node*, Node*> edge_key(Node* a, Node* b) {
        return a < b ? std::make_pair(a, b) : std::make_pair(b, a);
    }
}

std::array<std::array<int, 16>, 16> compactor::skeletonise() {
    std::array<std::array<int, 16>, 16> matrix = input_matrix;
    bool changed = true;
    while (changed) {
        changed = false;
        for (int pass = 0; pass < 2; pass++) {
            std::vector<std::pair<int, int>> to_remove;
            for (int i = 0; i < 16; i++) {
                for (int j = 0; j < 16; j++) {
                    if (matrix[i][j] == 0) {
                        continue;
                    }
                    int p[8] = {
                        pixel(matrix, i - 1, j), pixel(matrix, i - 1, j + 1),
                        pixel(matrix, i, j + 1), pixel(matrix, i + 1, j + 1),
                        pixel(matrix, i + 1, j), pixel(matrix, i + 1, j - 1),
                        pixel(matrix, i, j - 1), pixel(matrix, i - 1, j - 1)
                    };
                    int filled = 0;
                    int transitions = 0;
                    for (int k = 0; k < 8; k++) {
                        filled += p[k];
                        if (p[k] == 0 && p[(k + 1) % 8] == 1) {
                            transitions++;
                        }
                    }
                    if (filled < 2 || filled > 6 || transitions != 1) {
                        continue;
                    }
                    bool remove;
                    if (pass == 0) {
                        remove = p[0] * p[2] * p[4] == 0 && p[2] * p[4] * p[6] == 0;
                    }
                    else {
                        remove = p[0] * p[2] * p[6] == 0 && p[0] * p[4] * p[6] == 0;
                    }
                    if (remove) {
                        to_remove.emplace_back(i, j);
                    }
                }
            }
            for (auto [i, j] : to_remove) {
                matrix[i][j] = 0;
            }
            if (!to_remove.empty()) {
                changed = true;
            }
        }
    }
    return matrix;
}

void compactor::compact() {
    build_graph(skeletonise());
    contract();
}

void compactor::build_graph(const std::array<std::array<int, 16>, 16>& matrix) {
    nodes.clear();
    std::array<std::array<Node*, 16>, 16> grid{};
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if (matrix[i][j] == 1) {
                nodes.push_back(Node{i, j, {}});
                grid[i][j] = &nodes.back();
            }
        }
    }
    for (Node& node : nodes) {
        for (int di = -1; di <= 1; di++) {
            for (int dj = -1; dj <= 1; dj++) {
                if ((di == 0 && dj == 0) || pixel(matrix, node.row + di, node.column + dj) == 0) {
                    continue;
                }
                if (di != 0 && dj != 0 &&
                    (pixel(matrix, node.row + di, node.column) == 1 || pixel(matrix, node.row, node.column + dj) == 1)) {
                    continue;
                }
                node.neighbours.push_back(grid[node.row + di][node.column + dj]);
            }
        }
    }
}

void compactor::contract() {
    key_nodes.clear();
    segments.clear();
    std::set<std::pair<Node*, Node*>> visited;

    auto walk = [&](Node* start, Node* first) {
        Segment segment{start, nullptr, {start}};
        Node* previous = start;
        Node* current = first;
        visited.insert(edge_key(previous, current));
        segment.path.push_back(current);
        while (current != start && current->neighbours.size() == 2) {
            Node* next = current->neighbours[0] == previous ? current->neighbours[1] : current->neighbours[0];
            if (visited.contains(edge_key(current, next))) {
                break;
            }
            visited.insert(edge_key(current, next));
            previous = current;
            current = next;
            segment.path.push_back(current);
        }
        segment.end = current;
        segments.push_back(segment);
    };

    for (Node& node : nodes) {
        if (node.neighbours.size() != 2) {
            key_nodes.push_back(&node);
        }
    }
    for (Node* key : key_nodes) {
        for (Node* neighbour : key->neighbours) {
            if (!visited.contains(edge_key(key, neighbour))) {
                walk(key, neighbour);
            }
        }
    }
    // Whatever is left consists of closed loops without endpoints or junctions (e.g. a '0').
    for (Node& node : nodes) {
        if (node.neighbours.size() == 2 && !visited.contains(edge_key(&node, node.neighbours[0]))) {
            key_nodes.push_back(&node);
            walk(&node, node.neighbours[0]);
        }
    }
}
