#ifndef INC_2MBC30_PATH_TRAVERSAL_TRACER_H
#define INC_2MBC30_PATH_TRAVERSAL_TRACER_H

#include <vector>
#include "compactor.h"

// Every way of tracing the whole compacted figure in one go, as pixel paths. Each segment is traced once, except
// that a dead end may be traced out and back (the stem of an open 4, the middle of a 3) when the figure cannot be
// drawn without lifting the pen otherwise. Junctions that the skeleton splits into a few pixels joined by tiny
// segments are treated as one junction, and one-pixel spurs are ignored.
std::vector<std::vector<Node*>> traces(const compactor& compacted);


#endif //INC_2MBC30_PATH_TRAVERSAL_TRACER_H
