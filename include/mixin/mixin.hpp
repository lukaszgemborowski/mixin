#ifndef MIXIN_MIXIN_HPP
#define MIXIN_MIXIN_HPP

#include <tuple>
#include <type_traits>

namespace mixin
{

// list begin
template<class... Item> struct list;
template<class Head, class... Tail> struct list<Head, Tail...>
{
    using head_t = Head;
    using tail_t = list<Tail...>;
};

template<class List, class T> struct list_has
{
    using type_t =
        std::conditional_t<
            std::is_same_v<typename List::head_t, T>,
            std::true_type,
            typename list_has<typename List::tail_t, T>::type_t>;
    static constexpr bool value = type_t::value;
};

template<class T> struct list_has<list<>, T>
{
    using type_t = std::false_type;
    static constexpr bool value = false;
};
// list end

template<class... Impl>
struct impl
{
    using tuple_t = std::tuple<Impl...>;
};

struct access {};

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
struct interface_base<Sign, Last> : public Last<access>
{
    using sign_t = Sign;
};

template<class Impl, template<typename> typename... If>
struct composite
    : public interface_base<composite_signature<Impl, If...>, If...>
{
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
