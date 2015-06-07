#ifndef _POI_H_
#define _POI_H_

#include <string>

class POI {
public:
    POI(std::string name, double lat, double lng, std::string addr);
    ~POI();

    std::string name_;
    double lat_;
    double lng_;
    std::string addr_;
};

#endif  // _POI_H_
