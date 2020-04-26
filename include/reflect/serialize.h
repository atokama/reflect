#pragma once

#include <sstream>
#include <rttr/type>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

#include <reflect/error.h>

namespace reflect {

std::string serialize(const rttr::variant &var);

}
