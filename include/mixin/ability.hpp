#ifndef MIXIN_ABILITY_HPP
#define MIXIN_ABILITY_HPP

#include <mixin/function_traits.hpp>
#include <type_traits>

namespace mixin
{

// test if given objects have a method defined by T
template<class T>
struct TraitHasMethod
{
    template<class C, class B> static std::true_type  Test(typename T::template name<C, B>);
    template<class C, class B> static std::false_type Test(...);

    template<class C, class B>
    static constexpr bool value = std::is_same_v<std::true_type, decltype(Test<C, B>(nullptr))>;
};

// test if given object's method signature matches one defined by T
// if TraitHasMethod::value == false this template is ill-formed
template<class T>
struct TraitMatchArgs
{
    template<class C, class B>
    using method_t = typename T::template name<C, B>;

    template<class C, class B>
    using gc_t = method_traits<method_t<C, B>>;

    template<class C, class B>
    static constexpr bool value =
        std::is_same_v<typename gc_t<C, B>::args_list_t, typename T::args>;
};

// call a method defined by T on a object of type C
template<class T>
struct TraitCall
{
    template<class C, class B, class... Args>
    static auto call(C &c, B &b, Args... args)
    {
        return ((c).*( T::template addr<C, B>()))(b, args...);
    }
};

// call a method defined by T on a object of type C
// only if class C have a method defined by T
template<class T, class C, class B, class... Args>
auto CallIfTraitMatch(C &c, B& base, Args... args) -> typename T::ret
{
    if constexpr (TraitHasMethod<T>::template value<C, B>) {
        if constexpr (TraitMatchArgs<T>::template value<C, B>) {
            return TraitCall<T>::call(c, base, args...);
        }
    }

    return typename T::ret ();
}

} // namespace mixin

#define MIXIN_ABILITY_METHOD_NAME(NAME) \
    template<class C, class B> constexpr static auto addr() { return &C::template NAME<B>; }; \
    template<class C, class B> using name = decltype(&C::template NAME<B>);

#define MIXIN_ABILITY_METHOD(BASE, MNAME, ...) \
    struct MNAME { \
        MIXIN_ABILITY_METHOD_NAME(MNAME); \
        using signature = __VA_ARGS__; \
        using parent = BASE; \
    };

#endif // MIXIN_ABILITY_HPP
