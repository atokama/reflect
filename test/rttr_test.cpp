#include <reflect/serialize.h>
#include <reflect/deserialize.h>
#include <gtest/gtest.h>
#include <rttr/registration>

using std::cout;
using std::endl;
using std::string;

namespace reflect {

struct Car { string color; int num_wheels; };
struct Traffic { Car car; int bandwidth; };

bool operator==(const Car &l, const Car &r) { return l.color == r.color && l.num_wheels == r.num_wheels; }
bool operator==(const Traffic &l, const Traffic &r) { return l.car == r.car && l.bandwidth == r.bandwidth; }

RTTR_REGISTRATION {
     rttr::registration::class_<Car>("Car")
             .constructor<>()(rttr::policy::ctor::as_object)
             .property("color", &Car::color)
             .property("num_wheels", &Car::num_wheels);
     rttr::registration::class_<Traffic>("Traffic")
             .constructor<>()(rttr::policy::ctor::as_object)
             .property("car", &Traffic::car)
             .property("bandwidth", &Traffic::bandwidth);
}

Car car_obj{"yellow", 4};
Traffic traffic_obj{car_obj, 100};

TEST(rttr_test, deserialize_flat_obj) {
    string car_json =
            "{\n"
            "    \"color\": \"yellow\",\n"
            "    \"num_wheels\": 4\n"
            "}";
    Car actual = deserialize<Car>(car_json);
    ASSERT_EQ(actual, car_obj);
}

TEST(rttr_test, deserialize_compound_obj) {
    string traffic_json{
        "{\n"
        "  \"car\": {\n"
        "    \"color\": \"yellow\",\n"
        "    \"num_wheels\": 4\n"
        "  }, \n"
        "  \"bandwidth\": 100\n"
        "}"};
    Traffic actual = deserialize<Traffic>(traffic_json);
    Traffic expected{Car{"yellow", 4}, 100};
    ASSERT_EQ(actual, expected);
}

TEST(rttr_test, serialize_flat_obj) {
    string actual = serialize_rapidjson(car_obj);
    string expected =
            "{\n"
            "    \"color\": \"yellow\",\n"
            "    \"num_wheels\": 4\n"
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

}
