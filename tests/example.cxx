#include <mixin/mixin.hpp>
#include "catch.hpp"

// define abilities
namespace ability
{

struct IntAllocator
{
    MIXIN_ABILITY_METHOD(IntAllocator, allocate, int* (int));
    MIXIN_ABILITY_METHOD(IntAllocator, deallocate, void (int *));
};

struct IntArrayAccess
{
    MIXIN_ABILITY_METHOD(IntArrayAccess, at, int& (std::size_t));
};

} // namespace ability

template<class T>
struct IntContainerInterface : T
{
    int& at(std::size_t idx)
    {
        return mixin::execute_ability<ability::IntArrayAccess::at>(this, idx);
    }
};

struct IntMallocAllocator
{
    using implements = mixin::list<ability::IntAllocator>;

    template<class Mixin>
    int* allocate(Mixin &, int size)
    {
        return static_cast<int *>(malloc(size * sizeof (int)));
    }

    template<class Mixin>
    void deallocate(Mixin &, int* ptr)
    {
        free(ptr);
    }
};

struct IntStorageAccess
{
    using implements = mixin::list<
        ability::IntArrayAccess,
        mixin::ability::constructible,
        mixin::ability::destructible
    >;

    IntStorageAccess(std::size_t size)
        : size {size}
    {}

    template<class Mixin>
    void ctor(Mixin &m)
    {
        array = mixin::execute_ability<ability::IntAllocator::allocate>(&m, size);
    }

    template<class Mixin>
    void dtor(Mixin &m)
    {
        mixin::execute_ability<ability::IntAllocator::deallocate>(&m, array);
    }

    template<class Mixin>
    int& at(Mixin &m, std::size_t idx)
    {
        return array[idx];
    }

private:
    std::size_t size = 0u;
    int *array = nullptr;
};

TEST_CASE("Create int container with malloc backed allocator", "[example]")
{
    auto mix = mixin::make_composite<IntContainerInterface>(
        IntStorageAccess{100},
        IntMallocAllocator{}
    );

    mix.at(42) = 42;
    REQUIRE(mix.at(42) == 42);
}
