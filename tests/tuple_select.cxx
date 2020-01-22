#include <mixin/tuple_select.hpp>
#include "catch.hpp"

template<class T>
struct select_type
{
    template<class Q>
    using invoke = std::is_same<Q, T>;
};

TEST_CASE("Reference to ints", "[tuple_select]")
{
    std::tuple<int, float, double, int> t1 {1, 2.f, 3.0, 4};
    auto t2 = mixin::tuple_select_ref<select_type<int>>(t1);

    using t2_t = decltype(t2);
    constexpr auto t2_size =  std::tuple_size_v<t2_t>;

    REQUIRE(t2_size == 2);
    REQUIRE(std::is_same_v<t2_t, std::tuple<int &, int &>>);

    REQUIRE(std::get<0>(t2) == 1);
    REQUIRE(std::get<1>(t2) == 4);

    std::get<0>(t1) = 10;
    std::get<3>(t1) = 20;

    REQUIRE(std::get<0>(t2) == 10);
    REQUIRE(std::get<1>(t2) == 20);
}

TEST_CASE("Count types in tuple", "[tuple_select]")
{
    std::tuple<int, float, double, int> t1 {1, 2.f, 3.0, 4};

    REQUIRE(mixin::tuple_select_size<select_type<int>>(t1) == 2);
}
