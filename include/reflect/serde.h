#pragma once

#include <sstream>
#include <rttr/type>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

namespace reflect {

class Error : public std::runtime_error {
public:
    Error(const std::string &message) : std::runtime_error{message} {}
};

std::string serialize(const rttr::variant &var);

void deserialize_root(const std::string &json_str, rttr::variant &var);

template <typename T>
T deserialize(const std::string &json_str) {
    rttr::variant var{T{}};

    deserialize_root(json_str, var);

    auto &obj = var.template get_value<T>();
    return T{obj};
}

}
