D-Pipe
======

D-Pipe is pipeline-based data processing framework for C++.
It allows an application to define custom processing elements and connect them in series.

Main features
-------------

* Cross-platform.
* Provide a way to create used-defined processing elements that can be linked together into a pipeline.
* Provide a way to allocate user-defined reference-counted data objects.
* Use zero-copy mechanism to pass data objects through the pipeline.
* Provide a set of general-purpose ready-to-use elements.
* Allow to customize the underlying allocation mechanism.

A more detailed discussion of the features can be found in [`docs/overview.md`](docs/overview.md).

Usage at a glance
-----------------

...

Building
--------

Being header-only, the library does not need to be built.

Though, we provide a [CMake](https://cmake.org/) configuration to:
* Build the tests;
* Build the documentation with [Doxygen](https://www.doxygen.nl/) and [Graphviz](https://graphviz.org/);
* Install the library and the aforementioned documentation.

Moreover, the installation procedure deploys a CMake file that lets you import the library from another CMake project by simply adding the following lines.

```
find_package(dpipe REQUIRED)
add_executable(my-app main.cpp)
target_link_libraries(my-app PRIVATE dpipe::dpipe)
```

Library dependencies may be installed with [Conan 2](https://conan.io/).

Versioning
----------

The library version numbering follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

Coding conventions
------------------

The project uses [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) as code formatter and [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/) for Git messages.

License
-------

D-Pipe is distributed under the terms of the [MIT license](LICENSE).
