#ifndef CRISPY_DOOM_CPP_UTILS_MEMORY_HPP
#define CRISPY_DOOM_CPP_UTILS_MEMORY_HPP

#include <cstdlib> // malloc
#include <cstddef> // size_t
#include <type_traits>
#include <new>     // placement new

//make_unique : ))


template <typename T >
std::enable_if_t<!std::is_array<T>::value, T*>
 create_structure()
{
    auto* mem = std::malloc(sizeof(T));
    return new(mem)T{};
}

//C++20 have this type trait, but currently  it's not supported by old compilers.
namespace details
{
template<class T>
struct is_unbounded_array: std::false_type {};
 
template<class T>
struct is_unbounded_array<T[]> : std::true_type {};

template <class T>
struct is_bounded_array: std::false_type{};

template <typename T, size_t N>
struct is_bounded_array<T[N]>: std::true_type{};
} // namespace details

// T is U[]
template<typename T>
std::enable_if_t< ::details::is_unbounded_array<T>::value, std::remove_extent_t<T>* > 
create_structure(std::size_t size)
{
    // T is U[]
    using U = std::remove_extent_t<T>;
    auto *mem = std::malloc(sizeof(U)* size);
    return  new(mem)U[size]{} ;
}


// T is U[N]
template <typename T, class ... Args>
std::enable_if_t< ::details::is_bounded_array<T>::value> 
create_structure(Args&& ... ) = delete;

//please, include z_zone.hpp before using zmalloc
#ifdef __Z_ZONE__
//void*	Z_Malloc (int size, int tag, void *ptr);

template <typename T>
std::remove_reference_t<T> zmalloc(int size, int tag, void* ptr)
{
    return static_cast<std::remove_reference_t<T>>(Z_Malloc(size, tag, ptr));
}
#endif //declare first



#endif //CRISPY_DOOM_CPP_UTILS_MEMORY_HPP