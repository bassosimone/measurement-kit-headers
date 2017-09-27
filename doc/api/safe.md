# NAME

`mk/safe.hpp`

# LIBRARY

measurement-kit (`libmeasurement_kit`, `-lmeasurement_kit`)

# SYNOPSIS

```C++
#ifndef MK_SAFE_HPP
#define MK_SAFE_HPP

namespace mk {

template <typename PtrType> class Safe {
  public:
    Safe() {}

    Safe(PtrType ptr) : ptr_{std::move(ptr)} {}

    PtrType &underlying() { return ptr_; }

    auto get() const {
        if (!ptr_) {
            throw std::runtime_error("null pointer");
        }
        return ptr_.get();
    }

    auto operator-> () const { return get(); }

    auto operator*() const { return *get(); }

  private:
    PtrType ptr_;
};

} // namespace mk
#endif
```

# DESCRIPTION

The `mk/safe.hpp` file defines the `mk::Safe` smart pointer wrapper. This is widely used throughout measurement-kit to make smart pointers (both shared and unique) more robust against the case when we try to access the underlying pointer but that is, sadly, `nullptr`. 

This is part of the API because there are public objects that use it in their private implementations (defined in the public header). When using measurement-kit API, tho, it's unlikely you really need to _use_ this abstraction.

The `Safe` smart pointer wrapper can wrap both shared and unique pointers of the C++ standard library. 

Specifically, this wrapper ensures that a `std::runtime_error` is thrown if you attempt to dereference the underlying smart pointer and actually such smart pointer is empty (i.e. points to `nullptr`). 

This is a specific design choice for safety. We'd rather throw a runtime exception than dereference `nullptr`. It also simplifies debugging in that it is easier to see the stacktrace leading to the failure point. 

With this extra safety measure we are trying to defend ourself from code changing its behavior after refactoring. A particularly dangerous class of errors is the case where a smart pointer is accessed after being moved. 

This class implements checks for `get()`, `operator->()` and `operator*()` only. If you need to perform other kind of operations with the underlying smart pointer, use the `underlying()` method. 

From a design point of view, we chose to wrap the underlying pointer rather than extending it, because that seems clean and separates more clearly the concerns on the underlying pointer and of this class. 

### Methods

The default constructor initializes an empty underlying smart pointer that will throw an exception when used.

The constructor with underlying smart pointer takes in input an existing smart pointer and wraps it using this class.

The `underlying()` method allows you to access the underlying pointer.

The `get()` method returns the raw pointer wrapped by the underlying smart pointer, or throws if such raw pointer is `nullptr`.

The `operator->()` method is equivalent to `get()`.

The `operator*()` method is equivalent to `*get()`.

