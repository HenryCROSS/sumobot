#ifndef _FUNCTIONAL_TOOLS_HPP_
#define _FUNCTIONAL_TOOLS_HPP_

#include <Tools.hpp>

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

// Functor impl 
// fmap :: (a -> b) -> f a -> f b

// For Maybe
template <typename T, typename U>
Maybe<U> fmap(U (*func)(T), const Maybe<T>& maybe) {
    if (maybe.hasValue()) {
        return Maybe<U>(func(maybe.getValue()));
    } else {
        return Maybe<U>();
    }
}

// For Either
template <typename L, typename R, typename U>
Either<L, U> fmap(U (*func)(R), const Either<L, R>& either) {
    if (either.isRight()) {
        return Either<L, U>::Right(func(either.getRight()));
    } else {
        return Either<L, U>::Left(either.getLeft());
    }
}

// Monad impl
// returnM :: a -> m a
// bind :: m a -> (a -> m b) -> m b

// For Maybe
template <typename T>
Maybe<T> returnM(T value) {
    return Maybe<T>(value);
}

template <typename T, typename U>
Maybe<U> bind(const Maybe<T>& maybe, Maybe<U> (*func)(const T&)) {
    if (maybe.hasValue()) {
        return func(maybe.getValue());
    } else {
        return Maybe<U>();
    }
}

// For Either
template <typename L, typename R>
Either<L, R> returnM(R value) {
    return Either<L, R>::Right(value);
}

template <typename L, typename R, typename U>
Either<L, U> bind(const Either<L, R>& either, Either<L, U> (*func)(const R&)) {
    if (either.isRight()) {
        return func(either.getRight());
    } else {
        return Either<L, U>::Left(either.getLeft());
    }
}


#endif // !_FUNCTIONAL_TOOLS_HPP_