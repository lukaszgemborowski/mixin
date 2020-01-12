#include <mixin/mixin.hpp>
#include "catch.hpp"

// define abilities
namespace ability
{

// Allocator ability requires two methods from implementation:
// - void* allocate(int)
// - void deallocate(void *)
struct Allocator
{
    MIXIN_ABILITY_METHOD(Allocator, allocate, void* (int));
    MIXIN_ABILITY_METHOD(Allocator, deallocate, void (void*));
};

struct StaticContainer
{
    MIXIN_ABILITY_METHOD(StaticContainer, at, std::size_t ());
};

}

template<class T>
struct AllocatorIF : T
{
    void* allocate(int size)
    {
        return mixin::execute_ability<ability::Allocator::allocate>(this, size);
    }

    void deallocate(void *ptr)
    {
        mixin::execute_ability<ability::Allocator::deallocate>(this, ptr);
    }
};

struct MallocAllocator
{
    using implements = mixin::list<ability::Allocator>;

    void *allocate(int size)
    {
        return malloc(size);
    }

    void deallocate(void *ptr)
    {
        free(ptr);
    }
};

TEST_CASE("Can allocate and deallocate", "[example]")
{
    auto mix = mixin::make_composite<AllocatorIF>(
        MallocAllocator{}
    );

    auto &impl = mix.get<MallocAllocator>();

    mix.allocate(42);
}
