#include <mixin/mixin.hpp>
#include "catch.hpp"

struct Fooable {};
struct Barable {};
struct Mathable {};

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

    int do_math(int a, int b)
    {
        return execute(
            this,
            mixin::implements<Mathable>{},
            [this, a, b](auto &impl) {
                return impl.do_math(*this, a, b);
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

struct DoSomeMath
{
    using implements = mixin::list<Mathable>;

    template<class A>
    int do_math(A&, int a, int b)
    {
        return a + b;
    }
};

using Composite = mixin::composite<
    mixin::impl<ImplementingFooAndBar, DoSomeMath>,
    mixin::iface<FooIf, BarIf>
>;

TEST_CASE("Can call interface methods", "[mixin][composite]")
{
    auto comp = mixin::make_composite<FooIf, BarIf>(
        ImplementingFooAndBar{}, DoSomeMath{}
    );

    auto &impl = comp.get<ImplementingFooAndBar>();

    REQUIRE(impl.fooCalled == false);
    REQUIRE(impl.barCalled == false);

    comp.foo();

    REQUIRE(impl.fooCalled == true);
    REQUIRE(impl.barCalled == false);

    comp.bar();

    REQUIRE(impl.fooCalled == true);
    REQUIRE(impl.barCalled == true);

    REQUIRE(comp.do_math(40, 2) == 42);
}
