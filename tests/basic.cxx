#include <mixin/mixin.hpp>
#include "catch.hpp"

struct Fooable {};
struct Barable {};

template<class T>
struct FooIf : T
{
    void foo()
    {
        for_each(
            this,
            mixin::implements<Fooable>{},
            [this](auto &impl) {
                impl.foo(*this);
            }
        );
    }
};

template<class T>
struct BarIf : T
{
    void bar()
    {
        for_each(
            this,
            mixin::implements<Barable>{},
            [this](auto &impl) {
                impl.bar(*this);
            }
        );
    }
};

struct ImplementingFooAndBar
{
    using implements = mixin::list<Fooable, Barable>;

    template<class A>
    void foo(A &) {
        fooCalled = true;
    }

    template<class A>
    void bar(A &) {
        barCalled = true;
    }

    bool fooCalled = false;
    bool barCalled = false;
};

using Composite = mixin::composite<
    mixin::impl<ImplementingFooAndBar>,
    FooIf,
    BarIf
>;

TEST_CASE("", "[mixin][composite]")
{
    Composite comp;
    auto &impl = comp.get<ImplementingFooAndBar>();

    REQUIRE(impl.fooCalled == false);
    REQUIRE(impl.barCalled == false);

    comp.foo();

    REQUIRE(impl.fooCalled == true);
    REQUIRE(impl.barCalled == false);

    comp.bar();

    REQUIRE(impl.fooCalled == true);
    REQUIRE(impl.barCalled == true);
}
