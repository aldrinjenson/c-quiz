#include "Poco/URI.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace Poco;

json getData(URI uri);