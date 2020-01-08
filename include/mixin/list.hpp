#ifndef MIXIN_LIST_HPP
#define MIXIN_LIST_HPP

#include <type_traits>

namespace mixin
{
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
} // namespace mixin

#endif // MIXIN_LIST_HPP
