#include <mixin/is_template.hpp>
#include "catch.hpp"

struct non_template_class {};
template<class> struct single_arg_template_class {};
template<class, class> struct two_arg_template_class {};

static_assert(mixin::is_template<non_template_class>() == false);
static_assert(mixin::is_template<single_arg_template_class>() == true);
static_assert(mixin::is_template<two_arg_template_class>() == false);
