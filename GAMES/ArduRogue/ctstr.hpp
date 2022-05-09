#pragma once

#include <stddef.h>
#include <stdint.h>

namespace detail
{
template<size_t... I> struct seq {};
template<size_t N, size_t... I>
struct gen_seq : gen_seq<N - 1, N - 1, I...> {};
template<size_t... I>
struct gen_seq<0, I...> : seq<I...> {};
}

template<size_t N> struct ctstr_t
{
    char const data[N + 1];
    static constexpr size_t size = N;
};

namespace detail
{
template<size_t... I>
constexpr ctstr_t<sizeof...(I)> ctstr(char const* d, detail::seq<I...>)
{
    return{ { d[I]..., '\0' } };
}
}

template<size_t N>
constexpr ctstr_t<N - 1> ctstr(char const (&d)[N])
{
    return detail::ctstr(d, detail::gen_seq<N - 1>{});
}

constexpr ctstr_t<1> ctstr(char c)
{
    return{ { c, '\0' } };
}

namespace detail
{
constexpr size_t ctstr_numchars(uintmax_t n, uintmax_t base)
{
    return n < base ? 1 : ctstr_numchars(n / base, base) + 1;
}
static constexpr char BASE_CHARS[] = "01234565789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
template<bool B, class T = void>
struct enable_if {};
template<class T>
struct enable_if<true, T> { typedef T type; };
}

template<uintmax_t V, uintmax_t BASE>
constexpr ctstr_t<1> ctstr_unsigned(
    typename detail::enable_if<(BASE <= 36)>::type* = 0,
    typename detail::enable_if<(V < BASE)>::type* = 0)
{
    return ctstr(detail::BASE_CHARS[V]);
}
template<uintmax_t V, uintmax_t BASE>
constexpr ctstr_t<detail::ctstr_numchars(V, BASE)> ctstr_unsigned(
    typename detail::enable_if<(BASE <= 36)>::type* = 0,
    typename detail::enable_if<(V >= BASE)>::type* = 0)
{
    return ctstr_unsigned<V / BASE, BASE>() + ctstr(detail::BASE_CHARS[V % BASE]);
}

template<intmax_t V, uintmax_t BASE>
constexpr ctstr_t<detail::ctstr_numchars(uintmax_t(-V), BASE) + 1>
ctstr_signed(typename detail::enable_if<(V < 0)>::type* = 0)
{
    return ctstr('-') + ctstr_unsigned<uintmax_t(-V), BASE>();
}
template<intmax_t V, uintmax_t BASE>
constexpr ctstr_t<detail::ctstr_numchars(uintmax_t(V), BASE)>
ctstr_signed(typename detail::enable_if<(V >= 0)>::type* = 0)
{
    return ctstr_unsigned<uintmax_t(V), BASE>();
}

template<uintmax_t W, size_t N>
constexpr ctstr_t<N> ctstr_leftpad(
    char c, ctstr_t<N> const& s,
    typename detail::enable_if<(W <= N)>::type* = 0)
{
    return s;
}
template<uintmax_t W, size_t N>
constexpr ctstr_t<W> ctstr_leftpad(
    char c, ctstr_t<N> const& s,
    typename detail::enable_if<(W > N)>::type* = 0)
{
    return ctstr(c) + ctstr_leftpad<W - 1>(c, s);
}

template<class T, T V>
constexpr ctstr_t<sizeof(T) * 2> ctstr_hex()
{
    return ctstr_leftpad<sizeof(T) * 2>('0', ctstr_unsigned<V, 16>());
}

namespace detail
{
template<size_t N1, size_t N2, size_t... I1, size_t... I2>
constexpr ctstr_t<N1 + N2> concat_ctstr(
    ctstr_t<N1> const& s1, ctstr_t<N2> const& s2,
    detail::seq<I1...>, detail::seq<I2...>)
{
    return{ { s1.data[I1]..., s2.data[I2]..., '\0' } };
}
}

template<size_t N1, size_t N2>
constexpr ctstr_t<N1 + N2> operator+(ctstr_t<N1> const& s1, ctstr_t<N2> const& s2)
{
    return detail::concat_ctstr(
        s1, s2, detail::gen_seq<N1>{}, detail::gen_seq<N2>{});
}
