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
    auto foo = i.create<void>();

    REQUIRE(std::is_same<Foo, decltype(foo)>::value);
}

TEST_CASE("Create object with single int argument", "[impl_init]")
{
    auto i = mixin::impl<Foo2>(42);
    auto foo = i.create<void>();

    REQUIRE(std::is_same<Foo2, decltype(foo)>::value);
    REQUIRE(foo.x == 42);
}

template<class T>
struct Foo3 {
    Foo3(T v)
        : value {v}
    {}

    T value;
};

TEST_CASE("Create template object", "[impl_init]")
{
    auto i = mixin::impl<Foo3>(42);
    auto foo = i.create<int>();

    REQUIRE(foo.value == 42);
}

struct ArgumentType
{
    int copy_counter = 0;

    ArgumentType() = default;
    ArgumentType(ArgumentType &&at)
        : copy_counter {at.copy_counter}
    {}

    ArgumentType(const ArgumentType &at)
        : copy_counter {at.copy_counter + 1}
    {}

    ArgumentType& operator=(const ArgumentType &at)
    {
        copy_counter = at.copy_counter + 1;
    }
};

struct Foo4
{
    Foo4(ArgumentType at)
        : at {at}
    {
    }

    ArgumentType at;
};

TEST_CASE("Copyable argument", "[impl_init]")
{
    // create ArgumentType with a copy and std::forward it to temporary storage
    auto i = mixin::impl<Foo4>(ArgumentType());

    // copy temporarly stored argument to Foo4 constructor
    auto foo = i.create<void>();

    REQUIRE(foo.at.copy_counter == 2);
}

TEST_CASE("Create with a reference", "[impl_init]")
{
    ArgumentType at;
    auto i = mixin::impl<Foo4>(at);
    auto foo = i.create<void>();

    // TODO: FIXME: this should be 1, mixin::impl_init
    // should create T& storage for the argument
    REQUIRE(foo.at.copy_counter == 2);
}