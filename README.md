## Folly

[Folly](https://github.com/facebook/folly) is an open source library of C++14 components designed by Facebook with practicality and efficiency in their mind. Folly provides support for writing asynchronous code in C++ using **Future** and **Promise** 
pattern. They also support [locks](https://github.com/facebook/folly/blob/master/folly/docs/SmallLocks.md) for dealing with synchronization.

## Nekara

[Nekara](https://github.com/p-org/Nekara) is a language-agnostic **concurrency testing framework** for finding concurrency bugs that occur infrequently in normal executions. Nekara provides a set of APIs and requires the programmers to call them before the appropriate statements. Subsequently, Nekara takes over the scheduling of concurrent Tasks and systematically explores the various interleavings of Threads. Its server-client architecture allows the decoupling of the testing logic and the programming interface.

This repository contains the C++ implementation of the Nekara client.

## ControlledFolly - Overview

ControlledFolly is a "mock library" that implements Folly's API using **Nekara** concurrency testing framework. It allows applications using Folly to be tested with **Nekara** in a controlled manner. It consists of the following:

* `NekaraCppClient`: NekaraCppClient is a C++ implementation of the Nekara client, which is a thin proxy stub that makes remote procedure calls (RPC) to the Nekara testing service. It provides the same set of APIs as the Nekara testing service and communicates with it using HTTP calls. One can include "NekaraCppClient.h", start the client, and include API calls to allow the testing service to control the scheduling of threads.
* Wrappers for `Future`, `Promise` and `Lock`s: A wrapper is provided on top of *Future*, *Promise* and *Lock*, which call the testing API methods under the hood and consequently reduce the effort required to instrument a test program using the low-level APIs.
    * `ControlledFuture` - wrapper on top of Future.
    * `ControlledPromise` - wrapper on top of Promise.
    * `ControlledMicroLock, ControlledMicroSpinLock, and ControlledPicoSpinLock` - wrapper on top of Locks. 
  
  One can use these wrappers by including them in the headers (`ControlledFuture.h`, `ControlledPromise.h`, and `ControlledMicroLock.h`) for testing. For production execution, use *`typedef`* to replace `ControlledFuture` by `Future`. As of now the wrappers implement the basic APIs for writing asynchronous code and the complete wrapper (supporting all Folly APIs) will be made public in the upcoming days.
  
  
## How to Use it

* ControlledFolly is written in C++ 14 standard. Dependency: Folly.
* Start the [Nekara](https://github.com/p-org/Nekara) Testing service.
* Just include `NekaraCppClient.h`, `ControlledFuture.h`, `ControlledPromise.h` and `ControlledMicroLock.h` (as required) into your headers.
* Instrument the test program using the Nekara API and execute the program.


## Example
* Consider a sample code [FollyAccEx.cpp](https://github.com/p-org/ControlledFolly/blob/master/FollyTesting/FollyAccEx/FollyAccEx.cpp) which uses `Future` and `Lock` of Folly. There are three Future objects trying to access the two shared variables `sh_t1` and `sh_t2`. It uses locks to avoid race condition during concurrent execution.

* [Test-FollyAccEx.cpp](https://github.com/p-org/ControlledFolly/blob/master/FollyTesting/FollyTesting/FollyTest.cpp) shows the instrumented testing code.
   * `Helpers::CreateTestingServices(HTTP, "localhost", 8080, "")` starts the HTTP NekaraCppClient. 
   * `_client->CreateThread()`, `_client->StartThread(id)` and `_client->EndThread(id)` are the API calls made to Testing services.

* [Wrapper-FollyAccEx.cpp](https://github.com/p-org/ControlledFolly/blob/master/FollyTesting/FollyTesting/FollyTesting.cpp) shows the functionality of wrapper code (`ControlledFuture.h`, `ControlledPromise.h`, and `ControlledMicroLock.h`) where the API calls `CreateThread`, `StartThread` and `EndThread` were made by the wrapper itself.
