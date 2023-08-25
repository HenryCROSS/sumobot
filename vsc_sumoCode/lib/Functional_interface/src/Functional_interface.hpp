#ifndef _FUNCTIONAL_TOOLS_HPP_
#define _FUNCTIONAL_TOOLS_HPP_

template <typename FunctorType, typename ValueType>
class Functor
{
public:
    virtual FunctorType fmap(ValueType (*func)(ValueType)) const = 0;
};

template <typename M, typename T>
class Monad
{
public:
    virtual M returnM(const T &value) const = 0;
    virtual M bind(M (*func)(T)) const = 0;
};



#endif // !_FUNCTIONAL_TOOLS_HPP_