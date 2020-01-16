#include <mixin/mixin.hpp>
#include "catch.hpp"

// define abilities
namespace ability
{

template<class T>
struct Allocator
{
    MIXIN_ABILITY_METHOD(Allocator, allocate, T* (int));
    MIXIN_ABILITY_METHOD(Allocator, deallocate, void (T*));
};

template<class T>
struct ArrayAccess
{
    MIXIN_ABILITY_METHOD(ArrayAccess, at, T& (std::size_t));
};

} // namespace ability

template<class T, class Base>
struct StaticContainerIF : Base
{
    T* allocate(int size)
    {
        return mixin::execute_ability<typename ability::Allocator<T>::allocate>(this, size);
    }

    void deallocate(T* ptr)
    {
        mixin::execute_ability<typename ability::Allocator<T>::deallocate>(this, ptr);
    }

    T& at(std::size_t idx)
    {
        return mixin::execute_ability<typename ability::ArrayAccess<T>::at>(this, idx);
    }
};

template<class T>
struct MallocAllocator
{
    using implements = mixin::list<ability::Allocator<T>>;

    template<class Mixin>
    T* allocate(Mixin &, int size)
    {
        return static_cast<T *>(malloc(size * sizeof (T)));
    }

    template<class Mixin>
    void deallocate(Mixin &, T* ptr)
    {
        free(ptr);
    }
};

template<class T>
struct ArrayAccessor
{
    using implements = mixin::list<
        ability::ArrayAccess<T>,
        mixin::ability::constructible,
        mixin::ability::destructible
    >;

    ArrayAccessor(std::size_t size)
        : size {size}
    {}

    template<class Mixin>
    void ctor(Mixin &m)
    {
        array = m.allocate(size);
    }

    template<class Mixin>
    void dtor(Mixin &m)
    {
        m.deallocate(array);
    }

    template<class Mixin>
    T& at(Mixin &m, std::size_t idx)
    {
        return array[idx];
    }

private:
    std::size_t size = 0u;
    T *array = nullptr;
};

template<class Base> using IntContainer = StaticContainerIF<int, Base>;
using IntAllocatorAbility = ability::Allocator<int>;
using IntArrayAbility = ability::ArrayAccess<int>;
using IntAllocator = MallocAllocator<int>;
using IntArray = ArrayAccessor<int>;

TEST_CASE("Can allocate and deallocate", "[example]")
{
    auto mix = mixin::make_composite<IntContainer>(
        IntAllocator{}, IntArray{100}
    );

    mix.at(42) = 42;
    REQUIRE(mix.at(42) == 42);
}
