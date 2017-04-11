template <typename... T>
struct promise
{
    // DefaultConstructible, MoveAssignable, Swappable

    future<T...> get_future();

    // Requires: get_future().ready() == false.
    // Requires: T shall be constructible from U.
    // Effects: Sets the shared objects state to ready and
    // sets the value to u...
    template <typename... U>
    void set_value(U&&... u); 

    // set_exception
};
