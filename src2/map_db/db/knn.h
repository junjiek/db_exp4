#ifndef _KNN_H_
#define _KNN_H_

#include <queue>

#include "poi.h"

// Naive algorithm.
// Assume poi_list is sorted by lat.
std::vector<POI> Naive(double lat, double lng,
                       const std::vector<POI>& poi_list,
                       unsigned neighbor_num);

#endif  // _KNN_H_
