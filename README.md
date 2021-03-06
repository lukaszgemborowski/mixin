# mixin

Compile time composite of arbitrary number of interfaces and implementations,
glued together with a concept of ability.

## State

Currently the library is unstable and in "testing" state. You may encounter different bugs and weird
compilation errors.

## Inspiration
This repository and code is heavily inspired by Odin Holmes' C++ Mixin talk, watch it here:

[![C++Now 2018: Odin Holmes “C++ Mixins: Customization Through Compile Time Composition”](https://img.youtube.com/vi/wWZi_wPyVvs/0.jpg)](https://www.youtube.com/watch?v=wWZi_wPyVvs)

You can learn the idea and key concepts from the talk itself.

## Example

Before using mixin you need to include the library:

```cpp
#include <mixin/mixin.hpp>
```

Each mixin (or composite) consists of an interface classes and implementation classes. All public methods of interface classes
creates a mixin interface. Interface class is a template class which inherits its template parameter:

```cpp
template<class T>
struct IntContainerInterface : T
{
    int& at(std::size_t idx)
    {
    ...
    }
};
```

in that case we define an interface class with a single method `at`. The intention is to have an accesor to an int container. To
define this method we need to have an ability defined, so before the `at` defintion we need:

```cpp
namespace ability
{
struct IntArrayAccess
{
    MIXIN_ABILITY_METHOD(IntArrayAccess, at, int& (std::size_t));
};
} // namespace ability
```

no we can get back and implement interface `at` method:

```cpp
    int& at(std::size_t idx)
    {
        return mixin::execute_ability<ability::IntArrayAccess::at>(this, idx);
    }
```

as we can see, the ability defines a single method, `at` with `int & (std::size_t)` signature, which in C++ means a function
getting single std::size_t parameter and returning reference to an int. This is the method name and signature needed by the
implementation class and not by the interface class, but for consistency we define the interface class the same way. We will
need one more ability, an allocator ability:

```cpp
namespace ability
{
struct IntAllocator
{
    MIXIN_ABILITY_METHOD(IntAllocator, allocate, int* (int));
    MIXIN_ABILITY_METHOD(IntAllocator, deallocate, void (int *));
};
} // namespace ability
```

the pattern is the same as for `IntArrayAccess` ability, the only difference is that the `IntAllocator` ability requires
two methods from the implementation: `allocate` and `deallocate`. The main part of the mixin will be storage access implementation:

```cpp
struct IntStorageAccess
{
    using implements = mixin::mpl::list<
        ability::IntArrayAccess,        // we will provide at() method
        mixin::ability::constructible,  // internal mixin ability, will cause ctor() to be called
        mixin::ability::destructible    // internal mixin ability, will cause dtor() to be called
    >;

    IntStorageAccess(std::size_t size)
        : size {size}
    {}

    template<class Mixin>
    void ctor(Mixin &m)
    {
        // here we ask another implementation class to allocate an array for us,
        // the execute_ability<> call will find a suitable implementation from the
        // current mixin and invoke allocate method on it
        array = mixin::execute_ability<ability::IntAllocator::allocate>(&m, size);
    }

    template<class Mixin>
    void dtor(Mixin &m)
    {
        // similar to ctor/allocate
        mixin::execute_ability<ability::IntAllocator::deallocate>(&m, array);
    }

    // this is actual implementation of IntArrayAccess ability
    template<class Mixin>
    int& at(Mixin &m, std::size_t idx)
    {
        return array[idx];
    }

private:
    std::size_t size = 0u;
    int *array = nullptr;
};
```

now we can define a simple implementation or even different implementations for IntAllocator ability:

```cpp
struct IntMallocAllocator
{
    using implements = mixin::mpl::list<ability::IntAllocator>;

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

struct IntNewDelAllocator
{
    using implements = mixin::mpl::list<ability::IntAllocator>;

    template<class Mixin>
    int* allocate(Mixin &, int size)
    {
        return new int[size];
    }

    template<class Mixin>
    void deallocate(Mixin &, int* ptr)
    {
        delete [] ptr;
    }
};
```

with all the classes we can create a mixin consisting of defined elements, eg:

```cpp
auto mix = mixin::make_composite<IntContainerInterface>(
    mixin::impl<IntStorageAccess>{100},
    mixin::impl<IntMallocAllocator>{}
);
```

for a container backed by malloc allocator, or:

```cpp
auto mix = mixin::make_composite<IntContainerInterface>(
    mixin::impl<IntStorageAccess>{100},
    mixin::impl<IntNewDelAllocator>{}
);
```

for a container backed by a new/delete allocator. Both objects have exactly the same interface thanks to `IntContainerInterface`.

```cpp
mix.at(0) = 42;
auto foo = mix.at(0);
```
