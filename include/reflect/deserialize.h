#include <rttr/registration>

namespace reflect {

template <typename T>
T deserialize_car(const std::string &json) {
    auto type = rttr::type::get<T>();
    auto var = type.create();
    type.get_property("color").set_value(var, std::string{"red"});
    type.get_property("num_wheels").set_value(var, 2);
    auto &obj = var.template get_value<std::shared_ptr<T>>();
    return T{*obj};
}

}
