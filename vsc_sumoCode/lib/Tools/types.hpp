#ifndef _TYPES_H_H_
#define _TYPES_H_H_

template <typename _Tp, _Tp _v>
struct intergral_constant
{
    static constexpr _Tp value = _v;
    using value_type = _Tp;
    using type = intergral_constant<_Tp, _v>;

    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; } // since c++14
};

using true_type = intergral_constant<bool, true>;
using false_type = intergral_constant<bool, false>;

template <typename, typename>
struct is_same : public false_type
{
};
template <typename _Tp>
struct is_same<_Tp, _Tp> : public true_type
{
};

template< class T, class U >
inline constexpr bool is_same_v = is_same<T, U>::value;

#endif // !_TYPES_H_H_