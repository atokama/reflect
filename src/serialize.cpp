#include <reflect/serde.h>

namespace reflect {

using PrettyWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;

void serialize_array(const rttr::variant &var, PrettyWriter &writer);
void serialize_object(const rttr::variant &var, PrettyWriter &writer);

bool write_string(PrettyWriter &writer, const std::string &str) {
    return writer.String(str.c_str(),
                         static_cast<rapidjson::SizeType>(str.length()));
}

void serialize_value(const rttr::variant &var, PrettyWriter &writer) {
        const auto t = var.get_type();
        const auto type = t.is_wrapper() ? t.get_wrapped_type() : t;

        if (type == rttr::type::get<int>()) {
            const int val = var.to_int();
            writer.Int(val);
        } else if (type == rttr::type::get<std::string>()) {
            const std::string val = var.to_string();
            write_string(writer, val);
        } else if (type.is_sequential_container()){
            serialize_array(var, writer);
        } else if (type.is_class()) {
            serialize_object(var, writer);
        } else {
            throw Error{"Unsupported type: " + std::string{type.get_name()}};
        }
}

void serialize_object(const rttr::variant &var, PrettyWriter &writer) {
    writer.StartObject();

    const auto t = rttr::type::get(var);
    const auto type = t.is_wrapper() ? t.get_wrapped_type() : t;

    for (auto& prop : type.get_properties()) {

        const std::string name{prop.get_name()};
        write_string(writer, name);

        const auto var2 = prop.get_value(var);
        serialize_value(var2, writer);
    }

    writer.EndObject();
}

void serialize_array(const rttr::variant &var, PrettyWriter &writer) {
    writer.StartArray();
    const auto arr = var.create_sequential_view();
    for (const auto item : arr) {
        serialize_value(item, writer);
    }
    writer.EndArray();
}

std::string serialize(const rttr::variant &var) {
    if (!var.is_valid()) {
        throw Error{"Can't serialize invalid object"};
    }

    rapidjson::StringBuffer sb{};
    PrettyWriter writer{sb};

    const auto type = var.get_type();

    if (type.is_sequential_container()) {
        serialize_array(var, writer);
    } else if (type.is_class()) {
        serialize_object(var, writer);
    } else {
        throw Error{"Bad type of JSON root object: " + std::string{type.get_name()}};
    }

    return sb.GetString();
}

}
