#include <mixin/ability.hpp>
#include "catch.hpp"

struct Trait
{
    using args = mixin::mpl::list<int, char>;

    template<class C, class B> constexpr static auto addr() { return &C::template bar<B>; }
    template<class C, class B> using name = decltype(&C::template bar<B>);
};

struct Foo1 {
    template<class>
    void bar(int, char) {}
};

struct Foo2 {
    template<class>
    void foo(int, char) {}
};

struct Foo3 {
    template<class>
    void bar(float, double) {}
};

TEST_CASE("Properly match a method", "[ability]")
{
    REQUIRE(mixin::TraitHasMethod<Trait>::value<Foo1, int> == true);
    REQUIRE(mixin::TraitMatchArgs<Trait>::value<Foo1, int> == true);
}

TEST_CASE("Class does not have named method", "[ability]")
{
    REQUIRE(mixin::TraitHasMethod<Trait>::value<Foo2, void> == false);
}

TEST_CASE("Class does not have method with proper arguments", "[ability]")
{
    REQUIRE(mixin::TraitHasMethod<Trait>::value<Foo3, void> == true);
    REQUIRE(mixin::TraitMatchArgs<Trait>::value<Foo3, void> == false);
}
