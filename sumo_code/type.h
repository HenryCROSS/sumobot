#ifndef TYPE_H_
#define TYPE_H_

template <class T> class Maybe
{
  private:
    bool nothing;
    T var;

  public:
    Maybe() : nothing(true)
    {
    }

    Maybe(T src) : var(src), nothing(false)
    {
    }

    bool isNothing()
    {
        return nothing;
    }

    T just()
    {
        return var;
    }
};

#endif // TYPE_H_
