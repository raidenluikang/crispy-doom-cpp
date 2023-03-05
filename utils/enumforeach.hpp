

#ifndef CRISPY_DOOM_CPP_UTILS_ENUMFOREACH_HPP
#define CRISPY_DOOM_CPP_UTILS_ENUMFOREACH_HPP

#include <type_traits>

// #define CRISP_ENUM_FOREACH(enum_t, index, begin, end) \
//     for (int  begin_value = static_cast<int>(begin), end_value = static_cast<int>(end), index_value = begin_value; index_value < end_value; ++index_value ) 

template <typename TEnum, typename Functor>
void enum_foreach(TEnum begin, TEnum end, Functor&& functor)
{
    using under_t = std::underlying_type_t<TEnum>;
    
    const under_t begin_v = static_cast<under_t>(begin);
    const under_t end_v = static_cast<under_t>(end);

    for (under_t index = begin_v; index < end_v; ++index)
    {
        TEnum index_e = static_cast<TEnum>(index);
        functor(index_e);
    }
}

#endif // CRISPY_DOOM_CPP_UTILS_ENUMFOREACH_HPP
