#ifndef MIXIN_IS_TEMPLATE_HPP
#define MIXIN_IS_TEMPLATE_HPP

#include <type_traits>

namespace mixin
{

template<class T>
constexpr bool is_template() { return false; }

template<template<typename, typename, typename...> typename T>
constexpr bool is_template() { return false; }

template<template<typename> typename T>
constexpr bool is_template() { return true; }

} // namespace mixin

#endif // MIXIN_IS_TEMPLATE_HPP
