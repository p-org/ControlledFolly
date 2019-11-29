# Folly
[Folly](https://github.com/facebook/folly) is a open source library of C++14 components designed by Facebook with practicality and efficiency in their mind. Folly supports **Futures** as a framework for writing asynchronous code in C++ using **Future** and **Promise** 
pattern. They do support [locks](https://github.com/facebook/folly/blob/master/folly/docs/SmallLocks.md) to control the concurrency.

# Nekara
[Nekara](https://github.com/p-org/Nekara) is a language-agnostic **concurrency testing framework** for finding concurrency bugs that occur infrequently in normal executions. The way Nekara works is that, it provides a set of APIs and requires the programmers to call those APIs before apporiate statement. By doing so, Nekara takes over the scheduling of concurrent Threads and systematically explores the various interleavings of Threads.

# ControlledFolly - Overview
ControlledFolly is a wrapper on the top of Folly to test the folly's asynchronous code using **Nekara** testing framework. What does it contains?
* `NekaraCppClient`: A Cpp implemented NekaraCppClient is a thin proxy stub which make remote procedure calls (RPC) to the Nekara testing service for windows. It provides the same set of APIs as the Nekara testing service and communicates with it using HTTP calls. One can include "NekaraCppClient.h", start the client and include API calls for testing service to control the scheduling of threads.
* `Wrapper for Future, Promise and Locks`: A wrapper is provided on top of *Future*, *Promise* and *Lock* to make API calls by the wrapped code and subsequently reduces the user instrumentation.
  * `ControlledFuture` - wrapper on top of Future.
  * `ControlledPromise` - wrapper on top of Promise.
  * `ControlledMicroLock, ControlledMicroSpinLock, and ControlledPicoSpinLock` - wrapper on top of Locks. 
  
  Once can use these wrappers by including them in the headers (ControlledFuture.h, ControlledPromise.h and ControlledMicroLock.h) for testing. For production execution, use *`typedef`* to replace ControlledFuture by Future. As of now the wrappers include the basic APIs for writing asynchronous code and the complete wrapper (supporting all folly APIs) will be made public in up-coming days.
  
  
# How to Use it
* ControlledFolly is written in C++ 14 standard. Dependency: Folly.
* Just include NekaraCppClient.h, ControlledFuture.h, ControlledPromise.h and ControlledMicroLock.h (as required) into your headers.
* Instrument the APIs and do the normal execution.
