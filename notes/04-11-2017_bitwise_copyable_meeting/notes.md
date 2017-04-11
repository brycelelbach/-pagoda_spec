Attending: Bryce, Bachan

What's the best way to implement a standard layout tuple?

Why is tuple not TriviallyCopyable?

What's the deal with POD types? I know we are moving away from the term in the standard, but why?

What's the distinction between is_pod and is_trivially_copyable? is_pod seems to be a superset, right?

Why did the is_aggregate trait get killed?

What's the distinction between aggregate, POD and TriviallyCopyable?

We have a problem when serializing lambdas - we can't serialize a lambda that captures-by-value an object that is bitwise serializable (e.g. we know we can memcpy it) but is NOT TriviallyCopyable. For example, std::tuple.

It seems like we are missing a trait or concept. The set of TriviallyCopyable/is_trivially_copyable types is only a subset of the set of types that are bitwise serializable.

BitwiseCopyable
TriviallyCopyable == BitwiseCopyable && <insert trivially copyable requirements>
PODClass == TriviallyCopyableClass && StandardLayoutClass

We need to understand what makes a BitwiseCopyable thing non-TriviallyCopyable

The problem is the requirement that ALL copy constructors are trivial, so we can't have any utilty constructor.

BitwiseCopyable is not solvable, compiler can't prove it. It can prove that something is TriviallyCopyable.

We want to be able to tell the compiler a type is BitwiseCopyable and have it use that knowledge when determing if a lambda is BitwiseCopyable. Even if it did this for TriviallyCopyable we would be happy because we could just specialize it.

We could create PossiblyBitwiseCopyable. Ideally it would say yes when it could easy prove something is BitwiseCopyable.

We would still need a hook.
