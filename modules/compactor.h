#ifndef INC_2MBC30_PATH_TRAVERSAL_COMPACTOR_H
#define INC_2MBC30_PATH_TRAVERSAL_COMPACTOR_H

#include <array>
#include <deque>
#include <vector>
#include "../classes/Node.h"

// A stroke between two key nodes; path holds every pixel node from start to end (inclusive).
struct Segment {
    Node* start;
    Node* end;
    std::vector<Node*> path;
};

class compactor {
public:
    std::array<std::array<int, 16>, 16> input_matrix;

    std::deque<Node> nodes;
    std::vector<Node*> key_nodes;
    std::vector<Segment> segments;

    std::array<std::array<int, 16>, 16> skeletonise();

    void compact();

private:
    void build_graph(const std::array<std::array<int, 16>, 16>& matrix);
    void contract();
};


#endif //INC_2MBC30_PATH_TRAVERSAL_COMPACTOR_H
