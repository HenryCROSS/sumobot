#ifndef _TYPES_HPP_
#define _TYPES_HPP_

#include <Arduino.h>
#include <Configs.h>
#include <string.h>

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

template <typename T, int SIZE>
class Array
{
public:
    T &operator[](int index)
    {
        return data[index];
    }

    const T &operator[](int index) const
    {
        return data[index];
    }

    int capacity() const
    {
        return SIZE;
    }

    int size() const
    {
        return len;
    }

    bool push_back(const T &value)
    {
        if (len >= SIZE)
        {
            return false;
        }
        data[len] = value;
        ++len;
        return true;
    }

    bool pop_back()
    {
        if (len <= 0)
        {
            return false;
        }
        --len;
        return true;
    }

private:
    T data[SIZE];
    int len;
};

// WIP
template <typename V, size_t Size>
class Hashtable
{
public:
    struct Node
    {
        String key;
        V value;
        bool empty;
    };

    Hashtable()
    {
        for (size_t i = 0; i < Size; i++)
        {
            table[i] = Node{
                .key = "",
                .value = V(),
                .empty = true};
        }
    }

    ~Hashtable()
    {
    }

    // void insert(const String &key, V value)
    // {
    //     int i = 1;
    //     Node &node = table[hash(key) % Size];
    //     while (!node.empty && node.key != key)
    //     {
    //         node = table[(hash(key) + i) % Size];
    //         i++;
    //     }

    //     node.empty = false;
    //     node.key = key;
    //     node.value = value;
    // }

    // void remove(const String &key)
    // {
    //     int i = 1;
    //     Node &node = table[hash(key) % Size];
    //     while (!node.empty && node.key != key)
    //     {
    //         node = table[(hash(key) + i) % Size];
    //         i++;
    //     }

    //     node.empty = true;
    // }

    // V *find(const String &key)
    // {
    //     int i = 1;
    //     Node &node = table[hash(key) % Size];
    //     while (!node.empty && node.key != key && i < Size)
    //     {
    //         node = table[(hash(key) + i) % Size];
    //         i++;
    //     }
    // }

private:
    Array<Node, Size> table;
    size_t hash(const String &key)
    {
        return BKDRHash(key);
    }

    unsigned int BKDRHash(const String &str)
    {
        unsigned int seed = 131; // 31, 131, 1313, 13131, etc..
        unsigned int hash = 0;

        for (auto ch : str)
        {
            hash = hash * seed + ch;
        }

        return hash;
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
constexpr double curve_algorithm(double x)
{
    // return 1.0 / (1.0 + pow(E, -x));
    return x;
}

#endif // !_TYPES_HPP_