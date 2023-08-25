#ifndef _TYPES_HPP_
#define _TYPES_HPP_

template <typename T, class Tag>
class NewType
{
    T value;

public:
    explicit NewType(const T &v)
        : value(v) {}

    T &get()
    {
        return value;
    }
    const T &get() const
    {
        return value;
    }
};

template <typename T1, typename T2>
class Tuple
{
private:
    T1 fst;
    T2 snd;

public:
    Tuple(const T1 &a, const T2 &b)
        : fst(a), snd(b) {}

    T1 getFst() const
    {
        return fst;
    }
    T2 getSnd() const
    {
        return snd;
    }
};

template <class T1, class T2>
class TupleMut
{
public:
    T1 fst;
    T2 snd;

    TupleMut(const T1 &fst, const T2 &snd)
        : fst(fst), snd(snd) {}

    TupleMut(const TupleMut &src)
        : fst(src.fst), snd(src.snd) {}
    TupleMut &operator=(const TupleMut &src)
    {
        if (this != &src)
        {
            fst = src.fst;
            snd = src.snd;
        }
        return *this;
    }
};

// Tag
struct PinIdTag
{
};
struct ErrIdTag
{
};

using PinId = NewType<uint8_t, PinIdTag>;
using ErrId = NewType<int, ErrIdTag>;

#endif // !_TYPES_HPP_