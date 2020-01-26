#ifndef MIXIN_TUPLE_FOR_EACH_HPP
#define MIXIN_TUPLE_FOR_EACH_HPP

#include <tuple>

namespace mixin
{
namespace detail
{

template<std::size_t I, class Tuple, class Fun>
constexpr void tuple_for_each(Tuple &tuple, Fun &fun)
{
    fun(std::get<I>(tuple));
}

template<class Tuple, class Fun, std::size_t... Idx>
constexpr void tuple_for_each(Tuple &tuple, Fun &fun, std::index_sequence<Idx...>)
{
    (tuple_for_each<Idx>(tuple, fun), ...);
}

} // namespace detail

template<class Tuple, class Fun>
constexpr void tuple_for_each(Tuple &tuple, Fun fun)
{
    detail::tuple_for_each(
        tuple,
        fun,
        std::make_index_sequence<std::tuple_size_v<Tuple>>{}
    );
}

} // namespace mixin

#endif // MIXIN_TUPLE_FOR_EACH_HPP
