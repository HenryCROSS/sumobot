#ifndef _TYPES_HPP_
#define _TYPES_HPP_

#include <Arduino.h>

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

template <typename T>
class Maybe
{
private:
    bool isJust;
    T value;

public:
    Maybe()
        : isJust(false) {}

    explicit Maybe(T val)
        : isJust(true), value(val) {}

    bool hasValue() const
    {
        return isJust;
    }

    // call this after call hasValue()
    T getValue() const
    {
        if (!isJust)
        {
            return T();
        }
        return value;
    }

    static Maybe Nothing()
    {
        return Maybe();
    }
};

// testing
template <typename L, typename R>
class Either
{
private:
    enum class Tag
    {
        LEFT,
        RIGHT
    } tag;

    union
    {
        L leftValue;
        R rightValue;
    };

public:
    static Either Left(L value)
    {
        return Either(Tag::LEFT, value, R());
    }

    static Either Right(R value)
    {
        return Either(Tag::RIGHT, L(), value);
    }

    bool isLeft() const
    {
        return tag == Tag::LEFT;
    }

    bool isRight() const
    {
        return tag == Tag::RIGHT;
    }

    L getLeft() const
    {
        if (isRight())
        {
            R();
        }
        return leftValue;
    }

    R getRight() const
    {
        if (isLeft())
        {
            L();
        }
        return rightValue;
    }

private:
    Either(Tag t, L left, R right)
        : tag(t)
    {
        if (t == Tag::LEFT)
        {
            leftValue = left;
        }
        else
        {
            rightValue = right;
        }
    }
};


/*
  Example:
  actions_duration(1000, [](){
      car_stop();
  });
  actions_duration(1000, car_turn_left, 100);
  actions_duration(1000, car_stop());
*/
template <typename Fn, typename... Args>
void do_actions_duration(unsigned long ms, Fn fn, Args... args)
{
    fn(args...);
    delay(ms);
}

// Tag
struct PinIdTag
{
};
struct ErrIdTag
{
};

using PinId = NewType<uint8_t, PinIdTag>;
using ErrId = NewType<int, ErrIdTag>;

// f(0) = 0.5; f(1) = 0.7311; lim_x->inf f(x) = 1
double curve_algorithm(double x);

#endif // !_TYPES_HPP_