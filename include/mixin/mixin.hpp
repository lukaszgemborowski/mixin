#ifndef MIXIN_MIXIN_HPP
#define MIXIN_MIXIN_HPP

#include <mixin/tuple_for_each.hpp>
#include <mixin/tuple_select.hpp>
#include <mixin/is_template.hpp>
#include <mixin/ability.hpp>
#include <mixin/list.hpp>
#include <tuple>
#include <type_traits>

namespace mixin
{

namespace ability
{

struct constructible
{
    MIXIN_ABILITY_METHOD(constructible, ctor, void ());
};

struct destructible
{
    MIXIN_ABILITY_METHOD(destructible, dtor, void ());
};

} // ability

template<class... Impl>
struct ImplList
{
    using tuple_t = std::tuple<Impl...>;
    using list_t = list<Impl...>;
};

template<template<typename> typename... T>
struct iface {
};

template<class If, class Im>
struct temp_info
{
    using implementation = Im;
    using interface = If;
};

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
struct composite<ImplList<Impl...>, iface<Iface...>>
    : public interface_base<
        composite_signature<ImplList<Impl...>, iface<Iface...>>, Iface...>
{
    composite()
        : impl {}
    {
        construct();
    }

    composite(Impl&&... init)
        : impl{std::forward<Impl>(init)...}
    {
        construct();
    }

    ~composite()
    {
        destruct();
    }

    template<class T>
    auto & get()
    {
        return std::get<T>(impl);
    }

    typename ImplList<Impl...>::tuple_t impl;

private:
    void construct();
    void destruct();
};

template<class T, class... Args>
struct impl_init
{
    template<class>
    using type = T;

    impl_init(Args&&... args)
        : ctorArguments {args...}
    {
    }

    template<class>
    auto create()
    {
        return std::make_from_tuple<T>(ctorArguments);
    }

    // FIXME: TODO: needs to properly deduce the types here to avoid copying
    std::tuple<Args...> ctorArguments;
};

template<template<typename> typename T, class... Args>
struct impl_init_template
{
    template<class U>
    using type = T<U>;

    impl_init_template(Args&&... args)
        : ctorArguments {args...}
    {
    }

    template<class U>
    auto create()
    {
        return std::make_from_tuple<T<U>>(ctorArguments);
    }

    // FIXME: TODO: needs to properly deduce the types here to avoid copying
    std::tuple<Args...> ctorArguments;
};

template<class T, class... Args>
auto impl(Args&&... args)
{
    return impl_init<T, Args...>{std::forward<Args>(args)...};
}

template<template<typename> typename T, class... Args>
auto impl(Args&&... args)
{
    return impl_init_template<T, Args...>{std::forward<Args>(args)...};
}

template<class Info, class T>
constexpr auto info_count_impl()
{
    return list_count_if(
        typename Info::implementation::list_t{},
        mixin::list_is_type<T>
    );
}

template<
    template<typename> typename... Iface,
    typename... Impl>
auto make_composite(Impl&&... init)
{
    using inf = temp_info<
        iface<Iface...>,
        ImplList<Impl...>
    >;

    return composite<
        ImplList<typename Impl::template type<inf> ...>,
        iface<Iface...>
    >{init.template create<inf>()...};
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

template<class... Impl,
         template<typename> typename... Iface>
void composite<ImplList<Impl...>, iface<Iface...>>::construct()
{
    for_each_ability<ability::constructible::ctor>(this);
}

template<class... Impl,
         template<typename> typename... Iface>
void composite<ImplList<Impl...>, iface<Iface...>>::destruct()
{
    for_each_ability<ability::destructible::dtor>(this);
}

} // namespace mixin

#endif // MIXIN_MIXIN_HPP
