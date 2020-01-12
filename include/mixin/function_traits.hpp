#ifndef MIXIN_FUNCTION_TRAITS_HPP
#define MIXIN_FUNCTION_TRAITS_HPP

#include <mixin/list.hpp>

namespace mixin
{

template<class>
struct method_traits;

template<class R, class C, class... Args>
struct method_traits<R (C::*)(Args...)>
{
    using return_t = R;
    using class_t = C;
    using args_list_t = list<Args...>;
};

template<class>
struct function_traits;

template<class R, class... Args>
struct function_traits<R (Args...)>
{
    using return_t = R;
    using args_list_t = list<Args...>;
};

} // namespace mixin

#endif // MIXIN_FUNCTION_TRAITS_HPP
