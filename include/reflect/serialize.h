#pragma once

#include <sstream>
#include <rttr/type>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

#include <reflect/error.h>

namespace reflect {

using PrettyWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;

//template <typename T>
std::string serialize(rttr::variant obj, unsigned nest = 0) {//(const T& obj) {
    static const std::string obj_indent(4, ' ');
    const std::string indent(nest * 4, ' ');
    std::stringstream ss{};
    ss << "{\n";
    for (auto& prop : rttr::type::get(obj).get_properties()) {
        ss << indent << obj_indent << prop.get_name() << ": ";

        auto type = prop.get_type();
        if (type == rttr::type::get<int>()) {
            ss << prop.get_value(obj).to_int();
        } else if (type == rttr::type::get<std::string>()) {
            ss << "\"" << prop.get_value(obj).to_string() << "\"";
        } else {
            ss << serialize(prop.get_value(obj), nest + 1);
        }
        ss << "\n";
    }
    ss << indent << "}";
    return ss.str();
}

bool write_string(PrettyWriter &writer, const std::string &str) {
    return writer.String(str.c_str(),
                         static_cast<rapidjson::SizeType>(str.length()));
}

void serialize_object(const rttr::variant &var, PrettyWriter &writer) {
    writer.StartObject();

    for (auto& prop : rttr::type::get(var).get_properties()) {
        const std::string name{prop.get_name()};
        write_string(writer, name);

        auto type = prop.get_type();
        if (type == rttr::type::get<int>()) {
            const int val = prop.get_value(var).to_int();
            writer.Int(val);
        } else if (type == rttr::type::get<std::string>()) {
            const std::string val = prop.get_value(var).to_string();
            write_string(writer, val);
        } else {
        }
    }

    writer.EndObject();

}

std::string serialize_rapidjson(rttr::variant var) {
    if (!var.is_valid()) {
        throw Error{"Can't serialize invalid object"};
    }

    rapidjson::StringBuffer sb{};
    PrettyWriter writer{sb};

    const auto type = var.get_type();

    if (type.is_class()) {
        serialize_object(var, writer);
    } else if (type.is_sequential_container()) {

    } else {
        throw Error{std::string{"Bad type of JSON root object: "} + type.get_name()};
    }

    return sb.GetString();
}

}
