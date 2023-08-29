# Maybe
## Functor

``` cpp
#include <iostream>
#include <string>

double increment(const int& x) {
    return x + 1.5;
}

std::string intToString(const int& x) {
    return "Number: " + std::to_string(x);
}

int main() {
    // 使用 Maybe
    Maybe<int> justFive(5);
    Maybe<double> incremented = fmap(increment, justFive);
    if (incremented.isJust) {
        std::cout << "Incremented value: " << incremented.value << std::endl;  // 输出 6.5
    }

    Maybe<std::string> stringified = fmap(intToString, justFive);
    if (stringified.isJust) {
        std::cout << stringified.value << std::endl;  // 输出 "Number: 5"
    }
}
```

## Monad

``` cpp
// 示例转换函数
Maybe<int> addOne(const int& x) {
    return Maybe<int>(x + 1);
}

Either<std::string, int> subtractTwo(const int& x) {
    return Either<std::string, int>::Right(x - 2);
}

int main() {
    // 使用 Maybe Monad
    Maybe<int> justFive(5);
    Maybe<int> result = bind(justFive, addOne);
    if (result.isJust) {
        std::cout << result.value << std::endl;  // 输出 6
    }
}
```

# Either
## Functor

``` cpp
#include <iostream>
#include <string>

double increment(const int& x) {
    return x + 1.5;
}

std::string intToString(const int& x) {
    return "Number: " + std::to_string(x);
}

// 使用 Either
Either<std::string, int> rightValue = Either<std::string, int>::Right(7);
Either<std::string, double> incrementedEither = fmap(increment, rightValue);
if (incrementedEither.isRight) {
    std::cout << "Incremented Either value: " << incrementedEither.rightValue << std::endl;  // 输出 8.5
}

Either<std::string, std::string> stringifiedEither = fmap(intToString, rightValue);
if (stringifiedEither.isRight) {
    std::cout << stringifiedEither.rightValue << std::endl;  // 输出 "Number: 7"
}

Either<std::string, int> leftValue = Either<std::string, int>::Left("Error");
Either<std::string, double> leftIncremented = fmap(increment, leftValue);
if (!leftIncremented.isRight) {
    std::cout << "Left Either value: " << leftIncremented.leftValue << std::endl;  // 输出 "Error"
}
```

## Monad
``` cpp
// 示例转换函数
Maybe<int> addOne(const int& x) {
    return Maybe<int>(x + 1);
}

Either<std::string, int> subtractTwo(const int& x) {
    return Either<std::string, int>::Right(x - 2);
}

int main() {
    // 使用 Either Monad
    Either<std::string, int> rightValue = Either<std::string, int>::Right(7);
    Either<std::string, int> eitherResult = bind(rightValue, subtractTwo);
    if (eitherResult.isRight) {
        std::cout << eitherResult.rightValue << std::endl;  // 输出 5
    }
}
```

# compose的用法

``` cpp
// 示例转换函数
Maybe<int> addOne(const int& x) {
    return Maybe<int>(x + 1);
}

Maybe<int> multiplyByTwo(const int& x) {
    return Maybe<int>(x * 2);
}

Maybe<int> subtractThree(const int& x) {
    return Maybe<int>(x - 3);
}

Maybe<int> divideByTwo(const int& x) {
    if (x % 2 == 0) {
        return Maybe<int>(x / 2);
    } else {
        return Maybe<int>();  // 返回 Nothing
    }
}

int main() {
    // 创建一个组合函数
    auto combinedFunction = compose(addOne, multiplyByTwo, subtractThree, divideByTwo);
    
    Maybe<int> justFour(4);
    
    // 使用组合函数
    Maybe<int> result = combinedFunction(justFour.value);
    
    if (result.isJust) {
        std::cout << result.value << std::endl;
        // 输出 2. 因为 (((4 / 2) - 3) * 2) + 1 = 2
    } else {
        std::cout << "No result" << std::endl;
    }
}
```