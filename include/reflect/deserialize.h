#include <stdexcept>
#include <rttr/type>
#include <rapidjson/document.h>

namespace reflect {

class Error : public std::runtime_error {
public:
    Error(const std::string &message) : std::runtime_error{message} {}
};

rttr::variant desererialize_object(const rapidjson::Value& json, rttr::type type) {
    auto var = type.create();
    for (auto& prop : type.get_properties()) {
        const auto name = prop.get_name();
        if (!json.HasMember(name.data())) {
            continue;
        }

        auto &val = json[name.data()];
        const auto type = prop.get_type();
        if (type == rttr::type::get<int>()) {
            prop.set_value(var, val.GetInt());
        } else if (type == rttr::type::get<std::string>()) {
            prop.set_value(var, std::string{val.GetString()});
        } else {
//            auto type2 = prop.get_type();
//            auto var2 = desererialize_object(val, type2);
//            prop.set_value(var, var2);
        }
    }
    return var;
}

template <typename T>
T deserialize(const std::string &json) {
    rapidjson::Document doc{};
    if (doc.Parse(json.data()).HasParseError()) {
        throw Error{"JSON parse error at pos " + std::to_string(
                        doc.GetErrorOffset())};
    }

    auto type = rttr::type::get<T>();
    rttr::variant var{};

    if (type.is_class()) {
        var = desererialize_object(doc, type);
    } else if (type.is_sequential_container()) {

    } else {
    }

    auto &obj = var.template get_value<std::shared_ptr<T>>();
    return T{*obj};
}

}
