#ifndef MIXIN_TUPLE_AT_HPP
#define MIXIN_TUPLE_AT_HPP

#include <tuple>

namespace mixin
{

namespace detail
{
template<class Tuple, class Fun, std::size_t... I>
void tuple_at(Tuple &tuple, std::size_t idx, Fun fun, std::index_sequence<I...>)
{
    auto call_if = [idx, fun](auto &el, std::size_t i) {
        if (idx == i)
            fun(el);
    };

    (call_if(std::get<I>(tuple), I), ...);
}
} // namespace detail

template<class Tuple, class Fun>
void tuple_at(Tuple &tuple, std::size_t idx, Fun fun)
{
    return detail::tuple_at(
        tuple,
        idx,
        fun,
        std::make_index_sequence<std::tuple_size_v<Tuple>>{}
    );
}

} // namespace mixin

#endif // MIXIN_TUPLE_AT_HPP
