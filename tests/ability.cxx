#include <mixin/ability.hpp>
#include "catch.hpp"

struct Trait
{
    using args = mixin::list<int, char>;

    template<class C> constexpr static auto addr() { return &C::bar; }
    template<class C> using name = decltype(&C::bar);
};

TEST_CASE("Properly match a method", "[ability]")
{
    struct Foo {
        void bar(int, char) {}
    };

    REQUIRE(mixin::TraitHasMethod<Trait>::value<Foo> == true);
    REQUIRE(mixin::TraitMatchArgs<Trait>::value<Foo> == true);
}

TEST_CASE("Class does not have named method", "[ability]")
{
    struct Foo {
        void foo(int, char) {}
    };

    REQUIRE(mixin::TraitHasMethod<Trait>::value<Foo> == false);
}

TEST_CASE("Class does not have method with proper arguments", "[ability]")
{
    struct Foo {
        void bar(float, double) {}
    };

    REQUIRE(mixin::TraitHasMethod<Trait>::value<Foo> == true);
    REQUIRE(mixin::TraitMatchArgs<Trait>::value<Foo> == false);
}
