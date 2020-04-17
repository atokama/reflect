#include <iostream>
#include <gtest/gtest.h>
#include <rttr/registration>

struct Car {
    std::string color;
    int num_wheels;
};

RTTR_REGISTRATION {
     rttr::registration::class_<Car>("Car")
             .constructor<>()
             .property("color", &Car::color)
             .property("num_wheels", &Car::num_wheels);
}

TEST(rttr_test, 1) {
    auto t = rttr::type::get<Car>();
    for (auto& prop : t.get_properties()) {
        std::cout << "name: " << prop.get_name() << std::endl;
    }


}

