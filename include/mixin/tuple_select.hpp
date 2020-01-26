#ifndef MIXIN_TUPLE_SELECT_HPP
#define MIXIN_TUPLE_SELECT_HPP

#include <tuple>

namespace mixin
{

namespace detail
{

template<class T>
constexpr auto tupletize_if(T &item, std::true_type)
{
    return std::tuple<T &>{item};
}

template<class T>
constexpr auto tupletize_if(T &, std::false_type)
{
    return std::tuple<>{};
}

template<class Select, class Tuple, std::size_t... Idx>
constexpr auto tuple_select_ref(Tuple &tuple, std::index_sequence<Idx...>)
{
    return std::tuple_cat(
        tupletize_if(
            std::get<Idx>(tuple),
            std::conditional_t<
                Select::template invoke<
                    std::tuple_element_t<Idx, Tuple>
                >::value,
                std::true_type,
                std::false_type
            >{}
        )...
    );
}

template<class Select, class Tuple, std::size_t I>
constexpr int count_type_if()
{
    if constexpr (Select::template invoke<std::tuple_element_t<I, Tuple>>::value == true)
        return 1;
    else
        return 0;
}

template<class Select, class Tuple, std::size_t... Idx>
constexpr auto tuple_select_size(std::index_sequence<Idx...>)
{
    return (0 + ... + count_type_if<Select, Tuple, Idx>());
}


} // namespace detail

template<class Select, class Tuple>
constexpr auto tuple_select_ref(Tuple &tuple)
{
    return detail::tuple_select_ref<Select, Tuple>(
        tuple,
        std::make_index_sequence<std::tuple_size_v<Tuple>>{}
    );
}

template<class Select, class Tuple>
constexpr auto tuple_select_size(const Tuple &t)
{
    return detail::tuple_select_size<Select, Tuple>(
        std::make_index_sequence<std::tuple_size_v<Tuple>>{}
    );
}

} // namespace mixin

#endif // MIXIN_TUPLE_SELECT_HPP
