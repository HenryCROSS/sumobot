#ifndef _MAYBE_HPP_
#define _MAYBE_HPP_

template <typename T>
class Maybe
{
private:
    bool isJust;
    T value;

public:
    Maybe()
        : isJust(false), value() {}

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

    // map::(a -> b) -> M b
    template <typename Fn>
    auto map(Fn f) -> Maybe<decltype(f(value))>
    {
        if (!isJust)
        {
            return Maybe<decltype(f(value))>::Nothing();
        }
        else
        {
            return Maybe<decltype(f(value))>(f(value));
        }
    }

    // map_or::(a -> b) -> b -> M b
    template <typename Fn>
    auto map_or(Fn f, decltype(f(value)) default_val) -> Maybe<decltype(f(value))>
    {
        if (!isJust)
        {
            return Maybe<decltype(f(value))>(default_val);
        }
        else
        {
            return Maybe<decltype(f(value))>(f(value));
        }
    }

    // map_or_else::(a -> b) -> (() -> b) -> M b
    template <typename Fn, typename ElseFn>
    auto map_or_else(Fn f, ElseFn else_f) -> Maybe<decltype(f(val))>
    {
        if (!isJust)
        {
            return Maybe<decltype(f(value))>(else_f());
        }
        else
        {
            return Maybe<decltype(f(value))>(f(value));
        }
    }

    T unwrap_or_default(T default_val)
    {
        if (!isJust)
        {
            return default_val;
        }
        else
        {
            return value;
        }
    }

    T unwrap_unchecked()
    {
        return val;
    }
};

#endif // !_MAYBE_HPP_