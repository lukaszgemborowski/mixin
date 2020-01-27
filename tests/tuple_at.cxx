#include <mixin/tuple_at.hpp>
#include <catch.hpp>

TEST_CASE("Call at tuple index", "[tuple_at]")
{
    std::tuple<int, int, int> t {0, 1, 2};
    mixin::tuple_at(t, 0, [](auto &e) { e = 42; });

    REQUIRE(std::get<0>(t) == 42);
    REQUIRE(std::get<1>(t) == 1);
    REQUIRE(std::get<2>(t) == 2);

    mixin::tuple_at(t, 2, [](auto &e) { e = 42; });

    REQUIRE(std::get<0>(t) == 42);
    REQUIRE(std::get<1>(t) == 1);
    REQUIRE(std::get<2>(t) == 42);

    // out of range
    mixin::tuple_at(t, 3, [](auto &e) { e = 42; });

    REQUIRE(std::get<0>(t) == 42);
    REQUIRE(std::get<1>(t) == 1);
    REQUIRE(std::get<2>(t) == 42);
}
