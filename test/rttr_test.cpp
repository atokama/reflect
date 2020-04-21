#include <iostream>
#include <gtest/gtest.h>
#include <rttr/registration>

using std::cout;
using std::endl;
using std::string;

struct Car { string color; int num_wheels; };
struct Traffic { Car car; int bandwidth; };

bool operator==(const Car &l, const Car &r) { return l.color == r.color && l.num_wheels == r.num_wheels; }
bool operator==(const Traffic &l, const Traffic &r) { return l.car == r.car && l.bandwidth == r.bandwidth; }

RTTR_REGISTRATION {
     rttr::registration::class_<Car>("Car")
             .constructor<>()
             .property("color", &Car::color)
             .property("num_wheels", &Car::num_wheels);
     rttr::registration::class_<Traffic>("Traffic")
             .constructor<>()
             .property("car", &Traffic::car)
             .property("bandwidth", &Traffic::bandwidth);
}

Car car_obj{"yellow", 4};
Traffic traffic_obj{car_obj, 100};

//template <typename T>
string serialize(rttr::variant obj, unsigned nest = 0) {//(const T& obj) {
    static const string obj_indent(4, ' ');
    const string indent(nest * 4, ' ');
    std::stringstream ss{};
    ss << "{\n";
    for (auto& prop : rttr::type::get(obj).get_properties()) {
        ss << indent << obj_indent << prop.get_name() << ": ";

        auto type = prop.get_type();
        if (type == rttr::type::get<int>()) {
            ss << prop.get_value(obj).to_int();
        } else if (type == rttr::type::get<string>()) {
            ss << "\"" << prop.get_value(obj).to_string() << "\"";
        } else {
            ss << serialize(prop.get_value(obj), nest + 1);
        }
        ss << "\n";
    }
    ss << indent << "}";
    return ss.str();
}

template <typename T>
T deserialize_car(const string &json) {
    auto type = rttr::type::get<T>();
    auto var = type.create();
    type.get_property("color").set_value(var, string{"red"});
    type.get_property("num_wheels").set_value(var, 2);
    auto &obj = var.template get_value<std::shared_ptr<T>>();
    return T{*obj};
}

TEST(rttr_test, deserialize_car) {
    Car actual = deserialize_car<Car>("");
    Car expected{"red", 2};
    ASSERT_EQ(actual, expected);
}

TEST(rttr_test, serialize_flat_obj) {
    string actual = serialize(car_obj);
    string expected =
            "{\n"
            "    color: \"yellow\"\n"
            "    num_wheels: 4\n"
            "}";

    ASSERT_EQ(actual, expected);
}

TEST(rttr_test, serialize_compound_obj) {
    string actual = serialize(traffic_obj);
    string expected =
            "{\n"
            "    car: {\n"
            "        color: \"yellow\"\n"
            "        num_wheels: 4\n"
            "    }\n"
            "    bandwidth: 100\n"
            "}";
    ASSERT_EQ(actual, expected);
}
