#ifndef MIXIN_ABILITY_HPP
#define MIXIN_ABILITY_HPP

#include <mixin/function_traits.hpp>
#include <type_traits>

namespace mixin
{

template<class T>
struct TraitHasMethod
{
    template<class C, class B> static std::true_type  Test(typename T::template name<C, B>);
    template<class C, class B> static std::false_type Test(...);

    template<class C, class B>
    static constexpr bool value = std::is_same_v<std::true_type, decltype(Test<C, B>(nullptr))>;
};

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

template<class T>
struct TraitCall
{
    template<class C, class B, class... Args>
    static auto call(C &c, B &b, Args... args)
    {
        ((c).*( T::template addr<C, B>()))(b, args...);
    }
};


template<class T, class C, class B, class... Args>
void CallIfTraitMatch(C &c, B& base, Args... args)
{
    if constexpr (TraitHasMethod<T>::template value<C, B>) {
        if constexpr (TraitMatchArgs<T>::template value<C, B>) {
            TraitCall<T>::call(c, base, args...);
        }
    }
}

} // namespace mixin

#endif // MIXIN_ABILITY_HPP
