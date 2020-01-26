#ifndef MIXIN_LIST_HPP
#define MIXIN_LIST_HPP

#include <type_traits>

namespace mixin
{
namespace mpl
{
template<class T>
struct type_t
{
    using type = T;
};

template<class A, class B>
constexpr bool is_same(A, B)
{
    return std::is_same<A, B>::value;
}

constexpr std::size_t LIST_MAX_ELEMENTS = (0xffffffff - 1);
constexpr std::size_t LIST_NOT_FOUND = LIST_MAX_ELEMENTS + 1;

template<class... Item> struct list {};
template<class Head, class... Tail> struct list<Head, Tail...>
{
    using head_t = Head;
    using tail_t = list<Tail...>;

    static constexpr std::size_t size = 1 + sizeof...(Tail);
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

template<class L, class T> struct list_push_front {};
template<class T, class... Args> struct list_push_front<list<Args...>, T>
{
    using type_t = list<T, Args...>;
};

template<class List, class Fun>
constexpr std::size_t list_count_if(List list, Fun fun)
{
    if constexpr (list.size == 1) {
        return fun(mpl::type_t<typename List::head_t>{});
    } else {
        return fun(mpl::type_t<typename List::head_t>{}) + list_count_if(typename List::tail_t{}, fun);
    }
}

template<class T>
constexpr auto list_is_type = [](auto t) constexpr {
    return mpl::is_same(t, mpl::type_t<T>{});
};

namespace detail
{

template<class Fun>
constexpr std::size_t list_find_index_if(list<>, Fun, std::size_t)
{
    return LIST_NOT_FOUND;
}

template<class List, class Fun>
constexpr std::size_t list_find_index_if(List list, Fun fun, std::size_t index)
{
    if (fun(mpl::type_t<typename List::head_t>{})) {
        return index;
    } else {
        return list_find_index_if(typename List::tail_t{}, fun, index + 1);
    }
}
} // namespace detail

template<class List, class Fun>
constexpr std::size_t list_find_index_if(List list, Fun fun)
{
    return detail::list_find_index_if(list, fun, 0);
}

} // namespace mpl
} // namespace mixin

#endif // MIXIN_LIST_HPP
