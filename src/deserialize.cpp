#include <reflect/serde.h>

namespace reflect {

void deserialize_object(const rapidjson::Value &json, rttr::variant &var);
void deserialize_array(const rapidjson::Value &json, rttr::variant &var);

void deserialize_value(const rapidjson::Value &json, rttr::variant &var) {
    const auto t = var.get_type();
    const auto type = t.is_wrapper() ? t.get_wrapped_type() : t;

    if (type == rttr::type::get<int>()) {
        var = json.GetInt();
    } else if (type == rttr::type::get<std::string>()) {
        var = std::string{json.GetString()};
    } else if (type.is_sequential_container()) {
        deserialize_array(json, var);
    } else if (type.is_class()){
        deserialize_object(json, var);
    }
}

void deserialize_object(const rapidjson::Value &json, rttr::variant &var) {
    if (!json.IsObject()) {
        throw Error{""};
    }

    const auto t = var.get_type();
    const auto type = t.is_wrapper() ? t.get_wrapped_type() : t;

    for (auto& prop : type.get_properties()) {
        const auto name = prop.get_name();
        if (!json.HasMember(name.data())) {
            continue;
        }

        const rapidjson::Value &json2 = json[name.data()];
        auto var2 = prop.get_value(var);
        deserialize_value(json2, var2);
        prop.set_value(var, var2);
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

        auto var2 = arr.get_value(i);
        deserialize_value(json2, var2);
        arr.set_value(i, var2);
    }
}

void deserialize_root(const std::string &json_str, rttr::variant &var) {
    rapidjson::Document json{};

    if (json.Parse(json_str.data()).HasParseError()) {
        throw Error{"JSON parse error at pos " +
                    std::to_string(json.GetErrorOffset())};
    }

    const auto type = var.get_type();

    if (type.is_sequential_container()) {
        deserialize_array(json, var);
    } else if (type.is_class()) {
        deserialize_object(json, var);
    } else {
        throw Error{"Bad type of JSON root object: " + std::string{type.get_name()}};
    }
}

}
