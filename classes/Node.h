//
// Created by Bram on 2-10-2026.
//

#ifndef INC_2MBC30_PATH_TRAVERSAL_NODE_H
#define INC_2MBC30_PATH_TRAVERSAL_NODE_H
#include <vector>


class Node {
public:
    int row;
    int column;
    // All directly adjacent nodes, diagonals included.
    std::vector<Node*> neighbours;
};


#endif //INC_2MBC30_PATH_TRAVERSAL_NODE_H
