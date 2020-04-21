#include <sstream>
#include <rttr/registration>

namespace reflect {

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

}
