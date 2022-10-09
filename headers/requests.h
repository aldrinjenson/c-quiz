#ifndef REQUESTS

#include "Poco/URI.h"
#include <nlohmann/json.hpp>

nlohmann::json getData(Poco::URI uri);

#endif