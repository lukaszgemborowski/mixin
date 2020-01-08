#ifndef MIXIN_MIXIN_HPP
#define MIXIN_MIXIN_HPP

#include <mixin/tuple_select.hpp>
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

template<class Sign>
struct top_hierarchy {
    using sign_t = Sign;
};

template<class Impl, template<typename> typename... If>
struct composite;

template<class Impl, template<typename> typename... If>
struct composite_signature
{
    using composite_t = composite<Impl, If...>;
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

template<class Impl, template<typename> typename... If>
struct composite
    : public interface_base<composite_signature<Impl, If...>, If...>
{
    template<class T>
    auto & get()
    {
        return std::get<T>(impl);
    }

    typename Impl::tuple_t impl;
};

namespace detail
{
template<std::size_t I, class Base, class Selector, class F>
void for_each(Base *self, Selector sel, F fun)
{
    using sign_t = typename Base::sign_t;
    using comp_t = typename sign_t::composite_t;
    using impl_tuple_t = typename sign_t::impl_tuple_t;
    using impl_t = std::tuple_element_t<I, impl_tuple_t>;
    auto &impl = static_cast<comp_t *>(self)->impl;

    if constexpr (Selector::template invoke<impl_t>::value) {
        fun(std::get<I>(impl));
    }
}

template<class Base, class Selector, class F, std::size_t... Idx>
void for_each(Base *self, Selector sel, F fun, std::index_sequence<Idx...>)
{
    (for_each<Idx>(self, sel, fun), ...);
}
} // namespace detail

template<class T>
struct implements
{
    template<class Q>
    using invoke = list_has<typename Q::implements, T>;
};

template<class Base, class Selector, class F>
void for_each(Base *self, Selector sel, F fun)
{
    using sign_t = typename Base::sign_t;
    using comp_t = typename sign_t::composite_t;
    using impl_t = typename sign_t::impl_tuple_t;

    detail::for_each(
        self,
        sel,
        fun,
        std::make_index_sequence<std::tuple_size_v<impl_t>>{}
    );
}

} // namespace mixin

#endif // MIXIN_MIXIN_HPP
