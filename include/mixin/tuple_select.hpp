#ifndef MIXIN_TUPLE_SELECT_HPP
#define MIXIN_TUPLE_SELECT_HPP

#include <tuple>

namespace mixin
{

namespace detail
{

template<class T>
auto tupletize_if(T &item, std::true_type)
{
    return std::tuple<T &>{item};
}

template<class T>
auto tupletize_if(T &, std::false_type)
{
    return std::tuple<>{};
}

template<class Select, class Tuple, std::size_t... Idx>
auto tuple_select_ref(Tuple &tuple, std::index_sequence<Idx...>)
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
} // namespace detail

template<class Select, class Tuple>
auto tuple_select_ref(Tuple &tuple)
{
    return detail::tuple_select_ref<Select, Tuple>(
        tuple,
        std::make_index_sequence<std::tuple_size_v<Tuple>>{}
    );
}

} // namespace mixin

#endif // MIXIN_TUPLE_SELECT_HPP
