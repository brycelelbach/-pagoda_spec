// std::future
template <typename T>
struct future;

// Jon's approach
template <typename... T>
struct future;

// Bryce likes this because it simplifies when_all
//template < class InputIt >
//auto when_all(InputIt first, InputIt last)
//    -> future<std::vector<typename std::iterator_traits<InputIt>::value_type>>;

// Jon: my thing is not as powerful as Concurrency TS.
// Bryce: We could do something like this:
template <typename T, std::size_t N>
struct future;
// Jon: We could still have dynamic when_all (returning future<vector<T>>); variadic simplifies the static case because no tuple.
// Bryce: We could special case std::array
// All: No, that doesn't make sense. 

// Why does std::future not have a make_ready_future ctor?

void f(future<int, double, double> a);
f({i, j, k}); // Can't do this; future can't be aggregate initialized.
f(make_future(i, j, k)); // C++14
f(future{i, j, k}); // C++17
void g(future<int, int, int> a);
g({i, j, k}); // Assuming i, j and k are all int.
// We'd have to have an initialization_list constructor; those are scary.

// Jon: I don't like make_future, I prefer future_make.
// Bryce: Is there precedence for?
// Bryce: Bikeshedding, later.

// Jon: Is constructor is ambiguous with future unwrapping?
// Jon + Bryce: We don't think so.

// In C++:
    // When this is true, the future's value is ready.
    bool is_ready() const;

    // Effects: Waits until is_ready() == true and then returns
    // the value of the future.
    T... get() const;

// Jon future:
    // Requires: is_ready() == true.
    // Effects: Returns the value of the future.
    T... get() const;

    // Jon: There is great value to this since it divorces futures
    // from threads.
    // Bryce: I'm sold.

// Bryce: So what does get() return? Not tuple?
// Jon: I do a template <int i = 0> Ti get()
// Bryce: This is annoying in dependent context.

future<T...> f;
f.template get<1>(f);

future<X, Y, Z> f;
auto [x, y, z] = f.get_tuple();

// Bryce:
// future<T> -> T get()
// future<T...> -> tuple<T...> get()
// And then having std::get overloaded for future.

future<X, Y, Z> f;
auto [x, y, z] = f; // Implicit std::get.
// Bryce: Is this scary or a feature?

// Bryce: safer option: std::get_tuple.
// Bryce: I don't like the inconsistency.
// Bryce: Okay, we'll go with Jon's idea + get_tuple().

// Bryce: What is the return type of get()? const&?
// Jon: That has a dangling reference problem.
// Bryce: shared_future does const&.
// Jon: So maybe we should.

// Bryce: If all the Ts are the same (e.g. std::array case),
// bracket operator would be nice.

template <typename... T>
struct future
{
    // NOT DefaultConstructible.
    // CopyConstructible, CopyAssignable, Swappable.

    // Ready constructor.
    // Requires: T shall be constructible from U.
    template <typename... U> 
    future(U&&... u);

    // Ready constructor.
    // Requires: T shall be constructible from U.
    template <typename... U>
    future& operator=(U&&... u);

    // then constructor.

    // when_* constructor.

    bool ready() const;

    // Requires: is_ready() == true.
    // Requires: Shall not participate in overload resolution
    // if I < sizeof...(T)
    // Returns: The value of the shared state.
    template <size_type I = 0>
    typename std::tuple_element<I, std::tuple<T...>>::type const&
    get() const;

    // Requires: is_ready() == true.
    // Requires: Shall not participate in overload resolution
    // if I < sizeof...(T)
    // Returns: The value of the shared state.
    // Effects: Moves the value out of the shared state; the
    // shared state is left in a valid but unspecified state.
    template <size_type I = 0>
    typename std::tuple_element<I, std::tuple<T...>>::type
    move() const;

    std::tuple<T...> const& get_tuple() const;

    // move_tuple.
};

template <typename... T>
future<T...> make_future(T&&... t)
{
    return future<T...>(std::forward<T>(t)...);
}


