#pragma once

#include <stdexcept>
#include <rttr/type>
#include <rapidjson/document.h>

#include <reflect/error.h>

namespace reflect {

void deserialize_object(const rapidjson::Value &json, rttr::variant &var);
void deserialize_array(const rapidjson::Value &json, rttr::variant &var);

void deserialize_object(const rapidjson::Value &json, rttr::variant &var) {
    if (!json.IsObject()) {
        throw Error{""};
    }

    for (auto& prop : var.get_type().get_properties()) {
        const auto name = prop.get_name();
        if (!json.HasMember(name.data())) {
            continue;
        }

        const rapidjson::Value &json2 = json[name.data()];
        const auto type2 = prop.get_type();
        if (type2 == rttr::type::get<int>()) {
            prop.set_value(var, json2.GetInt());
        } else if (type2 == rttr::type::get<std::string>()) {
            prop.set_value(var, std::string{json2.GetString()});
        } else if (type2.is_sequential_container()) {
            auto var2 = prop.get_value(var);
            deserialize_array(json2, var2);
            prop.set_value(var, var2);
        } else if (type2.is_class()){
            auto var2 = prop.get_value(var);
            deserialize_object(json2, var2);
            prop.set_value(var, var2);
        }
    }
}

void deserialize_array(const rapidjson::Value &json, rttr::variant &var) {
    if (!json.IsArray()) {
        throw Error{""};
    }

    auto arr = var.create_sequential_view();
    const size_t size = json.Size();
    arr.set_size(size);

    for (size_t i = 0; i != size; ++i) {

        const rapidjson::Value &json2 = json[i];
        const auto type2 = arr.get_rank_type(1);

        if (type2 == rttr::type::get<int>()) {
            arr.set_value(i, json2.GetInt());
        } else if (type2 == rttr::type::get<std::string>()) {
            arr.set_value(i, std::string{json2.GetString()});
        } else if (type2.is_sequential_container()) {
            rttr::variant var2 = arr.get_value(i);
            deserialize_array(json2, var2);
            arr.set_value(i, var2);
        } else if (type2.is_class()){
            rttr::variant var2 = arr.get_value(i);
            deserialize_object(json2, var2);
            arr.set_value(i, var2);
        }
    }
}

template <typename T>
T deserialize(const std::string &json_str) {
    rapidjson::Document json{};

    if (json.Parse(json_str.data()).HasParseError()) {
        throw Error{"JSON parse error at pos " +
                    std::to_string(json.GetErrorOffset())};
    }

    auto type = rttr::type::get<T>();
    rttr::variant var{T{}};

    if (type.is_sequential_container()) {
        deserialize_array(json, var);
    } else if (type.is_class()) {
        deserialize_object(json, var);
    } else {
    }

    auto &obj = var.template get_value<T>();
    return T{obj};
}

}
