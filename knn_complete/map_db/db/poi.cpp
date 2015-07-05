#include "poi.h"

POI::POI(std::string name, double lat, double lng, std::string addr)
  : name_(name), lat_(lat), lng_(lng), addr_(addr) {}

POI::~POI() {}

