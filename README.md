## Golang-style defer in C++

### Motive

It is considered good practice to use the [RAII](https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization) approach when acquiring and releasing a resource.

But sometimes RAII looks bulky.
Perhaps you need to write a special wrapper for RAII and this adds extra entities and new meanings.
You must remember to handle all obvious and non-obvious uses of this new serving entity.

In addition, RAII hides the details of resource acquisition and release. This isn't always a good thing.

Besides RAII, there is another approach to deferred guaranteed resource release.
In golang, this is one of the uses of the [`defer`](https://go.dev/tour/flowcontrol/12) operator.
It postpones calling all lambdas passed to it until the function returns.

### defer.h

This library implements an approach similar to golang's `defer`.
The `defer_t` class remembers all passed lambdas and calls them in its destructor.

### How to use
Copy the `defer.h` header to where you want to use it.
Declare a `defer` variable and pass it some lambda.

```C++
defer_t defer;
defer += [] { std::cout << "Bye! I'm out." << std::endl; };
```

*TODO: warn about reverse destruction order for local objects.*

**Example 1:** lambdas call order demo.
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

**Example 2:** when you can't use RAII because the scope is different or you don't want RAII because of readability or simplicity.
```C++
std::set<int> active_events;

template<bool allow_recursion = false>
bool handler(int event_id, void (*worker)())
{
    defer_t defer;

    if constexpr (not allow_recursion)
    {
        auto [_, was_inserted] = active_events.insert(event_id);
        if (!was_inserted) return false;
        defer += [event_id] { active_events.erase(event_id); };
    }

    if (worker) worker();
    return true;
}
```

## Alternatives

There are alternative options for implementing an external deferred destructor.
Similar problems are discussed, for example, in https://stackoverflow.com/a/33055669 and https://stackoverflow.com/q/32432450

Here is an example of one of the short ways to solve the same problem using `std::shared_ptr` and a custom deleter.

```C++
using defer_t = std::shared_ptr<void>;
template <class Dx>
constexpr inline defer_t make_defer(Dx dx) { return defer_t(nullptr, dx); }

long file_len(const char* filename)
{
    auto file = fopen(filename, "rb");
    if (!file) return 0;

    auto _ = make_defer([file](auto) { fclose(file); });

    fseek(file, 0, SEEK_END);
    return ftell(file);
}
```

## License
MIT
