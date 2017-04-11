template <typename T>
struct global_ptr
{
    static_assert(is_bitwise_serializable_v<T>);

    // DefaultConstructible
    // Constructible and assignable from nullptr
    // CopyConstructible, CopyAssignable
    // Implicit conversion contructors

    // Returns: true if the memory address is local.
    // Note: A global_ptr with a value of nullptr is always local.
    bool is_local() noexcept;

    // LessThanComparable
    // EqualityComparable with null_ptr

    // ptrdiff_t tmp = gp - gp
    // gp = gp +/-[=] ptrdiff_t
    // increment and decrement

    // Contextually convertible to bool; meaning not null.  
};

bool is_shareable(void* p);

// Requires: p is Shareable.
// Throws: bad_global_cast if p is not Shareable.
// Returns: A global_ptr to local memory p.
template <typename T>
global_ptr<T> global_cast(T* p);

// Requires: gp.is_local() == true.
// Throws: bad_global_cast if gp.is_local() != true. 
// Returns: The local memory pointed to by gp. 
template <typename T>
T* local_cast(global_ptr<T> gp);

template <typename T, typename U>
global_ptr<T> global_reinterpret_cast(global_ptr<U> gp);

template <typename T>
global_ptr<T> global_const_cast(global_ptr<T const> gp);
template <typename T>
global_ptr<T> global_const_cast(global_ptr<T const volatile> gp);

