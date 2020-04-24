#pragma once

#include <sstream>
#include <rttr/type>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

#include <reflect/error.h>

namespace reflect {

using PrettyWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;

void serialize_array(const rttr::variant &var, PrettyWriter &writer);

bool write_string(PrettyWriter &writer, const std::string &str) {
    return writer.String(str.c_str(),
                         static_cast<rapidjson::SizeType>(str.length()));
}


void serialize_object(const rttr::variant &var, PrettyWriter &writer) {
    writer.StartObject();

    const auto t = rttr::type::get(var);
    const auto type = t.is_wrapper() ? t.get_wrapped_type() : t;

    for (auto& prop : type.get_properties()) {

        const std::string name{prop.get_name()};
        write_string(writer, name);

        const auto type2 = prop.get_type();
        if (type2 == rttr::type::get<int>()) {
            const int val = prop.get_value(var).to_int();
            writer.Int(val);
        } else if (type2 == rttr::type::get<std::string>()) {
            const std::string val = prop.get_value(var).to_string();
            write_string(writer, val);
        } else if (type2.is_class()) {
            const auto var2 = prop.get_value(var);
            serialize_object(var2, writer);
        } else if (type2.is_sequential_container()){
            const auto var2 = prop.get_value(var);
            serialize_array(var2, writer);
        } else {
            throw Error{"serialize_object(): Unsupported type: " + std::string{type2.get_name()}};
        }
    }

    writer.EndObject();
}

void serialize_array(const rttr::variant &var, PrettyWriter &writer) {
    writer.StartArray();
    const auto arr = var.create_sequential_view();
    for (const auto item : arr) {

        const auto t = item.get_type();
        const auto type = t.is_wrapper() ? t.get_wrapped_type() : t;

        if (type == rttr::type::get<int>()) {
            const int val = item.to_int();
            writer.Int(val);
        } else if (type == rttr::type::get<std::string>()) {
            const std::string val = item.to_string();
            write_string(writer, val);
        } else if (type.is_sequential_container()){
            serialize_array(item, writer);
        } else if (type.is_class()) {
            serialize_object(item, writer);
        } else {
            throw Error{"serialize_object(): Unsupported type: " + std::string{type.get_name()}};
        }

    }
    writer.EndArray();
}

std::string serialize(rttr::variant var) {
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
