#ifndef MIXIN_MIXIN_HPP
#define MIXIN_MIXIN_HPP

#include <mixin/tuple_for_each.hpp>
#include <mixin/tuple_select.hpp>
#include <mixin/ability.hpp>
#include <mixin/list.hpp>
#include <tuple>
#include <type_traits>

namespace mixin
{
template<class... Impl>
struct impl
{
    using tuple_t = std::tuple<Impl...>;
};

template<template<typename> typename...>
struct iface {};

template<class Sign>
struct top_hierarchy {
    using sign_t = Sign;
};

template<class Impl, class Iface>
struct composite;

template<class Impl, class Iface>
struct composite_signature
{
    using composite_t = composite<Impl, Iface>;
    using impl_tuple_t = typename Impl::tuple_t;
};

template<class Sign,
         template<typename> typename Head,
         template<typename> typename... Tail>
struct interface_base : public Head<interface_base<Sign, Tail...>>
{
    using sign_t = Sign;
};

template<class Sign, template<typename> typename Last>
struct interface_base<Sign, Last>
    : public Last<top_hierarchy<Sign>>
{
    using sign_t = Sign;
};

template<
    class... Impl,
    template<typename> typename... Iface>
struct composite<impl<Impl...>, iface<Iface...>>
    : public interface_base<
        composite_signature<impl<Impl...>, iface<Iface...>>, Iface...>
{
    composite() = default;
    composite(Impl&&... init)
        : impl{std::forward<Impl>(init)...}
    {}

    template<class T>
    auto & get()
    {
        return std::get<T>(impl);
    }

    typename impl<Impl...>::tuple_t impl;
};

template<
    template<typename> typename... Iface,
    typename... Impl>
auto make_composite(Impl&&... init)
{
    return composite<
        impl<Impl...>,
        iface<Iface...>
    >{std::forward<Impl>(init)...};
}

// accessors
template<class T>
struct implements
{
    template<class Q>
    using invoke = list_has<typename Q::implements, T>;
};

struct all
{
    template<class>
    using invoke = std::true_type;
};

template<class Base, class Selector, class F>
void for_each(Base *self, Selector sel, F fun)
{
    using sign_t = typename Base::sign_t;
    using comp_t = typename sign_t::composite_t;
    auto &impl = static_cast<comp_t *>(self)->impl;

    tuple_for_each(
        impl,
        [fun](auto &e) {
            using impl_t = std::decay_t<decltype(e)>;

            if constexpr (Selector::template invoke<impl_t>::value) {
                fun(e);
            }
        }
    );
}

template<class Base, class Selector, class F>
auto execute(Base *self, Selector sel, F fun)
{
    using sign_t = typename Base::sign_t;
    using comp_t = typename sign_t::composite_t;
    auto &impl = static_cast<comp_t *>(self)->impl;

    auto result = tuple_select_ref<Selector>(impl);
    static_assert(
        std::tuple_size_v<decltype(result)> == 1,
        "can't execute more than 1 element"
    );

    return fun(std::get<0>(result));
}

namespace detail
{
template<class BaseTrait, class FirstArg>
struct Trait : BaseTrait {
    using base_args = typename function_traits<typename BaseTrait::signature>::args_list_t;

    using args = typename list_push_front<base_args, FirstArg>::type_t;
    using ret = typename function_traits<typename BaseTrait::signature>::return_t;
};
} // namespace detail

template<class Ability, class Base, class... Args>
void for_each_ability(Base *self, Args&&... args)
{
    using Trait = detail::Trait<Ability, Base &>;

    for_each(
        self,
        all {},
        [self, &args...](auto &e) {
            CallIfTraitMatch<Trait>(e, *self, std::forward<Args>(args)...);
        }
    );
}

template<class Ability, class Base, class... Args>
decltype(auto) execute_ability(Base *self, Args&&... args)
{
    using sign_t = typename Base::sign_t;
    using comp_t = typename sign_t::composite_t;
    auto &impl = static_cast<comp_t *>(self)->impl;

    auto result = tuple_select_ref<implements<typename Ability::parent>>(impl);

    static_assert(
        std::tuple_size_v<decltype(result)> == 1,
        "can't execute more than 1 element"
    );

    using Trait = detail::Trait<Ability, Base &>;
    return CallIfTraitMatch<Trait>(
        std::get<0>(result),
        *self,
        std::forward<Args>(args)...
    );
}


} // namespace mixin

#endif // MIXIN_MIXIN_HPP
