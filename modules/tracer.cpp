#include "tracer.h"

#include <cstddef>
#include <map>
#include <numeric>
#include <utility>

namespace {
    // Segments of at most this many pixels between two junctions are part of one bigger junction.
    constexpr std::size_t junction_segment = 3;
    // Spurs of at most this many pixels (junction included) are skeleton noise.
    constexpr std::size_t noise_spur = 2;
    // Enough for any digit; keeps a messy skeleton from taking forever.
    constexpr std::size_t max_traces = 256;

    struct Edge {
        std::size_t from;
        std::size_t to;
        const std::vector<Node*>* path;
    };

    bool is_junction(const Node* node) {
        return node->neighbours.size() >= 3;
    }

    std::size_t find(std::vector<std::size_t>& parent, std::size_t i) {
        while (parent[i] != i) {
            parent[i] = parent[parent[i]];
            i = parent[i];
        }
        return i;
    }

    class Tracer {
    public:
        Tracer(std::vector<Edge> edges, std::size_t vertex_count)
            : edges(std::move(edges)), used(this->edges.size(), false), incident(vertex_count) {
            for (std::size_t e = 0; e < this->edges.size(); e++) {
                incident[this->edges[e].from].push_back(e);
                if (this->edges[e].to != this->edges[e].from) {
                    incident[this->edges[e].to].push_back(e);
                }
            }
        }

        void from(std::size_t vertex, std::vector<std::vector<Node*>>& output) {
            this->output = &output;
            walk(vertex, 0);
        }

    private:
        std::vector<Edge> edges;
        std::vector<bool> used;
        std::vector<std::vector<std::size_t>> incident;
        std::vector<Node*> path;
        std::vector<std::vector<Node*>>* output = nullptr;

        void walk(std::size_t vertex, std::size_t edges_used) {
            if (output->size() >= max_traces) {
                return;
            }
            if (edges_used == edges.size()) {
                output->push_back(path);
                return;
            }
            for (std::size_t e : incident[vertex]) {
                if (used[e]) {
                    continue;
                }
                const Edge& edge = edges[e];
                // A loop back to the same junction can be followed either way round.
                for (bool forwards : {true, false}) {
                    if ((forwards ? edge.from : edge.to) != vertex) {
                        continue;
                    }
                    std::size_t length = path.size();
                    append(*edge.path, forwards);
                    used[e] = true;
                    walk(forwards ? edge.to : edge.from, edges_used + 1);
                    used[e] = false;
                    path.resize(length);
                }
            }
        }

        void append(const std::vector<Node*>& segment, bool forwards) {
            for (std::size_t i = 0; i < segment.size(); i++) {
                Node* node = forwards ? segment[i] : segment[segment.size() - 1 - i];
                if (path.empty() || path.back() != node) {
                    path.push_back(node);
                }
            }
        }
    };
}

std::vector<std::vector<Node*>> traces(const compactor& compacted) {
    std::map<const Node*, std::size_t> index;
    for (Node* key : compacted.key_nodes) {
        index.emplace(key, index.size());
    }

    std::vector<std::size_t> parent(index.size());
    std::iota(parent.begin(), parent.end(), 0);
    std::vector<const Segment*> kept;
    for (const Segment& segment : compacted.segments) {
        bool tiny = segment.path.size() <= junction_segment;
        if (tiny && is_junction(segment.start) && is_junction(segment.end)) {
            parent[find(parent, index[segment.start])] = find(parent, index[segment.end]);
        }
        else if (segment.path.size() > noise_spur || (is_junction(segment.start) == is_junction(segment.end))) {
            kept.push_back(&segment);
        }
    }

    std::vector<Edge> edges;
    std::vector<int> degree(index.size(), 0);
    for (const Segment* segment : kept) {
        Edge edge{find(parent, index[segment->start]), find(parent, index[segment->end]), &segment->path};
        degree[edge.from]++;
        degree[edge.to]++;
        edges.push_back(edge);
    }
    std::vector<std::size_t> odd;
    for (std::size_t v = 0; v < degree.size(); v++) {
        if (degree[v] % 2 == 1) {
            odd.push_back(v);
        }
    }

    // With more than two odd vertices there is no single trace; tracing one segment twice can fix that when there
    // are exactly four, which covers a dead end traced out and back.
    std::vector<std::vector<Edge>> options;
    if (odd.size() <= 2) {
        options.push_back(edges);
    }
    else if (odd.size() == 4) {
        for (const Edge& edge : edges) {
            if (edge.from != edge.to && degree[edge.from] % 2 == 1 && degree[edge.to] % 2 == 1) {
                options.push_back(edges);
                options.back().push_back(edge);
            }
        }
    }

    std::vector<std::vector<Node*>> output;
    for (const std::vector<Edge>& option : options) {
        std::vector<int> option_degree(degree.size(), 0);
        for (const Edge& edge : option) {
            option_degree[edge.from]++;
            option_degree[edge.to]++;
        }
        std::vector<std::size_t> starts;
        for (std::size_t v = 0; v < option_degree.size(); v++) {
            if (option_degree[v] % 2 == 1) {
                starts.push_back(v);
            }
        }
        if (starts.empty()) {
            for (std::size_t v = 0; v < option_degree.size(); v++) {
                if (option_degree[v] > 0) {
                    starts.push_back(v);
                }
            }
        }
        Tracer tracer(option, degree.size());
        for (std::size_t start : starts) {
            tracer.from(start, output);
        }
    }
    return output;
}
