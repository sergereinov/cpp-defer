# Golang-style defer in C++

Example 1
```C++
void example1()
{
    int a = 0;

    defer_t defer;
    defer += [&a] { std::cout << "defer #1: 'a' is " << a << std::endl; };

    std::cout << "touch point #1: 'a' is " << a << std::endl;

    if (a == 0)
    {
        defer += [&a] { std::cout << "defer #2: 'a' is " << a << std::endl; };

        ++a;
        std::cout << "touch point #2: 'a' is " << a << std::endl;
    }

    ++a;
    std::cout << "touch point #3: 'a' is " << a << std::endl;
    return;
}
```
Output
```
touch point #1: 'a' is 0
touch point #2: 'a' is 1
touch point #3: 'a' is 2
defer #2: 'a' is 2
defer #1: 'a' is 2
```
---
Example 2
```C++
template <bool can_alloc = false>
bool example2()
{
    // some preparation
    auto fmt = "sqrt(2) = %f";
    int sz = std::snprintf(nullptr, 0, fmt, std::sqrt(2));

    // conditional resource acquisition
    void* ptr = nullptr;
    defer_t defer;
    if constexpr (can_alloc)
    {
        // you can't RAII because the scope is different
        // or don't want RAII due to readability or simplicity
        ptr = malloc(sz);
        if (ptr) defer += [&ptr] { free(ptr); };
    }

    // if fail then report false
    if (!ptr) return false;

    // work with resource
    std::snprintf(static_cast<char*>(ptr), sz, fmt, std::sqrt(2));
    std::cout << static_cast<char*>(ptr) << std::endl;

    // report true and release resource via defer
    return true;
}
```

## License
MIT
