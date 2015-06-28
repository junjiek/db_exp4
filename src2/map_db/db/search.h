#ifndef _KIT_H_
#define _KIT_H_

#include "poi.h"

#include <vector>

// Read POIs from file in json format.
bool ReadJson(char* file_name);

std::vector<std::pair<std::string, int> >
    Search(unsigned num, double xmin, double ymin, double xmax, double ymax);

#endif  // _KIT_H_
