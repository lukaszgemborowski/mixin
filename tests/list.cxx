#include <mixin/list.hpp>

using list_1 = mixin::mpl::list<int, float, float, char>;

constexpr std::size_t list_1_float_count = mixin::mpl::list_count_if(
    list_1{},
    mixin::mpl::list_is_type<float>
);

static_assert(list_1_float_count == 2);

constexpr std::size_t list_1_char_idx = mixin::mpl::list_find_index_if(
    list_1{},
    mixin::mpl::list_is_type<char>
);

static_assert(list_1_char_idx == 3);
