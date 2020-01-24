#include <mixin/mixin.hpp>
#include "catch.hpp"

struct Foo {};
struct Foo2 {
    Foo2(int x) : x {x} {}

    int x;
};

TEST_CASE("Create object with no arguments in constructor", "[impl_init]")
{
    auto i = mixin::impl<Foo>();
    auto foo = i.create();

    REQUIRE(std::is_same<Foo, decltype(foo)>::value);
}

TEST_CASE("Create object with single int argument", "[impl_init]")
{
    auto i = mixin::impl<Foo2>(42);
    auto foo = i.create();

    REQUIRE(std::is_same<Foo2, decltype(foo)>::value);
    REQUIRE(foo.x == 42);
}

template<class T>
struct Foo3
{
    Foo3(T arg) : arg {arg} {}
    T arg;
};

TEST_CASE("Can wrap class template", "[impl_init]")
{
    auto i = mixin::impl<Foo3>(2.3f);
}
