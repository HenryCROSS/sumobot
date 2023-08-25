#ifndef _TYPES_HPP_
#define _TYPES_HPP_

#include <Functional_interface.hpp>

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
class Maybe : public Functor<Maybe<T>, T>, public Monad<Maybe<T>, T>
{
private:
    bool isJust;
    T value;

public:
    Maybe()
        : isJust(false) {}

    explicit Maybe(const T &val)
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

    // Testing
    Maybe<T> fmap(T (*func)(T)) const override
    {
        if (isJust)
        {
            return Maybe<T>(func(value));
        }
        else
        {
            return Maybe<T>::Nothing();
        }
    }

    Maybe<T> returnM(const T &val) const override
    {
        return Maybe<T>(val);
    }

    // m a -> (a -> m b) -> m b
    Maybe<T> bind(Maybe<T> (*func)(T)) const override
    {
        if (isJust)
        {
            return func(value);
        }
        else
        {
            return Maybe<T>::Nothing();
        }
    }
};

// testing
template <typename L, typename R>
class Either : public Functor<Either<L, R>, R>, public Monad<Either<L, R>, R>
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
    static Either Left(const L &value)
    {
        return Either(Tag::LEFT, value, R());
    }

    static Either Right(const R &value)
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

    Either<L, R> fmap(R (*func)(R)) const override
    {
        if (isRight())
        {
            return Either<L, R>::Right(func(rightValue));
        }
        else
        {
            return *this; // return the Left value unchanged
        }
    }

    Either<L, R> returnM(const R &val) const override
    {
        return Either<L, R>::Right(val);
    }

    Either<L, R> bind(Either<L, R> (*func)(R)) const override
    {
        if (isRight())
        {
            return func(rightValue);
        }
        else
        {
            return *this; // Return the Left value unchanged.
        }
    }

private:
    Either(Tag t, const L &left, const R &right)
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

// BEGIN compose
/* Example:
  auto f = [](int x) { return x + 1; };
  auto g = [](int x) { return x * 2; };
  auto h = [](int x) { return x - 3; };

  auto combined = compose(f, g, h);
  std::cout << combined(3);  // ouput 4, because ((3 - 3) * 2) + 1 = 4
*/
template <typename F, typename G>
auto compose(F f, G g)
{
    return [f, g](auto x)
    {
        return f(g(x));
    };
}

template <typename F, typename... Funcs>
auto compose(F f, Funcs... funcs)
{
    return [f, funcs...](auto x)
    {
        return f(compose(funcs...)(x));
    };
}
// END compose

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

#endif // !_TYPES_HPP_