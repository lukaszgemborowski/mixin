#ifndef MIXIN_ABILITY_HPP
#define MIXIN_ABILITY_HPP

#include <mixin/function_traits.hpp>
#include <type_traits>

namespace mixin
{

template<class T>
struct TraitHasMethod
{
    template<class C> static std::true_type  Test(typename T::template name<C>);
    template<class C> static std::false_type Test(...);

    template<class C>
    static constexpr bool value = std::is_same_v<std::true_type, decltype(Test<C>(nullptr))>;
};

template<class T>
struct TraitMatchArgs
{
    template<class C>
    using method_t = typename T::template name<C>;

    template<class C>
    using gc_t = method_traits<method_t<C>>;

    template<class C>
    static constexpr bool value =
        std::is_same_v<typename gc_t<C>::args_list_t, typename T::args>;
};

template<class T>
struct TraitCall
{
    template<class C, class... Args>
    static auto call(C &c, Args... args)
    {
        ((c).*( T::template addr<C>()))(args...);
    }
};


template<class T, class C, class... Args>
void CallIfTraitMatch(C &c, Args... args)
{
    if constexpr (TraitHasMethod<T>::template value<C>) {
        if constexpr (TraitMatchArgs<T>::template value<C>) {
            TraitCall<T>::call(c, args...);
        }
    }
}

} // namespace mixin

#endif // MIXIN_ABILITY_HPP
