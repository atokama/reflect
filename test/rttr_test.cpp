#include <iostream>
#include <gtest/gtest.h>
#include <rttr/registration>

using std::cout;
using std::endl;
using std::string;

struct Car { string color; int num_wheels; };
struct Traffic { Car car; int bandwidth; };

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
string serialize(rttr::variant obj) {//(const T& obj) {
    std::stringstream ss{};
    for (auto& prop : rttr::type::get(obj).get_properties()) {
        ss << prop.get_name() << ": ";

        auto type = prop.get_type();
        if (type == rttr::type::get<int>()) {
            ss << prop.get_value(obj).to_int();
        } else if (type == rttr::type::get<string>()) {
            ss << "\"" << prop.get_value(obj).to_string() << "\"";
        } else {
            ss << serialize(prop.get_value(obj));
        }
        ss << endl;
    }
    return ss.str();
}

TEST(rttr_test, 1) {
    string actual = serialize(traffic_obj);
    string expected =
            "car: color: \"yellow\"\n"
            "num_wheels: 4\n"
            "\n"
            "bandwidth: 100\n";
    ASSERT_EQ(actual, expected);
}

TEST(rttr_test, serialize_car) {
    string actual = serialize(car_obj);
    string expected =
            "color: \"yellow\"\n"
            "num_wheels: 4\n";

    ASSERT_EQ(actual, expected);
}
