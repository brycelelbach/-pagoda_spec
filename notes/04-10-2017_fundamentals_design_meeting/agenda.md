=============================================
Pagoda Fundamentals Design Meeting 04-10-2017
=============================================

Attending: Bryce Adelstein Lelbach, John Bachan 

Objectives
==========

0.) Briefly confirm we agree on the programming model of our library (this is something that can be written up afterwards).

1.) Design the fundamental features (primitive types + basic operations on those primitives) of the C++ library. The primitives are `future`s, `promise`s and `remote_ptr`. The basic operations are `future` state/value access (`is_ready`, `get`), `future` composition (`ready`, `then`, `when_all`), `remote_ptr` upcast and downcast, `put`, `get`, ...

2.) Discover and establish overarching design guidelines for the C++ library. 

3.) Time permitting, talk about (a) remote procedure calls (`rpc`) and (b) a model for the progress guarantees and memory consistency model of the fundamental features.

**Questions and guidelines for today:**

* Avoid discussion of implementation details.
* Is the design consistent with itself? With C++? With UPC? With MPI?
* What are the run-time requirements (invariants/contracts)? What are the compile-time requirements (concepts)?
* What kind of types are we dealing with?

Programming Model
=================

Futures
=======

Basic operations
----------------

* Access Value
* Poll State
* Block on State

* Composition:
    * Create Ready Future
    * Attach Continuation
    * Combine Together

What are the run-time requirements (invariants/contracts)? 
----------------------------------------------------------

What are the compile-time requirements (concepts)?
--------------------------------------------------

What kind of type are we dealing with?
--------------------------------------

Futures have value semantics; value and state are in a object whose ownership
is shared with the future's promise (the *shared state* in standard C++ terms) 

How is the value of a future accessed?
What thread safety guarantees do our futures make?
Where do our futures fit  
                                   | Thread Safe          | Not Thread Safe |
Unique     (`get` takes ownership) |                      | `std::future`   |
Non-Unique (`get` returns a view)  | `std::shared_future` |                 |

Is the design consistent with itself? With C++? With UPC? With MPI?
-------------------------------------------------------------------

Promises
========

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

Basic operations:
-----------------

* Upcast and downcast
* Pointer arithmetic
* `put` and `get`

What are the run-time requirements (invariants/contracts)? 
----------------------------------------------------------

What are the compile-time requirements (concepts)?
--------------------------------------------------

What kind of type are we dealing with?
--------------------------------------

Is the design consistent with itself? With C++? With UPC? With MPI?
-------------------------------------------------------------------

