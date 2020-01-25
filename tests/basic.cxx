#include <mixin/mixin.hpp>
#include "catch.hpp"

struct Fooable {};
struct Barable {};
struct Mathable {
    MIXIN_ABILITY_METHOD(Mathable, do_math, int (int, int));
};

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
        return mixin::execute_ability<Mathable::do_math>(this, a, b);
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

TEST_CASE("Can call interface methods", "[mixin][composite]")
{
    auto comp = mixin::make_composite<FooIf, BarIf>(
        mixin::impl<ImplementingFooAndBar>(),
        mixin::impl<DoSomeMath>()
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

struct ImplA
{
    using implements = mixin::list<>;
};

template<class Info>
struct TemplateImpl
{
    using implements = mixin::list<>;

    static constexpr auto impl_a_count = mixin::info_count_impl<Info, ImplA>();
};

TEST_CASE("Inspect mixin from within impl class", "[mixin][composite]")
{
    auto comp = mixin::make_composite<FooIf>(
        mixin::impl<TemplateImpl>()
    );
}
