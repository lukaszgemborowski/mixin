#include <mixin/list.hpp>

using list_1 = mixin::list<int, float, float, char>;

constexpr std::size_t list_1_float_count = mixin::list_count_if(
    list_1{},
    mixin::list_is_type<float>
);

static_assert(list_1_float_count == 2);
