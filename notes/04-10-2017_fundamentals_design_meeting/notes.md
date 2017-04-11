=============================================
Pagoda Fundamentals Design Meeting 04-10-2017
=============================================

Attending: Bryce Adelstein Lelbach, John Bachan, Brian Van Straalen

Objectives
==========

0.) Briefly confirm we agree on the programming model of our library (this is
something that can be written up afterwards).

1.) Design the fundamental features (primitive types + basic operations on
those primitives) of the C++ library. The primitives are `future`s, `promise`s
and `remote_ptr`. The basic operations are `future` state/value access
(`is_ready`, `get`), `future` composition (`ready`, `then`, `when_all`),
`remote_ptr` upcast and downcast, `put`, `get`, ...

2.) Discover and establish overarching design guidelines for the C++ library. 

We should have a second document for this.

3.) Time permitting, talk about (a) remote procedure calls (`rpc`) and/or (b) a
model for the progress guarantees and memory consistency model of the
fundamental features.

**Questions and guidelines for today:**

* Avoid discussion of implementation details.
* Is the design consistent with itself? With C++? With UPC? With MPI?
* What are the run-time requirements (invariants/contracts)? What are the compile-time requirements (concepts)?
* What kind of types are we dealing with?

Programming Model
=================

Bryce, BVS: PGAS + asynchronous operations on the global addresses.

John: I think rpc is a fundamental part too.

Memory Model/Abstract Machine
=============================

Bryce's Topology:
* Local: my process
* Shareable: parts of my local meomry that others can see.
* Kinda Local: my node (defining property is bounded latency + 1 OS)
* Global: all the memory.
* Distributed (might be different from others): an object that notionally existed on multiple nodes.

Futures
=======

```
template <typename... T>
struct future;
```

Future == value[, state[, continuation]] (actually really 3 types)

Bryce: So now we have a concept.

Big Q: One future or many futures, one concept?
Jon: I don't have multiple types.
Bryce: Didn't we talk about a separate type for ready?
Bryce: Are you using dynamic polymorphism?
Jon: Yes.
Jon:
```
future<T> // Alias for future1<>
future1<future_ready<T>, T>{}.then([=](T){ ... });
```
Bryce: Does this work for 3rd party futures.


Bryce: In HPX/C++ (Concurrency TS and Executor TS), continuations are part of the future (e.g. list of future) to avoid forcing the executor/execution context to maintain state. This conceptually breaks the value semantics of a future; the list of continuations lives in the shared state, which future is not supposed to modify (but unique_future already kinda breaks this invariant by moving out of the shared state).
Jon: In my futures, then is a constructor.

Jon: Value, not values (e.g. tuple). You can't wait for just one.

Jon: My future has value semantics.
Bryce: It's a non-owning view. Non-owning views are great!
Bryce: Jon, do you believe std::future != value semantics. How does that differ?
Bryce: You can't mutate the std::future; you can move out of a unique_future though. std::future should be valuish semantics too.

Q: Does pagoda::future/std::future/std::shared_future have value semantics?
A: Yes. 

Jon: My future is not default constructible; a default-constructed future will never be satisfied.
Bryce + BVS: You need DefaultConstructiblity for value semantivcs.

Bryce: HPX pattern:

    std::vector<std::future<T>> futures;
    for (/* stuff */)
        futures.push_back(/* ... */);

Jon: I think this breaks b/c vector needs default constructible.
Bryce: That works.

Jon: I want to pull blocking (e.g. `get`) out of the future.

<@wash> K-ballo: Is an explicit deduction guide needed to disambugite constructors?
<@wash> K-ballo: to make future unwrapping work
<@K-ballo> wash: I thought we had killed that constructor... I suppose not, since constructors are deduction guides already, how does the constructor look?
<@K-ballo> wash: actually it might not ever work, since deduction prefers copies since Kona (I believe)? in which case the "copy" guideline would always win

Should we have future<T&> support?
Bryce: Not in this round of the spec, maybe later.

Is future a value type?
Bryce: I think so.

Bryce: std::futures have no polling, is this true?
Bryce: I don't think we can ever be in the !valid() state.

Bryce: We don't have a wait either.

Bryce: Defer *_for and *_until as future work.

Bryce: With std::future, you need a specialization for void.
Bryce: For yours, is it just the zero-arg parameter pack case?
Jon: get (aka get<>) doesn't compile for that case.
Bryce: we could sfinae out the get template if sizeof...(T) is zero, and sfinae in another one that just returns void.
Jon: get doesn't wait so it doesn't make sense.

Bryce: 4x future<void> is dumb and annoying to implement.

future<void, int> f;
f.then([=](/*...*/) {} ); // What is passed to the lambda? void, int (ill formed) or just int?

Bryce: What about

future<void, int, void, char> f;
f.then([=](/*...*/) {} ); 

Bryce: Can this just be ill-formed? Or only support the future<void> case?
Bryce: No, I dislike special casing more.

Jon: tuple<void> as a model?

Open Q: Support the current std .then model (passing futures to the continuation) AND the ".next" model of passing values, or just the latter?

Open Q: constexpr and noexcept

Bryce: Instead of future ctors that are public, we might want free functions/CPs and private ctors to preserve the class invariant.

Bryce: No blocking dtor. Lifetime issues?

f.then(g); // Drop the returned future.

Jon: It decrements the ref, but in the constructor we add 1 to the ref count.

Bryce: You have both a then ctor and a then factory.

Open Q: Do we surface the ready/then/when ctors?

Bryce: I want to surface the ready ctor at least because of class template argument deduction in C++17

Jon: Attaching a continuation is building a new object.

f.then(g); // Does this modify f's shared state?

Jon: Yes in my model
Bryce: Yes in std::future

f.then(g1);
f.then(g2);

Bryce: Ah, your then ctor takes (a) future(s) and adds itself to their continuation list.

Basic operations
----------------

* Poll State
* Block on State
* Access Value

* Composition:
    * Create Ready Future [X, ctor]
    * Attach Continuation [X, ctor]
    * Combine Together    [X, ctor]

What are the run-time requirements (invariants/contracts)? 
----------------------------------------------------------

What are the compile-time requirements (concepts)?
--------------------------------------------------

Requirements on T? Unclear.

CopyConstructible, CopyAssignable?

What kind of type are we dealing with?
--------------------------------------

Futures have value semantics; value and state are in a object whose ownership
is shared with the future's promise (the *shared state* in standard C++ terms) 

How is the value of a future accessed?
What thread safety guarantees do our futures make?
Where do our futures fit  
                                   | Thread Safe          | Not Thread Safe | (aka whether or not the shared state is ref counted)
Unique     (`get` takes ownership) |                      | `std::future`   |
Non-Unique (`get` returns a view)  | `std::shared_future` | `jon::future`   |

Bryce: I would also like us to have unique but not thread safe.
Bryce: e.g. destructive get.
Bryce: I'd prefer a separate type.

Jon: I think the standard has a split because of the other axis (thread safety).

Bryce: The question is whether users or the committee are not cool with us breaking an invariant.
Bryce: The constructors modify the shared state anyways so it's cool.

Is the design consistent with itself? With C++? With UPC? With MPI?
-------------------------------------------------------------------

Promises
========

Jon: My promise is variadic.

Bryce: What does a variadic promise mean? Does it have value over promise<tuple<>>?

Bryce: future is the consumer side, and future<T...> makes consuming nicer. Does promise do the same for producing.

Bryce: Is there a equivalent to when_all for promise?

Jon: It makes the 0 and 1 case easier to implement.

Bryce: I think it should be variadic for consistency, at least.

BVS: Composite promises?

Open Q: Allocator constructible.

Bryce: std::promise: DefaultConstructible and MoveConstructible.

Bryce: Is it DefaultConstructible?

promise<T> p;
future<T> f = p.get_future(); // I have a future in the invalid state now?

Bryce: Why DefaultConstructible here but not DefaultConstructible for promise?

Jon: For future, you can't mutate it so an invalid future is just useless.

Bryce: Motivating example:

future<T> f;
// Assume whatever == false 
if (whatever) f = // ...
f.then([](){}); // This leaks in Jon's impl?
while (!f.ready()) {}
f.get();

Bryce: I don't want this to compile.

Jon: I do have a future that will never be satisfied.

Bryce: You can still get a never-satisified future by creating a promise, getting a future from it, and never setting the future from the promise.

Bryce: We can't protect against this without doing horrible things in promises destructor.

Bryce: std::terminate, lol?

Bryce: std::promise::set_value is not templated and is overloaded for const&, &&, & (promise<T&>), no args (void case only), does this get ambiguous for us in the empty pack case?

void set_value(T const&... t); 
void set_value(T&&... t); 

Jon: We could just do it by value or do it by perfect forwarding.

Bryce: I wonder why the standard doesn't do it with perfect forwarding. Sounds good.

Jon: Extension I want - a count parameter in set_value.

Bryce: E.g. one future, but multiple set_values needed to make it ready.

Bryce: This fulfills a need that the BINOCULARS people described.

Bryce: I prefer to table this for now; this functionality might be fundable w/ the NSA guys.

Jon: get_future returns the same future.

Bryce: std::promise::get_future returns by value, why? Do the futures "compare equal"?

Jon: Note that it's fine to call get_future multiple times.

What are the run-time requirements (invariants/contracts)? 
----------------------------------------------------------

What are the compile-time requirements (concepts)?
--------------------------------------------------

What kind of type are we dealing with?
--------------------------------------

Basic operations
----------------

Remote Pointers
===============

What it is: A handle to an address in the global address space.

Basic operations:
-----------------

* Upcast (T* -> global_ptr<T>) and downcast (global_ptr<T> -> T*)

Bryce: Upcast is conceptually a constructor, although that may be too implicit.

Bryce: Do we allow upcast to global_ptr on another rank?
All: No.
Bryce: I would like this, but this is fine for now.

Bryce: Do we want a explicit constructor that takes T*, OR do we want some factory_function like global_cast<>?

Jon: I like the explicitness of global_cast

Bryce: In C++17, this is would deduce badly if we had an upcast constructor (even an explicit):

global_ptr p(nullptr); // Ok: gives you a global_ptr<nullptr_t>

Bryce: explicit solves some of this, but yah. Even with explicit:

T* p;
global_ptr<T> gp = p; // Ok
global_ptr<T> gp(p); // Ok, runtime error if not Shareable

Bryce: This is still too implicit for me.

* Accessors

BVS + Jon: We need is_local

Bryce: When we were designing HPX's AGAS we determined that is_local had to be faster.

Jon: is_local never communicates for us.

Jon + BVS: It may not be O(1).

Open Q: How do we say that?

Jon: Is the null-constructed global_ptr local?

Bryce: In HPX, we decided that is_local could have false negatives but not false positives.

Jon + BVS: Do we guarantee that there is at least one rank where the global_ptr is local.

Bryce: How could it be more than one?

Jon + BVS: Shared memory, etc.

All: is_local means it is safe to load/store.

All: Yes.

BVS: I have ranks on a node sharing memory with a segment. R0 makes a global_ptr of it, R1 (sharing with R0) might resolve it to a different virtual address (but pointing to the same physical memory).

Bryce: This comes up in other shared memory cases - you can map the same shared thing twice in one rank, and then you have somehting that has two virtual addresses.

Bryce: This is something I've been working on with NVIDIA people. Tough problem.

Bryce + BVS: What does coarray Fortran do?

Bryce: Does Fortran have pointers, do they have this problem?

Bryce: Back to casts, do we require this (it places requirements on the runtime):

bool is_shareable(void* p);

BVS: Yes you can answer it, same speed (e.g. same table) as is_local.

Open Q: Can we allow false positives/negatives here?

* Comparisons

Paul + Dan: Example, I have a graph or linked list, etc.

Bryce: So you have things pointed to by global_ptrs that contain global_ptrs?

Bryce: This requires that global_ptr is_bitwise_serializable?

BVS + John: Already required.

BVS + John: For equality we need a canonical representation for addresses, or we map tables everywhere?

Bryce: Why would we want the other one.

BVS + John: The global address space allows one object to have multiple addresses.

Bryce: Interesting, HPX makes this guarantee implicitly.

Bryce: I want to guarantee a if and only if relationship (bijective), e.g. one address one object.

Bryce: Can we get it?

BVS + John: Yes.

Bryce: Then we have comparisons.

Bryce: People want to do pointer arithmetic, so we need LessThanComparable. So we need a global ordering.

* Pointer arithmetic

BVS: Do we do it in terms of downcast?

John + Bryce: No.

John + Bryce: Everything that doesn't decrement.

gp +/-[=] gp
gp +/-[=] ptrdiff_t

All those with assignment forms.

Bryce: What happens when it overflows?

Bryce: In HPX we UB in a very bad way, it never comes up. We embed the entire 64 bit address space; overflowing would overflow into bits that have other meaning (e.g. ref cnting, etc).

All : UB

BVS: Convertible to bool?

John: What does it mean? Should mean not null.

Bryce: Where is the global_ptr<T>(nullptr) in the global ordering?

John: I would hope the global null pointer is unique and less than all other pointers.

Bryce: SF

John: What about memberof?

Bryce: Just don't do this, this means you have AoS not SoA. You won't be able to vectorize. And you must do that to get perf on all the mainstream modern systems. This is not just for flops, but for int arithmetic and memory bound code too.

John: What if it's not AoS, but just S? Graph stuff, for example

Bryce: How will you vectorize that code?

John: You won't.

Bryce: They don't do any clustering?

Bryce: Even if it's not vector machine, you need to batch to achieve medium-grained parallelism. Fine-grained doesn't work today.

Open Q: How many hoops do you need to jump through to construct a global-ptr-to-member from a global-ptr.

John: Should global_ptr<void> work?

Bryce: Yes.

* `put` and `get`

John: Should these be members of global_ptr or free functions?

Bryce: get may have issues as a free function.

Bryce: I'm leaning towards member functions. 

John: Does this mean global_mdspan has these as well?

Bryce: Sure.

Bryce: For now, let's go with member functions.

Bryce: Very nice connection to the datapar SIMD type proposal.

Bryce: Is it not weird that future::get doesn't block and returns a value, but global_ptr::get doesn't block and returns an future.

John + BVS: Not weird to us, neither blocks.

BVS: We could rename future get.

Bryce: Maybe we could call it value.

template <typename T>
future<> global_ptr<T>::put(T* p)

Bryce: The future<> that is returned. When is it ready? What does it represent?

John: It means that if someone does a get on the location they will see my value and if the receiving side does a native load they will see the value.

Bryce: What event triggers?

John: Remote NIC has confirmed it was written.

Bryce: When can I write to p?

John: After the future is ready.

Bryce: It could be earlier, right? E.g. there is an earlier event, the completion of the send (e.g. source buffer is fully on the wire). This API doesn't let us observe this event.

Bryce: Relative duration between the the remote NIC confirmation and the completion of the send?

John: I believe Paul will say it is tiny.

Bryce: I don't think we need to perf optimize this.

Bryce: I can always read from it right?

John: Yes.

Bryce: If p is invalid, we get UB.

John: Should we have a version that takes it by value?

Bryce: Yes, but probably perfect forwarding too.

BVS: What about lifetimes?

Bryce: Yah ouch.

Bryce: I would like to return future<T*>.

Bryce: The reason - it's called pass-the-buck, I'd like to use it in the next step in the continuation chain.

John: My way of doing it - lambda capture.

TODO: Change to span, add shared_ptr/unique_ptr forms?

What are the run-time requirements (invariants/contracts)? 
----------------------------------------------------------

What are the compile-time requirements (concepts)?
--------------------------------------------------

What are the requirements on T?

NonTriviallySerializable := Overloads/customizes some serialization CP.

Serializable := TriviallyCopyable || NonTriviallySerializable 

T needs to be Serializable.

Jon + BVS: NonTriviallySerializable requires remote attentiveness.
Jon + BVS: So maybe not Serializable, but just TriviallyCopyable?

Bryce + Jon: is_trivially_copyable is too restrictive.

Bryce: is_aggregate (C++17 feature that failed to reach consensus)

BVS: Examples of what it restricts?

Bryce: Maybe we should have our own trait that people can customize, is_bitwise_serializable?

Bryce: is_trivially_copyable works for array<>, but not for tuple (because it's not POD).

Bryce: We could have a different type of remote pointer that allows Serializable types.

What kind of type are we dealing with?
--------------------------------------

Is the design consistent with itself? With C++? With UPC? With MPI?
-------------------------------------------------------------------


MISC

Jon + BVS: Can't capture this for RPC.
