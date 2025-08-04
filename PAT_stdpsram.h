// Copyright (c) 2023, Pourya Afshintabar (PAT). All rights reserved.
// This project is licensed under the BSD 3-Clause License, which can be found in the LICENSE file of this repository.
//
// Project Overview:
// This code demonstrates how to use the C++ Standard Library containers such as std::vector, std::list, std::map,
// and std::string with PSRAM support on the ESP32 microcontroller. PSRAM (Pseudo Static RAM) is an external memory
// that can be used to extend the available memory on ESP32 devices, particularly helpful for memory-intensive operations.
//
// In platformio.ini, add the following build flags to use the PSRAM:
// build_flags =
//   -DBOARD_HAS_PSRAM
//   -mfix-esp32-psram-cache-issue

#ifndef PAT_STDPSRAM_H
#define PAT_STDPSRAM_H

#include <Arduino.h>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <tuple>
#include <memory>

///////////////////////////////////////////////////
// PSRAMAllocator: Custom allocator for PSRAM memory
// This allocator uses ps_malloc and free for memory allocation
// and deallocation in PSRAM.
template <typename T>
class PSRAMAllocator
{
public:
    using value_type = T;

    // Default constructor
    PSRAMAllocator() noexcept = default;

    // Copy constructor for different types
    template <typename U>
    PSRAMAllocator(const PSRAMAllocator<U> &) noexcept {}

    //     template <typename T, typename U>
    // bool operator==(const PSRAMAllocator<T> &, const PSRAMAllocator<U> &)
    // {
    //     return true;
    // }

    // template <typename T, typename U>
    // bool operator!=(const PSRAMAllocator<T> &a, const PSRAMAllocator<U> &b)
    // {
    //     return !(a == b);
    // }
    // Allocate memory for n objects of type T
    T *allocate(std::size_t n)
    {
        if (n > std::size_t(-1) / sizeof(T))
        {
            throw std::bad_alloc();
        }
        // Using ps_malloc for PSRAM allocation
        T *ptr = static_cast<T *>(ps_malloc(n * sizeof(T)));
        if (!ptr)
        {
            throw std::bad_alloc();
        }
        return ptr;
    }

    //--------------------------------
    // Deallocate memory for a single object of type T
    void deallocate(T *ptr, std::size_t) noexcept
    {
        if (ptr)
        {
            free(ptr);
        }
    }

    // Construct an object of type T at ptr with value
    void construct(T *ptr, const T &value)
    {
        new (ptr) T(value);
    }

    // template <typename... Args>
    // void construct(T *ptr, Args &&...args)
    // {
    //     new (ptr) T(std::forward<Args>(args)...);
    // }

    // Destroy an object of type T at ptr
    void destroy(T *ptr) noexcept
    {
        ptr->~T();
    }

    // Rebind allocator to another type
    template <typename U>
    struct rebind
    {
        using other = PSRAMAllocator<U>;
    };
};
///////////////////////////////////////////////////
// Wrapper for creating objects in external PSRAM
template <typename T>
class externalRAM
{
private:
    T *object;

public:
    // Constructor
    template <typename... Args>
    explicit externalRAM(Args &&...args)
    {
        object = PSRAMAllocator<T>().allocate(1);
        new (object) T(std::forward<Args>(args)...);
    }

    // Destructor
    ~externalRAM()
    {
        if (object)
        {
            object->~T();
            PSRAMAllocator<T>().deallocate(object, 1);
        }
    }
    // Provides access to the underlying object
    T &operator*() const
    {
        return *object;
    }

    // Allows accessing the object with dot (uses reference)
    T &get() const
    {
        return *object;
    }

    // Provides access to the underlying object
    T *operator->() const
    {
        return object;
    }
};
///////////////////////////////////////////////////

#include <utility>
#include <stdexcept>

template <typename Signature>
class psram_function;

template <typename R, typename... Args>
class psram_function<R(Args...)>
{
private:
    struct callable_base
    {
        virtual R invoke(Args &&...args) = 0;
        virtual callable_base *clone() const = 0;
        virtual void destroy() = 0;
        virtual ~callable_base() {}
    };

    template <typename Callable>
    struct callable_holder : callable_base
    {
        Callable c;

        explicit callable_holder(const Callable &callable) : c(callable) {}

        R invoke(Args &&...args) override
        {
            return c(std::forward<Args>(args)...);
        }

        callable_base *clone() const override
        {
            PSRAMAllocator<callable_holder> alloc;
            callable_holder *ptr = alloc.allocate(1);
            alloc.construct(ptr, *this);
            return ptr;
        }

        void destroy() override
        {
            PSRAMAllocator<callable_holder> alloc;
            alloc.destroy(this);
            alloc.deallocate(this, 1);
        }
    };

    callable_base *impl;

public:
    psram_function() noexcept : impl(nullptr) {}

    template <typename Callable>
    psram_function(const Callable &c)
    {
        using holder_t = callable_holder<Callable>;
        PSRAMAllocator<holder_t> alloc;
        impl = alloc.allocate(1);
        alloc.construct(impl, holder_t(c));
    }

    // Copy constructor
    psram_function(const psram_function &other) : impl(nullptr)
    {
        if (other.impl)
            impl = other.impl->clone();
    }

    // Copy assignment
    psram_function &operator=(const psram_function &other)
    {
        if (this != &other)
        {
            if (impl)
                impl->destroy();
            impl = other.impl ? other.impl->clone() : nullptr;
        }
        return *this;
    }

    // Destructor
    ~psram_function()
    {
        if (impl)
            impl->destroy();
    }

    R operator()(Args... args) const
    {
        if (!impl)
            throw std::bad_function_call();
        return impl->invoke(std::forward<Args>(args)...);
    }

    explicit operator bool() const noexcept
    {
        return impl != nullptr;
    }
};

///////////////////////////////////////////////////
// Namespace stdpsram: Defines containers using PSRAMAllocator
namespace stdpsram
{
    //------------------------------------------------
    // Vector with PSRAMAllocator
    template <typename T>
    using vector = std::vector<T, PSRAMAllocator<T>>;
    //------------------------------------------------
    // List with PSRAMAllocator
    template <typename T>
    using list = std::list<T, PSRAMAllocator<T>>;
    //------------------------------------------------
    // Map with PSRAMAllocator
    template <typename Key, typename Value>
    using map = std::map<Key, Value, std::less<Key>, PSRAMAllocator<std::pair<const Key, Value>>>;
    //------------------------------------------------
    // String with PSRAMAllocator
    using string = std::basic_string<char, std::char_traits<char>, PSRAMAllocator<char>>;
    //------------------------------------------------
    // Tuple (unchanged)
    template <typename... Types>
    using tuple = std::tuple<Types...>;
    //------------------------------------------------
    template <typename Signature>
    using function = psram_function<Signature>;

    template <typename Signature, typename Callable>
    function<Signature> make_function(Callable &&c)
    {
        return function<Signature>(std::forward<Callable>(c));
    }
    //------------------------------------------------
    // template <typename T>
    // struct hash {
    //     size_t operator()(const T& key) const {
    //         return std::hash<T>()(key);  // Use default hash function for key type
    //     }
    // };

    // // Custom equal_to function using PSRAMAllocator
    // template <typename T>
    // struct equal_to {
    //     bool operator()(const T& lhs, const T& rhs) const {
    //         return std::equal_to<T>()(lhs, rhs);  // Use default equal_to function for key type
    //     }
    // };
    // template <typename Key, typename Value>
    // using unordered_map = std::unordered_map<Key, Value, std::less<Key>, PSRAMAllocator<std::pair<const Key, Value>>>;

    // // Unordered Map with PSRAMAllocator
    // template <typename Key, typename Value>
    // using unordered_map = std::unordered_map<Key, Value, std::hash<Key>, std::equal_to<Key>, PSRAMAllocator<std::pair<const Key, Value>>>;

}

///////////////////////////////////////////////////
#endif // PAT_STDPSRAM_H
