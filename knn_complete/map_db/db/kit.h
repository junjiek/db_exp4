#ifndef _KIT_H_
#define _KIT_H_

#include "poi.h"

#include <vector>

// Read POIs from file in json format.
bool ReadJson(char* file_name);

std::vector<POI> Search(double lat, double lng, const std::string& input,
                        unsigned num);

#endif  // _KIT_H_
