#pragma once

#include <stdexcept>
#include <rttr/type>
#include <rapidjson/document.h>

#include <reflect/error.h>

namespace reflect {

void deserialize_root(const std::string &json_str, rttr::variant &var);

template <typename T>
T deserialize(const std::string &json_str) {
    rttr::variant var{T{}};

    deserialize_root(json_str, var);

    auto &obj = var.template get_value<T>();
    return T{obj};
}

}
